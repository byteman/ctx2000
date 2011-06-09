#include "mainctrl.h"
#include <Poco/Format.h>
#include <Poco/SingletonHolder.h>
#include <Poco/NumberParser.h>
#include <Poco/Timestamp.h>
#include <Poco/Timespan.h>
#include <Poco/StringTokenizer.h>
#include <Poco/String.h>
#include <yatengine.h>
#include <yateclass.h>
#include <assert.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <math.h>
#include "iniFile.h"
#include "comdata.h"
#include "lijuctrl.h"

using namespace TelEngine;
using Poco::SingletonHolder;
using Poco::StringTokenizer;
using Poco::TimerCallback;
//QtzParam  g_qtzs[NUMBER_OF_QTZ];
#define JDQ_OPEN  IO_LOW_LEVEL
#define JDQ_CLOSE IO_HIGH_LEVEL
Poco::Timespan g_SubWaitSubTime(3,0);
TIniFile cfg;
class CDataHandler{
public:


};

class MainCtrlHandler : public MessageHandler
{
public:
    MainCtrlHandler(CMainCtrl* ctrl) : MessageHandler("mainctrl.cmd") { m_ctrl = ctrl;}
    virtual bool received(Message &msg)
    {
        String type(msg.getValue("type"));

        type.trimBlanks();
        type.trimSpaces();
        std::cerr << "mainctrl received msg type:" << type.c_str() <<"\n";
        if(type == "save_worksite")
        {
            m_ctrl->SaveSiteInfo();
            return true;
        }else if(type == "read_worksite")
        {
            m_ctrl->ReadSiteInfo();
            return true;
        }else if(type == "save_tc"){
            m_ctrl->SaveTowerCraneInfo();
            return true;
        }else if(type == "save_wb"){
             m_ctrl->SaveBuildingInfo();
             return true;
        }
        return false;
    }
private:
    CMainCtrl* m_ctrl;
};
CMainCtrl::CMainCtrl()
{
    m_mode  = mode_slave;
    m_timer = NULL;
    for(int i = 0; i < 12; i ++)
        m_control_state.b[i] = false;
}
CMainCtrl::~CMainCtrl()
{

}

