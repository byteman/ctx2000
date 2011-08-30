#include "usbstormanager.h"
#include "Poco/StringTokenizer.h"
#include "Poco/FileStream.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <Poco/SingletonHolder.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <sys/vfs.h>
using Poco::StringTokenizer;
using Poco::FileInputStream;


#ifdef USB_DEBUG
#define USB_DBG(fmt...) fprintf(stderr,fmt);
#else
#define USB_DBG(fmt...) do { } while (0)
#endif

USBStorage::USBStorage(std::string devNode, std::string mountDir)
{
    this->devNode  = devNode;
    this->mountDir = mountDir;
    _exist = true;
    USB_DBG("create USBStorage\n");
}
USBStorage::~USBStorage()
{
    USB_DBG("delete USBStorage\n");
}
bool USBStorage::excuteCmd(const char* cmd)
{
    int ret = system(cmd);

    USB_DBG("excuteCmd = %s ret = %d\n",cmd,ret);
    if(ret != 0)
    {
        USB_DBG("system exec error:%s\n",strerror(errno));
        return false;
    }

    return true;
}

//将u盘中的数据导出到其他指定路径
bool USBStorage::exportData(std::string srcPath, std::string destPath)
{
    std::string usbPath = mountDir + "/" + srcPath;
    if(exist(usbPath))
    {
        char cmdBuf[256] = {0,};

        snprintf(cmdBuf,256,"cp %s %s",usbPath.c_str(),destPath.c_str());
        return excuteCmd(cmdBuf);
    }
    return false;
}
//将其他路径的数据导入到u盘中的指定路径
bool USBStorage::importData(std::string srcPath, std::string destPath)
{

    if(exist(srcPath))
    {
        char cmdBuf[256] = {0,};
        std::string usbPath = mountDir + "/" + destPath;
        snprintf(cmdBuf,256,"cp %s %s",srcPath.c_str(),usbPath.c_str());
        return excuteCmd(cmdBuf);
    }
    return false;
}

bool USBStorage::createFile(std::string filepath)
{
    return false;
}
//建立一个文件夹
bool USBStorage::createDir(std::string path)
{
    //char cmdBuf[256] = {0,};
    Poco::File tmp(mountDir+"/"+path);
    if(!tmp.exists ())
        return tmp.createDirectory ();
    return true;
    //snprintf(cmdBuf,256,"mkdir -p %s/%s",mountDir.c_str (),path.c_str ());
    //return excuteCmd(cmdBuf);
}
//检测u盘中是否有指定的文件
bool USBStorage::exist(std::string fileName)
{
    return (access(fileName.c_str(),R_OK)!=-1);
}
//删除文件
bool USBStorage::deleteFile(std::string fileName)
{
    return false;
}
unsigned long long  USBStorage::getTotalSpace()
{
    struct statfs diskInfo;
    statfs(mountDir.c_str (),&diskInfo);
    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long total = diskInfo.f_bavail*totalBlocks;
    return total>>10;
}
//获取剩余容
unsigned long long  USBStorage::getFreeSpace()
{
    struct statfs diskInfo;
    statfs(mountDir.c_str (),&diskInfo);
    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;
    return freeDisk>>10;
}
//卸载u盘
bool USBStorage::umount()
{
    char cmdBuf[256] = {0,};
    sprintf(cmdBuf,"umount %s",mountDir.c_str());
    int ret =  excuteCmd(cmdBuf); ;
    //USBStorManager::get ().checkDevice ();
    return ret;
}

