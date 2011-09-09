#include "mainctrl.h"
#include <Poco/Format.h>
#include <Poco/SingletonHolder.h>
#include <Poco/Timestamp.h>
#include <Poco/Timespan.h>
#include <Poco/StringTokenizer.h>
#include <Poco/NotificationQueue.h>
#include <Poco/NumberParser.h>
#include <Poco/String.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
#include <assert.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <math.h>
#include "iniFile.h"
#include "comdata.h"
#include "torque_mgr.h"
#include "beeper.h"
#include "jdqadmin.h"
#include "dt_core.h"
#include <guinotifyer.h>
#include "iniFile.h"
#include "gprs.h"
#include "LoggerMgr.h"
using Poco::SingletonHolder;
using Poco::StringTokenizer;
using Poco::NotificationQueue;


Poco::Timespan g_SubWaitSubTime(3,0);
TIniFile cfg;
#define CTX_DEBUG
#ifdef CTX_DEBUG
#define CTX_DBG(fmt...) fprintf(stderr,fmt);
//#define CTX_DBG(fmt...) TraceEx(fmt)
#else
#define CTX_DBG(fmt...) do { } while (0)
#endif

class CDBAdmin:public Poco::Runnable
{
public:

    CDBAdmin()
    {

    }

    bool start()
    {
        m_quit = false;
        m_thread.start(*this);
        return m_rdyEvt.tryWait(1000);
    }
    bool stop()
    {
        m_quit = true;
        return m_quitEvt.tryWait(2000);
    }
    bool PostMessage(Poco::Notification* ptr)
    {
        if(!m_quit)
            Poco::NotificationQueue::defaultQueue().enqueueNotification(ptr);
        return !m_quit;
    }
    virtual void run()
    {
        m_rdyEvt.set();
        m_quitEvt.reset();
        m_quit = false;
        while(!m_quit)
        {
            try
            {
                //CTX_DBG("Wait QCollisionNotification Msg\n");
                Poco::AutoPtr<Poco::Notification> pNf(Poco::NotificationQueue::defaultQueue().waitDequeueNotification(1000));
                if (pNf)
                {

                        Poco::AutoPtr<QCollisionNotification> pMsgNf = pNf.cast<QCollisionNotification>();
                        if(pMsgNf->m_code == 0)
                        {
                             CTajiDbMgr::Get().AddAlarmInfo(TCTypeName,pMsgNf->m_coll.m_slewing,pMsgNf->m_coll.m_trolley);
                        }else if(pMsgNf->m_code >= 1 && pMsgNf->m_code<=3 ){

                            /*
                                1.overload
                                2.wild speed > 20m/s
                                3.wild speed > 13m/s
                            */
                            TWeightHistoy value;
                            value.angle  = Poco::format("%0.2f", pMsgNf->m_fall.m_angle);
                            value.dist   = Poco::format("%0.2f", pMsgNf->m_fall.m_dist);
                            value.fall   = Poco::format("%d",    pMsgNf->m_fall.m_fall);
                            value.weight = Poco::format("%0.2f", pMsgNf->m_fall.m_weight);
                            value.speed  = Poco::format("%0.2f", pMsgNf->m_fall.m_speed);
                            value.serial = TCTypeName; //�����Ϊ�����ͺţ�ȥ���������
                            value.type   = pMsgNf->m_fall.m_type;

                            CTajiDbMgr::Get().AddWeightInfo(value);
                        }

                }
            }
            catch (Poco::Exception& e)
            {
                std::cerr << "CDataParseWorker: " << e.displayText() << std::endl;
                    // parsing exception, what should we do?
            }
            catch (...)
            {
            }
        }
        m_quitEvt.set();
    }
private:
    Poco::Thread  m_thread;
    Poco::Event   m_rdyEvt;
    Poco::Event   m_quitEvt;
    volatile bool m_quit;
};
CMainCtrl::CMainCtrl()
{
    m_mode    = mode_unknown;
    m_dbadmin = NULL;
    for(int i = 0; i < 12; i ++)
        m_control_state.b[i] = false;
}
CMainCtrl::~CMainCtrl()
{

}

int       CMainCtrl::ValideTCNum()
{
    int result = 0;
    for(int i = 1; i <= TCTotalNum;i++)
        if(g_TC[i].Valide)
            result +=1;

    return result;
}
/*
1.���ֻ�б������ߣ��ͷ��ر���id
2.����дӻ����ߣ��ͷ������һ����Ч�����ߴӻ����
*/
 int       CMainCtrl::MaxNo()
 {
    int result = 0;
    int valid_num = ValideTCNum();
    //CTX_DBG("valid=%d\n",valid_num);
    if(valid_num == 1){
        result =  m_local_id;
        //CTX_DBG("m_local_id=%d\n",m_local_id);
    }else{
        for(int i =1 ;i <= TCTotalNum;i++){
            if( (g_TC[i].Valide) && (i != m_main_id)){
                result = i;
            }
        }
    }
    return result;
 }
std::string CMainCtrl::GetAddID()
{
    std::string result = "1";

    if(AddNo == TCTotalNum){
        AddNo = 1;
        result="1";
    }else{
        AddNo+=1;
        result=Poco::format("%d",AddNo);
    }

    return result;
}
std::string CMainCtrl::GetNextID()
{
    bool found = false;
    std::string result = "0";

    if (RightNo==TCTotalNum ){
        for(int i = 1; i <= TCTotalNum; i++){
            if( (g_TC[i].Valide) && ( i != m_local_id ) )
            {
                RightNo = i;
                found = true;
                result=Poco::format("%d",i);
                break;
            }

        }
    }else{
        for(int i = RightNo+1; i <= TCTotalNum;i++){
            if( (g_TC[i].Valide) && ( i != m_local_id ) )
            {
                RightNo = i;
                found = true;
                result=Poco::format("%d",i);
                break;
            }

        }
        if(found==false){
            for(int i = 1; i < RightNo; i++){
                if( (g_TC[i].Valide) && ( i != m_local_id ) )
                {
                    RightNo = i;
                    found = true;
                    result=Poco::format("%d",i);
                    break;
                }

            }
        }
    }
     //DBG("%s %d\n",__FUNCTION__,__LINE__);
    if(result == "0"){
        result = Poco::format("%d",RightNo);
    }
 //DBG("%s %d\n",__FUNCTION__,__LINE__);
    if(ValideTCNum() == 1){
        RightNo = m_local_id;
        result  = CurID;
    }
     //DBG("%s %d\n",__FUNCTION__,__LINE__);
    return Poco::trim(result);

}
/*
mode :��д������ʵ��ģʽ
*/
std::string      CMainCtrl::build_qurey_msg()
{
    std::string Next;
    std::string result;
    std::string tmpStr;

    if(m_mode==mode_master){
        Next = GetNextID();

        int nextid= RightNo;//Poco::NumberParser::parse(Next);
        result = "%"+Poco::format("%2d",m_local_id)+"N"+Poco::format("%2d",nextid)+"N";
    }else {
        result = "%"+Poco::format("%2d",m_local_id)+"N 0N";
    }
    //�Ƕ�
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].Angle);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+=tmpStr;
    //λ��
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].Position);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+="N"+tmpStr;
    //���ٶ�
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].AngleSpeed);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+="N"+tmpStr;
    //����
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].Dang);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+="N"+tmpStr;
    if(m_mode==mode_master){

        int nextId = RightNo;//Poco::NumberParser::parse(Next);
        int maxid  = MaxNo() ;
        //CTX_DBG("maxid=%d,nextid=%d\n",maxid,nextId);
        if(maxid == nextId){//�Ѿ����������һ����Ч�������������һ���µ�����
            tmpStr=Poco::format("%2s",GetAddID());
        }else
            tmpStr=" 0";
    }else{
        tmpStr=" 0";
    }
    result+="N"+tmpStr+"N#";

    return result;
}

