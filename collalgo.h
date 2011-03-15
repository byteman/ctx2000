#pragma once
#ifndef __COLLALGO_H
#define __COLLALGO_H
#include <vector>

class QtzParam;

#define NUMBER_OF_QTZ 20

// 报警
#define CS_ALERT                0x00000001

// 断电减速
#define CS_SLOW_DOWN            0x00000002

// 限制运车向外
#define CS_CARRIER_OUT_LIMIT    0x00000004

// 限制运车向里
#define CS_CARRIER_IN_LIMIT     0x00000008

// 限制顺时针旋转
#define CS_ARM_CW_LIMIT         0x00000010

// 限制逆时针旋转
#define CS_ARM_CCW_LIMIT        0x00000020

// 强制刹车。此位为 1 时上面 4 为可不做判断，认为全是 1
#define CS_BRAKE                0x00000040

// 运车禁入第二危险区
#define CS_CARRIER_AREA2_LIMIT      0x00000080

// 小臂禁入第二危险区
#define CS_SHORT_ARM_AREA2_LIMIT    0x00000100

struct Coord
{
    double x;
    double y;
    Coord() : x(0), y(0) { };
    Coord( const double &_x, const double &_y ) : x(_x), y(_y) { };
};


typedef std::vector<Coord> PolyDef;

// typedef enum
// {
//     CT_NONE = 0,    // 无定义
//     CT_AB_1,        // A运车未进入危险区
//     CT_AB_2,        // A运车进入危险区，大臂不相交
//     CT_AB_3,        // A运车进入危险区，大臂相交
//     CT_AB_4,        // B小臂先进入第二危险区
//     CT_AB_5,        // A运车先进入第二危险区
//     CT_AA,
// } CollideType;


struct PolyDetectData
{
    enum PolyDetectType
    {
        PDT_NONE = 0,
        PDT_1,           // 慢行区
        PDT_2,           // 禁行区
        PDT_3,           // 围墙外
    };
    PolyDetectType type;
    PolyDetectData() : type( PDT_NONE ) { }
    const char* ToString();
};

struct QtzDetectData
{
    enum QtzDetectType
    {
        QDT_NONE = 0,
        QDT_1,          // 大臂进入第一危险区
        QDT_2,		    // 运车进入第一危险区
        QDT_3,          // 运车进入第二危险区
        QDT_4,		    // 小臂进入第二危险区
    };
    QtzParam* qtz_other;
    QtzDetectType type;
    unsigned long long ct_frame_number;

    QtzDetectData( QtzParam* other ) : qtz_other(other), type(QDT_NONE), ct_frame_number(0) { }
    const char* ToString();
};


class QtzParam
{
public:
    unsigned int number; //塔机的编号

    // preset param
    Coord  pos;           //塔机坐标
    double height;       //塔机高度
    double long_arm_len; //大臂长度
    double short_arm_len;//平衡臂长度

    // realtime param
    double long_arm_angle;//大臂转动的角度
    double carrier_pos;   //小车的幅度

    unsigned int controled_status;              // 受控状态

    std::vector<const PolyDef*> m_polys_1;      // 慢行区
    std::vector<const PolyDef*> m_polys_2;      // 禁行区
    std::vector<const PolyDef*> m_polys_3;      // 围墙

    // 各类冲突状态
    std::vector<QtzDetectData> critical_status;    // 塔机之间冲突状态
    PolyDetectData  poly_status;      // 区域冲突状态

    QtzParam(){}

    void QtzAddDetect( QtzParam* other );
    void QtzAddDetect( const PolyDef* poly, int poly_type );

    // UpdateCriticalStatus 后被更新
    Coord long_arm_coord, short_arm_coord, carrier_coord;

private:
    void DoQtzAddDetect( QtzParam* other );
    void UpdateCriticalStatus();
    friend void QtzCollideDetect();
};

void QtzCollideDetect();

extern QtzParam g_qtzs[NUMBER_OF_QTZ];
extern std::vector<PolyDef> g_polys_1;
extern std::vector<PolyDef> g_polys_2;
extern std::vector<PolyDef> g_polys_3;
extern double g_alert_dist;               // 报警距离
extern double g_slowdown_dist;            // 减速距离
extern double g_brake_dist;               // 刹车距离

#endif
