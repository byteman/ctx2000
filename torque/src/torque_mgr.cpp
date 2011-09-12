#include "torque_mgr.h"
#include <Poco/SingletonHolder.h>
#include <Poco/Mutex.h>
#include <Poco/String.h>
#include <Poco/NumberParser.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include "iniFile.h"
#include "comdata.h"
#ifdef TC_DEBUG
//#define TC_DBG(fmt...) do { } while (0)
#define TC_DBG
#else
#define TC_DBG(fmt...) fprintf(stderr,fmt);
#endif


#define ctx2000_torque_db "./etc/tc.db"
static Poco::FastMutex _mutex;
CTorQueMgr::CTorQueMgr()
{
    m_state  = TQ_NORMAL;
    m_load_ok= false;
    m_open   = false;
    m_rate_index   = 0;
    m_rated_weight = 0;
}
CTorQueMgr& CTorQueMgr::get()
{
    static Poco::SingletonHolder<CTorQueMgr> sh;
    return *sh.get();
}
bool lessmark(const TorQueItem& s1,const TorQueItem& s2)
{
    return s1.carPos < s2.carPos;
}
bool greatermark(const TorQueItem& s1,const TorQueItem& s2)
{
    return s1.carPos > s2.carPos;
}
/*
将rst按postion从小到排列
*/
void       CTorQueMgr::sortByInc(TorQueItemSet& rst)
{
    sort(rst.begin (),rst.end (),lessmark);
}
bool   CTorQueMgr::ReloadCfg(std::string type, std::string armlen, std::string rate)
{
    m_open = false;
    return loadCfg(type,armlen,rate);
}
bool   CTorQueMgr::loadCfg(std::string type, std::string armlen, std::string rate)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
    //TC_DBG("load type=%s\n",type.c_str ());
    TC_DBG("load tcType[%s] armlen[%s] rate[%s]\n",type.c_str (),armlen.c_str (),rate.c_str ());
    bool findRate = false;
    if(!m_open)
    {
        if(!m_db.open(ctx2000_torque_db)) {
            TC_DBG("can not open torque.db\n");
            return false;
        }
        m_open = true;
    }

    if(type   =="" )     type   = m_tcType;
    if(armlen == "")     armlen = m_armlen;
    if(rate   == "")     rate   = m_rate;
    m_tmp_rate_list.clear ();
    if(m_db.listRateByLen(type, armlen, m_tmp_rate_list))
    {
        for(size_t i = 0; i < m_tmp_rate_list.size(); i++)
        {
            TC_DBG ("type=%s,len=%s,rate[%d]=%s\n",type.c_str (),armlen.c_str (),i,m_tmp_rate_list.at (i).c_str());
            if(Poco::trim(m_tmp_rate_list.at(i)) == Poco::trim(rate))
            {
                findRate = true;
                m_rate_index = i;

                m_state  = TQ_NORMAL;
                m_rate_list.clear();
                m_rate_list.assign(m_tmp_rate_list.begin (),m_tmp_rate_list.end ());
                break;
            }
        }
    }
    if(!findRate)
    {
          TC_DBG ("can not find rate for type[%s] armlen[%s]\n",type.c_str (),armlen.c_str ());
          return false;
    }
    m_torque_list.clear ();
    if(m_db.listTorQueInfoByRate (type,armlen,rate,m_torque_list))
    {
        m_tcType = type;
        m_armlen = armlen;
        m_rate   = rate;
        if(Poco::NumberParser::tryParse (rate,g_tc_rate))
        {
            m_rate_int = g_tc_rate;
        }else{
            TC_DBG ("can not Convert Rate[%s] to int\n",rate.c_str ());
        }
        m_torque_item_list.clear ();
        for(size_t i = 0 ; i < m_torque_list.size (); i++)
        {
            double tmp_pos, tmp_wet;
            if(m_torque_list.at (i).tryGetLen(tmp_pos))
            {
                if(m_torque_list.at (i).tryGetWeight(tmp_wet))
                {
                    TorQueItem item(tmp_pos, tmp_wet);
                    //TC_DBG("add pos=%0.2f weight=%0.2f\n",tmp_pos,tmp_wet);
                    m_torque_item_list.push_back (item);
                }
            }
        }
    }else{
      TC_DBG ("can not load Torque By Rate\n");
      return false;
    }

    sortByInc (m_torque_item_list);
