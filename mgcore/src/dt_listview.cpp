#include "dt_listview.h"
#include <assert.h>
#include <stdio.h>
CListView::CListView(CTRLDATA* data,CSkinForm* parent,string typeName)
    :CCommCtrl(data,parent,typeName)
{
    _colFlags = DEF_COLFLAG;
    _colNum = 0;
    m_set_data = false;
    m_sort_index = 0;
}
bool CListView::EnableTransparent(bool yes)
{
    if( (!_parent) || (_parent->m_hWnd == HWND_INVALID))
        return false;
    HWND lvHwnd = GetDlgItem(_parent->m_hWnd,_id);
    if(lvHwnd == HWND_INVALID)
        return false;
    DWORD style = GetWindowExStyle(lvHwnd);
    if(yes)
    {
        style |= WS_EX_TRANSPARENT;
    }
    else
    {
        style &= ~WS_EX_TRANSPARENT;
    }
    IncludeWindowExStyle(lvHwnd,style);
    InvalidateRect(lvHwnd,NULL,true);
    return true;
}
CListView::CListView(COMM_CTRL_DESC* desc,CSkinForm* parent,string typeName)
{

    _colFlags = DEF_COLFLAG;
    _colNum = 0;

    CTRLDATA *data = new CTRLDATA;

    char* tmp = new char[strlen(desc->caption) + 1];
    memset(tmp,0,strlen(desc->caption) + 1);

    strcpy(tmp,desc->caption);
    data->caption = tmp;

    tmp = new char[typeName.size() + 1];
    memset(tmp,0,typeName.length() + 1);
    strcpy(tmp,typeName.c_str());

    data->class_name=tmp;
    data->dwAddData = 0;
    data->dwExStyle = DEF_LISTVIEW_EXSTYLE;
    data->dwStyle   = DEF_LISTVIEW_STYLE;
    data->id        = -1; //auto alloc id
    data->w         = desc->w;
    data->h         = desc->h;
    data->x         = desc->x;
    data->y         = desc->y;

    //std::cout << "b parent = " <<_parent << std::endl;
    SetParam(data,parent,typeName);
    //std::cout << "a parent = " <<_parent << std::endl;
    //std::cout << "CListView parent = " << &_parent << std::endl;
    _alloc_desc = true;
}
CListView::~CListView()
{

}
bool CListView::AddColumnList (TListColum col[],int size)
{
    for (int i = 0; i < size; i++)
    {
          AddColumn(i,col[i].caption,col[i].colum_width);
    }
    return true;
}
int CListView::AddColumn(int index,std::string itemText,int width)
{
    LVCOLUMN lvcol;

    lvcol.nCols = index;
    lvcol.pszHeadText = (char*)itemText.c_str();
    lvcol.width = width;
    lvcol.pfnCompare = NULL;
    lvcol.colFlags = _colFlags;

    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

        SendDlgItemMessage (_parent->m_hWnd, _id, LVM_ADDCOLUMN, 0, (LPARAM)&lvcol);
        _colNum++;
        _headerStr.push_back(itemText);
        return 1;
    }
    
    else
    {
        return 0;
    }
}

void CListView::Show()
{
    ShowWindow(_parent->GetItemHwnd(_id), SW_SHOW);
}

void CListView::Hide()
{
    ShowWindow(_parent->GetItemHwnd(_id), SW_HIDE);
}

