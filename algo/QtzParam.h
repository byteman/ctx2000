#pragma once

#include <vector>
#include <memory.h>
#include <math.h>

class QtzParam;

#define NUMBER_OF_QTZ 20

// 最终继电器输出
struct ControledStatus
{
	/*1#——右回转限制
	2#——回转制动
	3#——左回转限制	
	4#——小车向外运行高速限制
	5#——小车向外运行停车
	6#——小车向内运行停车
	7#——小车向内运行高速限制
	8#——吊钩起升限制
	9 #——左回转高速限制
	10#——右回转高速限制
	11#——左回转限制反顶继电器
	12#——右回转限制反顶继电器*/
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

// A塔 到 B塔 交互情况，A塔是 this。作为最终继电器输出的依据
// A塔有多个 QtzDetectData，对应周围要检测的塔机
struct QtzDetectData
{
	/*enum QtzDetectType
	{
	QDT_NONE = 0,
	QDT_1,          // 大臂进入第一危险区
	QDT_2,		    // 运车进入第一危险区
	QDT_3,          // 运车进入第二危险区
	QDT_4,		    // 小臂进入第二危险区
	QDT_5		    // 大臂进入第二危险区，QDT_3 优先于 QDT_5
	};*/
	/*
	struct
	{
	int status; // 0: 无 1: 第一危险区 2: 第二危险区
	unsigned long long frame_number;
	} long_arm, short_arm, carrier; // 算法生成，上层只读
	*/

	QtzParam* qtz_other;

	double carrer_larm_dist;    // 本塔到对方的车-大臂距。算法生成，上层只读。仅当高于 qtz_other 时有效(A塔高于B塔10米)
	double carrer_sarm_dist;	// 本塔到对方的车-短臂距。算法生成，上层只读。仅当高于 qtz_other 时有效(A塔高于B塔10米)
	bool cw, bin, cw_for_other;     // 冲突后本塔禁止方向（顺逆时针）、冲突后本车禁止方向（内外）、冲突后对方塔禁止方向（顺逆时针）

#pragma message("todo: 新功能未完成！！！！！！！！！！！！！！！！！！！！！！！！！！！")
	double arm_center_dist;		// 本塔到对方的塔心距。算法生成。吊臂到对方塔身距离
	bool arm_center_cw;
	//double center_center_dist;	// 算法生成。双方塔身距离

	double arm_arm_dist;		// 算法生成，上层只读。仅当同高时有效（高度差在10米以内）
	bool cw_arm_arm, cw_arm_arm_for_other;

	double carrer_carrer_dist;  // 算法生成，上层只读。控制小车内外限，无论AB高低都使能
	bool carrer_carrer_bin;

    double carrer_center_dist; // 算法生成，上层只读。控制小车内外限
    bool carrer_center_bin;

	int height_compare;         // 算法生成，上层只读。本塔跟另一塔高度比较。0：等高 1:高 -1:矮

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

	// 基本参数
	Coord m_coord;
	double m_height;
	double m_long_arm_len;             // 如果是动臂，使用 CalcMovableArmLength() 计算得到的值来填充
	double m_short_arm_len;
	double m_sarm_cc_len;              // 动臂支点离塔机轴心距离。cc: center to center

	double m_slowdown_dist;            // 回转高速限制
	double m_stop_dist;                // 回转限制
	double m_brake_dist;               // 回转制动,优先判断顺序：m_brake_dist、m_stop_dist、m_slowdown_dist

	double m_carrier_slowdown_dist;    // 小车高速限制距离
	double m_carrier_brake_dist;       // 小车停车距离,优先判断顺序：m_brake_dist、m_stop_dist、m_slowdown_dist

	// 实时采集参数
	double m_long_arm_angle;
	double m_carrier_pos;
	double m_sarm_angle;                      // 动臂仰角,动臂最小15°。平臂为0

	ControledStatus m_controled_status;           // 受控状态，上层读取

	//    std::vector<const PolyDef*> m_polys_1;      // 区域
	//    std::vector<const PolyDef*> m_polys_3;      // 围墙

	std::vector<const PolyDef*> m_polys;

	// 各类冲突状态
	std::vector<QtzDetectData> m_critical_status;     // 塔机之间冲突状态,UpdateCriticalStatus 后更新.
	PolyDetectData  m_poly_status;                    // 区域冲突状态

	QtzParam();

	void QtzAddDetect( QtzParam* other );
	void QtzAddDetect( const PolyDef* poly );

	double CalcSwingArmLength( double arm_length_under_angle_15 ); // 通过 15 度映射，计算实际长度
	bool CalcCW( const Coord& pt ) const;

	// UpdateCoordCache 后更新
	struct
	{
		Coord rt_pivot_coord;          // 平臂: = m_coord              动臂: 动臂支点的坐标

        // 动臂高度用 m_height + m_long_arm_len 代替 2012-06-18
		//double rt_long_arm_height;     // 平臂: = m_height             动臂: = m_height + m_long_arm_len * sin( m_sarm_angle )
		//double rt_short_arm_height;     // 平臂: = m_height             动臂: = m_height + m_long_arm_len * sin( m_sarm_angle )

		double rt_carrier_pos;
		Coord rt_carrier_coord;        // 平臂: = 小车投影坐标         动臂: 算上 runtime_pivot_coord 的小车投影坐标

		double rt_long_arm_len;
		Coord rt_long_arm_coord;       // 平臂: = 长臂外端投影坐标     动臂: 算上 runtime_pivot_coord 的长臂外端投影坐标 

		double rt_short_arm_len;
		Coord rt_short_arm_coord;      // 平臂动臂始终一致
	} cached_data;

private:
	void UpdateCoordCache();                // 顺序1
	void UpdateCriticalStatus();            // 顺序2

	void DoQtzAddDetect( QtzParam* other );
	friend void QtzCollideDetect( std::vector<QtzParam*>& qtzs );
};


extern QtzParam g_qtzs[NUMBER_OF_QTZ];
extern std::vector<PolyDef> g_polys;

