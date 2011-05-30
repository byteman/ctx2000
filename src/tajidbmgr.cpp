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
#include <yatengine.h>
#include "iniFile.h"
using namespace TelEngine;
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
    fprintf(stderr,"table name %s\n",tblname.c_str());
    return tblname;
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
/*
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
        if(!loadworksite())
        {
            fprintf(stderr,"loadworksite failed\n");
            return false;
        }
        loadtjparam();
        loadcalcparam();
        loadzoneparam();
        loaddivparam();
        m_opened = true;
    }
    catch(CppSQLite3Exception& e)
    {
        fprintf(stderr,"DataBase Open Error : %s\n",e.errorMessage());
        return false;
    }


}

bool CTajiDbMgr::loadworksite()
{
    bool bExist = false;
    int  index  = 0;
    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("select * from tblworksite");
        DBG("loadworksite %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            m_tj[index].ws.alert_dist    = qry.getFloatField("alert_dist");
            m_tj[index].ws.brake_dist    = qry.getFloatField("brake_dist");
            m_tj[index].ws.slowdown_dist = qry.getFloatField("slow_dist");
            m_tj[index].ws.car_high_spd_dist = qry.getFloatField("car_high_spd_dist");
            m_tj[index].ws.car_low_spd_dist  = qry.getFloatField("car_low_spd_dist");
            m_tj[index].ws.inertia_angle     = qry.getFloatField("angle");
            m_tj[index].ws.y_angle           = qry.getFloatField("y_angle");
    //工地坐标
            fprintf(stderr,"worksite_coord = %d\n",m_tj[index].ws.worksite_coord.size());
            for(int i = 0; i < 4; i++)
            {

                std::string x_field = Poco::format("x%d",i);
                std::string y_field = Poco::format("y%d",i);
                double x = qry.getFloatField(x_field.c_str());
                double y = qry.getFloatField(y_field.c_str());
                fprintf(stderr,"x=%0.2f,y=%0.2f\n",x,y);
                Coord pos(x,y);
                //pos.x = x;
                //pos.y = y;

                m_tj[index].ws.worksite_coord[i] = pos;

                fprintf(stderr,"x=%0.2f,y=%0.2f\n",m_tj[index].ws.worksite_coord.at(i).x,m_tj[index].ws.worksite_coord.at(i).y);
            }
    //回转标定开始和结束点的坐标
            for(int i = 0; i < 2; i++)
            {
                std::string x_field = Poco::format("calc_x%d",i);
                std::string y_field = Poco::format("calc_y%d",i);
                double x = qry.getFloatField(x_field.c_str());
                double y = qry.getFloatField(y_field.c_str());
                Coord pos(x,y);
                m_tj[index].ws.anchor_coord[i] = pos;
            }
            bExist  = true;
            index++;
            qry.nextRow();
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
            cerr << "loadworksite :" << e.errorCode() << ":" << e.errorMessage() << endl;
            bExist = false;
    }
    return bExist;
}
int  CTajiDbMgr::GetDevIndex(int id)
{
    //TDevIdMap::const_iterator it = m_idmap.begin();
    if(m_idmap.find(id) != m_idmap.end())
    {
        return m_idmap[id];
    }
    return -1;
}
bool CTajiDbMgr::loadtjparam(QtzParam tz[], int num)
{
    bool bExist = false;
    m_tj_num    = 0;
    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ","tbltajiparam");
        DBG("loadtjparam %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {

            int tj_index = m_tj_num;
            if(tj_index < num)
            {

                tz[tj_index].number   = qry.getIntField("id");
                tz[tj_index].pos.x    = qry.getFloatField("x_pos");
                tz[tj_index].pos.y    = qry.getFloatField("y_pos");
                tz[tj_index].height   = qry.getFloatField("height");
                tz[tj_index].long_arm_len  = qry.getFloatField("long_arm_len");
                tz[tj_index].short_arm_len = qry.getFloatField("short_arm_len");
            }
            m_idmap[tz[tj_index].number] = tj_index;
            m_tj_num++;
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
bool CTajiDbMgr::loadtjparam()
{
    bool bExist = false;
    int  index  = 0;
    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ","tbltajiparam");
        DBG("loadtjparam %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            m_tj[index].taji_id          = qry.getIntField("id");
            m_tj[index].devserail        = qry.getStringField("serail");
            m_tj[index].pos.x            = qry.getFloatField("x_pos");
            m_tj[index].pos.y            = qry.getFloatField("y_pos");
            m_tj[index].taji_type        = qry.getIntField("type");
            m_tj[index].height           = qry.getFloatField("height");
            m_tj[index].max_up_angle     = qry.getFloatField("max_up_angle");
            m_tj[index].min_up_angle     = qry.getFloatField("min_up_angle");
            m_tj[index].long_arm_length  = qry.getFloatField("long_arm_dist");
            m_tj[index].min_car_dist     = qry.getFloatField("min_car_dist");
            m_tj[index].short_arm_length = qry.getFloatField("short_arm_len");
            m_tj[index].a0               = qry.getFloatField("jiegou");
            m_tj[index].isown            = qry.getIntField("isown");
            if(m_tj[index].isown != 0)
            {
                m_local_num = index;
            }
            index++;
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
bool CTajiDbMgr::loadcalcparam()
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ","tblcalc");
        DBG("loadcalcparam %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        if(!qry.eof())
        {
            m_tj[m_local_num].calc.anchor_zero_ad   = qry.getIntField("angle_zero_ad");
            m_tj[m_local_num].calc.anchor_k         = qry.getFloatField("angle_calc_k");
            m_tj[m_local_num].calc.min_car_dist_ad  = qry.getIntField("car_zero_dist_ad");
            m_tj[m_local_num].calc.min_car_dist_k   = qry.getFloatField("car_calc_k");

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
bool CTajiDbMgr::loadzoneparam()
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ","tblprotarea");
        DBG("loadcalcparam %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        m_tj[m_local_num].zone.clear();
        while(!qry.eof())
        {
            PolyDef poly;
            poly.clear();
            Coord pos;
            for(int i = 0; i < 6; i++)
            {
                std::string x_field = Poco::format("x%d",i);
                std::string y_field = Poco::format("y%d",i);
                pos.x = qry.getFloatField(x_field.c_str());
                pos.y = qry.getFloatField(y_field.c_str());
                poly.push_back(pos);
            }
            m_tj[m_local_num].zone.push_back(poly);
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
bool CTajiDbMgr::loaddivparam()
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ","tbldividarea");
        DBG("loadcalcparam %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        while(!qry.eof())
        {
            PolyDef poly;
            poly.clear();
            Coord pos;
            for(int i = 0; i < 6; i++)
            {
                std::string x_field = Poco::format("x%d",i);
                std::string y_field = Poco::format("y%d",i);
                pos.x = qry.getFloatField(x_field.c_str());
                pos.y = qry.getFloatField(y_field.c_str());
                poly.push_back(pos);
            }
            m_tj[m_local_num].div.push_back(poly);
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
*/
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
/*
bool CTajiDbMgr::AlertWorksite(TWorkSiteParam* par)
{
    try{
        CppSQLite3Buffer  strModify;
        const char* tmp = strModify.format("update %Q set x0=%f, y0=%f, x1=%f,y1=%f, \
                                            x2=%f, y2=%f, x3=%f, y3=%f, y_angle=%f, \
                                            alert_dist=%f, slow_dist=%f, brake_dist=%f, \
                                            car_high_spd_dist=%f,car_low_spd_dist=%f,calc_x0=%f, calc_y0=%f, \
                                            calc_x1=%f, calc_y1=%f,angle=%f where 1=1;", \
                        "tblworksite", par->worksite_coord.at(0).x, par->worksite_coord.at(0).y, \
                        par->worksite_coord.at(1).x, par->worksite_coord.at(1).y, \
                        par->worksite_coord.at(2).x, par->worksite_coord.at(2).y, \
                        par->worksite_coord.at(3).x, par->worksite_coord.at(3).y, \
                        par->y_angle,par->alert_dist,par->slowdown_dist,par->brake_dist, \
                        par->car_high_spd_dist,par->car_low_spd_dist,par->anchor_coord.at(0).x,par->anchor_coord.at(0).y, \
                        par->anchor_coord.at(1).x,par->anchor_coord.at(1).y, par->inertia_angle);
        DBG("-=-=WetObjs.cpp=-=- %s \n", tmp);
        int rows = pDB->execDML(tmp);
        if (rows) {
                m_tj[m_local_num].set_worksite_param(par);
                return true;
        }
    }
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
    }
    return false;
}
bool CTajiDbMgr::AddWorksite(TWorkSiteParam* par)
{
    try{
        CppSQLite3Buffer  strInst;
        const char* tmp = strInst.format("Insert into %Q values(%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f);", \
              "tblworksite",par->alert_dist,par->brake_dist,par->slowdown_dist,\
              par->car_high_spd_dist,par->car_low_spd_dist,\
              par->inertia_angle,par->y_angle,\
              par->anchor_coord.at(0).x,  par->anchor_coord.at(0).y, \
              par->anchor_coord.at(1).x,  par->anchor_coord.at(1).y, \
              par->worksite_coord.at(0).x, par->worksite_coord.at(0).y, \
              par->worksite_coord.at(1).x, par->worksite_coord.at(1).y, \
              par->worksite_coord.at(2).x,par->worksite_coord.at(2).y, \
              par->worksite_coord.at(3).x,par->worksite_coord.at(3).y);
        DBG("-=-=WetObjs.cpp=-=- %s \r\n", tmp);
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
bool CTajiDbMgr::UpdateWorksite(TWorkSiteParam* par)
{
    return checkExistData("tblworksite")?AlertWorksite(par):AddWorksite(par);
}
bool CTajiDbMgr::UpdateTjParam(TTjParam* par)
{
    return checkExistData("tbltajiparam")?AlertTjParam(par):AddTjParam(par);
}
bool CTajiDbMgr::UpdateTjParam(int id,TTjParam* par)
{
    return checkExistData("tbltajiparam",id)?AlertTjParam(par):AddTjParam(par);
}
bool CTajiDbMgr::AddTjParam(TTjParam* par)
{
    try{
        CppSQLite3Buffer  strInst;
        const char* tmp = strInst.format("Insert into %Q values(%d,'%s',%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%d)",
                        "tbltajiparam", par->taji_id,par->devserail.c_str(), par->pos.x, par->pos.y, \
                        par->taji_type, par->height,par->long_arm_length,par->min_car_dist,par->long_arm_length,par->short_arm_length, \
                        par->max_up_angle, par->min_up_angle, par->a0,par->isown);
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
bool CTajiDbMgr::AlertTjParam(TTjParam* par)
{
    try{
        CppSQLite3Buffer  strModify;
        const char* tmp = strModify.format("update %Q set serial='%s', x_pos=%f, y_pos=%f,type=%d, \
                                            height=%f, max_car_dist=%f, min_car_dist=%f, long_arm_len=%f, short_arm_len=%f, \
                                            max_up_angle=%f, min_up_angle=%f, jiegou=%f, isown=%d where id=%d;", \
                        "tbltajiparam", par->devserail.c_str(), par->pos.x, par->pos.y, \
                        par->taji_type, par->height, par->long_arm_length,par->min_car_dist,par->long_arm_length,par->short_arm_length, \
                        par->max_up_angle, par->min_up_angle, par->a0,par->isown,par->taji_id);
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

bool CTajiDbMgr::UpdateCalibrateParam(TCalcParam* par)
{
     return checkExistData("tblcalc")?AlertCalibrateParam(par):AddCalibrateParam(par);
}
bool CTajiDbMgr::AlertCalibrateParam(TCalcParam* par)
{
    try{
        CppSQLite3Buffer  strModify;
        const char* tmp = strModify.format("update %Q set angle_zero_ad=%d, angle_calc_k=%f, car_zero_dist_ad=%d,car_calc_k=%f where 1=1;", \
                        "tbltajiparam", par->anchor_zero_ad,par->anchor_k,par->min_car_dist_ad,par->min_car_dist_k);
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
bool CTajiDbMgr::AddCalibrateParam(TCalcParam* par)
{
    try{
        CppSQLite3Buffer  strInst;
        const char* tmp = strInst.format("Insert into %Q values(%d %f,%d,%f);", \
                        "tblcalc", par->anchor_zero_ad,par->anchor_k,par->min_car_dist_ad,par->min_car_dist_k);
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

bool CTajiDbMgr::AlertZoneParam(int index,PolyDef par)
{

}
bool CTajiDbMgr::AddZoneParam(int index,PolyDef par)
{

}
bool CTajiDbMgr::UpdateZoneParam(int index,PolyDef par)
{

}
bool CTajiDbMgr::UpdateDividingParam(int index,PolyDef par)
{

}
bool CTajiDbMgr::AlertDividingParam(int index,PolyDef par)
{

}
bool CTajiDbMgr::AddDividingParam(int index,PolyDef par)
{

}
*/

