#include <stdio.h>
#include "gprs.h"
#include <Poco/Thread.h>
#include <stdlib.h>
bool gprs_test()
{

    return false;
}
bool exe_cmd(std::string cmd)
{
    std::string cmdout = cmd+" 1>/dev/null 2>&1";
    int ret = system(cmdout.c_str());
    fprintf(stderr,"exe %s ret=%d\n",cmd.c_str(),ret);
    return (ret!=0)?false:true;
}
int main(int argc, char *argv[])
{
    //240305002003158
    //gprs::get().start("118.123.17.25",56000,"a001");
    gprs::get().start("192.168.0.100",56000,"a001");
    while(0)
    {
    	Poco::Thread::sleep(1000);
    	exe_cmd("ifconfig eth0");
    }
    pause();
}
