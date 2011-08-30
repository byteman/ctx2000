#ifndef GPS_H
#define GPS_H
#include "bythread.h"
#include <SerialPort.h>
#include <Poco/Event.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
using namespace LibSerial;
class gps:public ByThread
{
public:
    gps();
    static gps& get();
    bool start(std::string path);
    void service ();
    bool before_run ();
    bool enable_other_output();
    std::string readline_msg(int timeout);
    std::string poll_gprmc_msg(bool force=false);
    bool set_ack_time(unsigned char s);
    bool is_active();
    bool gprmc_is_valid(std::string msg);
    unsigned short check_sum(unsigned char cmdbuf[] ,int len);
private:
    SerialPort *com;
    std::string gprmc_msg,prev_valid_gprmc_msg;
    int   m_timeout;
    bool  m_is_valid;
    int   m_send_read_gprmc_msg_counter;
    Poco::Event evt_read_msg;
    Poco::Timestamp now,pre_update;

};

#endif
