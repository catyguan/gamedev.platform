#include "CCEContainer.h"
#include "cocoa\CCValueSupport.h"

// CCEContainer
CCEContainer::CCEContainer()
: m_bIsOpacityModifyRGB(false)
{
	setCascadeColorEnabled(true);
	setCascadeOpacityEnabled(true);
}

CCEContainer::~CCEContainer()
{
    
}

void CCEContainer::setOpacityModifyRGB(bool bOpacityModifyRGB)
{
    m_bIsOpacityModifyRGB=bOpacityModifyRGB;
    CCObject* child;
    CCArray* children=getChildren();
    CCARRAY_FOREACH(children, child)
    {
        CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);        
        if (pNode)
        {
            pNode->setOpacityModifyRGB(bOpacityModifyRGB);
        }
    }
}

bool CCEContainer::isOpacityModifyRGB()
{
    return m_bIsOpacityModifyRGB;
}

CCEContainer* CCEContainer::create()
{
	CCEContainer *pRet = new CCEContainer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }    
    return pRet;
}

// CCEPanel
CCEPanel::CCEPanel()
{
	m_padding = 0;
	m_gridWidth = m_gridHeight = 1;
}

CCEPanel::~CCEPanel()
{

}

CCEPanel* CCEPanel::create()
{
	CCEPanel *pRet = new CCEPanel();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

void CCEPanel::setItems(std::vector<CCEPanelGridItem>& items)
{
	m_items.clear();
	std::vector<CCEPanelGridItem>::const_iterator it = items.begin();
	for(;it!=items.end();it++) {
		m_items.push_back(*it);
		if(it->node!=NULL && it->node->getParent()==NULL) {
			addChild(it->node);
		}
	}
}

CC_BEGIN_CALLS(CCEPanel, CCEContainer)
	CC_DEFINE_CALL(CCEPanel, type)
	CC_DEFINE_CALL(CCEPanel, padding)
	CC_DEFINE_CALL(CCEPanel, items)
CC_END_CALLS(CCEPanel, CCEContainer)

CCValue CCEPanel::CALLNAME(type)(CCValueArray& params) {
	if(params.size()>0) {
		std::string v = params[0].stringValue();
		setType(v);
	}
	return CCValue::stringValue(getType());
}

CCValue CCEPanel::CALLNAME(padding)(CCValueArray& params) {
	if(params.size()>0) {
		float v = params[0].floatValue();
		setPadding(v);
	}
	return CCValue::numberValue(getPadding());
}

static void createItem(CCValue* val, CCEPanelGridItem& item)
{
	if(val->isNull()) {
		item.node = NULL;
		return;
	}
	if(val->isObject()) {
		CCNode* node = dynamic_cast<CCNode*>(val->objectValue());
		if(node!=NULL) {
			item.node = node;			
		}
		return;
	}
	if(val->isMap()) {
		CCValueReader vr(val);
		CCValue* v;
		v = vr.getNull("node");
		if(v!=NULL) {
			createItem(v, item);
		} else {
			item.node = NULL;
		}

		item.width = vr.get("width")->stringValue();
		item.height = vr.get("height")->stringValue();
		
		item.rowspan = vr.get("rowspan")->intValue();
		item.colspan = vr.get("colspan")->intValue();
		return;
	}
}

CCValue CCEPanel::CALLNAME(items)(CCValueArray& params) {
	std::vector<CCEPanelGridItem> list;

	if(params.size()>0) {
		CCValueReader vr(&params[0]);
		if(vr.isArray()) {
			int c = vr.arraySize();
			int row = 1;
			for(int i=0;i<c;i++) {
				if(vr.beArray(i)) {
					int c2 = vr.arraySize();
					int col = 1;
					for(int j=0;j<c2;j++) {
						CCValue* val = vr.get(j);
						CCEPanelGridItem item;
						item.node = NULL;
						item.row = row;
						item.col = col;
						item.rowspan = item.rowspan = 0;
						createItem(val, item);
						if(item.colspan>1) {
							col+=item.colspan-1;
						}	
						list.push_back(item);
						col++;
					}
					vr.pop();
				} else {
					CCValue* val = vr.get(i);
					CCEPanelGridItem item;
					item.node = NULL;
					item.row = row;
					item.col = 1;
					item.rowspan = item.rowspan = 0;
					createItem(val, item);
					if(item.rowspan>1) {
						row+=item.rowspan-1;
					}	
					list.push_back(item);
				}
				row++;
			}
		}
	}
	setItems(list);
	return CCValue::nullValue();
}