#include "formcalib.h"
#include "formcalibitem.h"
#include "BmpRes.h"
#include "comdata.h"
#include "iniFile.h"
#include "lijuConfForm.h"
#include <math.h>
#include <Poco/NumberParser.h>
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
            MessageBox(m_hWnd,"Please input correct height\n","Error",MB_OK);
            return;
        }
        start = true;
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_height;
        double bd_height=0;
        if(!Poco::NumberParser::tryParseFloat(_edits[1]->GetText(),bd_height))
        {
            MessageBox(m_hWnd,"Please input correct height\n","Error",MB_OK);
            return;
        }
        double k = (end_ad - start_ad) / (bd_height-zero_height);
        if(k < 0.001)
        {
            MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Error",MB_OK);
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
      }else{
          MessageBox(m_hWnd,"Please Press start first\n","Error",MB_OK);
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
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_car_dist;
        double min_dist = g_TC[g_local_id].Rs;
        double max_dist = g_TC[g_local_id].LongArmLength;
        double k = (end_ad - start_ad) / (max_dist-min_dist);

        if(k < 2)
        {
            MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Dist Error",MB_OK);
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
    }else{
        MessageBox(m_hWnd,"Please Press start first\n","Error",MB_OK);
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
void CFormCalib::calibrate_weight(int type)
{
    static int start_ad = 0;
    static int end_ad   = 0;
    static bool start=false;
    if(type == 0){
        start_ad = ad_weight;
        start = true;
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_weight;
        double bd_weight=0;
        if(!Poco::NumberParser::tryParseFloat(_edits[0]->GetText(),bd_weight))
        {
            MessageBox(m_hWnd,"Please input correct weight\n","Error",MB_OK);
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
        if(k < 0.001)
        {
            MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Error",MB_OK);
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
    }else if((type == 1) && (start)){
        start = false;
        end_ad = ad_angle;
        double k = (end_ad - start_ad) / g_angle_C;

        if(k < 2)
        {
            MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Error",MB_OK);
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
    }else if(type == 1 && start){
        start = false;
        end_ad = ad_up_angle;
        double min_up_angle = g_TC[g_local_id].L2;
        double max_up_angle = g_TC[g_local_id].L1;
        double k = (end_ad-start_ad)/(max_up_angle-min_up_angle);
        if(k < 0.001)
        {
            MessageBox(m_hWnd,"Calibration ERROR! Please Calibrate again.","Error",MB_OK);
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
        CLiJuManForm lj;
        lj.CreateForm(m_hWnd);
    }
    else if(item->id == btn_ret->GetId())
    {
        Close();
    }
}
