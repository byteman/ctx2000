#ifndef __NMS_AGENT_H
#define __NMS_AGENT_H

#include "socket.h"

#define GPRSINITSTAT    0       //  GPRSģ���ʼ��״̬
#define GPRSPOWEROFF    1       //  GPRSģ��û�ϵ�
#define NETOFFLINE      2       //  GPRSģ�����ϵ磬����δ��������
#define NETONLINE       3       //  GPRSģ���Ѿ��������磬����δ���ӵ��������
#define CONNECTED       4       //  GPRSģ���Ѿ��ͼ�����Ľ�������Socket����

#define HEARTTIME_S     20
#define OFFLINECOUNT    3
#define HEARTEVENT      6000

#define MAPFILELEN_MAX  (2*1024*1024)

class NmsAgent{
public:
	NmsAgent();
	~NmsAgent();
    int  NmsService_Addr(unsigned int ipaddr, unsigned short port);
    void NmsAgent_Enable(bool enable);
    int  Send_Msg(TMessage *msg);
    void Register_Callback_Fun(register_func func);
    int  Get_Link_Status(void);

private:

    bool bNmsEnable;
    char cNmsStat;
    
    Socket  *sock_main;
    Socket  *sock_photo;
    static register_func MsgCallbackFun;
    static int callback_photo(TMessage* msg);
    static int callback_main(TMessage* msg);
	pthread_t id_heart;
    static void *Nms_Agent_Heart(void *arg);
	pthread_t id_pppd;
    static void *Nms_Agent_pppd(void *arg);
    void GprsPower(bool enable);
    int  PPP_Dial(bool enable);
    void NetworkWatch(void);
    void Heart_Beat_Message(void);
};
#endif

