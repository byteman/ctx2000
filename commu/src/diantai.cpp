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
#include "guinotifyer.h"
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
Poco::Timespan timeout_span(5,0);
#define DT_DEBUG
#ifdef DT_DEBUG
#define DT_DBG(fmt...) fprintf(stderr,fmt);
#else
#define DT_DBG(fmt...) do { } while (0)
#endif
class CDianTaiDataReceiver:public Poco::Runnable{
public:

    CDianTaiDataReceiver(std::string devpath,Poco::NotificationQueue &queue)
        :m_path(devpath),
         m_queue(queue)
    {
        m_allow_send = true;
        m_signal     = 0;
        m_port       = NULL;
        m_receive_msg_count  = 0;
        m_receive_byte_count = 0;
        if(m_port == NULL)
            m_port = new SerialPort(m_path);
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

    /*分析接收到的数据*/
    void parse_recv_data(char c)
    {
        static int work_site_count=0;
        int type = 0;
        //DT_DBG("%c\n",c);
        if( (c == '%') || (c=='(') || (c=='$')){
            m_pos = 0;
            m_start_flag = true;
        }else if( m_start_flag && c=='#')
        {
            //DT_DBG("recv # char m_pos = %d\n",m_pos);
            if(m_pos == 38)
            {
                type = 1;

            }
        }else if( m_start_flag && c=='*'){
            DT_DBG("recv * char m_pos = %d\n",m_pos);
            if(m_pos == 11)
            {
                type = 2;

            }
        }else if( m_start_flag && c==')'){
            DT_DBG("recv ) char m_pos = %d\n",m_pos);
            DT_DBG("m_pos=%d\n",m_pos);

            type = 3;

        }
        if(c=='*')
        {
            work_site_count++;
            if(work_site_count >= 3)
            {
                m_allow_send=false;
                m_upload_stamp.update ();
                DT_DBG("diantai recev worksite header\n");
            }
        }else{
            work_site_count = 0;
        }
        if( ( m_start_flag ) && (m_pos < MAX_LEN) )
            m_tmp[m_pos++]=c;

        if(type > 0){
            if((m_pos>=1) && (m_pos < MAX_LEN))
            {
                //m_tmp[0]       = 0;
                m_tmp[m_pos-1] = 0;

                CTX_Message msg;
                msg.context = std::string(m_tmp+1);
                msg.wType   = type;
                if(type == 1)
                {
                    Poco::FastMutex::ScopedLock lock(_mutex);
                    //m_recv_stamp.update();
                    m_signal = 5;
                    m_receive_msg_count++;
                    if(m_msg_que.size () < 1000)
                        m_msg_que.push(msg);

                    //TGuiMsg* guiMsg = new TGuiMsg(1,msg.context);
                    //TGuiNotifyer::Get().Notify(guiMsg);

                    //fprintf(stderr,"m_msg_que push data size=%d\n",m_msg_que.size());
                    if(m_msg_que.size() > 10)
                        fprintf(stderr,"m_msg_que push data size=%d\n",m_msg_que.size());

                }
                else if(type == 2)
                {
                    Poco::FastMutex::ScopedLock lock(_mutex);
                     if(m_height_msg_que.size () < 10)
                        m_height_msg_que.push(msg);

                    fprintf(stderr,"recv height msg %s\n",msg.context.c_str());
                    if(m_height_msg_que.size() > 10)
                        fprintf(stderr,"m_height_msg_que push data size=%d\n",m_height_msg_que.size());
                }
                else if(type == 3) // worksite
                {
                    Poco::FastMutex::ScopedLock lock(_mutex);
                    if(m_ws_msg_que.size () < 10)
                        m_ws_msg_que.push(msg);
                    fprintf(stderr,"recv ws msg %s\n",msg.context.c_str());
                    m_allow_send=true;
                    if(m_ws_msg_que.size() > 10)
                        fprintf(stderr,"m_ws_msg_que push data size=%d\n",m_ws_msg_que.size());
                }
                m_start_flag = false;

            }

        }


    }
#include <ctype.h>
    bool checkValid(char c)
    {
        return isprint (c);
        //return true;
        //暂时不过滤，因为有可能设置塔机编号里面有任意的ascii字符
        if( ( (c >= '0')&&(c<='9')) || (c=='N') || (c=='%')|| (c=='#') || (c=='.')|| (c=='-')|| (c==' ') ||  (c=='$') ||  (c=='*') ||(c=='(' || (c==')')))
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

             try{
                if(m_port)
                {
                    char c = m_port->ReadByte(1000);
                    m_receive_byte_count++;
                    if(checkValid(c)){
                       parse_recv_data(c);
                    }else{
                        fprintf(stderr,"***********************Invalid char 0x%x\n",c);
                        char buf[16] = {0,};
                        sprintf(buf,"echo errcode %x >> err.out",c);
                        system(buf);
                    }
                }
             }catch(LibSerial::SerialPort::ReadTimeout& e)
             {
                 if(m_signal>0)m_signal--;
                 //fprintf(stderr,"%s for diantai timeout len=%d\n",m_path.c_str(),m_buf.size());
                 continue;
            }catch(...){
                system("echo diantai `date` >> err.out");
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
    /*
    清空串口缓存数据，并清除已经收到的数据包
    */
    void clear()
    {
        if(m_port)
        {
            static SerialPort::DataBuffer buf;
            buf.clear();
            try{
                m_port->Read(buf, 0, 20); //20ms超时时间是否合适
            }catch(LibSerial::SerialPort::ReadTimeout& e)
            {
                fprintf(stderr,"clear data ok read size=%d\n",buf.size());
            }
        }
        while(m_msg_que.size()!=0)
            m_msg_que.pop();
    }
public:
    std::queue<CTX_Message> m_msg_que;
    std::queue<CTX_Message> m_ws_msg_que;
    std::queue<CTX_Message> m_height_msg_que;
    unsigned char m_receive_byte_count;
    int     m_signal;
    int     m_receive_msg_count;
    bool    m_allow_send;
    Poco::Timestamp             m_upload_stamp;
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
                system("echo diantai `date` >> err.out");
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
void         CDianTai::checkUploading()
{

    if(m_recv_worker)
    {
        static  Poco::Timestamp now;
        if(!m_recv_worker->m_allow_send)
        {
            now.update ();
            if( (now - m_recv_worker->m_upload_stamp ) > 10000000)
            {
                m_recv_worker->m_allow_send = true;
            }
        }
    }

}
bool         CDianTai::isUploading()
{
    if(m_recv_worker){
        return !m_recv_worker->m_allow_send;
    }
    return false;
}
bool CDianTai::Start(std::string path)
{
  return ( Init(path) && m_recv_worker->start(SerialPort::BAUD_19200));
}
bool CDianTai::Stop()
{
    if(m_recv_worker==NULL)return true;
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
    {
        m_recv_worker->clear();
    }
}
int  CDianTai::getSignalDB()
{
    if(m_recv_worker)
    {
        int tmp = m_recv_worker->m_receive_msg_count;
        m_recv_worker->m_receive_msg_count = 0;
        return tmp;
    }
    return 0;
}
bool CDianTai::haveSignal()
{
    if(m_recv_worker)
        return (m_recv_worker->m_signal>0);
    return false;
}
size_t  CDianTai::CheckMessage(int timeout_ms,int type)
{
    Poco::FastMutex::ScopedLock lock(_mutex);
    if(m_recv_worker==NULL)return 0;
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
    if(m_recv_worker==NULL)return 0;
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
    if(m_recv_worker)
        m_recv_worker->SendData(msg.context);
    return 0;
}
int  CDianTai::SendMessage(std::string& msg, int timeout_ms)
{
    if(m_recv_worker)
        m_recv_worker->SendData(msg);
    return 0;
}
unsigned int CDianTai::GetReceivedCount()
{
    if(m_recv_worker)
    {
        return m_recv_worker->m_receive_byte_count;
    }
    return 0;
}
void         CDianTai::ResetCount()
{
    if(m_recv_worker)
        m_recv_worker->m_receive_byte_count = 0;
}
