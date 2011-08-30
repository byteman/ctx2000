#ifndef USBSTORMANAGER_H
#define USBSTORMANAGER_H
#include <string>
#include <vector>
#include <iostream>
#include "bythread.h"
enum U_STATUS
{
    U_ATTACHED, //u盘插入，并可以读写操作
    U_DETACHED //u盘拔出。
};
struct IUSBStorNotify
{
    virtual bool UsbStorNotify(U_STATUS status,std::string mountDir) = 0;
};
class USBStorage
{
public:

    USBStorage(std::string devNode, std::string mountDir);
    ~USBStorage();
    //将u盘中的数据导出到其他指定路径
    bool exportData(std::string srcPath, std::string destPath);
    //将其他路径的数据导入到u盘中的指定路径
    bool importData(std::string srcPath, std::string destPath);
    //建立一个文件夹
    bool createDir(std::string path);
    bool createFile(std::string filepath);
    //检测u盘中是否有指定的文件
    bool exist(std::string fileName);
    //删除文件
    bool deleteFile(std::string fileName);
    //获取剩余容量
    unsigned long long  getFreeSpace();
    //获取total
    unsigned long long  getTotalSpace();
    //卸载u盘
    bool umount();

    std::string getLastErrorString();
    std::string devNode;
    std::string mountDir;
    bool _exist;
private:
    bool excuteCmd(const char* cmd);
};
typedef std::vector<USBStorage*> UsbDevList;
struct TUsbNode{
    TUsbNode(std::string node,std::string dir):
        node_name(node),
        mount_dir(dir)
    {

    }
    std::string node_name;
    std::string mount_dir;
};
typedef std::vector<TUsbNode> TUsbNodeList;
class USBStorManager:public ByThread
{
public:
    static USBStorManager& get();
    USBStorManager();
    ~USBStorManager();
    virtual void service();
    //检测u盘插入和拔出，返回插入的u盘个数
    int  checkDevice();
    void addListener(IUSBStorNotify* lister);
    void deleteListener(IUSBStorNotify* lister);
    //返回指定标示的u盘对象
    USBStorage* getUsbStorage(std::string mountDir);

    USBStorage* getUsbStorage(size_t no);
private:
    int  update_devlist();
    int  parse_devlist(TUsbNodeList& devlist);
    bool findNewDevice(std::string strNode, std::string strDir);
    void invalidAllUSBDev();
    IUSBStorNotify* _notifyer;
    UsbDevList _usbDevList;
    TUsbNodeList _availNodelist;

};


#endif // USBSTORMANAGER_H
