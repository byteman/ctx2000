#include <stdio.h>
#include "gps.h"
#include <Poco/Thread.h>
int main()
{
    gps::get ().start ("/dev/ttyS1");
    //gps::get ().set_ack_time (5);
    while(1)
    {
        if(gps::get ().is_active ())
            printf("poll %s\n",gps::get ().poll_gprmc_msg ().c_str ());
        Poco::Thread::sleep (1000);
    }

    return 0;
}
