#include "CCEContainer.h"

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
