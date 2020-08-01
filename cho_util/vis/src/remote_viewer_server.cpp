#include "cho_util/vis/remote_viewer_server.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <fmt/format.h>
#include <fmt/printf.h>
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "cho_util/proto/render.grpc.pb.h"
#include "cho_util/vis/convert_proto.hpp"
#include "cho_util/vis/io.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using proto::vis::render::Renderer;
using proto::vis::render::RenderReply;
using proto::vis::render::RenderRequest;

void AppendVector3f(const proto::core::geometry::Vector3f& v,
                    std::vector<float>* const data) {
  data->emplace_back(v.x());
  data->emplace_back(v.y());
  data->emplace_back(v.z());
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
    fmt::print("CREATE-RECV\n");
    // Make this instance progress to the PROCESS state.
    status_ = PROCESS;

    // As part of the initial CREATE state, we *request* that the system
    // start processing SayHello requests. In this request, "this" acts are
    // the tag uniquely identifying the request (so that different CallData
    // instances can serve different requests concurrently), in this case
    // the memory address of this CallData instance.
    service_->RequestRender(&ctx_, &request_, &responder_, cq_, cq_, this);
  } else if (status_ == PROCESS) {
    fmt::print("PROC-RECV\n");
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
      cho::type::Convert(request_, &rd);
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
}  // namespace cho
