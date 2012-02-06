/*
算法stub程序，方便在无算法模块时候编译通过，做调试用 
*/
#include "QtzParam.h"
QtzParam g_qtzs[NUMBER_OF_QTZ];
std::vector<PolyDef> g_polys;
QtzParam::QtzParam()
{

}

void QtzParam::QtzAddDetect( QtzParam* other )
{

}
void QtzParam::QtzAddDetect( const PolyDef* poly )
{

}
void QtzCollideDetectAll()                     // 全局检测，中心调用
{

}
void QtzCollideDetectOne( QtzParam* qtz )      // 单机检测，塔机调用
{

}
