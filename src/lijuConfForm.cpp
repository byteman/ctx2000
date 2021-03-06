#include "lijuConfFormItem.h"
#include "lijuConfForm.h"
#include "BmpRes.h"
#include "tajidbmgr.h"
#include "MineAddBox.h"
#include "comdata.h"
#include "lijuctrl.h"
#include "iniFile.h"
#include <Poco/Format.h>
#include "SoftKeyboard.h"
#include "MsgBox.h"
#include <Poco/String.h>
extern SoftKeyboard* skt;
static char* userlist_caption[] =
{
    "No.",
    "Arm Length",
    "Weight"
};

#define COL_NR	TABLESIZE(userlist_caption)

static char* userlist_group[] =
{
    "Supervisor",
    "Normal User"
//	"已禁用帐户"
};

#define GROUP_NR	TABLESIZE(userlist_group)

static const char *mmenu_bmps[] =
{
    PCOMM_LIST_BKGROUND,
    PCOMM_ADD_BTN,
    PCOMM_DEL_BTN,
    PCOMM_EDIT_BTN,
    PCOMM_RET_BTN,
    PCOMM_CLOSE_BTN,
    PCOMM_SAVE_BTN,
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] =
{
    SKIN_BUTTON_ADD,
    SKIN_BUTTON_DEL,
    SKIN_BUTTON_EDIT,
    SKIN_BUTTON_QUIT,
    SKIN_BUTTON_EXIT,
    SKIN_BUTTON_SAVE
};
static COMM_CTRL_DESC CommCtrlsDesc[] =
{
    LISTVIEW_USER,
    {95,250,110,40,"0"},
    {95,300,110,40,"0"},
    {95,350,110,40,"0"}
};

static const char *icon_path[] =
{
    PPRESET_SITE_ICON,
    PPRESET_SITE_LABEL
    //PCOMM_LINE_ICON,
    //PPRESET_ADMIN_TEXT
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

CLiJuManForm::CLiJuManForm()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    m_change_fall = false;

    for (int i = 0; i < USERMAN_SKIN_BTNS_NUM; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtnsDesc[i],this);
    }

    _lvLiJu = new CListView(&CommCtrlsDesc[0],this);

    cbx_type= new CComBox(&CommCtrlsDesc[1],this);
    cbx_arm_len=new CComBox(&CommCtrlsDesc[2],this);
    cbx_beilv=new CComBox(&CommCtrlsDesc[3],this);

    _icons.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));

    Font16 = CFontMgr::Get().GetFont("stxinwei",16);
    Font16->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

#define LBL_W  100
#define LBL_H  30
#define LBL_X  5
#define LBL_Y  255
    for(int i = 0; i < 3; i++)
    {
        SetRect(&m_lable[i],        LBL_X,LBL_Y+i*50,  LBL_W+LBL_X,LBL_Y+i*50+LBL_H);
    }
    InitSkinHeader("CUserManForm");

}

CLiJuManForm::~CLiJuManForm()
{
}

static const char* lables[] = {
    "塔机类型:",
    "     臂长:",
    "     倍率:"
};
void
CLiJuManForm::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    for (int i = 0; i < SKIN_BMP_NUM; i++)
        _icons.Show(hdc,BmpsPos[i].x,BmpsPos[i].y,i+1);

    SetPenColor(hdc,PIXEL_black);

    for(int i = 0; i < 3; i ++)
        DrawMyText(hdc,Font16,&m_lable[i],lables[i]);


    EndPaint(hWnd, hdc);
}
bool CLiJuManForm::checkChange()
{
    if(m_change_fall)
    {
        MsgBox box;
        char buf[32]={0,};
        snprintf(buf,32,"请重新设置当前倍率");

        box.ShowBox(this,buf,"提示");
        return false;
        //m_change_fall = false;
    }
    return true;
}
void
CLiJuManForm::OnClose()
{

}

