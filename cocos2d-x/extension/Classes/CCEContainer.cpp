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
	m_marginBottom = m_marginTop = m_marginLeft = m_marginRight = 0;
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


CC_BEGIN_CALLS(CCEPanel, CCEContainer)
	CC_DEFINE_CALL(CCEPanel, type)
	CC_DEFINE_CALL(CCEPanel, padding)
	CC_DEFINE_CALL(CCEPanel, margin)
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

CCValue CCEPanel::CALLNAME(margin)(CCValueArray& params) {
	if(params.size()>0) {
		CCValueReader vr(&params[0]);
		if(vr.isMap()) {
			CCValue* v;

			v = vr.getNull("left");
			if(v!=NULL)m_marginLeft = v->floatValue();

			v = vr.getNull("top");
			if(v!=NULL)m_marginTop = v->floatValue();

			v = vr.getNull("right");
			if(v!=NULL)m_marginRight = v->floatValue();

			v = vr.getNull("bottom");
			if(v!=NULL)m_marginBottom = v->floatValue();
		}
		return CCValue::nullValue();
	} else {
		CCValueMap map;
		map["left"] = CCValue::numberValue(m_marginLeft);
		map["top"] = CCValue::numberValue(m_marginTop);
		map["right"] = CCValue::numberValue(m_marginRight);
		map["height"] = CCValue::numberValue(m_marginRight);		
		return CCValue::mapValue(map);
	}
}

CCValue CCEPanel::CALLNAME(items)(CCValueArray& params) {
	if(params.size()>0) {
		CCValueReader vr(&params[0]);
		if(vr.isArray()) {
			int c = vr.arraySize();
			int row = 1;
			for(int i=0;i<c;i++) {
				if(vr.beArray(i)) {
				} else {
					CCValue* v = vr.getNull(i);
					if(v!=NULL && v->isObject()) {
						CCNode* node = dynamic_cast<CCNode*>(v->objectValue());
						if(node!=NULL) {
							node->attribute("layout_row", CCValue::intValue(row));
							addChild(node);
							int span = node->attribute("layout_rowspan").intValue();
							if(span>1) {
								row+=span-1;
							}							
						}
					}
				}
				row++;
			}
		}
	}
	return CCValue::nullValue();
}