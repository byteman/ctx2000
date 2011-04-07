#include "dt_skinlable.h"
static si_bmplabel_t num_bmp_desc = { "1258.00", "0123456789 -." };
CSkinLable::CSkinLable(SKIN_CTRL_DESC* desc, CSkinForm* parent,string typeName,void* attach_data):
        CSkinCtrl(parent,typeName)
{
    if(_parent) {
         _id =_parent->AddSkinItem(SKIN_BMPLBL_STYLE,desc->bmpindex, desc->left,desc->top,(void*)attach_data);
    }
}
CSkinLable::~CSkinLable()
{

}
void CSkinLable::Display(std::string strText)
{
    if(_parent) {
        _parent->SetSkinlableText(_id,strText.c_str());
    }
}
