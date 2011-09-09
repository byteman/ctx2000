#ifndef ERRORBUILDER_H
#define ERRORBUILDER_H

#include <ostream>
#include <sstream>
#include <string>

std::string _buildMsg(std::string msg,const char* file, const char* func,int len);

#define  buildMsg(msg) _buildMsg(msg,__FILE__,__FUNCTION__,__LINE__)

#ifdef DEBUG
#define ERR_DBG(fmt...) fprintf(stderr,fmt);
#else
#define ERR_DBG(fmt...) do { } while (0)
#endif


class ExceptionTracer
{
public:
    ExceptionTracer();
    std::string GetBackStrace();
private:
    std::string straceString;
};
#endif // ERRORBUILDER_H
