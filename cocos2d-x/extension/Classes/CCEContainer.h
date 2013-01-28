#ifndef __CCE_CONTAINER_H__
#define __CCE_CONTAINER_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEContainer : public CCNode, public CCRGBAProtocol
{
    //CCRGBAProtocol
    CC_PROPERTY(GLubyte, m_cOpacity, Opacity); 
    CC_PROPERTY_PASS_BY_REF(ccColor3B, m_tColor, Color);
	
    bool m_bIsOpacityModifyRGB;
    bool isOpacityModifyRGB();
    void setOpacityModifyRGB(bool bOpacityModifyRGB);

public:
    CCEContainer();    
    virtual ~CCEContainer();
};

#endif