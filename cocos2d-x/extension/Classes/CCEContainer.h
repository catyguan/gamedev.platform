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
} CCEGridItem;
class CCEGrid : public CCEContainer
{
	CC_SYNTHESIZE(float, m_padding, Padding)
	CC_SYNTHESIZE(int, m_gridWidth, GridWidth)
	CC_SYNTHESIZE(int, m_gridHeight, GridHeight)

public:
	static CCEGrid* create();

	void setItems(std::vector<CCEGridItem>& items);

	virtual void doLayout(bool deep);

public:
    CCEGrid();    
    virtual ~CCEGrid();

protected:
	std::vector<CCEGridItem> m_items;
	
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(doLayout)
	CC_DECLARE_CALL(padding)
	CC_DECLARE_CALL(grid)
	CC_DECLARE_CALLS_END
};

class CCEPanel : public CCEContainer
{
	CC_SYNTHESIZE(float, m_padding, Padding)
	CC_SYNTHESIZE(bool, m_vertical, Vertical)
	

public:
	static CCEPanel* create();

	void setAutoSize(bool v){m_autoSize = v;m_calSize = false;};
	bool isAutoSize(){return m_autoSize;};

	virtual void doLayout(bool deep);
	virtual const CCSize& getContentSize() const;
	virtual void addChild(CCNode* child, int zOrder, int tag);
	virtual void removeChild(CCNode* child, bool cleanup);

protected:
	void autoResize();

protected:
	bool m_calSize;
	bool m_autoSize;

public:
    CCEPanel();    
    virtual ~CCEPanel();
	
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(doLayout)	
	CC_DECLARE_CALL(padding)
	CC_DECLARE_CALL(vertical)
	CC_DECLARE_CALL(autoSize)
	CC_DECLARE_CALLS_END
};

#endif