void CMainCtrl::init_tj_data()
{
    TCTotalNum  = 20;
    for(int i =0 ;i <3; i++)
    {
        m_jdq[i].set_flag = false;
        m_jdq[i].set_timer=0;
        m_jdq[i].cur_timer=0;
    }

    ReadSetting();
}
int       CMainCtrl::ValideTCNum()
{
    int result = 0;
    for(int i = 1; i <= TCTotalNum;i++)
        if(g_TC[i].Valide)
            result +=1;

    return result;
}
 int       CMainCtrl::MaxNo()
 {
    int result = 0;
    int valid_num = ValideTCNum();
    //fprintf(stderr,"valid=%d\n",valid_num);
    if(valid_num == 1){
        result =  m_local_id;
        //fprintf(stderr,"m_local_id=%d\n",m_local_id);
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
    DBG("RightNo=%d\n",RightNo);

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
mode :填写本机的实际模式
*/
std::string      CMainCtrl::build_qurey_msg()
{
    std::string Next;
    std::string result;
    std::string tmpStr;
    DBG("localid=%d,mode=%d\n",m_local_id,m_mode);
    if(m_mode==mode_master){
        Next = GetNextID();

        int nextid= Poco::NumberParser::parse(Next);
        result = "%"+Poco::format("%2d",m_local_id)+"N"+Poco::format("%2d",nextid)+"N";
    }else {
        result = "%"+Poco::format("%2d",m_local_id)+"N 0N";
    }
    //角度
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].Angle);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+=tmpStr;
    //位置
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].Position);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+="N"+tmpStr;
    //角速度
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].AngleSpeed);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+="N"+tmpStr;
    //仰角
    tmpStr = Poco::format("%6.2f",g_TC[m_local_id].Dang);
    if(tmpStr.length() > 6)
        tmpStr=tmpStr.substr(0,6);
    result+="N"+tmpStr;
    if(m_mode==mode_master){

        int nextId=Poco::NumberParser::parse(Next);
        int maxid = MaxNo() ;
        //fprintf(stderr,"maxid=%d,nextid=%d\n",maxid,nextId);
        if(maxid == nextId){//已经呼叫了最后一个有效塔机，申请加入一个新的塔机
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
            g_TC[i].Valide = false;
        }else{
            g_TC[i].Valide = true;
            cnt++;
        }
    }
    //fprintf(stderr,"valid=%d\n",cnt);
    //DBG("%s main_id=%d local_id=%d\n",__FUNCTION__,m_main_id,m_local_id);
    g_TC[m_main_id].Valide  = true; //主机始终有效
    g_TC[m_local_id].Valide = true; //本机始终有效
}
void      CMainCtrl::DripMainNoAndAddNo(std::string &MainNo, std::string &RightNo, std::string &AddNo)
{
    CTX_Message msg;
    int id = 0;
    if(CDianTai::Get().GetMessage(msg))
    {
          int rightNo,senderId;
          double angle,position,AngleSpeed,Dang;
          fprintf(stderr,"DripMainNoAndAddNo %s\n",msg.context.c_str());
          Poco::StringTokenizer token(msg.context,"N");
#if 1
            for(size_t i = 0 ; i < token.count(); i++)
            {
                fprintf(stderr,"token[%d]=%s\n",i,token[i].c_str());
            }
#endif
          if(token.count() != 7)
          {
               fprintf(stderr,"DripMainNoAndAddNo Error MsgCount =%d\n",token.count());
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
              g_TC[senderId].DLong      = g_TC[id].Position;
              g_TC[senderId].DHeight    = g_TC[id].Height+g_TC[id].L*sin(g_TC[id].Dang);

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
    CDianTai::Get().ClearMessage(); //在申请加入之前要把电台缓存的数据全部丢掉
    //listen 5s to recv rtmsg;

    while( (CurTime-StartTime) < 5000000)
    {

        if(CDianTai::Get().GetMessage(msg))
        {
                fprintf(stderr,"%s Recv RtMsg\n",__FUNCTION__);
                FoundM = true;
                break;
        }

        Poco::Thread::sleep(50);

        CurTime.update();
    }
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    if(FoundM){
        //等待100ms，再接收一个消息
        Poco::Thread::sleep(100);
        if(CDianTai::Get().CheckMessage(msg))
        {
                DripMainNoAndAddNo(MainDevID,RightNo,AddNo);
        }
        while( (RightNo=="0") || (AddNo != CurID) || (MainDevID == CurID) ){
            Poco::Thread::sleep(10);
            //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
            if(CDianTai::Get().CheckMessage(msg))
            {
                DripMainNoAndAddNo(MainDevID,RightNo,AddNo);
                fprintf(stderr,"%s DripMainNoAndAddNo Ok MainId=%s RightId=%s AddNo=%s CurId\n",__FUNCTION__,MainDevID.c_str(),RightNo.c_str(),AddNo.c_str());
                if( (RightNo!="0") && (AddNo == CurID) && (MainDevID != CurID))
                {
                    //fprintf(stderr,"========================\n");
                }
            }
        }
        fprintf(stderr,"********************%s ok MainId=%s RightId=%s AddNo=%s***************************\n",__FUNCTION__,MainDevID.c_str(),RightNo.c_str(),AddNo.c_str());
        m_mode    = mode_slave;
        m_main_id = Poco::NumberParser::parse(MainDevID);

        fprintf(stderr,"DripMainNoAndAddNo ok MainDevID=%d local_id=%d\n ",m_main_id,m_local_id);
        g_TC[m_local_id].Valide = true;
        g_TC[m_main_id].Valide  = true;
        AddState = true;


        Poco::Thread::sleep(20);

        StartTime.update();
        CurTime=StartTime;
        DBG("[Ready Add TC] Wait Slave Ack\n");
        //再接受一个有效数据然后发送本机信息 [就是接收被查询的从机所发来的信息，必须要接收到这个消息后，才能发送，否则造成同时发送的冲突]
        //因为新加入塔机号是跟随在塔机查询信息中的，所以被查询塔机也会回应信息，而且是立即回应，而新加入塔机会等待20ms后，并且收到了该回应
        //后才会回应自己的信息
        while (!CDianTai::Get().GetMessage(msg)) {
            CurTime.update();
            if( (CurTime-StartTime) > 1000000){ //1000ms 等待1000ms以便接收查询塔机的回应信息
                AddState=false;
            }
            Poco::Thread::sleep(20);
            fprintf(stderr,"New Add TC wait [Slave RightNo Ack]\n");
        }
        if(!AddState)
        {
            fprintf(stderr,"[Ready Add TC] Wait Slave Ack Failed\n");
            return;
        }
        //这里为什么要回应数据,因为要发送塔机加入命令
        std::string sendInfo = build_qurey_msg();
        fprintf(stderr,"[New Add TC] Send %s\n",sendInfo.c_str());
        CDianTai::Get().SendMessage(sendInfo);
        lastDateTime.update();
        sendInfoTime=lastDateTime;

    }else{
        for(int i = 1; i <= TCTotalNum; i++)
        {
            g_TC[i].Valide = false;
        }
        g_TC[m_local_id].Valide = true;
        m_main_id = m_local_id;
        MainDevID = CurID;
        AddState  = true;
        m_mode    = mode_master;
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
#if 1
    for(size_t i = 0 ; i < token.count(); i++)
    {
        fprintf(stderr,"token[%d]=%s\n",i,token[i].c_str());
    }
#endif
    if(token.count() != 7)
    {
         fprintf(stderr,"DistillData Error MsgCount =%d\n",token.count());
         return;
    }
    ID = "0"; //先给一个默认值，即使转换失败了或则数据有误差，也有一个默认值
    if(!Poco::NumberParser::tryParse(Poco::trim(token[0]),senderId))return;
    if(!Poco::NumberParser::tryParse(Poco::trim(token[1]),rightNo))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[2]),angle))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[3]),position))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[4]),AngleSpeed))return;
    if(!Poco::NumberParser::tryParseFloat(Poco::trim(token[5]),Dang))return;

    if( (senderId <= TCTotalNum)&& (senderId > 0) )
    {
        ID=Poco::trim(token[1]); //被呼叫的id
        g_TC[senderId].InfoTime.update(); //更新塔机信息更新时间

        g_TC[senderId].Angle      = angle;
        g_TC[senderId].Position   = position;
        g_TC[senderId].AngleSpeed = AngleSpeed;
        if(g_TC[senderId].Dyna)
            g_TC[senderId].Dang       = Dang;
        else
            g_TC[senderId].Dang       = 0;
        g_TC[senderId].DLong      = g_TC[id].Position;
        g_TC[senderId].DHeight    = g_TC[id].Height+g_TC[id].L*sin(g_TC[id].Dang);
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
    Poco::Timespan sub_wait_span(6,0);
    bool AddState=false;
    fprintf(stderr,"slave_loop\n");
    Poco::Thread::sleep(20);
    while (CDianTai::Get().GetMessage(msg)) {
        fprintf(stderr,"slave_loop1\n");
        lastDateTime.update();
        DistillData(msg.context,ID);
        fprintf(stderr,"slave_loop2\n");
        span = lastDateTime-sendInfoTime; //计算多长时间没有收到主机发来的请求包了，超时后重新申请加入网络
        //fprintf(stderr,"span=%d\n",span.seconds());

        if(span > sub_wait_span){
            fprintf(stderr,"slave receive timeout\n");
#if 1
            AddState=false;
            while(AddState==false)
                  WatchNetWork(MainMachineID,AddState);
#endif
        }
        fprintf(stderr,"slave_loop3\n");
        if(ID==CurID){ //收到了主机发给本机的查询消息
            std::string sendInfo = build_qurey_msg();
            CDianTai::Get().SendMessage(sendInfo);
            sendInfoTime.update();
            fprintf(stderr,"recv my id\n");
        }
        fprintf(stderr,"slave_loop4\n");
    }
}
void CMainCtrl::master_loop()
{
    //fprintf(stderr,"master_loop\n");
    CTX_Message msg;
    std::string ID;
    int  msg_count = 0;
    bool RecieveValidData=false;
    Poco::Timespan span,wait_span;
    Poco::Timestamp start,curTime;
    int valid_num = ValideTCNum();
    if(valid_num == 1 )
    {
        wait_span.assign(0,100000); //100ms
    }
    else
    {
        wait_span.assign(0,500000); //500ms
    }
    DBG("Valid TC Num = %d\n",valid_num);
    span = curTime - start;
    while(span < wait_span)
    {
        curTime.update();
        span = curTime - start;
        //fprintf(stderr,"Master Wait Recv RtMsg\n");
        while (CDianTai::Get().GetMessage(msg)) {
                msg_count++;
                //fprintf(stderr,"Master recv RtMsg %d,context=%s\n",msg.wType,msg.context.c_str());
                DistillData(msg.context,ID);
                RecieveValidData=true;
        }
        if(RecieveValidData)
        {
            //fprintf(stderr,"Master Recved %d Data\n",msg_count);
            break;
        }else{
            //fprintf(stderr,"Master Can't Recved Data\n");
        }
        Poco::Thread::sleep(10);
    }
    if(!RecieveValidData)
    {
        DBG("Master can not receive slave ack \n");
    }

    MaxTCNo = MaxNo(); //获取有效塔机编号中最后一个塔机
    DBG("Master MaxTCNo=%d RightNo=%d\n",MaxTCNo,RightNo);
    //判断能否接收到塔机加入的回应信息
    if(MaxTCNo == RightNo){//如果呼叫完了最后一个塔机 ，并且发送过申请加入一个新的塔机的指令，就等待接收新加入塔机的回应
        fprintf(stderr,"Master wait New Add TC %d slave's ack \n",RightNo);
        //等待200ms，等新加入塔机的响应消息
        //Poco::Thread::sleep(200);

        int repeat = 0;
        while(!CDianTai::Get().GetMessage(msg)){
            repeat++;
            Poco::Thread::sleep(20);
            if(repeat >= 10) break;

        }
        if(repeat < 10){
            DistillData(msg.context,ID);
            fprintf(stderr,"Master received New Add TC slave's ack id=%s \n",ID.c_str());

            UpdateTCStatus();
        }
    }

    std::string sendInfo = build_qurey_msg();

    fprintf(stderr,"Master SendData %s len=%d\n",sendInfo.c_str(),sendInfo.length());
    CDianTai::Get().SendMessage(sendInfo);

}
#include <Poco/FileStream.h>
void CMainCtrl::SaveTowerCraneInfo()
{
    DBG("%s %d\n",__FUNCTION__,__LINE__);

    ofstream tcfile("tc.red",std::ios::binary);


    //FILE* fp = fopen("tc.red","wb+");
    for(int i = 1; i <= TCTotalNum; i++){
        //fwrite((const void*)&g_TC[i],sizeof(TTjRecord),1,fp);
        tcfile.write((const char*)&g_TC[i],sizeof(TTjRecord));
    }
    //fclose(fp);
    tcfile.close();
    /*
    fp = fopen("tcorder.red","wb+");
    for(int i = 1; i <= TcNum; i++){
        fwrite((const void*)&tcorder[i],1,1,fp);
    }
    fclose(fp);*/
    tcfile.open("tcorder.red",std::ios::binary);
    for(int i = 1; i <= TcNum; i++){
        //fwrite((const void*)&tcorder[i],1,1,fp);
        tcfile.write((const char*)&tcorder[i],1);
    }
    DBG("%s %d\n",__FUNCTION__,__LINE__);

}
void CMainCtrl::ReadSiteInfo()
{
    //TIniFile cfg("ctx2000.ini");
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
    FDTime   = cfg.ReadFloat("device","FDTime",0);
    BrakeTime= cfg.ReadFloat("device","BrakeTime",0);

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
    g_TC[2].LongArmLength=100;
    g_TC[2].ShortArmLenght=20;

    strcpy(g_TC[3].Serial,"C8004");
    g_TC[3].x=300;
    g_TC[3].y=1;
    g_TC[3].LongArmLength=105;
    g_TC[3].ShortArmLenght=20;

    SaveTowerCraneInfo();


}
void CMainCtrl::ReadSetting()
{
    cfg.Open("ctx2000.ini");
    TCTypeName   = Poco::trim(cfg.ReadString("device","TC_type","def_typename"));
    CurID        = Poco::trim(cfg.ReadString("device","CurID","0"));
    CurSerial    = Poco::trim(cfg.ReadString("device","Serial","def_serial"));
    StrTCArmLen  = Poco::trim(cfg.ReadString("device","armlen","70"));
    StrTCBeilv   = Poco::trim(cfg.ReadString("device","beilv","1"));
    encoder_addr = cfg.ReadInteger("device","encoder_addr",0);
    g_diantai_com= Poco::trim(cfg.ReadString("serial","diantai_com","/dev/ttymxc0"));
    g_ad_com1    = Poco::trim(cfg.ReadString("serial","ad_com1","/dev/ttymxc1"));
    g_ad_com2    = Poco::trim(cfg.ReadString("serial","ad_com2","/dev/ttymxc2"));
    g_gprs_com   = Poco::trim(cfg.ReadString("serial","gprs_com","/dev/ttymxc3"));

//根据设定的当前塔机类型和倍率还有长度加载载荷曲线表
    if( !CLijuCtrl::Get().Load(TCTypeName,StrTCArmLen,StrTCBeilv))
    {
        std::cerr << "load Liju Data Failed\n";
    }

    std::cerr << "type: " << TCTypeName <<  std::endl;
    std::cerr << "ArmLen: " << StrTCArmLen <<  std::endl;
    std::cerr << "BeiLv: " << StrTCBeilv <<  std::endl;
    std::cerr << "CurID: " << CurID <<  std::endl;
    std::cerr << "CurSerial: " << CurSerial <<  std::endl;

    ifstream tcfile;
//加载塔机配置
    tcfile.open("tc.red",std::ios::binary|std::ios::in);

    if(tcfile.good())
    {
        std::cerr << "open tc.red" << std::endl;
        TcNum = 0;
        while(!tcfile.eof()){
            if(TcNum>(TCTotalNum-1))
                break;
            TcNum++;
            tcfile.read((char*)&g_TC[TcNum],sizeof(TTjRecord));
            fprintf(stderr,"TC[%d].Serial=%s height=%6.2f\n",TcNum,g_TC[TcNum].Serial,\
                    g_TC[TcNum].Height);
            g_TC[TcNum].Angle      = 0;
            g_TC[TcNum].AngleSpeed = 0;
            g_TC[TcNum].Dang       = 0;
            g_TC[TcNum].Position   = 0;
            if(g_TC[TcNum].Serial == CurSerial){
                CurID = Poco::format("%d",TcNum);
                DBG("Find CurID:%s\n",CurID.c_str());
            }
            g_TC[TcNum].InfoTime.update();
        }
        tcfile.close();
    }else{
        CreateDefaultTjParam();
        std::cerr << "Can not open tc.red" << std::endl;
    }
    //g_TC[1].Height = 120;

    std::string tmp = CurID;
    m_local_id = Poco::NumberParser::parse(tmp);
    g_local_id = m_local_id;

    ReadSiteInfo();
    memset(&wba[0],0,11*sizeof(Twb));
    tcfile.open("bd.red",std::ios::binary);
    if(tcfile.good())
    {
        DBG("bd.red open ok\n");
        wbNum = 0;
        while(!tcfile.eof()){

            tcfile.read((char*)&wba[wbNum++],sizeof(Twb));
            DBG("wba[%d] VertexNum=%d height=%6.2f\n",wbNum,wba[wbNum].VertexNum,wba[wbNum].h);

        }
        tcfile.close();
    }else{
        DBG("bd.red open failed\n");
    }
    fprintf(stderr,"wbanum=%d\n",wbNum);
    for (int i = 1; i <= TCTotalNum ; i++)
    {
         g_TC[i].DHeight=0;
         g_TC[i].Valide=false;
    }
    fd_time = cfg.ReadInteger("device","FD_time",5);
    m_jdq[0].set_timer = fd_time;
    m_jdq[1].set_timer = fd_time;
    brake_time= cfg.ReadInteger("device","brake_time",5);
    m_jdq[2].set_timer = brake_time;
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

    //获取有效塔机信息
    int ret = GetValidTjList(g_valid_tj_list);
    fprintf(stderr,"valid dev count = %d\n",ret);
    for(int i = 0 ; i < ret; i++)
    {
        fprintf(stderr,"valid id[%d] = %d\n",i,g_valid_tj_list.at(i));
    }
    //在塔机参数已经读取成功后，再获取与本机id冲突的设备id列表
    ret = GetConflictTjList(g_conflict_tj_list);
    fprintf(stderr,"conflict dev count = %d\n",ret);
    for(int i = 0 ; i < ret; i++)
    {
        fprintf(stderr,"conflict localid[%d] : id[%d] = %d\n",g_local_id,i,g_conflict_tj_list.at(i));
    }
    //初始化标定系数
    InitBDParam();
    //计算ABC角度
    calc_angle(czwzb,g_TC[g_local_id].x,g_TC[g_local_id].y,g_angle_A,g_angle_B,g_angle_C);
    fprintf(stderr,"angleA=%0.2f,angleB=%0.2f,angleC=%0.2f\n",g_angle_A,g_angle_B,g_angle_C);
    InitAlgoData();
}
void CMainCtrl::InitBDParam()
{
    TIniFile cfg("ctx2000.ini");
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


    g_bd[BD_CAR_DIST].bd_k =cfg.ReadFloat("dist_bd","dist_k",1);
    g_bd[BD_CAR_DIST].start_value =cfg.ReadFloat("dist_bd","min_dist",0);
    g_bd[BD_CAR_DIST].zero_ad =cfg.ReadInteger("dist_bd","zero_ad",0);
    g_bd[BD_CAR_DIST].bd_ad   = cfg.ReadInteger("dist_bd","bd_ad",0);

    g_bd[BD_HEIGHT].bd_k = cfg.ReadFloat("height_bd","height_k",1);
    g_bd[BD_HEIGHT].start_value = cfg.ReadFloat("height_bd","min_height",0);
    g_bd[BD_HEIGHT].zero_ad = cfg.ReadInteger("height_bd","zero_ad",0);
    g_bd[BD_HEIGHT].bd_ad   = cfg.ReadInteger("height_bd","bd_ad",0);

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

//三个实时参数
        g_qtzs[i].m_sarm_angle       = 0; //动臂仰角,实时参数
        g_qtzs[i].m_carrier_pos      = 0;
        g_qtzs[i].m_long_arm_angle   = 0;

        fprintf(stderr,"g_qtzs[%d] car_pos=%0.2f height=%0.2f longarm=%0.2f shortarm=%0.2f,x=%0.2f y=%0.2f brake_dist=%0.2f stop_dist=%0.2f slowdown_dist=%0.2f v_slow_dist=%0.2f v_brake_dist=%0.2f\n",\
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
                fprintf(stderr,"local %d Detect %d \n ",g_local_id,id);
                g_qtzs[local_tower_id].QtzAddDetect(&g_qtzs[id-1]);

            }
        }
    }
    /*
    初始化区域检测
*/
    g_polys.clear();
    for(size_t i=0; i < wbNum; i++)
    {

        if( ( wba[i].VertexNum > 0 ) && ( wba[i].VertexNum < 6 ) )
        {
            fprintf(stderr,"i=%d num=%d h=%0.2f\n",i,wba[i].VertexNum,wba[i].h);
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

                fprintf(stderr,"i=%d x=%0.2f y=%0.2f\n",i,x,y);
            }
            poly.push_back(poly[0]);
            g_polys.push_back(poly);
        }
    }
    if(local_tower_id >= 0)
    {
        for(size_t i = 0; i < g_polys.size(); i++)
        {
            fprintf(stderr,"add diving \n");
            g_qtzs[local_tower_id].QtzAddDetect( &g_polys[i]);
            for(size_t j = 0 ; j < g_polys[i].size(); j++)
                fprintf(stderr,"x=%0.2f y=%0.2f\n",g_polys[i].at(j).x,g_polys[i].at(j).y);
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

    cfg.WriteInteger("device","FDTime",FDTime);
    cfg.WriteInteger("device","BrakeTime",BrakeTime);
}
void CMainCtrl::SaveDivideInfo()
{
    ofstream divfile("DivideLine.tc");

    if(divfile.good()){
        for (int i = 0; i < DividePointNum ; i++)
        {
            divfile<<DividePoint[i].x<<" "<<DividePoint[i].y<<std::endl;
        }
    }

    /*
    FILE* fp = fopen("DivideLine.tc","wb+");
    for(int i = 0 ;i < DividePointNum;i++)
    {
        fwrite((const void*)&DividePoint[i],sizeof(TDividePoint),1,fp);
    }
    fclose(fp);
    */
}
void CMainCtrl::SaveBuildingInfo()
{
    FILE* fp = fopen("bd.red","wb");
    fseek(fp,0,SEEK_SET);
    for(int i = 0 ;i < wbNum-1;i++)
    {
        fwrite((const void*)&wba[i],sizeof(Twb),1,fp);
    }
    fclose(fp);
    fprintf(stderr,"wbnum=%d\n",wbNum);
}
void CMainCtrl::SaveWorksiteInfomation()
{
    SaveTowerCraneInfo();
    SaveSiteInfo();
    SaveDivideInfo();
    SaveBuildingInfo();
}
/*
处理高度信息上传
*/
bool CMainCtrl::DealHeightInfo()
{
    CTX_Message msg;
    int HTCNo;
    double TCHeight;
    //DBG("%s %d\n",__FUNCTION__,__LINE__);
    if(CDianTai::Get().GetMessage(msg,2)){
        DBG("Recv HeighInfo =%s\n",msg.context.c_str());
        Poco::StringTokenizer token(msg.context,"N");
        if(token.count() != 2)
        {
            return false;
        }
        HTCNo = Poco::NumberParser::parse(token[0]);
        TCHeight = Poco::NumberParser::parseFloat(token[1]);

        DBG("Modify TC[%d] Height=%6.2f\n",HTCNo,TCHeight);
        g_TC[HTCNo].Height = TCHeight;
        SaveTowerCraneInfo();
        //Read
        Poco::Thread::sleep(m_local_id*100);
        std::string sendInfo="!"+CurID;
        CDianTai::Get().SendMessage(sendInfo);
        Poco::Thread::sleep(m_local_id*1000);

        //restart app
    }
    return true;
    //DBG("%s %d\n",__FUNCTION__,__LINE__);
}
void CMainCtrl::DiveideSettingInfomation(std::string &msg)
{
    int curpos=0;
    std::string tmp;
    DBG("%s %d\n",__FUNCTION__,__LINE__);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 2; j++)
        {
            wksp[i][j] = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
            curpos+=8;
        }
    }
    DBG("%s %d\n",__FUNCTION__,__LINE__);
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++)
        {
            czwzb[i][j] = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
            curpos+=8;
        }
    }
    DBG("%s %d\n",__FUNCTION__,__LINE__);
    YNAngle = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
    curpos+=8;
    VStopDis = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
    curpos+=8;
    VWarnDis = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
    curpos+=8;
    BrakeDis = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
    curpos+=8;
    DangerDis = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
    curpos+=8;
    WarnDis = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
    curpos+=8;
    AddAngle = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
    curpos+=8;

    tmp = Poco::trim(msg.substr(curpos,2));
    DBG("%s %d tcnum=%s\n",__FUNCTION__,__LINE__,tmp.c_str());
    TcNum = Poco::NumberParser::parse(tmp);
    curpos+=2;
