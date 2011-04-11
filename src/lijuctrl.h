#ifndef LIJUCTRL_H
#define LIJUCTRL_H
#include <vector>
#include <map>
#include "tajidbmgr.h"
typedef struct tag_lijuItem{
    tag_lijuItem(double pos,double weight)
    {
        car_pos = pos;
        max_weight = weight;
    }
    double car_pos;
    double max_weight;
}TLijuItem;
typedef std::vector<TLijuItem> TLijuGrp;

class CLijuCtrl
{
public:
    static CLijuCtrl& Get();
    CLijuCtrl(std::string type="", int armlen=0, int beilv=0);
    int Service(double car_pos, double weight);
    bool Load(std::string type="", std::string armlen="", std::string beilv=0);
    //设置新的倍率，
    int ChangeBeilv(int newBeilv=0);
    double m_percent;
private:
    int m_curBeilvIndex;
    TStringList m_BeilvList;
    std::string m_curBeilv;
    std::string m_arm_len;
    std::string m_tc_type;
    TLijuGrp m_lijuGrp;
    std::string m_tbl_name;
    bool m_load_ok;

    int  m_cur_state;
};

#endif // LIJUCTRL_H
