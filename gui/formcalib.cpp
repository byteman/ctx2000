#include "formcalib.h"
#include "formcalibitem.h"
#include "BmpRes.h"
#include "comdata.h"
#include "iniFile.h"
#include "torqueForm.h"
#include <math.h>
#include <Poco/NumberParser.h>
#include "MsgBox.h"
#include "Password.h"
#include "FastStatic.h"
#include <Poco/Format.h>
#define EDT_W 95
#define EDT_H 30
#define SX 115
#define EDT_ANGLE_AD {SX,70,EDT_W,EDT_H,"1"}
#define EDT_UP_ANGLE {SX,140,EDT_W,EDT_H,"1"}
#define EDT_POSTION  {SX,210,EDT_W,EDT_H,"1"}
#define EDT_WEIGHT   {SX,280,EDT_W,EDT_H,"1"}
#define EDT_HEIGHT   {SX,350,EDT_W,EDT_H,"1"}
#define S_X 25
#define LBL_ANGLE_AD {S_X,70,EDT_W,EDT_H,"回转AD: "}
#define LBL_UP_ANGLE {S_X,140,EDT_W,EDT_H,"仰角AD: "}
#define LBL_POSTION  {S_X,210,EDT_W,EDT_H,"幅度AD: "}
#define LBL_WEIGHT   {S_X,280,EDT_W,EDT_H,"重量AD: "}
#define LBL_HEIGHT   {S_X,350,EDT_W,EDT_H,"高度AD: "}

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
       PCOMM_RET_BTN,
       "ctx2000/btn_setzero.png"
};
static COMM_CTRL_DESC CommCtrls[] =
{
    EDIT_START_AD,
    EDIT_END_AD,
    EDT_ANGLE_AD,
    EDT_UP_ANGLE,
    EDT_POSTION,
    EDT_WEIGHT,
    EDT_HEIGHT,
};
static COMM_CTRL_DESC StaticCtrls[] =
{

    STATIC_CUR_AD,
    STATIC_START_X,
    STATIC_START_Y,
    STATIC_START_AD,
    STATIC_END_AD,
};
static COMM_CTRL_DESC LabelCtrls[] =
{
    LBL_ANGLE_AD,
    LBL_UP_ANGLE,
    LBL_POSTION,
    LBL_WEIGHT,
    LBL_HEIGHT
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
        {14,100,400}, //for qinxie angle zero

};
#define MSG_AD MSG_USER+0x100

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
#if 0
    for(int i = 0 ;i < TABLESIZE(CommCtrls)-5;i++)
    {
        _edits[i] = new CEdit(&CommCtrls[i],this);
    }
#endif

    for(int i = 2 ;i < TABLESIZE(CommCtrls);i++)
    {
        _edtAD[i-2] = new CFastStatic(&CommCtrls[i],this);
    }

    btn_ret    = new CSkinButton(&skinctrls[0],this);

    for(int i = 0 ;i < TABLESIZE(LabelCtrls);i++)
    {
        _edtLable[i] = new CFastStatic(&LabelCtrls[i],this);
    }

    btn_angle_zero = new CSkinButton(&skinctrls[1],this);
    InitSkinHeader("formcalib");
}

CFormCalib::~CFormCalib()
{

}

