#include "gprs.h"
//#include "comdata.h"
#include <Poco/SingletonHolder.h>
#include "gps.h"
#include <arpa/inet.h>
#include "gprs_connector.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "Poco/Net/SocketAddress.h"
#include "Poco/Timespan.h"
#include <iostream>
#include "gpio.h"

using Poco::Net::Socket;
using Poco::Net::DatagramSocket;
using Poco::Net::SocketAddress;
using Poco::Net::IPAddress;
static Poco::FastMutex _mutex;

#ifdef GPRS_DEBUG
//#define GPRS_DBG(fmt...) fprintf(stderr,fmt);
#define GPRS_DBG(fmt...)    syslog(LOG_ERR,fmt);
#else
#define GPRS_DBG(fmt...) do { } while (0)
#endif
#define MAX_GPS_BUF 128
struct TGPSMessage{
    TGPSMessage()
    {
        memset(gprmc,0,MAX_GPS_BUF);
    }
    U8   header;
    U8   packet_no;
    U8   packet_len;
    char gprmc[MAX_GPS_BUF];
}__attribute__ ((packed));;
struct TNetMessage{
    U8  header;
    U8  packet_no;
    U8  packet_len;
    U8  vendor;
    U8  version;
    U8  alarm;
    U16 count;
    U16 weight;
    U16 max_weight;
    U8  fall;
    U16 dist;
    U16 min_dist;
    U16 max_dist;
    U16 dg_height;
    U16 tc_height;
    U16 speed;
    U16 car_speed;
    U16 dg_speed;
    U16 angle;
    U16 m_gradient_x;
    U16 m_gradient_y;
}__attribute__ ((packed));
struct TNetMessageWetting{
    U8  header;
    U8  packet_no;
    U8  packet_len;
    U8  alarm;
    U16 count;
    U16 dist;
    U16 max_weight;   //起吊过程中最大重量
    U8  fall;  //倍率
    U16 rated_weight; //额定重量
    U16 speed; //风速
}__attribute__ ((packed));
static TNetMessage          tc_msg;
static TNetMessageWetting   tc_wet_msg;
static U16         tc_send_count;
static U8          tc_packet_no;

class GPRS_Server: public Poco::Runnable
        /// A simple sequential UDP echo server.
{
public:
        GPRS_Server();
                /// Creates the UDPEchoServer.

        GPRS_Server(const Poco::Net::SocketAddress& sa);
                /// Creates the UDPEchoServer and binds it to
                /// the given address.

        ~GPRS_Server();
                /// Destroys the UDPEchoServer.

        Poco::UInt16 port() const;
                /// Returns the port the echo server is
                /// listening on.

        Poco::Net::SocketAddress address() const;
                /// Returns the address of the server.

        void run();
                /// Does the work.
        void sendData(TShared_Data& d);
        void SendAlarmData(TShared_Data& d);
        void SendWetRecord(TShared_Data& d);

private:
        Poco::Net::DatagramSocket _socket;
        Poco::Thread _thread;
        Poco::Event  _ready;
        bool         _stop;
};
GPRS_Server::GPRS_Server():
        _thread("UDPEchoServer"),
        _stop(false)
{
        _socket.bind(SocketAddress(), true);
        _thread.start(*this);
        _ready.wait();
}


GPRS_Server::GPRS_Server(const SocketAddress& sa):
        _thread("UDPEchoServer"),
        _stop(false)
{

        _socket.bind(sa, true);
        _thread.start(*this);
        _ready.wait();
}


GPRS_Server::~GPRS_Server()
{
        _stop = true;
        _thread.join();
}


