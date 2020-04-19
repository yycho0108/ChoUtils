#include "cho_util/vis/remote_viewer_server.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "cho_util/vis/io.hpp"
#include "cho_util/vis/render_data.hpp"

// Generated Protobufs
#include "cho_util/proto/render.grpc.pb.h"

namespace cho_util {
namespace vis {

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using render::Renderer;
using render::RenderReply;
using render::RenderRequest;

void AppendVector3f(const Vector3f& v, std::vector<float>* const data) {
  data->emplace_back(v.x());
  data->emplace_back(v.y());
  data->emplace_back(v.z());
}

void Convert(const RenderRequest& req, RenderData* const rd) {
  rd->tag = req.name();
  rd->render_type = req.type();
  rd->representation =
      static_cast<RenderData::Representation>(req.representation());
  rd->quit = false;

  // Set Points.
  rd->data.clear();
  rd->color.clear();
  switch (req.type()) {
    case render::kNone: {
      break;
    }
    case render::kPoints: {
      rd->data.reserve(req.cloud().points_size() * 3);
      for (int i = 0; i < req.cloud().points_size(); ++i) {
        AppendVector3f(req.cloud().points(i), &rd->data);
      }
      break;
    }
    case render::kLines: {
      throw std::invalid_argument("kLines not implemented");
      break;
    }
    case render::kPlane: {
      rd->data.reserve(6);
      AppendVector3f(req.plane().center(), &rd->data);
      AppendVector3f(req.plane().normal(), &rd->data);
      break;
    }
    case render::kSphere: {
      rd->data.reserve(4);
      AppendVector3f(req.sphere().center(), &rd->data);
      rd->data.emplace_back(req.sphere().radius());
      break;
    }
    case render::kCuboid: {
      rd->data.reserve(6);
      switch (req.cuboid().bounds_case()) {
        case Cuboid::kMinMax: {
          AppendVector3f(req.cuboid().min_max().min(), &rd->data);
          AppendVector3f(req.cuboid().min_max().max(), &rd->data);
          break;
        }
        case Cuboid::kCenterRadius: {
          const auto& center = req.cuboid().center_radius().center();
          const auto& radius = req.cuboid().center_radius().radius();
          rd->data.emplace_back(center.x() - radius.x());
          rd->data.emplace_back(center.y() - radius.y());
          rd->data.emplace_back(center.z() - radius.z());
          rd->data.emplace_back(center.x() + radius.x());
          rd->data.emplace_back(center.y() + radius.y());
          rd->data.emplace_back(center.z() + radius.z());
          break;
        }
        default: {
          throw std::invalid_argument("Must be one of kMinMax/kCenterRadius");
          break;
        }
      }
    }
    default: { break; }
  }

  // Set Color.
  rd->color.clear();
  switch (req.color_case()) {
    case render::RenderRequest::kUniform: {
      rd->color.reserve(3);
      rd->color.emplace_back(req.uniform().r() * 255);
      rd->color.emplace_back(req.uniform().g() * 255);
      rd->color.emplace_back(req.uniform().b() * 255);
      break;
    }
    case render::RenderRequest::kEach: {
      rd->color.reserve(req.each().colors_size() * 3);
      for (int i = 0; i < req.each().colors_size(); ++i) {
        rd->color.emplace_back(req.each().colors(i).r() * 255);
        rd->color.emplace_back(req.each().colors(i).g() * 255);
        rd->color.emplace_back(req.each().colors(i).b() * 255);
      }
      break;
    }
    default:
      break;
  }
}

template <typename DataType>
RenderServer<DataType>::RenderServer(const std::string& address)
    : address_(address) {}

template <typename DataType>
RenderServer<DataType>::~RenderServer() {
  Stop();
}

// There is no shutdown handling in this code.
template <typename DataType>
void RenderServer<DataType>::Start() {
  std::string server_address(address_);

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service_" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *asynchronous* service.
  builder.RegisterService(&service_);
  // Get hold of the completion queue used for the asynchronous communication
  // with the gRPC runtime.
  cq_ = builder.AddCompletionQueue();
  // Finally assemble the server.
  server_ = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;

  // Proceed to the server's main loop.
  started_ = true;
  HandleRpcs();
}

template <typename DataType>
void RenderServer<DataType>::Stop() {
  server_->Shutdown();
  // Always shutdown the completion queue after the server.
  cq_->Shutdown();
}

// Class encompasing the state and logic needed to serve a request.
template <typename DataType>
RenderServer<DataType>::CallData::CallData(const RenderServer* const parent,
                                           Renderer::AsyncService* service,
                                           ServerCompletionQueue* cq)
    : parent(parent),
      service_(service),
      cq_(cq),
      responder_(&ctx_),
      status_(CREATE) {
  // Invoke the serving logic right away.
  Proceed();
}

template <typename DataType>
void RenderServer<DataType>::CallData::Proceed() {
  if (status_ == CREATE) {
    // Make this instance progress to the PROCESS state.
    status_ = PROCESS;

    // As part of the initial CREATE state, we *request* that the system
    // start processing SayHello requests. In this request, "this" acts are
    // the tag uniquely identifying the request (so that different CallData
    // instances can serve different requests concurrently), in this case
    // the memory address of this CallData instance.
    service_->RequestRender(&ctx_, &request_, &responder_, cq_, cq_, this);
  } else if (status_ == PROCESS) {
    // Spawn a new CallData instance to serve new clients while we process
    // the one for this CallData. The instance will deallocate itself as
    // part of its FINISH state.
    new CallData(parent, service_, cq_);

    // The actual processing.
    // FIXME(yycho0108): Implement this
    bool quit;
    if (0) {
      // request_.SerializeToString
      auto rd = *reinterpret_cast<DataType*>(&request_);
      quit = parent->OnData(std::move(rd));
    } else {
      RenderData rd;
      Convert(request_, &rd);
      quit = parent->OnData(std::move(rd));
    }

    const std::string prefix = (quit ? "quit" : "cont");
    reply_.set_message(prefix + request_.name());

    // And we are done! Let the gRPC runtime know we've finished, using the
    // memory address of this instance as the uniquely identifying tag for
    // the event.
    status_ = FINISH;
    responder_.Finish(reply_, Status::OK, this);
  } else {
    GPR_ASSERT(status_ == FINISH);
    // Once in the FINISH state, deallocate ourselves (CallData).
    delete this;
  }
}
// This can be run in multiple threads if needed.

template <typename DataType>
void RenderServer<DataType>::HandleRpcs() {
  // Spawn a new CallData instance to serve new clients.
  new CallData(this, &service_, cq_.get());
  void* tag;  // uniquely identifies a request.
  bool ok;
  while (true) {
    // Block waiting to read the next event from the completion queue. The
    // event is uniquely identified by its tag, which in this case is the
    // memory address of a CallData instance.
    // The return value of Next should always be checked. This return value
    // tells us whether there is any kind of event or cq_ is shutting down.
    GPR_ASSERT(cq_->Next(&tag, &ok));
    GPR_ASSERT(ok);
    static_cast<CallData*>(tag)->Proceed();
  }
}

// Explicit instantiation !
template class RenderServer<RenderData>;

}  // namespace vis
}  // namespace cho_util
