#include "dataacquire.h"
#include <cstdio>
#include <vector>
#include <SerialStream.h>
#include <SerialStreamBuf.h>
#include <Poco/Runnable.h>
#include <Poco/ThreadPool.h>
#include <Poco/Notification.h>
#include <Poco/Exception.h>
#include <Poco/Event.h>
#include <SerialPort.h>
#include <Poco/SingletonHolder.h>
#include <Poco/NumberParser.h>
#include "tajidbmgr.h"
#include "comdata.h"
#include "common.h"
#include <math.h>
#include "filter.h"
//#define AD_DEBUG
#ifdef AD_DEBUG
#define AD_DBG(fmt...) fprintf(stderr,fmt);
#else
#define AD_DBG(fmt...) do { } while (0)
#endif

#define AD_PRO_LEN           17
using Poco::Runnable;
using Poco::SingletonHolder;
using Poco::Exception;
using LibSerial::SerialPort;
using LibSerial::SerialStream;
using LibSerial::SerialStreamBuf;

static double crane_angle_save,crane_angle_thresh,crane_angle;
static void calc_angle(int ad_angle)
{
    crane_angle= (ad_angle-g_bd[BD_ANGLE].zero_ad)/g_bd[BD_ANGLE].bd_k+g_bd[BD_ANGLE].start_value;
    if (fabs(crane_angle_save-crane_angle)>crane_angle_thresh)
        crane_angle_save=crane_angle;
    else
        crane_angle=crane_angle_save;

    int kk = (int)crane_angle;
    crane_angle=crane_angle-kk+(kk % 360);
    if (crane_angle<0)
        crane_angle=crane_angle+360;
    g_angle = crane_angle;
    g_TC[g_local_id].Angle = g_angle*3.14/180;
}

class CDataAcquireWorker1:public Poco::Runnable{
public:

