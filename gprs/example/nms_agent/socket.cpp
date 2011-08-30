#include "socket.h"


#define NMS_AGENT_VERSION   "nms agent v0.0.2"
#define WAIT_SEC            60
#define TCP_IDLE_TIME       60
#define TCP_INTVL_TIME      40
#define TCP_CONNECT_CNT     3


//#define  NMS_VERSION_1      1
//#define  NMS_SERIAL_V1      1


void *
Socket::NmsAgent(void *arg)
{
    Socket *nms = (Socket *)arg;
    nms->Nms_Agent();
}

void *
Socket::NmsAgentRecv(void *arg)
{
    Socket *nms = (Socket *)arg;
    nms->Nms_Agent_Recv();
}
void *
Socket::NmsAgentSend(void *arg)
{
    Socket *nms = (Socket *)arg;
    nms->Nms_Agent_Send();
}

Socket::Socket()
{    
    isconnected = false;
    nms_enable  = false;
    DataRecvSta = false;
    sock_fd = 0;
    addr_ip = 0;
    net_port = 0;
    MsgCallbackFun = NULL;

    for(int i = 0;i <SENDBUFLEN;i++) {
        MsgSendBuf[i] = NULL;
    }
    recvmsg = new TMessage;
    recvmsg->pchContent = NULL;
    
    pthread_mutex_init(&connected_mutex, NULL);
    pthread_mutex_init(&send_msg_mutex, NULL);
    pthread_mutex_init(&change_ip_port, NULL);



    if (pthread_create(&id_agent, NULL, NmsAgent, this)) {
        perror("pthread_create NmsAgent");
        exit(-1);
    }
    pthread_detach(id_agent);

    if (pthread_create(&id_recv, NULL, NmsAgentRecv, this)) {
        perror("pthread_create NmsAgentRecv");
        exit(-1);
    }
    pthread_detach(id_recv);

    if (pthread_create(&id_send, NULL, NmsAgentSend, this)) {
        perror("pthread_create NmsAgentSend");
        exit(-1);
    }
    pthread_detach(id_send);
}

Socket::~Socket()
{
    pthread_mutex_destroy (&connected_mutex); 
    pthread_mutex_destroy (&send_msg_mutex); 
    pthread_mutex_destroy (&change_ip_port); 
    pthread_cancel(id_agent);
    pthread_join(id_agent, NULL);
    pthread_cancel(id_recv);
    pthread_join(id_recv, NULL);
    pthread_cancel(id_send);
    pthread_join(id_send, NULL);
}

void 
Socket::Nms_Agent(void)
{
    while(1) {
        if (isconnected == false) {
            if (sock_fd) {
                CloseSocket();
                sleep(1);
            }
            
            if (nms_enable) {
                if (false == OpenSocket()) {
                    DPRINTF("%s  %d false == OpenSocket()\n",__func__,__LINE__);
                    continue;
                }

                int ret = Set_Socket_Timeout(true);
                if (ret != 0) {
                    //DPRINTF("%s  %d ret = %d\n",__func__,__LINE__,ret);
                    //continue;
                }

                if (false == TryToConnect()) {
                    DPRINTF("%s  %d false == TryToConnect()\n",__func__,__LINE__);
                    CloseSocket();
                    sleep(3);
                    continue;
                }
                DPRINTF("Connected to ip=%d port=%d \n",addr_ip,net_port);
                ConnectEthSta(true);
            } else {
                usleep(10000);
            }
        } else {
            usleep(10000);
        }
    }
}

void 
Socket::Nms_Agent_Recv(void)
{
    struct timeval timeout={5,0};
    int ret;
    while(1) {
        timeout.tv_sec  = 5;
        timeout.tv_usec = 0;
        if (isconnected && sock_fd) {
            FD_ZERO(&readfds);
            FD_SET(sock_fd, &readfds);
            ret = select(sock_fd + 1, &readfds, NULL, NULL, &timeout);
            if (ret <= 0) {
                if (ret) {
                    ConnectEthSta(false);
                }
                continue;
            }
            if (FD_ISSET(sock_fd, &readfds)) {
                if (0 != Recv_Msg()) {
                    ConnectEthSta(false);
                    continue;
                }

                if (recvmsg->wEvent != 6000) {
                    if (MsgCallbackFun) {
                        MsgCallbackFun(recvmsg);
                    } else {
                    }
                }
            }
        } else {
            usleep(100000);
        }
    }
}

