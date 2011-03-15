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
    //ͨ��Զ�̹㲥�޸ģ�ͨ����������
    bool UpdateWorksite(TWorkSiteParam* par);
    //���»��������������
    bool UpdateTjParam(TTjParam* par);
    //���»������ĳ���궨����
    bool UpdateCalibrateParam(TCalcParam* par);
    //���»������ĳ���ֽ�����
    bool UpdateZoneParam(int index,PolyDef par);
    //���»������ĳ���ֽ�����
    bool UpdateDividingParam(int index,PolyDef par);
    /*
        slewing:
                2 :ͣ��ָ��
                3��ɲ��ָ��
        trolley:
                1 :ת����ָ��
                2 :ͣ��ָ��
    */
    bool AddAlarmInfo(int slewing, int trolley);
    /*
      param:
        start : ��ʼ���
        count : ��ȡ����
        rst: ���صļ�¼��
      return:
        ʵ�ʶ�ȡ������
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