Poco::UInt16 GPRS_Server::port() const
{
        return _socket.address().port();
}
void      GPRS_Server::SendAlarmData(TShared_Data& d)
{
#if 1
    gprs::tc_data data;
    data.m_has_alarm = d.g_alarm?true:false;
    data.m_angle     = d.g_angle;
    data.m_car_speed = 0;
    data.m_dg_height = d.g_dg_height;
    data.m_dist      = d.g_car_dist;
    data.m_fall      = d.g_tc_rate;
    data.m_gradient_x= d.g_angle_x;
    data.m_gradient_y= d.g_angle_y;
    data.m_max_dist  = d.g_local_long_arm_len;
    data.m_min_dist  = d.g_local_Rs;
    data.m_speed     = d.g_wild_speed;
    data.m_tc_height = d.g_tc_height;
    data.m_up_angle  = d.g_up_angle;
    data.m_weight    = d.g_dg_weight;
    data.m_max_weight= d.g_rated_weight;
    data.m_dg_speed  =  0;

    if(!gprs::get().send_tc_data(0,data))
    {
        GPRS_DBG("send_alarm_tc_data failed\n");
    }
#endif


}
void   GPRS_Server::SendWetRecord(TShared_Data& d)
{
#if 1
    gprs::tc_data data;
    data.m_has_alarm = d.g_alarm?true:false;
    data.m_angle     = d.g_angle;
    data.m_car_speed = 0;
    data.m_dg_height = d.g_dg_height;
    data.m_fall      = d.g_tc_rate;
    data.m_dist      = d.g_car_dist;
    data.m_weight    = d.g_dg_weight;
    data.m_max_weight= d.g_rated_weight;
    gprs::get ().send_tc_data (1,data);
#endif
}
void GPRS_Server::sendData(TShared_Data& d)
{
    if(d.g_avail == 'a')
    {
        //fprintf(stderr,"fall=%d\n",d.g_tc_rate);
        if(d.g_type == 0)
        {
            SendAlarmData(d);
        }else if(d.g_type == 1){
            SendWetRecord(d);
        }
    }
}
void GPRS_Server::run()
{
    GPRS_DBG("GPRS_Server run\n");
        _ready.set();
        Poco::Timespan span(250000);
        while (!_stop)
        {
                if (_socket.poll(span, Socket::SELECT_READ))
                {

                        try
                        {
                                TShared_Data buffer;
                                SocketAddress sender;
                                int n = _socket.receiveFrom((U8*)&buffer, sizeof(buffer), sender);
                                //GPRS_DBG("have data size=%d %d\n",n,sizeof(buffer));
                                if(n == sizeof(buffer))
                                {
                                    //GPRS_DBG("receved data\n");
                                    if(gprs::get ().m_login)
                                        sendData(buffer);
                                }
                        }
                        catch (Poco::Exception& exc)
                        {
                            GPRS_DBG("GPRS-Server: %s\n" , exc.displayText().c_str() );
                        }
                }
        }
}


