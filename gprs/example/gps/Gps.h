//////////////////////////////////////////////////////////////////////

#ifndef __GPS_H
#define __GPS_H
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "../main/dt_gprshead.h"
using namespace std;
#define DEFAULT_GPSD_PORT    "2947"    /* IANA assignment */
#define DEFAULT_RTCM_PORT    "2101"    /* IANA assignment */
#define GETCLOCKSECOND(x) {                \
        x= time(NULL);                \
    }
/*����������ն˷���ͼƬ��Ϣ*/
struct GPS_INIT_DATA{
        unsigned int uiPicture_UP_Longitude;    //ͼƬ���ϽǾ���
        unsigned int uiPicture_UP_Latitude;     //ͼƬ���Ͻ�γ��
        unsigned int uiPicture_DOWN_Longitude;  //ͼƬ���½Ǿ���
        unsigned int uiPicture_DOWN_Latitude;   //ͼƬ���½�γ��

        unsigned short usPicture_UP_X_Pixel;    //ͼƬ���Ͻ�xֵ
        unsigned short usPicture_UP_Y_Pixel ;   //ͼƬ���Ͻ�Y����
        unsigned short usPicture_DOWN_X_Pixel;  //ͼƬ���½�X����
        unsigned short usPicture_DOWN_Y_Pixel;  //ͼƬ���½�Y����
        unsigned short usPicture_X_Pixel;       //ͼƬX�����ش�С
        unsigned short usPicture_Y_Pixel  ;     //ͼƬY�����ش�С

        unsigned short usGPS_TTI;               //���������س�����GPS��Ϣ���
        unsigned short usGPS_GUI_TTI;           //������GUI����GPS��Ϣ���

        char cPicture_UP_Longitude_Symbols;     //ͼƬ���ϽǾ��ȷ���
        char cPicture_UP_Latitude_Symbols;      //ͼƬ���Ͻ�γ�ȷ���
        char cPicture_DOWN_Longitude_Symbols;   //ͼƬ���½Ǿ��ȷ���
        char cPicture_DOWN_Latitude_Symbols;    //ͼƬ���½�γ�Ⱥ���
};      
/*����������ն˷��͹켣��Ϣ*/
struct GPS_Route_Point_data{
        unsigned char   uiGPS_Route_la_sign;    //γ�ȷ���
        unsigned int    uiGPS_Route_Latitude;   //·�������γ��ֵ
        unsigned char   uiGPS_Route_lo_sign;    //���ȷ���
        unsigned int    uiGPS_Route_Longitude;  //·������㾭��ֵ
        unsigned int    uiGPS_Route_altitude;   //�߶�
        unsigned int    uiGPS_Route_X;          //��ͼX����
        unsigned int    uiGPS_Route_Y;          //γ��Y����
};
/*����������ն˷��͹켣������Ϣ*/
struct GPS_Route_data{
        unsigned int    uiGPS_Offset_distance;  //��ǰλ�þ���·�ߵ����ƫ�ƾ���
        int             uiGPS_Offset_cout;      //��ƫ�ƶ��ٴκ��������
        int             uiGPS_Offset_Cout_time; //��ʱ�����һ�α���״̬
        bool            bOn_OFF;                //���ÿ���
};
/*����������ն˷��ͳ�����Ϣ*/
struct GPS_Speed_Point_data{
        GPS_Route_Point_data    start;          //�ö����
        GPS_Route_Point_data    end;            //�ö�ֹ��
        unsigned int    maxspeed;               //�ö�����ٶ�
};
/*����������ն˷��ͳ��ٿ�����Ϣ*/
struct GPS_Speed_Cont_data{
        int     uiGPS_Speed_cout;               //�����ٶ��ٴκ��������
        int     uiGPS_Speed_Cout_time;          //��ʱ�����һ�α���״̬
        int     uiGPS_ALL_Max_Speed;            //ȫ·������ٶ�
        bool    bGPS_Speed_On_OFF;              //���ÿ���
};
/*����������ն˷��͵�����Ϣ*/
struct GPS_Point_data
{
        unsigned char   uiGPS_Point_la_sign;    //γ�ȷ���,'N'��'S'
        unsigned int    uiGPS_Point_Latitude;   //γ��
        unsigned char   uiGPS_Point_lo_sign;    //���ȷ���,'E'��'W'
        unsigned int    uiGPS_Point_Longitude;  //����
        unsigned int    uiGPS_Point_altitude;   //�߶�
        unsigned int    uiGPS_Point_X;          //��ͼX����
        unsigned int    uiGPS_Point_Y;          //γ��Y����
};
/*����������ն˷��͵��������Ϣ*/
struct GPS_Point_Contrl {
        unsigned int    uiGPS_Point_distance;   //��ǰλ�þ���·�ߵ����ƫ�ƾ���
        int             uiGPS_Point_cout;       //��ƫ�ƶ��ٴκ��������
        int             uiGPS_Point_Cout_time;  //��ʱ�����һ�α���״̬
        bool            bGPS_Point_On_OFF;      //���ÿ���
};
/*����������ն˷������ж����Ϣ*/
struct GPS_other_places
{
        unsigned char   uiGPS_Point_la_sign;    //γ�ȷ���,'N'��'S'
        unsigned int    uiGPS_Point_Latitude;   //γ��
        unsigned char   uiGPS_Point_lo_sign;    //���ȷ���,'E'��'W'
        unsigned int    uiGPS_Point_Longitude;  //����
        unsigned int    uiGPS_Point_altitude;   //�߶�
        unsigned int    uiGPS_Point_X;          //��ͼX����
        unsigned int    uiGPS_Point_Y;          //γ��Y����
};


