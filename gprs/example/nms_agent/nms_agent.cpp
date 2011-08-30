#include "nms_agent.h"
#include "../main/dt_systool.h"

//#define  NMS_VERSION_1      1
//#define  NMS_SERIAL_V1      1
#define D_POWERON_T     15
#define D_POWEROFF_T    2
#define D_PPPSTART_T    10
#define D_PPPCLOSE_T    5
#define D_ONLINE_T      5
#define D_OFFLINE_T     10

static Socket  *sock_tmp;
static TMessage maprecv;

register_func NmsAgent::MsgCallbackFun = NULL;

void *
NmsAgent::Nms_Agent_pppd(void *arg)
{
    NmsAgent *nms = (NmsAgent *)arg;
    nms->NetworkWatch();
}

void *
NmsAgent::Nms_Agent_Heart(void *arg)
{
    NmsAgent *nms = (NmsAgent *)arg;
    nms->Heart_Beat_Message();
}


NmsAgent::NmsAgent()
{
    bNmsEnable = false;
    cNmsStat   = GPRSINITSTAT;
    sock_main  = NULL;
    sock_photo = NULL;
    sock_main  = new Socket;
    sock_photo = new Socket;
    sock_main->Register_Callback_Fun(callback_main);
    sock_photo->Register_Callback_Fun(callback_photo);
    sock_tmp = sock_main;
    maprecv.pchContent = NULL;
	if (pthread_create(&id_pppd,  NULL, Nms_Agent_pppd,  this)) {
		perror("pthread_create Nms_Agent_pppd");
	}
	if (pthread_create(&id_heart, NULL, Nms_Agent_Heart, this)) {
		perror("pthread_create Nms_Agent_Heart");
	}
	pthread_detach(id_pppd);
	pthread_detach(id_heart);
}

NmsAgent::~NmsAgent()
{
    if (sock_main) {
        delete sock_main;
    }
    if (sock_photo) {
        delete sock_photo;
    }
    pthread_cancel(id_heart);
    pthread_cancel(id_pppd);
    pthread_join(id_heart, NULL);
    pthread_join(id_pppd, NULL);
}

int
NmsAgent::NmsService_Addr(unsigned int ipaddr, unsigned short port)
{
    if (sock_main) {
        sock_main->NmsService_Addr(ipaddr, port);
    }
    if (sock_photo) {
        sock_photo->NmsService_Addr(ipaddr, port);
    }
	return 0;
}

void
NmsAgent::NmsAgent_Enable(bool enable)
{
    bNmsEnable = enable;
    if (sock_main) {
        sock_main->Socket_Enable(enable);
    }
    if (sock_photo) {
        sock_photo->Socket_Enable(enable);
    }
}

int 
NmsAgent::Get_Link_Status(void)
{
    return (int)cNmsStat;
    #if 0
    int ret = 0;
    if (sock_main) {
        if (!sock_main->Get_Link_Status()) {
            ret |= 0x01;
        }
    }
    if (sock_photo) {
        if (!sock_photo->Get_Link_Status()) {
            ret |= 0x10;
        }
    }
    return ret;
    #endif
}

int
NmsAgent::callback_photo(TMessage* msg)
{
    static TMessage msgtmp;
    int ret = 0;
    msgtmp.pchContent = NULL;
    memcpy(&msgtmp,msg,TMessage_length);
    
    if (msg->wType == 0x05) {        // 收到地图
        if (msg->wEvent == 1) {      // 新的地图
            if (maprecv.pchContent) {
                delete maprecv.pchContent;
                maprecv.pchContent = NULL;
            }
            SysTool::Dump(msg->pchContent, msg->dwLength, 100);
            if ((msg->dwReserve > 0) && (msg->dwReserve < MAPFILELEN_MAX)) {
                DPRINTF("New Map: %d / %d\n",msg->dwLength,msg->dwReserve);
                memcpy(&maprecv,msg,TMessage_length);
                maprecv.pchContent = new unsigned char[msg->dwReserve];
                if (maprecv.pchContent) {
                    memcpy(maprecv.pchContent, msg->pchContent,msg->dwLength);
                } else {
                    DPRINTF("Error:new unsigned char[msg->dwReserve] fail\n");
                    ret = -1;
                }
            } else {
                DPRINTF("Error:Map file: msg->dwReserve = %d\n",msg->dwReserve);
                if (maprecv.pchContent) {
                    delete maprecv.pchContent;
                    maprecv.pchContent = NULL;
                }
                ret = -1;
            }
        } else if (maprecv.pchContent){
            if ((maprecv.dwLength + msg->dwLength) <= maprecv.dwReserve) {
                memcpy(maprecv.pchContent + maprecv.dwLength, msg->pchContent,msg->dwLength);
                maprecv.dwLength += msg->dwLength;
                DPRINTF("Map Recved: %d / %d\n",maprecv.dwLength,maprecv.dwReserve);
            } else {
                DPRINTF("Error:maprecv.dwReserve: %d + %d > %d\n",maprecv.dwLength,msg->dwLength,msg->dwReserve);
                if (maprecv.pchContent) {
                    delete maprecv.pchContent;
                    maprecv.pchContent = NULL;
                }
                ret = -1;
            }
        } else {
            DPRINTF("Error:maprecv.pchContent == NULL\n",msg->dwReserve);
            ret = -1;
        }
        
        if (ret) {
            msgtmp.wEvent = 0;
        }
        msgtmp.dwLength = 0;
        sock_tmp->Send_Msg(&msgtmp);

        if (MsgCallbackFun      && 
            maprecv.dwLength    && 
            (maprecv.dwLength == maprecv.dwReserve)) {
            DPRINTF("Map(%d) Recved!!! \n",maprecv.dwLength + TMessage_length);
            MsgCallbackFun(&maprecv);
        }
    }
    return 0;
}
int
NmsAgent::callback_main(TMessage* msg)
{
    if (MsgCallbackFun) {
        MsgCallbackFun(msg);
    }
    return 0;
}
void
NmsAgent::Register_Callback_Fun(register_func func)
{
    MsgCallbackFun = func;
    //sock_main->Register_Callback_Fun(func);
    //sock_photo->Register_Callback_Fun(func);
}


