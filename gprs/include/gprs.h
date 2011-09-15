#ifndef GPRS_H
#define GPRS_H
#include <string>
#include "datatypes.h"
#include <Poco/Thread.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/SharedMemory.h>
#include "Poco/Net/Net.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Thread.h"
#include "Poco/Event.h"
#include <queue>
#include "bythread.h"

#define MAX_QUEUE_DATA_COUNT  20
struct TShared_Data{
    double g_angle;
    double g_dg_height;
    double g_car_dist;
    U8     g_avail;
    U8     g_type;
    U8     g_alarm;
    U8     g_tc_rate;
    double g_angle_x;
    double g_angle_y;
    double g_local_long_arm_len;
    double g_local_Rs;
    double g_wild_speed;
    double g_tc_height;
    double g_up_angle;
    double g_dg_weight;
    double g_rated_weight;
    double g_dg_speed;
}__attribute__ ((packed));
class gprs_connector;
class GPRS_Server;
class gprs:public ByThread
{
public:
    typedef struct tag_tc_data{
        tag_tc_data(U8 type=0)
        {
            m_has_alarm = false;
            m_weight    = 0;
            m_fall      = 0;
            m_type      = type;
        }
       Bool m_has_alarm;    //是否是报警数据
       Dbl  m_weight;       //当前重量
       Dbl  m_max_weight;   //额定重量
       Dbl  m_dist;         //当前幅度
       Dbl  m_min_dist;     //最小幅度
       Dbl  m_max_dist;     //最大幅度
       Dbl  m_dg_height;    //吊钩高度
       Dbl  m_tc_height;    //塔机高度
       Dbl  m_speed;        //风速
       Dbl  m_car_speed;    //小车速度
       Dbl  m_dg_speed;     //起升速度
       Dbl  m_angle;        //回转角度
       Dbl  m_up_angle;     //动臂仰角
       Dbl  m_gradient_x;   //倾斜角度x方向
       Dbl  m_gradient_y;   //倾斜角度y方向
       U8   m_fall;         //倍率
       U8   m_type;         //数据类型
    }tc_data;
    gprs();
    virtual ~gprs();
    static gprs& get();
    bool start(std::string ip, U16 port,std::string dtu_id="001",std::string gpspath="/dev/tts1");
    //服务器端口为56000,IP地址为 :118.123.17.25
    bool connect(std::string ip, U16 port);
    bool connect(Poco::Net::SocketAddress addr);
    bool build_send_gps_data(std::string &gps);
    void set_control_func(void *func,void* arg);
    void build_send_5s_packet();
    /*
    发送塔机数据
    type
            0 : 塔机实时状态信息
            1 : 报警数据
            2 : 塔机起吊记录信息
    */
    bool send_tc_data(int type,tc_data data);

    //15位的dtu_id;
    bool request_login(std::string dtu_id,int &reason);
    bool is_connected();
    bool before_run ();
    void service();
    bool m_login;
private:
    void buildpacket(tc_data& data);
    bool _send_queue_data(tc_data &data);
    Poco::Net::SocketAddress m_addr;
    gprs_connector* m_conn;
    bool m_conn_flag;
    bool m_start_ok;
    std::string m_dtu_id;
    std::queue<tc_data> m_tcdata_queue;
    Poco::SharedMemory m_shared_data;
    GPRS_Server* server;

};

#endif // GPRS_H