int             GPS_INIT(void);
int             GPS_Control( struct GPS_INIT_DATA GPS_INIT_data1);
int             GPS_CUR_LO_LA_DATA( GpsMapPoint& la_lo);
int             gpsclose();
static void *   getgps(void *arg);
static int      GPS_Data_decomposition(char *cRead_buf);
static void     GPS_Init_gprmc(void);
static int      GPS__INIT_Client(int *sock);
static int      netlib_connectsock(const char *host, const char *service, const char *protocol);
static int      get_time(char time_years[12],char time_hhmmss[12]);
static int      getN_E_S_W(char north_south[12],char est_wth[12]);
static int      GPS_TO_GUI(char north_south[12],char est_wth[12],char weidu_log[12],char jingdu_log[12]);



static double   GPS_F_jingdu(char north_south[12]);
static double   GPS_F_weidu(char north_south[12]);
static int      GPS_Analysis_Data(int *sock);
static int      GPS_Send_Data_GUI( void);
int			    GPS_Send_Data_APP( void);
static void *   GPS_DATA_to_GUI_treatment(void *arg);
static void *   GPS_DATA_to_APP_treatment(void *arg);
//Խ�籨��
int             GPS_Deviated_Route_init(vector<GPS_Route_Point_data> vecPoint_data, struct GPS_Route_data GPS_Offset_data);
int             GPS_Route_Contrl_init(vector<GPS_Route_Point_data> vecPoint_data, struct GPS_Route_data GPS_Offset_data);
static void *   GPS_Route_Contrl(void *arg);
static unsigned int     GPS_Route_Point_range(double dStart_X,double dStart_Y,double dEnd_X,double dEnd_Y,double dCur_P_X,double dCur_P_Y) ;
static unsigned int     GPS_Route_range ();
static int      GPS_Rotue_Info_to_APP( unsigned int  uiDistance);
//���ٱ���
int             GPS_Speed_Limit_init(vector<GPS_Speed_Point_data> vecS_Point,struct GPS_Speed_Cont_data GPS_Speed_Cont);
int             GPS_Speed_Contrl_init(vector<GPS_Speed_Point_data> vecS_Point,struct GPS_Speed_Cont_data GPS_Speed_Cont);
static void *   GPS_Spped_Limit(void *arg);
static  int     GPS_Speed_range ();
static unsigned int    GPS_Line_Speed_range(double dStart_X,double dStart_Y,double dEnd_X,double dEnd_Y,double dCur_P_X,double dCur_P_Y) ;
static int      GPS_Speed_Alarm_to_APP( int  speed);
//���㱨��
int             GPS_Point_init(vector<GPS_Point_data> vec_Point,struct GPS_Point_Contrl GPS_Point_Cont);
int             GPS_Point_Contrl_init(vector<GPS_Point_data> vec_Point,struct GPS_Point_Contrl GPS_Point_Cont);
static void *   GPS_Point_Limit(void *arg);
static  int     GPS_Point_Judge();
static unsigned int    GPS_Point_to_Point_dis(double dPoint_X,double dPoint_Y,double dCur_Point_X,double dCur_Point_Y) ;
static int      GPS_Point_Info_to_APP( int  uiDistance);

//�ж�ֹͣʱ��
int             GPS_Stop_Max_time(unsigned int time);
static void *   GPS_Stop_time(void *arg);
//�ж����ź�ʱ��
int             GPS_No_signal_Max_time(unsigned int time);
static void *   GPS_No_signal_time(void *arg);
//���ж��
int             GPS_Other_place_discharge(vector<GprsUnLoadPoint> vec_GprsUnLoadPoint);
///@brief
///����Χ��
int             GPS_rail_init(vector<GprsRail> vec_GprsRail);
static void *   GPS_rail(void *arg);
static  int     GPS_rail_Judge();

static  int     GPS_rail_Point_convert(double cur_x,double cur_y);
static  int     GPS_rail__Alarm_to_APP( unsigned int  index,unsigned char inorout, unsigned short func,unsigned char armType);
///@brief
///����Χ����ж����ת���ж�
///@parm 0x0001 - �����ж� 0x0002 ж���ж� 0x0004 װ���ж�
int             GPS_rail_Load_or_unload(unsigned short  func);
///@brief
///�������ж�ر���
static  int     GPS_Unload_Alarm_to_APP();

///@brief
///�������װ�ر���
static  int     GPS_Load_Alarm_to_APP();

///@brief
///����״̬�ı�
int     GPSAlarmChange(unsigned int FLAG,bool AlarmIng);


#endif // !defined(__GPS_H)
