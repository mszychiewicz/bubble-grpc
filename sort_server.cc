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

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "bubble.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using bubble_grpc::SortingService;
using bubble_grpc::SortRequest;
using bubble_grpc::SortResponse;

// Logic and data behind the server's behavior.
class SortServiceImpl final : public SortingService::Service {
  Status Sort(ServerContext* context, const SortRequest* request,
                  SortResponse* response) override {
      int arr[request->numbers_size()];
      for (int i=0; i<request->numbers_size(); i++){
          arr[i] = request->numbers()[i];
      }
      bubbleSort(arr, request->numbers_size());
      for (int x : arr){
          response->add_numbers(x);
      }
//    std::copy(request->mutable_numbers(), request->mutable_numbers()+request->numbers_size(), response->numbers());
//    int arr[request->numbers_size()] = request->mutable_numbers();
//    response->add_numbers(request->numbers_size());
//    response->add_numbers(arr[1]);
//    response->add_numbers(arr[2]);
    return Status::OK;
  }
    void swap(int *x, int *y)
    {
        int temp = *x;
        *x = *y;
        *y = temp;
    }

    void bubbleSort(int toSort[], int length)
    {
        int i, j;
        for (i = 0; i < length-1; i++)

            // Last i elements are already in place
            for (j = 0; j < length-i-1; j++)
                if (toSort[j] > toSort[j+1])
                    swap(&toSort[j], &toSort[j+1]);
    }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
    SortServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
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
