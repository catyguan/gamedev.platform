#ifndef __CCE_CONTAINER_H__
#define __CCE_CONTAINER_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEContainer : public CCNodeRGBA
{
protected:
    bool m_bIsOpacityModifyRGB;

public:
    virtual bool isOpacityModifyRGB();
    virtual void setOpacityModifyRGB(bool bOpacityModifyRGB);

public:
    CCEContainer();    
    virtual ~CCEContainer();
};

#endif