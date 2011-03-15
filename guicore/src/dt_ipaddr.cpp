#include "dt_ipaddr.h"
#include <assert.h>
CIpAddr::CIpAddr(COMM_CTRL_DESC* desc,CSkinForm* parent,string typeName)
{
    CTRLDATA* data = new CTRLDATA();
    char* tmp = new char[strlen(desc->caption) + 1];
    memset(tmp,0,strlen(desc->caption) + 1);

    strcpy(tmp,desc->caption);
    data->caption = tmp;

    tmp = new char[typeName.size() + 1];
    memset(tmp,0,strlen(desc->caption) + 1);
    strcpy(tmp,typeName.c_str());

    data->class_name=tmp;
    data->dwAddData = 0;
    data->dwExStyle = DEF_IPADDR_EXSTYLE;
    data->dwStyle   = DEF_IPADDR_STYLE;
    data->id        = -1; //auto alloc id
    data->w         = desc->w;
    data->h         = desc->h;
    data->x         = desc->x;
    data->y         = desc->y;

    SetParam(data,parent,typeName);
    _alloc_desc = true;
}
CIpAddr::~CIpAddr()
{

}
bool CIpAddr::setAddress(std::string strIp)
{
    SendDlgItemMessage(_parent->m_hWnd,_id,IPM_SET_IPADDRESS,0,(LPARAM)strIp.c_str());
    return true;
}
std::string CIpAddr::getAddress()
{
    char buf[32] = {0,};
    assert(_parent);

    if(_parent->m_hWnd != HWND_INVALID)
    {

        SendDlgItemMessage(_parent->m_hWnd,_id,IPM_GET_IPADDRESS,0,(LPARAM)buf);
        //fprintf(stderr,"ipaddr=%s\n",buf);
        return string(buf);
    }
    return "127.0.0.1";

}