USBStorManager& USBStorManager::get()
{
    static Poco::SingletonHolder<USBStorManager> sh;
    return *sh.get ();
}
void USBStorManager::service()
{
    checkDevice();
    Poco::Thread::sleep (1000);
}
USBStorManager::USBStorManager()
{
    _usbDevList.clear();
    _notifyer = NULL;

    invalidAllUSBDev();
}
USBStorManager::~USBStorManager()
{

}
bool USBStorManager::findNewDevice(std::string strNode, std::string strDir)
{
    bool bExist = false;


    for(size_t i = 0; i < _usbDevList.size(); i++)
    {
        USB_DBG("mountDir=%s,strDir=%s\n",_usbDevList.at(i)->mountDir.c_str(),strDir.c_str());
        if(_usbDevList.at(i)->mountDir == strDir && _usbDevList.at(i)->devNode == strNode)
        {
            _usbDevList.at(i)->_exist = true;
            bExist = true;
            break;
        }else{

        }
    }
    return !bExist;
}
void USBStorManager::invalidAllUSBDev()
{
    for(UsbDevList::iterator it = _usbDevList.begin(); it != _usbDevList.end(); ++it)
    {
        if(*it)
        {
            (*it)->_exist = false;
        }
    }
}
int  USBStorManager::update_devlist()
{
    std::string strLine;
    std::string tempNode,tempDir;
    size_t  num_usb = 0;
    Poco::FileInputStream mountInfo("/proc/mounts");
    _availNodelist.clear ();

    while(getline(mountInfo,strLine))
    {
        if(std::string::npos != strLine.find("/media/usb",1))
        {

            USB_DBG("%s\n",strLine.c_str ());

            StringTokenizer tokenizer(strLine, " ", StringTokenizer::TOK_TRIM);
            tempNode = tempDir = "";
            if(tokenizer.count () >= 2)
            {
                tempNode = tokenizer[0];
                tempDir  = tokenizer[1];
                if( (tempDir.length () > 0) && (tempDir.length () > 0 ) )
                {
                    TUsbNode node(tempNode,tempDir);
                    _availNodelist.push_back (node);
                    num_usb++;
                }
            }
        }

    }
    return num_usb;
}
int USBStorManager::parse_devlist(TUsbNodeList& devlist)
{
    for(size_t i = 0; i < _usbDevList.size (); i++)
    {
        _usbDevList.at (i)->_exist = false;
    }
    for(size_t i = 0; i < devlist.size (); i++)
    {
        std::string node = devlist.at (i).node_name;
        std::string dir  = devlist.at (i).mount_dir;
        if(findNewDevice(node, dir))
        {
            USB_DBG("find new usb dev node = %s dir = %s\n",node.c_str (),dir.c_str ());
            USBStorage* tmp = new USBStorage(node,dir);
            tmp->_exist     = true;
            _usbDevList.push_back(tmp);
            if(_notifyer)
                _notifyer->UsbStorNotify(U_ATTACHED,dir);
        }else{ //exist device

        }

    }
    if(devlist.size () != _usbDevList.size ())
    {
        for(UsbDevList::iterator it = _usbDevList.begin(); it != _usbDevList.end(); ++it)
        {
            if( ((*it)->_exist == false) )
            {
                if(_notifyer)
                    _notifyer->UsbStorNotify(U_DETACHED,(*it)->mountDir);
                delete (*it);
                --(it = _usbDevList.erase(it));
                USB_DBG("erase usbdev last size = %d\n",_usbDevList.size());
            }
        }
    }
    return devlist.size ();
}
int USBStorManager::checkDevice()
{
    update_devlist ();
    return parse_devlist (_availNodelist);
}
void USBStorManager::addListener(IUSBStorNotify* lister)
{
    _notifyer = lister;
}
void USBStorManager::deleteListener(IUSBStorNotify* lister)
{
    _notifyer = NULL;
}
//返回指定标示的u盘对象
USBStorage* USBStorManager::getUsbStorage(std::string mountDir)
{
    for(UsbDevList::iterator it = _usbDevList.begin(); it != _usbDevList.end(); ++it)
    {
        if(*it)
        {
            if((*it)->mountDir == mountDir)
                return *it;
        }
    }
    return NULL;
}
USBStorage* USBStorManager::getUsbStorage(size_t no)
{
    if( _usbDevList.size () == 0 ) return NULL;
// 0 1
    if(no  < _usbDevList.size ())
    {
        return _usbDevList.at (no);
    }
    return NULL;
}
