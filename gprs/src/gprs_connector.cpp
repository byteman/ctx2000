#include "gprs_connector.h"
#include <stdio.h>
#include <Poco/Timespan.h>
#include <Poco/Net/NetException.h>
#include <stdlib.h>
#include <syslog.h>
#ifdef GPRS_DEBUG
//#define GPRS_DBG(fmt...)  fprintf(stderr,fmt)
#define GPRS_DBG(fmt...)    syslog(LOG_ERR,fmt);
#else
#define GPRS_DBG(fmt...) do { } while (0)
#endif
using Poco::Timespan;
using Poco::Net::Socket;
using Poco::Exception;
#define PPPD_DIAL_MS    10000
#define PPPD_KILL_MS    10000
#define DNS2 "119.75.218.45"
bool gprs_connector::exe_cmd(std::string cmd)
{

    std::string cmdout = cmd+" 1>/dev/null 2>&1";

    int ret = system(cmdout.c_str());
    //GPRS_DBG("exe %s ret=%d\n",cmd.c_str(),ret);
    //fprintf(stderr,"exe %s ret=%d\n",cmd.c_str(),ret);
    return (ret!=0)?false:true;
}
gprs_connector::gprs_connector()
{
    m_socket_conn_flag = false;
    m_gprs_conn_flag   = false;
    m_ctrl_func=NULL;
    m_ping_cnt = 0;
}
bool gprs_connector::is_pppd_exist()
{
    return exe_cmd("pidof pppd");
}
bool gprs_connector::is_dial_ok()
{
    return exe_cmd("/sbin/ifconfig ppp0 2>/dev/null | grep inet");
}
/*
  call and wait 60s,wait call ok
*/
bool gprs_connector::call_pppd_dial()
{
    //pppd call gprs 1>/dev/null 2>&1 &
     GPRS_DBG("call_pppd_dial ----> begin\n");
     //exe_cmd("pppd call gprs &");
     exe_cmd("/usr/bin/run-pppd &");
     int cnt = 0;
     while(!is_dial_ok () && (cnt++ < 6)){
        GPRS_DBG("call_pppd_dial check dial ok ---> count=%d\n",cnt);
        Poco::Thread::sleep(PPPD_DIAL_MS);
     }
     if(is_dial_ok ())
     {
         GPRS_DBG("call_pppd_dial ok -----> wait 5s to init ppp0 ok\n");
         Poco::Thread::sleep(PPPD_DIAL_MS);
     }else{
        GPRS_DBG("call_pppd_dial ---> timeout \n");
     }
     return is_dial_ok ();
}
bool gprs_connector::cutoff_pppd()
{
    int count = 20;
    GPRS_DBG("cutoff_pppd ---> begin \n");
    exe_cmd("killall  pppd");
    exe_cmd("killall  chat");
    while(count--){
        GPRS_DBG("cutoff_pppd check state ---> count=%d  \n",count);
        Poco::Thread::sleep(2000);
        if(!is_pppd_exist() && !is_chat_exist ())
            break;
        exe_cmd("killall  pppd");
        exe_cmd("killall  chat");
    }
    if(count==0){
        GPRS_DBG("cutoff_pppd  ---> timeout\n");
    }else{
        GPRS_DBG("cutoff_pppd  ---> ok\n");
    }
    return (!is_pppd_exist() && !is_chat_exist ());
}
bool gprs_connector::can_ping(std::string ip)
{
    //return true;
    char buf[128] = {0,};
    sprintf(buf,"ping -s 1 -c 2 %s ",ip.c_str());
    return exe_cmd(buf);

}