bool      CMainCtrl::is_conflcit(int callto_id)
{

     return false;
}
void      CMainCtrl::UpdateTCStatus()
{
    Poco::Timestamp curTime;
    Poco::Timespan  span;
    int cnt =0;
    for(int i = 1; i <= TCTotalNum;i++){
        span = curTime-g_TC[i].InfoTime;
        if(span > g_SubWaitSubTime)
        {
            if(i != m_local_id) //ֻ��Ҫ���ôӻ�������״̬
                g_TC[i].Valide = false;
        }else{
            g_TC[i].Valide = true;
            cnt++;
        }
    }
    //CTX_DBG("valid=%d\n",cnt);
    //DBG("%s main_id=%d local_id=%d\n",__FUNCTION__,m_main_id,m_local_id);
    g_TC[m_main_id].Valide  = true; //����ʼ����Ч
    g_TC[m_local_id].Valide = true; //����ʼ����Ч
}
void      CMainCtrl::DripMainNoAndAddNo(std::string &MainNo, std::string &RightNo, std::string &AddNo)
{
    CTX_Message msg;
    if(CDianTai::Get().GetMessage(msg))
    {
          int    rightNo,senderId;
          double angle,position,AngleSpeed,Dang;
          CTX_DBG("DripMainNoAndAddNo %s\n",msg.context.c_str());
          Poco::StringTokenizer token(msg.context,"N");
#ifdef CTX_DEBUG
            for(size_t i = 0 ; i < token.count(); i++)
            {
                CTX_DBG("token[%d]=%s\n",i,token[i].c_str());
            }
#endif
          if(token.count() != 7)
          {
               CTX_DBG("DripMainNoAndAddNo Error MsgCount =%d\n",token.count());
               return;
          }

          if(!Poco::NumberParser::tryParse(Poco::trim(token[0]),senderId))return;
          if(!Poco::NumberParser::tryParse(Poco::trim(token[1]),rightNo))return;
          if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[2]),angle))return;
          if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[3]),position))return;
          if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[4]),AngleSpeed))return;
          if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[5]),Dang))return;

          if(senderId >0 && senderId <= TCTotalNum )
          {
              g_TC[senderId].Angle      = angle;
              g_TC[senderId].Position   = position;
              g_TC[senderId].AngleSpeed = AngleSpeed;
              g_TC[senderId].Dang       = Dang;
              //g_TC[senderId].DLong      = g_TC[id].Position;
              //g_TC[senderId].DHeight    = g_TC[id].Height+g_TC[id].L*sin(g_TC[id].Dang);

              MainNo  = Poco::trim(token[0]);
              RightNo = Poco::trim(token[1]);
              AddNo   = Poco::trim(token[6]);
          }
    }
}
void      CMainCtrl::WatchNetWork(std::string &MainDevID, bool &AddState)
{
    Poco::Timestamp StartTime;
    Poco::Timestamp CurTime;
    CTX_Message msg;
    std::string RightNo,AddNo;
    bool FoundM = false;
    RightNo = "0";
    AddNo   = "0";
    m_mode  = mode_unknown;

    CDianTai::Get().ClearMessage(); //���������֮ǰҪ�ѵ�̨���������ȫ������
    //listen 5s to recv rtmsg;
    CDianTai::Get().ResetCount();
    while( (CurTime-StartTime) < 5000000) //�ȴ�5s���Ƿ����յ�������ȡ�����ӻ�������
    {
        if(CDianTai::Get().GetMessage(msg))//��������п������������ģ�Ҳ�п����Ǵӻ���Ӧ��
        {
                CTX_DBG("WatchNetWork Capture Message ----> Maybe Find Master\n");
                FoundM = true;
                break;
        }

        Poco::Thread::sleep(50);

        CurTime.update();
    }

    if(FoundM){
        //�ȴ�100ms���ٽ���һ����Ϣ,����100ms�п����Ѿ��յ��˺ü�����
        Poco::Thread::sleep(100);
        if(CDianTai::Get().CheckMessage())
        {
                DripMainNoAndAddNo(MainDevID,RightNo,AddNo);
        }
        while( (RightNo=="0") || (AddNo != CurID) || (MainDevID == CurID) ){
            Poco::Thread::sleep(10);
            if(CDianTai::Get().CheckMessage())
            {
                DripMainNoAndAddNo(MainDevID,RightNo,AddNo);
                CTX_DBG("DripMainNoAndAddNo Ok MainId=%s RightId=%s AddNo=%s CurId\n",MainDevID.c_str(),RightNo.c_str(),AddNo.c_str());
                if( (RightNo!="0") && (AddNo == CurID) && (MainDevID != CurID))
                {

                }
            }
        }
        CTX_DBG("WatchNetwork ok MainId=%s RightId=%s AddNo=%s\n",MainDevID.c_str(),RightNo.c_str(),AddNo.c_str());
        m_mode    = mode_slave;
        if(!Poco::NumberParser::tryParse(MainDevID,m_main_id))
        {
            CTX_DBG("Cant Convert MainDevID=%s\n",MainDevID.c_str());
        }

        CTX_DBG("DripMainNoAndAddNo ok MainDevID=%d local_id=%d\n ",m_main_id,m_local_id);
        g_TC[m_local_id].Valide = true;
        g_TC[m_main_id].Valide  = true;
        AddState = true;

        StartTime.update();
        CurTime=StartTime;
        CTX_DBG("[Ready Add TC] Wait Slave Ack\n");
        //�ٽ���һ����Ч����Ȼ���ͱ�����Ϣ [���ǽ��ձ���ѯ�Ĵӻ�����������Ϣ������Ҫ���յ������Ϣ�󣬲��ܷ��ͣ��������ͬʱ���͵ĳ�ͻ]
        //��Ϊ�¼����������Ǹ�����������ѯ��Ϣ�еģ����Ա���ѯ����Ҳ���Ӧ��Ϣ��������������Ӧ�����¼���������ȴ�20ms�󣬲����յ��˸û�Ӧ
        //��Ż��Ӧ�Լ�����Ϣ,���ֻ��һ������������£���ô��������һ������ָ��1s����Ҳ���յ����ʿ��Լ���ɹ�
        while (!CDianTai::Get().GetMessage(msg)) {
            CurTime.update();
            if( (CurTime-StartTime) > 1200000){ //1200ms �ȴ�1200ms�Ա���ղ�ѯ�����Ļ�Ӧ��Ϣ
                AddState=false;
            }
            Poco::Thread::sleep(20);
            CTX_DBG("New Add TC wait [Slave RightNo Ack]\n");
        }
        if(!AddState)
        {
            CTX_DBG("[Ready Add TC] Wait Slave Ack Failed\n");
            return;
        }
        //����ΪʲôҪ��Ӧ����,��ΪҪ����������������
        std::string sendInfo = build_qurey_msg();
        CDianTai::Get().SendMessage(sendInfo);
        lastDateTime.update();
        sendInfoTime=lastDateTime;

    }else{
        //�ϸ�Ҫ�������ڵȴ������в����յ��κ�һ������,���ܱ�Ϊ����
        if( CDianTai::Get().GetReceivedCount() > 0) //��Ȼû���յ���ȷ�����ݣ������п������յ������롣����Ҳ���������������
        {
            Error("WatchNetwork Failed beacuse of GetReceivedCount=%d\n",CDianTai::Get().GetReceivedCount());
            AddState  = false;
        }else{
            for(int i = 1; i <= TCTotalNum; i++)
            {
                g_TC[i].Valide = false;
                if( i != m_local_id )
                {
                    g_TC[i].InfoTime = 0;
                    //g_TC[i].InfoTime.update(); //��λ��������ÿ������������ˢ��ʱ��
                }
            }
            g_TC[m_local_id].Valide = true;
            m_main_id = m_local_id;
            MainDevID = CurID;
            AddState  = true;
            m_mode    = mode_master;
        }

    }
}
void dealOtherData()
{

}
void      CMainCtrl::DistillData(std::string &msg,std::string &ID)
{
    int id = 0;
    int rightNo,senderId;
    double angle,position,AngleSpeed,Dang;
    Poco::StringTokenizer token(msg,"N");
#if 0
    for(size_t i = 0 ; i < token.count(); i++)
    {
        CTX_DBG("token[%d]=%s\n",i,token[i].c_str());
    }
#endif
    if(token.count() != 7)
    {
         CTX_DBG("DistillData Error MsgCount =%d\n",token.count());
         return;
    }
    ID = "0"; //�ȸ�һ��Ĭ��ֵ����ʹת��ʧ���˻�����������Ҳ��һ��Ĭ��ֵ
    if(!Poco::NumberParser::tryParse(Poco::trim(token[0]),senderId))return;
    if(!Poco::NumberParser::tryParse(Poco::trim(token[1]),rightNo))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[2]),angle))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[3]),position))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[4]),AngleSpeed))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[5]),Dang))return;

    if( (senderId <= TCTotalNum)&& (senderId > 0) )
    {
        ID=Poco::trim(token[1]); //�����е�id
        g_TC[senderId].InfoTime.update(); //����������Ϣ����ʱ��

        g_TC[senderId].Angle      = angle;
        g_TC[senderId].Position   = position;
        g_TC[senderId].AngleSpeed = AngleSpeed;

        if(g_TC[senderId].Dyna)
            g_TC[senderId].Dang       = Dang;
        else
            g_TC[senderId].Dang       = 0;
        //g_TC[senderId].DLong      = g_TC[id].Position;
        //g_TC[senderId].DHeight    = g_TC[id].Height+g_TC[id].L*sin(g_TC[id].Dang);
        //AddNo = token[6];
    }
}
void CMainCtrl::dealMainData(std::string &msg)
{

}

