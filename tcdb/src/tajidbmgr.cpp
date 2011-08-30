#include "tajidbmgr.h"
#include <stdio.h>
#include <Poco/Format.h>
#include <Poco/SingletonHolder.h>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/String.h>
#include <iostream>
#include "iniFile.h"

using namespace std;
using Poco::LocalDateTime;
using Poco::DateTimeFormatter;
#define DEBUG 1
CTajiDbMgr& CTajiDbMgr::Get()
{
    static Poco::SingletonHolder<CTajiDbMgr> sh;
    return *sh.get();
}
CTajiDbMgr::CTajiDbMgr():
    m_opened(false)
{
    m_local_serial = "noset";
    pDB = new CppSQLite3DB();
}
CTajiDbMgr::~CTajiDbMgr()
{
    if(pDB)
    {
        pDB->close();
        delete pDB;
    }

}
std::string CTajiDbMgr::getTableName(std::string  type,  std::string  armlen, std::string  beilv)
{
    std::string tblname= Poco::format("tbl%s_%s_%s",type,armlen,beilv);
    //Poco::UUID uuid = Poco::UUIDGenerator::defaultGenerator().create();
    //std::string tblname= Poco::format("tbl_%s",uuid.toString());
    tblname = Poco::replace(tblname,"-","_");
    //fprintf(stderr,"table name %s\n",tblname.c_str());
    return tblname;
}
bool CTajiDbMgr::deletelijuItem(std::string  type,  std::string armlen, std::string beilv)
{
    CppSQLite3Buffer strSql;
    const char* tmp = NULL;
    bool bExist = false;
    try{

        tmp = strSql.format("delete from tbltjtype where tjtype='%s' and tjarmlen='%s' and tjbeilv='%s'",type.c_str(),armlen.c_str(),beilv.c_str());
        fprintf(stderr,"tbltjtype %s \r\n", tmp);
        int rows = pDB->execDML(tmp);
        fprintf(stderr,"row=%d\n",rows);
        if (rows) {
                return true;
        }
    }catch (CppSQLite3Exception& e) {
        cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
        bExist = false;

    }
    return false;
}
bool CTajiDbMgr::addlijuItem(std::string  type,  std::string  armlen, std::string  beilv,std::string  len,std::string  weight)
{
    CppSQLite3Buffer strSql;
    const char* tmp = NULL;
    bool bExist = false;
    try{
        tmp = strSql.format("select * from tbltjtype where tjtype='%s' and tjarmlen='%s' and tjbeilv='%s'",type.c_str(),armlen.c_str(),beilv.c_str());
        fprintf(stderr,"tbltjtype %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        if(!qry.eof())
        {
            bExist = true;
        }
        qry.finalize();

    }catch (CppSQLite3Exception& e) {
        cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
        bExist = false;

    }
    bool insert_flag=false;
    if(!bExist)
    {
        try{
            tmp = strSql.format("Insert into tbltjtype values('%s','%s','%s')",type.c_str(),armlen.c_str(),beilv.c_str());
            fprintf(stderr,"%s\n",tmp);
            int rows = pDB->execDML(tmp);
            if (rows) {
                    insert_flag =true;
            }
        }catch (CppSQLite3Exception& e) {
            cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
            insert_flag = false;
        }
    }

    std::string tblname = getTableName(type,armlen,beilv);
    if(!pDB->tableExists(tblname.c_str()))
    {
        try{

            tmp = strSql.format("CREATE TABLE '%s' (len TEXT,weight TEXT)",tblname.c_str());
            pDB->execDML(tmp);
        }catch (CppSQLite3Exception& e) {
            cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
            return false;
        }
    }
    try{
        tmp = strSql.format("insert into '%s' values('%s','%s')",tblname.c_str(),len.c_str(),weight.c_str());
        fprintf(stderr,"%s\n",tmp);
        pDB->execDML(tmp);
    }catch (CppSQLite3Exception& e) {
        cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
        return false;
    }
    return true;
}
bool CTajiDbMgr::addlijuItem(std::string type,  int armlen, int beilv,double len,double weight)
{

}
bool CTajiDbMgr::CheckExistFall(std::string  type,  std::string  armlen, std::string  beilv)
{
    std::string name = getTableName(type,armlen,beilv).c_str();
    return checkExistData(name);
}
bool CTajiDbMgr::isLijuExist(std::string type,  std::string armlen, std::string beilv)
{
    return pDB->tableExists(getTableName(type,armlen,beilv).c_str());
}
bool CTajiDbMgr::getlijuItems(std::string type, std::string armlen, std::string beilv,TLijuRst& rst)
{
    bool bExist = false;
    if(isLijuExist(type,armlen,beilv)){
        CppSQLite3Buffer strSql;
        const char* tmp = NULL;
        try{
            tmp = strSql.format("select * from %s",getTableName(type,armlen,beilv).c_str());
            fprintf(stderr,"getlijuItems %s \r\n", tmp);
            CppSQLite3Query qry =  pDB->execQuery(tmp);
            while(!qry.eof())
            {
                TLiju item;
                item.armlen = qry.getStringField("len");
                item.weight = qry.getStringField("weight");
                rst.push_back(item);
                bExist = true;
                qry.nextRow();
            }
            qry.finalize();

        }catch (CppSQLite3Exception& e) {
            cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
            bExist = false;

        }
    }else{
        return false;
    }
    return bExist;
}
bool CTajiDbMgr::getlijuItems(std::string type,TLijuRst& rst)
{

}
bool CTajiDbMgr::modifylijuItem(std::string type, TLiju& item)
{

}
bool CTajiDbMgr::modifylijuItem(std::string  type,  std::string  armlen, std::string  beilv,TLiju& olditem,TLiju& newitem)
{
    std::string tblname = getTableName(type,armlen,beilv);
    if(isLijuExist(type,armlen,beilv))
    {
        try{
            CppSQLite3Buffer  strInst;
            const char* tmp = strInst.format("update  %s set len='%s', weight='%s' where len='%s' and weight='%s';", \
                            tblname.c_str(), newitem.armlen.c_str(),newitem.weight.c_str(),olditem.armlen.c_str(),olditem.weight.c_str());
            fprintf(stderr,"-=-=WetObjs.cpp=-=- %s \n", tmp);
            int rows = pDB->execDML(tmp);
            if (rows) {
                    return true;
            }
        }
        catch (CppSQLite3Exception& e) {
            cerr << e.errorCode() << ":" << e.errorMessage() << endl;
        }
        return false;
    }else{
        return true;
    }
}
bool CTajiDbMgr::DeleteFall(std::string type,  std::string armlen, std::string beilv)
{
    std::string tblname = getTableName(type,armlen,beilv);
    if(isLijuExist(type,armlen,beilv))
    {
        try{
            CppSQLite3Buffer  strInst;
            const char* tmp = strInst.format("drop table %Q",tblname.c_str());
            fprintf(stderr,"-=-=WetObjs.cpp=-=- %s \n", tmp);
            int rows = pDB->execDML(tmp);
            if (rows) {
                    return deletelijuItem(type,armlen,beilv);
            }
        }
        catch (CppSQLite3Exception& e) {
            cerr << e.errorCode() << ":" << e.errorMessage() << endl;
        }
        return false;
    }else{
        return true;
    }
}
bool CTajiDbMgr::deleteTjItem(std::string type,  std::string armlen, std::string beilv,std::string len, std::string weight)
{
    std::string tblname = getTableName(type,armlen,beilv);
    if(isLijuExist(type,armlen,beilv))
    {
        try{
            CppSQLite3Buffer  strInst;
            const char* tmp = strInst.format("delete from  %s where len='%s' and weight='%s';", \
                            tblname.c_str(), len.c_str(),weight.c_str());
            fprintf(stderr,"-=-=WetObjs.cpp=-=- %s \n", tmp);
            int rows = pDB->execDML(tmp);
            if (rows) {
                    return true;
            }
        }
        catch (CppSQLite3Exception& e) {
            cerr << e.errorCode() << ":" << e.errorMessage() << endl;
        }
        return false;
    }else{
        return true;
    }
}

bool CTajiDbMgr::getTjBeilv(std::string type, std::string armlen,TStringList& rst)
{
    bool bExist = false;
    int  index  = 0;
    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("select * from tbltjtype where tjtype='%s' and tjarmlen='%s'",type.c_str(),armlen.c_str());
        fprintf(stderr,"getTjBeilv %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            rst.push_back(qry.getStringField("tjbeilv"));
            index++;
            qry.nextRow();
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
        cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
        bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::getTjArmLen(std::string type,TStringList& rst)
{
    bool bExist = false;
    int  index  = 0;
    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("select distinct(tjarmlen) from tbltjtype where tjtype='%s'",type.c_str());
        fprintf(stderr,"tbltjtype %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            rst.push_back(qry.getStringField("tjarmlen"));
            index++;
            qry.nextRow();
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
        cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
        bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::getTjTypes(TStringList &types)
{
    bool bExist = false;
    int  index  = 0;
    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("select distinct(tjtype) from tbltjtype");
        DBG("tbltjtype %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            types.push_back(qry.getStringField("tjtype"));
            index++;
            qry.nextRow();
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
        cerr << "getTjTypes :" << e.errorCode() << ":" << e.errorMessage() << endl;
        bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::load(std::string dbpath)
{
    try{
        if(m_opened)
        {
            pDB->close();
            m_opened = false;
        }

        pDB->open(dbpath.c_str());
        m_dbpath = dbpath;
        m_opened = true;
        return true;
    }
    catch(CppSQLite3Exception& e)
    {
        fprintf(stderr,"DataBase Open Error : %s\n",e.errorMessage());
        return false;
    }
    return false;
}

bool CTajiDbMgr::checkExistData(std::string tblname)
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ",tblname.c_str());
        DBG("checkExistData %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        bExist = !qry.eof();
        qry.finalize();
    }catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
        bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::checkExistData(std::string tblname,int id)
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q where id=%d",tblname.c_str(),id);
        DBG("checkExistData %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        bExist = !qry.eof();
        qry.finalize();
    }catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
        bExist = false;
    }
    return bExist;
}

void CTajiDbMgr::GetWorkSiteParam(TWorkSiteParam &ws)
{

    TIniFile cfg("ctx2000.config");
    cfg.ReadString("worksite","leftdown","0,0");

}
bool  CTajiDbMgr::ListAlaramInfo(int start, int count, THistoyRst &rst )
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ORDER BY tjindex desc limit %d,%d","tbalarm",start,count);
        DBG("ListAlaramInfo %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            THistoy item;
            item.id      = qry.getIntField("tjindex");
            item.date    = qry.getStringField("dt");
            item.serial  = qry.getStringField("serial");
            item.slewing = qry.getStringField("dist");
            item.trolley = qry.getStringField("angle");
            rst.push_back(item);
            qry.nextRow();
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
            cerr << e.errorCode() << ":" << e.errorMessage() << endl;
            bExist = false;
    }
    return bExist;
}
bool  CTajiDbMgr::ListAlaramInfo(THistoyRst &rst )
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ","tbalarm");
        DBG("ListAlaramInfo %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            THistoy item;
            item.id      = qry.getIntField("tjindex");
            item.date    = qry.getStringField("dt");
            item.serial  = qry.getStringField("serial");
            item.slewing = qry.getStringField("dist");
            item.trolley = qry.getStringField("angle");
            rst.push_back(item);
            qry.nextRow();
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
            cerr << e.errorCode() << ":" << e.errorMessage() << endl;
            bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::ListWeightInfo(int start, int count,TWeightHistoyRst &rst)
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q order by tjindex desc limit %d,%d ","tblrecord",start,count);
        DBG("ListWeightInfo %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            TWeightHistoy item;
            item.id      = qry.getIntField("tjindex");
            item.date    = qry.getStringField("dt");
            item.serial  = qry.getStringField("serial");
            item.dist    = qry.getStringField("dist");
            item.angle   = qry.getStringField("angle");
            item.fall    = qry.getStringField("fall");
            item.weight  = qry.getStringField("weight");
            item.speed   = qry.getStringField("speed");
            item.type    = qry.getIntField("type");

            rst.push_back(item);
            qry.nextRow();
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
            cerr << e.errorCode() << ":" << e.errorMessage() << endl;
            bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::AddAlarmInfo(std::string serial,int slewing, int trolley)
{
    try{
        CppSQLite3Buffer  strInst;
        LocalDateTime now;
        std::string dt = DateTimeFormatter::format(now,"%Y-%m-%d %h:%M");


        const char* tmp = strInst.format("Insert into %Q values(NULL,'%s',%d,%d,'%s');", \
                        "tbalarm", serial.c_str(),slewing, trolley,dt.c_str());
        DBG("-=-=WetObjs.cpp=-=- %s \n", tmp);
        int rows = pDB->execDML(tmp);
        if (rows) {
                return true;
        }
    }
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
    }
    return false;
}
bool CTajiDbMgr::AddWeightInfo(TWeightHistoy& value)
{
    try{
        CppSQLite3Buffer  strInst;
        LocalDateTime now;
        std::string dt = DateTimeFormatter::format(now,"%Y-%m-%d %h:%M");


        const char* tmp = strInst.format("Insert into %Q values(NULL,'%s','%s','%s','%s','%s','%s','%s',%d);", \
                        "tblrecord",     dt.c_str(),value.dist.c_str(),value.weight.c_str(),value.fall.c_str(), \
                                         value.angle.c_str(),value.serial.c_str(),value.speed.c_str(),value.type);
        DBG("-=-=WetObjs.cpp=-=- %s \n", tmp);
        int rows = pDB->execDML(tmp);
        if (rows) {
                return true;
        }
    }
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
    }
    return false;
}
bool  CTajiDbMgr::GetAllTjId(std::vector<int> &devlist)
{
    devlist.clear();

    TDevIdMap::iterator it = m_idmap.begin();
    while(it != m_idmap.end())
    {
        devlist.push_back((*it).first);
    }
}
#include <math.h>
bool CTajiDbMgr::is_circle_interset(double x1, double y1, double x2, double y2, double r1, double r2)
{
   double len = sqrt( (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2) );

   return ( len < (r1+r2) );
}
bool  CTajiDbMgr::GetConflictTjList(std::vector<int> &devlist)
{
    for(int i = 0; i < m_tj_num; i++)
    {
        if(i == m_local_num)
            continue;
        if( is_circle_interset(m_tj[i].pos.x,m_tj[i].pos.y,m_tj[m_local_num].pos.x,m_tj[m_local_num].pos.y,m_tj[i].long_arm_length,m_tj[m_local_num].long_arm_length))
        {
            devlist.push_back(m_idmap[i]);
        }

    }
}
bool CTajiDbMgr::AddSysLog(std::string logText,int logLevel)
{
    try{
        CppSQLite3Buffer  strInst;
        LocalDateTime now;
        std::string dt = DateTimeFormatter::format(now,"%Y-%m-%d %h:%M:%S");


        const char* tmp = strInst.format("Insert into %Q values(NULL,'%s',%d,%Q);", "tblsyslog",dt.c_str(),logLevel,logText.c_str());
        fprintf(stderr,"AddSysLog %s\n",tmp);
        int rows = pDB->execDML(tmp);
        if (rows) {
                return true;
        }
    }
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
    }
    return false;
}
bool CTajiDbMgr::QuerySysLog(int start, int count,TSysLogRst& rst)
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        //const char* tmp = strSql.format("Select * from tblsyslog");
        const char* tmp = strSql.format("Select * from %Q order by logId desc limit %d,%d ","tblsyslog",start,count);
        //fprintf(stderr,"qry=%s\n",tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            TSysLog item;
            item.id      = qry.getIntField("logId");
            item.logtime = qry.getStringField("logTm");
            item.loglevel= qry.getIntField("logLevel");
            item.logtext = qry.getStringField("logText");

            rst.push_back(item);
            qry.nextRow();
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
            cerr << e.errorCode() << ":" << e.errorMessage() << endl;
            bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::DeleteSysLog(int id)
{

}
