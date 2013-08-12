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
		CCValueReader r(&cfg);
		CCLabelTTF* o;
		if(r.isMap()) {
			CCValue* v;
			std::string str;
			v = r.getNull("content");
			if(v!=NULL)str = v->stringValue();							

			std::string fontName;
			v = r.getNull("fontName");
			if(v!=NULL)fontName = v->stringValue();			

			float fontSize = -1;
			v = r.getNull("fontSize");
			if(v!=NULL)fontSize = v->floatValue();			

			CCSize dm(-1,-1);
			v = r.getNull("dimensions");
			if(v!=NULL)dm = CCValueUtil::size(*v);

			int aligunment = -1;
			v = r.getNull("horizontalAlignment");
			if(v!=NULL)aligunment = v->intValue();

			int valigunment = -1;
			v = r.getNull("verticalAlignment");
			if(v!=NULL)valigunment = v->intValue();

			if(str.c_str()>0 && fontName.c_str()>0 && fontSize>0) {
				r.remove("content");
				r.remove("fontName");
				r.remove("fontSize");
				if(dm.width>=0 && dm.height>=0 && aligunment>=0) {
					r.remove("dimensions");
					r.remove("horizontalAlignment");
					if(valigunment>=0) {
						r.remove("verticalAlignment");
						o = CCLabelTTF::create(str.c_str(), fontName.c_str(), fontSize, dm, CCTextAlignment(aligunment), CCVerticalTextAlignment(valigunment));
					} else {
						o = CCLabelTTF::create(str.c_str(), fontName.c_str(), fontSize, dm, CCTextAlignment(aligunment));
					}
				} else {
					o = CCLabelTTF::create(str.c_str(), fontName.c_str(), fontSize);
				}
			}			
		}
		if(o==NULL) {
			o = CCLabelTTF::create();
		}
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
