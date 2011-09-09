#include "AlarmInfoManFormItem.h"
#include "AlarmInfoManForm.h"
#include "BmpRes.h"
#include <Poco/Format.h>
#include "usbstormanager.h"
#include "MsgBox.h"
#include "comdata.h"
static TListColum alarminfolist[] =
{
        {"编号",      100},
        {"时间",      200},
        {"小车指令",  150},
        {"回转指令",  150}
};

static TListColum weightinfolist[] =
{
        //{"类型",    55},
        {"编号",    100},
        {"时间",    200},
        {"幅度",    50},
        {"重量",    50},
        {"倍率",    50},
        {"角度",    50},
        {"风速",    50},
};

static const char *mmenu_bmps[] = {
    PCOMM_LIST_BKGROUND,
    PCOMM_PREVPAGE_BTN,
    PCOMM_NEXTPAGE_BTN,
    PCOMM_RET_BTN,
    PCOMM_CLOSE_BTN,
    "ctx2000/btn_alarm.png",
    "ctx2000/btn_weight.png",
    "comm/export.png"
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] = {
     SKIN_BUTTON_PRE,
     SKIN_BUTTON_NEXT,
     SKIN_BUTTON_EXPORT,
     SKIN_BUTTON_EXIT,
     SKIN_BUTTON_ALARM,
     SKIN_BUTTON_WEIGHT,
};
static COMM_CTRL_DESC CommCtrlsDesc[] = {
    LISTVIEW_ALARMINFO
};

static const char *icon_path[] = {
	QUERY_SITE_ICON, 
	QUERY_SITE_LABEL,
    PCOMM_LINE_ICON,
	QUERY_ALARMINFO_TEXT
};

static POS BmpsPos[] = {
    {QUERY_X,QUERY_Y},
    {QUERY_TEXT_X,QUERY_TEXT_Y},
    {LINE_X,LINE_Y},
    {QUERY_ALARMINFO_X,QUERY_ALARMINFO_Y}
};


bool CAlarmInfoManForm::UsbStorNotify(U_STATUS status,std::string mountDir)
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
CAlarmInfoManForm::CAlarmInfoManForm()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
            printf("Can't loadres\n");
            exit(0);
    }

    for (int i = 0; i < ALARMINFO_SKIN_BTNS_NUM; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtnsDesc[i],this);
    }
    _lvAlarmInfo = new CListView(&CommCtrlsDesc[0],this);
    _icons.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));
    m_tailindex = 0;
    m_type      = 0;
    m_colum_num = 0;
    start       = 0;
    m_usb.LoadFile ("comm/usb.png");
    InitSkinHeader("CAlarmInfoManForm");

}

CAlarmInfoManForm::~CAlarmInfoManForm()
{

}
void   CAlarmInfoManForm::ChangeType(int type)
{
    _lvAlarmInfo->ClearColumn();
    if(type==0){
        m_colum_num = TABLESIZE(alarminfolist);
    }else if(type == 1){
        m_colum_num = TABLESIZE(weightinfolist);
    }
    m_type = type;
    InitListCol();

}
void    CAlarmInfoManForm::OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd);

	for (int i = 0; i < SKIN_BMP_NUM; i++)
            _icons.Show(hdc,BmpsPos[i].x,BmpsPos[i].y,i+1);
    if(USBStorManager::get ().getUsbStorage (0))
    {
        m_usb.Show (hdc, 720,400);
    }
	EndPaint(hWnd, hdc);
}
bool exportdata(std::string tc_serial, std::string path)
{
    char buf[128]={0,};
    snprintf(buf,128,"./etc/dump.sh %s %s",tc_serial.c_str(),path.c_str());
    int ret = system(buf);
    printf("cmd = %s ret = %d\n",buf,ret);
    return !ret;
}
void    CAlarmInfoManForm::OnButtonClick(skin_item_t* item)
{


        if(item->id == _skinBtns[0]->GetId())
        {
            start = ( (start-10)>0) ? (start-10):0;
            RefreshList(start);
        }else if(item->id == _skinBtns[1]->GetId()){
            start = (start + 10);
            RefreshList(start);
        }else if(item->id == _skinBtns[2]->GetId()){
            USBStorage* usb = USBStorManager::get ().getUsbStorage (0);
            MsgBox box;
            if(usb){

                if(exportdata (CurSerial,usb->mountDir+"/"))
                {
                    box.ShowBox (this,"导出成功","导出数据");
                }else{
                    box.ShowBox (this,"导出失败","导出数据");
                }
            }else{
                box.ShowBox (this,"没有检测到U盘","导出数据");
            }
        }else if(item->id == _skinBtns[3]->GetId()){
            Close();
        }else if(item->id == _skinBtns[4]->GetId()){
            ChangeType(0);
            start = 0;
            RefreshList(start);
        }else if(item->id == _skinBtns[5]->GetId()){
            ChangeType(1);
            start = 0;
            RefreshList(start);
        }

}

