#include "AlarmInfoManFormItem.h"
#include "AlarmInfoManForm.h"
#include "BmpRes.h"
#include <Poco/Format.h>
typedef struct tag_ListColum{
    const char* caption;
    int         colum_width;
}TListColum;
static TListColum alarminfolist[] =
{
        {"编号",      50},
        {"时间",      200},
        //{"塔吊编号",  100},
        {"小车指令",  100},
        {"回转指令",  100}
};

static TListColum weightinfolist[] =
{
        //{"类型",    55},
        {"编号",    100},
        {"时间",    150},
        {"幅度",    50},
        {"重量",    50},
        {"倍率",    50},
        {"角度",    50},
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
     SKIN_BUTTON_QUIT,
     SKIN_BUTTON_EXIT,
     SKIN_BUTTON_ALARM,
     SKIN_BUTTON_WEIGHT

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
	EndPaint(hWnd, hdc);
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
            Close();
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
       for (int i = 0; i < m_colum_num; i++)
       {
           _lvAlarmInfo->AddColumn(i,alarminfolist[i].caption,alarminfolist[i].colum_width);
       }
   }else if(m_type==1)
   {
       for (int i = 0; i < m_colum_num; i++)
       {
           _lvAlarmInfo->AddColumn(i,weightinfolist[i].caption,weightinfolist[i].colum_width);
       }
   }

}

void   CAlarmInfoManForm::OnShow()
{
    _lvAlarmInfo->EnableSkinStyle(true);
    ChangeType(m_type);
    RefreshList(start);
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
            add_alarminfo_item(list,rst.at(i));
            _lvAlarmInfo->AddSubItems(list);
        }
    }else if(m_type == 1){
        TWeightHistoyRst rst;
        CTajiDbMgr::Get().ListWeightInfo(start_index,10,rst);

        for(size_t i = 0 ; i < rst.size(); i++)
        {
            StringList list;
            add_weightinfo_item(list,rst.at(i));
            if(rst.at(i).type == 1)
            {
                _lvAlarmInfo->AddSubItems(list,25,0,PIXEL_red);
            }
            else
                _lvAlarmInfo->AddSubItems(list);
        }
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
                sprintf(buff,"%d",AlarmInfo.id);
                alarminfoItems.push_back(buff);
                break;
            case 1:
                alarminfoItems.push_back(AlarmInfo.date);
                break;
            case 2:
                alarminfoItems.push_back(AlarmInfo.slewing);
                break;
            case 3:
                alarminfoItems.push_back(AlarmInfo.trolley);
                break;


            default:
                    break;
        }
    }
#endif
}