    CDataAcquireWorker1(std::string devpath,Poco::NotificationQueue &queue)
        :m_port(NULL),
         m_path(devpath),
         m_queue(queue)
    {
        if(m_port == NULL)
            m_port = new SerialPort(m_path);
    }
    virtual ~CDataAcquireWorker1()
    {
        if(m_port)
        {
            delete m_port;
        }
    }
    virtual bool start(SerialPort::BaudRate baud)
    {
        try{
            if(m_port)
                m_port->Open(baud);
        }catch(...){
            fprintf(stderr,"serial %s open failed\n",m_path.c_str());
            return false;
        }
        m_quit = false;
        m_thread.start(*this);
        return m_rdyEvt.tryWait(1000);
    }
    bool stop()
    {
        m_quit = true;
        return m_quitEvt.tryWait(1000);
    }
    void calc_rt_value(LibSerial::SerialPort::DataBuffer& data)
    {

#ifdef AD_DEBUG
        AD_DBG("ad1 data=");
        for(size_t i=0; i <data.size(); i++)
        {
            AD_DBG("0x%x ",data.at(i));
        }
        fprintf(stderr,"\n");
#endif
        if( (data.at(0) == 0xaa) && (data.at(AD_PRO_LEN-1) == 0x55) )
        {
             static double tmp_car_dist  = 0;
             static double tmp_dg_weight = 0;
             static double tmp_dg_height = 0;

            ad_car_dist = m_filter[0].Filter ((data.at(1)<<8)+data.at(2));

            AD_DBG("ad_car=%d up_angle=%0.2f\n",ad_car_dist,g_up_angle);
            if(g_TC[g_local_id].Dyna){ //动臂式的幅度计算

                ad_up_angle = ad_car_dist;
                g_up_angle  = (ad_up_angle-g_bd[BD_UP_ANGLE].zero_ad)/g_bd[BD_UP_ANGLE].bd_k+g_bd[BD_UP_ANGLE].start_value;
                tmp_car_dist  = g_TC[g_local_id].LongArmLength*cos(g_up_angle*3.1415/180)+g_TC[g_local_id].a0;
                g_TC[g_local_id].Dang     = g_up_angle;
                g_TC[g_local_id].Position = g_TC[g_local_id].LongArmLength;

            }else{ //平臂式的幅度计算
                tmp_car_dist = (ad_car_dist-g_bd[BD_CAR_DIST].zero_ad)/g_bd[BD_CAR_DIST].bd_k+g_bd[BD_CAR_DIST].start_value;
                g_TC[g_local_id].Dang     = 0;
                g_TC[g_local_id].Position = g_car_dist;
            }
            //PostMessage(GetActiveWindow(),0x804,0,0);
            ad_height =  m_filter[1].Filter((data.at(3)<<8)+data.at(4));
            tmp_dg_height=(ad_height-g_bd[BD_HEIGHT].zero_ad)/g_bd[BD_HEIGHT].bd_k+g_bd[BD_HEIGHT].start_value;
            ad_weight =  m_filter[2].Filter((data.at(5)<<8)+data.at(6));

            double k = g_bd[BD_WEIGHT].bd_k;
            double z = g_bd[BD_WEIGHT].zero_ad;

            if( (ad_weight <= g_bd[BD_WEIGHT].bd_ad) && (ad_weight >= g_bd[BD_WEIGHT].zero_ad))
            {
                k = g_bd[BD_WEIGHT].bd_k;

            }
            else if(g_bd[BD_WEIGHT2].valid &&  (ad_weight <= g_bd[BD_WEIGHT2].bd_ad) && (ad_weight >= g_bd[BD_WEIGHT2].zero_ad))
            {
                    k = g_bd[BD_WEIGHT2].bd_k;
            }
            else if(g_bd[BD_WEIGHT3].valid && ad_weight >= g_bd[BD_WEIGHT3].zero_ad){

                k = g_bd[BD_WEIGHT3].bd_k;
            }

            tmp_dg_weight=(ad_weight-z)/k;
#if 0
            if(gMainMenuIndex==2) //single device mode
            {
                ad_angle = m_filter[3].Filter((data.at(7)<<8)+data.at(8));
                calc_angle (ad_angle);
            }
#endif
            ad_angle_x    = m_filter[4].Filter((data.at(9)<<8)+data.at(10));
            ad_angle_y    = m_filter[5].Filter((data.at(11)<<8)+data.at(12));

            static int wild_cnt  = 0;
            if(data.at(13)&0x80)
            {
                wild_cnt++;
                if(wild_cnt > 30)
                {
                    g_wild_speed  = 0;
                    wild_cnt = 0;
                }

            }else{
                wild_cnt = 0;
                ad_wild_speed =  (data.at(13)<<8)+data.at(14);
                ad_wild_speed &= 0x7FFF;
                //635 == 20m/s
                g_wild_speed   = (double)((ad_wild_speed*20)/635);
            }

#define K_X 		7.72797527
#define K_Y 		7.7183153
            //71/51 = 1.392156863
            double vx = (ad_angle_x - g_bd[BD_ANGLE_X].zero_ad)*1.392156863/1000;
            double vy = (ad_angle_y - g_bd[BD_ANGLE_Y].zero_ad)*1.392156863/1000;
            //fprintf(stderr,"ax=%d,ay=%d,xz=%d yz=%d vx=%0.2f,vy=%0.2f\n",ad_angle_x,ad_angle_y,g_bd[BD_ANGLE_X].zero_ad,g_bd[BD_ANGLE_Y].zero_ad,vx,vy);
            g_angle_x  = asin(vx/K_X) * 180.0f;
            g_angle_y  = asin(vy/K_Y) * 180.0f;
            //fprintf(stderr,"x=%0.2f y=%0.2f\n",g_angle_x,g_angle_y);
            g_dg_weight= tmp_dg_weight<0?0:tmp_dg_weight;
            g_car_dist = tmp_car_dist<0?0:tmp_car_dist;
            g_dg_height= tmp_dg_height<0?0:tmp_dg_height;
        }
    }
    virtual void run()
    {
        set_thread_title("ctx2000.ad1");
        m_rdyEvt.set();
        m_quitEvt.reset();
        m_quit = false;
        m_buf.clear();
        while(!m_quit)
        {

            try{
                Poco::Thread::sleep(50);
                m_buf.clear();
                m_port->WriteByte(0x2);
                m_port->Read(m_buf,AD_PRO_LEN,1000);
                if(m_buf.size () >= AD_PRO_LEN)
                    calc_rt_value(m_buf);
            }catch(LibSerial::SerialPort::ReadTimeout& e)
            {
                AD_DBG("%s for 6 chanle ad timeout len=%d\n",m_path.c_str(),m_buf.size());
                continue;
            }
        }
        m_port->Close();
        m_quitEvt.set();
    }
protected:
    SerialPort  *m_port;
    LibSerial::SerialPort::DataBuffer m_buf;
    Poco::Thread m_thread;
    Poco::Event  m_rdyEvt;
    Poco::Event  m_quitEvt;
    std::string  m_path;
    CFilter      m_filter[6];
    Poco::NotificationQueue &m_queue;
    volatile bool m_quit;

};
class CDataAcquireWorker2:public CDataAcquireWorker1{
public:
    CDataAcquireWorker2(std::string devpath,Poco::NotificationQueue &queue):
        CDataAcquireWorker1(devpath,queue)
    {
        // [ #01 enter ]
        sprintf(m_cmd,"#%02d\r",encoder_addr);
        fprintf(stderr,"cmd=%s",m_cmd);
    }
    virtual ~CDataAcquireWorker2()
    {

    }
    void my_calc_angle()
    {
        calc_angle(ad_angle);
    }
    virtual void run()
    {
        set_thread_title("ctx2000.ad2");
        m_rdyEvt.set();
        m_quitEvt.reset();
        m_quit = false;
        std::string data="";
        char buf[15] = {0,};
        int tmp=0;
        while(!m_quit)
        {

            try{

                m_buf.clear();

                m_port->Write((unsigned char*)m_cmd,4);
                Poco::Thread::sleep(100);
                m_port->Read(m_buf, 0, 100);

                if(m_buf.size() != 13)
                {
                    continue;
                }
                memcpy(buf,m_buf.data()+1,11);
                buf[11]=0;
                //fprintf(stderr,"%s\n",buf);
                if(Poco::NumberParser::tryParse(buf,tmp))
                {
                    ad_angle = tmp;//m_filter[5].Filter(tmp);
                    my_calc_angle();
                }
            }catch(LibSerial::SerialPort::ReadTimeout& e)
            {

                #ifdef AD_DEBUG
                    AD_DBG("ad1 data=");
                    for(size_t i = 0; i <m_buf.size();i++)
                    {
                        AD_DBG("0x%x ",m_buf.at(i));
                    }
                    fprintf(stderr,"\n");
                #endif
                continue;
            }
        }
        m_port->Close();
        m_quitEvt.set();
    }

private:
    char m_cmd[4];

};