#if 0
    TC_DBG ("after sort by inc\n");
    for(size_t i = 0 ; i < m_torque_item_list.size (); i++)
    {
        TC_DBG("add pos=%0.2f weight=%0.2f\n",m_torque_item_list.at (i).carPos,m_torque_item_list.at (i).weight);
    }
#endif
    TC_DBG ("load Torque ok\n");
    m_load_ok = true;
    return true;

}
/*
必须要保证，塔机的位置要从小到大
*/
ETorQueState CTorQueMgr::getState(double carPos, double weight)
{
    //double max_weight = 0;
    //double prev_pos   = 99999;
    bool   find       = false;
    Poco::FastMutex::ScopedLock lock(_mutex);
    for(size_t i = 0; i < m_torque_item_list.size(); i++)
    {
        //TC_DBG("car_pos=%0.2f cur=%0.2f\n",carPos , m_torque_item_list.at(i).carPos);
        if(carPos <= m_torque_item_list.at(i).carPos)
        {
            /* 经过排序后，就不需要这里判断了
            if(!m_sorted)
            {
                if(m_torque_item_list.at(i).carPos <= prev_pos) //找到最小的一个大于carPos的位置
                {
                    prev_pos   = m_torque_item_list.at(i).carPos;
                    max_weight = m_torque_item_list.at(i).weight;
                }
            }*/
            //TC_DBG("find car_pos=%0.2f distance=%0.2f weight=%0.2f rated_weight=%0.2f\n",carPos,m_torque_item_list.at(i).carPos, weight,m_torque_item_list.at(i).weight);
            m_rated_weight = m_torque_item_list.at(i).weight;
            find       = true;
            break;
        }
    }
    if( !find )
    {
        //如果小车的当前位置没有在给定类型的塔机的长度列表中，则返回最后一个位置的额定重量
        size_t cnt = m_torque_item_list.size();
        if( cnt > 0 )
            m_rated_weight = m_torque_item_list.at(cnt-1).weight;
        else
            m_rated_weight = weight;
    }
    //if( max_weight < 0.1 )//得到了异常的重量
    //    return 0;
    //m_rated_weight = max_weight;
    m_percent      = weight / m_rated_weight;
    if(m_percent >= 1.05)
    {
        m_state = TQ_FATAL;
    }else if(m_percent >= 1)
        m_state = TQ_ALARM;
    else if(m_percent >= 0.95)
        m_state = TQ_WARN;
    else if(m_percent >= 0.90)
        m_state = TQ_ALTER;
    else
        m_state = TQ_NORMAL;
    //TC_DBG("weight=%0.2f, max_weight=%0.2f,percent=%0.3f\n",weight,max_weight,m_percent);
    return m_state;

}
int    CTorQueMgr::changeRate(int newRate)
{
      newRate = newRate;
      if( m_torque_list.size() == 0 )
          return -1;
      if( !m_load_ok )
          return -2;
      size_t max = m_rate_list.size();
      size_t tmp_index = m_rate_index+1;
      if( tmp_index >= max)
          tmp_index  = 0;

      std::string tmp_rate = m_rate_list.at(tmp_index);

      if(loadCfg(m_tcType,m_armlen,tmp_rate))
      {

          int rate = 0;
          if(Poco::NumberParser::tryParse(Poco::trim(tmp_rate),rate))
          {
              m_rate_int   = rate;
              m_rate_index = tmp_index;
          }
      }else{
          TC_DBG("changeRate Failed\n");

      }

      return m_rate_int;
}
bool   CTorQueMgr::saveCfg(std::string type,std::string armlen, std::string rate)
{
    TIniFile cfg("./etc/ctx2000.ini");
    if(type.length () == 0)
        cfg.WriteString("device","tctype",m_tcType);
    else
        cfg.WriteString("device","tctype",type);
    if(armlen.length () == 0)
        cfg.WriteString("device","armlen", m_armlen);
    else
        cfg.WriteString("device","armlen", armlen);
    if(rate.length () == 0)
        cfg.WriteString("device","rate",  m_rate);
    else
        cfg.WriteString("device","rate",  rate);
    return true;
}
