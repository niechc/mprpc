#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    // connect to zookeeper server
    void Start();
    void Create(const char* path, const char* data, int datalen, int state = 0);
    std::string GetData(const char* path);
private:
    zhandle_t* m_zhandle;
};
