#ifndef __CCE_RECTS_NODE_H__
#define __CCE_RECTS_NODE_H__

#include "cocos2d.h"

USING_NS_CC;

typedef struct _cceRectInfo
{
	ccVertex2F square[4];
	ccColor4F  color[4];
} cceRectInfo;

//
// CCERectNode
//
class CCERectsNode : public CCNode, public CCBlendProtocol
{
protected:	
	int m_size;
	int m_dataLen;
	cceRectInfo* m_pInfos;

public:
	CCERectsNode();
	virtual ~CCERectsNode();

	virtual void draw();
	
	static CCERectsNode* create();
	virtual bool init();

	/** BlendFunction. Conforms to CCBlendProtocol protocol */
	CC_PROPERTY(ccBlendFunc, m_tBlendFunc, BlendFunc)

public:
	void add(CCRect rect, ccColor4B color);
	void clear();

};

#endif // __CCE_PIXEL_NODE_H__