void
CAlarmInfoManForm::InitListCol()
{
   if(m_type==0)
   {
       _lvAlarmInfo->AddColumnList (alarminfolist,TABLESIZE(alarminfolist));
   }else if(m_type==1)
   {
       _lvAlarmInfo->AddColumnList (weightinfolist,TABLESIZE(weightinfolist));
   }

}
void  CAlarmInfoManForm::OnClose ()
{
    USBStorManager::get ().stop ();
    USBStorManager::get ().deleteListener (this);
}
void   CAlarmInfoManForm::OnShow()
{
    _lvAlarmInfo->EnableSkinStyle(true);
    ChangeType(m_type);
    RefreshList(start);
    USBStorManager::get ().start ();
    USBStorManager::get ().addListener (this);
}
std::string GetSubItemText(HWND hlist,HLVITEM item)
{
    if (hlist != HWND_INVALID){
        LVSUBITEM subitem;
        char strBuf[256] = {0,};

        subitem.subItem = 0;
        subitem.pszText = strBuf;
        if( -1 == SendMessage(hlist,LVM_GETSUBITEMTEXT, item , (LPARAM)&subitem))
            return "";
        return strBuf;

    }
}
void CAlarmInfoManForm::RefreshList(int start_index)
{
    _lvAlarmInfo->Clear();
    if(m_type == 0)
    {
        THistoyRst rst;
        CTajiDbMgr::Get().ListAlaramInfo(start_index,10,rst);
        for(size_t i = 0 ; i < rst.size(); i++)
        {
            StringList list;
            list.clear ();
            add_alarminfo_item (list,rst.at(i));
            _lvAlarmInfo->AddSubItems(list);
        }
        if(rst.size() > 0)
        {
            _lvAlarmInfo->SortByColnum (0);
            //_lvAlarmInfo->SetSortFunction (0,listview_sortfunc);
        }

    }else if(m_type == 1){
        TWeightHistoyRst rst;
        CTajiDbMgr::Get().ListWeightInfo(start_index,10,rst);
        for(size_t i = 0 ; i < rst.size(); i++)
        {
            StringList list;
            add_weightinfo_item(list,rst.at(i));
            if(rst.at(i).type > 0)
            {
                _lvAlarmInfo->AddSubItems(list,25,0,PIXEL_red);
            }
            else
                _lvAlarmInfo->AddSubItems(list);
        }
        if(rst.size() > 0)
            _lvAlarmInfo->SortByColnum (0);
    }

}


void CAlarmInfoManForm::add_weightinfo_item (StringList &weightinfoItems,TWeightHistoy& WeightInfo)
{
    char buff[20];
#if 1
    for (int j = 0; j < m_colum_num; j++)
    {
        switch(j)
        {
            case 0:
                   sprintf (buff, "%d", WeightInfo.id);
                   weightinfoItems.push_back(buff);
                   break;
            case 1:
                    weightinfoItems.push_back(WeightInfo.date);
                    break;

            case 2:
                    weightinfoItems.push_back(WeightInfo.dist);
                    break;

            case 3:
                    weightinfoItems.push_back(WeightInfo.weight);
                    break;
            case 4:
                    weightinfoItems.push_back(WeightInfo.fall);
                    break;
            case 5:
                    weightinfoItems.push_back(WeightInfo.angle);
                    break;
            case 6:
                    weightinfoItems.push_back(WeightInfo.speed);
                    break;

            default:
                    break;
        }
    }
#endif
}

void CAlarmInfoManForm::add_alarminfo_item (StringList &alarminfoItems,THistoy& AlarmInfo)
{
    char buff[20];
#if 1
    for (int j = 0; j < m_colum_num; j++)
    {
        switch(j)
        {

            case 0:
                memset(buff,0,20);
                sprintf(buff,"%d",AlarmInfo.id);
                alarminfoItems.push_back(buff);
                break;
            case 1:
                alarminfoItems.push_back(AlarmInfo.date);
                break;
            case 2:
                if(AlarmInfo.slewing=="170")
                    alarminfoItems.push_back("Bypass");
                else{
                    alarminfoItems.push_back(AlarmInfo.slewing);
                }
                break;
            case 3:
                if(AlarmInfo.slewing=="170")
                {
                    if(AlarmInfo.trolley=="1"){
                        alarminfoItems.push_back("ON");
                    }else {
                        alarminfoItems.push_back("OFF");
                    }
                }else
                    alarminfoItems.push_back(AlarmInfo.trolley);
                break;


            default:
                    break;
        }
    }
#endif
}

