#include "diantai.h"

#include <Poco/Runnable.h>
#include <Poco/ThreadPool.h>
//#include <Poco/NotificationQueue.h>
#include <Poco/Notification.h>
#include <Poco/Exception.h>
#include <Poco/Event.h>
#include <SerialPort.h>
#include <Poco/SingletonHolder.h>
#include <Poco/Timespan.h>
#include <Poco/Mutex.h>
#include <SerialStream.h>
#include <SerialStreamBuf.h>
#include "tajidbmgr.h"
#include "comdata.h"
using Poco::Runnable;
using Poco::SingletonHolder;
using Poco::Notification;
using Poco::Exception;
using LibSerial::SerialPort;
using LibSerial::SerialStream;
using LibSerial::SerialStreamBuf;
static Poco::FastMutex _mutex;
const int DataLength   = 39;
const int WSDataLength = 154;
#define     MAX_LEN         8192
class CDianTaiDataReceiver:public Poco::Runnable{
public:

    CDianTaiDataReceiver(std::string devpath,Poco::NotificationQueue &queue)
        :m_path(devpath),
         m_queue(queue)
    {
        m_allow_send = true;
        m_port = NULL;
        if(m_port == NULL)
            m_port = new SerialPort(m_path);
    }
    bool start(SerialPort::BaudRate baud)
    {
        DBG("%s %d\n",__FUNCTION__,__LINE__);
/*
        m_port.Open(m_path.c_str());

        if ( ! m_port.good() )
        {
            std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
                      << "Error: Could not open serial port." << m_path
                      << std::endl ;
            return false;
        }
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        m_port.SetBaudRate( SerialStreamBuf::BAUD_19200 ) ;
        if ( ! m_port.good() )
        {
            std::cerr << "Error: Could not set the baud rate." << std::endl ;
            return false;
        }
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        //
        // Set the number of data bits.
        //
        m_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
        if ( ! m_port.good() )
        {
            std::cerr << "Error: Could not set the character size." << std::endl ;
            return false;
        }
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        //
        // Disable parity.
        //
        m_port.SetParity( SerialStreamBuf::PARITY_NONE ) ;
        if ( ! m_port.good() )
        {
            std::cerr << "Error: Could not disable the parity." << std::endl ;
            return false;
        }
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        //
        // Set the number of stop bits.
        //
        m_port.SetNumOfStopBits( 1 ) ;
        if ( ! m_port.good() )
        {
            std::cerr << "Error: Could not set the number of stop bits."
                      << std::endl ;
             return false;
        }
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        //
        // Turn off hardware flow control.
        //
        m_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;
        if ( ! m_port.good() )
        {
            std::cerr << "Error: Could not use hardware flow control."
                      << std::endl ;
             return false;
        }
        m_port.flush();
*/
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

    /*分析接收到的数据*/
    void parse_recv_data(char c)
    {
        static int work_site_count=0;
        int type = 0;
        //std::cerr << c << std::endl;
        if( (c == '%') || (c=='(') || (c=='$')){
            m_pos = 0;
            m_start_flag = true;
        }else if( m_start_flag && c=='#')
        {
            //fprintf(stderr,"recv # char m_pos = %d\n",m_pos);
            if(m_pos == 38)
            {
                type = 1;

            }
        }else if( m_start_flag && c=='*'){
            DBG("recv * char m_pos = %d\n",m_pos);
            if(m_pos == 11)
            {
                type = 2;

            }
        }else if( m_start_flag && c==')'){
            DBG("recv ) char m_pos = %d\n",m_pos);
            DBG("m_pos=%d\n",m_pos);
            type = 3;

        }
        if(c=='*')
        {
            work_site_count++;
            if(work_site_count >= 3)
            {
                m_allow_send=false;
            }
        }else{
            work_site_count = 0;
        }
        if( ( m_start_flag ) && (m_pos < MAX_LEN) )
            m_tmp[m_pos++]=c;
        Poco::Timestamp curTime;
        Poco::Timespan span = curTime-m_recv_stamp;
        Poco::Timespan timeout_span(5,0);
        if(span > timeout_span)
        {
            m_signal=false;
        }

        if(type > 0){
            if(m_pos>=1)
            {
                //m_tmp[0]       = 0;
                m_tmp[m_pos-1] = 0;

                CTX_Message msg;
                msg.context = std::string(m_tmp+1);
                msg.wType   = type;
                if(type == 1)
                {
                    Poco::FastMutex::ScopedLock lock(_mutex);
                    m_recv_stamp.update();
                    m_signal=true;
                    m_msg_que.push(msg);
                    //fprintf(stderr,"m_msg_que push data size=%d\n",m_msg_que.size());
                    if(m_msg_que.size() > 10)
                        fprintf(stderr,"m_msg_que push data size=%d\n",m_msg_que.size());

                }
                else if(type == 2)
                {
                    Poco::FastMutex::ScopedLock lock(_mutex);
                    m_height_msg_que.push(msg);
                    if(m_height_msg_que.size() > 10)
                        fprintf(stderr,"m_height_msg_que push data size=%d\n",m_height_msg_que.size());
                }
                else if(type == 3) // worksite
                {
                    Poco::FastMutex::ScopedLock lock(_mutex);
                    m_ws_msg_que.push(msg);
                    m_allow_send=true;
                    if(m_ws_msg_que.size() > 10)
                        fprintf(stderr,"m_ws_msg_que push data size=%d\n",m_ws_msg_que.size());
                }
                m_start_flag = false;

            }

        }


    }
    bool checkValid(char c)
    {
        return true;
        if( ( (c >= '0')&&(c<='9')) || (c=='N') || (c=='%')|| (c=='#') || (c=='.')|| (c=='-')|| (c==' ')||(c=='(' || (c==')')))
            return true;
        else
            return false;
    }
    void run()
    {
        set_thread_title("ctx2000.diantai");
        m_rdyEvt.set();
        m_quitEvt.reset();
        m_quit = false;
        m_buf.clear();
        while(!m_quit)
        {
/*
             char c = m_port->ReadByte();
             char c;
             m_port.get(c);
             if(checkValid(c)){
                parse_recv_data(c);
             }
             else{
                DBG("***********************Invalid char 0x%x\n",c);
             }
*/
             try{
                if(m_port)
                {
                    char c = m_port->ReadByte(1000);
                    if(checkValid(c)){
                       parse_recv_data(c);
                    }else{
                        fprintf(stderr,"***********************Invalid char 0x%x\n",c);
                     }
                }
             }catch(LibSerial::SerialPort::ReadTimeout& e)
             {

                 fprintf(stderr,"%s timeout len=%d\n",m_path.c_str(),m_buf.size());
                 continue;
             }

        }
        if(m_port)
            m_port->Close();

        m_quitEvt.set();
    }
    void SendData(std::string data)
    {
        //m_port->Write(data);
        if(m_allow_send && m_port)
            m_port->Write(data);
    }
    void clear()
    {
        while(m_msg_que.size()!=0)
            m_msg_que.pop();
    }
public:
    std::queue<CTX_Message> m_msg_que;
    std::queue<CTX_Message> m_ws_msg_que;
    std::queue<CTX_Message> m_height_msg_que;
    bool    m_signal;
    bool    m_allow_send;
private:
    //SerialStream m_port;
    SerialPort  *m_port;
    Poco::Thread m_thread;
    Poco::Event  m_rdyEvt;
    Poco::Event  m_quitEvt;
    std::string  m_path;
    Poco::NotificationQueue &m_queue;
    LibSerial::SerialPort::DataBuffer m_buf;
    std::string m_DataString;
    Poco::Timestamp             m_recv_stamp;

    char  m_tmp[MAX_LEN];
    int   m_pos;
    bool  m_start_flag;
    bool  m_bRtInfo;
    bool  m_bWsInfo;
    volatile bool m_quit;
};

class CDianTaiDataParseWorker:public Poco::Runnable{
public:
    CDianTaiDataParseWorker(Poco::NotificationQueue& nq,Poco::NotificationCenter& nc):
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
        //m_nc.postNotification(new ADNotification(&m_ad_queue));
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
                        //Poco::AutoPtr<TADNotification> pMsgNf = pNf.cast<TADNotification>();
                        //parse(pMsgNf->m_type,pMsgNf->m_data);
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
     //TAdQueue    m_ad_queue;
     volatile bool m_quit;
};

CDianTai::CDianTai()
{
    m_recv_worker = NULL;
    m_ps_worker   = NULL;
    m_send_worker = NULL;
}
CDianTai& CDianTai::Get()
{
    static SingletonHolder<CDianTai> sh;
    return *sh.get();
}
bool CDianTai::Start(std::string path)
{
  return ( Init(path) && m_recv_worker->start(SerialPort::BAUD_19200));
}
bool CDianTai::Stop()
{
    return (m_recv_worker->stop());
}
void CDianTai::addObserver(const AbstractObserver& observer)
{
    m_nc.addObserver(observer);
}
void CDianTai::removeObserver(const AbstractObserver& observer)
{
    m_nc.removeObserver(observer);
}
bool CDianTai::Init(std::string path)
{
    if(!m_recv_worker)
        m_recv_worker = new CDianTaiDataReceiver(path,m_nq);
    //if(!m_ps_worker)
     //   m_ps_worker  = new CDianTaiDataParseWorker(m_nq,m_nc);
    return true;
}
void CDianTai::ClearMessage()
{
    Poco::FastMutex::ScopedLock lock(_mutex);
    if(m_recv_worker)
        m_recv_worker->clear();
}
bool CDianTai::haveSignal()
{
    return m_recv_worker->m_signal;
}
size_t  CDianTai::CheckMessage(CTX_Message& msg, int timeout_ms,int type)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
    if(type == 1)
        return m_recv_worker->m_msg_que.size();
    else if(type == 2)
        return m_recv_worker->m_height_msg_que.size();
    else if(type == 3)
        return m_recv_worker->m_ws_msg_que.size();
    return 0;
}
int  CDianTai::GetMessage(CTX_Message& msg,int type, int timeout_ms)
{
    //fprintf(stderr,"msg num = %d\n",m_recv_worker->m_msg_que.size());
    Poco::FastMutex::ScopedLock lock(_mutex);

    if(type == 1)
    {
        if(m_recv_worker->m_msg_que.size() > 0 )
        {
            msg = m_recv_worker->m_msg_que.front();
            m_recv_worker->m_msg_que.pop();
            return 1;
        }else
           return 0;
    }else if(type == 2)
    {
        //DBG("%s %d\n",__FUNCTION__,__LINE__);
        if(m_recv_worker->m_height_msg_que.size() > 0 )
        {
            //DBG("%s %d\n",__FUNCTION__,__LINE__);
            msg = m_recv_worker->m_height_msg_que.front();
            m_recv_worker->m_height_msg_que.pop();
            return 1;
        }else
           return 0;
    }else if(type == 3)
    {
        if(m_recv_worker->m_ws_msg_que.size() > 0 )
        {
            msg = m_recv_worker->m_ws_msg_que.front();
            m_recv_worker->m_ws_msg_que.pop();
            return 1;
        }else
           return 0;
    }
    return 0;


}
int  CDianTai::SendMessage(CTX_Message& msg, int timeout_ms)
{
    m_recv_worker->SendData(msg.context);
    return 0;
}
int  CDianTai::SendMessage(std::string& msg, int timeout_ms)
{
    m_recv_worker->SendData(msg);
    return 0;
}
