#include "gprs_connector.h"
#include <stdio.h>
#include <Poco/Timespan.h>
#include <Poco/Net/NetException.h>
#include <stdlib.h>
#ifdef GPRS_DEBUG
#define GPRS_DBG(fmt...) fprintf(stderr,fmt);
#else
#define GPRS_DBG(fmt...) do { } while (0)
#endif
using Poco::Timespan;
using Poco::Net::Socket;
using Poco::Exception;
#define PPPD_DIAL_MS    10000
#define PPPD_KILL_MS    10000
#define DNS1 "119.75.218.45"
#define DNS2 "202.202.96.33"
bool gprs_connector::exe_cmd(std::string cmd)
{

    std::string cmdout = cmd+" 1>/dev/null 2>&1";
    int ret = system(cmdout.c_str());
    GPRS_DBG("exe %s ret=%d\n",cmd.c_str(),ret);
    return (ret!=0)?false:true;
}
gprs_connector::gprs_connector()
{
    m_socket_conn_flag = false;
    m_gprs_conn_flag   = false;
}
bool gprs_connector::is_pppd_exist()
{
    return exe_cmd("pidof pppd");
}
bool gprs_connector::is_dial_ok()
{
    return exe_cmd("ifconfig ppp0");
}
bool gprs_connector::call_pppd_dial()
{
    //pppd call gprs 1>/dev/null 2>&1 &
     exe_cmd("pppd call gprs &");
     Poco::Thread::sleep(PPPD_DIAL_MS);
     return true;
}
bool gprs_connector::cutoff_pppd()
{
    int count = 10;
    exe_cmd("killall pppd");
    while(count--){
        Poco::Thread::sleep(2000);
        if(!is_pppd_exist())
            break;
    }
    return (count >= 0);
}
bool gprs_connector::can_ping(std::string ip)
{
    return true;
    char buf[128] = {0,};
    sprintf(buf,"ping -s 1 -c 2 %s ",ip.c_str());
    return exe_cmd(buf);

}
void gprs_connector::service()
{
#ifdef PC_DEBUG
    Poco::Thread::sleep (1000);
    GPRS_DBG("gprs_connector sleep\n");
    return;
#endif
    if(!is_pppd_exist()){
        //不存在pppd就拨号
        GPRS_DBG("pppd dont exist,call pppd\n");
        call_pppd_dial();
    }else{
        //存在了，但是是否连接成功否，是否建立了pppd
        if(is_dial_ok()){
            GPRS_DBG("pppd exist,dial_ok\n");
            if(can_ping(DNS1))
            {
                GPRS_DBG("ping %s ok\n",DNS1);
                m_gprs_conn_flag = true;
            }
            else{
                //两个dns都ping不通，就认为gprs不在线
                if( !can_ping(DNS2))
                {
                    GPRS_DBG("ping %s  %s failed gprs off\n",DNS1,DNS2);
                    m_gprs_conn_flag = false;
                    if(cutoff_pppd())
                    {
                        call_pppd_dial();
                    }
                }else{ //其中一个在线，也可以认为gprs连接成功。只是soccket没有连接成功
                    m_gprs_conn_flag = true;
                }
            }
            Poco::Thread::sleep(10000);
        }else{//没能建立成功,有可能在拨号中，等待3次,看是否连接成功
            m_gprs_conn_flag = false;
            if(m_dial_cnt++ > 3)
            {
                //3次后等没有连接成功，可能是模块的问题，复位模块并重启pppd进程
                if(cutoff_pppd())
                {
                    call_pppd_dial();
                }

                m_dial_cnt = 0;
            }
            Poco::Thread::sleep(10000);
       }
    }


}
bool gprs_connector::connect(SocketAddress remoteAddr)
{
   try{
        GPRS_DBG("Con1\n");
       m_socket.close();
       GPRS_DBG("Con2\n");
       m_socket.connect(remoteAddr);
          GPRS_DBG("Con3\n");
       m_addr = remoteAddr;
   }catch(Poco::Net::ConnectionRefusedException& e){
        GPRS_DBG("ConnectionRefusedException [%s] : %s\n",e.what(),e.displayText().c_str());
        return false;
   }catch(Poco::Net::NetException& e){
        GPRS_DBG("connect error [%s] : %s\n",e.what(),e.displayText().c_str());
        return false;
   }catch(Poco::TimeoutException& e){
       GPRS_DBG("connect timeout [%s] : %s\n",e.what(),e.displayText().c_str());
       return false;
   }catch(...){
        GPRS_DBG("gprs_connector::connect unkown error\n");
        return false;
    }
   m_socket_conn_flag = true;
   return true;
}
U32  gprs_connector::send(U8* data, U32 datalen)
{
    return m_socket.sendBytes(data,datalen);
}
size_t gprs_connector::send(TNetBuffer& data)
{
    return 0;
}
size_t gprs_connector::recv(TNetBuffer& data,size_t datalen, long timeout_ms)
{

    Timespan timeout(timeout_ms*1000);
    m_socket.setReceiveTimeout(timeout);

    data.assign(datalen+1,0);
    int bRes = 0;
    try{

        bRes = m_socket.receiveBytes(data.data(),datalen);
    }catch(Poco::TimeoutException& e){

        GPRS_DBG("recv timeout res=%d\n",bRes);
        return 0;
    }catch(...){
        GPRS_DBG("recv unkown error\n");
        return 0;
    }
    if(bRes==0)
    {
        m_socket_conn_flag = false;
    }
#ifdef GPRS_DEBUG
    GPRS_DBG("recv data = ");
    for(int i = 0; i < bRes; i++)
    {
        GPRS_DBG("%02x ",data.at(i));
    }
    GPRS_DBG("\n");
#endif

    return bRes;
}
bool gprs_connector::is_socket_connectd()
{
    if(!m_socket_conn_flag) return false;
    checknetwork();
    return m_socket_conn_flag;
}
bool gprs_connector::is_gprs_conneted()
{
#ifdef PC_DEBUG
    return true;
#else
    return m_gprs_conn_flag;
#endif
}
void gprs_connector::checknetwork()
{
    TNetBuffer tmp;
    Timespan timeout(1000000);
    if(m_socket.poll(timeout, Socket::SELECT_READ))
    {
        recv(tmp,m_socket.available(),1);
    }

}
