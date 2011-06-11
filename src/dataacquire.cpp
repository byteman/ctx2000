#include "dataacquire.h"
#include <cstdio>
#include <vector>
//#include <iostream>
#include <SerialStream.h>
#include <SerialStreamBuf.h>
#include <Poco/Runnable.h>
#include <Poco/ThreadPool.h>
//#include <Poco/NotificationQueue.h>
#include <Poco/Notification.h>
#include <Poco/Exception.h>
#include <Poco/Event.h>
#include <SerialPort.h>
#include <Poco/SingletonHolder.h>
#include <Poco/NumberParser.h>
#include "tajidbmgr.h"
#include "comdata.h"
#include "common.h"
#define DEBUG 1
using Poco::Runnable;
using Poco::SingletonHolder;
using Poco::Notification;
using Poco::Exception;
using LibSerial::SerialPort;
using LibSerial::SerialStream;
using LibSerial::SerialStreamBuf;
static CFilter m_filter[6];
class TADNotification: public Notification
{
public:
        TADNotification(int type ,SerialPort::DataBuffer &src):
            m_type(type)
        {
            m_data.assign(src.begin(),src.end());
        }
        ~TADNotification()
        {
        }
public:
        int m_type;
        SerialPort::DataBuffer m_data;
};

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
#if 0
        for(size_t i=0; i <data.size(); i++)
        {
            fprintf(stderr,"0x%x ",data.at(i));
        }
        fprintf(stderr,"\n");
#endif
        if( (data.at(0) == 0xaa) && (data.at(14) == 0x55) )
        {
            //ad_up_angle =  m_filter[3].Filter((data.at(7)<<8)+data.at(8));

            ad_car_dist = m_filter[0].Filter((data.at(1)<<8)+data.at(2));

            //fprintf(stderr,"ad_car=%d up_angle=%0.2f\n",ad_car_dist,g_up_angle);
            if(g_TC[g_local_id].Dyna){ //动臂式的幅度计算
                ad_up_angle = ad_car_dist;
                g_up_angle = (ad_up_angle-g_bd[BD_UP_ANGLE].zero_ad)/g_bd[BD_UP_ANGLE].bd_k+g_bd[BD_UP_ANGLE].start_value;
                g_car_dist = g_TC[g_local_id].LongArmLength*cos(g_up_angle*3.14/180)+g_TC[g_local_id].a0;
                //fprintf(stderr,"dist=%0.2f %0.2f %0.2f %0.2f\n",g_car_dist,g_TC[g_local_id].LongArmLength,cos(g_up_angle*3.14/180),g_TC[g_local_id].a0);
            }else{ //平臂式的幅度计算
                g_car_dist=(ad_car_dist-g_bd[BD_CAR_DIST].zero_ad)/g_bd[BD_CAR_DIST].bd_k+g_bd[BD_CAR_DIST].start_value;
                //fprintf(stderr,"dist=%0.2f\n",g_car_dist);
            }
            //PostMessage(GetActiveWindow(),0x804,0,0);
            ad_height =  m_filter[1].Filter((data.at(3)<<8)+data.at(4));
            g_dg_height=(ad_height-g_bd[BD_HEIGHT].zero_ad)/g_bd[BD_HEIGHT].bd_k+g_bd[BD_HEIGHT].start_value;
            ad_weight =  m_filter[2].Filter((data.at(5)<<8)+data.at(6));
            g_dg_weight=(ad_weight-g_bd[BD_WEIGHT].zero_ad)/g_bd[BD_WEIGHT].bd_k+g_bd[BD_WEIGHT].start_value;

            ad_fengsu =  m_filter[4].Filter((data.at(9)<<8)+data.at(10));

            g_speed   = (double)(50*ad_fengsu)/(4096*3);

        }

        //g_speed=(ad_car_dist-g_bd[BD_CAR_DIST].zero_ad)/g_bd[BD_CAR_DIST].bd_k+g_bd[BD_CAR_DIST].start_value;
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
                m_port->Read(m_buf,15,1000);

                calc_rt_value(m_buf);
            }catch(LibSerial::SerialPort::ReadTimeout& e)
            {

                fprintf(stderr,"%s timeout len=%d\n",m_path.c_str(),m_buf.size());
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
        //m_cmd[0] = 0x23;
        //m_cmd[1] = 0x30;
        //m_cmd[2] = 0x31;
        //m_cmd[3] = 0x0D;
    }
    virtual ~CDataAcquireWorker2()
    {

    }
    void calc_angle()
    {
        crane_angle= (ad_angle-g_bd[BD_ANGLE].zero_ad)/g_bd[BD_ANGLE].bd_k+g_bd[BD_ANGLE].start_value;
        if (abs(crane_angle_save-crane_angle)>crane_angle_thresh)
            crane_angle_save=crane_angle;
        else
            crane_angle=crane_angle_save;

        int kk = (int)crane_angle;
        crane_angle=crane_angle-kk+(kk % 360);
        if (crane_angle<0)
            crane_angle=crane_angle+360;
        g_angle = crane_angle;
        //PostMessage(GetActiveWindow(),0x805,0,0);
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
                Poco::Thread::sleep(50);
                m_buf.clear();

                m_port->Write((unsigned char*)m_cmd,4);
                //fprintf(stderr,"send data%s\n",m_cmd);
                m_port->Read(m_buf,13,1000);

                memcpy(buf,m_buf.data()+1,11);
                buf[11]=0;
                //data=buf;
               //fprintf(stderr,"data=%s\n",buf);
                //data = m_port->ReadLine(100,'\r');
                //if( (data.length() == 13) && (data.at(0)=='='))
                {

                    if(Poco::NumberParser::tryParse(buf,tmp))
                    {
                        ad_angle = m_filter[5].Filter(tmp);
                        calc_angle();
                    }
                }
            }catch(LibSerial::SerialPort::ReadTimeout& e)
            {
#if 0
    for(size_t i = 0; i <m_buf.size();i++)
    {
        fprintf(stderr,"0x%x ",m_buf.at(i));
    }
    fprintf(stderr,"\n");
#endif
                fprintf(stderr,"%s timeout len=%d\n",m_path.c_str(),m_buf.size());
                continue;
            }
        }
        m_port->Close();
        m_quitEvt.set();
    }

