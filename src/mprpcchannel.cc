
#include "rpcheader.pb.h"
#include "mprpcapplication.h"
#include "zookeeperutil.h"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>


/*
header_size(4) + header(service_name + method_name + args_size(4)) + args
*/
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, const 
                    google::protobuf::Message* request,
                    google::protobuf::Message* response, 
                    google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* service = method->service();
    std::string service_name = service->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符长度args_size
    uint32_t args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request args error");
        return;
    }

    // 定义rpc请求头
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error");
        return;
    }

    // 组织消息
    std::string send_rpc_str((char*)&header_size, 4);
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    // 打印调试信息
    /*
    std::cout << "====================================================================" << std::endl;
    std::cout << "header_size:" << header_size << std::endl;
    std::cout << "rpc_header_str:" << rpc_header_str << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_size:" << args_size << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "====================================================================" << std::endl;
    */

    // 直接用socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == clientfd)
    {
        controller->SetFailed(strerror(errno));
        return;
    }

    ZkClient zkCli;
    zkCli.Start();

    std::string method_path = "/" + service_name + "/" + method_name;
    std::string method_data = zkCli.GetData(method_path.c_str());
    if(method_data.empty())
    {
        controller->SetFailed(method_path + "is not exist");
        return;
    }
    int idx = method_data.find(':');
    if(-1 == idx)
    {
        controller->SetFailed(method_path + "address is invalid");
        return;
    }
    std::string ip = method_data.substr(0, idx);
    uint16_t port = std::stoi(method_data.substr(idx + 1));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        controller->SetFailed(strerror(errno));
        close(clientfd);
        return;
    }
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        controller->SetFailed(strerror(errno));
        close(clientfd);
        return;
    }

    // 接收rpc响应
    char recv_buf[1024]{};
    int recv_size = 0;
    if(-1 == (recv_size = recv(clientfd, recv_buf, sizeof(recv_buf), 0)))
    {
        controller->SetFailed(strerror(errno));
        close(clientfd);
        return;
    }

    // 反序列化响应
    std::string response_str(recv_buf, recv_size);
    if(!response->ParseFromString(response_str))
    {
        controller->SetFailed(strerror(errno));
    }
    close(clientfd);
}