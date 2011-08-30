#include <stdio.h>
#include "gprs.h"
#include <Poco/Thread.h>
bool gprs_test()
{

    return false;
}
int main(int argc, char *argv[])
{
    //240305002003158
    gprs::get().start("118.123.17.25",56000,"a001");
    //gprs::get().start("192.168.0.100",56000,"a001");
    pause();
}