void CTajiDbMgr::GetWorkSiteParam(TWorkSiteParam &ws)
{

    TIniFile cfg("ctx2000.config");
    cfg.ReadString("worksite","leftdown","0,0");

}
int  CTajiDbMgr::ListAlaramInfo(int start, int count, THistoyRst &rst )
{
    return 0;
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
            item.type    = qry.getIntField("type");
            item.date    = qry.getStringField("dt");
            item.serial  = qry.getStringField("tjid");
            item.slewing = qry.getIntField("dist");
            item.trolley = qry.getIntField("angle");
            item.fudu    = qry.getIntField("fudu");
            item.wet     = qry.getIntField("wet");
            item.jiaodu  = qry.getIntField("jiaodu");
            item.beilv   = qry.getIntField("beilv");
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
bool CTajiDbMgr::AddAlarmInfo(int type,int slewing, int trolley,int fudu,int wet,int jiaodu,int beilv)
{
    try{
        CppSQLite3Buffer  strInst;
        LocalDateTime now;
        std::string dt = DateTimeFormatter::format(now,"%Y-%m-%d %h:%M:%S");

        const char* tmp = strInst.format("Insert into %Q values(%d,'%s',%d,%d,'%s',%d,%d,%d,%d);", \
                        "tbalarm", type,m_local_serial.c_str(),slewing, trolley,dt.c_str(),fudu,wet,jiaodu,beilv);
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