bool gprs_connector::fix_route_table()
{
    return true;
}
void gprs_connector::set_controller(t_dev_control_func func,void* arg)
{
    m_ctrl_func = func;
    m_ctrl_arg  = arg;
}
bool gprs_connector::is_chat_exist()
{
     return exe_cmd("pidof chat");
}
bool gprs_connector::cutoff_chat()
{
    GPRS_DBG("cutoff_chat\n");
    return exe_cmd("killall  chat");
}
#if 0
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void
sig_chld(int signo)
{
        pid_t        pid;
        int        stat;

        while ( (pid = waitpid(-1, &stat, WNOHANG)) >0)
                printf("child %d terminated\n", pid);
        return;
}
#endif
bool gprs_connector::before_run ()
{
    //ignore child quit, and zombile process
    //signal(SIGCLD, SIG_IGN);
    //signal(SIGCHLD, sig_chld);
    set_thread_name ("ctx2000.gprs_ppp");
    return true;
}
void gprs_connector::service()
{
#ifdef PC_DEBUG
    Poco::Thread::sleep (1000);
    GPRS_DBG("gprs_connector sleep\n");
    return;
#endif
    static int dial_retry_counter = 0;
    if(!is_pppd_exist()){
        //不存在pppd就拨号
        m_gprs_conn_flag = false;
        m_dial_cnt = 0;

        if(dial_retry_counter++ > 0)
        {
            if(m_ctrl_func)
            {
                m_ctrl_func(m_ctrl_arg,1);
                Poco::Thread::sleep (30000);
            }
        }


        //when return there,the gprs must be reset ok
        int kill_chat_counter = 0;
        while(is_chat_exist() && !is_pppd_exist() && (kill_chat_counter++ < 5)){

            GPRS_DBG("cutoff_chat----> wait %d \n",kill_chat_counter);
            cutoff_chat ();
            Poco::Thread::sleep (1000);
        }
        if(!is_pppd_exist())
        {
            GPRS_DBG("reset gprs ok,----> wait 20s for gprs modules init \n");
            //Poco::Thread::sleep (20000);
        }
        if(!is_pppd_exist())
        {
            GPRS_DBG("pppd dont exist,call pppd and max wait 60s \n");
            if(!call_pppd_dial())
            {
                GPRS_DBG("xxx pppd call failed\n");
            }else{
                GPRS_DBG("xxx pppd call ok\n");
            }
        }

    }else{
        dial_retry_counter = 0;
        //存在了pppd ，但是是否连接成功否，是否建立了pppd, and is zombile process??
        if(is_dial_ok()){
            //存在了pppd ，dial ok
            m_dial_cnt = 0;
            GPRS_DBG("pppd1 exist and dial_ok,check network by ping %s\n",DNS1.c_str ());
            if(can_ping(DNS1))
            {
                GPRS_DBG("ping %s ok\n",DNS1.c_str ());
                m_gprs_conn_flag = true;
                m_ping_cnt = 0;
                Poco::Thread::sleep(60000);
            }
            else{
                //两个dns都ping不通，就认为gprs不在线
                if( !can_ping(DNS2))
                {
                    GPRS_DBG("ping %s  %s failed  ---> count=%d\n",DNS1.c_str (),DNS2,m_ping_cnt);

                    m_ping_cnt++;
                    if(m_ping_cnt >= 2)
                    {
                        GPRS_DBG("ping failed ----> gprs reset pppd and reset gprs\n");
                        m_gprs_conn_flag = false;
                        if(cutoff_pppd()) //maybe gprs offline,re dial it
                        {//after kill, can not immdiely pppd call
                            //call_pppd_dial();
                            if(m_ctrl_func)
                                m_ctrl_func(m_ctrl_arg,1);
                        }
                        m_ping_cnt = 0;
                    }

                }else{ //其中一个在线，也可以认为gprs连接成功。只是soccket没有连接成功
                    m_gprs_conn_flag = true;
                    m_ping_cnt = 0;
                    Poco::Thread::sleep(60000);
                }
            }

        }else{
#if 0
            //没能建立成功ppp0,but exist pppd,有可能在拨号中，等待60s次,看是否连接成功
            m_gprs_conn_flag = false;
            GPRS_DBG("pppd exist ,but not dial ok--->wait %ds to restart pppd\n",20-m_dial_cnt*10);
            if(m_dial_cnt++ >= 2) //60s
            {
                //3次后等没有连接成功，可能是模块的问题，复位模块并重启pppd进程
                GPRS_DBG("cutoff_pppd\n");
                if(m_ctrl_func)
                    m_ctrl_func(m_ctrl_arg,1);
                if(cutoff_pppd()) //
                {

                    //call_pppd_dial();
                }else{
                    //zobile process
                    //GPRS_DBG("reset gprs ok,----> wait 10s for gprs init \n");
                    //Poco::Thread::sleep (10000);
                    //GPRS_DBG("pppd dont exist,call pppd and max wait 60s \n");

                    //call_pppd_dial();
                }

                m_dial_cnt = 0;
            }
#endif
            GPRS_DBG("pppd exist ,but can not dial ok\n");
            Poco::Thread::sleep(10000);
       }
    }


}
bool gprs_connector::connect(SocketAddress remoteAddr)
{
   try{
       m_socket.close();
       m_socket.connect(remoteAddr);
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
#if 0
    GPRS_DBG("gprs send data:");
    for(U32 i = 0; i < datalen;i++)
    {
        GPRS_DBG("%02x ",data[i]);
    }
    GPRS_DBG("\n");
#endif
    int sendBytes = 0;
    try{
        sendBytes = m_socket.sendBytes(data,datalen);
    }catch(...){
        GPRS_DBG("Send Failed Exception occur!\n");
    }
    return sendBytes;
}
#if 0
size_t gprs_connector::send(TNetBuffer& data)
{
    return 0;
}
#endif
size_t gprs_connector::recv(TNetBuffer& data,size_t datalen, long timeout_ms)
{
    int bRes = 0;
    try{
        Timespan timeout(timeout_ms*1000);
        m_socket.setReceiveTimeout(timeout);

        data.assign(datalen,0);
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
    try{
        if(m_socket.poll(timeout, Socket::SELECT_READ))
        {
            recv(tmp,m_socket.available(),1);
        }
    }catch(...){
        GPRS_DBG("checknetwork excetpions\n");
    }


}
