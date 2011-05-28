#include "dirstatus.h"

CDirStatus::CDirStatus(std::string type):
    m_type(type)
{
    if(type == "up")
    {

    }
    m_icon.LoadFile("ctx2000/"+type+".jpg",3);
}
bool CDirStatus::SetStatus(HDC hdc,EDevStatus status)
{
    m_icon.Show(hdc,m_pos.left,m_pos.top,status+1);
}
CDirStatusMgr::CDirStatusMgr(int left, int top)
{
      int span = 0;
      m_dirs[0] = new CDirStatus("up");
      m_dirs[1] = new CDirStatus("down");
      m_dirs[2] = new CDirStatus("left");
      m_dirs[3] = new CDirStatus("right");

      int up_h  = m_dirs[0]->m_icon.GetBitmap()->bmHeight;
      int up_w  = m_dirs[0]->m_icon.GetBitmap()->bmWidth/3;

      int left_h  = m_dirs[2]->m_icon.GetBitmap()->bmHeight;
      int left_w  = m_dirs[2]->m_icon.GetBitmap()->bmWidth/3;

      m_upRect.top    = top+span;
      m_upRect.left   = left+left_w;

      fprintf(stderr,"top=%d,left=%d\n",m_upRect.top,m_upRect.left );


      m_leftRect.left = left+span;
      m_leftRect.top  = top + up_h;

      m_downRect.top  = top + up_h+left_h-span;
      m_downRect.left = left+left_w;

      m_rightRect.left = left+left_w+up_w-span;
      m_rightRect.top  = top+up_h;

      m_centerRect.left   = m_upRect.left;
      m_centerRect.top    = m_leftRect.top;
      m_centerRect.right  = up_w;
      m_centerRect.bottom = left_h;

      m_dirs[0]->SetRect(m_upRect);
      m_dirs[1]->SetRect(m_downRect);
      m_dirs[2]->SetRect(m_leftRect);
      m_dirs[3]->SetRect(m_rightRect);

      SetRect(&m_areaRect,left,top,2*left_w+span+left+up_w,2*up_h+span+top+left_h);
     //m_dirs[1] = new CDirStatus("down", m_downRect);
    //m_dirs[2] = new CDirStatus("left", m_leftRect);
    //m_dirs[3] = new CDirStatus("right",m_rightRect);

}
bool CDirStatusMgr::Show(HDC hdc, std::string type, EDevStatus status)
{
    int index = 0;
    if(type == "up")
    {
        index =0;
    }else if(type == "down") index = 1;
    else if (type == "left") index = 2;
    else if(type == "right") index = 3;

    m_dirs[index]->SetStatus(hdc,status);
}
bool CDirStatusMgr::Show(HDC hdc, EDevStatus up,EDevStatus down,EDevStatus left,EDevStatus right)
{
    Rectangle(hdc,m_areaRect.left,m_areaRect.top,m_areaRect.right,m_areaRect.bottom);
    m_dirs[0]->SetStatus(hdc,up);
    m_dirs[1]->SetStatus(hdc,down);
    m_dirs[2]->SetStatus(hdc,left);
    m_dirs[3]->SetStatus(hdc,right);

    FillBox(hdc,m_centerRect.left,m_centerRect.top,m_centerRect.right,m_centerRect.bottom);

}
