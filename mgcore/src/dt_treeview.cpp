#include "dt_treeview.h"

CTreeView::CTreeView(COMM_CTRL_DESC* desc,CSkinForm* parent,std::string root,string typeName)
{
    assert(desc && parent);
    CTRLDATA* data = new CTRLDATA();

    char* tmp = new char[strlen(desc->caption) + 1];
    memset(tmp,0,strlen(desc->caption) + 1);
    strcpy(tmp,desc->caption);
    data->caption = tmp;

    tmp = new char[typeName.size() + 1];
    memset(tmp,0,typeName.length() + 1);
    strcpy(tmp,typeName.c_str());
    data->class_name=tmp;



    m_root_caption   = root;
    m_root_node.text =(char*)m_root_caption.c_str ();
    data->dwAddData = (DWORD)&m_root_node;

    data->dwExStyle = DEF_TREEVIEW_EXSTYLE;
    data->dwStyle   = DEF_TREEVIEW_STYLE;
    data->id        = -1; //auto alloc id
    data->w         = desc->w;
    data->h         = desc->h;
    data->x         = desc->x;
    data->y         = desc->y;

    SetParam(data,parent,typeName);
    _alloc_desc = true;

}
CTreeView::~CTreeView()
{

}
bool  CTreeView::SelectItem(GHANDLE item)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_SETSELITEM  , item, 0)==-1?false:true;

    }
    return false;
}
GHANDLE CTreeView::AddItem(GHANDLE parent,std::string caption,bool fold)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        TVITEMINFO newIteminfo;
        newIteminfo.text = (char*)caption.c_str ();
        if(fold)
            newIteminfo.dwFlags=TVIF_FOLD;
        else
            newIteminfo.dwFlags=0;
        GHANDLE hwnd =  SendDlgItemMessage (_parent->m_hWnd, _id, TVM_ADDITEM, parent, (LPARAM)&newIteminfo);
        return hwnd;
    }
    return 0;
}
bool  CTreeView::GetItemCaption(GHANDLE item,std::string &caption)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
       int len = GetItemCaptionLength(item);
       if(len == 0) return false;

       char* tmp = new char[len+1];
       memset(tmp,0,len+1);

       int ret = SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETITEMTEXT  , (WPARAM)item, (LPARAM)tmp);
       if(ret != -1)
       {
            caption = std::string(tmp);
       }
       delete tmp;

       return ret==-1?false:true;

    }
    return false;
}

int   CTreeView::GetItemCaptionLength(GHANDLE item)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        int len = SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETITEMTEXTLEN  , item, 0);
        return len==-1?0:len;
    }
    return 0;
}
bool  CTreeView::GetSelectItemCaption(std::string &caption)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        GHANDLE selhwnd= GetSelectItem ();
        if(selhwnd != 0)
        {
            return GetItemCaption (selhwnd,caption);
        }
    }
    return false;
}
GHANDLE CTreeView::GetSelectItem()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

       return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETSELITEM  , 0, 0);
    }
    return 0;
}
bool CTreeView::EnableIconStyle(bool yes)
{
    if( (!_parent) || (_parent->m_hWnd == HWND_INVALID))
        return false;
    HWND treeHwnd = GetDlgItem(_parent->m_hWnd,_id);
    if(treeHwnd == HWND_INVALID)
        return false;
    DWORD style = GetWindowStyle(treeHwnd);
    if(yes){
        style |= TVS_WITHICON;
    }else{
        style &= ~TVS_WITHICON;
    }
    //style |= TVS_ICONFORSELECT;
    IncludeWindowStyle(treeHwnd,style);
    InvalidateRect(treeHwnd,NULL,true);
    return true;
}
bool CTreeView::Clear()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

       return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_DELTREE, GetRoot(), 0)==0?true:false;
    }
    return false;
}
GHANDLE CTreeView::GetRoot()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {

       return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETROOT, 0, 0);
    }
    return 0;
}
GHANDLE CTreeView::GetParentItem(GHANDLE item)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
       return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETRELATEDITEM , TVIR_PARENT, item);
    }
    return 0;
}
GHANDLE CTreeView::GetFirstChildItem(GHANDLE item)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
       return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETRELATEDITEM , TVIR_FIRSTCHILD, item);
    }
    return 0;
}
GHANDLE CTreeView::GetPrevItem(GHANDLE item)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
       return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETRELATEDITEM , TVIR_PREVSIBLING, item);
    }
    return 0;
}
GHANDLE CTreeView::GetNextItem(GHANDLE item)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
       return SendDlgItemMessage (_parent->m_hWnd, _id, TVM_GETRELATEDITEM , TVIR_NEXTSIBLING, item);
    }
    return 0;
}
bool CTreeView::EnableTransparent(bool yes)
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
