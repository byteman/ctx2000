#include "gps.h"
#include <Poco/SingletonHolder.h>
#include <Poco/Mutex.h>

#include <Poco/StringTokenizer.h>
#include <Poco/String.h>
#include <stdio.h>
#define GPS_DEBUG
#ifdef GPS_DEBUG
#define GPS_DBG(fmt...) fprintf(stderr,fmt);
#else
#define GPS_DBG(fmt...) do { } while (0)
#endif
#define MAX_CNT  6
#define MAX_LEN  11

static Poco::FastMutex _mutex;
static unsigned char cmd_gps[MAX_CNT][MAX_LEN] = {
    {0xB5,0x62,0x6,0x1,0x3,0x0,0xF0,0x0,0,0xFA,0x0F}, //GGA
    {0xB5,0x62,0x6,0x1,0x3,0x0,0xF0,0x1,0,0xFB,0x11}, //GLL
    {0xB5,0x62,0x6,0x1,0x3,0x0,0xF0,0x2,0,0xFC,0x13}, //GSA
    {0xB5,0x62,0x6,0x1,0x3,0x0,0xF0,0x3,0,0xFD,0x15}, //GSV
    {0xB5,0x62,0x6,0x1,0x3,0x0,0xF0,0x4,0,0xFE,0x17}, //RMC
    {0xB5,0x62,0x6,0x1,0x3,0x0,0xF0,0x5,0,0xFF,0x19}, //VTG
};
static unsigned char cmd_gps_ack[10] = {0xB5 ,0x62 ,0x05 ,0x01 ,0x02 ,0x00 ,0x06 ,0x01 ,0x0F ,0x38};
static std::string cmd_gprmc = "$EIGPQ,RMC*3A\r\n";
gps::gps()
{
    com = NULL;
    gprmc_msg.clear ();
    m_is_valid = false;
    prev_valid_gprmc_msg.clear ();
    now.update ();
    pre_update=now;
}
gps& gps::get()
{
    static Poco::SingletonHolder<gps> sh;
    return *sh.get ();
}
bool gps::is_active()
{
    return m_is_valid;
}
bool gps::gprmc_is_valid(std::string msg)
{
    Poco::StringTokenizer token(msg,",");
    //GPS_DBG("count = %d\n",token.count ());
    if(token.count () == 13 )
    {
        if(token[2] == "A" && token[0] == "$GPRMC")
        {
            return true;
        }
    }
    return false;
}
std::string gps::readline_msg(int timeout)
{
    if(com){
        Poco::FastMutex::ScopedLock lock(_mutex);
        SerialPort::DataBuffer buf;
        std::string tmp = "";
        buf.clear ();

        try{
            tmp = com->ReadLine (timeout);
        }catch(SerialPort::ReadTimeout& e){
            return "";
        }
        return tmp;
    }
    return "";
}
std::string gps::poll_gprmc_msg(bool force)
{
    if(com)
    {
        now.update ();
        if ( (now-pre_update) > 10000000 )
        {
            m_is_valid = false;
        }
        if(force){
            com->Write (cmd_gprmc);
            m_send_read_gprmc_msg_counter++;
            evt_read_msg.tryWait (1000);
        }

        if(gprmc_is_valid (gprmc_msg))
        {
            prev_valid_gprmc_msg = gprmc_msg;
        }
    }

    return prev_valid_gprmc_msg;

}
bool gps::before_run ()
{
     //enable_other_output();
     return true;
}
bool gps::enable_other_output()
{
    int cur_cmd = 0;
    SerialPort::DataBuffer buf;
    //com->Read (buf);
    while(cur_cmd < MAX_CNT){
        if(com){
            com->Write (cmd_gps[cur_cmd],MAX_LEN);
            buf.clear ();
            try{
                com->Read (buf,10,1000);
            }catch(SerialPort::ReadTimeout& e){
                continue;
            }
            size_t i = 0;
            for(i = 0; i < 10; i++)
            {
                if(buf.at (i) != cmd_gps_ack[i])
                    break;
            }
            if(i != 10) continue;

            ++cur_cmd;

        }
    }
    return (cur_cmd == MAX_CNT) ;

}
bool gps::start(std::string path)
{
    fprintf(stderr,"gps open %s\n",path.c_str ());
    if(com == NULL)
    {
        com = new SerialPort(path);
    }
    try{
        com->Open (SerialPort::BAUD_9600);
    }catch(...){
        GPS_DBG("open %s failed\n",path.c_str ());
        return false;
    }
    return ByThread::start ();
}
unsigned short gps::check_sum(unsigned char cmdbuf[] ,int len)
{
    unsigned char chk1=0,chk2=0;

    for(int i = 0; i < len; i++)
    {
        chk1 += cmdbuf[i];
        chk2 += chk1;
    }
    return ((chk1<<8)+chk2);
}
bool gps::set_ack_time(unsigned char s)
{
    cmd_gps[4][8] = s;
    unsigned short sum = check_sum(cmd_gps[4]+2,7);
    cmd_gps[4][9] = sum>>8;
    cmd_gps[4][10] = sum&0xff;
    com->Write (cmd_gps[4],MAX_LEN);
}
void gps::service ()
{

    static int  repeat_cnt = 0;
    //while(com->IsDataAvailable ()){

        std::string msg = readline_msg (0);
#if 0
        for(size_t i = 0; i < msg.length (); i++)
            GPS_DBG("%x ",msg.at(i));
        GPS_DBG("\n");
#endif
        if(msg.length () >2 && ((unsigned char)msg.at (0) == 0xB5))
        {
            GPS_DBG("readline");
            for(size_t i = 0; i < msg.length (); i++)
                GPS_DBG("%x ",(unsigned char)msg.at(i));
            GPS_DBG("\n");
        }else
            GPS_DBG("readline %s\n",msg.c_str ());
        if(msg.find ("GPGGA") != std::string::npos){
            com->Write (cmd_gps[0],MAX_LEN);
        }else if(msg.find ("GPGLL") != std::string::npos){
            com->Write (cmd_gps[1],MAX_LEN);
        }else if(msg.find ("GPGSA") != std::string::npos){
            com->Write (cmd_gps[2],MAX_LEN);
        }else if(msg.find ("GPGSV") != std::string::npos){
            com->Write (cmd_gps[3],MAX_LEN);
        }else if(msg.find ("GPVTG") != std::string::npos){
            com->Write (cmd_gps[5],MAX_LEN);
        }else if(msg.find ("GPRMC") != std::string::npos){
            gprmc_msg  = msg;
            m_is_valid = true;
            pre_update.update ();

        }
    //}

    Poco::Timestamp now;
    if( (now-pre_update) < 2000000){
        if(repeat_cnt++ > 5)
        {
            set_ack_time (5);
            repeat_cnt = 0;
        }
    }else{
         repeat_cnt = 0;
    }
}
