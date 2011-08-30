#include "dt_skinbutton.h"

CSkinButton::CSkinButton(skin_item_t* data, CSkinForm* parent,string typeName):
    CSkinCtrl(data,parent,typeName)

{
    if(_parent)
    {
        _parent->AddSkinItem(SKIN_BUTTON_STYLE, data);
        _id = data->id;
    }
}
CSkinButton::CSkinButton(SKIN_BUTTON_DESC* desc, CSkinForm* parent,string typeName):
    CSkinCtrl(parent,typeName)
{
    if(_parent)
    {
        _id =_parent->AddSkinItem(SKIN_BUTTON_STYLE,desc->bmpindex, desc->left,desc->top,desc->caption);
        //printf("id = %d\n",_id);
    }
}
CSkinButton::CSkinButton(int bmpindex,SKIN_BUTTON_DESC* desc, CSkinForm* parent,string typeName):
    CSkinCtrl(parent,typeName)
{
    if(_parent)
    {
        _id =_parent->AddSkinItem(SKIN_BUTTON_STYLE,bmpindex, desc->left,desc->top);
    }
}
CSkinButton::~CSkinButton()
{

}

bool CSkinButton::GetCheckStatus()
{
    if(_parent)
    {
        return _parent->GetCheckStatus(_id);
    }
    return false;
}
