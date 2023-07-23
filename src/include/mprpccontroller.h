#pragma once

#include <google/protobuf/service.h>
#include <string>

class MprpcController : public google::protobuf::RpcController
{
public:
    // init failed flag false and no text
    MprpcController();
    // set failed flag false and no text
    void Reset();
    // if failed return true
    bool Failed() const;
    // return failed information
    std::string ErrorText() const;
    // set failed information
    void SetFailed(const std::string& reason);

    // empty
    void StartCancel();
    // empty
    bool IsCanceled() const;
    // empty
    void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    bool m_failed;
    std::string m_errorText;
};