void CMainCtrl::slave_loop()
{
    CTX_Message msg;
    std::string ID;
    Poco::Timespan span;
    Poco::Timespan sub_wait_span(20,0); //10s
    bool AddState=false;
    if(CDianTai::Get().CheckMessage()) //�������Ϣ�Ž��봦����Ϣ
    {
        while (CDianTai::Get().GetMessage(msg)) {
            lastDateTime.update();
            DistillData(msg.context,ID); //ID���������е�������š�
            span = lastDateTime-sendInfoTime; //����೤ʱ��û���յ�����������������ˣ���ʱ����������������磬˵�������Ѿ����������������Ѿ���Ϊ�ôӻ��Ѿ����ߡ�������Ҫ���������������
            //CTX_DBG("span=%d\n",span.seconds());

            if(span > sub_wait_span){
                CTX_DBG("========slave receive timeout re WatchNetWork\n");
            #if 1
                AddState=false;
                while(AddState==false)
                      WatchNetWork(MainMachineID,AddState);
            #endif
            }
            //CTX_DBG("slave_loop3\n");
            if(ID==CurID){ //�յ����������������Ĳ�ѯ��Ϣ
                std::string sendInfo = build_qurey_msg();
                CDianTai::Get().SendMessage(sendInfo);
                sendInfoTime.update();
                //CTX_DBG("slave %s %s\n",ID.c_str(),sendInfo.c_str());
            }
            //CTX_DBG("slave_loop4\n");
        }

    }else{ //������ﲻ��Ϣ����ô�ӻ���cpuռ���ʺܸ�
        //���һֱû���յ���Ϣ����ֱ�ӷ���,���ı��Լ��Ĺ���״̬�����ǵȴ���Ļ�����Ϊ����
        //�Լ���������Ϊ�������ӻ���������Ϊ���������ǿ���������������ɴӻ�
        Poco::Thread::sleep(20);
    }
    //CTX_DBG("slave_loop\n");
    //Poco::Thread::sleep(20); //���԰�����ĵȴ���ȥ������Ϊ�ڵ���slave_loop֮ǰ�Ѿ����Ĺ�25ms��

}
void CMainCtrl::master_loop()
{
    //CTX_DBG("master_loop\n");
    CTX_Message msg;
    std::string ID;
    int  msg_count = 0;
    bool RecieveValidData=false;
    Poco::Timespan span,wait_span;
    Poco::Timestamp start,curTime;
    int valid_num = ValideTCNum();
    if(valid_num == 1 ) //ֻ�������Լ����ߵ������ֻ�ȴ�100ms
    {
        wait_span.assign(0,100000); //100ms
    }
    else //�ж�̨�ӻ�������µȴ�500ms
    {
        wait_span.assign(0,500000); //500ms
    }
    //DBG("Valid TC Num = %d\n",valid_num);
    span = curTime - start;
    while(span < wait_span)
    {
        curTime.update();
        span = curTime - start;
        //CTX_DBG("Master Wait Recv RtMsg\n");
        while (CDianTai::Get().GetMessage(msg)) {
                msg_count++;
                //CTX_DBG("Master recv %d RtMsg %d,context=%s\n",msg_count+1,msg.wType,msg.context.c_str());
                DistillData(msg.context,ID);
                RecieveValidData=true;
        }
        if(RecieveValidData)
        {
            //CTX_DBG("Master Recved %d Data\n",msg_count);
            break;
        }else{
            //CTX_DBG("Master Can't Recved Slave's Ack\n");
        }
        Poco::Thread::sleep(10);
    }
    if(!RecieveValidData)
    {
        //DBG("Master can not receive slave ack \n");
    }

    MaxTCNo = MaxNo(); //��ȡ��Ч������������һ������
    //CTX_DBG("Master MaxTCNo=%d RightNo=%d\n",MaxTCNo,RightNo);
    //�ж��ܷ���յ���������Ļ�Ӧ��Ϣ
    if(MaxTCNo == RightNo){//��������������һ������ �����ҷ��͹��������һ���µ�������ָ��͵ȴ������¼��������Ļ�Ӧ
        //CTX_DBG("Master wait New Add TC %d slave's ack \n",RightNo);
        //�ȴ�200ms�����¼�����������Ӧ��Ϣ
        //Poco::Thread::sleep(200);

        int repeat = 0;
        while(!CDianTai::Get().GetMessage(msg)){
            repeat++;
            Poco::Thread::sleep(20);
            if(repeat > 10) break;

        }
        if(repeat <= 10){
            DistillData(msg.context,ID);
            CTX_DBG("Master received New Add TC slave's ack id=%s \n",ID.c_str());

            UpdateTCStatus();
        }
    }else{ //�������һ�μ��������������ݵĻ�Ӧ
        //CTX_DBG("Call Slave %d Without AddTC\n",RightNo);
    }
/*
ȡ�����豸�б��е���һ����ţ���Ϊ���е�������ţ��������������������һ�������豸���
��ȡһ��������[�����������]������[�����������]Ϊ0.�����ں������޸��˺����������[RightNo]
*/
    std::string sendInfo = build_qurey_msg();

    //CTX_DBG("Master SendData %s len=%d\n",sendInfo.c_str(),sendInfo.length());
    CDianTai::Get().SendMessage(sendInfo);

}

void CMainCtrl::SaveTowerCraneInfo()
{

    ofstream tcfile(ctx2000_tc_data,std::ios::binary);

    for(int i = 1; i <= TCTotalNum; i++){

        tcfile.write((const char*)&g_TC[i],sizeof(TTjRecord));
    }

    tcfile.close();
#if 0
    tcfile.open("tcorder.red",std::ios::binary);
    for(int i = 1; i <= TcNum; i++){
        tcfile.write((const char*)&tcorder[i],1);
    }
#endif
}
void CMainCtrl::ReadSiteInfo()
{
    wksp[0][0] = cfg.ReadFloat("device","leftup_x",0);
    wksp[0][1] = cfg.ReadFloat("device","leftup_y",0);
    wksp[1][0] = cfg.ReadFloat("device","leftdown_x",0);
    wksp[1][1] = cfg.ReadFloat("device","leftdown_y",0);
    wksp[2][0] = cfg.ReadFloat("device","rightup_x",0);
    wksp[2][1] = cfg.ReadFloat("device","rightup_y",0);
    wksp[3][0] = cfg.ReadFloat("device","rightdown_x",0);
    wksp[3][1] = cfg.ReadFloat("device","rightdown_y",0);

    czwzb[0][0] =  cfg.ReadFloat("device","cz_x0",0);
    czwzb[0][1] =  cfg.ReadFloat("device","cz_y0",0);
    czwzb[1][0] =  cfg.ReadFloat("device","cz_x1",0);
    czwzb[1][1] =  cfg.ReadFloat("device","cz_y1",0);

    YNAngle  = cfg.ReadFloat("device","YNAngle",0);
    BrakeDis = cfg.ReadFloat("device","BrakeDis",0);
    DangerDis= cfg.ReadFloat("device","DangerDis",0);
    WarnDis  = cfg.ReadFloat("device","WarnDis",0);
    VWarnDis = cfg.ReadFloat("device","VWarnDis",0);
    VStopDis = cfg.ReadFloat("device","VStopDis",0);
    AddAngle = cfg.ReadFloat("device","AddAngle",0);
    FDTime   = cfg.ReadInteger ("device","fd_time",0);
    BrakeTime= cfg.ReadFloat("device","brake_time",0);

}
bool CMainCtrl::is_circle_interset(double x1, double y1, double x2, double y2, double r1, double r2)
{
   double len = sqrt( (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2) );

   return ( len < (r1+r2) );
}
int  CMainCtrl::GetValidTjList(std::vector<int> &devlist)
{
    int cnt = 0;
    for(int i = 0; i < 21; i++)
    {
        std::string serial = g_TC[i].Serial;
        if(Poco::trim(serial).length() != 0)
        {
            devlist.push_back(i);
            cnt++;
        }
    }
    return cnt;
}
int  CMainCtrl::GetConflictTjList(std::vector<int> &devlist)
{
    int cnt = 0;
    for(int i = 0; i < 21; i++)
    {
        if(i == g_local_id)
            continue;
        std::string serial = g_TC[i].Serial;
        if(Poco::trim(serial).length() == 0)
            continue;
        if( is_circle_interset(g_TC[i].x,g_TC[i].y,g_TC[g_local_id].x,g_TC[g_local_id].y,g_TC[i].LongArmLength,g_TC[g_local_id].LongArmLength))
        {
            devlist.push_back(i);
            cnt++;
        }
    }
    return cnt;
}
void      CMainCtrl::CreateDefaultTjParam()
{

    strcpy(g_TC[1].Serial,"C8002");
    g_TC[1].x=1;
    g_TC[1].y=300;
    g_TC[1].LongArmLength=100;
    g_TC[1].ShortArmLenght=20;

    strcpy(g_TC[2].Serial,"C8003");
    g_TC[2].x=100;
    g_TC[2].y=200;
    g_TC[2].LongArmLength =100;
    g_TC[2].ShortArmLenght=20;

    strcpy(g_TC[3].Serial,"C8004");
    g_TC[3].x=300;
    g_TC[3].y=1;
    g_TC[3].LongArmLength =105;
    g_TC[3].ShortArmLenght=20;

    SaveTowerCraneInfo();
}