void CListView::SetColWidth(int index,int width)
{
    if(index <= _colNum)
    {
        if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
        {
            LVCOLUMN lvcol;
            //std::cout <<"id = " <<_id <<std::endl;
            SendDlgItemMessage (_parent->m_hWnd, _id, LVM_GETCOLUMN, index, (LPARAM)&lvcol);
            lvcol.width = width;
            SendDlgItemMessage (_parent->m_hWnd, _id, LVM_SETCOLUMN, index, (LPARAM)&lvcol);
        }


    }
}
void CListView::SetColHeight(int height)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

        //std::cout <<"id = " <<_id <<std::endl;
        SendDlgItemMessage (_parent->m_hWnd, _id, LVM_SETHEADHEIGHT, (WPARAM)0, (LPARAM)0);

    }
}
void CListView::SetColHeadText(int index, std::string itemText)
{
    if(index <= _colNum)
    {
        if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
        {
            LVCOLUMN lvcol;
            //std::cout <<"id = " <<_id <<std::endl;
            SendDlgItemMessage (_parent->m_hWnd, _id, LVM_GETCOLUMN, index, (LPARAM)&lvcol);
            lvcol.pszHeadText = (char*)itemText.c_str();
            SendDlgItemMessage (_parent->m_hWnd, _id, LVM_SETCOLUMN, index, (LPARAM)&lvcol);
        }


    }
}
static char* _getColHeaderText(HWND hwnd,int idx)
{
    LVCOLUMN lvcol;
    if(hwnd == HWND_INVALID)
        return "hello";
    SendMessage (hwnd, LVM_GETCOLUMN, idx, (LPARAM)&lvcol);
    printf("%d,%s\n",idx,lvcol.pszHeadText);
    return lvcol.pszHeadText;
}
std::string  CListView::GetColHeaderText(int idx)
{
    /*
    if(idx <= _colNum)
    {
        if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
        {
            HWND h = GetDlgItem(_parent->m_hWnd,_id);
            return _getColHeaderText(h,idx);
        }


    }*/
    if(idx <= _colNum)
    {
        return _headerStr.at(idx);
    }
    return "";
}
int  CListView::GetColWidth(int index)
{
    int ret = 0;
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        //std::cout <<"id = " <<_id <<std::endl;
        ret =  SendDlgItemMessage (_parent->m_hWnd, _id, LVM_GETCOLUMNWIDTH, (WPARAM)0, (LPARAM)0);
        assert(ret != -1);
    }
    return ret;
}
static std::string GetSubItemText(HWND hlist,HLVITEM item,int sub_index=0)
{
    if (hlist != HWND_INVALID)
    {
        LVSUBITEM subitem;
        char strBuf[256] = {0,};

        subitem.subItem = sub_index;
        subitem.pszText = strBuf;
        if( -1 == SendMessage(hlist,LVM_GETSUBITEMTEXT, item , (LPARAM)&subitem))
        {
            return "";
        }
        return strBuf;

    }
    else
    {
        return "";
    }
}
static bool tryParse(const std::string& s, int& value)
{
    char temp;
    return sscanf(s.c_str(), "%d%c", &value, &temp) == 1;
}
static int listview_sortfunc_high (HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortData)
{
    int id1,id2;
    CListView* listview = (CListView*)GetWindowAdditionalData(sortData->hLV);
    assert(listview);
    int sort_idx = listview->m_sort_index;
    //fprintf(stderr,"listview_sortfunc_high col=%d high=%d\n",sortData->ncol,sortData->losorted);
    if(tryParse (GetSubItemText(sortData->hLV,nItem1,sort_idx),id1))
    {
        if(tryParse (GetSubItemText(sortData->hLV,nItem2,sort_idx),id2))
        {
              if(id1 > id2) return -1;
              if(id1 == id2) return 0;
              if(id1< id2) return 1;
        }
    }
    fprintf(stderr,"listview_sortfunc_high error\n");
    return 0;
}
static int listview_sortfunc_low (HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortData)
{
    int id1,id2;
    CListView* listview = (CListView*)GetWindowAdditionalData(sortData->hLV);
    assert(listview);
    int sort_idx = listview->m_sort_index;
    // fprintf(stderr,"listview_sortfunc_low col=%d high=%d\n",sortData->ncol,sortData->losorted);
    if(tryParse (GetSubItemText(sortData->hLV,nItem1,sort_idx),id1))
    {
        if(tryParse (GetSubItemText(sortData->hLV,nItem2,sort_idx),id2))
        {
              if(id1 > id2) return 1;
              if(id1 == id2) return 0;
              if(id1< id2) return -1;
        }
    }
    fprintf(stderr,"listview_sortfunc_low error\n");
    return 0;
}
bool CListView::SortByColnum(int index,bool high)
{
   if(!m_set_data){
       HWND lvHwnd = GetDlgItem(_parent->m_hWnd,_id);
       if(lvHwnd == HWND_INVALID)
           return false;
        SetWindowAdditionalData(lvHwnd,(DWORD)this);
        m_set_data = true;
   }
   return SetSortFunction(index,high?listview_sortfunc_high:listview_sortfunc_low);
}
int  CListView::GetColNum()
{
    int ret = 0;
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

        //std::cout <<"id = " <<_id <<std::endl;
        ret =  SendDlgItemMessage (_parent->m_hWnd, _id, LVM_GETCOLUMNCOUNT, (WPARAM)0, (LPARAM)0);
        assert(ret != -1);
    }
    return ret;
}
//itemHwnd == root item hwnd
GHANDLE  CListView::_AddItem(LVITEM& item,GHANDLE itemHwnd,int height)
{

    item.nItemHeight = height;
    item.nItem       = GetItemNum ();
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

        return SendDlgItemMessage (_parent->m_hWnd, _id, LVM_ADDITEM, (WPARAM)itemHwnd, (LPARAM)&item);

    }
    return HWND_INVALID;
}
GHANDLE CListView::AddRootItem(std::string root)
{
    StringList temp;
    temp.push_back(root);
    return AddSubItems(temp,25,0);
}
bool CListView::SetSortFunction(int colnum,PFNLVCOMPARE sortfunc)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        static LVSORTDATA  data;
        data.hLV      = GetDlgItem (_parent->m_hWnd, _id);
        data.losorted = 1;
        data.ncol     = colnum;
        m_sort_index  = colnum;
        return SendDlgItemMessage (_parent->m_hWnd, _id, LVM_SORTITEMS, (WPARAM)&data, (LPARAM)sortfunc);
    }
   return false;
}
GHANDLE CListView::AddSubItems(StringList subitems,int itemHeight,GHANDLE hRootItem,gal_pixel color)
{
    LVITEM    item;
    LVSUBITEM subdata;
    Lock();

    GHANDLE hItem = _AddItem(item,hRootItem,itemHeight);

    assert(hItem != HWND_INVALID);

    subdata.flags = 0;
    subdata.image = 0;
    subdata.nTextColor = color;
    subdata.nItem   =  0;


    int num = (_colNum>(int)subitems.size())?(int)subitems.size():_colNum;
    for(int  i = 0 ; i < num; i++)
    {
        //subdata.nItem   =  i;
        subdata.subItem = i;
        //subdata.subItem = 0;
        subdata.pszText = (char*)subitems.at(i).c_str();
        //HWND hh = GetDlgItem (parent->m_hWnd, _id);
        //SendNotifyMessage (hh,LVM_SETSUBITEM,(WPARAM)hItem, (LPARAM)&subdata);
        SendDlgItemMessage (_parent->m_hWnd, _id, LVM_SETSUBITEM, (WPARAM)hItem, (LPARAM)&subdata);
    }

    Unlock();
    return hItem;
}
/*
GHANDLE CListView::AddItems(StringList items,int itemHeight)
{
    LVITEM    item;
    LVSUBITEM subdata;
    Lock();

    GHANDLE hItem = _AddItem(item,0,itemHeight);

    assert(hItem != HWND_INVALID);
    static int index = 0;
    subdata.flags = 0;
    subdata.image = 0;
    subdata.nTextColor = RGB2Pixel(HDC_SCREEN,0,0,0);
    subdata.nItem   =  0;


    //printf("nItem = %d\n",subdata.nItem);
    subdata.nTextColor = 0;
    int num = (_colNum>items.size())?items.size():_colNum;
    for(int  i = 0 ; i < num; i++)
    {
        //subdata.nItem   =  i;
        subdata.subItem = i;
        //subdata.subItem = 0;
        subdata.pszText = (char*)items.at(i).c_str();
        SendDlgItemMessage (_parent->m_hWnd, _id, LVM_SETSUBITEM, (WPARAM)hItem, (LPARAM)&subdata);
    }

    Unlock();
    return hItem;
}*/
void CListView::Lock()
{
    SendDlgItemMessage (_parent->m_hWnd, _id, MSG_FREEZECTRL, true,(LPARAM)0);
}	
void CListView::Unlock()
{
    SendDlgItemMessage (_parent->m_hWnd, _id, MSG_FREEZECTRL, FALSE,(LPARAM)0);
				
}	
void CListView::ClearColumn()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        while(GetColNum() > 0)
        {
            SendDlgItemMessage (_parent->m_hWnd, _id, LVM_DELCOLUMN, (WPARAM)0,0);
            if(_colNum>0)_colNum--;
        }
        _headerStr.clear();
    }
}
void CListView::DelColumn(int index)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        SendDlgItemMessage (_parent->m_hWnd, _id, LVM_DELCOLUMN, (WPARAM)index,0);
        if(_colNum>0)_colNum--;

    }
}
 /*LVM_DELITEM
 * int nRow;
 * HLVITEM pi;
 *
 * wParam = (WPARAM)nRow;
 * lParam = (LPARAM)pi;
 * \endcode
 *
 * \param nRow If pi is zero, nRow specified the row index of the target item to delete.
 * \param pi Handle of the target item.
 */
