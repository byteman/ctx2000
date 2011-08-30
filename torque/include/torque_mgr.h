#ifndef TORQUE_MGR_H
#define TORQUE_MGR_H
#include <string>
#include "torque_db.h"

struct TorQueItem{
    TorQueItem(double pos, double w)
    {
        carPos = pos;
        weight = w;
    }
    double carPos;
    double weight;
};
typedef enum{
    TQ_NORMAL=0, // < 90%
    TQ_ALTER,    //90% <= x < 95%
    TQ_WARN,     //95% <= x < 100%
    TQ_ALARM,    //100% < x < 105%
    TQ_FATAL,    //>=105%
}ETorQueState;
typedef std::vector<TorQueItem> TorQueItemSet;
class CTorQueMgr
{
public:

    CTorQueMgr();
    static CTorQueMgr& get();
    bool   saveCfg(std::string type="",std::string armlen="", std::string rate="");
    /*

    */
    bool   loadCfg(std::string type="", std::string armlen="", std::string rate="");
    bool   ReloadCfg(std::string type="", std::string armlen="", std::string rate="");
    ETorQueState getState(double carPos, double weight);

    int    changeRate(int newRate=0);
public:
    TStrList	 m_rate_list; //当前塔机类型和长度下的倍率列表
    std::string  m_rate; //当前的倍率
    int          m_rate_int;
    std::string  m_tcType; //当前的塔机类型
    std::string  m_armlen; //当前塔机长度
    ETorQueState m_state;
    double       m_percent;      //力距百分比 当前吊钩重量/额定重量
    double       m_rated_weight; //当前位置的额定重量,该值会根据小车的当前位置而刷新
private:
    void          sortByInc(TorQueItemSet& rst);
    CTorQueDB     m_db;
    size_t        m_rate_index; //当前倍率在倍率列表中的序号
    bool          m_load_ok;
    bool          m_open;
    bool          m_sorted;
    TorQueSet     m_torque_list;
    TorQueItemSet m_torque_item_list;
    TStrList      m_tmp_rate_list;

};

#endif // TORQUE_MGR_H