void CMainCtrl::PushErrorMsg(std::string msg)
{
    CTX_DBG("%s\n",msg.c_str());
    m_errList.push_back("******error*****: " + msg);
}
void CMainCtrl::PushWarnMsg(std::string  msg)
{
    CTX_DBG("%s\n",msg.c_str());
    m_warnList.push_back("******warn******: " + msg);
}
bool CMainCtrl::ReloadParam()
{
    TCTotalNum  = 20;
    cfg.Open(ctx2000_cfg);
    gMainMenuIndex = cfg.ReadInteger("display","mainmenu",1);
    TCTypeName   = Poco::trim(cfg.ReadString("device","tctype","def_typename"));
    CurID        = "0";
    CurSerial    = Poco::trim(cfg.ReadString("device","Serial","def_serial"));
    StrTCArmLen  = Poco::trim(cfg.ReadString("device","armlen","70"));
    StrTCRate   = Poco::trim(cfg.ReadString("device","rate","1"));
    fd_time      = cfg.ReadInteger("device","fd_time",   2);
    brake_time   = cfg.ReadInteger("device","brake_time",2);
    gprs_remote_ip = Poco::trim(cfg.ReadString("gprs","ip","118.123.17.25"));
    gprs_dtu_id    = Poco::trim(cfg.ReadString("gprs","dtuid","001"));
    gprs_remote_port = cfg.ReadInteger("gprs","port",56000);

    g_show_speed      = cfg.ReadBool("display","speed",   false);
    g_show_dg_height  = cfg.ReadBool("display","height",  false);
    g_show_max_weight = cfg.ReadBool("display","weight",  false);
    g_alarm_wild_speed= cfg.ReadInteger("display","alarm_speed",13);
    if(!Poco::NumberParser::tryParse(StrTCRate,g_tc_rate))
    {
        std::string msg = Poco::format("Invalid Fall=%s and reset Fall to 1\n",StrTCRate);
        PushWarnMsg(msg);
        StrTCRate = "1";
        g_tc_rate    = 1;
    }
    encoder_addr = cfg.ReadInteger("device","encoder_addr",0);
    g_encoder_com = Poco::trim(cfg.ReadString("serial","encoder_com","/dev/ttymxc0"));
    g_ad_com      = Poco::trim(cfg.ReadString("serial","ad_com","/dev/ttymxc1"));
    g_diantai_com = Poco::trim(cfg.ReadString("serial","diantai_com","/dev/ttymxc2"));


    g_gprs_com   = Poco::trim(cfg.ReadString("serial","gprs_com","/dev/ttymxc3"));
    g_gps_com    = Poco::trim(cfg.ReadString("serial","gps_com","/dev/tts1"));
    std::cerr << "type: "      << TCTypeName  <<  std::endl;
    std::cerr << "ArmLen: "    << StrTCArmLen <<  std::endl;
    std::cerr << "Fall: "      << StrTCRate  <<  std::endl;
    std::cerr << "CurSerial: " << CurSerial   <<  std::endl;

//�����趨�ĵ�ǰ�������ͺͱ��ʻ��г��ȼ����غ����߱�
    if( !CTorQueMgr::get ().loadCfg (TCTypeName,StrTCArmLen,StrTCRate))
    {
        PushWarnMsg("Load Moment Data Failed");
    }



    ifstream tcfile;
//������������,�����ҵ�����ID
    tcfile.open(ctx2000_tc_data,std::ios::binary|std::ios::in);
    bool FindLocalId = false;
    if(tcfile.good())
    {
        std::cerr << "open tc.red" << std::endl;
        TcNum = 0;
        while(!tcfile.eof()){
            if(TcNum>(TCTotalNum-1))
                break;
            TcNum++;
            tcfile.read((char*)&g_TC[TcNum],sizeof(TTjRecord));
            CTX_DBG("TC[%d].Serial=%s height=%6.2f\n",TcNum,g_TC[TcNum].Serial,\
                    g_TC[TcNum].Height);
            g_TC[TcNum].Angle      = 0;
            g_TC[TcNum].AngleSpeed = 0;
            g_TC[TcNum].Dang       = 0;

            if(g_TC[TcNum].Dyna)
                g_TC[TcNum].Position   = g_TC[TcNum].LongArmLength;
            std::string  Serial = g_TC[TcNum].Serial;
            if(Poco::trim(Serial) == CurSerial){
                CurID = Poco::format("%d",TcNum);
                m_local_id = TcNum;
                g_local_id = m_local_id;
                CTX_DBG("Find CurID:%s\n",CurID.c_str());
                FindLocalId =  true;
            }
            g_TC[TcNum].InfoTime = 0; //fix ��Ϊ�ճ�ʼ�����������ݽ���ʱ��Ϊ0��������״̬���µ�ʱ������Ϊ������״̬
            //g_TC[TcNum].InfoTime.update();
        }
        tcfile.close();
    }else{
        CreateDefaultTjParam();
        PushWarnMsg("Can not open tc.red CreateDefaultTjParam");
    }
    if(!FindLocalId)
    {
        CurID     = "1";
        m_local_id = 1;
        g_local_id = 1;
        PushErrorMsg(Poco::format("Cant Find LocalId for %s",CurSerial));
        return false;
    }
//���ع��ز�����Ϣ
    ReadSiteInfo();
//���ر���������Ϣ
    memset(&wba[0],0,11*sizeof(Twb));
    tcfile.open(ctx2000_protect_data,std::ios::binary);
    int count = 0;
    if(tcfile.good())
    {
        DebugMsg("load Diving Info ok");

        while(!tcfile.eof()){
            //if(count < 11)
            {
                tcfile.read((char*)&wba[count++],sizeof(Twb));
                CTX_DBG("wba[%d] VertexNum=%d height=%6.2f\n",count,wba[count].VertexNum,wba[count].h);
            }
        }
        tcfile.close();
    }else{
        PushWarnMsg("protect.red open failed");
    }
    CTX_DBG("Total wbNum=%d Vaild Num=%d\n",wbNum,count);

    for (int i = 1; i <= TCTotalNum ; i++)
    {
         g_TC[i].DHeight=0;
         g_TC[i].Valide=false;
    }
#if 0
    //���طֽ�������Ϣ
    tcfile.open("DivideLine.tc",std::ios::in);
    if(tcfile.good())
    {
        DBG("DivideLine.tc open ok\n");
        DividePointNum = 0;
        while(!tcfile.eof()){
            tcfile >> DividePoint[DividePointNum].x >> DividePoint[DividePointNum].y;
            DBG("DividePoint[%d] x=%6.2f y=%6.2f\n",DividePointNum,DividePoint[DividePointNum].x, DividePoint[DividePointNum].y);

            DividePointNum++;
        }
        tcfile.close();
    }else
        DBG("DivideLine.tc open failed\n");
#endif
//��ȡ��Ч������Ϣ,������tc.red�ļ��д����豸���кŵ�����
    int ret = GetValidTjList(g_valid_tj_list);
    CTX_DBG("Total Tower Number = %d\n",ret);
    for(int i = 0 ; i < ret; i++)
    {
        CTX_DBG("valid id[%d] = %d\n",i,g_valid_tj_list.at(i));
    }
//�����������Ѿ���ȡ�ɹ����ٻ�ȡ�뱾��id��ͻ���豸id�б�
    ret = GetConflictTjList(g_conflict_tj_list);
    CTX_DBG("Conflict Tower Number = %d\n",ret);
    for(int i = 0 ; i < ret; i++)
    {
        CTX_DBG("conflict localid[%d] : id[%d] = %d\n",g_local_id,i,g_conflict_tj_list.at(i));
    }
    //��ʼ���궨ϵ��
    InitBDParam();
//����ABC�Ƕ�
    calc_angle(czwzb,g_TC[g_local_id].x,g_TC[g_local_id].y,g_angle_A,g_angle_B,g_angle_C);
    CTX_DBG("angleA=%0.2f,angleB=%0.2f,angleC=%0.2f\n",g_angle_A,g_angle_B,g_angle_C);
    InitAlgoData();

    return true;
}
void CMainCtrl::InitBDParam()
{
    g_bd[BD_ANGLE].bd_k =cfg.ReadFloat("angle_bd","angle_k",1);
    g_bd[BD_ANGLE].start_value =cfg.ReadFloat("angle_bd","a_angle",0);
    g_bd[BD_ANGLE].zero_ad =cfg.ReadInteger("angle_bd","zero_ad",0);
    g_bd[BD_ANGLE].bd_ad   = cfg.ReadInteger("angle_bd","bd_ad",0);

    g_bd[BD_UP_ANGLE].bd_k =cfg.ReadFloat("up_angle_bd","up_angle_k",1);
    g_bd[BD_UP_ANGLE].start_value =cfg.ReadFloat("up_angle_bd","min_up_angle",0);
    g_bd[BD_UP_ANGLE].zero_ad =cfg.ReadInteger("up_angle_bd","zero_ad",0);
    g_bd[BD_UP_ANGLE].bd_ad   = cfg.ReadInteger("up_angle_bd","bd_ad",0);


    g_bd[BD_WEIGHT].bd_k =cfg.ReadFloat("weight_bd","weight_k",1);
    g_bd[BD_WEIGHT].start_value =cfg.ReadFloat("weight_bd","min_weight",0);
    g_bd[BD_WEIGHT].zero_ad =cfg.ReadInteger("weight_bd","zero_ad",0);
    g_bd[BD_WEIGHT].bd_ad   = cfg.ReadInteger("weight_bd","bd_ad",0);
    g_bd[BD_WEIGHT].valid   = cfg.ReadBool ("weight_bd","valid",false);

    g_bd[BD_WEIGHT2].bd_k        = cfg.ReadFloat("weight_bd2","weight_k",1);
    g_bd[BD_WEIGHT2].start_value = cfg.ReadFloat("weight_bd2","min_weight",0);
    g_bd[BD_WEIGHT2].zero_ad     = cfg.ReadInteger("weight_bd2","zero_ad",0);
    g_bd[BD_WEIGHT2].bd_ad       = cfg.ReadInteger("weight_bd2","bd_ad",0);
    g_bd[BD_WEIGHT2].valid       = cfg.ReadBool ("weight_bd2","valid",false);

    g_bd[BD_WEIGHT3].bd_k        = cfg.ReadFloat("weight_bd3","weight_k",1);
    g_bd[BD_WEIGHT3].start_value = cfg.ReadFloat("weight_bd3","min_weight",0);
    g_bd[BD_WEIGHT3].zero_ad     = cfg.ReadInteger("weight_bd3","zero_ad",0);
    g_bd[BD_WEIGHT3].bd_ad       = cfg.ReadInteger("weight_bd3","bd_ad",0);
    g_bd[BD_WEIGHT3].valid       = cfg.ReadBool ("weight_bd3","valid",false);


    g_bd[BD_CAR_DIST].bd_k =cfg.ReadFloat("dist_bd","dist_k",1);
    g_bd[BD_CAR_DIST].start_value =cfg.ReadFloat("dist_bd","min_dist",0);
    g_bd[BD_CAR_DIST].zero_ad =cfg.ReadInteger("dist_bd","zero_ad",0);
    g_bd[BD_CAR_DIST].bd_ad   = cfg.ReadInteger("dist_bd","bd_ad",0);


    g_bd[BD_HEIGHT].bd_k = cfg.ReadFloat("height_bd","height_k",1);
    g_bd[BD_HEIGHT].start_value = cfg.ReadFloat("height_bd","min_height",0);
    g_bd[BD_HEIGHT].zero_ad = cfg.ReadInteger("height_bd","zero_ad",0);
    g_bd[BD_HEIGHT].bd_ad   = cfg.ReadInteger("height_bd","bd_ad",0);

    g_bd[BD_SPEED].bd_k = 1;
    g_bd[BD_ANGLE_X].bd_k = 1;
    g_bd[BD_ANGLE_X].zero_ad = cfg.ReadFloat("angle_x_bd","zero_ad",2500*51/71);
    g_bd[BD_ANGLE_Y].bd_k = 1;
    g_bd[BD_ANGLE_Y].zero_ad = cfg.ReadFloat("angle_y_bd","zero_ad",2500*51/71);

    for(int i = 0; i < BD_MAX; i++)
    {
        if(fabs(g_bd[i].bd_k) < 0.0001)
        {
            std::string msg = Poco::format("[%d] Bd Invalid k=%0.6f",i,g_bd[i].bd_k);
            PushErrorMsg(msg);
            g_bd[i].bd_k    = 1;
        }
    }

}
void CMainCtrl::InitAlgoData()
{
    for(int i = 0; i < NUMBER_OF_QTZ; i++)
    {
        g_qtzs[i].m_number           = i+1;
        g_qtzs[i].m_height           = g_TC[i+1].Height;
        g_qtzs[i].m_long_arm_len     = g_TC[i+1].LongArmLength;
        g_qtzs[i].m_short_arm_len    = g_TC[i+1].ShortArmLenght;
        g_qtzs[i].m_coord.x          = g_TC[i+1].x;
        g_qtzs[i].m_coord.y          = g_TC[i+1].y;
        //YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
        g_qtzs[i].m_brake_dist       = BrakeDis;
        g_qtzs[i].m_stop_dist        = DangerDis;
        g_qtzs[i].m_slowdown_dist    = WarnDis;
        g_qtzs[i].m_carrier_slowdown_dist= VWarnDis;
        g_qtzs[i].m_carrier_brake_dist   = VStopDis;
        if(g_TC[g_local_id].Dyna)
            g_qtzs[i].m_sarm_cc_len      = g_TC[g_local_id].a0;
        else
            g_qtzs[i].m_sarm_cc_len      = 0;

//����ʵʱ����
        g_qtzs[i].m_sarm_angle       = 0; //��������,ʵʱ����
        g_qtzs[i].m_carrier_pos      = 0;
        g_qtzs[i].m_long_arm_angle   = 0;

        CTX_DBG("g_qtzs[%d] car_pos=%0.2f height=%0.2f longarm=%0.2f shortarm=%0.2f,x=%0.2f y=%0.2f brake_dist=%0.2f stop_dist=%0.2f slowdown_dist=%0.2f v_slow_dist=%0.2f v_brake_dist=%0.2f\n",\
                i,g_qtzs[i].m_carrier_pos,g_qtzs[i].m_height,g_qtzs[i].m_long_arm_len,g_qtzs[i].m_short_arm_len,\
                g_qtzs[i].m_coord.x,g_qtzs[i].m_coord.y,BrakeDis,DangerDis,WarnDis,VWarnDis,VStopDis);
    }

    int local_tower_id =  g_local_id - 1;
    if(local_tower_id >= 0)
    {
        for(size_t i = 0; i < g_conflict_tj_list.size();i++)
        {
            int id = g_conflict_tj_list.at(i);
            if(id < NUMBER_OF_QTZ)
            {
                CTX_DBG("local TC[%d] Conflict with TC[%d]\n",g_local_id,id);
                g_qtzs[local_tower_id].QtzAddDetect(&g_qtzs[id-1]);

            }
        }
    }
    /*
    ��ʼ��������
*/
    CTX_DBG("Loading Diving Paramter\n");
    g_polys.clear();
    for(int i=0; i < wbNum; i++)
    {

        if( ( wba[i].VertexNum > 0 ) && ( wba[i].VertexNum < 6 ) )
        {
            CTX_DBG("Add Diving[%d] VertexNum=%d Height=%0.2f\n",i,wba[i].VertexNum,wba[i].h);
            PolyDef poly;
            poly.clear();
            poly.height  = wba[i].h;
            poly.is_wall = false;
            for(int j =0; j < wba[i].VertexNum;j++)
            {
                double x = wba[i].Pointxy[j][0];
                double y = wba[i].Pointxy[j][1];
                Coord z;
                z.x = x;
                z.y = y;
                poly.push_back(z);

                CTX_DBG("Divding[%d] Vertex[%d]  X=%0.2f Y=%0.2f\n",i,j,x,y);
            }
            //һ��Ҫʹ�����ڱպ�״̬�������㷨ģ��������
            poly.push_back(poly[0]);

            g_polys.push_back(poly);
        }
    }
    if(local_tower_id >= 0)
    {
        for(size_t i = 0; i < g_polys.size(); i++)
        {
            g_qtzs[local_tower_id].QtzAddDetect( &g_polys[i]);
            //for(size_t j = 0 ; j < g_polys[i].size(); j++)
            //    CTX_DBG("x=%0.2f y=%0.2f\n",g_polys[i].at(j).x,g_polys[i].at(j).y);
        }

    }

}
void CMainCtrl::SaveSiteInfo()
{
    cfg.WriteFloat("device","leftup_x",wksp[0][0]);
    cfg.WriteFloat("device","leftup_y",wksp[0][1]);
    cfg.WriteFloat("device","leftdown_x",wksp[1][0]);
    cfg.WriteFloat("device","leftdown_y",wksp[1][1]);
    cfg.WriteFloat("device","rightup_x",wksp[2][0]);

    cfg.WriteFloat("device","rightup_y",wksp[2][1]);
    cfg.WriteFloat("device","rightdown_x",wksp[3][0]);
    cfg.WriteFloat("device","rightdown_y",wksp[3][1]);


    cfg.WriteFloat("device","cz_x0",czwzb[0][0]);
    cfg.WriteFloat("device","cz_y0",czwzb[0][1]);

    cfg.WriteFloat("device","cz_x1",czwzb[1][0]);
    cfg.WriteFloat("device","cz_y1",czwzb[1][1]);

    cfg.WriteFloat("device","YNAngle",YNAngle);
    cfg.WriteFloat("device","BrakeDis",BrakeDis);
    cfg.WriteFloat("device","DangerDis",DangerDis);

    cfg.WriteFloat("device","WarnDis",WarnDis);
    cfg.WriteFloat("device","VStopDis",VStopDis);
    cfg.WriteFloat("device","VWarnDis",VWarnDis);
    cfg.WriteFloat("device","AddAngle",AddAngle);

    cfg.WriteInteger("device","fd_time",FDTime);
    cfg.WriteInteger("device","brake_time",BrakeTime);
}
void CMainCtrl::SaveDivideInfo()
{
#if 0
    ofstream divfile("DivideLine.tc");

    if(divfile.good()){
        for (int i = 0; i < DividePointNum ; i++)
        {
            divfile<<DividePoint[i].x<<" "<<DividePoint[i].y<<std::endl;
        }
    }


    FILE* fp = fopen("DivideLine.tc","wb+");
    for(int i = 0 ;i < DividePointNum;i++)
    {
        fwrite((const void*)&DividePoint[i],sizeof(TDividePoint),1,fp);
    }
    fclose(fp);
#endif
}
void CMainCtrl::SaveBuildingInfo()
{
    ofstream tcfile(ctx2000_protect_data,std::ios::binary);


    for(int i = 0; i < 10; i++){
        tcfile.write((const char*)&wba[i],sizeof(Twb));
    }

    tcfile.close();
/*
    FILE* fp = fopen(ctx2000_protect_data,"wb");
    fseek(fp,0,SEEK_SET);

    for(int i = 0 ;i < wbNum-1;i++)
    {
        fwrite((const void*)&wba[i],sizeof(Twb),1,fp);
    }


    fclose(fp);
    */
    CTX_DBG("wbnum=%d\n",wbNum);
}
void CMainCtrl::SaveWorksiteInfomation()
{
    SaveTowerCraneInfo();
    SaveSiteInfo();
    SaveDivideInfo();
    SaveBuildingInfo();
}
/*
����߶���Ϣ�ϴ�
*/
bool CMainCtrl::DealHeightInfo()
{
    CTX_Message msg;
    int HTCNo;
    double TCHeight;
    //DBG("%s %d\n",__FUNCTION__,__LINE__);
    if(CDianTai::Get().GetMessage(msg,2)){
        CTX_DBG("Recv HeighInfo =%s\n",msg.context.c_str());
        Poco::StringTokenizer token(msg.context,"N");
        if(token.count() != 2)
        {
            CTX_DBG("Error Height\n");
            return false;
        }
        if(!Poco::NumberParser::tryParse(token[0],HTCNo))         {
            CTX_DBG("Error No %s\n",token[0].c_str());
            return false;
        }
        if(!Poco::NumberParser::tryParseFloat(token[1],TCHeight)) {
            CTX_DBG("Error Height %s \n",token[1].c_str());
            return false;
        }
        CTX_DBG("Modify TC[%d] Height=%6.2f\n",HTCNo,TCHeight);

        g_TC[HTCNo].Height = TCHeight;
        SaveTowerCraneInfo();
        //Read
        Poco::Thread::sleep(m_local_id*100);
        std::string sendInfo="!"+CurID;
        CDianTai::Get().SendMessage(sendInfo);

        if(m_mode == mode_master)
        {
            //Poco::Thread::sleep(m_local_id*1000);
            CTX_DBG("Master[%d] DealHeightInfo OK\n",m_local_id);
            //Poco::Thread::sleep(3000);

        }else{
            CTX_DBG("Slave[%d] DealWorkSiteInfo OK\n",m_local_id);
        }
        Poco::Thread::sleep(m_local_id*1000);

        TGuiMsg msg(2,"Restart App");
        TGuiNotifyer::Get().Notify(&msg);

        //restart app
    }
    return true;
    //DBG("%s %d\n",__FUNCTION__,__LINE__);
}
bool  CMainCtrl::DiveideSettingInfomation(std::string &msg)
{
    int curpos=0;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 2; j++)
        {
            if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_wksp[i][j]))
            {
                CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
                return false;
            }

            curpos+=8;
        }
    }
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++)
        {
            if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_czwzb[i][j]))
            {
                CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
                return false;
            }

            curpos+=8;
        }
    }

    if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_YNAngle))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=8;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_VStopDis))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=8;

    if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_VWarnDis))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=8;

    if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_BrakeDis))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=8;

    if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_DangerDis))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=8;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_WarnDis))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=8;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_AddAngle))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=8;

    if(!Poco::NumberParser::tryParse(Poco::trim(msg.substr(curpos,2)),tmp_TcNum))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=2;
    if( (tmp_TcNum < 1) || (tmp_TcNum >20 ))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
