#ifndef __SOCKET_H
#define __SOCKET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define __DEBUG

#ifdef __DEBUG
#ifndef ASSERT
#define ASSERT(pararm) assert(pararm)
#endif
#define  DPRINTF(x...)     printf(x)
#else
#ifndef ASSERT
#define ASSERT(pararm)
#endif
#define  DPRINTF(x...)     
#endif

#define  RECVBUFLEN 5
#define  SENDBUFLEN 50

#define  DATAMAXLEN (2*1024*1024)
#if 0
typedef struct tag_Message
{
    unsigned int     dwVersion;          // 消息版本号
    unsigned int     dwSerial;           // 消息流水号
    unsigned int     dwDestIp;           // 目的Ip地址
    unsigned int     dwSrcIp;            // 源Ip地址
    unsigned short   wDestPort;          // 目的端口    
    unsigned short   wSrcPort;           // 源端口
    unsigned short   wDestStreamId;      // 目的连接标志
    unsigned short   wSrcStreamId;       // 源连接标志
    unsigned short   wType;              // 消息类型
    unsigned short   wEvent;             // 消息事件号
    unsigned int     dwLength;           // 消息体长度，achContent长度
    unsigned int     dwReserve;          // 保留
    char*            pchContent;         // 消息体
}TMessage;
#else
#include "../main/dt_gprshead.h"
#endif

#define TMessage_length (sizeof(TMessage) - 4)

typedef int (*register_func)(TMessage *msg);


enum SOCKETMSG{
    WM_MESSAGE_SOCKET_ONLINE = 500,
    WM_MESSAGE_SOCKET_OFFLINE
};

enum LINKSTATUS{
    DISCONNECT = 0,
    CONNECT
};

class Socket{
public:
	Socket();
	~Socket();
    int  NmsService_Addr(unsigned int ipaddr, unsigned short port);
    void Socket_Enable(bool enable);
    int  Send_Msg(TMessage *msg);
    void Register_Callback_Fun(register_func func);
    bool Get_Link_Status(void);
    void ConnectEthSta(bool online);
    bool DataRecvSta;
    bool isconnected;

private:
    bool nms_enable;
    unsigned int addr_ip;
    unsigned short net_port;

    int sock_fd;
	fd_set readfds;
    
	struct sockaddr_in addr;
    
	TMessage *recvmsg;
    
    //TMessage *MsgRecvBuf[RECVBUFLEN];
    TMessage *MsgSendBuf[SENDBUFLEN];

    register_func MsgCallbackFun;

    pthread_mutex_t connected_mutex;
    pthread_mutex_t change_ip_port;
    pthread_mutex_t send_msg_mutex;
	pthread_t id_agent;
	pthread_t id_recv;
	pthread_t id_send;
    
    bool OpenSocket(void);
    void CloseSocket(void);
    int  Set_Socket_Timeout(bool is_client);
    bool TryToConnect();
    int  Recv_Msg();
    void swap_TMessage_ntoh(TMessage *msg);
    void swap_TMessage_hton(TMessage *msg);
    
    static void *NmsAgent(void *arg);
    static void *NmsAgentRecv(void *arg);
    static void *NmsAgentSend(void *arg);
    void Nms_Agent(void);
    void Nms_Agent_Recv(void);
    void Nms_Agent_Send(void);
    int  __Recv(int s, void *buf, size_t len, int flags);
    int  __Send(int s, const void *buf, size_t len, int flags);
};
#endif