SocketAddress GPRS_Server::address() const
{
        return _socket.address();
}
gprs& gprs::get()
{
    static Poco::SingletonHolder<gprs> sh;
    return *sh.get();
}
gprs::gprs():m_conn(NULL)
{
    m_conn = new gprs_connector();
    m_start_ok = false;
    server = NULL;
    m_login = false;
}
gprs::~gprs()
{
    if(m_conn)
        delete m_conn;
}
CGpio gpio;
bool dev_control_func(void* arg,int state)
{
    GPRS_DBG("reset gprs\n");
    if(state == 1)
    {
        gpio.Output (12,IO_LOW_LEVEL);
        gpio.Output (13,IO_LOW_LEVEL);
        Poco::Thread::sleep (1000);
        gpio.Output (12,IO_HIGH_LEVEL);
        gpio.Output (13,IO_HIGH_LEVEL);
        Poco::Thread::sleep (1000);
    }

    return true;
}
void gprs::set_control_func(void *func,void* arg)
{
    if(m_conn){
        m_conn->set_controller((t_dev_control_func)func,arg);
    }
}
bool gprs::start(std::string ip, U16 port,std::string dtu_id,std::string gpspath)
{
#if 1
    if(!gps::get ().start (gpspath))
    {
        GPRS_DBG("gps start failed\n");
    }
#endif
    GPRS_DBG("gprs connect to %s %d with id=%s\n",ip.c_str (),port,dtu_id.c_str ());
    m_addr=SocketAddress(ip,port);
    m_dtu_id = dtu_id;

    gpio.Init (8,8,false);
    set_control_func((void*)dev_control_func,this);
    dev_control_func(this,1);
    GPRS_DBG("reset wait 30s begin------\n");
    Poco::Thread::sleep (30000);
    GPRS_DBG("reset wait 30s ok------\n");

    if(m_conn){
        bool bRes = m_conn->start();
        if(!bRes){
            GPRS_DBG("gprs connector start failed\n");
        }
    }
#if 0
    m_shared_data = Poco::SharedMemory("gprs",4096,Poco::SharedMemory::AM_WRITE);
    assert(m_shared_data.begin () < m_shared_data.end ());
    assert(m_shared_data.end() - m_shared_data.begin () == 4096);
#endif
    if(server==NULL){
        server = new GPRS_Server(SocketAddress("localhost", 3389));
        assert(server);
    }


    return ByThread::start();


}
bool gprs::before_run ()
{
    set_thread_name ("ctx2000.gprs");
    return true;
}
bool gprs::_send_queue_data(tc_data &data)
{
    if(data.m_type==0)
    {

    }else if(data.m_type==1){

    }else if(data.m_type==2){

    }
    return false;
}