void CListView::DelItem(int index)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        SendDlgItemMessage (_parent->m_hWnd, _id, LVM_DELCOLUMN,index,0 );
    }
}
void CListView::Clear()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        SendDlgItemMessage (_parent->m_hWnd, _data->id, LVM_DELALLITEM, 0, 0);
    }
}

int  CListView::GetItemNum()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        return SendDlgItemMessage (_parent->m_hWnd, _data->id, LVM_GETITEMCOUNT, 0, 0);
    }
    return 0;
}
void CListView::SelectRow(int row)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        SendDlgItemMessage (_parent->m_hWnd, _data->id, LVM_CHOOSEITEM , row, 0);
    }
}
void CListView::SetSubItemText(int index, std::string text)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        SendDlgItemMessage (_parent->m_hWnd, _data->id, LB_SETTEXT, index, (LPARAM)text.c_str());
    }
}

int  CListView::GetCurSelItem()
{
    int ret = 0;
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
    	
        int index = SendDlgItemMessage (_parent->m_hWnd, _data->id, LVM_GETSELECTEDCOLUMN , 0, 0);
       
        ret = index;
    }
    return ret;
}
int  CListView::GetSelCount()
{
    //fprintf(stderr,"_parent=%x,hwnd=%x\n",_parent,_parent->m_hWnd);
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

        return SendDlgItemMessage (_parent->m_hWnd, _data->id, LVM_GETSELECTEDITEM  , 0, 0);
    }
    return 0;
}
bool  CListView::isSelected()
{
    //fprintf(stderr,"_parent=%x,hwnd=%x\n",_parent,_parent->m_hWnd);
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

        return (SendDlgItemMessage (_parent->m_hWnd, _data->id, LVM_GETSELECTEDITEM  , 0, 0) != HWND_NULL);
    }
    return false;

}
std::string CListView::GetSelSubItemText(int index)
{
    if(_parent->m_hWnd)
    {
        HLVITEM hSelItem = SendDlgItemMessage(_parent->m_hWnd, _data->id, LVM_GETSELECTEDITEM, 0, 0);
        if (hSelItem != HWND_INVALID){
            LVSUBITEM subitem;
            char strBuf[256] = {0,};

            subitem.subItem = index;
            subitem.pszText = strBuf;
            if( -1 == SendDlgItemMessage(_parent->m_hWnd, _data->id,LVM_GETSUBITEMTEXT, hSelItem , (LPARAM)&subitem))
                return "";
            return strBuf;

        }
    }

    return "";
}
void CListView::SetCustomDrawFunc(LVCUSTOMDRAWFUNCS* drawFuncs)
{
    if(_parent->m_hWnd)
    {
        SendDlgItemMessage(_parent->m_hWnd, _data->id, LVM_SETCUSTOMDRAW, 0, (LPARAM)drawFuncs);
    }
}


