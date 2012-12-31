#include "CCESpriteFrameUtil.h"

#include "CCEUtil.h"

USING_NS_CC;

std::string CCESpriteFrameUtil::tileName(const char* frameName, int x,int y)
{
	return StringUtil::format("%s_%d_%d", frameName, x, y);
}

CCSpriteFrame* CCESpriteFrameUtil::addSpriteFramesWithTiles(const char* frameName, const char* textureFileName, 
			int tileOrgWidth, int tileOrgHeight, int tileOffsetX, int tileOffsetY, int tileXCount,int tileYCount)
{
	CCAssert(textureFileName, "texture name should not be null");
    CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(textureFileName);

    if (texture==NULL)
    {
        CCLOG("CCESpriteFrameUtil: couldn't load texture file. File not found %s", textureFileName);
		return NULL;
    }

	CCSpriteFrameCache* fc = CCSpriteFrameCache::sharedSpriteFrameCache();

	CCSpriteFrame* first = NULL;
	const char* fname = frameName==NULL?textureFileName:frameName;
	for(int y=0;y<tileXCount;y++) {
		for(int x=0;x<tileXCount;x++) {			
			std::string name = tileName(fname, x, y);
			CCSpriteFrame* spriteFrame = fc->spriteFrameByName(name.c_str());
			if(spriteFrame==NULL) {
				spriteFrame = new CCSpriteFrame();
				spriteFrame->initWithTexture(texture,
								CCRectMake((tileOffsetX+x)*tileOrgWidth, (tileOffsetY+y)*tileOrgHeight, tileOrgWidth, tileOrgHeight),
								false,
								CCPointMake(0, 0),
								CCSizeMake(tileOrgWidth,tileOrgHeight));
				fc->addSpriteFrame(spriteFrame, name.c_str());
				spriteFrame->release();
			}
			if(first==NULL)first = spriteFrame;
		}
	}
	if(first==NULL) {
		CCLOG("CCESpriteFrameUtil: no sprite frame");
		return NULL;
	}
	return first;
}

CCAnimate* CCESpriteFrameUtil::createAnimateWithTiles(const char* frameName, std::list<CCPoint>& tilePosList, float delay)
{
	CCSpriteFrameCache* fc = CCSpriteFrameCache::sharedSpriteFrameCache();
	CCArray* animFrames = CCArray::createWithCapacity(tilePosList.size()); 
	for(std::list<CCPoint>::const_iterator it = tilePosList.begin();it!=tilePosList.end(); it++) 
    {
		std::string name = tileName(frameName, (int) it->x, (int) it->y);
		CCSpriteFrame* frame = fc->spriteFrameByName(name.c_str());
		CC_ASSERT(frame!=NULL);
        animFrames->addObject(frame);
    }
    return CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, delay));
}

CCETileSpriteBuilder::CCETileSpriteBuilder(const char* fn, const char* textureFileName, 
			int tileOrgWidth, int tileOrgHeight, int tileXOffset, int tileYOffset, int tileXCount,int tileYCount)
{
	frameName = fn;	
	CCESpriteFrameUtil::addSpriteFramesWithTiles(fn,textureFileName,tileOrgWidth,tileOrgHeight,tileXOffset,tileYOffset,tileXCount,tileYCount);
}

CCETileSpriteBuilder::~CCETileSpriteBuilder()
{
	
}

CCSprite* CCETileSpriteBuilder::createSprite()
{
	return CCSprite::createWithSpriteFrameName(CCESpriteFrameUtil::tileName(frameName.c_str(),0,0).c_str());
}

CCAnimate* CCETileSpriteBuilder::createAnimate(int x,int xcount, int y, float delay)
{	
	std::list<CCPoint> list;
	for(int i=0;i<xcount;i++) {
		list.push_back(ccp(x+i,y));
	}
	return CCESpriteFrameUtil::createAnimateWithTiles(frameName.c_str(),list,delay);
}
