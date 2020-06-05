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

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
using helloworld::ReadRequest;
using helloworld::ReadReply;
using helloworld::WriteRequest;
using helloworld::WriteReply;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

    void ReadData(uint32_t numBytes)
    {
        std::cout << "ReadData" << std::endl;

        // Data we are sending to the server.
        ReadRequest request;
        request.set_numbytes(numBytes);

        // Container for the data we expect from the server.
        ReadReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        std::cout << "Client is sending read request for " << request.numbytes() << " bytes" << std::endl;
        // The actual RPC.
        Status status = stub_->ReadData(&context, request, &reply);
        std::cout << "Client received read reply with " << reply.numbytes() << " bytes of data; reply.data().size() " << reply.data().size() << std::endl;
        if (reply.data().size() != reply.numbytes())
        {
            std::cout << "ERROR: size != numBytes" << std::endl;
            return; //exit(1);
        }
        const uint8_t *data = (uint8_t*)reply.data().c_str();
        std::cout << "first 4 bytes of read data received: " << data[0] << " " << data[1] << " " << data[2] << " " << data[3] << std::endl;
        for (int i = 0; i < reply.numbytes(); ++i)
        {
            if (data[i] != 68)
            {
                std::cout << "ERROR: Client detected read data != 68 (data[i] " << data[i] << "i " << i << ")" << std::endl;
                return; //exit(1);
            }
        }

        // Act upon its status.
        if (status.ok()) {
            return;
        } else {
            std::cout << "ERROR: ReadData failed" << status.error_code() << ": " << status.error_message() << std::endl;
            return; //exit(1);
        }
    }

    void WriteData(uint32_t numBytes, char *data)
    {
        std::cout << "WriteData" << std::endl;
        
        // Data we are sending to the server.
        WriteRequest request;
        request.set_numbytes(numBytes);
        request.set_data(data, numBytes);

        // Container for the data we expect from the server.
        WriteReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        std::cout << "Client is sending write request with " << request.numbytes() << " bytes" << std::endl;
        // The actual RPC.
        Status status = stub_->WriteData(&context, request, &reply);
        std::cout << "Client received write reply for " << reply.numbytes() << " bytes of data" << std::endl;

        // Act upon its status.
        if (status.ok()) {
            return;
        } else {
            std::cout << "ERROR: WriteData RPC failed" << status.error_code() << ": " << status.error_message() << std::endl;
            return; //exit(1);
        }
    }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  GreeterClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

{
    auto start = std::chrono::high_resolution_clock::now();
    greeter.ReadData(4096);
    auto end = std::chrono::high_resolution_clock::now();

    double elapsedUS = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "ReadData elapsedUS " << elapsedUS << std::endl; 
}
{
    char data[4096];
    memset(data, 69, 4096);

    auto start = std::chrono::high_resolution_clock::now();
    greeter.WriteData(4096, data);
    auto end = std::chrono::high_resolution_clock::now();

    double elapsedUS = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "WriteData elapsedUS " << elapsedUS << std::endl; 
}

  return 0;
}