//������Ϣ
    for(int i = 1 ; i <= tmp_TcNum; i++ ){
        memset(tmp_TC[i].Serial,0,20);
        msg.substr(curpos,8).copy(tmp_TC[i].Serial,8);
        curpos+=8;
        CTX_DBG("serial=%s\n",tmp_TC[i].Serial);
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].x))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].y))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].Height))
        {
            std::string height = msg.substr(curpos,8);
            CTX_DBG("Invalid Param %s %d %s\n",__FUNCTION__,__LINE__,height.c_str ());
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].LongArmLength))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        CTX_DBG("left=%s\n",msg.substr(curpos,50).c_str());
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].ShortArmLenght))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        CTX_DBG("left=%s\n",msg.substr(curpos,50).c_str());
        int  dy_flag = 0;
        std::string tmp = Poco::trim(msg.substr(curpos,1));
        if(!Poco::NumberParser::tryParse(tmp,dy_flag))
        {
            CTX_DBG("Invalid Param %s %d dy_flag=%s\n",__FUNCTION__,__LINE__,tmp.c_str());

            return false;
        }
        if(dy_flag==1) tmp_TC[i].Dyna = true;
        else           tmp_TC[i].Dyna = false;
        curpos+=1;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].Rs))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].L1))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].L2))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].a0))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].Dang))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_TC[i].L))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
    }

 //����������Ϣ
    if(!Poco::NumberParser::tryParse(Poco::trim(msg.substr(curpos,2)),tmp_wbNum))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=2;
    if( (tmp_wbNum < 1) || (tmp_wbNum >6 ))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    for(int i = 0 ; i < tmp_wbNum; i++ ){
        if(!Poco::NumberParser::tryParse(Poco::trim(msg.substr(curpos,2)),tmp_wba[i].VertexNum))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=2;

        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_wba[i].h))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;

        for(int j = 0; j < tmp_wba[i].VertexNum; j++){
            if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_wba[i].Pointxy[j][0]))
            {
                CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
                return false;
            }
            curpos+=8;
            if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_wba[i].Pointxy[j][1]))
            {
                CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
                return false;
            }
            curpos+=8;
        }
    }

    if(!Poco::NumberParser::tryParse(Poco::trim(msg.substr(curpos,2)),tmp_DividePointNum))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    curpos+=2;
    if( (tmp_DividePointNum < 1) || (tmp_DividePointNum >10 ))
    {
        CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
        return false;
    }
    for(int i = 0 ; i < tmp_DividePointNum; i++){
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_DividePoint[i].x))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
        if(!Poco::NumberParser::tryParseFloat(Poco::trim(msg.substr(curpos,8)),tmp_DividePoint[i].y))
        {
            CTX_DBG("Invalid Param %s %d\n",__FUNCTION__,__LINE__);
            return false;
        }
        curpos+=8;
    }

    /*
        ���²�����ʱ����ü���д������ֹ�������������޸���ȫ�ֲ���
    */
    memcpy(wksp, tmp_wksp, sizeof(wksp));
    memcpy(czwzb,tmp_czwzb,sizeof(czwzb));
    memcpy(g_TC,tmp_TC,    sizeof(g_TC));
    memcpy(wba,tmp_wba,    sizeof(wba));
    memcpy(DividePoint,tmp_DividePoint,sizeof(DividePoint));

    YNAngle  =tmp_YNAngle;
    VStopDis =tmp_VStopDis;
    VStopDis =tmp_VStopDis;
    VWarnDis =tmp_VWarnDis;
    BrakeDis =tmp_BrakeDis;
    DangerDis=tmp_DangerDis;
    WarnDis  = tmp_WarnDis;
    AddAngle = tmp_AddAngle;
    TcNum    = tmp_TcNum;
    wbNum    = 10;//tmp_wbNum;
    DividePointNum = tmp_DividePointNum;
    return  true;
}
/*
��������Ϣ�ϴ�
*/
bool CMainCtrl::DealWorkSiteInfo()
{
    CTX_Message msg;
    if(CDianTai::Get().GetMessage(msg,3)){
        if(!DiveideSettingInfomation(msg.context))
        {
            CTX_DBG("=======Invalid Upload Parameter\n");
            return false;
        }
        SaveWorksiteInfomation();
        Poco::Thread::sleep(m_local_id*100);
        std::string sendInfo = "!"+CurID;
        CDianTai::Get().SendMessage(sendInfo);
        if(m_mode == mode_master)
        {
            //Poco::Thread::sleep(m_local_id*1000);
            CTX_DBG("Master[%d] DealWorkSiteInfo OK\n",m_local_id);
            Poco::Thread::sleep(3000);

        }else{
            CTX_DBG("Slave[%d] DealWorkSiteInfo OK\n",m_local_id);
        }
        TGuiMsg msg(2,"Restart App");
        TGuiNotifyer::Get().Notify(&msg);
        return true;
        //restart app
    }
    return false;
}
void CMainCtrl::ParseCollideStatus()
{
    int slewing = 0, trolley=0;
#if 0
    for(int i = 0 ;i < 12; i++)
    {
        if(m_control_state.b[i] != m_old_ctrl_state.b[i])
        {
            CJDQAdmin::Get().Service((CTX_JDQ)i,m_control_state.b[i]);
            PostMessage(GetActiveWindow(),MSG_CONTRL_MSG,(WPARAM)i,(LPARAM)(m_control_state.b[i]?1:0));
        }
    }
#endif
    if(m_control_state.b1 != m_old_ctrl_state.b1){

        if(m_control_state.b1) //�һ�ת����
        {
            trolley |= 0x1;
        }
    }
    if(m_control_state.b2 != m_old_ctrl_state.b2){

        if(m_control_state.b2) //��ת�ƶ�
        {
            //trolley |= 0x2;
            trolley |= 0x1;
        }
    }
    if(m_control_state.b3 != m_old_ctrl_state.b3){

        if(m_control_state.b3)//���ת����
        {
            //trolley |= 0x4;
            trolley |= 0x1;
        }
    }
    if(m_control_state.b4 != m_old_ctrl_state.b4){

        if(m_control_state.b4) //С�������������
        {
            //trolley |= 0x1;
            slewing |= 0x2;
         }
    }
    if(m_control_state.b5 != m_old_ctrl_state.b5){

        if(m_control_state.b5) //С����������ͣ��
        {
             slewing |= 0x2;
        }
    }
    if(m_control_state.b6 != m_old_ctrl_state.b6){
        if(m_control_state.b6){ //С����������ͣ��
            //slewing |= 0x4;
            slewing |= 0x2;
        }
    }
    if(m_control_state.b7 != m_old_ctrl_state.b7){
        if(m_control_state.b7){ //С�����ڸ�������
            //slewing |= 0x8;
            slewing |= 0x2;
        }
    }
    if(m_control_state.b9 != m_old_ctrl_state.b9){
        if(m_control_state.b9){ //���ת��������
            //trolley |= 0x8;
            trolley |= 0x1;
        }
    }
    if(m_control_state.b10 != m_old_ctrl_state.b10){
        if(m_control_state.b10){//�һ�ת��������
            //trolley |= 0x16;
            trolley |= 0x1;
        }
    }

    m_old_ctrl_state=m_control_state ;

    if( (trolley !=0 ) || (slewing != 0))
    {
        if(m_dbadmin)
        {
            if(!m_dbadmin->PostMessage(new QCollisionNotification(0,slewing,trolley)))
            {
                CTX_DBG("Post Collision Message to DBAdmin Failed\n");
            }
        }
        //Poco::NotificationQueue::defaultQueue().enqueueNotification();
    }
}
bool      CMainCtrl::NotifyBypass(bool on)
{
    if(m_dbadmin)
    {
        if(!m_dbadmin->PostMessage(new QCollisionNotification(0,0xAA,on?1:0)))
        {
            CTX_DBG("Post Collision Message to DBAdmin Failed\n");

        }
        return true;
    }
    return false;
}
/*
������ط�����㷨ģ���ʵʱ����
*/
void CMainCtrl::CollideDetect()
{

    int index= g_local_id-1;
    int id   = 0;
//�ñ�����ʵʱ���������±������㷨��ʵʱ����,����������ɱ�������ˢ��ʱ��ʱ������Ϊ��200ms����ʱ
    //CTX_DBG("g_local_id=%d angle=%0.2f\n",g_local_id,g_TC[g_local_id].Angle);
    /*
    g_TC[g_local_id].Angle    = g_angle*3.14/180;

    if(g_TC[g_local_id].Dyna){ //�����Ƕ���ʽ
        g_TC[g_local_id].Dang     = g_up_angle;
        g_TC[g_local_id].Position = g_TC[g_local_id].LongArmLength;
    }
    else{//������ƽ��ʽ
        g_TC[g_local_id].Dang     = 0;
        g_TC[g_local_id].Position = g_car_dist;
    }*/
//���ݲɼ����ݸ����㷨ģ���еı���ʵʱ����
    if( ( index >= 0) && (index < NUMBER_OF_QTZ) )
    {
        g_qtzs[index].m_long_arm_angle   = g_angle;
        if(g_TC[g_local_id].Dyna){//�����ǰ�Ƕ���ʽ
            //���㷨ģ��ķ��Ⱦ����趨�Ĵ�۵ĳ��ȣ����۵�֧�㵽�۶˵ľ���
            g_qtzs[index].m_carrier_pos  = g_TC[g_local_id].LongArmLength;
            g_qtzs[index].m_sarm_angle   = g_up_angle;
        }
        else{
            g_qtzs[index].m_carrier_pos  = g_car_dist;
            g_qtzs[index].m_sarm_angle   = 0;
        }
        g_qtzs[index].m_online   = true; //����ʼ������
        //CTX_DBG("local [%d] angle=%0.2f dang=%0.2f h=%0.2f p=%0.2f\n",index+1,g_qtzs[index].m_long_arm_angle,g_qtzs[index].m_sarm_angle,g_qtzs[index].m_height,g_qtzs[index].m_carrier_pos);
    }
//���ݵ�̨��������Χ������ʵʱ���������³�ͻ�������㷨ģ����Ҫ��ʵʱ����
    for(size_t i = 0; i < g_conflict_tj_list.size(); i++)
    {
        id = g_conflict_tj_list.at(i)-1;
        if( (id >= 0) &&  (id < NUMBER_OF_QTZ))
        {
            g_qtzs[id].m_long_arm_angle   = g_TC[id+1].Angle*180.0f/3.14f;//
            g_qtzs[id].m_carrier_pos      = g_TC[id+1].Position;
            g_qtzs[id].m_sarm_angle       = g_TC[id+1].Dang;
            g_qtzs[id].m_online           = g_TC[id+1].Valide; //��������������״̬
        }
        //CTX_DBG("tj conflict[%d] angle=%0.2f dang=%0.2f h=%0.2f p=%0.2f \n",id+1,g_qtzs[id].m_long_arm_angle,g_qtzs[id].m_sarm_angle,g_qtzs[id].m_height,g_qtzs[id].m_carrier_pos);
    }

    QtzCollideDetectOne(&g_qtzs[index]);
    m_control_state = g_qtzs[index].m_controled_status;
    ParseCollideStatus();
    //CTX_DBG("state=%d%d%d%d%d%d%d%d%d%d%d%d\n",m_control_state.b1,m_control_state.b2,m_control_state.b3,\
    //        m_control_state.b4,m_control_state.b5,m_control_state.b6,m_control_state.b7,m_control_state.b8,\
    //        m_control_state.b9,m_control_state.b10,m_control_state.b11,m_control_state.b12);
}

