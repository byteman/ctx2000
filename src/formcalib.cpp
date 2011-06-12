#include "formcalib.h"
#include "formcalibitem.h"
#include "BmpRes.h"
#include "comdata.h"
#include "iniFile.h"
#include "lijuConfForm.h"
#include <math.h>
#include <Poco/NumberParser.h>
#include "MsgBox.h"
#include "Password.h"

static const char* mmenu_bmps[] = {
       PCLBACKGROUND,
       PANGLE_A ,
       PANGLE_B ,
       PMIN_UP_ANGLE,
       PMAX_UP_ANGLE,
       PMIN_DIST ,
       PMAX_DIST ,
       PCAL_ZERO,
       PCAL_KG,
       PMIN_HEIGHT ,
       PMAX_HEIGHT ,
       PCOMM_CLOSE_BTN,
       PLIJU,
       PCOMM_RET_BTN
};
static COMM_CTRL_DESC CommCtrls[] =
{
    EDIT_START_AD,
    EDIT_END_AD,
};
static COMM_CTRL_DESC StaticCtrls[] =
{

    STATIC_CUR_AD,
    STATIC_START_X,
    STATIC_START_Y,
    STATIC_START_AD,
    STATIC_END_AD,
};
static const char* Msg[]={
    "回转标定,起始点记录成功",
    "回转标定成功",
    "回转标定参数异常，请重新标定",

    "仰角标定,最小角度记录成功",
    "仰角标定成功",
    "仰角标定参数异常，请重新标定",

    "最小幅度记录成功",
    "幅度标定成功",
    "幅度标定参数异常，请重新标定",

    "重量清零成功",
    "重量标定完成",
    "重量标定参数异常，请重新标定",

    "高度清零成功",
    "高度标定完成",
    "高度标定参数异常，请重新标定",
};
/*
#define BUTTON_CAB_ANGLE_START     {1,400,100}
#define BUTTON_CAB_ANGLE_STOP      {2,500,100}
#define BUTTON_CAB_UPAGL_START     {1,400,150}
#define BUTTON_CAB_UPAGL_STOT      {2,500,150}
#define BUTTON_CAB_CAR_START       {1,400,200}
#define BUTTON_CAB_CAR_STOP        {2,500,200}
#define BUTTON_CAB_WET_START       {1,400,250}
#define BUTTON_CAB_WET_STOP        {2,500,250}
#define BUTTON_CAB_HIG_START       {1,400,300}
#define BUTTON_CAB_HIG_STOP        {2,500,300}
*/
static SKIN_CTRL_DESC SkinCtrls[] = {
    BUTTON_CAB_ANGLE_START,
    BUTTON_CAB_ANGLE_STOP,
    BUTTON_CAB_UPAGL_START,
    BUTTON_CAB_UPAGL_STOT,
    BUTTON_CAB_CAR_START,
    BUTTON_CAB_CAR_STOP,
    BUTTON_CAB_WET_START,
    BUTTON_CAB_WET_STOP,
    BUTTON_CAB_HIG_START,
    BUTTON_CAB_HIG_STOP,
    SKIN_BUTTON_EXIT,
    BUTTON_LIJU
};
static SKIN_BUTTON_DESC skinctrls[] = {
        {13,BUTTON_RETURN_X,BUTTON_RETURN_Y},

};
#define MSG_AD MSG_USER+0x100
class ADMessageHandler:public MessageHandler{
public:
    ADMessageHandler(HWND handle) :
        MessageHandler("ad.angle"),
        m_handle(handle)
    {

    }
    virtual bool received(Message &msg)
    {
        int angle = msg.getIntValue("value");
        PostMessage(m_handle,MSG_AD,(WPARAM)angle,0);
    }
private:
    HWND m_handle;
};
CFormCalib::CFormCalib()
{
    if(!LoadRes(&mmenu_bmps[0],ARRAY_SIZE(mmenu_bmps,char*)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    for(int i = 0 ;i < (sizeof(SkinCtrls)/sizeof(SKIN_CTRL_DESC));i++)
    {
        _btns[i] = new CSkinButton(&SkinCtrls[i],this);
    }
    for(int i = 0 ;i < (sizeof(CommCtrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        _edits[i] = new CEdit(&CommCtrls[i],this);
    }
    btn_ret    = new CSkinButton(&skinctrls[0],this);;
    /*
    for(int i = 0 ;i < (sizeof(StaticCtrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        _lables[i] = new CStatic(&StaticCtrls[i],this);
    }
    */
    InitSkinHeader("formcalib");
}

CFormCalib::~CFormCalib()
{

}

void CFormCalib::OnCreate()
{
    //msgHandler = new ADMessageHandler(m_hWnd);
    //TelEngine::Engine::install(msgHandler);
}
void CFormCalib::OnClose()
{
    //TelEngine::Engine::uninstall(msgHandler);
}
void CFormCalib::OnTimer(int ID)
{

}
void CFormCalib::OnUserMsg(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    if( (hWnd == m_hWnd) && (message == MSG_AD) )
    {
        int angle_ad = (int)wParam;
        _edits[0]->SetIntText(angle_ad);
    }
}
void CFormCalib::OnPaint(HWND hWnd)
{

}
void CFormCalib::OnShow()
{
    if(g_TC[g_local_id].Dyna){
        EnableSkinButton(_btns[2]->GetId(),false);
        EnableSkinButton(_btns[3]->GetId(),false);
        EnableSkinButton(_btns[4]->GetId(),true);
        EnableSkinButton(_btns[5]->GetId(),true);
    }else{
        EnableSkinButton(_btns[2]->GetId(),true);
        EnableSkinButton(_btns[3]->GetId(),true);
        EnableSkinButton(_btns[4]->GetId(),false);
        EnableSkinButton(_btns[5]->GetId(),false);
    }
}
//a === bd[0][0..1]
//b === bd[1][0..1]
void CFormCalib::calibrate_height(int type)
{
    static int start_ad = 0;
    static int end_ad   = 0;
    static bool start   = false;
    double zero_height = 0;
    if(type == 0){
        start_ad = ad_height;

        if(!Poco::NumberParser::tryParseFloat(_edits[1]->GetText(),zero_height))
        {
            MsgBox box;
            box.ShowBox(this,"请输入正确的最小高度\n","输入错误");
            ///box.ShowBox(this,"Please input correct height\n","Error");
            return;
        }

        start = true;
        MsgBox box;
        box.ShowBox(this,"最小高度记录成功","提示");
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_height;
        double bd_height=0;
        if(!Poco::NumberParser::tryParseFloat(_edits[1]->GetText(),bd_height))
        {
            MsgBox box;
            box.ShowBox(this,"请输入正确的最大高度\n","输入错误");
            //box.ShowBox(this,"Please input correct height\n","Error");
            return;
        }
        double k = (end_ad - start_ad) / (bd_height-zero_height);
        if( (fabs(k) < 0.001) || ( fabs(k) > 10000 ) )
        {
            MsgBox box;
            box.ShowBox(this,"高度标定参数错误，请重新标定","错误");
            return;
        }
        g_bd[BD_HEIGHT].bd_k        = k;
        g_bd[BD_HEIGHT].zero_ad     = start_ad;
        g_bd[BD_HEIGHT].bd_ad       = end_ad;
        g_bd[BD_HEIGHT].start_value = zero_height;

        TIniFile cfg("ctx2000.ini");
        cfg.WriteFloat("height_bd","min_height",g_bd[BD_HEIGHT].start_value);
        cfg.WriteFloat("height_bd","height_k",  g_bd[BD_HEIGHT].bd_k);
        cfg.WriteInteger("height_bd","zero_ad", g_bd[BD_HEIGHT].zero_ad);
        cfg.WriteInteger("height_bd","bd_ad",   g_bd[BD_HEIGHT].bd_ad);
        MsgBox box;
        box.ShowBox(this,"高度标定完成","提示");
      }else{
        MsgBox box;
        box.ShowBox(this,"请先将高度清零","错误");

      }
}
void CFormCalib::calibrate_car_dist(int type)
{
    static int start_ad = 0;
    static int end_ad   = 0;
    static bool start=false;
    if(type == 0){
        start_ad = ad_car_dist;
        start = true;
        MsgBox box;
        box.ShowBox(this,"最小幅度记录成功\n","提示");
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_car_dist;
        double min_dist = g_TC[g_local_id].Rs;
        double max_dist = g_TC[g_local_id].LongArmLength;
        double k = (end_ad - start_ad) / (max_dist-min_dist);

        if((fabs(k) < 0.001) || ( fabs(k) > 10000 ) )
        {
            MsgBox box;
            box.ShowBox(this,"幅度标定参数错误，请重新标定\n","参数错误");
            //MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Dist Error",MB_OK);
            return;
        }
        g_bd[BD_CAR_DIST].bd_k        = k;
        g_bd[BD_CAR_DIST].zero_ad     = start_ad;
        g_bd[BD_CAR_DIST].bd_ad       = end_ad;
        g_bd[BD_CAR_DIST].start_value = g_TC[g_local_id].Rs;

        TIniFile cfg("ctx2000.ini");
        cfg.WriteFloat("dist_bd","min_dist",  g_bd[BD_CAR_DIST].start_value);
        cfg.WriteFloat("dist_bd","dist_k",    g_bd[BD_CAR_DIST].bd_k);
        cfg.WriteInteger("dist_bd","zero_ad", g_bd[BD_CAR_DIST].zero_ad);
        cfg.WriteInteger("dist_bd","bd_ad",   g_bd[BD_CAR_DIST].bd_ad);
        MsgBox box;
        box.ShowBox(this,"幅度标定完成","提示");
    }else{
        MsgBox box;
        box.ShowBox(this,"请先将点击最小幅度按钮","错误");
    }
}
#include "SoftKeyboard.h"
extern SoftKeyboard *skt;
void CFormCalib::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{

    if(nc==EN_SETFOCUS)
    {
        fprintf(stderr,"setfocus\n");
        if(skt)
        {
            //fprintf(stderr,"t9show\n");
            skt->T9_Show(true);
        }
    }
}
#include <Poco/Format.h>
void CFormCalib::calibrate_weight(int type)
{
    static int start_ad = 0;
    static int end_ad   = 0;
    static bool start=false;
    if(type == 0){
        start_ad = ad_weight;
        start = true;
        MsgBox box;
        box.ShowBox(this,"重量清零成功\n","提示");
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_weight;
        double bd_weight=0;
        if(!Poco::NumberParser::tryParseFloat(_edits[0]->GetText(),bd_weight))
        {
            MsgBox box;
            box.ShowBox(this,"请输入正确的重量值\n","错误");
            //MessageBox(m_hWnd,"Please input correct weight\n","Error",MB_OK);
            return;
        }
        /*
        double bd_weight = _edits[0]->GetFloatValue(ok);
        if(!ok){
            MessageBox(m_hWnd,"Please input correct weight\n","Error",MB_OK);
            return;
        }
        */
        double k = (end_ad - start_ad) / bd_weight;

        if(( fabs(k) < 0.001) || ( fabs(k) > 10000 ) )
        {
            std::string value = Poco::format("%0.2f",k);
            MsgBox box;
            box.ShowBox(this,"重量标定参数错误，请重新标定 k=" + value +"\n","错误");
           // MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Error",MB_OK);
            return;
        }
        g_bd[BD_WEIGHT].bd_k        = k;
        g_bd[BD_WEIGHT].zero_ad     = start_ad;
        g_bd[BD_WEIGHT].bd_ad       = end_ad;
        g_bd[BD_WEIGHT].start_value = 0;

        TIniFile cfg("ctx2000.ini");
        cfg.WriteFloat("weight_bd","min_weight",g_bd[BD_WEIGHT].start_value);
        cfg.WriteFloat("weight_bd","weight_k",g_bd[BD_WEIGHT].bd_k);
        cfg.WriteInteger("weight_bd","zero_ad", g_bd[BD_WEIGHT].zero_ad);
        cfg.WriteInteger("weight_bd","bd_ad", g_bd[BD_WEIGHT].bd_ad);

        MsgBox box;
        box.ShowBox(this,"重量标定完成","提示");
    }else{
        MsgBox box;
        box.ShowBox(this,"请先将重量清零按钮","错误");
    }
}
void CFormCalib::calibrate_angle(int type)
{
    static int start_ad = 0;
    static int end_ad   = 0;
    static bool start=false;
    if(type == 0){
        start_ad = ad_angle;
        start = true;
        MsgBox box;
        box.ShowBox(this,"A点记录成功","提示");
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_angle;
        double k = (end_ad - start_ad) / g_angle_C;

        if( (fabs(k) < 0.001) || ( fabs(k) > 10000 ) )
        {
            MsgBox box;
            box.ShowBox(this,"回转角度标定参数错误，请重新标定.","错误");
            //box.ShowBox(this,"Calibration ERROR! Please Calibrate again.","Error");
            return;
        }
        g_bd[BD_ANGLE].bd_k        = k;
        g_bd[BD_ANGLE].zero_ad     = start_ad;
        g_bd[BD_ANGLE].bd_ad     = end_ad;
        g_bd[BD_ANGLE].start_value = g_angle_A;

        TIniFile cfg("ctx2000.ini");
        cfg.WriteFloat("angle_bd","a_angle",g_bd[BD_ANGLE].start_value);
        cfg.WriteFloat("angle_bd","angle_k",g_bd[BD_ANGLE].bd_k);
        cfg.WriteInteger("angle_bd","zero_ad", g_bd[BD_ANGLE].zero_ad);
        cfg.WriteInteger("angle_bd","bd_ad", g_bd[BD_ANGLE].bd_ad);
        MsgBox box;
        box.ShowBox(this,"回转标定完成.","提示");
    }else{
        MsgBox box;
        box.ShowBox(this,"请先点击标定A点.","错误");
    }
}
void CFormCalib::calc_up_angle(int type)
{
    static int start_ad = 0;
    static int end_ad   = 0;
    static bool start=false;
    if(type == 0){
        start_ad = ad_up_angle;
        start = true;
        MsgBox box;
        box.ShowBox(this,"最小仰角记录成功","提示");
    }else if(type == 1 && start){
        start = false;
        end_ad = ad_up_angle;
        double min_up_angle = g_TC[g_local_id].L2;
        double max_up_angle = g_TC[g_local_id].L1;
        double k = (end_ad-start_ad)/(max_up_angle-min_up_angle);
        if( (fabs(k) < 0.001) || ( fabs(k) > 10000 ) )
        {
            MsgBox box;
            box.ShowBox(this,"仰角标定参数错误，请重新标定","标定错误");
            //MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Error",MB_OK);
            return;
        }
        g_bd[BD_UP_ANGLE].bd_k        = k;
        g_bd[BD_UP_ANGLE].zero_ad     = start_ad;
        g_bd[BD_UP_ANGLE].start_value = min_up_angle;

        TIniFile cfg("ctx2000.ini");
        cfg.WriteFloat("up_angle_bd","min_up_angle",min_up_angle);
        cfg.WriteFloat("up_angle_bd","up_angle_k",k);
        cfg.WriteInteger("up_angle_bd","zero_ad", start_ad);
        cfg.WriteInteger("up_angle_bd","bd_ad", end_ad);
        MsgBox box;
        box.ShowBox(this,"仰角标定完成.","提示");
    }else{
        MsgBox box;
        box.ShowBox(this,"请先点击最小仰角按钮.","错误");
    }
}
void CFormCalib::OnButtonClick(skin_item_t* item)
{
    if(item->id       == _btns[0]->GetId()){
        //回转标定开始
        calibrate_angle(0);
    }else if(item->id == _btns[1]->GetId()){
        //回转标定结束
        calibrate_angle(1);
    }else if(item->id == _btns[2]->GetId()){
        //动臂仰角标定开始
        calc_up_angle(0);
    }else if(item->id == _btns[3]->GetId()){
        //动臂仰角标定结束
        calc_up_angle(1);
    }else if(item->id == _btns[4]->GetId()){
        //幅度标定开始
        calibrate_car_dist(0);
    }else if(item->id == _btns[5]->GetId()){
        //幅度标定接收
        calibrate_car_dist(1);
    }else if(item->id == _btns[6]->GetId()){
        //称重标定开始
        calibrate_weight(0);
    }else if(item->id == _btns[7]->GetId()){
        //称重标定结束
         calibrate_weight(1);
    }else if(item->id == _btns[8]->GetId()){
        //吊钩高度标定开始
        calibrate_height(0);
    }else if(item->id == _btns[9]->GetId()){
        //吊钩高度标定结束
        calibrate_height(1);
    }else if(item->id == _btns[10]->GetId()){
        //吊钩高度标定结束
        Close();
    }else if(item->id == _btns[11]->GetId()){

        //PassWord pwd;
        //if(pwd.ShowBox(this,"密码:","请输入密码","1020"))
        {
            CLiJuManForm lj;
            lj.CreateForm(m_hWnd);
        }


    }
    else if(item->id == btn_ret->GetId())
    {
        Close();
    }
}
