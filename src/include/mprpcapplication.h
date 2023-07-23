#pragma once
#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

class MprpcApplication
{
public:
    // initial and load configure file
    static void Init(int argc, char** argv);
    // get mprpcapplication instance
    static MprpcApplication& GetInstance();
    // get configure instance
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig m_config;
    
    MprpcApplication();
    MprpcApplication(MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};