#ifndef TAJIDBMGR_H
#define TAJIDBMGR_H
#include <vector>
#include <string>
#include <map>
#include "ctx2000.h"
#include "CppSQLite3.h"
struct THistoy{
    int         id;
    std::string serial;
    std::string date;
    std::string slewing;
    std::string trolley;
};
struct TWeightHistoy{
    int id;
    int type; //��������
    std::string serial;
    std::string date;
    std::string dist;
    std::string weight;
    std::string fall;
    std::string angle;

};
struct TLiju{
    std::string armlen;
    std::string weight;
};
typedef std::vector<TLiju>   TLijuRst;
typedef std::vector<THistoy> THistoyRst;
typedef std::vector<TWeightHistoy> TWeightHistoyRst;
typedef std::map<int, int> TDevIdMap;
typedef std::vector<std::string> TStringList;
typedef std::vector<double>    TTjLenRst;

class CTajiDbMgr
{
public:
    CTajiDbMgr();
    ~CTajiDbMgr();
    static CTajiDbMgr& Get();
    //bool addlijuType(std::string type, int armlen, int beilv);
    bool addlijuItem(std::string  type,  int armlen, int beilv,double len=0,double weight=0);
    bool deletelijuItem(std::string  type,std::string armlen, std::string beilv);
    bool addlijuItem(std::string  type,  std::string  armlen, std::string  beilv,std::string  len,std::string  weight);
    bool getlijuItems(std::string type,  std::string armlen, std::string beilv,TLijuRst& rst);
    bool getlijuItems(std::string type,TLijuRst& rst);
    bool deleteNoDataFall(std::string type,  std::string armlen, std::string beilv);
    bool isLijuExist(std::string type,  std::string armlen, std::string beilv);
    bool modifylijuItem(std::string type, TLiju& item);
    bool modifylijuItem(std::string  type,  std::string  armlen, std::string  beilv,TLiju& olditem,TLiju& newitem);
    bool getTjTypes(TStringList &types);
    bool getTjArmLen(std::string type,TStringList& rst);
    bool getTjBeilv(std::string type, std::string armlen,TStringList& rst);
    bool deleteTjItem(std::string type,  std::string armlen, std::string beilv,std::string len, std::string weight);
    std::string getTableName(std::string  type,  std::string  armlen, std::string  beilv);
    bool DeleteFall(std::string type,  std::string armlen, std::string beilv);
    //bool load(std::string dbpath);
    bool load(std::string dbpath);
    //ͨ��Զ�̹㲥�޸ģ�ͨ����������
    bool UpdateWorksite(TWorkSiteParam* par);
    //���»��������������
    bool UpdateTjParam(TTjParam* par);
    bool UpdateTjParam(int id,TTjParam* par);
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
    bool AddAlarmInfo(std::string serial,int slewing, int trolley);

    bool AddWeightInfo(TWeightHistoy& value);

    /*
      param:
        start : ��ʼ���
        count : ��ȡ����
        rst: ���صļ�¼��
      return:
        ʵ�ʶ�ȡ������
    */

    bool ListAlaramInfo(int start, int count, THistoyRst &rst );
    bool ListAlaramInfo(THistoyRst &rst);
    bool ListWeightInfo(int start, int count,TWeightHistoyRst &rst);
    void GetWorkSiteParam(TWorkSiteParam &ws);
    int  GetLocalId(){return m_local_id;}
    int  GetLocalIndex(){return m_local_num;}
    bool  GetAllTjId(std::vector<int> &devlist);
    bool  GetConflictTjList(std::vector<int> &devlist);
    bool CheckExistFall(std::string  type,  std::string  armlen, std::string  beilv);
public:
    bool is_circle_interset(double x1, double y1, double x2, double y2, double r1, double r2);
    int  get_tj_num(){return m_tj_num;}
    bool loadtjparam(QtzParam tz[], int num);
    bool loadworksite();
    bool loadtjparam();
    bool loadcalcparam();
    bool loadzoneparam();
    bool loaddivparam();
    bool AlertWorksite(TWorkSiteParam* par);
    bool AddWorksite(TWorkSiteParam* par);
    int  GetDevIndex(int id);
    bool AlertTjParam(TTjParam* par);
    bool AddTjParam(TTjParam* par);

    //bool AlertTjParam(int id, TTjParam* par,int isown);
    //bool AddTjParam(int id , TTjParam* par,int isown);

    bool AlertCalibrateParam(TCalcParam* par);
    bool AddCalibrateParam(TCalcParam* par);

    bool AlertZoneParam(int index,PolyDef par);
    bool AddZoneParam(int index,PolyDef par);

    bool AlertDividingParam(int index,PolyDef par);
    bool AddDividingParam(int index,PolyDef par);

    bool checkExistData(std::string tblname);
    bool checkExistData(std::string tblname,int id);
    TTjParam      m_tj[NUMBER_OF_QTZ];

    CppSQLite3DB* pDB;
    TDevIdMap     m_idmap;
    std::string   m_dbpath;
    bool    m_opened;
    int    m_tj_num;
    int    m_local_num;
    int    m_local_id;
    std::string m_local_serial;
};

#endif // TAJIDBMGR_H