/*发送TMessage结构体消息*/
int
NmsAgent::Send_Msg(TMessage *msg)
{
    //printf("NmsAgent::Send_Msg sock_fd=%d,msg->wType=%d,msg->dwReserve=%d\n",
    //        sock_photo->sock_fd,msg->wType,msg->dwReserve);
    if ((msg->wType == 4)||(msg->wType == 21)) {
    //if ((msg->dwReserve == 1800)||(msg->dwReserve == 1801)) {
        if (sock_photo) {
            return sock_photo->Send_Msg(msg);
        } else {
            return sock_main->Send_Msg(msg);
        }
    } else {
        return sock_main->Send_Msg(msg);
    }
}


void
NmsAgent::GprsPower(bool enable)
{
    if (enable) {
        if (!system("/usr/local/etc/getgprspower.sh")) {
            system("/usr/local/etc/gprspower.sh");
            printf("gprs power on\n");
            sleep(D_POWERON_T);
        }
    } else {
        if (system("/usr/local/etc/getgprspower.sh")) {
            system("/usr/local/etc/gprspower.sh");
            printf("gprs power off\n");
            sleep(D_POWEROFF_T);
        }
    }
}

int
NmsAgent::PPP_Dial(bool enable)
{
    int cnt=0;
    
    system("ps>/tmp/0");
    if (enable) {
        if (!system("cat /tmp/0|grep -q pppd")) {
            if(!system("ifconfig | grep -q ppp")) {
                cnt = 0;
                printf("network is online\n");
                return 0;
            } else {
                //printf("network is disconnected\n");
                sleep(D_OFFLINE_T);
                return -1;
            }
        } else {
            if ((++cnt) > 10) {
                cnt = 0;
                printf("networking is overtime,poweroff gprs\n");
                return -2;
            } else {
                printf("The pppd no runing,reset gprs & starting pppd\n");
                system("/usr/local/etc/gprsreset.sh");
                sleep(D_POWERON_T);
                system("pppd call gprs &");
                sleep(D_PPPSTART_T);
                return -1;
            }
        }
    } else {
        if (!system("cat /tmp/0|grep -q pppd")) {
            printf("killall pppd\n");
            system("killall pppd");
            sleep(D_PPPCLOSE_T);
        }
        return 0;
    }
}

void
NmsAgent::NetworkWatch(void)
{
    int cnt=0;
    sleep(1);
    while(1) {
        if (bNmsEnable) {
            if (sock_main->isconnected) {
                cnt = 0;
                cNmsStat = CONNECTED;
            }
            switch (cNmsStat) {
            case GPRSINITSTAT:
            case GPRSPOWEROFF:
                GprsPower(true);
                cNmsStat = NETOFFLINE;
                break;

            case NETOFFLINE:
            {
                int ret = PPP_Dial(true);
                switch (ret) {
                case 0:
                    cNmsStat = NETONLINE;
                    break;
                    
                case -1:
                    break;
                    
                case -2:
                    bNmsEnable = false;
                    break;

                default:
                    break;
                }
                break;
            }
                
            case NETONLINE:
                if (++cnt > 5) {
                    cnt = 0;
                    printf("connecting is overtime,reset gprs\n");
                    PPP_Dial(false);
                    cNmsStat = GPRSINITSTAT;
                } else {
                    sleep(D_ONLINE_T);
                }
                break;

            case CONNECTED:
                if (!sock_main->isconnected) {
                    //PPP_Dial(false);
                    cNmsStat = NETONLINE;
                } else {
                    sleep(1);
                }
                break;

            default:
                break;
            }
        } else {
            if (cNmsStat != GPRSPOWEROFF) {
                if (!sock_main->isconnected) {
                    sleep(1);
                    PPP_Dial(false);
                    GprsPower(false);
                    cNmsStat = GPRSPOWEROFF;
                }
            } else {
                sleep(1);
            }
        }
    }
}


void
NmsAgent::Heart_Beat_Message(void)
{
    int OFFLineCount = 0;
	TMessage msg;

	msg.dwVersion = GPRSMSGVER;
	msg.dwSerial = 0;
	msg.dwLength = 0;
	msg.wType = 0;
	msg.wEvent = HEARTEVENT;

	while (1) {
		sleep(HEARTTIME_S);
        if (CONNECTED == Get_Link_Status()) {
            //if (sock_main->DataRecvSta || sock_photo->DataRecvSta) {
            if (sock_main->DataRecvSta) {
                sock_main->DataRecvSta = false;
                //sock_photo->DataRecvSta = false;
                OFFLineCount = 0;
            } else {
                if (OFFLineCount >= OFFLINECOUNT) {
                    OFFLineCount = 0;
                    DPRINTF("Heart_Beat_Message Offline\n");
                    sock_main->ConnectEthSta(false);
                }
        		Send_Msg(&msg);
                OFFLineCount++;
            }
        } else {
            OFFLineCount = 0;
        }
	}
}

