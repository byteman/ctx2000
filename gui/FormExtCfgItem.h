#ifndef FORMEXTCFGITEM_H
#define FORMEXTCFGITEM_H

#define IN_START_X  300
#define IN_START_Y  80
#define SPAN        40
#define CBX_IN1   {IN_START_X, IN_START_Y,         160, 33,""}
#define CBX_IN2   {IN_START_X, IN_START_Y+1*SPAN,  160, 33,""}
#define CBX_IN3   {IN_START_X, IN_START_Y+2*SPAN,  160, 33,""}
#define CBX_IN4   {IN_START_X, IN_START_Y+3*SPAN,  160, 33,""}
#define CBX_IN5   {IN_START_X, IN_START_Y+4*SPAN,  160, 33,""}
#define CBX_IN6   {IN_START_X, IN_START_Y+5*SPAN,  160, 33,""}
#define CBX_IN7   {IN_START_X, IN_START_Y+6*SPAN,  160, 33,""}
#define CBX_IN8   {IN_START_X, IN_START_Y+7*SPAN,  160, 33,""}

#define OUT_START_X 600
#define OUT_START_Y  80
#define CBX_OUT1   {OUT_START_X, OUT_START_Y,  160, 33,""}
#define CBX_OUT2   {OUT_START_X, OUT_START_Y+1*SPAN,  160, 33,""}
#define CBX_OUT3   {OUT_START_X, OUT_START_Y+2*SPAN,  160, 33,""}
#define CBX_OUT4   {OUT_START_X, OUT_START_Y+3*SPAN,  160, 33,""}
#define CBX_OUT5   {OUT_START_X, OUT_START_Y+4*SPAN,  160, 33,""}
#define CBX_OUT6   {OUT_START_X, OUT_START_Y+5*SPAN,  160, 33,""}
#define CBX_OUT7   {OUT_START_X, OUT_START_Y+6*SPAN,  160, 33,""}
#define CBX_OUT8   {OUT_START_X, OUT_START_Y+7*SPAN,  160, 33,""}

#define SKIN_BUTTON_SAVEDATA    {1, 450, 410}
#define SKIN_BUTTON_TESTIO      {2, 270, 410}
#define SKIN_BUTTON_RETURN      {3, 630, 410}
#define SKIN_BUTTON_EXIT        {4,738,  8}

#define IN_ICON_X        (IN_START_X   - 55)
#define IN_ICON_NUM_X    (IN_START_X   - 20)
#define OUT_ICON_X       (OUT_START_X  - 55)
#define OUT_ICON_NUM_X   (OUT_START_X  - 20)

#define IN_LBL1 {IN_ICON_X,IN_START_Y}
#define IN_LBL2 {IN_ICON_X,IN_START_Y+1*SPAN}
#define IN_LBL3 {IN_ICON_X,IN_START_Y+2*SPAN}
#define IN_LBL4 {IN_ICON_X,IN_START_Y+3*SPAN}
#define IN_LBL5 {IN_ICON_X,IN_START_Y+4*SPAN}
#define IN_LBL6 {IN_ICON_X,IN_START_Y+5*SPAN}
#define IN_LBL7 {IN_ICON_X,IN_START_Y+6*SPAN}
#define IN_LBL8 {IN_ICON_X,IN_START_Y+7*SPAN}


#define IN_NUM1  {IN_ICON_NUM_X,IN_START_Y}
#define IN_NUM2  {IN_ICON_NUM_X,IN_START_Y+1*SPAN}
#define IN_NUM3  {IN_ICON_NUM_X,IN_START_Y+2*SPAN}
#define IN_NUM4  {IN_ICON_NUM_X,IN_START_Y+3*SPAN}
#define IN_NUM5  {IN_ICON_NUM_X,IN_START_Y+4*SPAN}
#define IN_NUM6  {IN_ICON_NUM_X,IN_START_Y+5*SPAN}
#define IN_NUM7  {IN_ICON_NUM_X,IN_START_Y+6*SPAN}
#define IN_NUM8  {IN_ICON_NUM_X,IN_START_Y+7*SPAN}

#define OUT_LBL1 {OUT_ICON_X,IN_START_Y}
#define OUT_LBL2 {OUT_ICON_X,IN_START_Y+1*SPAN}
#define OUT_LBL3 {OUT_ICON_X,IN_START_Y+2*SPAN}
#define OUT_LBL4 {OUT_ICON_X,IN_START_Y+3*SPAN}
#define OUT_LBL5 {OUT_ICON_X,IN_START_Y+4*SPAN}
#define OUT_LBL6 {OUT_ICON_X,IN_START_Y+5*SPAN}
#define OUT_LBL7 {OUT_ICON_X,IN_START_Y+6*SPAN}
#define OUT_LBL8 {OUT_ICON_X,IN_START_Y+7*SPAN}


#define OUT_NUM1  {OUT_ICON_NUM_X,IN_START_Y}
#define OUT_NUM2  {OUT_ICON_NUM_X,IN_START_Y+1*SPAN}
#define OUT_NUM3  {OUT_ICON_NUM_X,IN_START_Y+2*SPAN}
#define OUT_NUM4  {OUT_ICON_NUM_X,IN_START_Y+3*SPAN}
#define OUT_NUM5  {OUT_ICON_NUM_X,IN_START_Y+4*SPAN}
#define OUT_NUM6  {OUT_ICON_NUM_X,IN_START_Y+5*SPAN}
#define OUT_NUM7  {OUT_ICON_NUM_X,IN_START_Y+6*SPAN}
#define OUT_NUM8  {OUT_ICON_NUM_X,IN_START_Y+7*SPAN}



#define SYSMT_X		60
#define SYSMT_Y		65
#define SYSMT_TEXT_X	60
#define SYSMT_TEXT_Y	185
#define LINE_X		62
#define LINE_Y		220

#define ICON_SYS_ICON   {SYSMT_X,       SYSMT_Y}
#define ICON_SYS_TEXT   {SYSMT_TEXT_X,  SYSMT_TEXT_Y}
#define ICON_SYS_LIN    {LINE_X,        LINE_Y}
#define PIOSETXT_POS    {85,225}
#endif // FORMEXTCFGITEM_H
