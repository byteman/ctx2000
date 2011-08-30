#ifndef LIJUCONF_H
#define LIJUCONF_H

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
#define SKIN_CTRL_STYLE_NUM		5

#define	BUTTON_ADD_ID					1
#define	BUTTON_ADD_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_ADD_X						270
#define	BUTTON_ADD_Y						410
#define	BUTTON_ADD_ITEMRC			{}
#define	BUTTON_ADD_BMPINDEX		1
#define	BUTTON_ADD_TIP					NULL
#define	BUTTON_ADD_ATTACHED		0
#define	BUTTON_ADD_DATA				NULL

#define	BUTTON_DEL_ID					2
#define	BUTTON_DEL_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_DEL_X						390
#define	BUTTON_DEL_Y						410
#define	BUTTON_DEL_ITEMRC			{}
#define	BUTTON_DEL_BMPINDEX		2
#define	BUTTON_DEL_TIP					NULL
#define	BUTTON_DEL_ATTACHED		0
#define	BUTTON_DEL_DATA				NULL

#define	BUTTON_EDIT_ID					3
#define	BUTTON_EDIT_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_EDIT_X						510
#define	BUTTON_EDIT_Y						410
#define	BUTTON_EDIT_ITEMRC			{}
#define	BUTTON_EDIT_BMPINDEX		3
#define	BUTTON_EDIT_TIP					NULL
#define	BUTTON_EDIT_ATTACHED		0
#define	BUTTON_EDIT_DATA				NULL

#define	BUTTON_QUIT_ID					4
#define	BUTTON_QUIT_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_QUIT_X						630
#define	BUTTON_QUIT_Y						410
#define	BUTTON_QUIT_ITEMRC			{}
#define	BUTTON_QUIT_BMPINDEX		4
#define	BUTTON_QUIT_TIP					NULL
#define	BUTTON_QUIT_ATTACHED		0
#define	BUTTON_QUIT_DATA				NULL

#define	BUTTON_EXIT_ID					5
#define	BUTTON_EXIT_STYLE				SI_TYPE_CMDBUTTON|SI_TEST_SHAPE_RECT|SI_STATUS_VISIBLE
#define	BUTTON_EXIT_X						738
#define	BUTTON_EXIT_Y						8
#define	BUTTON_EXIT_ITEMRC			{}
#define	BUTTON_EXIT_BMPINDEX		5
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

#define SYSMT_X						60
#define SYSMT_Y						65

#define SYSMT_TEXT_X						60
#define SYSMT_TEXT_Y						185

#define LINE_X		62
#define LINE_Y					220

#define SYSMT_ADMIN_X						85
#define SYSMT_ADMIN_Y						225
/* new style */
#define LISTVIEW_USER       {LIST_USER_X, LIST_USER_Y, LIST_USER_W, LIST_USER_H,"0"}
#define SKIN_BUTTON_ADD        {1,BUTTON_ADD_X,BUTTON_ADD_Y}
#define SKIN_BUTTON_DEL        {2,BUTTON_DEL_X,BUTTON_DEL_Y}
#define SKIN_BUTTON_EDIT        {3,BUTTON_EDIT_X,BUTTON_EDIT_Y}
#define SKIN_BUTTON_QUIT       {4,BUTTON_QUIT_X,BUTTON_QUIT_Y}
#define SKIN_BUTTON_EXIT       {5,BUTTON_EXIT_X,BUTTON_EXIT_Y}
#define SKIN_BUTTON_SAVE       {6,BUTTON_ADD_X-100,BUTTON_ADD_Y}


#endif