private:
    double crane_angle_save,crane_angle_thresh,crane_angle;
    char m_cmd[4];

};

class CDataParseWorker:public Poco::Runnable{
public:
    CDataParseWorker(Poco::NotificationQueue& nq,Poco::NotificationCenter& nc):
        m_nq(nq),
        m_nc(nc)
    {

    }
    bool start()
    {
        m_quit = false;
        m_thread.start(*this);
        return m_rdyEvt.tryWait(1000);
    }
    bool stop()
    {
        m_quit = true;
        return m_quitEvt.tryWait(1000);
    }
    void parse(int type, LibSerial::SerialPort::DataBuffer& data)
    {
        if(type == 1)
        {
            if( (data.size() != 15) || ( (data.at(0) != 0xaa) && (data.at(14) != 0x55) ) )
                return;

            short ad[6] = {0,};
            ad[0] = (data.at(1)<<8)+data.at(2);
            ad[1] = (data.at(3)<<8)+data.at(4);
            ad[2] = (data.at(5)<<8)+data.at(6);
            ad[3] = (data.at(7)<<8)+data.at(8);
            ad[4] = (data.at(9)<<8)+data.at(10);
            ad[5] = (data.at(11)<<8)+data.at(12);
            //while(m_ad_queue.size())
            //{
            //    m_ad_queue.pop();
           // }
            for(size_t i = 0 ; i < 6; i++)
            {
                TAD a(i+1, ad[i]);
                m_ad_queue.push(a);
            }
        }else if(type == 2)
        {
            if( (data.size() != 13) || ( (data.at(0) != 0x3D) && (data.at(12) != 0x0D) ) )
                return;
            char buf[12] = {0,};
            for(int i = 1; i < 12; i++)
            {
                buf[i-1] = data[i];
            }
            int angle_ad = 0;
            if(! Poco::NumberParser::tryParse(buf,angle_ad))
                return;
            TAD a(7, angle_ad); //7 === angle ad
            m_ad_queue.push(a);
        }else{
            return;
        }

        m_nc.postNotification(new ADNotification(&m_ad_queue));

    }
    virtual void run()
    {
        m_rdyEvt.set();
        m_quitEvt.reset();
        m_quit = false;
        LibSerial::SerialPort::DataBuffer buf;
        while(!m_quit)
        {
            try
            {
                Poco::AutoPtr<Poco::Notification> pNf(m_nq.waitDequeueNotification(1000));
                if (pNf)
                {
                        Poco::AutoPtr<TADNotification> pMsgNf = pNf.cast<TADNotification>();
                        parse(pMsgNf->m_type,pMsgNf->m_data);
                }
            }
            catch (Poco::Exception& e)
            {
                std::cerr << "CDataParseWorker: " << e.displayText() << std::endl;
                    // parsing exception, what should we do?
            }
            catch (...)
            {
            }
        }
        m_quitEvt.set();
    }
private:
     Poco::NotificationQueue  &m_nq;
     Poco::NotificationCenter &m_nc;
     Poco::Thread  m_thread;
     Poco::Event   m_rdyEvt;
     Poco::Event   m_quitEvt;
     TAdQueue      m_ad_queue;
     volatile bool m_quit;
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
    //if(!m_ps_work)
    //    m_ps_work  = new CDataParseWorker(m_nq,m_nc);
    return true;
}
/*

*/
bool CDataAcquire::Start(std::string path1,std::string path2)
{

    if( !Init(path1,path2) ) return false;
    if(!m_aq_work1)return false;
    if(!m_aq_work2)return false;

        if( !m_aq_work1->start(LibSerial::SerialPort::BAUD_57600) ) return false;
    if(m_aq_work2)
        if( !m_aq_work2->start(LibSerial::SerialPort::BAUD_115200)   ) return false;

    return  true;
}
bool CDataAcquire::Stop()
{
    return (m_aq_work1->stop() && m_ps_work->stop());
}
bool CDataAcquire::Pause(bool yes)
{

}
void CDataAcquire::addObserver(const AbstractObserver& observer)
{
    m_nc.addObserver(observer);
}
void CDataAcquire::removeObserver(const AbstractObserver& observer)
{
    m_nc.removeObserver(observer);
}
