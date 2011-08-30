#ifndef USBSTORMANAGER_H
#define USBSTORMANAGER_H
#include <string>
#include <vector>
#include <iostream>
#include "bythread.h"
enum U_STATUS
{
    U_ATTACHED, //u�̲��룬�����Զ�д����
    U_DETACHED //u�̰γ���
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
    //��u���е����ݵ���������ָ��·��
    bool exportData(std::string srcPath, std::string destPath);
    //������·�������ݵ��뵽u���е�ָ��·��
    bool importData(std::string srcPath, std::string destPath);
    //����һ���ļ���
    bool createDir(std::string path);
    bool createFile(std::string filepath);
    //���u�����Ƿ���ָ�����ļ�
    bool exist(std::string fileName);
    //ɾ���ļ�
    bool deleteFile(std::string fileName);
    //��ȡʣ������
    unsigned long long  getFreeSpace();
    //��ȡtotal
    unsigned long long  getTotalSpace();
    //ж��u��
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
    //���u�̲���Ͱγ������ز����u�̸���
    int  checkDevice();
    void addListener(IUSBStorNotify* lister);
    void deleteListener(IUSBStorNotify* lister);
    //����ָ����ʾ��u�̶���
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
