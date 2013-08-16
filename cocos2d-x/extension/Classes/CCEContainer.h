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

typedef struct _CCEPanelGridItem {
	int col;
	int row;
	int colspan;
	int rowspan;
	std::string width;
	std::string height;
	CCNode* node;
} CCEPanelGridItem;
class CCEPanel : public CCEContainer
{
	CC_SYNTHESIZE(std::string, m_type, Type)
	CC_SYNTHESIZE(float, m_padding, Padding)
	CC_SYNTHESIZE(int, m_gridWidth, GridWidth)
	CC_SYNTHESIZE(int, m_gridHeight, GridHeight)

public:
	static CCEPanel* create();

	void setItems(std::vector<CCEPanelGridItem>& items);

public:
    CCEPanel();    
    virtual ~CCEPanel();

protected:
	std::vector<CCEPanelGridItem> m_items;
	
	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(type)
	CC_DECLARE_CALL(padding)
	CC_DECLARE_CALL(items)
	CC_DECLARE_CALLS_END
};

#endif