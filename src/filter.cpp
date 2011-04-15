#include "filter.h"

CFilter::CFilter(size_t size):
    max_size(size)
{
    Reset(max_size);
}
size_t CFilter::SetSize(size_t size)
{
    max_size=size;
    Reset(max_size);
}
int  CFilter::CalcValue()
{
    int sum = 0;
    for(size_t i = 0; i < m_filter_queue.size(); i++)
    {
        sum+=m_filter_queue.at(i);
    }
    return sum/max_size;

}

int CFilter::Filter(int ad)
{
    if(index >= max_size)
    {
        index     =0;
        m_init_ok = true;
    }
    m_filter_queue.at(index++) = ad;
    if(m_init_ok)
        return CalcValue();
    else
        return ad;
}
void CFilter::Reset(size_t size)
{

     m_filter_queue.clear();
     for( size_t i = 0; i < size; i++ )
     {
         m_filter_queue.push_back(0);
     }
     index     = 0;
     m_init_ok = false;
}