//塔机信息
    for(int i = 1 ; i <= TcNum; i++ ){
           DBG("%s %d\n",__FUNCTION__,__LINE__);
        msg.substr(curpos,8).copy(g_TC[i].Serial,8);
        curpos+=8;
        g_TC[i].x = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
           DBG("%s %d\n",__FUNCTION__,__LINE__);
        curpos+=8;
        g_TC[i].y = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
           DBG("%s %d\n",__FUNCTION__,__LINE__);
        curpos+=8;
        tmp = Poco::trim(msg.substr(curpos,8));
        std::cerr << "heigh=" << tmp <<std::endl;
        g_TC[i].Height = Poco::NumberParser::parseFloat(tmp);
           DBG("%s %d\n",__FUNCTION__,__LINE__);
        curpos+=8;
        g_TC[i].LongArmLength = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
           DBG("%s %d\n",__FUNCTION__,__LINE__);
        curpos+=8;
        g_TC[i].ShortArmLenght = Poco::NumberParser::parseFloat(msg.substr(curpos,8));
           DBG("%s %d\n",__FUNCTION__,__LINE__);
        curpos+=8;
        if(Poco::NumberParser::parse(Poco::trim(msg.substr(curpos,1))) == 1){
               DBG("%s %d\n",__FUNCTION__,__LINE__);
           g_TC[i].Dyna = true;
        }else
           g_TC[i].Dyna = false;
        curpos+=1;
           DBG("%s %d\n",__FUNCTION__,__LINE__);
        g_TC[i].Rs = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        g_TC[i].L1 = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        g_TC[i].L2 = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        g_TC[i].a0 = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        g_TC[i].CAng = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        g_TC[i].L = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
    }
    DBG("%s %d\n",__FUNCTION__,__LINE__);
 //保护区域信息
    wbNum = Poco::NumberParser::parse(Poco::trim(msg.substr(curpos,2)));
    curpos+=2;
