#ifndef FILTER_H
#define FILTER_H
#include <queue>
#include <vector>
//typedef std::queue<int>  TFilterQueue;
typedef std::vector<int> TFilterQueue;
class CFilter
{
public:
    CFilter(size_t size=8);
    size_t SetSize(size_t size);
    int Filter(int ad);
    void Reset(size_t size);
    int  CalcValue();
private:
    size_t max_size;
    size_t index;
    TFilterQueue m_filter_queue;
    bool m_init_ok;
};

#endif // FILTER_H
