#include "mprpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

#include <functional>

void MprpcProvider::NotifyService(google::protobuf::Service* service)
{
    ServiceInfo service_info;
    service_info.m_service = service;

    // 获取服务文件描述符
    const google::protobuf::ServiceDescriptor* pServiceDesc = service->GetDescriptor();

    // 获取服务的名称
    std::string serviceName = pServiceDesc->name();

    // 获取服务对象的方法数量
    int methodCnt = pServiceDesc->method_count();

    for (int i = 0; i < methodCnt; i++)
    {
        // 获取对应方法的文件描述符
        const google::protobuf::MethodDescriptor* pMethodDesc = pServiceDesc->method(i);
        std::string methodName = pMethodDesc->name();
        service_info.m_methodMap.emplace(methodName, pMethodDesc);
        
    }
    
    m_serviceInfoMap.emplace(serviceName, service_info);
}

void MprpcProvider::Run()
{   
    auto& config = MprpcApplication::GetInstance().GetConfig();
    std::string logPath = config.GetConfigValue("logpath");
    if(!logPath.empty())
    {
        Logger::GetInstance().SetLogPath(logPath);
    }
    std::string ip = config.GetConfigValue("rpcserverip");
    uint16_t port = atoi(config.GetConfigValue("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "MprpcProvider");
    // 绑定连接回调和消息读写回调
    server.setConnectionCallback(std::bind(&MprpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&MprpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库线程数量
    server.setThreadNum(4);

    ZkClient zkCli;
    zkCli.Start();

    for(auto& sim : m_serviceInfoMap)
    {
        std::string server_path = "/" + sim.first;
        zkCli.Create(server_path.c_str(), nullptr, 0);
        for(auto& mm : sim.second.m_methodMap)
        {
            std::string method_path = server_path + "/" + mm.first;
            std::string method_data = ip + ":" + std::to_string(port);
            zkCli.Create(method_path.c_str(), method_data.c_str(),
                            method_data.size(), ZOO_EPHEMERAL);
        }
    }

    LOG_INFO("%s:%s:%d: Mprpcprovider start service at ip:%s port:%d", __FILE__, __FUNCTION__, __LINE__,  ip.c_str(), port);
    // 启动网络服务
    server.start();
    m_eventLoop.loop();
}

void MprpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}

/*
数据包括：
service_name method_name args

protobuf消息格式
header_size(4) + header(service_name + method_name + args_size(4)) + args
*/
void MprpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, 
                            muduo::net::Buffer* buffer, muduo::Timestamp)
{
    // 接收到字符流转换位string
    std::string recv_buf = buffer->retrieveAllAsString();

    // 读出头长度
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4);

    // 读取头字符
    std::string rpc_header_str = recv_buf.substr(4, header_size);

    // 反序列化
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if(rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        // 数据头反序列化失败
        LOG_ERROR("%s:%s:%d: rpc_header_str:%s parse erorr", __FILE__, __FUNCTION__, __LINE__, rpc_header_str.c_str());
        return;
    }

    // 参数字符
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    /*
    // 打印调试信息
    std::cout << "====================================================================" << std::endl;
    std::cout << "header_size:" << header_size << std::endl;
    std::cout << "rpc_header_str:" << rpc_header_str << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_size:" << args_size << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "====================================================================" << std::endl;
    */

    auto sit = m_serviceInfoMap.find(service_name);
    if(sit == m_serviceInfoMap.end())
    {
        LOG_INFO("%s:%s:%d: request %s is not exist", __FILE__, __FUNCTION__, __LINE__, service_name.c_str());
        return;
    }

    auto mit = sit->second.m_methodMap.find(method_name);
    if(mit == sit->second.m_methodMap.end())
    {
        LOG_INFO("%s:%s:%d: request %s:%s is not exist", __FILE__, __FUNCTION__, __LINE__, service_name.c_str(), method_name.c_str());
        return;        
    }

    google::protobuf::Service* service = sit->second.m_service;
    const google::protobuf::MethodDescriptor* method = mit->second;

    // 生成rpc方法调用的请求request和响应response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        LOG_ERROR("%s:%s:%d: requset parse erorr args_str: %s", __FILE__, __FUNCTION__, __LINE__, args_str.c_str());
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给method方法绑定一个回调Closure
    google::protobuf::Closure* done = google::protobuf::NewCallback<MprpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                    (this, &MprpcProvider::SendRpcResponse, conn, response);

    // 根据请求调用rpc方法
    service->CallMethod(method, nullptr, request, response, done);
}

void MprpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        // 序列化成功
        conn->send(response_str);
    }
    else
    {
        LOG_ERROR("%s:%s:%d: response_str: %s serialize erorr", __FILE__, __FUNCTION__, __LINE__, response_str.c_str());
    }
    conn->shutdown();
}