DBG("%s %d wbNum=%d\n",__FUNCTION__,__LINE__,wbNum);
    for(int i = 0 ; i < wbNum; i++ ){
        wba[i].VertexNum = Poco::NumberParser::parse(Poco::trim(msg.substr(curpos,2)));
        curpos+=2;
DBG("%s %d VertexNum=%d\n",__FUNCTION__,__LINE__,wba[i].VertexNum);
        wba[i].h = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
        curpos+=8;

        for(int j = 0; j < wba[i].VertexNum; j++){
            wba[i].Pointxy[j][0] = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
            curpos+=8;
DBG("%s %d Pointxy=%6.2f\n",__FUNCTION__,__LINE__,wba[i].Pointxy[j][0] );
            wba[i].Pointxy[j][1] = Poco::NumberParser::parseFloat(Poco::trim(msg.substr(curpos,8)));
            curpos+=8;
            DBG("%s %d Pointxy=%6.2f\n",__FUNCTION__,__LINE__,wba[i].Pointxy[j][1] );
        }
    }
DBG("%s %d\n",__FUNCTION__,__LINE__);
    DividePointNum = Poco::NumberParser::parse(Poco::trim(msg.substr(curpos,2)));
    curpos+=2;
DBG("%s %d DividePointNum=%d\n",__FUNCTION__,__LINE__,DividePointNum);
    for(int i = 0 ; i < DividePointNum; i++){
        tmp = Poco::trim(msg.substr(curpos,8));
        DBG("tmp1=%s\n",tmp.c_str());
        DividePoint[i].x = Poco::NumberParser::parseFloat(tmp);
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
        tmp = Poco::trim(msg.substr(curpos,8));
        DBG("tmp2=%s\n",tmp.c_str());
        DividePoint[i].y = Poco::NumberParser::parseFloat(tmp);
        curpos+=8;
        DBG("%s %d\n",__FUNCTION__,__LINE__);
    }
    DBG("%s %d\n",__FUNCTION__,__LINE__);

}
/*
处理工地信息上传
*/
bool CMainCtrl::DealWorkSiteInfo()
{
    CTX_Message msg;
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    if(CDianTai::Get().GetMessage(msg,3)){
        DBG("%s %d %s\n",__FUNCTION__,__LINE__,msg.context.c_str());
        DiveideSettingInfomation(msg.context);
        SaveWorksiteInfomation();
        Poco::Thread::sleep(m_local_id*100);
        std::string sendInfo = "!"+CurID;
        CDianTai::Get().SendMessage(sendInfo);
        Poco::Thread::sleep(m_local_id*1000);
        return true;
        //restart app
    }
    return false;
}
/*
在这个地方填充算法模块的实时参数
*/
void CMainCtrl::Gather_AD()
{
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    int index= g_local_id-1;
    int id   = 0;
//用本机的实时参数，更新本机和算法的实时参数
    //fprintf(stderr,"g_local_id=%d angle=%0.2f\n",g_local_id,g_TC[g_local_id].Angle);
    g_TC[g_local_id].Angle    = g_angle*3.14/180;
    g_TC[g_local_id].Position = g_car_dist;
    g_TC[g_local_id].Dang     = g_up_angle;
    if( ( index >= 0) && (index < NUMBER_OF_QTZ) )
    {
        g_qtzs[index].m_long_arm_angle   = g_angle;
        g_qtzs[index].m_carrier_pos      = g_car_dist;
        g_qtzs[index].m_sarm_angle       = g_up_angle;
    }else{
        return;
    }
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
//根据电台传来的周围塔机的实时参数，更新算法模块需要的实时参数
    for(size_t i = 0; i < g_conflict_tj_list.size(); i++)
    {
        id = g_conflict_tj_list.at(i)-1;
        if( (id >= 0) &&  (id < NUMBER_OF_QTZ))
        {
            g_qtzs[id].m_long_arm_angle   = g_TC[id+1].Angle*180.0f/3.14f;//
            g_qtzs[id].m_carrier_pos      = g_TC[id+1].Position;
            g_qtzs[id].m_sarm_angle       = g_TC[id+1].Dang;
        }
        //fprintf(stderr,"tj conflict[%d] angle=%0.2f h=%0.2f p=%0.2f\n",id+1,g_qtzs[id].m_long_arm_angle,g_qtzs[id].m_height,g_qtzs[id].m_carrier_pos);
    }
    //fprintf(stderr,"tj local [%d] angle=%0.2f h=%0.2f p=%0.2f\n",index+1,g_qtzs[index].m_long_arm_angle,g_qtzs[index].m_height,g_qtzs[index].m_carrier_pos);
    QtzCollideDetectOne(&g_qtzs[index]);
    m_control_state = g_qtzs[index].m_controled_status;

    //fprintf(stderr,"state=%d%d%d%d%d%d%d%d%d%d%d%d\n",m_control_state.b1,m_control_state.b2,m_control_state.b3,\
    //        m_control_state.b4,m_control_state.b5,m_control_state.b6,m_control_state.b7,m_control_state.b8,\
    //        m_control_state.b9,m_control_state.b10,m_control_state.b11,m_control_state.b12);

    if(m_control_state.b1 != m_old_ctrl_state.b1){
         fprintf(stderr,"output0\n");
        if(m_control_state.b1)
        {
            m_gpio.Output(0,JDQ_OPEN);
            m_gpio.Output(11,JDQ_OPEN); //右回转反顶继电器
            m_jdq[11].cur_timer=fd_time;
            m_jdq[11].set_flag = true;
        }
        else
        {
            m_gpio.Output(0,JDQ_CLOSE);
        }
        m_old_ctrl_state.b1 = m_control_state.b1;
    }
    if(m_control_state.b2 != m_old_ctrl_state.b2){
         fprintf(stderr,"output1\n");
        if(m_control_state.b2)
        {
            m_gpio.Output(1,JDQ_OPEN);
            m_jdq[1].cur_timer= brake_time;
            m_jdq[1].set_flag = true;

        }
        else m_gpio.Output(1,JDQ_CLOSE);
    }
    if(m_control_state.b3 != m_old_ctrl_state.b3){
         fprintf(stderr,"output2\n");
        if(m_control_state.b3)
        {
            m_gpio.Output(2,JDQ_OPEN);
            m_gpio.Output(10,JDQ_OPEN);
            m_jdq[10].cur_timer=fd_time;
            m_jdq[10].set_flag = true;
        }
        else m_gpio.Output(2,JDQ_CLOSE);
    }
    if(m_control_state.b4 != m_old_ctrl_state.b4){
         fprintf(stderr,"output3\n");
        if(m_control_state.b4)m_gpio.Output(3,JDQ_OPEN);
        else m_gpio.Output(3,JDQ_CLOSE);
    }
    if(m_control_state.b5 != m_old_ctrl_state.b5){
         fprintf(stderr,"output4\n");
        if(m_control_state.b5)m_gpio.Output(4,JDQ_OPEN);
        else m_gpio.Output(4,JDQ_CLOSE);


        m_old_ctrl_state.b5=m_control_state.b5;
    }
    if(m_control_state.b6 != m_old_ctrl_state.b6){
         fprintf(stderr,"output6\n");
        if(m_control_state.b6)m_gpio.Output(5,JDQ_OPEN);
        else m_gpio.Output(5,JDQ_CLOSE);
    }
    if(m_control_state.b7 != m_old_ctrl_state.b7){
         fprintf(stderr,"output7\n");
        if(m_control_state.b7)m_gpio.Output(6,JDQ_OPEN);
        else m_gpio.Output(6,JDQ_CLOSE);
    }
    if(m_control_state.b9 != m_old_ctrl_state.b9){
         fprintf(stderr,"output9\n");
        if(m_control_state.b9)m_gpio.Output(8,JDQ_OPEN);
        else m_gpio.Output(8,JDQ_CLOSE);
    }
    if(m_control_state.b10 != m_old_ctrl_state.b10){
        fprintf(stderr,"output10\n");
        if(m_control_state.b10)
            m_gpio.Output(9,JDQ_OPEN);
        else m_gpio.Output(9,JDQ_CLOSE);
    }
    /*
    if(m_control_state.b11 != m_old_ctrl_state.b11){
        if(m_control_state.b11)m_gpio.Output(10,JDQ_OPEN);
        else m_gpio.Output(10,JDQ_CLOSE);
    }
    if(m_control_state.b12 != m_old_ctrl_state.b12){
        if(m_control_state.b12)m_gpio.Output(11,JDQ_OPEN);
        else m_gpio.Output(11,JDQ_CLOSE);
    }*/

    m_old_ctrl_state=m_control_state ;


}
#include "beeper.h"
/*
力矩服务程序
*/
void    CMainCtrl::LjService()
{
    int out_state = CLijuCtrl::Get().Service(g_car_dist,g_dg_weight);
    static bool over_flag = false;
    //fprintf(stderr,"out_status=%d\n",out_state);
    //根据力矩输出的状态来控制继电器的动作
    if(out_state == 4) //力矩超过105%
    {
        m_gpio.Output(4,JDQ_OPEN);//限制 小车向外运行停车
        m_gpio.Output(7,JDQ_OPEN);//限制吊钩向上
        over_flag = true;
        CBeeper::get().BeepMs(1000,100000);
    }else if(out_state == 1 && over_flag ){ //力矩小于了95%
        m_gpio.Output(4,JDQ_CLOSE);//恢复小车向外
        m_gpio.Output(7,JDQ_CLOSE);//恢复吊钩向上
        over_flag = false;
    }else if(out_state >= 2 && out_state<4){
        static int delay=0;
        delay++;
        if( (delay%2) == 0)
            CBeeper::get().BeepMs(100,100000);
    }
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

    fprintf(stderr,"watch ok mode=%d mainID=%s\n",m_mode,MainMachineID.c_str());
    while(!m_quit)
    {
        Gather_AD();
        DealWorkSiteInfo();
        DealHeightInfo();
        UpdateTCStatus();
        LjService();

        if(m_mode == mode_master)
        {
            master_loop();
        }else if (m_mode == mode_slave) {
            slave_loop();
        }else{
            std::cerr << "unkown mode\n";
        }
        //Poco::Thread::sleep(10);

    }
}
//处理接收到的AD,调用算法模块，得出冲突值，通知界面更新，通知gpio进行控制子
void CMainCtrl::handle_ad(ADNotification* pNf)
{
    poco_check_ptr (pNf);
    AutoPtr<ADNotification> nf = pNf;
    TAdQueue *que = nf->Message();

    while(que->size() > 0)
    {
        TAD ad = que->front();
        que->pop();
        fprintf(stderr,"GetAd type = %d value = %d\n",ad.m_type,ad.m_value);
        m_angle = 0;
        m_dist  = 0;
    }
}
//处理接收到的电台数据，调用算法模块，得出冲突值，通知界面更新，通知gpio进行控制子，已经更新参数
void CMainCtrl::handle_diantai(Poco::Notification* pNf)
{

}
//处理gprs数据，上传数据
void CMainCtrl::handle_gprs(Poco::Notification* pNf)
{

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
void CMainCtrl::ad_handle(ADNotification* pNf)
{
    poco_check_ptr (pNf);
    AutoPtr<ADNotification> nf = pNf;
    TAdQueue *que = nf->Message();

    while(que->size() > 0)
    {
        TAD ad = que->front();
        que->pop();
        fprintf(stderr,"GetAd type = %d value = %d\n",ad.m_type,ad.m_value);
        if(ad.m_type == 7)
        {
            ad_angle = ad.m_value;
            g_angle = (ad_angle-g_bd[BD_ANGLE].zero_ad)*g_bd[BD_ANGLE].bd_k;
        }else  if(ad.m_type == 1)
        {
            ad_up_angle = ad.m_value;
            g_up_angle  = (ad_up_angle-g_bd[BD_UP_ANGLE].zero_ad)*g_bd[BD_UP_ANGLE].bd_k;
        }else if(ad.m_type == 2)
        {
            ad_weight = ad.m_value;
        }else if(ad.m_type == 3)
        {
            ad_car_dist = ad.m_value;
            g_car_dist = (ad_car_dist-g_bd[BD_CAR_DIST].zero_ad)*g_bd[BD_CAR_DIST].bd_k;
        }else if(ad.m_type == 4)
        {
            ad_height = ad.m_value;
            g_dg_height = (ad_height-g_bd[BD_HEIGHT].zero_ad)*g_bd[BD_HEIGHT].bd_k;
        }else if(ad.m_type == 5)
        {
            ad_fengsu = ad.m_value;
            g_speed =  (ad_fengsu-g_bd[BD_SPEED].zero_ad)*g_bd[BD_SPEED].bd_k;
        }
    }

}
void* dispatch_thread(void* arg)
{
    while(1)
    {
        Poco::Thread::sleep(1000);
        fprintf(stderr,"dispath_thread run\n");
    }
    return NULL;
}
void create_dispath_thread()
{
    pthread_t tid;
    pthread_create(&tid,NULL,dispatch_thread,NULL);
    Poco::Thread::sleep(100);
    pthread_detach(tid);
}
bool CMainCtrl::Start()
{
//这是仅仅打开了数据库
    if( !CTajiDbMgr::Get().load("ctx2000.sqlite3"))
    {
        fprintf(stderr,"TajiDbMgr load failed\n");
        return false;
    }
    if(m_timer==NULL)
    {
        m_timer = new Timer(1000,1000);
        TimerCallback<CMainCtrl> tc(*this, &CMainCtrl::onTimer);
        m_timer->start(tc);
    }
    init_tj_data();

#if 1
//启动编码器采集和6路外部AD采集模块
    if( ! CDataAcquire::Get().Start(g_ad_com1,g_ad_com2))
    {
        fprintf(stderr,"DataAcquire Start Failed\n");
        return false;
    }
#endif
//启动电台模块
    if( ! CDianTai::Get().Start(g_diantai_com))
    {
        fprintf(stderr,"DianTai Start Failed\n");
        return false;
    }
    blockRtSignal();
//初始化gpio模块
    m_gpio.CreateVirtualFiles();
    m_gpio.Init();
    m_thread.start(*this);
    return m_rdyEvt.tryWait(1000);

}
void CMainCtrl::onTimer(Timer& timer)
{
    for(int i =10 ;i < 12; i++)
    {
        if(m_jdq[i].set_flag)
        {
            if(m_jdq[i].cur_timer<=0)
            {
                m_gpio.Output(i,JDQ_CLOSE);
                m_jdq[i].set_flag=false;
            }else{
                m_jdq[i].cur_timer--;
            }
        }
    }
    for(int i =1 ;i < 2; i++)
    {
        if(m_jdq[i].set_flag)
        {
            if(m_jdq[i].cur_timer<=0)
            {
                m_gpio.Output(i,JDQ_CLOSE);
                m_jdq[i].set_flag=false;
            }else{
                m_jdq[i].cur_timer--;
            }
        }
    }
    //std::cout << "Callback called after  milliseconds." << std::endl;
}