void CListView::ex_listview_draw_bk(HWND hwnd, HLVHDR hlvhdr, HDC hdc , RECT *rcDraw)
{
    RECT rt = *rcDraw;
    rt.bottom+=1;
    bmpBk.Show(hdc,rt);
    //bmpBk.Show(hdc,rcDraw->left,rcDraw->top);
}
void listview_draw_bk(HWND hwnd, HLVHDR hlvhdr, HDC hdc , RECT *rcDraw)
{
    CListView* listview = (CListView*)GetWindowAdditionalData(hwnd);
    if(listview)
        listview->ex_listview_draw_bk(hwnd,hlvhdr,hdc,rcDraw);
}
void sDrawText(HDC hdc,int x,int y,int width,int height,const char* str,UINT format)
{
    RECT rect;
    if(str != NULL){
        SetRect(&rect,x+2,y+2,x+width,y+height);
        DrawText(hdc,str,-1,&rect,format);
    }
}
void CListView::ex_listview_draw_item(HWND hwnd, int idx, HDC hdc , RECT *rcDraw)
{
    bmpBk.Show(hdc,rcDraw->left,rcDraw->top);
    bmpSplit.Show(hdc,rcDraw->right-3,rcDraw->top);
    SetBkMode(hdc,BM_TRANSPARENT);

    const char* strTitle = GetColHeaderText(idx).c_str();
    assert(strTitle != NULL);
    PLOGFONT old = SelectFont(hdc,GetSystemFont(SYSLOGFONT_CAPTION));
    sDrawText(hdc,rcDraw->left+3,0,rcDraw->right-rcDraw->left-4,rcDraw->bottom-rcDraw->top,strTitle,DT_SINGLELINE|DT_LEFT);
    SelectFont(hdc,old);
}
void listview_draw_item(HWND hwnd, int idx, HDC hdc , RECT *rcDraw)
{

    CListView* listview = (CListView*)GetWindowAdditionalData(hwnd);
    if(listview)
        listview->ex_listview_draw_item(hwnd,idx,hdc,rcDraw);
    //SetBkColor(hdc,GetWindowBkColor(hwnd));
    //SetTextColor(hdc,RGBA2Pixel(hdc,16,92,173,255));



}
bool CListView::EnableSkinStyle(bool yes)
{
    LVCUSTOMDRAWFUNCS drawFuncs;
    if(yes)
    {
        HWND lvHwnd = GetDlgItem(_parent->m_hWnd,_id);
        if(lvHwnd == HWND_INVALID)
            return false;
        drawFuncs.pfnDrawHdrBk   = listview_draw_bk;
        drawFuncs.pfnDrawHdrItem = listview_draw_item;
        bmpBk.LoadFile("comm/lvbk.png");
        bmpSplit.LoadFile("comm/lvcol.png");

        SetWindowAdditionalData(lvHwnd,(DWORD)this);
        m_set_data = true;
    }
    else
    {
        drawFuncs.pfnDrawHdrBk   = NULL;
        drawFuncs.pfnDrawHdrItem = NULL;

    }

    this->SetCustomDrawFunc(&drawFuncs);
    return true;
}
bool CListView::EnableTreeViewStyle(bool yes)
{
    if( (!_parent) || (_parent->m_hWnd == HWND_INVALID))
        return false;
    HWND lvHwnd = GetDlgItem(_parent->m_hWnd,_id);
    if(lvHwnd == HWND_INVALID)
        return false;
    DWORD style = GetWindowStyle(lvHwnd);
    if(yes){
        style |= LVS_TREEVIEW;
    }else{
        style &= ~LVS_TREEVIEW;
    }
    IncludeWindowStyle(lvHwnd,style);
    InvalidateRect(lvHwnd,NULL,true);
    return true;
}
GHANDLE  CListView::GetSelectItem()
{
    if(_parent->m_hWnd)
    {
        return SendDlgItemMessage(_parent->m_hWnd, _data->id, LVM_GETSELECTEDITEM, 0, 0);
    }
    return HWND_INVALID;
}
GHANDLE  CListView::GetRelatedWithItem(GHANDLE item,int related)
{
    if(_parent->m_hWnd)
    {
        GHANDLE ret = SendDlgItemMessage(_parent->m_hWnd, _data->id, LVM_GETRELATEDITEM, (WPARAM)related, (LPARAM)item);
        return ret?ret:HWND_INVALID;
    }
    return HWND_INVALID;
}
