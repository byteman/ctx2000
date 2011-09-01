#ifndef GPRS_CONNECTOR_H
#define GPRS_CONNECTOR_H
#include <Poco/Thread.h>
#include <Poco/Net/StreamSocket.h>
#include <string>
#include <vector>
#include "datatypes.h"
#include "bythread.h"
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;

class gprs_connector:public ByThread
{
public:
    typedef std::vector<U8> TNetBuffer;
    gprs_connector();
    Bool connect(SocketAddress remoteAddr);
    U32  send(U8* data, U32 datalen);
    U32  recv(U8* data, U32 datalen, long timeout=0);
    size_t send(TNetBuffer& data);
    size_t recv(TNetBuffer& data,size_t datalen, long timeout_ms=0);
    bool is_socket_connectd();
    bool is_gprs_conneted();
    void checknetwork();
    virtual void service();
private:
    bool exe_cmd(std::string cmd);
    bool is_pppd_exist();
    bool is_dial_ok();
    bool call_pppd_dial();
    bool cutoff_pppd();
    bool can_ping(std::string ip);
    bool fix_route_table();
    Poco::Net::StreamSocket  m_socket;
    SocketAddress m_addr;
    Poco::Thread  m_thread;
    bool m_socket_conn_flag;
    bool m_gprs_conn_flag;
    int  m_dial_cnt;
    int  m_ping_cnt;
};

#endif // GPRS_IMPL_H
