#include "LoggerMgr.h"
#include <Poco/SingletonHolder.h>
#include <Poco/SplitterChannel.h>
#include <Poco/Util/LoggingConfigurator.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/AutoPtr.h>
#include <Poco/Format.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
using Poco::Logger;

using Poco::SingletonHolder;

using Poco::Util::LoggingConfigurator;
using Poco::Util::PropertyFileConfiguration;
using Poco::AutoPtr;
using Poco::Formatter;
using Poco::FormattingChannel;
using Poco::PatternFormatter;
/*
MyLogStreamBuf::MyLogStreamBuf(Logger& logger, Message::Priority priority):
    _logger(logger),
    _priority(priority)
{
}

MyLogStreamBuf::MyLogStreamBuf():
        _logger(Logger::get(""))
{
    //_logger=Logger::get("");
    //_logger = Logger::get("");
}
MyLogStreamBuf::~MyLogStreamBuf()
{
}


void MyLogStreamBuf::setPriority(Message::Priority priority)
{
    _priority = priority;
}


int MyLogStreamBuf::writeToDevice(char c)
{
    if (c == '\n' || c == '\r')
    {
        Message msg(_logger.name(), _message, _priority);
        _message.clear();
        _logger.log(msg);
    }
    else _message += c;
    return c;
}


//
// LogIOS
//


MyLogIOS::MyLogIOS(Logger& logger, Message::Priority priority):
    _buf(logger, priority)
{
    poco_ios_init(&_buf);
}


MyLogIOS::~MyLogIOS()
{
}


MyLogStreamBuf* MyLogIOS::rdbuf()
{
    return &_buf;
}
MyLogIOS::MyLogIOS()
{

}
*/
/*
让打印的格式化模式也可以配置
1.可以向多个通道输出
2.也可以单独向某个指定通道输出
*/

CLoggerMgr::CLoggerMgr()
{
    try
    {
        AutoPtr<PropertyFileConfiguration> pConfig = new PropertyFileConfiguration("logger.conf");

        LoggingConfigurator configurator;
        configurator.configure(pConfig);
    }
    catch(Poco::Exception& e)
    {
        fprintf(stderr,"load app.conf error %s\n",e.displayText().c_str());

    }
    curLogger = &Logger::get("logger2");
    FormattingChannel* pFC = dynamic_cast<FormattingChannel*>(curLogger->getChannel());
    /*
    timezone
    */
    //if add this , the Timestamp in truckscaler.cpp will not correct
    //static_cast<PatternFormatter*>(pFC->getFormatter())->setProperty("times","local");

}

/*
函数内部会做线程互斥
*/
void CLoggerMgr::log(std::string msg)
{
    try
    {
        curLogger->error(msg);
    }
    catch(Poco::FileAccessDeniedException &e)
    {
        fprintf(stderr,"CLoggerMgr error: %s\n",e.displayText().c_str());
    }
}
void CLoggerMgr::trace(std::string msg)
{
    try
    {
        curLogger->trace(msg);
    }
    catch(Poco::FileAccessDeniedException &e)
    {
        fprintf(stderr,"CLoggerMgr error: %s\n",e.displayText().c_str());
    }

}
void CLoggerMgr::info(std::string msg)
{
    try
    {
        curLogger->information(msg);
    }
    catch(Poco::FileAccessDeniedException &e)
    {
        fprintf(stderr,"CLoggerMgr error: %s\n",e.displayText().c_str());
    }
}
void CLoggerMgr::error(std::string msg)
{
    try
    {
        curLogger->error(msg);
    }
    catch(Poco::FileAccessDeniedException &e)
    {
        fprintf(stderr,"CLoggerMgr error: %s\n",e.displayText().c_str());
    }

}
void CLoggerMgr::warning(std::string msg)
{
    try
    {
        curLogger->warning(msg);
    }
    catch(Poco::FileAccessDeniedException &e)
    {
        fprintf(stderr,"CLoggerMgr error: %s\n",e.displayText().c_str());
    }

}
void CLoggerMgr::fatal(std::string msg)
{
    try
    {
        curLogger->fatal(msg);
    }
    catch(Poco::FileAccessDeniedException &e)
    {

    }

}
void CLoggerMgr::setLevel(std::string level)
{
    curLogger->setLevel(level);
}
/*
return instance
*/
CLoggerMgr& CLoggerMgr::Get()
{
    static SingletonHolder<CLoggerMgr> sh;
    return *sh.get();
}
void CLoggerMgr::setLogger(std::string name)
{
    if(name == "file")
    {
        curLogger = &Logger::get("logger2");
    }
    else if(name == "console")
    {
        curLogger = &Logger::get("logger2");
    }
    else
        curLogger = &Logger::get(name);
}
void CLoggerMgr::setModuleName(std::string name)
{
    _moduleName = name;
}