void
CLiJuManForm::OnButtonClick(skin_item_t * item)
{
    if(item->id == _skinBtns[0]->GetId())
    {
        CMineAddBox liju;
        liju.m_arm_len="";
        liju.m_weight ="";
        liju.CreateForm(m_hWnd);
        if(liju.m_bOk){
            std::string type = cbx_type->GetSelItemText();
            std::string len  = cbx_arm_len->GetSelItemText();
            std::string beilv= cbx_beilv->GetSelItemText();
            if( (type.length() > 0 ) && (len.length() > 0) && (beilv.length() > 0)&&(liju.m_arm_len.length() > 0) && (liju.m_weight.length() > 0) )
            {
                CTajiDbMgr::Get().addlijuItem(type,len,beilv,liju.m_arm_len,liju.m_weight);
            }else{
                return;
            }
            RefreshList(type,len,beilv);
        }
    }
    else if(item->id == _skinBtns[1]->GetId())
    {
        std::string len     = _lvLiJu->GetSelSubItemText(1);
        std::string weight  = _lvLiJu->GetSelSubItemText(2);
        std::string type    = cbx_type->GetSelItemText();
        std::string armlen  = cbx_arm_len->GetSelItemText();
        std::string beilv   = cbx_beilv->GetSelItemText();

        fprintf(stderr,"delete len=%s,we=%s\n",len.c_str(),weight.c_str());
        CTajiDbMgr::Get().deleteTjItem(type,armlen,beilv,len,weight);
        //如果已经将该倍率的数据全部删除掉了
        if(false == CTajiDbMgr::Get().CheckExistFall(type,armlen,beilv))
        {
            if(CTajiDbMgr::Get().DeleteFall(type,armlen,beilv))
            {
                fprintf(stderr,"delete fall %s %s %s ok\n",type.c_str(),armlen.c_str(),beilv.c_str());
                MsgBox box;
                char buf[32]={0,};
                snprintf(buf,32,"请重新设置当前倍率");
                if( (Poco::trim(type)==Poco::trim(TCTypeName)) && \
                    (Poco::trim(armlen)==Poco::trim(StrTCArmLen)) && \
                    (Poco::trim(beilv)==Poco::trim(StrTCBeilv)))
                {
                    box.ShowBox(this,buf,"提示");
                    m_change_fall = true;
                }
            }
        }
        ReloadLijuItems();
    }
    else if(item->id == _skinBtns[2]->GetId())
    {
        CMineAddBox liju;
        liju.m_arm_len = _lvLiJu->GetSelSubItemText(1);
        liju.m_weight  = _lvLiJu->GetSelSubItemText(2);
        liju.CreateForm(m_hWnd);
        if(liju.m_bOk){
            std::string type = cbx_type->GetSelItemText();
            std::string len  = cbx_arm_len->GetSelItemText();
            std::string beilv= cbx_beilv->GetSelItemText();
            std::string armlen  = _lvLiJu->GetSelSubItemText(1);
            std::string weight  = _lvLiJu->GetSelSubItemText(2);
            if( (type.length() > 0 ) && (len.length() > 0) && (beilv.length() > 0)&&(liju.m_arm_len.length() > 0) && (liju.m_weight.length() > 0) )
            {
                TLiju newitem,olditem;
                newitem.armlen = liju.m_arm_len;
                newitem.weight = liju.m_weight;
                olditem.armlen = armlen;
                olditem.weight = weight;
                CTajiDbMgr::Get().modifylijuItem(type,len,beilv,olditem,newitem);
            }
            CLijuCtrl::Get().Load();
            RefreshList(type,len,beilv);

        }
    }
    else if(item->id == _skinBtns[3]->GetId())
    {
        if(checkChange())
            Close();
    }
    else if(item->id == _skinBtns[4]->GetId())
    {
        if(checkChange())
            Close();
    }
    else if(item->id == _skinBtns[5]->GetId())
    {
        std::string type    = cbx_type->GetSelItemText();
        std::string armlen  = cbx_arm_len->GetSelItemText();
        std::string beilv   = cbx_beilv->GetSelItemText();
        if( (type.length() > 0 ) && (beilv.length() > 0)&&(armlen.length() > 0) )
        {
            //保存的倍率必须要有数据存在
            if(CTajiDbMgr::Get().checkExistData(CTajiDbMgr::Get().getTableName(type,armlen,beilv)))
            {
                TIniFile cfg("ctx2000.ini");
                cfg.WriteString("device","TC_type",type);
                cfg.WriteString("device","armlen", armlen);
                cfg.WriteString("device","beilv",  beilv);
                MsgBox msg;
                msg.ShowBox(this,"力矩信息保存成功","提示信息");
                m_change_fall = false;
            }else{
                MsgBox msg;
                msg.ShowBox(this,"该倍率没有力距信息","错误提示");
                //m_change_fall = true;
            }

        }


    }
}

