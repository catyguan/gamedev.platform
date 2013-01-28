#include "CCEContainer.h"

CCEContainer::CCEContainer()
: m_cOpacity(0)
, m_tColor(ccBLACK)
, m_bIsOpacityModifyRGB(false)
{

}

CCEContainer::~CCEContainer()
{
    
}

// CRGBA protocol
void CCEContainer::setColor(const ccColor3B& color)
{
    m_tColor=color;
    CCObject* child;
    CCArray* children=getChildren();
    CCARRAY_FOREACH(children, child)
    {
        CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);        
        if (pNode)
        {
            pNode->setColor(m_tColor);
        }
    }
}

ccColor3B CCEContainer::getColor(void)
{
    return m_tColor;
}


void CCEContainer::setOpacity(GLubyte opacity)
{
    m_cOpacity = opacity;
    
    CCObject* child;
    CCArray* children=getChildren();
    CCARRAY_FOREACH(children, child)
    {
        CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);        
        if (pNode)
        {
            pNode->setOpacity(opacity);
        }
    }
}

GLubyte CCEContainer::getOpacity()
{
    return m_cOpacity;
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
