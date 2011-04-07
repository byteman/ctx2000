#include "dataacquire.h"
#include <cstdio>
#include <vector>
//#include <iostream>

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


using Poco::Runnable;
using Poco::SingletonHolder;
using Poco::Notification;
using Poco::Exception;
using LibSerial::SerialPort;

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
    bool start(SerialPort::BaudRate baud)
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

    virtual void run()
    {

        m_rdyEvt.set();
        m_quitEvt.reset();
        m_quit = false;

        while(!m_quit)
        {
            try{
                m_buf.clear();
                m_port->Read(m_buf, 15, 50);
            }catch(LibSerial::SerialPort::ReadTimeout& e)
            {
                //std::cerr << "Serial TimeOut\n";
                continue;
            }
    #if 1
            for(size_t i = 0; i < m_buf.size(); i++)
            {
                fprintf(stderr , "0x%x ",m_buf.at(i));
            }
            fprintf(stderr , "\n");
    #endif
            if( (m_buf.at(0) == 0xaa) && (m_buf.at(14) == 0x55) )
            {
                m_queue.enqueueNotification(new TADNotification(1,m_buf));
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
        m_cmd[0] = 0x23;
        m_cmd[1] = 0x30;
        m_cmd[2] = 0x31;
        m_cmd[3] = 0x0D;
    }
    virtual ~CDataAcquireWorker2()
    {

    }
    virtual void run()
    {
        m_rdyEvt.set();
        m_quitEvt.reset();
        m_quit = false;
        while(!m_quit)
        {
            try{
                  m_buf.clear();
                  m_port->Write(m_cmd,4);
                  m_port->Read(m_buf, 13, 50); //µÈ´ý50ms
            }catch(LibSerial::SerialPort::ReadTimeout& e)
            {
                //std::cerr << "Serial TimeOut\n";
                continue;
            }
    #if 1
            for(size_t i = 0; i < m_buf.size(); i++)
            {
                fprintf(stderr , "0x%x ",m_buf.at(i));
            }
            fprintf(stderr , "\n");
    #endif
            if( (m_buf.at(0) == 0x3D) && (m_buf.at(12) == 0x0D) )
            {
                m_queue.enqueueNotification(new TADNotification(2,m_buf));
            }
        }
        m_port->Close();
        m_quitEvt.set();
    }

private:
    unsigned char m_cmd[4];

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
    m_ps_work(NULL)
{

}

bool CDataAcquire::Init(std::string path)
{
    if(!m_aq_work1)
        m_aq_work1 = new CDataAcquireWorker1(path,m_nq);
    if(!m_aq_work2)
        m_aq_work2 = new CDataAcquireWorker2(path,m_nq);
    if(!m_ps_work)
        m_ps_work  = new CDataParseWorker(m_nq,m_nc);
    return true;
}
/*

*/
bool CDataAcquire::Start(std::string path)
{
    if( !Init(path) ) return false;
    if( !m_aq_work1->start(LibSerial::SerialPort::BAUD_115200) ) return false;
    if( !m_aq_work2->start(LibSerial::SerialPort::BAUD_9600)   ) return false;

    return  m_ps_work->start();
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
