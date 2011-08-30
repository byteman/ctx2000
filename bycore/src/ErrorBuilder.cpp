#include "ErrorBuilder.h"

std::string _buildMsg(std::string msg,const char* file, const char* func,int len)
{
    std::ostringstream str;
    str << msg << " [in file \"" << file << "\", func " << func << "\", line "  << len << "]";
    \
    return (str.str());
    \
}


#include <execinfo.h>
#include <stdlib.h>

ExceptionTracer::ExceptionTracer()
{
    void * array[25];
    int nSize = backtrace(array, 25);
    char ** symbols = backtrace_symbols(array, nSize);
    std::ostringstream str;
    for (int i = 0; i < nSize; i++)
    {
        str << symbols[i] << "\n";
    }

    free(symbols);
    straceString = str.str ();

};
std::string ExceptionTracer::GetBackStrace()
{
    return straceString;
}
