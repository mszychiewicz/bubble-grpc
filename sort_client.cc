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
#include "bubble.grpc.pb.h"

#define NO_THREADS 4
#define MAX_VALUE 1000
#define SIZE 40000

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using bubble_grpc::SortingService;
using bubble_grpc::SortRequest;
using bubble_grpc::SortResponse;


class SortClient {
public:
    SortClient(std::shared_ptr <Channel> channel)
            : stub_(SortingService::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    void Sort(int arr[], int size) {
        // Data we are sending to the server.
        SortRequest request;
        for (int i = 0; i < size; ++i) {
            request.add_numbers(arr[i]);
        }

        // Container for the data we expect from the server.
        SortResponse response;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->Sort(&context, request, &response);

        for (int i = 0; i < size; ++i) {
            arr[i] = response.numbers(i);
        }
        // Act upon its status.
//        if (status.ok()) {
//            int arr3[size];
//            for (int i = 0; i < size; ++i) {
//                arr3[i] = response.numbers(i);
//            }
//            return arr3;
//        } else {
//            std::cout << status.error_code() << ": " << status.error_message()
//                      << std::endl;
//        }
    }

private:
    std::unique_ptr <SortingService::Stub> stub_;
};


void generate(int arr[], int length) {
    srand(256); //
    for (int i = 0; i < length; i++) {
        arr[i] = rand() % 1000;
    }
}

int findMin(int *split[], int iterators[], int numberOfThreads, int splitLength) {
    int min = MAX_VALUE + 1; // all rand numbers are non-negative
    int index = -1;
    for (int i = 0; i < numberOfThreads; i++) {
        if (split[i][iterators[i]] < min && iterators[i] < splitLength) {
            index = i;
            min = split[i][iterators[i]];
        }
    }
    return index;
}


void merge(int arr[], int **split, int length, int numberOfThreads) {
    int splitLength = ceil(length / numberOfThreads);
    int iterators[numberOfThreads];
    for (int i = 0; i < numberOfThreads; i++) // initializing iterators
        iterators[i] = 0;
    int i = 0;
    while (i < length) {
        int index = findMin(split, iterators, numberOfThreads, splitLength);
        arr[i] = split[index][iterators[index]];
        iterators[index]++;
        i++;
    }
}

void split(int arr[], int **split, int length, int numberOfThreads) {
    int splitLength = ceil(length / numberOfThreads);
    for (int i = 0; i < numberOfThreads; i++) {
        for (int j = 0; j < splitLength; j++) {
            split[i][j] = arr[j + i * splitLength];
        }
    }
}

int main(int argc, char **argv) {
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    std::string target_str;
    std::string arg_str("--target");
    if (argc > 1) {
        std::string arg_val = argv[1];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != std::string::npos) {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=') {
                target_str = arg_val.substr(start_pos + 1);
            } else {
                std::cout << "The only correct argument syntax is --target="
                          << std::endl;
                return 0;
            }
        } else {
            std::cout << "The only acceptable argument is --target=" << std::endl;
            return 0;
        }
    } else {
        target_str = "localhost:50051";
    }
    SortClient sortingService(
            grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

    //generate data
    int length = SIZE;
    int numberOfThreads = NO_THREADS;
    int array[length];
    generate(array, length);

    //start timer
    auto start = std::chrono::high_resolution_clock::now();

    //split
    int splitLength = ceil(length / numberOfThreads);
    int **splitArray = (int **) malloc(numberOfThreads * sizeof(int *));
    for (int i = 0; i < numberOfThreads; i++)
        splitArray[i] = (int *) malloc(splitLength * sizeof(int));
    split(array, splitArray, length, numberOfThreads);

    //sort
    for (int i = 0; i < numberOfThreads; i++) {
        sortingService.Sort(splitArray[i], splitLength);
    }

    //merge
    merge(array, splitArray, length, numberOfThreads);

    //end timer
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Bubble sort: " << (end - start) / std::chrono::milliseconds(1) << "ms" << std::endl;

//    for (int i = 0; i < length; ++i) {
//        std::cout << array[i] <<std::endl;
//    }

    return 0;
}
