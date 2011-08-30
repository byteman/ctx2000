#include <stdio.h>
#include "usbstormanager.h"

bool exportdata(std::string tc_serial, std::string path)
{
    char buf[128]={0,};
    snprintf(buf,128,"./dump.sh %s %s",tc_serial.c_str(),path.c_str());
    int ret = system(buf);
    printf("cmd = %s ret = %d\n",buf,ret);
    return !ret;
}
#include <Poco/Format.h>
class test:public IUSBStorNotify
{
public:
    bool UsbStorNotify(U_STATUS status,std::string mountDir)
    {
        if(status == U_ATTACHED)
        {
            printf("%s attch\n",mountDir.c_str ());
            USBStorage* dev= USBStorManager::get ().getUsbStorage (mountDir);
            if(dev){
                static int i = 0;
                std::string path = dev->mountDir+"/";
                if(dev->createDir ("ctx2000"))
                {
                    path = path + "ctx2000/";
                }
                printf("free size=%dk\n",dev->getFreeSpace ());
                std::string tc = Poco::format ("tc%d",i++);
                if(exportdata(tc,path) == 0)
                {
                    printf("export %d ok\n",i);
                }else{

                }

                dev->umount ();


            }
        }else if(status == U_DETACHED){
            printf("%s deattch\n",mountDir.c_str ());
        }
    }
};
int main()
{
    test t;
    USBStorManager::get ().addListener (&t);
    USBStorManager::get ().start ();

    Poco::Thread::sleep (15);

    printf("dev count=%d\n",USBStorManager::get ().checkDevice ());
    for(int i = 0; i < 3; i++)
    {
        USBStorage* dev= USBStorManager::get ().getUsbStorage (i);
        if(dev)
        {
            if(dev->createDir ("hello"))
            {
                printf("create dir ok\n");
            }
            printf("dev[%d] node=%s dir=%s\n",i,dev->devNode.c_str (),dev->mountDir.c_str ());
        }else{
            printf("dev[%d] not exist\n",i);
        }
    }

    pause();

    return 0;
}
