#include "AlarmInfoManFormItem.h"
#include "AlarmInfoManForm.h"

#include "BmpRes.h"

static char* alarminfolist_caption[] =
{
	"编号",
	"时间",
        "塔吊编号",
        "小车指令",
        "回转指令"
};
#define COL_NR	TABLESIZE(alarminfolist_caption)

static const char *mmenu_bmps[] = {
    PCOMM_LIST_BKGROUND,
    PCOMM_PREVPAGE_BTN,
    PCOMM_NEXTPAGE_BTN,
    PCOMM_RET_BTN,
    PCOMM_CLOSE_BTN
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] = {
	 SKIN_BUTTON_PRE,
	 SKIN_BUTTON_NEXT,
	 SKIN_BUTTON_QUIT,
	 SKIN_BUTTON_EXIT
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
	{
	 QUERY_X,
	 QUERY_Y
	 },
	{
	 QUERY_TEXT_X,
	 QUERY_TEXT_Y
	 },
	{
	 LINE_X,
	 LINE_Y
	 },
	{
	 QUERY_ALARMINFO_X,
	 QUERY_ALARMINFO_Y
	 }
};
static int start = 0;
CAlarmInfoManForm::CAlarmInfoManForm()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
            printf("Can't loadres\n");
            exit(0);
    }

	m_tailindex = 0;

    for (int i = 0; i < ALARMINFO_SKIN_BTNS_NUM; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtnsDesc[i],this);
    }
    _lvAlarmInfo = new CListView(&CommCtrlsDesc[0],this);
    _icons.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));

    InitSkinHeader("CAlarmInfoManForm");

}

CAlarmInfoManForm::~CAlarmInfoManForm()
{

}

void    CAlarmInfoManForm::OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd);

	for (int i = 0; i < SKIN_BMP_NUM; i++)
            _icons.Show(hdc,BmpsPos[i].x,BmpsPos[i].y,i+1);
	EndPaint(hWnd, hdc);
}

void    CAlarmInfoManForm::OnClose()
{

}

void    CAlarmInfoManForm::OnButtonClick(skin_item_t* item)
{
        if(item->id == _skinBtns[0]->GetId())
        {
            start = ( (start-10)>0) ? (start-10):0;
            RefreshList(start);
        }else if(item->id == _skinBtns[1]->GetId()){
            start = (start+10);
            RefreshList(start);
        }else if(item->id == _skinBtns[2]->GetId()){
            Close();
        }else if(item->id == _skinBtns[3]->GetId()){
            Close();
        }

}

void
CAlarmInfoManForm::InitListCol()
{
        int width[COL_NR] = {50, 200, 80, 110,LIST_ALARMINFO_W-440};
        for (int i = 0; i < COL_NR; i++) {
                 _lvAlarmInfo->AddColumn(i,alarminfolist_caption[i],width[i]);
        }

}

void   CAlarmInfoManForm::OnShow()
{
    _lvAlarmInfo->EnableSkinStyle(true);
    InitListCol();   
RefreshList(0);
}

void CAlarmInfoManForm::RefreshList(int start_index)
{

    THistoyRst rst;
    CTajiDbMgr::Get().ListAlaramInfo(rst);
    _lvAlarmInfo->Clear();
    for(size_t i = 0 ; i < rst.size(); i++)
    {
        StringList list;
        add_alarminfo_item(list,rst.at(i));
        _lvAlarmInfo->AddSubItems(list);
    }
}

void CAlarmInfoManForm::add_alarminfo_item (StringList &alarminfoItems,THistoy& AlarmInfo)
{
    char buff[20];

    for (int j = 0; j < COL_NR; j++)
    {
        switch(j)
        {
			case 0:
                               alarminfoItems.push_back("0");
                               break;
			case 1:
                                alarminfoItems.push_back(AlarmInfo.date);
                                break;

			case 2:
                                alarminfoItems.push_back(AlarmInfo.serial);
                                break;

			case 3:
                                sprintf (buff, "%d", AlarmInfo.slewing);
                                alarminfoItems.push_back(buff);
                                break;
                        case 4:
                                sprintf (buff, "%d", AlarmInfo.trolley);
                                alarminfoItems.push_back(buff);
                                break;
			default:
				break;
		}
    }
}

