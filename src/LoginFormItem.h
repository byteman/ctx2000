// #define COMMON_CTRL_STYLE   SI_TYPE_CONTROL|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
// #define SKIN_BUTTON_STYLE   SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE

/*
CTRLDATA	//²Î¿¼static CTRLDATA Ctrls[]
	const char * class_name		//CTRL_STATIC/CTRL_EDIT
	DWORD dwStyle
	int x
	int y
	int w
	int h
	int id
	const char * caption
	DWORD dwAddData
	DWORD dwExStyle
*/
#define COMMON_CTRL_STYLE_NUM		2

#define EDIT_USERNAME_CLASS			CTRL_EDIT
#define EDIT_USERNAME_STYLE			WS_VISIBLE|WS_GROUP
#define EDIT_USERNAME_X					360
#define EDIT_USERNAME_Y					190
#define EDIT_USERNAME_W					160
#define EDIT_USERNAME_H					30
#define EDIT_USERNAME_ID				100
#define EDIT_USERNAME_CAPTION		"admin"
#define EDIT_USERNAME_ADDDATA		0
#define EDIT_USERNAME_EXSTYLE		WS_EX_TRANSPARENT

#define EDIT_USERNAME   {360,190,160,30,""}
#define EDIT_PASSWORD   {360,250,160,30,""}

#define EDIT_PASSWORD_CLASS		CTRL_EDIT
#define EDIT_PASSWORD_STYLE		WS_VISIBLE|WS_GROUP|ES_PASSWORD
#define EDIT_PASSWORD_X			360
#define EDIT_PASSWORD_Y			250
#define EDIT_PASSWORD_W			160
#define EDIT_PASSWORD_H					30
#define EDIT_PASSWORD_ID				101
#define EDIT_PASSWORD_CAPTION		"admin"
#define EDIT_PASSWORD_ADDDATA		0
#define EDIT_PASSWORD_EXSTYLE		WS_EX_TRANSPARENT

/*
skin_item_s					//²Î¿¼CSkinForm::AddSkinItem(DWORD style , int bmpidx, int left, int top)
	int id						//
	DWORD style				//=style	//SKIN_BUTTON_STYLE
	int x							//=left
	int y							//=top
	RECT item_rc			//
	int bmp_index			//=bmpindex
	char * tip				//
	DWORD attached		//
	void * type_data	//
*/
#define SKIN_CTRL_STYLE_NUM		1

#define	BUTTON_OK_ID					1
#define	BUTTON_OK_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_OK_X						360
#define	BUTTON_OK_Y						300
#define	BUTTON_OK_ITEMRC			{}
#define	BUTTON_OK_BMPINDEX		1
#define	BUTTON_OK_TIP					NULL
#define	BUTTON_OK_ATTACHED		0
#define	BUTTON_OK_DATA				NULL

/*
typedef struct skin_bmp_s		//Add by Achille 2009/11/27 11:15:55
{
  int  x;
  int  y; 
  int  w;
  int  h;
  BITMAP *bmp; 
} skin_bmp_t;  
*/
#define SKIN_BMP_NUM					3

#define USE_TEXT_X						250
#define USE_TEXT_Y						110

#define USR_ICON_X						350
#define USR_ICON_Y						180

#define SEC_ICON_X						350
#define SEC_ICON_Y						240

#define VER_ICON_X                                              500
#define VER_ICON_Y                                              430

#define SKIN_MSG_NUM					2

#define ERROR_ICON_X					480
#define ERROR_ICON_Y					120

#define MSG_ICON_X						480
#define MSG_ICON_Y						120

#define BUTTON_LOGIN    {1,360,300}
#define ICON_USER_POS        USE_TEXT_X,USE_TEXT_Y
#define ICON_USER_ICON_POS   USR_ICON_X,USR_ICON_Y
#define ICON_SEC_POS         SEC_ICON_X,SEC_ICON_Y
#define ICON_VER_POS         VER_ICON_X,VER_ICON_Y
