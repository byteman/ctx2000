
float nStartX;//:线段起点横坐标   
float nStartY;//:线段起点横坐标 
int   nEndX;    //:   线段终点横坐标 
int   nEndY;    //:   线段终点横坐标 
int   nDotX;    //:   平面上点横坐标 
int   nDotY;    //:   平面点横坐标
bool bSwitch =   false; 
int nPointToLineDistance; 
//   如果经过点并且与直线段垂直的直线和直线段的交点在直线段内，计算点到直线段的距离就用通常的 
//   点到直线的距离公式计算. bSwitch =   true; 
//   如果经过点并且与直线段垂直的直线和直线段的交点不在直线段内，计算点到直线段的距离就为点到 
//   直线段两个端点的距离较小的一个. bSwitch =   false; 


//   点到直线段两端的距离,   
//   计算点到线段起点的距离 
int   nOffsetX   =   nDotX   -   nStartX; 
int   nOffsetY   =   nDotY   -   nStartY; 
double   dbSquareOfDistance=   pow(abs(nOffsetX),2)+pow(abs(nOffsetY),2); //
double   dbPointToptStartDistance = sqrt(dbSquareOfDistance); 
//   计算点到线段终点的距离 
nOffsetX   =   nDotX   -   nEndX; 
nOffsetY   =   nDotY   -   nEndY; 
dbSquareOfDistance =pow   (abs(   nOffsetX),2) +pow(abs(nOffsetY),2); 
double   dbPointToptEndDistance =   sqrt(dbSquareOfDistance); 

//   计算直线段的长度 
nOffsetX   =   nStartX   -   nEndX; 
nOffsetY   =   nStartY   -   nEndY; 
dbSquareOfDistance =   pow(abs(nOffsetX),2)+pow(abs(nOffsetY),2); 
double   dbLineLength =   sqrt   (   dbSquareOfDistance   ); 

//   如果经过点并与直线段垂直的直线和直线段的交点在直线段内，点和直线段的两个端点构成的三角形为一个非钝角 
//   三角形，极限为一个直角三角形，其中一条直边为点到直线段两个端点距离较小的那条，直角三角形的斜边为点到 
//   直线段两个端点距离较大那条，要符合条件，也就是点到直线段两个端点的距离中较大一个的平方不会不会大于其 
//   中较小的一条的平方与直线段的长度平方的和. 
if   (dbPointToptStartDistance   >   dbPointToptEndDistance   ) { 
    if(pow(dbPointToptStartDistance,2)>pow(dbPointToptEndDistance,2)+pow(dbLineLength,2)){ 
        bSwitch =   false; 
        nPointToLineDistance =   (int)   dbPointToptEndDistance; 
    } 
    else 
        bSwitch =   true; 
} 
else{ 
    if(pow(dbPointToptEndDistance,2)>pow(dbPointToptStartDistance,2)+pow(dbLineLength,2)) 
    { 
        bSwitch =   false; 
        nPointToLineDistance =   (int)   dbPointToptStartDistance; 
    } 
    else 
    bSwitch =   true; 
} 

//   经过点并与直线段垂直的直线与直线段的交点在直线段上. 
if(bSwitch){ 
    //   点到直线的距离 
    if(nEndY==nStartY){ 
        nPointToLineDistance = abs(nDotY-nStartY); 
    } 
    else if(nEndX == nStartX){ 
        nPointToLineDistance =abs(nDotX-nStartX); 
    } 
    else 
    { 
        //   经过点并与X-Axis平行的直线与直线段所在直线相交，点到相交点的距离乘上相交角的正弦 
        //   即为点到直线的距离 
        double   k =   ((double)(nEndY -   nStartY))/((double)(nEndX-nStartX)); 
        //   水平距离 
        double   dbPointToLineHorizotalDistance = fabs((nDotY- nStartY)/k+nStartX - nDotX); 
        //   相交角 
        double   dbAngle =atan(k); 
        nPointToLineDistance = (int)(dbPointToLineHorizotalDistance * sin(fabs(dbAngle))); 
    } 
} 

return   nPointToLineDistance;
