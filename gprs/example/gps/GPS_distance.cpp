
float nStartX;//:�߶���������   
float nStartY;//:�߶��������� 
int   nEndX;    //:   �߶��յ������ 
int   nEndY;    //:   �߶��յ������ 
int   nDotX;    //:   ƽ���ϵ������ 
int   nDotY;    //:   ƽ��������
bool bSwitch =   false; 
int nPointToLineDistance; 
//   ��������㲢����ֱ�߶δ�ֱ��ֱ�ߺ�ֱ�߶εĽ�����ֱ�߶��ڣ�����㵽ֱ�߶εľ������ͨ���� 
//   �㵽ֱ�ߵľ��빫ʽ����. bSwitch =   true; 
//   ��������㲢����ֱ�߶δ�ֱ��ֱ�ߺ�ֱ�߶εĽ��㲻��ֱ�߶��ڣ�����㵽ֱ�߶εľ����Ϊ�㵽 
//   ֱ�߶������˵�ľ����С��һ��. bSwitch =   false; 


//   �㵽ֱ�߶����˵ľ���,   
//   ����㵽�߶����ľ��� 
int   nOffsetX   =   nDotX   -   nStartX; 
int   nOffsetY   =   nDotY   -   nStartY; 
double   dbSquareOfDistance=   pow(abs(nOffsetX),2)+pow(abs(nOffsetY),2); //
double   dbPointToptStartDistance = sqrt(dbSquareOfDistance); 
//   ����㵽�߶��յ�ľ��� 
nOffsetX   =   nDotX   -   nEndX; 
nOffsetY   =   nDotY   -   nEndY; 
dbSquareOfDistance =pow   (abs(   nOffsetX),2) +pow(abs(nOffsetY),2); 
double   dbPointToptEndDistance =   sqrt(dbSquareOfDistance); 

//   ����ֱ�߶εĳ��� 
nOffsetX   =   nStartX   -   nEndX; 
nOffsetY   =   nStartY   -   nEndY; 
dbSquareOfDistance =   pow(abs(nOffsetX),2)+pow(abs(nOffsetY),2); 
double   dbLineLength =   sqrt   (   dbSquareOfDistance   ); 

//   ��������㲢��ֱ�߶δ�ֱ��ֱ�ߺ�ֱ�߶εĽ�����ֱ�߶��ڣ����ֱ�߶ε������˵㹹�ɵ�������Ϊһ���Ƕ۽� 
//   �����Σ�����Ϊһ��ֱ�������Σ�����һ��ֱ��Ϊ�㵽ֱ�߶������˵�����С��������ֱ�������ε�б��Ϊ�㵽 
//   ֱ�߶������˵����ϴ�������Ҫ����������Ҳ���ǵ㵽ֱ�߶������˵�ľ����нϴ�һ����ƽ�����᲻������� 
//   �н�С��һ����ƽ����ֱ�߶εĳ���ƽ���ĺ�. 
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

//   �����㲢��ֱ�߶δ�ֱ��ֱ����ֱ�߶εĽ�����ֱ�߶���. 
if(bSwitch){ 
    //   �㵽ֱ�ߵľ��� 
    if(nEndY==nStartY){ 
        nPointToLineDistance = abs(nDotY-nStartY); 
    } 
    else if(nEndX == nStartX){ 
        nPointToLineDistance =abs(nDotX-nStartX); 
    } 
    else 
    { 
        //   �����㲢��X-Axisƽ�е�ֱ����ֱ�߶�����ֱ���ཻ���㵽�ཻ��ľ�������ཻ�ǵ����� 
        //   ��Ϊ�㵽ֱ�ߵľ��� 
        double   k =   ((double)(nEndY -   nStartY))/((double)(nEndX-nStartX)); 
        //   ˮƽ���� 
        double   dbPointToLineHorizotalDistance = fabs((nDotY- nStartY)/k+nStartX - nDotX); 
        //   �ཻ�� 
        double   dbAngle =atan(k); 
        nPointToLineDistance = (int)(dbPointToLineHorizotalDistance * sin(fabs(dbAngle))); 
    } 
} 

return   nPointToLineDistance;