void CFormCalib::OnCreate()
{
    //msgHandler = new ADMessageHandler(m_hWnd);
    //TelEngine::Engine::install(msgHandler);
    SetTimer(m_hWnd,101,10);
}
void CFormCalib::OnClose()
{
    //TelEngine::Engine::uninstall(msgHandler);
    KillTimer(m_hWnd,101);
}
void CFormCalib::OnTimer(int ID)
{
    _edtAD[0]->SetText(Poco::format("%d",ad_angle));
    _edtAD[1]->SetText(Poco::format("%0.2f",(double)5.70227*ad_up_angle/4096));
    _edtAD[2]->SetText(Poco::format("%0.2f",(double)5.70227*ad_car_dist/4096));
    _edtAD[3]->SetText(Poco::format("%0.2f",(double)5.70227*ad_weight/4096));
    _edtAD[4]->SetText(Poco::format("%0.2f",(double)5.70227*ad_height/4096));


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
    for(int i = 2 ;i < TABLESIZE(CommCtrls);i++)
    {
        _edtAD[i-2]->Create();
    }
}
//a === bd[0][0..1]
//b === bd[1][0..1]
void CFormCalib::calibrate_height(int type)
{
    static int start_ad = 0;
    static bool start   = false;
    if(type == 0){
        start_ad = ad_height;
        start = true;
        MsgBox box;
        box.ShowBox(this,"最小高度记录成功","提示");
    }else if((type == 1) && (start)){
        start = false;
        if(calibrate_height_next (start_ad))
        {
            MsgBox box;
            box.ShowBox(this,"高度标定完成","提示");
        }

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

        TIniFile cfg(ctx2000_cfg);
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
#include "CaliBox.h"
bool CFormCalib::calibrate_height_ok(int start_ad,double bd_height)
{
    int end_ad = ad_height;

    double k = (end_ad - start_ad) / (bd_height);
    if( (fabs(k) < 0.001) || ( fabs(k) > 10000 ) || ((int)(bd_height) == 0))
    {
        MsgBox box;
        box.ShowBox(this,"高度标定参数错误，请重新标定","错误");
        return false;
    }
    g_bd[BD_HEIGHT].bd_k        = k;
    g_bd[BD_HEIGHT].zero_ad     = start_ad;
    g_bd[BD_HEIGHT].bd_ad       = end_ad;
    g_bd[BD_HEIGHT].start_value = 0;

    TIniFile cfg(ctx2000_cfg);
    cfg.WriteFloat("height_bd","min_height",g_bd[BD_HEIGHT].start_value);
    cfg.WriteFloat("height_bd","height_k",  g_bd[BD_HEIGHT].bd_k);
    cfg.WriteInteger("height_bd","zero_ad", g_bd[BD_HEIGHT].zero_ad);
    cfg.WriteInteger("height_bd","bd_ad",   g_bd[BD_HEIGHT].bd_ad);
}
bool CFormCalib::calibrate_ok(int index,int start_ad,double bd_weight)
{
    TIniFile cfg(ctx2000_cfg);
    int end_ad = ad_weight;

    double k = (end_ad - start_ad) / bd_weight;
    printf("end_ad=%d ,start_ad = %d, bd_weight=%0.2f k = %0.2f\n",end_ad,start_ad,bd_weight,k);
    if(( fabs(k) < 0.001) || ( fabs(k) > 10000 ) || ((int)bd_weight) == 0 )
    {
        std::string value = Poco::format("%0.2f",k);
        MsgBox box;
        box.ShowBox(this,"重量标定参数错误，请重新标定 k=" + value +"\n","错误");
        return false;
    }
    g_bd[BD_WEIGHT+index].valid       = true;
    g_bd[BD_WEIGHT+index].bd_k        = k;
    g_bd[BD_WEIGHT+index].zero_ad     = start_ad;
    g_bd[BD_WEIGHT+index].bd_ad       = end_ad;
    g_bd[BD_WEIGHT+index].start_value = bd_weight;


    if(index==0)
    {
        cfg.WriteFloat("weight_bd","min_weight",g_bd[BD_WEIGHT+index].start_value);
        cfg.WriteFloat("weight_bd","weight_k",g_bd[BD_WEIGHT+index].bd_k);
        cfg.WriteInteger("weight_bd","zero_ad", g_bd[BD_WEIGHT+index].zero_ad);
        cfg.WriteInteger("weight_bd","bd_ad", g_bd[BD_WEIGHT+index].bd_ad);
        cfg.WriteBool ("weight_bd","valid",true);
        cfg.WriteBool ("weight_bd2","valid",false);
        cfg.WriteBool ("weight_bd3","valid",false);

    }else if(index==1){
        cfg.WriteFloat("weight_bd2","min_weight",g_bd[BD_WEIGHT+index].start_value);
        cfg.WriteFloat("weight_bd2","weight_k",g_bd[BD_WEIGHT+index].bd_k);
        cfg.WriteInteger("weight_bd2","zero_ad", g_bd[BD_WEIGHT+index].zero_ad);
        cfg.WriteInteger("weight_bd2","bd_ad", g_bd[BD_WEIGHT+index].bd_ad);
        cfg.WriteBool ("weight_bd2","valid",true);
        cfg.WriteBool ("weight_bd3","valid",false);
    }else if(index==2){
        cfg.WriteFloat("weight_bd3","min_weight",g_bd[BD_WEIGHT+index].start_value);
        cfg.WriteFloat("weight_bd3","weight_k",g_bd[BD_WEIGHT+index].bd_k);
        cfg.WriteInteger("weight_bd3","zero_ad", g_bd[BD_WEIGHT+index].zero_ad);
        cfg.WriteInteger("weight_bd3","bd_ad", g_bd[BD_WEIGHT+index].bd_ad);
        cfg.WriteBool ("weight_bd3","valid",true);
    }


    return true;

}
bool CFormCalib::calibrate_height_next(int start_ad)
{
   int ret = 0;
   double value = 0;

   bool res = false;
   {
       {
           CaliBox box(0);
           ret = box.ShowBox (this,"请输入标定高度","高度标定");
           value = box.m_double_input_value;
       }
   }
   if(ret > 0 )
   {
       return calibrate_height_ok(start_ad,value);
   }
   return res;

}
 bool CFormCalib::calibrate_weight_next(int start_ad)
{
    int ret = 0;
    double value = 0;

    bool res = false;
    {
        {
            CaliBox box;
            ret = box.ShowBox (this,"请输入第一段的标定重量","第一段重量标定");
            value = box.m_double_input_value;
        }
    }
    if(ret > 0 )
    {
        res =  calibrate_ok(0,start_ad,value);
        if(res && (ret == 2)) //calib ok && need next
        {
            {
                CaliBox box;
                ret = box.ShowBox (this,"请输入第二段的标定重量","第二段重量标定");
                value = box.m_double_input_value;
            }

            if(ret > 0)
            {
                res = calibrate_ok(1,start_ad,value);
                if(res && (ret == 2)) //need next
                {
                    {
                        CaliBox box;
                        ret = box.ShowBox (this,"请输入第三段的标定重量","第三段重量标定");
                        value = box.m_double_input_value;
                    }

                    if(ret > 0)
                    {
                        return calibrate_ok(2,start_ad,value);
                    }
                }
            }
        }
    }
    return res;

}
#include <Poco/Format.h>
void CFormCalib::calibrate_weight(int type)
{
    static int start_ad = 0;
    static bool start=false;
    if(type == 0){
        start_ad = ad_weight;
        start    = true;
        MsgBox box;
        box.ShowBox(this,"重量清零成功\n","提示");
    }else if((type == 1) && (start)){
        start = false;
        if(calibrate_weight_next(start_ad))
        {
            MsgBox box;
            box.ShowBox(this,"重量标定完成","提示");
        }

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

        TIniFile cfg(ctx2000_cfg);
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

        TIniFile cfg(ctx2000_cfg);
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

        PassWord pwd;
        if(pwd.ShowBox(this,"密码","力矩配置密码:","1020"))
        {
            KillTimer(m_hWnd,101);
            CTorQueForm lj;
            lj.CreateForm(m_hWnd);
            SetTimer(m_hWnd,101,10);
        }


    }
    else if(item->id == btn_ret->GetId())
    {
        Close();
    }else if(item->id == btn_angle_zero->GetId ()){

    }
}
