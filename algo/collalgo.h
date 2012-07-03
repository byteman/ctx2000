#pragma once

#include "QtzParam.h"


void QtzCollideDetectAll();                     // 全局检测，中心调用
void QtzCollideDetectOne( QtzParam* qtz );      // 单机检测，塔机调用。虽为One，实际上是全部加入本 qtz 检测的塔机



