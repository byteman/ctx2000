#include "SkinForm.h"
#include "dt_icon.h"
#include "dt_cachebmp.h"
static CacheBitmapPool gBmpPool;

CIcon::CIcon(bool cache):
    _hIcon(NULL),
    _num(0),
    _bLoad(false),
    _cacheBmp(NULL),
    _bCache(cache),
    _curShow(0)
{
    Reset();
    _bCache = cache;
    _left = _top = 0;
}
CIcon::CIcon(std::string filename,int count,bool cache):
    _hIcon(NULL),
    _num(0),
    _bLoad(false),
    _cacheBmp(NULL),
    _bCache(false),
    _curShow(0)
{
    Reset();
    _bCache = cache;
    LoadFile(filename,count,_bCache);
    _left = _top = 0;
}
CIcon::~CIcon()
{
    if(_cacheBmp)
    {
        //_cacheBmp->Free();
        gBmpPool.FreeBitmap(_cacheBmp);
        _cacheBmp = NULL;
    }

    _num = 0;
    _bLoad = false ;
    _hIcon = NULL;

}
void CIcon::SetPos(int x, int y)
{
    _left = x;
    _top  = y;
}
void CIcon::Reset()
{
    _rect.left		= 0;
    _rect.top  	= 0;
    _rect.right	= 0;
    _rect.bottom = 0;
    _hdc = -1;
}
BITMAP* CIcon::GetBitmap()
{
    return _hIcon;
}
bool CIcon::LoadFile(std::string filename,int count,bool cache)
{

    string flag = filename.substr(0,1);
    //std::cout << "flag=" << flag << std::endl;
    string realPath = "";
    if( (flag != ".") &&  (flag != "/")) //relative path
    {
        string curLang = "chinese";
        char* p = getenv("dt1000lang");
        if(p != NULL){
            curLang = p;
        }
        realPath = "../resource/" + curLang + "/" + filename;
    }else
        realPath = filename;

    if(_cacheBmp = gBmpPool.getBitmap(realPath,cache))
    {
        _hIcon = _cacheBmp->GetData();
        _num = count;
        if(_num == 0)
            _num = 1;
        _w = _hIcon->bmWidth /  _num;
        _h = _hIcon->bmHeight ;
        _bLoad = true;
    }

    return _bLoad;
}
void CIcon::Show(HDC hdc,RECT &rt,int index)
{
    if(_hIcon && _bLoad && (index >= 1))
    {
        int xo = (index-1)*_w;
        int yo = 0;
        _curShow = index;

        fprintf(stderr,"w=%d,h=%d\n",RECTW(rt), RECTH(rt));
        FillBoxWithBitmapPart(hdc, rt.left, rt.top, RECTW(rt), RECTH(rt), RECTW(rt), RECTH(rt),/*0, 0,*/ _hIcon, xo, yo);
        _rect.left		= rt.left;
        _rect.top    = rt.top;
        _rect.right  = rt.left+ _w;
        _rect.bottom = rt.top + _h;
        _hdc = hdc;
    }
}
/*
该函数的一般就是在整个屏幕上画ICON，但也有可能是传的是某个子窗口的HDC
所以left和top是相对于对应的窗口的起始坐标的
*/
void CIcon::Show(HDC hdc,int left,int top,int index)
{
    if(_hIcon && _bLoad && (index >= 1))
    {
        int xo = (index-1)*_w;
        int yo = 0;
        _curShow = index;
        FillBoxWithBitmapPart(hdc, left, top, _w, _h, 0, 0, _hIcon, xo, yo);
        _rect.left		= left;
        _rect.top  	= top;
        _rect.right	= left+ _w;
        _rect.bottom = top + _h;
        _hdc = hdc;
    }
}
void CIcon::Show(int left,int top,int index)
{
    Show(HDC_SCREEN,left,top,index);
}
void CIcon::Show(HDC hdc)
{
    Show(hdc,_left,_top,1);
}
/*
该函数目前的功能限制
1.只能隐藏最后一次画图的位置
2.只能使用一个HDC
*/
void CIcon::Hide()
{
    if(_hdc == -1)
        return;

    HWND hwnd = WindowFromDC(_hdc);
    if(hwnd != HWND_INVALID)
        InvalidateRect(hwnd, &_rect, true);
}
bool CIcon::IsLoad()
{
    return _bLoad;
}
