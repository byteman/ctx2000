#pragma once

#include <vector>
#include <memory.h>
#include <math.h>

class QtzParam;

#define NUMBER_OF_QTZ 20

// ���ռ̵������
struct ControledStatus
{
	/*1#�����һ�ת����
	2#������ת�ƶ�
	3#�������ת����	
	4#����С���������и�������
	5#����С����������ͣ��
	6#����С����������ͣ��
	7#����С���������и�������
	8#����������������
	9 #�������ת��������
	10#�����һ�ת��������
	11#�������ת���Ʒ����̵���
	12#�����һ�ת���Ʒ����̵���*/
	union 
	{
		struct
		{
			bool b1;
			bool b2;
			bool b3;
			bool b4;
			bool b5;
			bool b6;
			bool b7;
			bool b8;
			bool b9;
			bool b10;
			bool b11;
			bool b12;
		};
		bool b[12];
	};
};


struct Coord
{
	double x;
	double y;
	Coord() : x(0), y(0) { };
	Coord( const double& _x, const double& _y ) : x(_x), y(_y) { };
	double Dist( const Coord& other ) 
	{
		double dx = x - other.x;
		double dy = y - other.y;
		return sqrt( dx * dx + dy * dy );
	}
};


struct PolyDef : std::vector<Coord>
{
	double height;
	bool is_wall;
	PolyDef() : height(0), is_wall(false) { };
};

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
	//     bool limited_area;
	//     bool forbidden_area;
	//     bool fence;
	double limited_area_dist, limited_area_r;
	bool limited_area_cw, limited_area_bin;
	double forbidden_area_dist, forbidden_area_r;
	bool forbidden_area_cw;
	double fence_dist, fence_r;
	bool fence_cw;
	const char* ToString();
};

// A�� �� B�� ���������A���� this����Ϊ���ռ̵������������
// A���ж�� QtzDetectData����Ӧ��ΧҪ��������
struct QtzDetectData
{
	/*enum QtzDetectType
	{
	QDT_NONE = 0,
	QDT_1,          // ��۽����һΣ����
	QDT_2,		    // �˳������һΣ����
	QDT_3,          // �˳�����ڶ�Σ����
	QDT_4,		    // С�۽���ڶ�Σ����
	QDT_5		    // ��۽���ڶ�Σ������QDT_3 ������ QDT_5
	};*/
	/*
	struct
	{
	int status; // 0: �� 1: ��һΣ���� 2: �ڶ�Σ����
	unsigned long long frame_number;
	} long_arm, short_arm, carrier; // �㷨���ɣ��ϲ�ֻ��
	*/

	QtzParam* qtz_other;

	double carrer_larm_dist;    // �������Է��ĳ�-��۾ࡣ�㷨���ɣ��ϲ�ֻ������������ qtz_other ʱ��Ч(A������B��10��)
	double carrer_sarm_dist;	// �������Է��ĳ�-�̱۾ࡣ�㷨���ɣ��ϲ�ֻ������������ qtz_other ʱ��Ч(A������B��10��)
	bool cw, bin, cw_for_other;     // ��ͻ������ֹ����˳��ʱ�룩����ͻ�󱾳���ֹ�������⣩����ͻ��Է�����ֹ����˳��ʱ�룩

#pragma message("todo: �¹���δ��ɣ�����������������������������������������������������")
	double arm_center_dist;		// �������Է������ľࡣ�㷨���ɡ����۵��Է��������
	bool arm_center_cw;
	//double center_center_dist;	// �㷨���ɡ�˫���������

	double arm_arm_dist;		// �㷨���ɣ��ϲ�ֻ��������ͬ��ʱ��Ч���߶Ȳ���10�����ڣ�
	bool cw_arm_arm, cw_arm_arm_for_other;

	double carrer_carrer_dist;  // �㷨���ɣ��ϲ�ֻ��������С�������ޣ�����AB�ߵͶ�ʹ��
	bool carrer_carrer_bin;

    double carrer_center_dist; // �㷨���ɣ��ϲ�ֻ��������С��������
    bool carrer_center_bin;

	int height_compare;         // �㷨���ɣ��ϲ�ֻ������������һ���߶ȱȽϡ�0���ȸ� 1:�� -1:��

	QtzDetectData( QtzParam* other )
	{
		memset( this, 0, sizeof(*this) );
		qtz_other = other;
	}
	//const char* ToString();
};


class QtzParam
{
public:
	unsigned int m_number;

	bool m_online;

	// ��������
	Coord m_coord;
	double m_height;
	double m_long_arm_len;             // ����Ƕ��ۣ�ʹ�� CalcMovableArmLength() ����õ���ֵ�����
	double m_short_arm_len;
	double m_sarm_cc_len;              // ����֧�����������ľ��롣cc: center to center

	double m_slowdown_dist;            // ��ת��������
	double m_stop_dist;                // ��ת����
	double m_brake_dist;               // ��ת�ƶ�,�����ж�˳��m_brake_dist��m_stop_dist��m_slowdown_dist

	double m_carrier_slowdown_dist;    // С���������ƾ���
	double m_carrier_brake_dist;       // С��ͣ������,�����ж�˳��m_brake_dist��m_stop_dist��m_slowdown_dist

	// ʵʱ�ɼ�����
	double m_long_arm_angle;
	double m_carrier_pos;
	double m_sarm_angle;                      // ��������,������С15�㡣ƽ��Ϊ0

	ControledStatus m_controled_status;           // �ܿ�״̬���ϲ��ȡ

	//    std::vector<const PolyDef*> m_polys_1;      // ����
	//    std::vector<const PolyDef*> m_polys_3;      // Χǽ

	std::vector<const PolyDef*> m_polys;

	// �����ͻ״̬
	std::vector<QtzDetectData> m_critical_status;     // ����֮���ͻ״̬,UpdateCriticalStatus �����.
	PolyDetectData  m_poly_status;                    // �����ͻ״̬

	QtzParam();

	void QtzAddDetect( QtzParam* other );
	void QtzAddDetect( const PolyDef* poly );

	double CalcSwingArmLength( double arm_length_under_angle_15 ); // ͨ�� 15 ��ӳ�䣬����ʵ�ʳ���
	bool CalcCW( const Coord& pt ) const;

	// UpdateCoordCache �����
	struct
	{
		Coord rt_pivot_coord;          // ƽ��: = m_coord              ����: ����֧�������

        // ���۸߶��� m_height + m_long_arm_len ���� 2012-06-18
		//double rt_long_arm_height;     // ƽ��: = m_height             ����: = m_height + m_long_arm_len * sin( m_sarm_angle )
		//double rt_short_arm_height;     // ƽ��: = m_height             ����: = m_height + m_long_arm_len * sin( m_sarm_angle )

		double rt_carrier_pos;
		Coord rt_carrier_coord;        // ƽ��: = С��ͶӰ����         ����: ���� runtime_pivot_coord ��С��ͶӰ����

		double rt_long_arm_len;
		Coord rt_long_arm_coord;       // ƽ��: = �������ͶӰ����     ����: ���� runtime_pivot_coord �ĳ������ͶӰ���� 

		double rt_short_arm_len;
		Coord rt_short_arm_coord;      // ƽ�۶���ʼ��һ��
	} cached_data;

private:
	void UpdateCoordCache();                // ˳��1
	void UpdateCriticalStatus();            // ˳��2

	void DoQtzAddDetect( QtzParam* other );
	friend void QtzCollideDetect( std::vector<QtzParam*>& qtzs );
};


extern QtzParam g_qtzs[NUMBER_OF_QTZ];
extern std::vector<PolyDef> g_polys;

