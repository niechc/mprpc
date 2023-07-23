#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <string>
#include <unordered_map>


class MprpcProvider
{
public:
    // register a service
    void NotifyService(google::protobuf::Service* service);

    // start service
    void Run();
private:
    muduo::net::EventLoop m_eventLoop;

    // service information
    struct ServiceInfo
    {
        google::protobuf::Service* m_service; // service
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; // service's method map
    };

    // service information map
    std::unordered_map<std::string, ServiceInfo> m_serviceInfoMap;

    // connection callback
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    
    // message callback
    // read request and deal it
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

    // Closure callback
    // send response
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);

};