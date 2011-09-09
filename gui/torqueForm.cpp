#include "torqueForm.h"
#include <Poco/Format.h>
#include <Poco/String.h>
#include "BmpRes.h"
#include "torque_mgr.h"
#include "torqueInfoBox.h"
#include <Poco/String.h>
#include "MsgBox.h"
#include "comdata.h"
#define SKIN_CTRL_STYLE_NUM		5


#define SKIN_BMP_NUM					4
#define SYSMT_X						60
#define SYSMT_Y						65
#define SYSMT_TEXT_X						60
#define SYSMT_TEXT_Y						185
#define LINE_X		62
#define LINE_Y					220
#define SYSMT_ADMIN_X						85
#define SYSMT_ADMIN_Y						225


#define	BUTTON_ADD_X						270
#define	BUTTON_ADD_Y						410
#define	BUTTON_DEL_X						390
#define	BUTTON_DEL_Y						410
#define	BUTTON_EDIT_X						510
#define	BUTTON_EDIT_Y						410
#define	BUTTON_QUIT_X						630
#define	BUTTON_QUIT_Y						410
#define	BUTTON_EXIT_X						738
#define	BUTTON_EXIT_Y						8

/* new style */

#define LEFT -30
#define SKIN_BUTTON_ADD        {1,BUTTON_ADD_X+LEFT,BUTTON_ADD_Y}
#define SKIN_BUTTON_DEL        {2,BUTTON_DEL_X+LEFT,BUTTON_DEL_Y}
#define SKIN_BUTTON_EDIT       {3,BUTTON_EDIT_X+LEFT,BUTTON_EDIT_Y}
#define SKIN_BUTTON_IMPORT     {4,BUTTON_QUIT_X+LEFT,BUTTON_QUIT_Y}
#define SKIN_BUTTON_EXIT       {5,BUTTON_EXIT_X,BUTTON_EXIT_Y}
#define SKIN_BUTTON_SAVE       {6,BUTTON_ADD_X-200,BUTTON_ADD_Y}

#ifdef TOR_DEBUG
#define TOR_DBG(fmt...) fprintf(stderr,fmt);
#else
#define TOR_DBG(fmt...) do { } while (0)
#endif
static CTorQueDB gDB;
static TStrList  typelist;

#if 0
static char* userlist_caption[] =
{
    "No.",
    "Arm Length",
    "Weight"
};
#endif
static TListColum columlist[] =
{
        {"编号",      50},
        {"小车位置",  250},
        {"额定重量",  250},
};
#define COL_NR	TABLESIZE(columlist)

static const char *mmenu_bmps[] =
{
    PCOMM_LIST_BKGROUND,
    PCOMM_ADD_BTN,
    PCOMM_DEL_BTN,
    PCOMM_EDIT_BTN,
    "comm/import.png",
    PCOMM_CLOSE_BTN,
    PCOMM_SAVE_BTN,

};

static SKIN_BUTTON_DESC SkinBtnsDesc[] =
{
    SKIN_BUTTON_SAVE,
    SKIN_BUTTON_ADD,
    SKIN_BUTTON_DEL,
    SKIN_BUTTON_EDIT,
    SKIN_BUTTON_EXIT,
    SKIN_BUTTON_IMPORT

};
static COMM_CTRL_DESC CommCtrlsDesc[] =
{
    {20,50,200,350,"0"},
    {230,100,550,300,"0"},
    {320,55,100,40,""},
    {480,55,100,40,""},
    {640,55,100,40,""}
};

static const char *icon_path[] =
{
    PPRESET_SITE_ICON,
    PPRESET_SITE_LABEL
};

