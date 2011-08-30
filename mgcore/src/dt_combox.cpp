#include "dt_combox.h"

CComBox::CComBox(CTRLDATA* data,CSkinForm* parent,string typeName):
    CCommCtrl(data,parent,typeName)
{

}
CComBox::CComBox(COMM_CTRL_DESC* desc,CSkinForm* parent,string typeName)
{
    CTRLDATA *data = new CTRLDATA();
    char* tmp = new char[strlen(desc->caption) + 1];
    memset(tmp,0,strlen(desc->caption) + 1);

    strcpy(tmp,desc->caption);
    data->caption = tmp;

    tmp = new char[typeName.size() + 1];
    memset(tmp,0,typeName.length() + 1);
    strcpy(tmp,typeName.c_str());

    data->class_name=tmp;
    data->dwAddData = 0;
    data->dwExStyle = DEF_COMBOX_EXSTYLE;
    data->dwStyle   = DEF_COMBOX_STYLE;
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
CComBox::~CComBox()
{

}
void CComBox::AddItem(std::string itemText,std::string icon)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        //std::cout <<"CComBox::AddItem id = " <<_id <<std::endl;
        SendDlgItemMessage (_parent->m_hWnd, _id, CB_ADDSTRING, 0, (LPARAM)itemText.c_str());
    }

}
void CComBox::InsertItem(int index,std::string itemText,std::string icon)
{

    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        //std::cout <<"CComBox::InsertItem = " <<_id <<std::endl;
        SendDlgItemMessage (_parent->m_hWnd, _id, CB_INSERTSTRING, index, (LPARAM)itemText.c_str());
    }
}
void CComBox::DelItem(std::string text)
{
    int cnt = TotalCounts();
    fprintf(stderr,"cbx cnt =%d\n",cnt);
    for(int i = 0; i < cnt; i++)
    {
        std::string text1 = GetItemText(i);
        if(text1 == text)
        {
            DelItem(i);
            return;
        }
    }
}
void CComBox::DelItem(int index)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        //std::cout <<"id = " <<_id <<std::endl;
        SendDlgItemMessage (_parent->m_hWnd, _id, CB_DELETESTRING, (WPARAM)index,0);
    }

}
void CComBox::Clear()
{

    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        //std::cout <<"id = " <<_id <<std::endl;
        SendDlgItemMessage (_parent->m_hWnd, _id, CB_RESETCONTENT, 0,0);
    }
}
int  CComBox::TotalCounts()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        return SendDlgItemMessage (_parent->m_hWnd, _data->id, CB_GETCOUNT, 0, 0);
    }
    return -1;

}
int  CComBox::GetCurSel()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        return SendDlgItemMessage (_parent->m_hWnd, _data->id, CB_GETCURSEL, 0, 0);
    }
    return -1;
}
void CComBox::SetText(std::string text)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        _parent->SetItemText(_data->id,text);
    }
}
std::string CComBox::GetText()
{
    return GetSelItemText();
}
void CComBox::EnableTransparent()
{
    HWND wnd_edit = HWND_INVALID;
    HWND wnd_listbox = HWND_INVALID;
    //wParam = (WPARAM)wnd_edit;
    //lParam = (LPARAM)wnd_listbox;

    SendDlgItemMessage(_parent->m_hWnd,_id,CB_GETCHILDREN,(WPARAM)&wnd_edit,(LPARAM)&wnd_listbox);

    if(wnd_edit != HWND_INVALID)
    {
        IncludeWindowExStyle(wnd_edit,WS_EX_TRANSPARENT);
        //InvalidateRect(wnd_edit,NULL,true);
        InvalidateRect(GetDlgItem(_parent->m_hWnd,_id),NULL,true);
    }
}



std::string CComBox::GetItemText(int index)
{
    _strText = "";
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        int textLen = SendDlgItemMessage (_parent->m_hWnd, _data->id, CB_GETLBTEXTLEN, index, 0);

        if(textLen > 0)
        {
            cout<<"textLen"<<textLen<<endl;
            char* textPtr = new char[textLen + 1];
            memset(textPtr, 0, textLen + 1);
            if(textPtr)
            {
                SendDlgItemMessage (_parent->m_hWnd, _data->id, CB_GETLBTEXT, index, (LPARAM)textPtr);
                _strText = textPtr;
                delete []textPtr;

            }

        }

    }
    return _strText;
}
std::string CComBox::GetSelItemText()
{
    _strText = "";
    if(_parent)
    {
        _strText = _parent->GetItemText(_id);
    }
    return _strText;

}
int  CComBox::FindItemByText(std::string text,int start,bool exact)
{
    int ret = 0;
    int msg = exact?CB_FINDSTRINGEXACT:CB_FINDSTRING;
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        ret = SendDlgItemMessage (_parent->m_hWnd, _data->id, msg , start, (LPARAM)text.c_str());
        if(ret == CB_ERR)
        {
            ret = -1;
        }
    }
    return ret;

}

bool CComBox::IsDrapDown()
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        return SendDlgItemMessage (_parent->m_hWnd, _data->id, CB_GETDROPPEDSTATE, 0, 0);
    }
    return false;
}
void CComBox::SetCurSel(int index)
{
    SEND_CMD1(CB_SETCURSEL,index);
}
void CComBox::EditorReadOnly(bool yes)
{

}
void CComBox::ReadOnly(bool yes)
{

    HWND wnd_edit = HWND_INVALID;
    HWND wnd_listbox = HWND_INVALID;

    SendDlgItemMessage(_parent->m_hWnd,_id,CB_GETCHILDREN,(WPARAM)&wnd_edit,(LPARAM)&wnd_listbox);

    if(wnd_edit != HWND_INVALID)
    {
        if(yes)
        {
            IncludeWindowStyle(wnd_edit,ES_READONLY);
        }
        else
            ExcludeWindowStyle(wnd_edit,ES_READONLY);
        UpdateWindow(wnd_edit,TRUE);
    }
}
void EditorReadOnly(bool yes)
{

}
void CComBox::Enable(bool yes)
{

    HWND hwnd = GetDlgItem(_parent->m_hWnd,_id);
    if(hwnd != HWND_INVALID)
    {

        EnableWindow(hwnd,yes);
        UpdateWindow(hwnd,true);

    }

}

bool CComBox::SetLimitLength(int len)
{
    if( (_parent) && (_parent->m_hWnd != HWND_INVALID))
    {
        SendDlgItemMessage (_parent->m_hWnd, _data->id, CB_LIMITTEXT, len, 0);
    }
    return true;
}

