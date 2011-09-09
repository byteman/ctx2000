#ifndef LOGGERMGR_H
#define LOGGERMGR_H
#include "Poco/Logger.h"
#include <Poco/LogStream.h>
#include "Poco/Foundation.h"
#include "Poco/UnbufferedStreamBuf.h"
#include <istream>
#include <cstdio>
#include "ErrorBuilder.h"
using Poco::Logger;
using Poco::LogStream;
using Poco::UnbufferedStreamBuf;
using Poco::Message;
/*
根据模块名来进行调试
*/
#define LOG (CLoggerMgr::Get ())
#define Trace(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.trace(buf); \
}while(0)
#define TraceEx(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.trace(buildMsg(buf)); \
}while(0)
#define Error(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.error(buf); \
}while(0)
#define ErrorEx(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.error(buildMsg(buf)); \
}while(0)
#define Fatal(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.fatal(buf); \
}while(0)
#define FatalEx(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.fatal(buildMsg(buf)); \
}while(0)
#define Warning(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.warning(buf); \
}while(0)
#define WarningEx(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.warning(buildMsg(buf)); \
}while(0)
#define Info(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.warning(buf); \
}while(0)
#define InfoEx(fmt...) do { \
    char buf[256] = {0,}; snprintf(buf,256,fmt); \
    LOG.warning(buildMsg(buf)); \
}while(0)
class CLoggerMgr
{
public:
    CLoggerMgr();
    static CLoggerMgr& Get();
    void log(std::string msg);
    void info(std::string msg);
    void error(std::string msg);
    void warning(std::string msg);
    void fatal(std::string msg);
    void trace(std::string msg);
    void setLogger(std::string name);
    void setModuleName(std::string name);
    /*
        设置当前logger的输出级别，小于该级别的不输出
    */
    void setLevel(std::string level);
private:
    Logger *curLogger;
    std::string _moduleName;
};

#endif // LOGGERMGR_H
