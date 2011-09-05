#ifndef CLISTVIEW_H
#define CLISTVIEW_H
/*
 目前一个界面只能支持一个ListView控件
*/
#include "dt_commctrl.h"
#include "dt_icon.h"

#define DEF_LISTVIEW_STYLE 	  WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|WS_BORDER
#define DEF_COLFLAG               LVCF_LEFTALIGN
#define DEF_LISTVIEW_EXSTYLE      WS_EX_TRANSPARENT
typedef vector<std::string>  StringList;
enum SORT_TYPE{
    SORT_ALPHA,
    SORT_NUM
};
typedef struct tag_ListColum{
    std::string caption;
    int         colum_width;
}TListColum;

class CListView:public CCommCtrl
{
public:
    CListView(CTRLDATA* data,CSkinForm* parent,string typeName = CTRL_LISTVIEW);
    CListView(COMM_CTRL_DESC* desc,CSkinForm* parent,string typeName=CTRL_LISTVIEW);
    virtual ~CListView();
    bool AddColumnList (TListColum col[],int size);
    int  AddColumn(int index=0,std::string itemText="null",int width=50);
    void DelColumn(int index);
    void ClearColumn();
    void SetColWidth(int index, int width);
    void SetColHeight(int height);
    void SetColHeadText(int index, std::string itemText);
    std::string  GetColHeaderText(int idx);
    bool SetSortFunction(int colnum,PFNLVCOMPARE sortfunc);
    int  GetColWidth(int index);
    int  GetColNum();
    bool SortByColnum(int index,bool high=true);
    void ex_listview_draw_item(HWND hwnd, int idx, HDC hdc , RECT *rcDraw);
    void ex_listview_draw_bk(HWND hwnd, HLVHDR hlvhdr, HDC hdc , RECT *rcDraw);

    //GHANDLE AddItems(StringList items,int itemHeight=25);
    GHANDLE AddRootItem(std::string root);
    GHANDLE AddSubItems(StringList subitems,int itemHeight=25,GHANDLE hRootItem=0,gal_pixel color=PIXEL_black);
    void Lock();
    void Unlock();
    void DelItem(int index);
    void Clear();
    void SelectRow(int row);
    int  GetItemNum();
    std::string GetSelSubItemText(int index);
    void SetSubItemText(int index, std::string text);  
    int  GetCurSelItem();
    void SetCustomDrawFunc(LVCUSTOMDRAWFUNCS* drawFuncs);
    bool EnableSkinStyle(bool yes);
    bool EnableTreeViewStyle(bool yes);
    int  GetSelCount();
    bool isSelected();
    void Show();
    void Hide();
    bool EnableTransparent(bool yes);
    GHANDLE  GetSelectItem();

/*
    related 	A integer which indicates the relationship between the item to retrive
                and the specified item, can be one of the following values:

    * TVIR_PARENT
      To retrive the parent item of the specified item.
    * TVIR_FIRSTCHILD
      To retrive the first child item of the specified item.
    * TVIR_NEXTSIBLING
      To retrive the next sibling item of the specified item.
    * TVIR_PREVSIBLING
      To retrive the previous sibling item of the specified item.

        item 	The handle to the known item.
*/
    GHANDLE  GetRelatedWithItem(GHANDLE item,int related);
    //bool SetSortType(SORT_TYPE type=SORT_NUM);
private:   
    GHANDLE  _AddItem(LVITEM& item,GHANDLE itemHwnd,int height);

    std::string _strText;
    StringList _headerStr;
    DWORD _colFlags;
    int		_colNum;
    CIcon bmpBk,bmpSplit;
    //ItemsList selItemList;
};

#endif // CLISTVIEW_H