static POS BmpsPos[] =
{
    {
        SYSMT_X,
        SYSMT_Y
    },
    {
        SYSMT_TEXT_X,
        SYSMT_TEXT_Y
    },
    {
        LINE_X,
        LINE_Y
    },
    {
        SYSMT_ADMIN_X,
        SYSMT_ADMIN_Y
    }
};
bool CTorQueForm::UsbStorNotify(U_STATUS status,std::string mountDir)
{
    if(status == U_ATTACHED)
    {
        HDC hdc = GetClientDC (m_hWnd);
        m_usb.Show (hdc, 720,400);
        ReleaseDC (hdc);
    }else{
        HDC hdc = GetClientDC (m_hWnd);
        m_usb.Hide ();
        ReleaseDC (hdc);
    }
}
CTorQueForm::CTorQueForm()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    m_change_fall = false;
    m_sel_hwnd    = 0;
    m_usb.LoadFile ("comm/usb.png");
    for (int i = 0; i < TABLESIZE(SkinBtnsDesc); i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtnsDesc[i],this);
    }

    tv = new CTreeView(&CommCtrlsDesc[0],this,"塔机曲线信息");
    lv = new CListView(&CommCtrlsDesc[1],this);
    cbx_type= new CComBox(&CommCtrlsDesc[2],this);
    cbx_arm_len=new CComBox(&CommCtrlsDesc[3],this);
    cbx_beilv=new CComBox(&CommCtrlsDesc[4],this);

    _icons.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));

    Font16 = CFontMgr::Get().GetFont("stxinwei",16);
    assert(Font16);
    Font16->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

#define LBL_W  80
#define LBL_H  30
#define LBL_X  230
#define LBL_Y  60
    for(int i = 0; i < 3; i++)
    {
        SetRect(&m_lable[i],        LBL_X+i*150,LBL_Y,  LBL_W+LBL_X+i*180,LBL_Y+LBL_H);
    }
    InitSkinHeader("CUserManForm");

    m_cur_tctype = TCTypeName;
    m_cur_armlen = StrTCArmLen;
    m_cur_rate   = StrTCRate;

}

CTorQueForm::~CTorQueForm()
{
}

