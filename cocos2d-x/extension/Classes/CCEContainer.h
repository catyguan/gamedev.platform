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

	virtual void cleanup();

	virtual void addRef(CCObject* obj);
	virtual CCObject* getRefById(const char* id);
	virtual CCObject* getRefByTag(int tag);
	virtual void removeRefById(const char* id);
	virtual void removeRefByTag(int tag);
	virtual std::list<CCObject*>& getRefs();
	virtual void removeAllRefs();

public:
    CCEContainer();    
    virtual ~CCEContainer();

protected:
	void clearRefs();

protected:
	std::list<CCObject*> m_refs;

	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(addRef)
	CC_DECLARE_CALL(getRef)
	CC_DECLARE_CALL(removeRef)
	CC_DECLARE_CALL(removeAllRefs)
	CC_DECLARE_CALL(getAllRefs)
	CC_DECLARE_CALLS_END
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
	CC_SYNTHESIZE(float, m_padding, Padding)
	CC_SYNTHESIZE(int, m_gridWidth, GridWidth)
	CC_SYNTHESIZE(int, m_gridHeight, GridHeight)

public:
	static CCEPanel* create();

	void setItems(std::vector<CCEPanelGridItem>& items);

	virtual void doLayout(bool deep);

public:
    CCEPanel();    
    virtual ~CCEPanel();

protected:
	std::vector<CCEPanelGridItem> m_items;
	
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(doLayout)
	CC_DECLARE_CALL(padding)
	CC_DECLARE_CALL(grid)
	CC_DECLARE_CALLS_END
};

#endif