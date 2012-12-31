#ifndef  __CCE_SPRITEFRAME_UTIL_H__
#define  __CCE_SPRITEFRAME_UTIL_H__

#include "cocos2d.h"
#include <list>

class CCESpriteFrameUtil
{
public:
	static cocos2d::CCSpriteFrame* addSpriteFramesWithTiles(const char* frameName, const char* textureFileName, 
			int tileOrgWidth, int tileOrgHeight, int tileXOffset, int tileYOffset, int tileXCount,int tileYCount);

	static std::string tileName(const char* frameName, int x,int y);

	static cocos2d::CCAnimate* createAnimateWithTiles(const char* frameName, std::list<cocos2d::CCPoint>& tilePosList, float delay);
};

class CCETileSpriteBuilder
{
public:
	CCETileSpriteBuilder(const char* frameName, const char* textureFileName, 
			int tileOrgWidth, int tileOrgHeight, int tileXOffset, int tileYOffset, int tileXCount,int tileYCount);
	~CCETileSpriteBuilder();

	cocos2d::CCSprite* createSprite();
	cocos2d::CCAnimate* createAnimate(int x,int xcount, int y, float delay);

protected:
	std::string frameName;
};

#endif // __CCE_APPUTIL_H_

