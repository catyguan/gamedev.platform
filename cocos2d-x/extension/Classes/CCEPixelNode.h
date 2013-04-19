#ifndef __CCE_PIXEL_NODE_H__
#define __CCE_PIXEL_NODE_H__

#include "cocos2d.h"

USING_NS_CC;

typedef struct _ccePixelInfo
{
	int type;
	ccVertex2F square[4];
	ccColor4F  color[4];
	bool rgbColor;
	bool hide;
} ccePixelInfo;

typedef struct _ccePixelPart
{
	int id;
	int len;				// pixels valid len
	ccePixelInfo* pixels;
	bool hide;
	_ccePixelPart* next;
} ccePixelPart;

typedef struct _ccePixelDef {
	int type;
	int x;	
	int y;
	int width;
	int height;
	GLubyte r;
	GLubyte g;
	GLubyte b;
	bool rgbColor;
	bool hide;
} ccePixelDef;

//
// CCEPixelNode
//
class CCEPixelNode : public CCNodeRGBA, public CCBlendProtocol
{
protected:
	int m_size;			// pixel size
	ccePixelPart* m_pPixels;

	bool m_flipX, m_flipY;
	CCSize m_pixelContentSize;

public:
	CCEPixelNode();
	virtual ~CCEPixelNode();

	virtual void draw();
	
	static CCEPixelNode* create(int pixelSize,CCSize& pixelContentSize);
	virtual bool init(int pixelSize,CCSize& pixelContentSize);

	virtual void setPixelContentSize(CCSize& pixelContentSize);
	bool isFlipX(){return m_flipX;};
	void setFlipX(bool v);
	bool isFlipY(){return m_flipY;};
	void setFlipY(bool v);


	/** Opacity: conforms to CCRGBAProtocol protocol */
	CC_PROPERTY(GLubyte, m_cOpacity, Opacity)
	/** Color: conforms to CCRGBAProtocol protocol */
	CC_PROPERTY_PASS_BY_REF(ccColor3B, m_tColor, Color)
	/** BlendFunction. Conforms to CCBlendProtocol protocol */
	CC_PROPERTY(ccBlendFunc, m_tBlendFunc, BlendFunc)

public:
	virtual void setOpacityModifyRGB(bool bValue) {CC_UNUSED_PARAM(bValue);}
    virtual bool isOpacityModifyRGB(void) { return false;}
	
public:
	// operator
	static int count(ccePixelDef* pdef);

	void loadPart(int partId, ccePixelDef* def, int len);
	void showPart(int partId, bool show);
	void removePart(int partId);
	
	void setPixelsColor(int type, ccColor3B color);
	void showPixels(int type, bool show);
	
	void clearPixels();

protected:
	ccePixelPart* part(int partId);
	ccePixelPart* alloc(int partId, int len);	
	void pixel(ccePixelInfo* pinfo, ccePixelDef* def);
   
protected:
	virtual void updateColor();
};

#endif // __CCE_PIXEL_NODE_H__

