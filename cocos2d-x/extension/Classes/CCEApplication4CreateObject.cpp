#include "CCEApplication.h"
#include "cocoa/CCValueSupport.h"
#include "CCENarrate.h"

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
	if(strcmp(type,"CCLabelTTF")==0) {
		std::string content = ccvpString(ps,0);
		std::string fontName = ccvpString(ps,1);
		float fontSize = ccvpFloat(ps,2);
		return CCLabelTTF::create(content.c_str(),fontName.c_str(),fontSize);
	}
	if(strcmp(type,"CCENarrate")==0) {
		CCLabelTTF* label = ccvpObject(ps,0,CCLabelTTF);
		if(label==NULL) {
			throw new std::string("invalid param 1 - CCLabelTTF");
		}
		int width = ccvpInt(ps,1);
		int height = ccvpInt(ps,2);
		if(height<=0) {
			throw new std::string("invalid param 3 - height");
		}
		int mode = ccvpInt(ps,3);
		float speed = ccvpFloat(ps, 4);
		return CCENarrate::create(label, width, height,mode,speed);
	}
	
	return  NULL;
}
