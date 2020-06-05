/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
using helloworld::ReadRequest;
using helloworld::ReadReply;
using helloworld::WriteRequest;
using helloworld::WriteReply;

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }

    Status ReadData(ServerContext* context,
                    const ReadRequest* request,
                    ReadReply* reply) override
    {
        std::cout << "Server received read request for " << request->numbytes() << " bytes of data" << std::endl;
        char data[request->numbytes()];
        memset(data, 68, request->numbytes());
        reply->set_numbytes(request->numbytes());
        reply->set_data(data, request->numbytes());
        std::cout << "Server is sending reply with " << request->numbytes() << " bytes of 68" << std::endl;
        return Status::OK;
    }

    Status WriteData(ServerContext* context,
                     const WriteRequest* request,
                     WriteReply* reply) override
    {
        std::cout << "Server received write request with " << request->numbytes() << " bytes of data; request->data().size() " << request->data().size() << std::endl;
        const char *data = request->data().c_str();
        std::cout << "first 4 bytes of write data received: " << data[0] << " " << data[1] << " " << data[2] << " " << data[3] << std::endl;
        for (int i = 0; i < request->numbytes(); ++i)
        {
            if (data[i] != 69)
            {
                std::cout << "ERROR: Server detected write data != 69" << std::endl;
                break;
            }
        }
        reply->set_numbytes(request->numbytes());
        std::cout << "Server sending write reply" << std::endl;
        return Status::OK;
    }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
