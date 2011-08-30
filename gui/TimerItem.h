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
#define COMMON_CTRL_STYLE_NUM		3

#define EDIT_HOUR_CLASS		CTRL_EDIT
#define EDIT_HOUR_STYLE		WS_VISIBLE|WS_GROUP|ES_DIGIT_ONLY
#define EDIT_HOUR_X			520
#define EDIT_HOUR_Y			328
#define EDIT_HOUR_W			30
#define EDIT_HOUR_H					36
#define EDIT_HOUR_ID				100
#define EDIT_HOUR_CAPTION		""
#define EDIT_HOUR_ADDDATA		0
#define EDIT_HOUR_EXSTYLE		WS_EX_TRANSPARENT

#define EDIT_MINUTE_CLASS		CTRL_EDIT
#define EDIT_MINUTE_STYLE		WS_VISIBLE|WS_GROUP|ES_DIGIT_ONLY
#define EDIT_MINUTE_X			587
#define EDIT_MINUTE_Y			328
#define EDIT_MINUTE_W			30
#define EDIT_MINUTE_H					36
#define EDIT_MINUTE_ID				101
#define EDIT_MINUTE_CAPTION		""
#define EDIT_MINUTE_ADDDATA		0
#define EDIT_MINUTE_EXSTYLE		WS_EX_TRANSPARENT

#define EDIT_SECOND_CLASS		CTRL_EDIT
#define EDIT_SECOND_STYLE		WS_VISIBLE|WS_GROUP|ES_DIGIT_ONLY
#define EDIT_SECOND_X			648
#define EDIT_SECOND_Y			328
#define EDIT_SECOND_W			30
#define EDIT_SECOND_H					36
#define EDIT_SECOND_ID				102
#define EDIT_SECOND_CAPTION		""
#define EDIT_SECOND_ADDDATA		0
#define EDIT_SECOND_EXSTYLE		WS_EX_TRANSPARENT

#define CALENDAR_CLASS			CTRL_MONTHCALENDAR
#ifdef ENGLISH_VER
#define CALENDAR_STYLE			WS_CHILD|WS_VISIBLE|MCS_NOTIFY|MCS_ENG_S
#else
#define CALENDAR_STYLE			WS_CHILD|WS_VISIBLE|MCS_NOTIFY|MCS_CHN
#endif
#define CALENDAR_X					234
#define CALENDAR_Y					110
#define CALENDAR_W					250
#define CALENDAR_H					240
#define CALENDAR_ID				103
#define CALENDAR_CAPTION		"calendar"
#define CALENDAR_ADDDATA		0
#define CALENDAR_EXSTYLE		WS_EX_TRANSPARENT

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
#define SKIN_CTRL_STYLE_NUM		3

#define	BUTTON_OK_ID					1
#define	BUTTON_OK_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_OK_X						270
#define	BUTTON_OK_Y						410
#define	BUTTON_OK_ITEMRC			{}
#define	BUTTON_OK_BMPINDEX		1
#define	BUTTON_OK_TIP					NULL
#define	BUTTON_OK_ATTACHED		0
#define	BUTTON_OK_DATA				NULL

#define	BUTTON_CANCEL_ID					2
#define	BUTTON_CANCEL_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_CANCEL_X						630
#define	BUTTON_CANCEL_Y						410
#define	BUTTON_CANCEL_ITEMRC			{}
#define	BUTTON_CANCEL_BMPINDEX		2
#define	BUTTON_CANCEL_TIP					NULL
#define	BUTTON_CANCEL_ATTACHED		0
#define	BUTTON_CANCEL_DATA				NULL

#define	BUTTON_EXIT_ID					3
#define	BUTTON_EXIT_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_EXIT_X						738
#define	BUTTON_EXIT_Y						8
#define	BUTTON_EXIT_ITEMRC			{}
#define	BUTTON_EXIT_BMPINDEX		3
#define	BUTTON_EXIT_TIP					NULL
#define	BUTTON_EXIT_ATTACHED		0
#define	BUTTON_EXIT_DATA				NULL

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
#define SKIN_BMP_NUM					4

#define SYSCFG_X						60
#define SYSCFG_Y						65

#define SYSCFG_TEXT_X						60
#define SYSCFG_TEXT_Y						185

#define LINE_X					62
#define LINE_Y					220

#define SYSCFG_TIMESET_X						85
#define SYSCFG_TIMESET_Y						225

#define SKIN_MSG_NUM					3

#define HOUR_X				601
#define HOUR_Y				200

#define MINUTE_X				601
#define MINUTE_Y				200

#define SECOND_X				601
#define SECOND_Y				200

/*
*/
#define RECT_LEFT			524
#define RECT_TOP			122
#define RECT_RIGHT			682
#define RECT_BOTTOM			280

/* new style */
#define EDIT_HOUR   {EDIT_HOUR_X,EDIT_HOUR_Y,EDIT_HOUR_W,EDIT_HOUR_H,""}
#define EDIT_MINUTE   {EDIT_MINUTE_X,EDIT_MINUTE_Y,EDIT_MINUTE_W,EDIT_MINUTE_H,""}
#define EDIT_SECOND  {EDIT_SECOND_X,EDIT_SECOND_Y,EDIT_SECOND_W,EDIT_SECOND_H,""}

#define SKIN_BUTTON_OK        {1,BUTTON_OK_X,       BUTTON_OK_Y}
#define SKIN_BUTTON_CANCEL    {2,BUTTON_CANCEL_X,   BUTTON_CANCEL_Y}
#define SKIN_BUTTON_EXIT      {3,BUTTON_EXIT_X,     BUTTON_EXIT_Y}

