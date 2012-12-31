#ifndef  __CCE_ACTOR_CHARACTER_H__
#define  __CCE_ACTOR_CHARACTER_H__

#include "CCEActor.h"

class CCEActorCharacter : public CCEActorSprite
{
public:
	CCEActorCharacter();
	virtual ~CCEActorCharacter();

public:
	cocos2d::CCAction* action(const char* name);
	cocos2d::CCAction* actionForever(const char* name);

	void setAction(const char* name, cocos2d::CCAction* action);

	CCEActorCommand* statusSwitchAction(const char* type, const char* newActionName);

	CCEActorCommand* statusEndAction(const char* type);

public:
	virtual void cleanup();

protected:
	cocos2d::CCDictionary actions;
	cocos2d::CCDictionary statusActions;
};


#endif