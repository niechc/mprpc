#include <iostream>
#include "mprpcapplication.h"
#include "example.pb.h"


int main(int argc, char** argv)
{
    MprpcApplication::Init(argc, argv);

    example::ExampleServiceRpc_Stub stub(new MprpcChannel());

    // 请求参数
    example::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    // 响应参数
    example::LoginResponse response;

    // 创建一个controller
    MprpcController controller;

    // 发起rpc调用 => 同步调用，阻塞等待结果
    stub.Login(&controller, &request, &response, nullptr);

    if(controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        // 调用成功，读取rpc调用响应结果
        if(0 == response.result().errcode())
        {
            std::cout << "rpc login response success:" << response.success() << std::endl;
        }
        else
        {
            std::cout << "rpc login response erorr:" << response.result().errmsg() << std::endl;
        }        
    }

    return 0;
}