#include "CCEApplication.h"
#include "cocoa/CCValueSupport.h"
#include "CCELayerTouch.h"
#include "CCENarrate.h"
#include "CCEDialogue.h"

USING_NS_CC;

CCObject* CCEApplication::createObject(const char* type, CCValue& cfg)
{
	for(std::list<CCE_CREATE_OBJECT>::const_iterator it = coFunctionList.begin();it!=coFunctionList.end();it++) {
		CCObject* r = (*it)(type, cfg);
		if(r!=NULL) {
			return r;
		}
	}
	if(strcmp(type,"CCLayerColor")==0) {
		CCLayerColor* o = CCLayerColor::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCELayerTouch")==0) {
		return CCELayerTouch::create();
	}
	if(strcmp(type,"CCLabelTTF")==0) {
		CCLabelTTF* o = CCLabelTTF::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCEDialogue")==0) {
		CCEDialogue* o = CCEDialogue::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCENarrate")==0) {
		/*CCLabelTTF* label = ccvpObject(ps,0,CCLabelTTF);
		if(label==NULL) {
			throw new std::string("invalid param 1 - CCLabelTTF");
		}
		int width = ccvpInt(ps,1);
		int height = ccvpInt(ps,2);
		if(height<=0) {
			throw new std::string("invalid param 3 - height");
		}
		float speed = ccvpFloat(ps, 3);
		return CCENarrate::create(label, width, height,speed);*/
	}
	
	return  NULL;
}
