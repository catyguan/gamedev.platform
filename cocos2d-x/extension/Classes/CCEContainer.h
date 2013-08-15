#ifndef __CCE_CONTAINER_H__
#define __CCE_CONTAINER_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEContainer : public CCNodeRGBA
{
protected:
    bool m_bIsOpacityModifyRGB;

public:
	static CCEContainer* create();

    virtual bool isOpacityModifyRGB();
    virtual void setOpacityModifyRGB(bool bOpacityModifyRGB);

public:
    CCEContainer();    
    virtual ~CCEContainer();
};

class CCEPanel : public CCEContainer
{
	CC_SYNTHESIZE(std::string, m_type, Type)
	CC_SYNTHESIZE(float, m_padding, Padding)
	CC_SYNTHESIZE(float, m_marginLeft, MarginLeft)
	CC_SYNTHESIZE(float, m_marginTop, MarginTop)
	CC_SYNTHESIZE(float, m_marginRight, MarginRight)
	CC_SYNTHESIZE(float, m_marginBottom, MarginBottom)
	CC_SYNTHESIZE(int, m_gridWidth, GridWidth)
	CC_SYNTHESIZE(int, m_gridHeight, GridHeight)

public:
	static CCEPanel* create();

public:
    CCEPanel();    
    virtual ~CCEPanel();

protected:
	
	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(type)
	CC_DECLARE_CALL(padding)
	CC_DECLARE_CALL(items)
	CC_DECLARE_CALL(margin)
	CC_DECLARE_CALLS_END
};

#endif