#include "lijuctrl.h"
#include "comdata.h"
#include <Poco/NumberParser.h>
#include <Poco/SingletonHolder.h>
#include <Poco/String.h>
#include <Poco/Mutex.h>
#include <iostream>
static Poco::FastMutex _mutex;
CLijuCtrl&  CLijuCtrl::Get()
{
    static Poco::SingletonHolder<CLijuCtrl> sh;
    return *sh.get();
}
CLijuCtrl::CLijuCtrl(std::string type, int armlen, int beilv)
{

}
bool CLijuCtrl::Load(std::string type, std::string armlen, std::string beilv)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
    TLijuRst rst;
    m_lijuGrp.clear();
    m_BeilvList.clear();
    m_curBeilvIndex = -1;
    bool bExistBeilv= false;
    //获取指定的塔机类型和大臂长度所对应的所有倍率，存入倍率列表
    if(CTajiDbMgr::Get().getTjBeilv(type,armlen,m_BeilvList))
    {
        for(size_t i =0; i < m_BeilvList.size(); i++)
        {
            std::cerr << "beilv[" << i << "] " << m_BeilvList.at(i) << std::endl;
            if(m_BeilvList.at(i) == beilv)
            {
                bExistBeilv = true;
                m_curBeilvIndex = i;
                break;
            }
        }
    }else{
        std::cerr << "Can't Get " << type << ":" << armlen << "'s beilv\n";
    }
    if(!bExistBeilv)
    {
        std::cerr << "do not exist beilv\n";
        m_tc_type = "";
        m_arm_len = "";
        return false;
    }
    //获取
    if(CTajiDbMgr::Get().getlijuItems(type,armlen,beilv,rst))
    {
        m_tc_type = type;
        m_arm_len = armlen;
        m_curBeilv= beilv;

        for(size_t i = 0; i < rst.size(); i++)
        {
            double tmp1=0,tmp2 = 0;
            if( Poco::NumberParser::tryParseFloat(rst.at(i).armlen,tmp1) )
            {
                if(Poco::NumberParser::tryParseFloat(rst.at(i).weight,tmp2))
                {
                    TLijuItem item(tmp1,tmp2);
                    m_lijuGrp.push_back(item);
                }

            }
        }
    }else{
        std::cerr<<"Can't getlijuItems\n";
    }
    std::cerr << "Load Liju Successful\n";
    m_load_ok = true;
    return true;
}
int CLijuCtrl::Service(double car_pos, double weight)
{
    double max_weight = 0;
    bool   find       = false;
    for(size_t i = 0; i < m_lijuGrp.size(); i++)
    {
        if(car_pos <= m_lijuGrp.at(i).car_pos)
        {
            max_weight = m_lijuGrp.at(i).max_weight;
            find       = true;
            break;
        }
    }
    if( !find )
    {
        int cnt = m_lijuGrp.size();
        if( cnt > 0 )
            max_weight = m_lijuGrp.at(cnt-1).max_weight;
    }
    if( max_weight < 0.1 )//得到了异常的重量
        return 0;
    m_percent = weight / max_weight;
    if(m_percent >= 1.05)
    {
        m_cur_state = 3;
    }else if(m_percent >= 1)
        m_cur_state = 2;
    else if(m_percent >= 0.95)
        m_cur_state = 1;
    else
        m_cur_state = 0;

    //fprintf(stderr,"weight=%0.2f, max_weight=%0.2f,percent=%0.3f\n",weight,max_weight,m_percent);
    return m_cur_state;
}
int CLijuCtrl::ChangeBeilv(int newBeilv)
{
    if( ( m_curBeilvIndex == -1) || ( m_curBeilv.size() == 0 ) )
        return -1;
    if( !m_load_ok )
        return -2;

    int max = m_BeilvList.size();
    if( (++m_curBeilvIndex) >= max )
        m_curBeilvIndex = 0;
    std::string tmp = m_BeilvList.at(m_curBeilvIndex);

    if(Load(m_tc_type,m_arm_len,tmp))
    {
        int beilv = 0;
        if(Poco::NumberParser::tryParse(Poco::trim(tmp),beilv))
        {
            return beilv;
        }

    }
    return 0;
}