CDataAcquire& CDataAcquire::Get()
{
    static SingletonHolder<CDataAcquire> sh;
    return *sh.get();
}

CDataAcquire::CDataAcquire():
    m_aq_work1(NULL),
    m_aq_work2(NULL),
    m_ps_work(NULL)
{

}

bool CDataAcquire::Init(std::string path1,std::string path2)
{
    if(!m_aq_work1)
        m_aq_work1 = new CDataAcquireWorker1(path1,m_nq);
    if(!m_aq_work2)
        m_aq_work2 = new CDataAcquireWorker2(path2,m_nq);
#if 0
    if(gMainMenuIndex==1) //multi type
    {
        if(!m_aq_work2)
            m_aq_work2 = new CDataAcquireWorker2(path2,m_nq);
    }
#endif
    return true;

}
bool CDataAcquire::StartAcquireMcuAD(std::string pathToMCU)
{
    if(!m_aq_work1)
        m_aq_work1 = new CDataAcquireWorker1(pathToMCU,m_nq);
    fprintf(stderr,"mcu = %s\n",pathToMCU.c_str ());
    return m_aq_work1->start(LibSerial::SerialPort::BAUD_57600);
}
bool CDataAcquire::StartAcquireAngleEncoder(std::string pathToEncoder)
{
    if(!m_aq_work2)
        m_aq_work2 = new CDataAcquireWorker2(pathToEncoder,m_nq);
    return m_aq_work2->start(LibSerial::SerialPort::BAUD_115200);
}
/*
启动数据采集模块
path1:  单片机通讯的串口获取8路ad数据
path2:  和编码器通讯的串口
*/
bool CDataAcquire::Start(std::string path1,std::string path2)
{
    return  false;
}
bool CDataAcquire::Stop()
{
    if(m_aq_work1) m_aq_work1->stop();
    if(m_aq_work1) m_aq_work2->stop();

    if(m_aq_work1) delete m_aq_work1;
    if(m_aq_work2) delete m_aq_work2;
    m_aq_work1 = NULL;
    m_aq_work2 = NULL;

    return true;
}
bool CDataAcquire::Pause(bool yes)
{
    return false;
}
void CDataAcquire::addObserver(const AbstractObserver& observer)
{
    m_nc.addObserver(observer);
}
void CDataAcquire::removeObserver(const AbstractObserver& observer)
{
    m_nc.removeObserver(observer);
}
