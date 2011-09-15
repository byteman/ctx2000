#include <stdio.h>
#include "gprs.h"
#include <Poco/Thread.h>
#include <stdlib.h>
#include "iniFile.h"
int main(int argc, char *argv[])
{
    TIniFile cfg("./etc/ctx2000.ini");

    std::string ip,dtuid,gps;
    int port;
    ip    = cfg.ReadString ("gprs","ip","118.123.17.25");
    port  = cfg.ReadInteger ("gprs","port",56000);
    dtuid = cfg.ReadString ("gprs","dtuid","a001");
    gps   = cfg.ReadString ("serial","gps_com","/dev/tts1");
    gprs::get().start(ip,port,dtuid,gps);
    while(1)
    {
    	Poco::Thread::sleep(100000);
    	pause();
    }
    
}