void 
Socket::Nms_Agent_Send(void)
{
    TMessage *t_msg = NULL;
    int i;
    while(1) {
        if (t_msg == NULL) {
            pthread_mutex_lock(&send_msg_mutex);
            for(i = 0; i < SENDBUFLEN; i++) {
                if (MsgSendBuf[i] != NULL) 
                    break;
            }
            if (i < SENDBUFLEN) {
                t_msg = new TMessage;
                memcpy(t_msg,MsgSendBuf[i],TMessage_length);
                if (MsgSendBuf[i]->pchContent) {
                    t_msg->pchContent = new unsigned char[MsgSendBuf[i]->dwLength];
                    memcpy(t_msg->pchContent,MsgSendBuf[i]->pchContent,MsgSendBuf[i]->dwLength);
                    delete MsgSendBuf[i]->pchContent;
                    MsgSendBuf[i]->pchContent = NULL;
                } else {
                    t_msg->pchContent = NULL;
                    t_msg->dwLength   = 0;
                }
                delete MsgSendBuf[i];
                MsgSendBuf[i] = NULL;
                for (i = 1; i < SENDBUFLEN; i++) {
                    if ( (MsgSendBuf[i] !=NULL) && (MsgSendBuf[i-1] ==NULL)) {
                        MsgSendBuf[i - 1] = MsgSendBuf[i];
                        MsgSendBuf[i] = NULL;
                    }
                }
            }
            pthread_mutex_unlock(&send_msg_mutex);
        }
        if (t_msg != NULL) {
            if (isconnected && sock_fd){
                int ret;
                int len = 0;
                if (t_msg->pchContent) {
                    len=t_msg->dwLength;
                }
                swap_TMessage_hton(t_msg);
                
                if ((ret=__Send(sock_fd, t_msg, TMessage_length, 0)) < 0) {
                    ConnectEthSta(false);
                    DPRINTF("__Send:%s,%d\n",__func__,__LINE__);
                    continue;
                }
                if (ret != TMessage_length) 
                    DPRINTF("\nERROR:%s,%d, ret=%d,dwLength=%d\n",__func__,__LINE__,ret,TMessage_length);

                if (len) {
                    if ((ret = __Send(sock_fd, t_msg->pchContent, len, 0)) < 0) {
                        ConnectEthSta(false);
                        DPRINTF("__Send ERROR:%s,%d\n",__func__,__LINE__);
                        continue;
                    }
                    if (ret != len)  {
                        DPRINTF("\nERROR:%s,%d, ret=%d,dwLength=%d\n",__func__,__LINE__,ret,len);
                    }
                }
                usleep(10000);
            }
            if (t_msg->pchContent) {
                delete t_msg->pchContent;
                t_msg->pchContent = NULL;
            }
            if (t_msg) {
                delete t_msg;
                t_msg = NULL;
            }
        } else {
            usleep(10000);
        }
        usleep(1000);
    }
}


int
Socket::NmsService_Addr(unsigned int ipaddr, unsigned short port)
{
    addr_ip     = ipaddr;
    net_port    = port;
    isconnected = false;
    //nms_enable  = true;
    return 0;
}

void
Socket::Socket_Enable(bool enable)
{
    //isconnected = false;
    nms_enable = enable;
    if (!enable) {
        isconnected = false;
        CloseSocket();
    }
}

bool 
Socket::Get_Link_Status(void)
{
    return isconnected;
}

void
Socket::Register_Callback_Fun(register_func func)
{
    if (func) {
        MsgCallbackFun = func;
    }
}


bool
Socket::OpenSocket(void)
{
    pthread_mutex_lock(&connected_mutex);
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        sock_fd = 0;
        pthread_mutex_unlock(&connected_mutex);
        perror("socket not open");
        return false;
    }
    pthread_mutex_unlock(&connected_mutex);
    return true;
}

void
Socket::CloseSocket(void)
{
    if (sock_fd) {
        pthread_mutex_lock(&connected_mutex);
        close(sock_fd);
        sock_fd = 0;
        pthread_mutex_unlock(&connected_mutex);
    }
}

