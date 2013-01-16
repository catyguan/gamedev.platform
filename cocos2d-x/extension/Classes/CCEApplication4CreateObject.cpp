#include "CCEApplication.h"
#include "cocoa/CCValueSupport.h"

USING_NS_CC;

CCObject* CCEApplication::createObject(const char* type, CCValueArray& ps)
{
	for(std::list<CCE_CREATE_OBJECT>::const_iterator it = coFunctionList.begin();it!=coFunctionList.end();it++) {
		CCObject* r = (*it)(type, ps);
		if(r!=NULL) {
			return r;
		}
	}
	if(strcmp(type,"CCLayerColor")==0) {
		ccColor4B color = ccvpColor(ps,0);
		int width = ccvpInt(ps,1);
		int height = ccvpInt(ps,2);
		if(width>0 && height>0) {
			return CCLayerColor::create(color,(GLfloat) width, (GLfloat) height);
		} else {
			return CCLayerColor::create(color);
		}
	}	
	return  NULL;
}
