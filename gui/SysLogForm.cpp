#include "SysLogForm.h"
#include "SysLogFormItem.h"
#include "BmpRes.h"
#include <Poco/Format.h>
#include "tajidbmgr.h"

static TListColum SysLogList[] =
{
        {"编号",  50},
        {"时间",  200},
        {"级别",  70},
        {"内容",  400}
};


static const char *mmenu_bmps[] = {
    PCOMM_LIST_BKGROUND,
    PCOMM_PREVPAGE_BTN,
    PCOMM_NEXTPAGE_BTN,
    PCOMM_RET_BTN,
    PCOMM_CLOSE_BTN,
    "ctx2000/btn_alarm.png",
    "ctx2000/btn_weight.png",
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] = {
     SKIN_BUTTON_PRE,
     SKIN_BUTTON_NEXT,
     //SKIN_BUTTON_QUIT,
     SKIN_BUTTON_EXIT,
     //SKIN_BUTTON_ALARM,
     //SKIN_BUTTON_WEIGHT

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



CSysLogForm::CSysLogForm()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
            printf("Can't loadres\n");
            exit(0);
    }

    for (int i = 0; i < TABLESIZE(SkinBtnsDesc); i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtnsDesc[i],this);
    }
    _lvAlarmInfo = new CListView(&CommCtrlsDesc[0],this);
    _icons.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));
    m_tailindex = 0;
    m_type      = 0;
    m_colum_num = 0;
    start       = 0;
    InitSkinHeader("SysLogForm");

}

CSysLogForm::~CSysLogForm()
{

}
void   CSysLogForm::ChangeType(int type)
{
    _lvAlarmInfo->ClearColumn();

    m_colum_num = TABLESIZE(SysLogList);

    InitListCol();

}
void    CSysLogForm::OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd);
	EndPaint(hWnd, hdc);
}
void    CSysLogForm::OnButtonClick(skin_item_t* item)
{
        if(item->id == _skinBtns[0]->GetId())
        {
            start = ( (start-10)>0) ? (start-10):0;
            RefreshList(start);
        }else if(item->id == _skinBtns[1]->GetId()){
            start = (start + 10);
            RefreshList(start);
        }else if(item->id == _skinBtns[2]->GetId()){
            Close();
        }

}

void
CSysLogForm::InitListCol()
{

   for (int i = 0; i < m_colum_num; i++)
   {
       _lvAlarmInfo->AddColumn(i,SysLogList[i].caption,SysLogList[i].colum_width);
   }

}

void   CSysLogForm::OnShow()
{
    _lvAlarmInfo->EnableSkinStyle(true);
    _lvAlarmInfo->EnableTransparent(false);
    ChangeType(m_type);
    RefreshList(start);
}
void CSysLogForm::RefreshList(int start_index)
{
    _lvAlarmInfo->Clear();

    TSysLogRst rst;
    CTajiDbMgr::Get().QuerySysLog(start_index,10,rst);

    for(size_t i = 0 ; i < rst.size(); i++)
    {
        StringList list;

        add_alarminfo_item(list,rst.at(i));
        _lvAlarmInfo->AddSubItems(list);
    }
    if(rst.size () > 0)
    {
        _lvAlarmInfo->SortByColnum (0);
    }
}
void CSysLogForm::add_alarminfo_item (StringList &alarminfoItems,TSysLog &AlarmInfo)
{
    char buff[20];
#if 1
    for (int j = 0; j < m_colum_num; j++)
    {
        switch(j)
        {
            case 0:
                sprintf(buff,"%d",AlarmInfo.id);
                alarminfoItems.push_back(buff);
                break;
            case 1:
                alarminfoItems.push_back(AlarmInfo.logtime);
                break;
            case 2:
                static const char* gLogText[] = {"消息","警告","错误"};
                alarminfoItems.push_back(gLogText[AlarmInfo.loglevel]);
                break;
            case 3:
                alarminfoItems.push_back(AlarmInfo.logtext);
                break;
            default:
                    break;
        }
    }
#endif
}

