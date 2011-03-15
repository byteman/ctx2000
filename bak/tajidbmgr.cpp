#include "tajidbmgr.h"
#include <stdio.h>
#include <Poco/Format.h>
#include <Poco/SingletonHolder.h>
#include <iostream>
using namespace std;
CTajiDbMgr& CTajiDbMgr::Get()
{
    static Poco::SingletonHolder<CTajiDbMgr> sh;
    return *sh.get();
}
CTajiDbMgr::CTajiDbMgr():
    m_opened(false)
{
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

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("select * from tblworksite");
        DBG("loadworksite %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        if(!qry.eof())
        {
            m_tj.ws.alert_dist    = qry.getFloatField("alert_dist");
            m_tj.ws.brake_dist    = qry.getFloatField("brake_dist");
            m_tj.ws.slowdown_dist = qry.getFloatField("slow_dist");
            m_tj.ws.car_high_spd_dist = qry.getFloatField("car_high_spd_dist");
            m_tj.ws.car_low_spd_dist  = qry.getFloatField("car_low_spd_dist");
            m_tj.ws.inertia_angle     = qry.getFloatField("angle");
            m_tj.ws.y_angle           = qry.getFloatField("y_angle");
    //工地坐标
            for(int i = 0; i < 4; i++)
            {
                std::string x_field = Poco::format("x%d",i);
                std::string y_field = Poco::format("y%d",i);
                double x = qry.getFloatField(x_field.c_str());
                double y = qry.getFloatField(y_field.c_str());
                Coord pos(x,y);
                m_tj.ws.worksite_coord.push_back(pos);
            }
    //回转标定开始和结束点的坐标
            for(int i = 0; i < 2; i++)
            {
                std::string x_field = Poco::format("calc_x%d",i);
                std::string y_field = Poco::format("calc_y%d",i);
                double x = qry.getFloatField(x_field.c_str());
                double y = qry.getFloatField(y_field.c_str());
                Coord pos(x,y);
                m_tj.ws.anchor_coord.push_back(pos);
            }
            bExist  = true;
        }
        qry.finalize();
    }
    catch (CppSQLite3Exception& e) {
            cerr << "loadworksite :" << e.errorCode() << ":" << e.errorMessage() << endl;
            bExist = false;
    }
    return bExist;
}
bool CTajiDbMgr::loadtjparam()
{
    bool bExist = false;

    try{
        CppSQLite3Buffer strSql;
        const char* tmp = strSql.format("Select * from %Q ","tbltajiparam");
        DBG("loadtjparam %s \r\n", tmp);
        CppSQLite3Query qry =  pDB->execQuery(tmp);
        if(!qry.eof())
        {
            m_tj.devserail        = qry.getStringField("serail");
            m_tj.pos.x            = qry.getFloatField("x_pos");
            m_tj.pos.y            = qry.getFloatField("y_pos");
            m_tj.taji_type        = qry.getIntField("type");
            m_tj.height           = qry.getFloatField("height");
            m_tj.max_up_angle     = qry.getFloatField("max_up_angle");
            m_tj.min_up_angle     = qry.getFloatField("min_up_angle");
            m_tj.long_arm_length  = qry.getFloatField("long_arm_dist");
            m_tj.min_car_dist     = qry.getFloatField("min_car_dist");
            m_tj.short_arm_length = qry.getFloatField("short_arm_len");
            m_tj.a0               = qry.getFloatField("jiegou");

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
            m_tj.calc.anchor_zero_ad   = qry.getIntField("angle_zero_ad");
            m_tj.calc.anchor_k         = qry.getFloatField("angle_calc_k");
            m_tj.calc.min_car_dist_ad  = qry.getIntField("car_zero_dist_ad");
            m_tj.calc.min_car_dist_k   = qry.getFloatField("car_calc_k");

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
        m_tj.zone.clear();
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
            m_tj.zone.push_back(poly);
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
            m_tj.div.push_back(poly);
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
                m_tj.set_worksite_param(par);
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
bool CTajiDbMgr::AddTjParam(TTjParam* par)
{
    try{
        CppSQLite3Buffer  strInst;
        const char* tmp = strInst.format("Insert into %Q values(%d,'%s',%f,%f,%f,%d,%f,%f,%f,%f,%f,%f,%f)",
                        "tbltajiparam", 0,par->devserail.c_str(), par->pos.x, par->pos.y, \
                        par->taji_type, par->height,par->long_arm_length,par->min_car_dist,par->long_arm_length,par->short_arm_length, \
                        par->max_up_angle, par->min_up_angle, par->a0);
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
        const char* tmp = strModify.format("update %Q set serial=%s, x_pos=%f, y_pos=%f,type=%d, \
                                            height=%f, max_car_dist=%f, min_car_dist=%f, long_arm_len=%f, short_arm_len=%f, \
                                            max_up_angle=%f, min_up_angle=%f, jiegou=%f where 1=1;", \
                        "tbltajiparam", par->devserail.c_str(), par->pos.x, par->pos.y, \
                        par->taji_type, par->height, par->long_arm_length,par->min_car_dist,par->long_arm_length,par->short_arm_length, \
                        par->max_up_angle, par->min_up_angle, par->a0);
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
