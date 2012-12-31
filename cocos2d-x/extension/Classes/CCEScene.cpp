#include "CCEScene.h"
#include "CCEApplication.h"
#include "CCEActor.h"

USING_NS_CC;

CCEScene::CCEScene()
{
	
}

CCEScene::~CCEScene()
{
	
}

bool CCEScene::init()
{
	return true;
}

void CCEScene::onEnter()
{
	CCScene::onEnter();

	if(enterFunction.size()>0) {
		// TODO
		// CCEApplication::sharedApp()->call(NULL, );
	}
}

void CCEScene::addActor(const char* name, CCEActor* actor)
{
	actors.setObject(actor, name);
}

CCEActor* CCEScene::getActor(const char* name)
{
	return (CCEActor*) actors.objectForKey(name);
}

CCEActor* CCEScene::createActor(const char* name,CCValueArray& ps)
{
	return NULL;
}


void CCEScene::removeActor(const char* name)
{
	actors.removeObjectForKey(name);
}

void CCEScene::cleanup()
{
	CCScene::cleanup();
	CCArray* keys = actors.allKeys();
	if(keys!=NULL) {
		for(unsigned int i=0;i<keys->count();i++) {
			CCString* name = (CCString*) keys->objectAtIndex(i);
			CCEActor* actor = (CCEActor*) actors.objectForKey(name->m_sString);
			if(actor!=NULL) {
				actor->cleanup();
			}
		}
	}
	actors.removeAllObjects();
}
