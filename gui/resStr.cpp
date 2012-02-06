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
    "密码",
    "系统设置密码",
    "时间",
    "编号",
    "Bypass密码",
    "取消Bypass",
    "Bypass设置",
    "小车指令",
    "回转指令",
    "幅度",
    "重量",
    "倍率",
    "角度",
    "风速",
    "导出成功",
    "导出数据",
    "导出失败",
    "没有检测到U盘",
    "提示",
    "输入不正确",
    "高度",
    "参数保存成功",
    "回转AD",
    "仰角AD",
    "幅度AD",
     "重量AD",
     "高度AD",
     "回转标定,起始点记录成功",
     "回转标定成功",
};
const char** g_str = zh_str_table;