void    CMainCtrl::WildService()
{
    static bool over_flag = false;
    if(g_wild_speed >= g_alarm_wild_speed) //����> 20m/s ҲҪ������¼
    {
        CBeeper::get().BeepMs(1000,100000);
        if(!over_flag){
            SendAlarmData();
        }
        over_flag = true;
    }else{
        over_flag = false;
    }
}
void      CMainCtrl::SendAlarmData()
{
    gprs::tc_data data;
    data.m_has_alarm = true;
    data.m_angle     = g_angle;
    data.m_car_speed = 0;
    data.m_dg_height = g_dg_height;
    data.m_dist      = g_car_dist;
    data.m_fall      = g_tc_rate;
    data.m_gradient_x= g_angle_x;
    data.m_gradient_y=g_angle_y;
    data.m_max_dist  =g_TC[g_local_id].LongArmLength;
    data.m_min_dist  =g_TC[g_local_id].Rs;
    data.m_speed     =g_wild_speed;
    data.m_tc_height =g_TC[g_local_id].Height;
    data.m_up_angle  =g_up_angle;
    data.m_weight    =g_dg_weight;
    data.m_max_weight=g_rated_weight;
    data.m_dg_speed  =  0;
    if(!gprs::get().send_tc_data(0,data))
    {
        CTX_DBG("send_alarm_tc_data failed\n");
    }
}
void   CMainCtrl::SendWetRecord(double qd_max_weight,bool alarm)
{
    gprs::tc_data data;
    data.m_has_alarm = alarm;
    data.m_angle     = g_angle;
    data.m_car_speed = 0;
    data.m_dg_height = g_dg_height;
    data.m_fall      = CTorQueMgr::get ().m_rate_int;
    data.m_dist      = g_car_dist;
    data.m_weight    = qd_max_weight;
    data.m_max_weight= CTorQueMgr::get ().m_rated_weight;
    gprs::get ().send_tc_data (1,data);
}
/*
���ط������
*/
void    CMainCtrl::LjService()
{
    static int  out_state = 0;
    static bool over_flag = false; //���ر�־
    static bool up_flag   = false; //����������ڷ�ֵ0.5t
    static int  alarmType = 0;     //��������ı������� 0��ÿ����������е��������û�г�����غɵ�95% 1����������95%
    static double max_weight = 0;  //ÿ����������е��������
    //static double max_speed  = 0; //��������е�������
    //CTX_DBG("out_status=%d\n",out_state);
    //�������������״̬�����Ƽ̵����Ķ���

    out_state     = CTorQueMgr::get ().getState(g_car_dist,g_dg_weight);
    g_rated_weight= CTorQueMgr::get ().m_rated_weight;
    if(out_state == 4) //���س���105% x > 105%
    {
        CJDQAdmin::Get().Control(JDQ_CAR_OUTSIDE_BREAK,JDQ_OPEN);//���� С����������ͣ��
        CJDQAdmin::Get().Control(JDQ_HOOK_UP_LIMIT,JDQ_OPEN);//���Ƶ�������
        if(!over_flag){
            SendAlarmData();
        }
        over_flag = true;
        CBeeper::get().BeepMs(1000,100000);
    }else if(out_state <= 1 && over_flag ){ //���� x <95%
        CJDQAdmin::Get().Control(JDQ_CAR_OUTSIDE_BREAK,JDQ_CLOSE);
        CJDQAdmin::Get().Control(JDQ_HOOK_UP_LIMIT,JDQ_CLOSE);//���Ƶ�������
        over_flag = false;
    }else if(out_state >= 2 && out_state < 4){ // 95% < x < 105%
        static int delay=0;
        delay++;
        if( (delay%2) == 0)
            CBeeper::get().BeepMs(100,100000);
    }


    if( (g_dg_weight > 0.5) )
    {
            up_flag = true;
            max_weight = (g_dg_weight>max_weight)?g_dg_weight:max_weight;
            if(out_state >= 2 ){ //x > 95% ����95�����ݾ����ھ������ݣ������ݿ���Ҫ������ɫ��ʾ
                alarmType = 1;
            }
    }else if( (g_dg_weight < 0.3) && (up_flag))
    {
            if(m_dbadmin)
            {
                if(!m_dbadmin->PostMessage(new QCollisionNotification(1,g_car_dist,max_weight,g_tc_rate,g_angle,g_wild_speed,alarmType)))
                {
                    CTX_DBG("Post Fall  Message to DBAdmin Failed\n");
                }
            }
            SendWetRecord (max_weight,alarmType?true:false);
            up_flag     = false;
            alarmType   = 0;
            max_weight  = 0;
    }

}

