#pragma once
#include <unordered_map>
#include <string>

// rpcserverip rpcserverport zookeeperip zookeeperport
// 用于读取配置文件
class MprpcConfig
{
public:
    // load configure file, it can be done by mprpcapplication::init
    void LoadConfigFile(const char* config_file);

    // get configure value by key
    std::string GetConfigValue(const std::string& key);

private:
    std::unordered_map<std::string, std::string> m_configMap;

    void Trim(std::string &line);
};