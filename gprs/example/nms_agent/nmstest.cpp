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
#include "nms_agent.h"

#define  MSGLEN  400
#define  MSG1LEN 400

int callback_func(TMessage* msg)
{
    printf("__________%d___\n",msg->wEvent);
    return 0;
}
int main()
{
    unsigned int i = 0;
    int ret;
    TMessage msg;
	msg.dwVersion		= 0x30;	// ��Ϣ�汾��
	msg.dwSerial			= 0;          	// ��Ϣ��ˮ��
	msg.dwDestIp			= 0;     // Ŀ��Ip��ַ
	msg.dwSrcIp			= 0;            // ԴIp��ַ
	msg.wDestPort		= 0;   // Ŀ�Ķ˿�        
	msg.wSrcPort			= 0;            // Դ�˿�
	msg.wDestStreamId	= 0;          	// Ŀ�����ӱ�־
	msg.wSrcStreamId		= 0;            // Դ���ӱ�־
	msg.wType			= 0;       		// ��Ϣ����
	msg.wEvent			= 0;      		// ��Ϣ�¼���
	msg.dwLength			= MSGLEN;            // ��Ϣ�峤�ȣ�achContent����
	msg.dwReserve		= 0;            // ����
	msg.pchContent		= new unsigned char[MSGLEN];         // ��Ϣ��

    TMessage msg1;
	msg1.dwVersion		= 0x31;	// ��Ϣ�汾��
	msg1.dwSerial			= 0;          	// ��Ϣ��ˮ��
	msg1.dwDestIp			= 0;     // Ŀ��Ip��ַ
	msg1.dwSrcIp			= 0;            // ԴIp��ַ
	msg1.wDestPort		= 0;   // Ŀ�Ķ˿�        
	msg1.wSrcPort			= 0;            // Դ�˿�
	msg1.wDestStreamId	= 0;          	// Ŀ�����ӱ�־
	msg1.wSrcStreamId		= 0;            // Դ���ӱ�־
	msg1.wType			= 0;       		// ��Ϣ����
	msg1.wEvent			= 0;      		// ��Ϣ�¼���
	msg1.dwLength			= MSG1LEN;            // ��Ϣ�峤�ȣ�achContent����
	msg1.dwReserve		= 1800;            // ����
	msg1.pchContent		= new unsigned char[MSG1LEN];         // ��Ϣ��
    
    NmsAgent  nms;
    nms.Register_Callback_Fun(callback_func);
    #if 0
    unsigned int ip = inet_addr("222.178.190.34");
    nms.NmsService_Addr(ip,9003);
    nms.NmsAgent_Enable(true);
    #else
    unsigned int ip = inet_addr("192.168.50.35");
    nms.NmsService_Addr(ip,1234);
    nms.NmsAgent_Enable(true);
    #endif
    while(1){
        if (ret = nms.Get_Link_Status()) {
            printf("unlink                      0x%x\n",ret);
        } else {
        }
            memset(msg.pchContent,i + 0x30,MSGLEN);
            memset(msg1.pchContent,i + 0x30,MSG1LEN);
            if (nms.Send_Msg(&msg)) {
            } else {
                printf("msg  send %d ************ %d\n",MSGLEN,i);
            }
            if (nms.Send_Msg(&msg1)) {
            } else {
                printf("msg1 send    %d              %d\n",MSG1LEN,i);
            }
            i++;
            sleep(1);
    }
	return 0;
}