EWorkMode CMainCtrl::GetDevMode()
{
    return m_mode;
}
void    CMainCtrl::DiantaiService()
{
    static int cnt=0;
    if( ((cnt++)%10) == 0)
        CDianTai::Get ().checkUploading ();
}
void CMainCtrl::run()
{
    bool AddState=false;
    set_thread_title("ctx2000.mainctrl");
    m_rdyEvt.set();
    m_quitEvt.reset();
    m_quit = false;

    while (false == AddState) {
        WatchNetWork(MainMachineID,AddState);
    }

    CTX_DBG("watch ok mode=%d mainID=%s\n",m_mode,MainMachineID.c_str());
    while(!m_quit)
    {
        CollideDetect();
        DealWorkSiteInfo();
        DealHeightInfo();
        UpdateTCStatus();
        if(g_show_max_weight)
            LjService();
        if(g_show_speed)
            WildService ();
        DiantaiService();
        if(m_mode == mode_master)
        {
            master_loop();
        }else if (m_mode == mode_slave) {
            slave_loop();
        }else{
            std::cerr << "unkown mode\n";
        }

        //Poco::Thread::sleep(100);

    }
}
CMainCtrl& CMainCtrl::Get()
{
    static SingletonHolder<CMainCtrl> sh;
    return *sh.get();
}
int getRtSignal(sigset_t *mask)
{
    sigemptyset(mask);
    sigaddset(mask,SIGIO);
    return SIGIO;
}
void blockRtSignal()
{
    sigset_t mask;
    int sigNo = getRtSignal(&mask);
    if(sigNo > 0)
        pthread_sigmask(SIG_BLOCK,&mask,NULL);

}