static const char* lables[] = {
    "塔机类型:",
    "    臂长:",
    "    倍率:"
};
void
CTorQueForm::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    //for (int i = 0; i < SKIN_BMP_NUM; i++)
    //    _icons.Show(hdc,BmpsPos[i].x,BmpsPos[i].y,i+1);

    SetPenColor(hdc,PIXEL_black);

    for(int i = 0; i < 3; i ++)
        DrawMyText(hdc,Font16,&m_lable[i],lables[i]);

    if(USBStorManager::get ().getUsbStorage (0)){
        m_usb.Show (hdc, 720,400);
    }
    EndPaint(hWnd, hdc);
}
void
CTorQueForm::OnClose()
{
    USBStorManager::get ().stop();
    USBStorManager::get ().deleteListener (this);
}
bool CTorQueForm::DoIt(int type,std::string dist,std::string wet)
{
    TorQueInfo info( dist,wet);
    std::string strType = Poco::trim(cbx_type->GetText ());
    std::string strDist = Poco::trim(cbx_arm_len->GetText ());
    std::string strRate = Poco::trim(cbx_beilv->GetText ());

    if( (strType.length () == 0) || (strDist.length () == 0) || (strRate.length () == 0))
    {
        return false;

    }
    if(type == _skinBtns[1]->GetId ())
    {

        if(gDB.addTorQueInfo (strType,strDist,strRate,info))
        {
             return loadDB (strType,strDist,strRate);
        }
    }else   if(type == _skinBtns[2]->GetId ())
    {

        if(gDB.delTorQueInfo(strType,strDist,strRate,info))
        {
             return loadDB (strType,strDist,strRate);
        }
    }else   if(type == _skinBtns[3]->GetId ())
    {

        TorQueInfo oldinfo;
        oldinfo.len     = lv->GetSelSubItemText (1);
        oldinfo.weight  = lv->GetSelSubItemText (2);
        if(gDB.modifyTorQueInfo(strType,strDist,strRate,info,oldinfo))
        {
             return loadDB (strType,strDist,strRate);

        }
    }
    return false;
}
void
CTorQueForm::OnButtonClick(skin_item_t * item)
{
    if(item->id == _skinBtns[0]->GetId ()) //save
    {
        std::string type = cbx_type->GetText ();
        std::string arm  = cbx_arm_len->GetText ();
        std::string rate = cbx_beilv->GetText ();

        if(type.length () != 0 && arm.length () != 0 && rate.length () != 0)
        {
            if( CTorQueMgr::get ().saveCfg (type,arm,rate))
            {
                MsgBox box;
                box.ShowBox (this,"力矩配置保存成功,重启后生效","提示");
            }
        }
    }
    else if(item->id == _skinBtns[1]->GetId ()) //add
    {
           CTorqueInfoBox box;
           box.CreateForm (m_hWnd);
           if(box.m_bOk)
           {
               DoIt(item->id,box.m_arm_len,box.m_weight);
           }
    }else if(item->id == _skinBtns[2]->GetId ()) //del
    {
        std::string dist = Poco::trim(lv->GetSelSubItemText (1));
        std::string wet  = Poco::trim(lv->GetSelSubItemText (2));
        if( (dist.length () == 0) || (wet.length () == 0)) return;
        DoIt(item->id,dist,wet);

    }else if(item->id == _skinBtns[3]->GetId ())//edit
    {
        CTorqueInfoBox box;
        box.CreateForm (m_hWnd);
        if(box.m_bOk)
        {
            DoIt(item->id,box.m_arm_len,box.m_weight);
        }
    }else if(item->id == _skinBtns[4]->GetId ()) //exit
    {
        Close();
    }else if(item->id == _skinBtns[5]->GetId ()) //import
    {
        USBStorage *usb = USBStorManager::get ().getUsbStorage (0);
        if(!usb){
            MsgBox box;
            box.ShowBox (this,"没有检测到U盘","提示");
            return;
        }
        if(!usb->exportData ("tc.db",ctx2000_torque_db))
        {
            MsgBox box;
            box.ShowBox (this,"导入数据失败","提示");
            return;
        }
        try{
             gDB.open(ctx2000_torque_db);
             loadDB(m_cur_tctype,m_cur_armlen,m_cur_rate);
             CTorQueMgr::get ().ReloadCfg ();
          }catch(...)
          {
            MsgBox box;
            box.ShowBox (this,"数据库打开失败\n","提示");
          }
          MsgBox box;
          box.ShowBox (this,"导入数据成功,重启后生效\n","提示");

    }
}