void
CLiJuManForm::InitListCol()
{
    int width[COL_NR] = {120, 120, LIST_USER_W-360};
    int index = -1;
    for (int i = 0; i < COL_NR; i++)
    {
        _lvLiJu->AddColumn(i,userlist_caption[i],width[i]);
    }
    _lvLiJu->Clear();
    cbx_arm_len->Clear();
    cbx_type->Clear();
    cbx_beilv->Clear();
    TStringList item;
    if(CTajiDbMgr::Get().getTjTypes(item))
    {
        for(size_t i = 0 ; i < item.size(); i++)
        {
            cbx_type->AddItem(item.at(i));
            if(item.at(i) == TCTypeName)
            {
                index = i;
            }
        }
        cbx_type->SetCurSel(index);
    }
    cbx_arm_len->SetText(StrTCArmLen);
    cbx_beilv->SetText(StrTCBeilv);

}
void CLiJuManForm::ReloadLijuItems()
{
    std::string type = cbx_type->GetSelItemText();
    std::string len  = cbx_arm_len->GetSelItemText();
    std::string beilv= cbx_beilv->GetSelItemText();
    if( type.length()>0 || len.length()>0 || beilv.length()>0)
    {
        RefreshList(type,len,beilv);
    }else{
        _lvLiJu->Clear();
    }
}
void CLiJuManForm::OnShow()
{

    _lvLiJu->EnableSkinStyle(true);
    _lvLiJu->EnableTreeViewStyle(true);
    InitListCol();
    ReloadLijuItems();

}
void CLiJuManForm::ReloadArmLen()
{
    TStringList rst;
    rst.clear();
    cbx_arm_len->Clear();
    if(CTajiDbMgr::Get().getTjArmLen(cbx_type->GetSelItemText(),rst))
    {
        for(size_t i = 0 ; i < rst.size(); i++)
        {
            cbx_arm_len->AddItem(rst.at(i));
        }
        cbx_arm_len->SetCurSel(-1);
    }
}
void CLiJuManForm::ReloadFall()
{
    TStringList rst;
    rst.clear();
    cbx_beilv->Clear();
    if(CTajiDbMgr::Get().getTjBeilv(cbx_type->GetSelItemText(),cbx_arm_len->GetSelItemText(),rst))
    {

        for(size_t i = 0 ; i < rst.size(); i++)
        {
            cbx_beilv->AddItem(rst.at(i));
        }
        cbx_beilv->SetCurSel(-1);
    }
}
void CLiJuManForm::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{
    fprintf(stderr,"nc=%d\n",nc);
    if( (id == cbx_type->GetId()) &&  (nc == CBN_CLOSEUP)){
        ReloadArmLen();
        ReloadFall();
        ReloadLijuItems();
    }else if(id == cbx_arm_len->GetId()){
        if(nc == CBN_CLOSEUP)
        {
            ReloadFall();
            ReloadLijuItems();
        }else if((nc == CBN_DROPDOWN)&&cbx_arm_len->TotalCounts()==0)
        {
            ReloadArmLen();
        }

    }else if(id == cbx_beilv->GetId()){

        if(nc == CBN_CLOSEUP)
        {
            ReloadLijuItems();
        }else if((nc == CBN_DROPDOWN)&&cbx_beilv->TotalCounts()==0)
        {
            ReloadFall();
        }
    }
    if( (nc==EN_SETFOCUS) || (nc==CBN_SETFOCUS ))
    {
        if(skt)skt->T9_Show(true);
    }
}
void CLiJuManForm::RefreshList(std::string type, std::string len ,std::string beilv)
{
    TLijuRst rst;
    _lvLiJu->Clear();
    if(CTajiDbMgr::Get().getlijuItems(type,len,beilv,rst))
    {

         TStringList list;
         for(size_t i = 0; i < rst.size(); i++)
         {
             list.clear();
             list.push_back(Poco::format("%u",i+1));
             list.push_back(rst.at(i).armlen);
             list.push_back(rst.at(i).weight);
            _lvLiJu->AddSubItems(list,25);
         }
    }
}
