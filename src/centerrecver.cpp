#include "centerrecver.h"
#include "Poco/SingletonHolder.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/ThreadPool.h"
#include "Poco/Thread.h"
#include "Poco/Token.h"
#include "Poco/StringTokenizer.h"
#include <iostream>
#include <yateclass.h>
#include <yatengine.h>
#include <Poco/String.h>
#include "ctx2000.h"
using namespace TelEngine;
using Poco::Net::StreamSocket;
using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Thread;
class CenterHandler:public Poco::Runnable{
public:
    CenterHandler(SocketAddress remoteAddr)
    {
        m_addr = remoteAddr;
        m_connflag = false;
    }
    bool start()
    {
        m_thread.start(*this);
        return m_rdyevt.tryWait(1000);
    }
    int send(char* data, int length)
    {
        if(m_connflag)
            return m_socket.sendBytes(data,length);
        return 0;
    }
    /*
上传参数的格式

    */
    bool parseData(char* data, int length)
    {
        Message msg("center.cmd");


        std::string tmp(data);
        Poco::StringTokenizer tk(Poco::trimRight(tmp)," ");
        std::cerr << "recv: " << tk.count() << " param\n";
        for(size_t i = 0; i < tk.count(); i++)
        {
            Poco::StringTokenizer tk_sub(tk[i],":");

            std::cerr << tk_sub[0] << ": ";
            for(size_t j = 1; j < tk_sub.count(); j++)
                std::cerr << tk_sub[j];
            std::cerr << std::endl;

            msg.addParam(tk_sub[0].c_str(),tk_sub[1].c_str());

        }
        if(!TelEngine::Engine::dispatch(msg))
        {
            std::cerr << "Center dispatch Failed\n";
            return false;
        }
        return true;


    }
    bool send_ack_msg(CTX_Message& msg)
    {
        msg.wLength = 0;
        msg.wEvent  = msg_ack;
        m_socket.sendBytes(&msg,CTX_Message_Length);
    }
    bool send_nack_msg(CTX_Message& msg)
    {
        msg.wLength = 0;
        msg.wEvent  = msg_nack;
        m_socket.sendBytes(&msg,CTX_Message_Length);
    }
    void run()
    {
        CTX_Message msg;
        m_rdyevt.set();
        for(;;)
        {
            m_mutex.lock();
            m_connflag = false;
            m_mutex.unlock();
            try{
                m_socket.connect(m_addr);
            }catch(Poco::Exception& e){
                std::cerr << "connect timeout\n";
                Poco::Thread::sleep(5000);

                continue;
            }
            std::cerr << "connect ok\n";
            m_connflag = true;
            while(true)
            {

                int rc = m_socket.receiveBytes(&msg,CTX_Message_Length);
                if(rc == CTX_Message_Length)
                {
                    std::cerr << "left lenght: " << msg.wLength << std::endl;
                    rc = m_socket.receiveBytes(&msg.pchContent,msg.wLength);
                    if(rc == msg.wLength)
                    {
                        if(!msg.isValid())
                        {
                            std::cerr << "checksum failed\n";
                            send_nack_msg(msg);
                        }
                        else if( parseData(msg.pchContent,rc))
                        {

                            send_ack_msg(msg);
                            std::cerr << "send ack\n";
                            //m_socket.sendBytes()
                        }
                    }
                }else if(rc == 0){
                    m_socket.close();
                    std::cerr<<"remote close\n";
                    break;
                }
            }
        }
    }
    StreamSocket  m_socket;
    SocketAddress m_addr;
    Poco::Thread  m_thread;
    volatile bool m_connflag;
    Poco::Event   m_rdyevt;
    //FastMutex     m_mutex;
    Poco::FastMutex         m_mutex;
    //ScopedLockWithUnlock<Mutex> m_mutex;
    char buf[1024];
};

CenterRecver::CenterRecver():
    _handler(NULL)
{

}
CenterRecver& CenterRecver::Get()
{
    static Poco::SingletonHolder<CenterRecver> sh;
    return *sh.get();
}
bool CenterRecver::Start(std::string remoteIp, int port)
{
    _handler = new CenterHandler(SocketAddress(remoteIp,port));
    return _handler->start();
}
int    CenterRecver::Send(char* data, int length)
{
    return _handler->send(data,length);
}
int    CenterRecver::Send(std::string)
{

}
