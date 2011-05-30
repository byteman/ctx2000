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
    bool SaveData();
    int Service(double car_pos, double weight);
    bool Load(std::string type="", std::string armlen="", std::string beilv="");
    //设置新的倍率，
    int ChangeBeilv(int newBeilv=0);
    std::string m_curBeilv;
    double m_percent;
    double m_max_weight;
    int  m_cur_state; /*0 正常 1: 90 2 : 95  3 :100*/
private:
    int m_curBeilvIndex;
    TStringList m_BeilvList;

    std::string m_arm_len;
    std::string m_beilv;
    std::string m_tc_type;
    TLijuGrp m_lijuGrp;
    std::string m_tbl_name;
    bool m_load_ok;


};

#endif // LIJUCTRL_H
