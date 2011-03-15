#ifndef TAJIDBMGR_H
#define TAJIDBMGR_H
#include <vector>
#include <string>
#include "ctx2000.h"
#include "CppSQLite3.h"
struct THistoy{
    std::string serial;
    std::string date;
    int slewing;
    int trolley;
};
typedef std::vector<THistoy> THistoyRst;
class CTajiDbMgr
{
public:
    CTajiDbMgr();
    ~CTajiDbMgr();
    static CTajiDbMgr& Get();
    bool load(std::string dbpath);
    //通过远程广播修改，通过本地设置
    bool UpdateWorksite(TWorkSiteParam* par);
    //更新或者添加塔机参数
    bool UpdateTjParam(TTjParam* par);
    //更新或者添加某个标定参数
    bool UpdateCalibrateParam(TCalcParam* par);
    //更新或者添加某个分界区域
    bool UpdateZoneParam(int index,PolyDef par);
    //更新或者添加某个分界区域
    bool UpdateDividingParam(int index,PolyDef par);
    /*
        slewing:
                2 :停车指令
                3：刹车指令
        trolley:
                1 :转低速指令
                2 :停车指令
    */
    bool AddAlarmInfo(int slewing, int trolley);
    /*
      param:
        start : 起始序号
        count : 读取条数
        rst: 返回的记录集
      return:
        实际读取的条数
    */
    int  ListAlaramInfo(int start, int count, THistoyRst &rst );

public:
    bool loadworksite();
    bool loadtjparam();
    bool loadcalcparam();
    bool loadzoneparam();
    bool loaddivparam();
    bool AlertWorksite(TWorkSiteParam* par);
    bool AddWorksite(TWorkSiteParam* par);

    bool AlertTjParam(TTjParam* par);
    bool AddTjParam(TTjParam* par);

    bool AlertCalibrateParam(TCalcParam* par);
    bool AddCalibrateParam(TCalcParam* par);

    bool AlertZoneParam(int index,PolyDef par);
    bool AddZoneParam(int index,PolyDef par);

    bool AlertDividingParam(int index,PolyDef par);
    bool AddDividingParam(int index,PolyDef par);

    bool checkExistData(std::string tblname);
    TTjParam      m_tj;
    CppSQLite3DB* pDB;
    std::string   m_dbpath;
    bool    m_opened;
};

#endif // TAJIDBMGR_H