bool CMainCtrl::Stop()
{
    CTX_DBG("==============Begin Stop Ctx2000=============\n");
    m_quit  = true;

    if(false == m_quitEvt.tryWait(1000))
    {
        CTX_DBG("MainCtrl Stop Failed\n");
    }

    if(m_dbadmin){
        if(false == m_dbadmin->stop())
        {
            CTX_DBG("DBAdmin Stop Failed\n");
        }
        delete m_dbadmin;
    }
    if(false == CDataAcquire::Get().Stop())
    {
        CTX_DBG("DataAcquire Stop Failed\n");
    }
    if(false == CDianTai::Get().Stop())
    {
        CTX_DBG("DianTai Stop Failed\n");
    }
    if(false == CJDQAdmin::Get().Stop())
    {
        CTX_DBG("JDQAdmin Stop Failed\n");
    }
    return true;
}
bool CMainCtrl::mainctrl_dev_ctrl_func(int state)
{
    if(state==1){
        CJDQAdmin::Get ().ResetDevice (1);
        CTX_DBG("ResetDevice called -----> wait 5s for reset ok\n");
        Poco::Thread::sleep (5000); //wait 5s for reset ok;
    }
    return true;
}
bool dev_ctrl_func(void* arg, int state)
{
    CTX_DBG("dev_ctrl_func reset gprs\n");
    CMainCtrl* ctrl = (CMainCtrl*)arg;
    if(ctrl)
        return ctrl->mainctrl_dev_ctrl_func (state);
    return false;
}
#define RET_ERR   return false
bool CMainCtrl::Start()
{
//���ǽ����������ݿ�
    if( !CTajiDbMgr::Get().load(ctx2000_db))
    {
        PushErrorMsg("TajiDbMgr load failed");
        RET_ERR;
    }
#if 1
    if(!ReloadParam())
    {
        PushErrorMsg("ReloadParam failed");
        RET_ERR;
    }
#endif

//�����������ɼ���6·�ⲿAD�ɼ�ģ��
    if( ! CDataAcquire::Get().StartAcquireMcuAD (g_ad_com) )
    {
        PushErrorMsg("StartAcquireMcuAD  Failed");
        RET_ERR;
    }

    if( ! CDataAcquire::Get().StartAcquireAngleEncoder (g_encoder_com) )
    {
        PushErrorMsg("StartAcquireAngleEncoder  Failed");
        RET_ERR;
    }
#if 0
    if(gMainMenuIndex == 1) // when multi type,call StartAcquireAngleEncoder
    {
        if( ! CDataAcquire::Get().StartAcquireAngleEncoder (g_encoder_com) )
        {
            PushErrorMsg("StartAcquireAngleEncoder  Failed");
            RET_ERR;
        }
    }
#endif

//������̨ģ��
    if( ! CDianTai::Get().Start(g_diantai_com))
    {
        PushErrorMsg("DianTai Start Failed");
        RET_ERR;
    }
 //�̵�������ģ��
    if(m_dbadmin == NULL)
    {
        m_dbadmin = new CDBAdmin();
        if(!m_dbadmin->start())
        {
            PushErrorMsg("DB service module start failed");
            RET_ERR;
        }
    }else{

    }
//��ʼ��gpioģ��

    if( ! CJDQAdmin::Get().Start())
    {
        PushErrorMsg("CJDQAdmin Start Failed");
        RET_ERR;
    }

#if 1
    CJDQAdmin::Get ().ResetDevice (1);
//����gprs�ϴ�ģ��
    if( !gprs::get ().start (gprs_remote_ip,gprs_remote_port,gprs_dtu_id))
    {
        PushErrorMsg("gprs Start Failed");
        RET_ERR;
    }
    gprs::get ().set_control_func ((void*)dev_ctrl_func,(void*)this);
#endif

    blockRtSignal();

    m_thread.start(*this);


    return m_rdyEvt.tryWait(1000);

}
bool CMainCtrl::RegisterModuleCtrl(CTX_ModuleCtrl* obj)
{
    m_ModuleList.push_back(obj);
    return true;
}