int
Socket::Set_Socket_Timeout(bool is_client)
{
    int tcp_nodely = 1;
    int keep_alive = 1; // 开启keepalive属性
    int keep_idle = TCP_IDLE_TIME; // 如该连接在TCP_IDLE_TIME秒内没有任何数据往来,则进行探测
    int keep_interval = TCP_INTVL_TIME; // 探测时发包的时间间隔为TCP_INTVL_TIME 秒
    int keep_count = TCP_CONNECT_CNT; // 探测尝试的次数.如果第1次探测包就收到响应了,则后TCP_CONNECT_CNT次的不再发
    socklen_t tv_len = sizeof(struct timeval);
    struct timeval tv;
    struct linger ling_t = {1, 30};

    tv.tv_sec = WAIT_SEC;
    tv.tv_usec = 0;

    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, tv_len) < 0)
        return -1;

    if (setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, tv_len) < 0)
        return -2;

    if (setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodely, sizeof(tcp_nodely)) < 0)    
        return -3;

    if (setsockopt(sock_fd, IPPROTO_TCP, SO_LINGER, &ling_t, sizeof(ling_t)) < 0)
        return -4;

    if (is_client) {
        if (setsockopt(sock_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keep_alive, sizeof(keep_alive)) < 0)
            return -5;

        if (setsockopt(sock_fd, SOL_TCP, TCP_KEEPIDLE, (void*)&keep_idle, sizeof(keep_idle)) < 0)
            return -6;

        if (setsockopt(sock_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keep_interval, sizeof(keep_interval)) < 0)
            return -7;

        if (setsockopt(sock_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keep_count, sizeof(keep_count)) < 0)
            return -8;
    }

    return 0;
}

bool
Socket::TryToConnect()
{
    /*填写 sockadd_in 结构*/
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    pthread_mutex_lock(&change_ip_port);
    addr.sin_port = htons(net_port);
    addr.sin_addr.s_addr = addr_ip;
    pthread_mutex_unlock(&change_ip_port);

    /*尝试连接*/
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        //perror("connet is error");
        return false;
    }
    return true;
}

void
Socket::ConnectEthSta(bool online)
{
    isconnected = online;
    if (MsgCallbackFun) {
        TMessage msg_t;
        msg_t.dwVersion = GPRSMSGVER;
        msg_t.dwSerial =  0;
        msg_t.dwLength =  0;
        msg_t.wType =     0;
        msg_t.pchContent= NULL;
        if (online) {
            msg_t.wEvent =    WM_MESSAGE_SOCKET_ONLINE;
        } else {
            msg_t.wEvent =    WM_MESSAGE_SOCKET_OFFLINE;
        }

        MsgCallbackFun(&msg_t);
    }
}
    
/*接收TMessage结构体消息*/
int
Socket::Recv_Msg()
{
    if (!sock_fd)
        return -1;

    //bzero(recvmsg, TMessage_length);

    if (__Recv(sock_fd, (void *)recvmsg, TMessage_length, MSG_WAITALL) < 0)
        return -1;

    swap_TMessage_ntoh(recvmsg);

    //DPRINTF("ready recv leng=%d\n",recvmsg->dwLength);
    if (recvmsg->dwLength <= 0) {
        if (recvmsg->pchContent) {
            delete recvmsg->pchContent;
        }
        recvmsg->pchContent = NULL;
        return 0;
    } else {
        int ret;
        if (recvmsg->pchContent) {
            delete recvmsg->pchContent;
            recvmsg->pchContent = NULL;
        }
        if (recvmsg->dwLength > DATAMAXLEN) {
            DPRINTF("ERROR:%s,%d  recvmsg->dwLength > DATAMAXLEN\n",__func__,__LINE__);
            return -1;
        }
        recvmsg->pchContent = new unsigned char[recvmsg->dwLength];
        DPRINTF("Ready recv leng=%d\n",recvmsg->dwLength);
        if ((ret=__Recv(sock_fd, (void *)recvmsg->pchContent, recvmsg->dwLength, MSG_WAITALL)) < 0) {
            DPRINTF("__Recv ERROR:%s,%d\n",__func__,__LINE__);
            return -1;
        } else {
            if (ret != recvmsg->dwLength) 
                DPRINTF("ERROR:%s,%d ret=%d,dwLength=%d\n",__func__,__LINE__,ret,recvmsg->dwLength);
            return 0;
        }
    }
}

