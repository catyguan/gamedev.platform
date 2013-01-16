#include "CCEActorCharacter.h"
#include "CCEUtil.h"

USING_NS_CC;

CCEActorCharacter::CCEActorCharacter()
{
    
}

CCEActorCharacter::~CCEActorCharacter()
{
	
}

void CCEActorCharacter::cleanup()
{
	statusActions.removeAllObjects();
	actions.removeAllObjects();
	CCSprite::cleanup();
}

/*
bool CCEActorCharacter::processCommand(CCELuaHost* host, int callId, std::string name, CCValueArray& ctx)
{	
	if(name.compare("statusSwitch")==0) {
		if(ctx.size()<1)throw std::string("invalid param size");
		const char* type = ctx[0].stringValue().c_str();
		CCAction* a = (CCAction*) statusActions.objectForKey(type);
		if(a!=NULL) {
			CC_ASSERT(m_pSprite);
			m_pSprite->stopAction(a);
		}
		statusActions.removeObjectForKey(type);
		if(ctx.size()>1) {
			const char* newActionName = ctx[1].stringValue().c_str();
			a = (CCAction*) actions.objectForKey(newActionName);
			if(a==NULL) {
				throw std::string(ctx[1].stringValue()+" invalid action");
			}			
			m_pSprite->runAction(a);
			statusActions.setObject(a, type);
		}
		ctx.clear();
		return true;
	}
	throw std::string("unknow command '"+name+"'");
}
*/

CCAction* CCEActorCharacter::action(const char* name)
{
	return (CCAction*) actions.objectForKey(name);
}

CCAction* CCEActorCharacter::actionForever(const char* name)
{
	CCAction* a = (CCAction*)actions.objectForKey(name);
	CC_ASSERT(a!=NULL);
	CC_ASSERT(dynamic_cast<CCActionInterval*>(a)!=NULL);
	return CCRepeatForever::create((CCActionInterval*) a);
}

void CCEActorCharacter::setAction(const char* name,CCAction* action)
{
	actions.setObject(action, name);
}

/*
CCEActorCommand* CCEActorCharacter::statusSwitchAction(const char* type, const char* newActionName)
{
	CCValueArray ps;
	ps.push_back(CCValue::stringValue(type));
	if(newActionName!=NULL) {
		ps.push_back(CCValue::stringValue(newActionName));
	}
	return CCEActorCommand::create(this,"statusSwitch",ps);	
}

CCEActorCommand* CCEActorCharacter::statusEndAction(const char* type)
{
	return statusSwitchAction(type,NULL);
}
*/