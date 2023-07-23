#include <iostream>
#include <string>
#include "example.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"
#include "logger.h"

class ExampleService : public example::ExampleServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "do local service: Login" << std::endl;
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    void Login(google::protobuf::RpcController* controller,
                       const ::example::LoginRequest* request,
                       ::example::LoginResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        // get requset args
        std::string name = request->name();
        std::string pwd = request->pwd();

        // deal request
        bool login_result = Login(name, pwd);

        // fill results to response
        response->set_success(login_result);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");

        // callback();
        done->Run();
    }
};

int main(int argc, char** argv)
{
    MprpcApplication::Init(argc, argv);
    Logger& log = Logger::GetInstance();
    log.SetLogLevel(ERROR);
    LOG_INFO("this is a test information");
    LOG_ERROR("%s:%s:%d: this is a test error", __FILE__, __FUNCTION__, __LINE__);

    MprpcProvider rpcProvider;
    rpcProvider.NotifyService(new ExampleService);

    rpcProvider.Run();

    return 0;
}