bool gprs::build_send_gps_data(std::string &gps)
{
    if(gps.length () >= MAX_GPS_BUF){
        GPRS_DBG("gps len %d > max_gps_buf[%d]\n",gps.length (),MAX_GPS_BUF);
        return false;
    }
    TGPSMessage msg;
    msg.header = 0x8;
    msg.packet_no = 0;
    msg.packet_len = gps.length ();
    gps.copy (msg.gprmc,gps.length ());
    if(m_conn)
        m_conn->send ((U8*)&msg,3+gps.length ());
    return true;
}
void gprs::build_send_5s_packet()
{
#if 0
    gprs::tc_data data;

    TShared_Data* share = (TShared_Data*)m_shared_data.begin ();
    if(!share || (share->g_avail != 'a')) return;

    data.m_has_alarm    = false;
    data.m_angle        = share->g_angle;
    data.m_car_speed    = 0;
    data.m_dg_height    = share->g_dg_height;
    data.m_dist         = share->g_car_dist;
    data.m_fall         = share->g_tc_rate;
    data.m_gradient_x   = share->g_angle_x;
    data.m_gradient_y   = share->g_angle_y;
    data.m_max_dist     = share->g_local_long_arm_len;
    data.m_min_dist     = share->g_local_Rs;
    data.m_speed        = share->g_wild_speed;
    data.m_tc_height    = share->g_tc_height;
    data.m_up_angle     = share->g_up_angle;
    data.m_weight       = share->g_dg_weight;
    data.m_max_weight   = share->g_rated_weight;
    data.m_dg_speed     = 0;
    if(!gprs::get().send_tc_data(0,data))
    {
        GPRS_DBG("send_tc_data failed\n");
    }
    share->g_avail = 0;
#endif
}
void gprs::service()
{
    GPRS_DBG("begin connect\n");
    bool bRes  = false;
    m_start_ok = true;
gprs_connect:
    m_login    = false;
    while(1){
        if( (m_conn) && m_conn->is_gprs_conneted())
            break;
        //GPRS_DBG("wait gprs conneted\n");
        Poco::Thread::sleep(5000);
    }
    GPRS_DBG("gprs pppd dial ok\n");
    while(1)
    {
        bRes = gprs::get().connect(m_addr);
        while(bRes==false){
             GPRS_DBG("connect failed\n");
             Poco::Thread::sleep(3000);
             if(!m_conn->is_gprs_conneted()){
                    goto gprs_connect;
             }
             bRes = gprs::get().connect(m_addr);
        }
        GPRS_DBG("connect ok\n");

        GPRS_DBG("send login packet\n");
        int fail_reason = 0;
        m_login = false;
        while(!gprs::get().request_login(m_dtu_id,fail_reason))
        {
             GPRS_DBG("**********request_login failed=%d\n",fail_reason);
             if(fail_reason==-1){ //对方关闭了连接
                 break;
             }
             if(!m_conn->is_gprs_conneted()){
                    goto gprs_connect;
             }
             Poco::Thread::sleep(5000);
        }
        if(fail_reason!=0) continue;
        GPRS_DBG("=============recv login packet ack ok\n");
        m_login = true;
        while(1)
        {
             static int count = 0;

             if(!m_conn->is_gprs_conneted()){
                    goto gprs_connect;
             }
#if 0
             count++;
             if( (count % 5) == 0) //5s
             {
                build_send_5s_packet();
             }
#endif
             if( (count % 10) == 0) //10s
             {
                 std::string gps_info = gps::get ().poll_gprmc_msg ();
                 if(gps_info.length () != 0)
                    build_send_gps_data(gps_info);
             }

             //if( (count % 2) == 0) //1s
             {
                 //GPRS_DBG("run %d\n",m_tcdata_queue.size());
                 if(m_tcdata_queue.size() > 0){
                     tc_data d;
                     {
                         Poco::FastMutex::ScopedLock lock(_mutex);
                         d = m_tcdata_queue.front();
                         m_tcdata_queue.pop();
                     }
                     buildpacket(d);


                 }else{

                 }
             }
             if(!gprs::get().is_connected()) //wait 1s
             {
                 GPRS_DBG("disconnected\n");
                 break;
             }
             //Poco::Thread::sleep(500);
        }
    }
}
bool gprs::connect(Poco::Net::SocketAddress addr)
{
    if(m_conn){
        return m_conn->connect(addr);
    }
    return false;
}
bool gprs::connect(std::string ip, U16 port)
{

    return false;
}
//0x04 0x00 0x2f + 15位ID号
bool gprs::request_login(std::string dtu_id,int &reason)
{
    reason = 0;
    char login_packet[20]={0x04,0x0,0x2F,0,};

    int len = (dtu_id.length()>15)?15:dtu_id.length();
    dtu_id.copy(login_packet+3,len,0);
#ifdef GPRS_DEBUG
    GPRS_DBG("login packet = ");
    for(int i = 0; i < 18; i++)
    {
        GPRS_DBG("%02x ",login_packet[i]);
    }
    GPRS_DBG("\n");
#endif
    m_conn->send((U8*)login_packet,18);
    U8 login_ok_packet[] = {0x07,0x00 ,0x0B ,0x00 ,0xB4 ,0x00 ,0x3C ,0x00 ,0x00 ,0x00 ,0x00};
    gprs_connector::TNetBuffer recvBuf;
    //等待2s回应数据
    recvBuf.clear ();
    int recvLen = m_conn->recv(recvBuf,11,2000);
    GPRS_DBG("send login request packet recvLen=%d\n",recvLen);
    if(11 == recvLen && recvBuf.size () >= 11)
    {
        size_t i = 0;
        for(i = 0; i < 11; i++)
        {

            if(login_ok_packet[i] != recvBuf.at(i))
            {
                reason = -2;
                break;
            }
        }
        if(i==recvBuf.size())
        {
            reason = 0;
            return true;
        }

    }else{
        reason = -3;
    }
    if(!m_conn->is_socket_connectd())reason = -1;

    return false;
}
void gprs::buildpacket(tc_data& data)
{
    if(data.m_type==0){ //work info
        tc_msg.vendor  = 0x2; // 0:reserve 2: hitch
        tc_msg.version = 0x1; // version v1.0
        tc_msg.alarm        = data.m_has_alarm?0xAA:0x55;
        tc_msg.angle        = (U16)(data.m_angle*10+0.5);
        tc_msg.car_speed    = 0;
        tc_msg.count        = tc_send_count++;    
        tc_msg.dg_height    = (U16)(data.m_dg_height*100);
        tc_msg.dg_speed     = 0; 
        tc_msg.dist         = (U16)(data.m_dist*100+0.5);
        tc_msg.fall         =   data.m_fall;
        tc_msg.weight     = (U16)(data.m_weight*100+0.5);
        tc_msg.max_weight = (U16)(data.m_max_weight*100);
        tc_msg.speed      = (U16)(data.m_speed*100);
        tc_msg.tc_height  = (U16)(data.m_tc_height*100);
        tc_msg.max_dist   = (U16)(data.m_max_dist*100);
        tc_msg.min_dist   = (U16)(data.m_min_dist*100);
        tc_msg.header     = 0x8;
        tc_msg.packet_no  = tc_packet_no++;
        tc_msg.packet_len = sizeof(TNetMessage);

        tc_msg.min_dist   = htons(tc_msg.min_dist);
        tc_msg.max_dist   = htons(tc_msg.max_dist);
        tc_msg.tc_height  = htons(tc_msg.tc_height);
        tc_msg.speed      = htons(tc_msg.speed);
        tc_msg.max_weight = htons(tc_msg.max_weight);
        tc_msg.dist         = htons(tc_msg.dist);
        tc_msg.dg_speed     = htons(tc_msg.dg_speed);
        tc_msg.dg_height    = htons(tc_msg.dg_height);
        tc_msg.count        = htons(tc_msg.count);
        tc_msg.car_speed    = htons(tc_msg.car_speed);
        tc_msg.angle        = htons(tc_msg.angle);
        tc_msg.weight       = htons(tc_msg.weight);
        if(m_conn){
            m_conn->send((U8*)&tc_msg,sizeof(TNetMessage));
        }
    }
    else if(data.m_type==1){ //qi diao ji lu
        tc_wet_msg.alarm        = data.m_has_alarm?0x24:0x0A;
        tc_wet_msg.count        = tc_send_count++;
        tc_wet_msg.dist         = (U16)(data.m_dist*100+0.5);
        tc_wet_msg.fall         = data.m_fall;
        tc_wet_msg.max_weight   = (U16)(data.m_weight*100+0.5);
        tc_wet_msg.rated_weight = (U16)(data.m_max_weight*100);
        tc_wet_msg.header       = 0x8;
        tc_wet_msg.packet_no    = tc_packet_no++;
        tc_wet_msg.packet_len   = sizeof(TNetMessageWetting);
        tc_wet_msg.count        = htons(tc_wet_msg.count);
        tc_wet_msg.dist         = htons(tc_wet_msg.dist);
        tc_wet_msg.max_weight   = htons(tc_wet_msg.max_weight);
        tc_wet_msg.rated_weight = htons(tc_wet_msg.rated_weight);
        if(m_conn){
            m_conn->send((U8*)&tc_wet_msg,sizeof(TNetMessageWetting));
        }
    }else if(data.m_type==2){

    }
    GPRS_DBG("send tcdata type=%d count=%d ok\n",data.m_type,tc_send_count);

}

bool gprs::send_tc_data(int type,tc_data data)
{
    if(!m_start_ok) return false;
    if(m_tcdata_queue.size() < MAX_QUEUE_DATA_COUNT)
    {
        Poco::FastMutex::ScopedLock lock(_mutex);
        data.m_type = type;  
        m_tcdata_queue.push(data);
        return true;
    }

    GPRS_DBG("send_tc_data m_tcdata_queue size too large %d\n",MAX_QUEUE_DATA_COUNT);
    return false;
}
bool gprs::is_connected()
{
    return m_conn->is_socket_connectd();
}
