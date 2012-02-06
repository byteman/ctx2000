#include "resStr.h"
#include <Poco/FileStream.h>
#include <Poco/String.h>
#include <Poco/SingletonHolder.h>
static TStringVector gStrList;
CResStr::CResStr()
{
    gStrList.clear ();
    m_size = 0;
}
CResStr& CResStr::Get()
{
    static Poco::SingletonHolder<CResStr> sh;
    return *sh.get ();
}
std::string CResStr::at(int index)
{
    if(index < gStrList.size ())
        return gStrList.at (index);
    return "xxx";
}
int CResStr::LoadRes(std::string path)
{
    Poco::FileStream fileRes;
    std::string tmpStr;
    fileRes.open (path,std::ios::in);
    while( std::getline (fileRes,tmpStr))
    {
        tmpStr = Poco::trim(tmpStr);
        if(tmpStr != "")
        {
            gStrList.push_back (tmpStr);
        }
        tmpStr = "";
    }
    m_size = gStrList.size ();
    return gStrList.size ();
}

static const char* en_str_table[] = {
    "password",
    "system setting password",
    "time",
    "No."
};
static const char* zh_str_table[] = {
    "����",
    "ϵͳ��������",
    "ʱ��",
    "���",
    "Bypass����",
    "ȡ��Bypass",
    "Bypass����",
    "С��ָ��",
    "��תָ��",
    "����",
    "����",
    "����",
    "�Ƕ�",
    "����",
    "�����ɹ�",
    "��������",
    "����ʧ��",
    "û�м�⵽U��",
    "��ʾ",
    "���벻��ȷ",
    "�߶�",
    "��������ɹ�",
    "��תAD",
    "����AD",
    "����AD",
     "����AD",
     "�߶�AD",
     "��ת�궨,��ʼ���¼�ɹ�",
     "��ת�궨�ɹ�",
};
const char** g_str = zh_str_table;
