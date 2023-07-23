#include <fstream>
#include <iostream>

#include "mprpcconfig.h"

void MprpcConfig::LoadConfigFile(const char* config_file)
{
    std::ifstream cf(config_file, std::ios::in);
    std::string line;
    if(!cf)
    {
        std::cout << "config file is not exist" << std::endl;
        exit(EXIT_FAILURE);
    }

    while(!cf.eof())
    {
        std::getline(cf, line, '\n');
        // 去掉前面多余空格
        Trim(line);

        // 判断注释
        if(line.empty() || line[0] == '#')
        {
            continue;
        }

        // 解析配置项
        int idx = line.find('=');
        if(-1 == idx)
        {
            continue;
        }

        std::string key;
        std::string value;
        key = line.substr(0, idx);
        Trim(key); 

        value = line.substr(idx + 1);
        Trim(value);
        // 去掉末尾换行符
        idx = value.find('\n');
        value = value.substr(0, idx);

        m_configMap.emplace(key, value);
    }
}

// 查询配置项信息
std::string MprpcConfig::GetConfigValue(const std::string& key)
{
    if(m_configMap.find(key) != m_configMap.end())
    {
        return m_configMap[key];
    }
    else
    {
        return "";
    }
}

void MprpcConfig::Trim(std::string &line)
{
    // 去掉前面多余空格
    int idx = line.find_first_not_of(' ');
    if(-1 != idx)
    {
        line = line.substr(idx);
    }

    // 去掉后面多余空格
    idx = line.find_last_not_of(' ');
    if(-1 != idx)
    {
        line = line.substr(0, idx + 1);
    }
}