#include "cho_util/vis/remote_viewer.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "cho_util/vis/remote_viewer_client.hpp"
#include "cho_util/vis/render_data.hpp"

#include "cho_util/proto/render.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

using render::Renderer;
using render::RenderReply;
using render::RenderRequest;

namespace cho {
namespace vis {

void Convert(const RenderData& rd, RenderRequest* const req) {
  req->set_name(rd.tag);
  req->set_type(static_cast<render::RenderType>(rd.render_type));
  req->set_representation(
      static_cast<render::RepresentationType>(rd.representation));

  // opacity 1.0 for now.
  req->set_opacity(1.0f);

  switch (rd.render_type) {
    case RenderData::RenderType::kNone: {
      req->set_type(render::RenderType::kNone);
      break;
    }
    case RenderData::RenderType::kPoints: {
      req->set_type(render::RenderType::kPoints);
      auto cloud = new PointCloud();
      for (int i = 0; i < rd.data.size(); i += 3) {
        auto v = cloud->add_points();
        v->set_x(rd.data[i + 0]);
        v->set_y(rd.data[i + 1]);
        v->set_z(rd.data[i + 2]);
      }
      req->set_allocated_cloud(cloud);
      break;
    }
    case RenderData::RenderType::kLines: {
      req->set_type(render::RenderType::kLines);
      auto lines = new Lines();
      for (int i = 0; i < rd.data.size(); i += 6) {
        auto l = lines->add_lines();
        auto start = new Vector3f();
        auto end = new Vector3f();
        start->set_x(rd.data[i + 0]);
        start->set_y(rd.data[i + 1]);
        start->set_z(rd.data[i + 2]);
        start->set_x(rd.data[i + 3]);
        start->set_y(rd.data[i + 4]);
        start->set_z(rd.data[i + 5]);
        l->set_allocated_end(end);
        l->set_allocated_start(start);
      }
      req->set_allocated_lines(lines);
      break;
    }
    case RenderData::RenderType::kPlane: {
      req->set_type(render::RenderType::kPlane);
      auto plane = new Plane();
      auto center = new Vector3f();
      auto normal = new Vector3f();
      center->set_x(rd.data[0]);
      center->set_y(rd.data[1]);
      center->set_z(rd.data[2]);
      normal->set_x(rd.data[3]);
      normal->set_y(rd.data[4]);
      normal->set_z(rd.data[5]);
      plane->set_allocated_center(center);
      plane->set_allocated_normal(normal);
      req->set_allocated_plane(plane);
      break;
    }
    case RenderData::RenderType::kSphere: {
      req->set_type(render::RenderType::kSphere);
      auto sphere = new Sphere();
      auto center = new Vector3f();
      auto normal = new Vector3f();
      center->set_x(rd.data[0]);
      center->set_y(rd.data[1]);
      center->set_z(rd.data[2]);
      sphere->set_allocated_center(center);
      sphere->set_radius(rd.data[3]);
      req->set_allocated_sphere(sphere);
      break;
    }
    case RenderData::RenderType::kBox: {
      req->set_type(render::RenderType::kCuboid);
      auto cuboid = new Cuboid();
      auto min_max = new Cuboid::MinMax();
      auto min = new Vector3f();
      auto max = new Vector3f();
      min->set_x(rd.data[0]);
      min->set_y(rd.data[1]);
      min->set_z(rd.data[2]);
      max->set_x(rd.data[3]);
      max->set_y(rd.data[4]);
      max->set_z(rd.data[5]);
      min_max->set_allocated_min(min);
      min_max->set_allocated_max(max);
      cuboid->set_allocated_min_max(min_max);
      req->set_allocated_cuboid(cuboid);
      break;
    }
    case RenderData::RenderType::kUser: {
      break;
    }
  }

  // Set Color.

  if (rd.color.size() == 3) {
    // uniform
    auto color = new Color();
    color->set_r(rd.color[0] / 255.0f);
    color->set_g(rd.color[1] / 255.0f);
    color->set_b(rd.color[2] / 255.0f);
    req->set_allocated_uniform(color);
  }
  if (rd.color.size() > 3) {
    auto colors = new Colors();
    for (int i = 0; i < rd.color.size(); i += 3) {
      auto color = colors->add_colors();
      color->set_r(rd.color[i] / 255.0f);
      color->set_g(rd.color[i + 1] / 255.0f);
      color->set_b(rd.color[i + 2] / 255.0f);
    }
    req->set_allocated_each(colors);
  }
}

RenderClient::RenderClient(const std::string& channel)
    : stub_(Renderer::NewStub(
          grpc::CreateChannel(channel, grpc::InsecureChannelCredentials()))) {}

// Assembles the client's payload, sends it and presents the response back
// from the server.
void RenderClient::Render(const RenderData& rd) {
  // Data we are sending to the server.
  RenderRequest request;
  Convert(rd, &request);

  // Container for the data we expect from the server.
  RenderReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The producer-consumer queue we use to communicate asynchronously with the
  // gRPC runtime.
  CompletionQueue cq;

  // Storage for the status of the RPC upon completion.
  Status status;

  // stub_->PrepareAsyncSayHello() creates an RPC object, returning
  // an instance to store in "call" but does not actually start the RPC
  // Because we are using the asynchronous API, we need to hold on to
  // the "call" instance in order to get updates on the ongoing RPC.
  std::unique_ptr<ClientAsyncResponseReader<RenderReply> > rpc(
      stub_->PrepareAsyncRender(&context, request, &cq));

  // StartCall initiates the RPC call
  rpc->StartCall();

  // Request that, upon completion of the RPC, "reply" be updated with the
  // server's response; "status" with the indication of whether the operation
  // was successful. Tag the request with the integer 1.
  rpc->Finish(&reply, &status, (void*)1);
  void* got_tag;
  bool ok = false;
  // Block until the next result is available in the completion queue "cq".
  // The return value of Next should always be checked. This return value
  // tells us whether there is any kind of event or the cq_ is shutting down.
  GPR_ASSERT(cq.Next(&got_tag, &ok));

  // Verify that the result from "cq" corresponds, by its tag, our previous
  // request.
  GPR_ASSERT(got_tag == (void*)1);
  // ... and that the request was completed successfully. Note that "ok"
  // corresponds solely to the request for updates introduced by Finish().
  GPR_ASSERT(ok);

#if 0
    // Act upon the status of the actual RPC.
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
#else
  return;
#endif
}

}  // namespace vis
}  // namespace cho
