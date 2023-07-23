## example
### description
- services provider  
  register service and its methods to zookeeper and provide service to consumers
- services consumer  
  inquire from zookeeper to get service ip and port and ask provider for service
- zookeeper  
  provide service register and inquiries  

details to see the source files [provider.cc](./provider/provider.cc), [consumer.cc](./consumer/consumer.cc), [example.proto](./example.proto)


### to begin with
1. define request response Message and RPC service, for  [example](./example.proto) 
   ```proto
   // example
   option cc_generic_services = true;
   service ExampleServiceRpc
   {
        // can define more than one rpc method in one service
        rpc Login (LoginRequest) returns (LoginResponse);
   }
   ```
2. using protoc to create it
   ```shell
   protoc example.proto --cpp_out=./
   ```
3. run zookeeper service and set the zookeeper ip and port in [configurefile](../bin/eample.conf)

### provider

1. define a service class inherits from RPC service class which created by protoc  
   ```c++
   // need these header files
   #include "example.pb.h" // you define
   #include "mprpcapplication.h"
   #include "mprpcprovider.h"
   // service class
   class ExampleService : public example::ExampleServiceRpc;
   ```
2. override the virtual RPC method
   ```c++
    // name from rpc "Login" (LoginRequest) returns (LoginResponse);
    void ExampleService::Login(google::protobuf::RpcController* controller,
                    const ::example::LoginRequest* request,
                    ::example::LoginResponse* response,
                    ::google::protobuf::Closure* done) override
    {
        // 1. get request args from request you define
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 2. call local service
        // this Login is local overload
        bool login_result = Login(name, pwd);

        // 3. fill the result to response you define
        response->set_success(login_result);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");

        // 4. call done->Run();
        done->Run();
    }
   ```
3. in main function
   ```c++
   int main(int argc, char** argv)
   {
        // 1. init
        MprpcApplication::Init(argc, argv);

        // 2. notify service
        MprpcProvider rpcProvider;
        rpcProvider.NotifyService(new ExampleService);

        // 3. Run it
        rpcProvider.Run();
   }
   ```
### consumer

1. in main function  
   ```c++
   #include "mprpcapplication.h"
   #include "example.pb.h" // you define

   int main(int argc, char** argv)
   {
        // 1. also need to init
        MprpcApplication::Init(argc, argv);
        // 2. should create a stub instance
        example::ExampleServiceRpc_Stub stub(new MprpcChannel());
        // 3. organize request message and create a response instance
        example::LoginRequest request;
        request.set_name("zhang san");
        request.set_pwd("123456");
        example::LoginResponse response;

        // 4. controller help to know RPC call if success
        MprpcController controller;

        // 5. call
        stub.Login(&controller, &request, &response, nullptr);

        // 6. if success do something
        if(!controller.Failed())
        {
            //......
        }
   }
   ```