void
CTorQueForm::InitListCol()
{
    lv->AddColumnList (columlist,COL_NR);
}
void CTorQueForm::ReloadTorqueItems(std::string type, std::string armLen, std::string rate,bool clear)
{
    TorQueSet rst;
    cbx_type->SetText (type);
    cbx_arm_len->SetText (armLen);
    cbx_beilv->SetText (rate);
    if(clear) lv->Clear ();
    if(gDB.listTorQueInfoByRate(type.c_str(),armLen.c_str(),rate.c_str(),rst))
    {
        lv->Clear ();

        for(size_t i = 0; i < rst.size (); i++)
        {
            StringList info;
            info.push_back(Poco::format ("%u",i));
            info.push_back(rst.at (i).len);
            info.push_back(rst.at (i).weight);
            lv->AddSubItems (info,25);
        }
        if(rst.size () > 0){
            lv->SortByColnum (1,false);
        }
    }
}
bool CTorQueForm::loadDB(std::string cur_type, std::string cur_armLen, std::string cur_rate)
{
     if(!gDB.existData() )
     {
        if(!gDB.createParamTable())
        {
            //MessageBox(m_hWnd,"数据不存在,创建参数表失败" + AnsiString(gDB.m_lastErr.c_str()));
            return false;
        }
     }
     typelist.clear();
     tv->Clear();
     if(gDB.listAllTCTypes(typelist))
     {
       GHANDLE root =  tv->GetRoot ();
       for(size_t i = 0; i < typelist.size(); i++)
       {
           bool fold = true;
           bool equal_type=false;
           std::string type =   "塔机类型_"+ typelist.at(i);
           TOR_DBG("cur=%s type=%s\n",cur_type.c_str (),typelist.at(i).c_str());
           if(typelist.at(i) == cur_type)
           {
               //fold = false;
               TOR_DBG("type=%s\n",cur_type.c_str ());
               equal_type = true;
           }
           GHANDLE tyNode = tv->AddItem(root,type,fold);
           fold = true;
           TStrList  lenlist;
           if(gDB.listArmLenByType(typelist.at(i),lenlist))
           {
                 for(size_t j = 0; j < lenlist.size(); j++)
                 {
                    bool equal_armlen = false;
                    std::string len =   "臂长_"+lenlist.at(j);
#if 1
                    if( (lenlist.at(j) == cur_armLen) && (equal_type) )
                    {
                        equal_armlen=true;
                        TOR_DBG("equal_armlen=%s\n",cur_armLen.c_str ());
                    }
#endif
                    GHANDLE lenNode = tv->AddItem(tyNode,len,fold);

                    TStrList  ratelist;
                    if(gDB.listRateByLen(typelist.at(i),lenlist.at(j),ratelist))
                    {
                        for(size_t k = 0; k < ratelist.size(); k++)
                        {
                              std::string rate =   "倍率_"+ratelist.at(k);
                              GHANDLE hwnd = tv->AddItem(lenNode,rate);
                              if(ratelist.at(k) == cur_rate)
                              {
                                  if(equal_type && equal_armlen)
                                  {
                                      m_sel_hwnd = hwnd;
                                      TOR_DBG("cur_rate=%s\n",cur_rate.c_str ());
                                      equal_type   = false;
                                      equal_armlen = false;
                                  }

                              }
                        }
                    }
                 }
           }

       }
       if(m_sel_hwnd!=0)
       {
            tv->SelectItem (m_sel_hwnd);
            m_sel_hwnd = 0;
       }
       ReloadTorqueItems (cur_type,cur_armLen,cur_rate);

     }
}
void CTorQueForm::OnShow()
{

    InitListCol ();
    tv->EnableIconStyle(true);
    try{
         gDB.open(ctx2000_torque_db);
         loadDB(m_cur_tctype,m_cur_armlen,m_cur_rate);
    }catch(...)
    {
        TOR_DBG("open db failed\n");
        return;
    }
    USBStorManager::get ().start();
    USBStorManager::get ().addListener (this);
    cbx_type->SetText (TCTypeName);
    cbx_arm_len->SetText (StrTCArmLen);
    cbx_beilv->SetText (StrTCRate);
}
std::string GetRealText(std::string txt)
{
          int pos = txt.find ("_");
          if(pos != std::string::npos)
          {
            return txt.substr (pos+1,txt.length ()-pos);
          }
          return "";

}
#include "SoftKeyboard.h"
extern SoftKeyboard *skt;
void CTorQueForm::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{
    if(id == tv->GetId ())
    {
        if(nc == TVN_CLICKED)
        {
            std::string rate;
            bool ret = tv->GetSelectItemCaption (rate);
            if(ret)
            {
                if(rate.find ("倍率") != std::string::npos)
                {
                    GHANDLE hwndRate =  tv->GetSelectItem ();
                    GHANDLE hwndArmLen =  tv->GetParentItem (hwndRate);
                    GHANDLE hwndType = tv->GetParentItem (hwndArmLen);
                    std::string armLen,type;
                    if(tv->GetItemCaption (hwndArmLen,armLen) && tv->GetItemCaption (hwndType,type) )
                    {
                        rate   = GetRealText(rate);
                        armLen = GetRealText(armLen);
                        type   = GetRealText(type);

                        cbx_type->SetText (type);
                        cbx_arm_len->SetText (armLen);
                        cbx_beilv->SetText (rate);
                        ReloadTorqueItems (type,armLen,rate,true);
                    }

                }
            }
        }
    }
    else if( id == cbx_type->GetId () || id == cbx_arm_len->GetId () || id==cbx_beilv->GetId ())
    {
        if(nc==CBN_SETFOCUS)
        {
            if(skt)
            {
                skt->T9_Show(true);
            }
        }
    }
}
