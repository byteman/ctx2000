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
#include "centerrecver.h"
#include "iniFile.h"
#include "comdata.h"
#include "lijuctrl.h"
using namespace TelEngine;
using Poco::SingletonHolder;
using Poco::StringTokenizer;
QtzParam  g_qtzs[NUMBER_OF_QTZ];

Poco::Timespan g_SubWaitSubTime(15,0);
TIniFile cfg;
class CDataHandler{
public:


};
//
class CenterDataHandler : public MessageHandler
{
public:
    CenterDataHandler() : MessageHandler("center.cmd") { }
    //parse and  update system param
    void parseUploadParam(Message &msg)
    {

        unsigned char id = msg.getIntValue("id",0);
        if(id == 0)//没有id为0的塔机编号
        {
            msg.setParam("ret","fail");
            msg.setParam("error","id==0");
            return;
        }

        unsigned char index = CTajiDbMgr::Get().GetDevIndex(id);
        assert( index!=-1 );
        std::string var_name = msg.getValue("name","no");
        fprintf(stderr,"parseUploadParam name=%s id: %d\n",var_name.c_str(),id);
        if( (var_name == "taji") && (index < NUMBER_OF_QTZ))
        {

            TTjParam &t = CTajiDbMgr::Get().m_tj[index];


            t.a0        = msg.getDoubleValue("jiegou",0);
            t.devserail = msg.getValue("serial","noset");
            t.height    = msg.getDoubleValue("height",0);
            t.isown     = msg.getIntValue("isown",0);
            t.long_arm_length  = msg.getDoubleValue("long_arm_len",0);
            t.short_arm_length = msg.getDoubleValue("short_arm_len",0);
            t.max_up_angle     = msg.getDoubleValue("max_up_angle",0);
            t.min_up_angle     = msg.getDoubleValue("min_up_angle",0);
            t.min_car_dist     = msg.getDoubleValue("min_car_dist",0);
            t.taji_id          = id;
            t.pos.x            = msg.getDoubleValue("x_pos",0);
            t.pos.y            = msg.getDoubleValue("y_pos",0);
            t.taji_type        = msg.getIntValue("type",0);

            CTajiDbMgr::Get().UpdateTjParam(id,&t);

            msg.setParam("ret","ok");
        }else if(var_name == "worksite"){
            TWorkSiteParam ws;// = CTajiDbMgr::Get().m_tj[id].ws;

            ws.alert_dist = msg.getDoubleValue("alert_dist",0);

            ws.anchor_coord.at(0).x = msg.getDoubleValue("ax1",0);
            ws.anchor_coord.at(0).y = msg.getDoubleValue("ay1",0);
            ws.anchor_coord.at(1).x = msg.getDoubleValue("ax2",0);
            ws.anchor_coord.at(1).y = msg.getDoubleValue("ay2",0);

            ws.brake_dist = msg.getDoubleValue("brake_dist",0);
            ws.car_high_spd_dist = msg.getDoubleValue("car_high_spd_dist",0);
            ws.car_low_spd_dist  = msg.getDoubleValue("car_low_spd_dist",0);
            ws.inertia_angle     = msg.getDoubleValue("inertia_angle",0);
            ws.slowdown_dist     = msg.getDoubleValue("slowdown_dist",0);
            ws.y_angle           = msg.getDoubleValue("y_angle",0);

            for(int i = 0; i < 4; i++)
            {
                std::string x_field = Poco::format("wx%d",i);
                std::string y_field = Poco::format("wy%d",i);
                ws.worksite_coord.at(i).x = msg.getDoubleValue(x_field.c_str(),0);
                ws.worksite_coord.at(i).y = msg.getDoubleValue(y_field.c_str(),0);
            }
            CTajiDbMgr::Get().UpdateWorksite(&ws);
            //std::vector<PolyDef> &zone = CTajiDbMgr::Get().m_tj[id].zone;
            msg.setParam("ret","ok");
        }


    }
    void parseRtParam(Message &msg)
    {
        unsigned char id = msg.getIntValue("id",0);
        if(id == 0)//没有id为0的塔机编号
        {
            msg.setParam("ret","fail");
            msg.setParam("error","id==0");
            return;
        }
        double angle = msg.getDoubleValue("angle",0);
        double dist  = msg.getDoubleValue("dist",0);
        unsigned char index = CTajiDbMgr::Get().GetDevIndex(id);
        assert( index != -1);
        if(index < NUMBER_OF_QTZ)
        {
            fprintf(stderr,"update %d tj \n");
            //CTajiDbMgr::Get().m_tj[id].rt.carrier_pos    = dist-CTajiDbMgr::Get().m_tj[id].calc.;
            //CTajiDbMgr::Get().m_tj[id].rt.long_arm_angle = angle*3.1415926/180;;
            g_qtzs[index].carrier_pos    = dist;
            g_qtzs[index].long_arm_angle = angle*3.1415926/180;
            //calcRtParam(id,dist,angle);
        }
    }
    void calcRtParam(int id,int dist_ad, int angle_ad)
    {
        //dist_zero = CTajiDbMgr::Get().m_tj[id].calc.anchor_zero_ad;
        //CTajiDbMgr::Get().m_tj[id].calc
    }
    virtual bool received(Message &msg)
    {
        String type(msg.getValue("cmd"));

        type.trimBlanks();
        type.trimSpaces();
        std::cerr << "mainctrl received msg type:" << type.c_str() <<"\n";
        if(type == "upload")
        {
            std::cerr << "type upload\n";
            parseUploadParam(msg);
            return true;

        }else if(type == "runtime"){
            parseRtParam(msg);
            return true;
        }
        return false;
    }
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
    m_mode = mode_slave;

}
CMainCtrl::~CMainCtrl()
{

}
void CMainCtrl::init_tj_data()
{

    m_local_id  = 2;//CTajiDbMgr::Get().GetLocalId();
    CurID       = "02";
    TCTotalNum  = 20;
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
    if(ValideTCNum() == 1){
        result =  m_local_id;
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
    //DBG("%s %d\n",__FUNCTION__,__LINE__);
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

        if(MaxNo() == nextId){
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

    for(int i = 1; i <= TCTotalNum;i++){
        span = curTime-g_TC[i].InfoTime;
        if(span > g_SubWaitSubTime)
        {
            g_TC[i].Valide = false;
        }else{
            g_TC[i].Valide = true;
        }
    }
    DBG("%s main_id=%d local_id=%d\n",__FUNCTION__,m_main_id,m_local_id);
    g_TC[m_main_id].Valide  = true;
    g_TC[m_local_id].Valide = true;
}
void      CMainCtrl::DripMainNoAndAddNo(std::string &MainNo, std::string &RightNo, std::string &AddNo)
{
    CTX_Message msg;
    int id = 0;
    if(CDianTai::Get().GetMessage(msg))
    {
          Poco::StringTokenizer token(msg.context,"N");
          MainNo = token[0];
          RightNo= token[1];
          if(Poco::NumberParser::tryParse(MainNo,id))
          {
              g_TC[id].Angle = Poco::NumberParser::parseFloat(token[2]);
              g_TC[id].Position = Poco::NumberParser::parseFloat(token[3]);
              g_TC[id].AngleSpeed = Poco::NumberParser::parseFloat(token[4]);
              g_TC[id].Dang = Poco::NumberParser::parseFloat(token[5]);
              g_TC[id].DLong   = g_TC[id].Position;
              g_TC[id].DHeight = g_TC[id].Height+g_TC[id].L*sin(g_TC[id].Dang);
              AddNo =token[6];
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
    CDianTai::Get().ClearMessage();
    //listen 5s to recv rtmsg;
    while( (CurTime-StartTime) < 5000000)
    {
        DBG("%s Wait RtMsg\n",__FUNCTION__);
        if(CDianTai::Get().GetMessage(msg))
        {
                DBG("%s Recv RtMsg\n",__FUNCTION__);
                FoundM = true;
                break;
        }
        Poco::Thread::sleep(50);
        CurTime.update();

    }
    if(FoundM){
        //等待100ms，再接收一个消息
        Poco::Thread::sleep(100);
        if(CDianTai::Get().CheckMessage(msg))
        {
                DripMainNoAndAddNo(MainDevID,RightNo,AddNo);
        }
        while( (RightNo=="0") || (AddNo != CurID) || (MainDevID == CurID) ){
            Poco::Thread::sleep(10);
            if(CDianTai::Get().CheckMessage(msg))
            {
                DripMainNoAndAddNo(MainDevID,RightNo,AddNo);
                DBG("%s DripMainNoAndAddNo Ok MainId=%s RightId=%s AddNo=%s\n",__FUNCTION__,MainDevID.c_str(),RightNo.c_str(),AddNo.c_str());
                AddState = true;
            }
        }
        m_mode = mode_slave;
        m_main_id = Poco::NumberParser::parse(MainDevID);

        DBG("DripMainNoAndAddNo ok MainDevID=%d local_id=%d\n ",m_main_id,m_local_id);
        g_TC[m_local_id].Valide = true;
        g_TC[m_main_id].Valide  = true;
        AddState=true;
        //再接受一个有效数据然后发送本机信息 [就是接收被查询的从机所发来的信息，必须要接收到这个消息后，才能发送，否则造成同时发送的冲突]
        //因为新加入塔机号是跟随在塔机查询信息中的，所以被查询塔机也会回应信息，而且是立即回应，而新加入塔机会等待20ms后，并且收到了该回应
        //后才会回应自己的信息

        Poco::Thread::sleep(20);

        StartTime.update();
        CurTime=StartTime;
        DBG("[Ready Add TC] Wait Slave Ack\n");
        while (!CDianTai::Get().GetMessage(msg)) {
            CurTime.update();
            if( (CurTime-StartTime) > 500000){ //500ms 等待500ms以便接收查询塔机的回应信息
                AddState=false;
            }
        }
        if(!AddState)
        {
            DBG("[Ready Add TC] Wait Slave Ack Failed\n");
        }
        std::string sendInfo = build_qurey_msg();
        DBG("[New Add TC] Send %s\n",sendInfo.c_str());
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
    Poco::StringTokenizer token(msg,"N");
    for(size_t i = 0 ; i < token.count(); i++)
    {
        fprintf(stderr,"token[%d]=%s\n",i,token[i].c_str());
    }
    std::string MainNo = token[0];
    RightNo= Poco::NumberParser::parse(token[1]);
    if(Poco::NumberParser::tryParse(MainNo,id))
    {
        ID=MainNo;
        g_TC[id].InfoTime.update();
        g_TC[id].Angle      = Poco::NumberParser::parseFloat(token[2]);
        g_TC[id].Position   = Poco::NumberParser::parseFloat(token[3]);
        g_TC[id].AngleSpeed = Poco::NumberParser::parseFloat(token[4]);
        g_TC[id].Dang       = Poco::NumberParser::parseFloat(token[5]);
        g_TC[id].DLong      = g_TC[id].Position;
        g_TC[id].DHeight    = g_TC[id].Height+g_TC[id].L*sin(g_TC[id].Dang);
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
    Poco::Timespan sub_wait_span(60);
    bool AddState=false;
    DBG("slave_loop\n");
    Poco::Thread::sleep(20);
    while (CDianTai::Get().GetMessage(msg)) {
        lastDateTime.update();
        DistillData(msg.context,ID);
        span = lastDateTime-sendInfoTime;
        if(span > sub_wait_span){
            AddState=false;
            while(AddState==false)
                  WatchNetWork(MainMachineID,AddState);
        }
        if(ID==CurID){ //收到了主机发给本机的查询消息
            std::string sendInfo = build_qurey_msg();
            CDianTai::Get().SendMessage(sendInfo);
            sendInfoTime.update();
        }
    }
}
void CMainCtrl::master_loop()
{
    DBG("master_loop\n");
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
        //DBG("Master Wait Recv RtMsg\n");
        while (CDianTai::Get().GetMessage(msg)) {
                msg_count++;
                fprintf(stderr,"Master recv RtMsg context=%s\n",msg.wType,msg.context.c_str());
                DistillData(msg.context,ID);
                RecieveValidData=true;
        }
        if(RecieveValidData)
        {
            DBG("Master Recved %d Data\n",msg_count);
            break;
        }
        Poco::Thread::sleep(10);
    }
    if(!RecieveValidData)
    {
        DBG("Master can not receive slave ack \n");
    }

    MaxTCNo = MaxNo();
    DBG("Master MaxTCNo=%d RightNo=%d\n",MaxTCNo,RightNo);
    //判断能否接收到塔机加入的回应信息
    if(MaxTCNo == RightNo){
        DBG("Master wait New Add TC slave's ack \n");
        //等待200ms，等新加入塔机的响应消息
        Poco::Thread::sleep(200);

        if(CDianTai::Get().GetMessage(msg)){

            DistillData(msg.context,ID);
            DBG("Master received New Add TC slave's ack id=%s \n",ID.c_str());
            UpdateTCStatus();
        }
    }

    std::string sendInfo =build_qurey_msg();

    //DBG("Master SendData %s len=%d\n",sendInfo.c_str(),sendInfo.length());
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

    YNAngle = cfg.ReadFloat("device","YNAngle",0);
    BrakeDis= cfg.ReadFloat("device","BrakeDis",0);
    DangerDis= cfg.ReadFloat("device","DangerDis",0);
    WarnDis= cfg.ReadFloat("device","WarnDis",0);
    VWarnDis= cfg.ReadFloat("device","VWarnDis",0);
    VStopDis= cfg.ReadFloat("device","VStopDis",0);
    AddAngle= cfg.ReadFloat("device","AddAngle",0);
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

void CMainCtrl::ReadSetting()
{
    cfg.Open("ctx2000.ini");
    TCTypeName = Poco::trim(cfg.ReadString("device","TC_type",""));
    CurID      = Poco::trim(cfg.ReadString("device","CurID","0"));
    CurSerial  = Poco::trim(cfg.ReadString("device","Serial",""));
    StrTCArmLen = Poco::trim(cfg.ReadString("device","armlen",""));
    StrTCBeilv  = Poco::trim(cfg.ReadString("device","beilv",""));

    if( !CLijuCtrl::Get().Load(TCTypeName,StrTCArmLen,StrTCBeilv))
    {
        std::cerr << "load Liju Data Failed\n";
    }
    //int b = CLijuCtrl::Get().ChangeBeilv();
    //std::cerr <<  "new beilv:" <<  b <<std::endl;

    //CLijuCtrl::Get().Service(32.2,23);
    //fprintf(stderr,"percent=%0.3f\n",CLijuCtrl::Get().m_percent);
    std::cerr << "type:" << TCTypeName <<  std::endl;
    std::cerr << "CurID:" << CurID <<  std::endl;
    std::cerr << "CurSerial:" << CurSerial <<  std::endl;

    ifstream tcfile;

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
            if(g_TC[TcNum].Serial == CurSerial){
                CurID = Poco::format("%d",TcNum);
            }
        }
        tcfile.close();
    }else{
        std::cerr << "Can not open tc.red" << std::endl;
    }
    DBG("CurID:%s\n",CurID.c_str());

    std::string tmp = CurID;
    m_local_id = Poco::NumberParser::parse(tmp);
    g_local_id = m_local_id;
    Trolly_dist_max = cfg.ReadFloat("dist_bd","V_dist_max",2.0);   //小车最近距离
    Trolly_dist_min = cfg.ReadFloat("dist_bd","V_dist_max",60.0);  //小车最远距离
    V0 = cfg.ReadFloat("dist_bd","V_dist_max",5);

    ReadSiteInfo();
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
    DBG("wbanum=%d\n",wbNum);
    for (int i = 1; i <= TCTotalNum ; i++)
    {
         g_TC[i].DHeight=0;
         g_TC[i].Valide=false;
    }
    fd_time = cfg.ReadInteger("device","FD_time",5);
    brake_time= cfg.ReadInteger("device","brake_time",5);
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
    fprintf(stderr,"come1\n");
    //在塔机参数已经读取成功后，再获取与本机id冲突的设备id列表
    ret = GetConflictTjList(g_conflict_tj_list);
    fprintf(stderr,"conflict dev count = %d\n",ret);
    for(int i = 0 ; i < ret; i++)
    {
        fprintf(stderr,"conflict localid[%d] : id[%d] = %d\n",g_local_id,i,g_conflict_tj_list.at(i));
    }
    //计算ABC角度
    calc_angle(czwzb,g_TC[g_local_id].x,g_TC[g_local_id].y,g_angle_A,g_angle_B,g_angle_C);
    InitAlgoData();
}
void CMainCtrl::InitAlgoData()
{
    for(int i = 0; i < NUMBER_OF_QTZ; i++)
    {
        g_qtzs[i].number     =  i+1;
        g_qtzs[i].carrier_pos = 0;
        g_qtzs[i].height      = g_TC[i+1].Height;
        g_qtzs[i].long_arm_len     = g_TC[i+1].LongArmLength;
        g_qtzs[i].short_arm_len    = g_TC[i+1].ShortArmLenght;
        g_qtzs[i].long_arm_angle   = 0;
        g_qtzs[i].pos.x      = g_TC[i+1].x;
        g_qtzs[i].pos.y      = g_TC[i+1].y;
        fprintf(stderr,"g_qtzs[%d] car_pos=%0.2f height=%0.2f longarm=%0.2f shortarm=%0.2f,x=%0.2f y=%0.2f\n",\
                i,g_qtzs[i].carrier_pos,g_qtzs[i].height,g_qtzs[i].long_arm_len,g_qtzs[i].short_arm_len,\
                g_qtzs[i].pos.x,g_qtzs[i].pos.y);
    }

}
void CMainCtrl::SaveSiteInfo()
{
    /*
    FILE* fp = fopen("ws.red","wb+");
    for(int i = 0; i <4; i++){
        fwrite((const void*)&wksp[i][0],sizeof(double),fp);
        fwrite((const void*)&wksp[i][1],sizeof(double),fp);
    }

    for(int i = 0; i <2; i++){
        fwrite((const void*)&czwzb[i][0],sizeof(double),fp);
        fwrite((const void*)&czwzb[i][1],sizeof(double),fp);
    }

    fwrite((const void*)&YNAngle,sizeof(double),fp);
    fwrite((const void*)&BrakeDis,sizeof(double),fp);
    fwrite((const void*)&DangerDis,sizeof(double),fp);
    fwrite((const void*)&WarnDis,sizeof(double),fp);
    fwrite((const void*)&VStopDis,sizeof(double),fp);
    fwrite((const void*)&VWarnDis,sizeof(double),fp);
    fwrite((const void*)&Addangle,sizeof(double),fp);

    fclose(fp);
*/

    //TIniFile cfg("ctx2000.ini");
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
    DBG("***********BrakeDis=%6.2f\n",BrakeDis);
    cfg.WriteFloat("device","DangerDis",DangerDis);
    cfg.WriteFloat("device","WarnDis",WarnDis);
    cfg.WriteFloat("device","VStopDis",VStopDis);
    cfg.WriteFloat("device","VWarnDis",VWarnDis);
    cfg.WriteFloat("device","AddAngle",AddAngle);

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
    FILE* fp = fopen("bd.red","wb+");
    for(int i = 0 ;i < wbNum;i++)
    {
        fwrite((const void*)&wba[i],sizeof(Twb),1,fp);
    }
    fclose(fp);
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
    DBG("%s %d\n",__FUNCTION__,__LINE__);
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
void Gather_AD()
{
    static int i = 0;
    g_TC[1].Angle = 23.4+i;
    g_TC[1].AngleSpeed= 32;
    g_TC[1].Position = 24;
    i+=5;

}
void CMainCtrl::run()
{
    bool AddState=false;

    m_rdyEvt.set();
    m_quitEvt.reset();
    m_quit = false;
    init_tj_data();

    while (false == AddState) {
        WatchNetWork(MainMachineID,AddState);
    }
    DBG("watch ok mode=%d mainID=%s\n",m_mode,MainMachineID.c_str());
    while(!m_quit)
    {
        Gather_AD();
        DealWorkSiteInfo();
        DealHeightInfo();

        UpdateTCStatus();
        //DBG("%s %d\n",__FUNCTION__,__LINE__);
        if(m_mode == mode_master)
        {
            master_loop();
        }else if (m_mode == mode_slave) {
            slave_loop();
        }else{
            std::cerr << "unkown mode\n";
        }


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
            g_up_angle = (ad_up_angle-g_bd[BD_UP_ANGLE].zero_ad)*g_bd[BD_UP_ANGLE].bd_k;
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
bool CMainCtrl::Start()
{

    if( !CTajiDbMgr::Get().load("ctx2000.sqlite3",g_qtzs, NUMBER_OF_QTZ))
    {
        fprintf(stderr,"TajiDbMgr load failed\n");
        return false;
    }
    fprintf(stderr,"tj num = %d\n",CTajiDbMgr::Get().get_tj_num());

    if( ! CDataAcquire::Get().Start("/dev/ttyUSB0"))
    {
        fprintf(stderr,"DataAcquire Start Failed\n");
        return false;
    }
    if( ! CDianTai::Get().Start("/dev/ttyUSB1"))
    {
        fprintf(stderr,"DianTai Start Failed\n");
        //return false;
    }
    blockRtSignal();
    //CDataAcquire::Get().addObserver(Notifyer(CMainCtrl,ADNotification,handle_ad));
    TelEngine::Engine::install(new MainCtrlHandler(this));

    CDataAcquire::Get().addObserver(Notifyer(CMainCtrl,ADNotification,ad_handle));
    m_thread.start(*this);

    return m_rdyEvt.tryWait(1000);

}
