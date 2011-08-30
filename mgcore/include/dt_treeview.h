#ifndef DT_TREEVIEW_H
#define DT_TREEVIEW_H
#include <vector>
#include "dt_commctrl.h"
//#define DEF_TREEVIEW_EXSTYLE  WS_EX_TRANSPARENT
#define DEF_TREEVIEW_EXSTYLE  WS_EX_NONE
#define DEF_TREEVIEW_STYLE    WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|TVS_NOTIFY
struct TTreeNode{
    GHANDLE hwnd;
    //std::string caption;
};
typedef std::vector<TTreeNode*>  TTreeNodeList;
class CTreeView : public CCommCtrl
{
public:

    CTreeView(COMM_CTRL_DESC* desc,CSkinForm* parent,std::string root,string typeName=CTRL_TREEVIEW);
    virtual ~CTreeView();
    GHANDLE AddItem(GHANDLE parent,std::string caption,bool fold=true);
    GHANDLE GetRoot();
    GHANDLE GetSelectItem();
    GHANDLE GetParentItem(GHANDLE item);
    GHANDLE GetFirstChildItem(GHANDLE item);
    GHANDLE GetPrevItem(GHANDLE item);
    GHANDLE GetNextItem(GHANDLE item);
    bool  SelectItem(GHANDLE item);
    bool  GetSelectItemCaption(std::string &caption);
    bool  GetItemCaption(GHANDLE item,std::string &caption);
    int   GetItemCaptionLength(GHANDLE item);
    bool EnableIconStyle(bool yes);
    bool EnableTransparent(bool yes);
    bool Clear();

private:
    TVITEMINFO m_root_node;
    std::string m_root_caption;
    TTreeNodeList nodelist;

};


#endif // DT_TREEVIEW_H
