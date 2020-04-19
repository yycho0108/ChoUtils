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

namespace cho_util {
namespace vis {

RenderClient::RenderClient(const std::string& channel)
    : stub_(Renderer::NewStub(
          grpc::CreateChannel(channel, grpc::InsecureChannelCredentials()))) {}

// Assembles the client's payload, sends it and presents the response back
// from the server.
void RenderClient::Render(const RenderData& rd) {
  // Data we are sending to the server.
  RenderRequest request;
  request.set_name(rd.tag);

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
}  // namespace cho_util
