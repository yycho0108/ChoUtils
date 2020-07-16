#pragma once

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

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using render::Renderer;
using render::RenderReply;
using render::RenderRequest;

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

namespace cho {
namespace vis {

template <typename DataType>
class RenderServer : public Listener<DataType> {
  using typename Listener<DataType>::Callback;

 private:
  std::string address_;
  bool started_;
  std::function<bool(DataType&&)> on_data;

 public:
  inline void SetCallback(const Callback& on_data) { this->on_data = on_data; }
  inline bool OnData(DataType&& data) const { return on_data(std::move(data)); }
  inline bool IsRunning() const { return started_; }

 public:
  RenderServer(const std::string& address = "0.0.0.0:50051");
  ~RenderServer();

  // There is no shutdown handling in this code.
  void Start();

  void Stop();

 private:
  // Class encompasing the state and logic needed to serve a request.
  class CallData {
   private:
    const RenderServer* const parent;

   public:
    // Take in the "service" instance (in this case representing an asynchronous
    // server) and the completion queue "cq" used for asynchronous communication
    // with the gRPC runtime.
    CallData(const RenderServer* const parent, Renderer::AsyncService* service,
             ServerCompletionQueue* cq);
    void Proceed();

   private:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    Renderer::AsyncService* service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue* cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    RenderRequest request_;
    // What we send back to the client.
    RenderReply reply_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<RenderReply> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.
  };

  // This can be run in multiple threads if needed.
  void HandleRpcs();

  std::unique_ptr<ServerCompletionQueue> cq_;
  Renderer::AsyncService service_;
  std::unique_ptr<Server> server_;
};

}  // namespace vis
}  // namespace cho
