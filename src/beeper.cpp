#include "beeper.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <Poco/SingletonHolder.h>
CBeeper::CBeeper()
{
}
CBeeper& CBeeper::get()
{
    static Poco::SingletonHolder<CBeeper> sh;
    return *sh.get();
}
bool CBeeper::BeepMs(int last_ms, int interval_ms)
{
    int fd=open("/dev/tty0",O_RDWR|O_NDELAY);

    if (fd<0) {
            perror("/dev/tty0");
            return false;
    }

    /* 125 = 125 milliseconds, or 1/8th of a second;
       0x637 is the number of clock cycles in the standard kernel beep */
    if (ioctl(fd,KDMKTONE,(last_ms<<16)+interval_ms)) {
            perror("/dev/tty0: ioctl");
            return false;
    }

    close(fd);
    return true;
}
