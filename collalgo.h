#pragma once
#ifndef __COLLALGO_H
#define __COLLALGO_H
#include <vector>

class QtzParam;

#define NUMBER_OF_QTZ 20

// ����
#define CS_ALERT                0x00000001

// �ϵ����
#define CS_SLOW_DOWN            0x00000002

// �����˳�����
#define CS_CARRIER_OUT_LIMIT    0x00000004

// �����˳�����
#define CS_CARRIER_IN_LIMIT     0x00000008

// ����˳ʱ����ת
#define CS_ARM_CW_LIMIT         0x00000010

// ������ʱ����ת
#define CS_ARM_CCW_LIMIT        0x00000020

// ǿ��ɲ������λΪ 1 ʱ���� 4 Ϊ�ɲ����жϣ���Ϊȫ�� 1
#define CS_BRAKE                0x00000040

// �˳�����ڶ�Σ����
#define CS_CARRIER_AREA2_LIMIT      0x00000080

// С�۽���ڶ�Σ����
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
//     CT_NONE = 0,    // �޶���
//     CT_AB_1,        // A�˳�δ����Σ����
//     CT_AB_2,        // A�˳�����Σ��������۲��ཻ
//     CT_AB_3,        // A�˳�����Σ����������ཻ
//     CT_AB_4,        // BС���Ƚ���ڶ�Σ����
//     CT_AB_5,        // A�˳��Ƚ���ڶ�Σ����
//     CT_AA,
// } CollideType;


struct PolyDetectData
{
    enum PolyDetectType
    {
        PDT_NONE = 0,
        PDT_1,           // ������
        PDT_2,           // ������
        PDT_3,           // Χǽ��
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
        QDT_1,          // ��۽����һΣ����
        QDT_2,		    // �˳������һΣ����
        QDT_3,          // �˳�����ڶ�Σ����
        QDT_4,		    // С�۽���ڶ�Σ����
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
    unsigned int number; //�����ı��

    // preset param
    Coord  pos;           //��������
    double height;       //�����߶�
    double long_arm_len; //��۳���
    double short_arm_len;//ƽ��۳���

    // realtime param
    double long_arm_angle;//���ת���ĽǶ�
    double carrier_pos;   //С���ķ���

    unsigned int controled_status;              // �ܿ�״̬

    std::vector<const PolyDef*> m_polys_1;      // ������
    std::vector<const PolyDef*> m_polys_2;      // ������
    std::vector<const PolyDef*> m_polys_3;      // Χǽ

    // �����ͻ״̬
    std::vector<QtzDetectData> critical_status;    // ����֮���ͻ״̬
    PolyDetectData  poly_status;      // �����ͻ״̬

    QtzParam(){}

    void QtzAddDetect( QtzParam* other );
    void QtzAddDetect( const PolyDef* poly, int poly_type );

    // UpdateCriticalStatus �󱻸���
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
extern double g_alert_dist;               // ��������
extern double g_slowdown_dist;            // ���پ���
extern double g_brake_dist;               // ɲ������

#endif