/*发送TMessage结构体消息*/
int
Socket::Send_Msg(TMessage *t_msg)
{
    int i,ret = 0;
    if(isconnected == false)
        return -1;
    pthread_mutex_lock(&send_msg_mutex);
    for (i = 0; i < SENDBUFLEN; i++) {
        if ( MsgSendBuf[i] == NULL) {
            break;
        }
    }
    if (i >= SENDBUFLEN) {
        ret =  -2;
    } else {
        if (t_msg->dwLength < DATAMAXLEN) {
            MsgSendBuf[i] = new TMessage;
            memcpy(MsgSendBuf[i],t_msg,TMessage_length);
            if (t_msg->dwLength) {
                MsgSendBuf[i]->pchContent = new unsigned char[t_msg->dwLength];
                //memcpy(MsgSendBuf[i]->pchContent,t_msg->pchContent,t_msg->dwLength);
                memcpy(MsgSendBuf[i]->pchContent,(char *)t_msg+TMessage_length,t_msg->dwLength);
            } else {
                MsgSendBuf[i]->pchContent = NULL;
            }
        } else {
            ret = -3;
        }
    }
    pthread_mutex_unlock(&send_msg_mutex);
    return ret;
}
void
Socket::swap_TMessage_ntoh(TMessage *t_msg)
{
    t_msg->dwVersion =      ntohl(t_msg->dwVersion);
    t_msg->dwSerial =       ntohl(t_msg->dwSerial);
    t_msg->dwDestIp =       ntohl(t_msg->dwDestIp);
    t_msg->dwSrcIp =        ntohl(t_msg->dwSrcIp);
    t_msg->wDestPort =      ntohs(t_msg->wDestPort);
    t_msg->wSrcPort =       ntohs(t_msg->wSrcPort);
    t_msg->wDestStreamId =  ntohs(t_msg->wDestStreamId);
    t_msg->wSrcStreamId =   ntohs(t_msg->wSrcStreamId);
    t_msg->wType =          ntohs(t_msg->wType);
    t_msg->wEvent =         ntohs(t_msg->wEvent);
    t_msg->dwLength =       ntohl(t_msg->dwLength);
    t_msg->dwReserve =      ntohl(t_msg->dwReserve);
}

void
Socket::swap_TMessage_hton(TMessage *t_msg)
{
    t_msg->dwVersion =      htonl(t_msg->dwVersion);
    t_msg->dwSerial =       htonl(t_msg->dwSerial);
    t_msg->dwDestIp =       htonl(t_msg->dwDestIp);
    t_msg->dwSrcIp =        htonl(t_msg->dwSrcIp);
    t_msg->wDestPort =      htons(t_msg->wDestPort);
    t_msg->wSrcPort =       htons(t_msg->wSrcPort);
    t_msg->wDestStreamId =  htons(t_msg->wDestStreamId);
    t_msg->wSrcStreamId =   htons(t_msg->wSrcStreamId);
    t_msg->wType =          htons(t_msg->wType);
    t_msg->wEvent =         htons(t_msg->wEvent);
    t_msg->dwLength =       htonl(t_msg->dwLength);
    t_msg->dwReserve =      htonl(t_msg->dwReserve);
}

int
Socket::__Recv(int s, void *buf, size_t len, int flags)
{
    int count = 0, real_count = 0;
    int recv_cel = 0;
    while (real_count < len) {
        if ((len - real_count) / 4096) {
            recv_cel = 4096;
        } else {
            recv_cel = len - real_count;
        }
        if ((count = recv(s, (char *)buf + real_count, recv_cel, flags)) <= 0) {
            if (errno == EINTR) {
                DPRINTF("__Recv EINTR  \n");
                continue;
            } else {
                return -1;
            }
        }
        real_count += count;
        DataRecvSta = true;
    }
    return real_count;
}

/* origin version
int
Socket::__Send(int s, const void *buf, size_t len, int flags)
{
    int count = 0, real_count = 0;

    ASSERT(buf != NULL);

    while (real_count < len) {
        if ((count = send(s, (char *)buf + real_count, len - real_count, flags)) < 0) {
            if (errno == EINTR) {
                DPRINTF("__Send EINTR  \n");
                continue;
            } else {
                return -1;
            }
        }
        real_count += count;
    }
    return real_count;
}*/


//modify by unreal for check gprs link down
int
Socket::__Send(int s, const void *buf, size_t len, int flags)
{
    int count = 0, real_count = 0;

    ASSERT(buf != NULL);

    while (real_count < len) {
        int piece_size = 1 * 1024;
        if((len - real_count) < piece_size) {
            piece_size = len - real_count;
        }
        if ((count = send(s, (char *)buf + real_count, piece_size, flags)) < 0) {
            DPRINTF("count=%d ", count);
            if (errno == EINTR) {
                DPRINTF("__Send EINTR  \n");
                continue;
            } else {
                return -1;
            }
        }
        real_count += count;
        DPRINTF(" current s=%d, send %d byte, has sent %d byte, total %d byte\n",
                s, count, real_count, len);
        //sleep(1);
    }
    return real_count;
}