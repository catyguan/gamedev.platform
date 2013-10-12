#include "CCEApplication.h"
#include "cocoa/CCValueSupport.h"
#include "CCEScene.h"
#include "CCELayerTouch.h"
#include "CCEContainer.h"
#include "CCENarrate.h"
#include "CCEDialogue.h"
#include "CCScale9Sprite.h"
#include "CCEAction.h"
#include "CCEButton.h"
#include "CCEScrollView.h"
#include "editbox\CCEditBox.h"
#include "misc_nodes\CCRenderTexture.h"

USING_NS_CC;

CCObject* CCEApplication::createObject(const char* type, CCValue& cfg)
{
	for(std::list<CCE_CREATE_OBJECT>::const_iterator it = coFunctionList.begin();it!=coFunctionList.end();it++) {
		CCObject* r = (*it)(type, cfg);
		if(r!=NULL) {
			return r;
		}
	}
	if(strcmp(type,"CCEScene")==0) {
		CCEScene* o = CCEScene::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCLayerColor")==0) {
		CCLayerColor* o = CCLayerColor::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCSprite")==0) {
		CCSprite* o = NULL;

		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;
			std::string image;
			v = r.getNull("image");
			if(v!=NULL)image = v->stringValue();

			if(image.size()>0) {
				r.remove("image");

				CCRect rect = CCRectMake(0,0,0,0);
				v = r.getNull("rect");
				if(v!=NULL)rect = CCValueUtil::rect(*v);

				if(rect.size.width>0 && rect.size.height>0) {
					r.remove("rect");
					o = CCSprite::create(image.c_str(), rect);
				} else {
					o = CCSprite::create(image.c_str());
				}
			}
		}

		if(o==NULL) {
			o = CCSprite::create();
		}
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCSpriteFrame")==0) {
		CCSpriteFrame* o = NULL;

		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;
			std::string image;
			v = r.getNull("image");
			if(v!=NULL)image = v->stringValue();

			CCRect rect = CCRectMake(0,0,0,0);
			v = r.getNull("rect");
			if(v!=NULL)rect = CCValueUtil::rect(*v);

			if(image.size()>0) {
				r.remove("image");
				if(rect.size.width>0 && rect.size.height>0) {
					r.remove("rect");
					o = CCSpriteFrame::create(image.c_str(), rect);
				}
			}
		}			
		return o;
	}
	if(strcmp(type,"CCTexture2D")==0) {
		CCTexture2D* o = NULL;

		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;
			std::string image;
			v = r.getNull("image");
			if(v!=NULL)image = v->stringValue();

			if(image.size()>0) {
				r.remove("image");
				o = CCTextureCache::sharedTextureCache()->addImage(image.c_str());
			}
		}
		return o;
	}
	if(strcmp(type,"CCELayerTouch")==0) {
		CCELayerTouch* o = CCELayerTouch::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCLabelTTF")==0) {
		CCValueReader r(&cfg);
		CCLabelTTF* o = NULL;
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
			if(v!=NULL) {
				fontSize = v->floatValue();
			}

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
	if(strcmp(type,"CCEPanel")==0) {
		CCEPanel* o = CCEPanel::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCEGrid")==0) {
		CCEGrid* o = CCEGrid::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCEContainer")==0) {
		CCEContainer* o = CCEContainer::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCEControl")==0) {
		CCEControl* o = CCEControl::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCEButton")==0) {
		CCEButton* o = NULL;
		
		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;
			CCNode* node = NULL;
			v = r.getNull("node");
			if(v!=NULL) {
				CCObject* obj = v->objectValue();
				if(obj!=NULL)node = dynamic_cast<CCNode*>(obj);
			}

			CCNode* bgnode = NULL;
			v = r.getNull("background");
			if(v!=NULL) {
				CCObject* obj = v->objectValue();
				if(obj!=NULL)bgnode = dynamic_cast<CCNode*>(obj);
			}

			if(node!=NULL || bgnode!=NULL) {
				r.remove("node");
				r.remove("background");
				o = CCEButton::create(node, bgnode);				
			}
		}

		if(o==NULL)o = CCEButton::create();
		o->setup(cfg);
		return o;
	}	
	if(strcmp(type,"CCEditBox")==0) {
		CCEditBox* o = NULL;
		CCSize sz;
		
		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;			
			v = r.getNull("size");
			if(v!=NULL) {
				sz = CCValueUtil::size(*v);
				r.remove("size");
			}
				
			
		}
		o = CCEditBox::create(sz);
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCEDialogue")==0) {
		CCEDialogue* o = CCEDialogue::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCENarrate")==0) {
		CCENarrate* o = CCENarrate::create();
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCScale9Sprite")==0) {
		CCScale9Sprite* o = NULL;

		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;
			std::string image;
			v = r.getNull("image");
			if(v!=NULL)image = v->stringValue();

			if(image.size()>0) {
				r.remove("image");

				CCRect rect = CCRectMake(0,0,0,0);
				v = r.getNull("rect");
				if(v!=NULL)rect = CCValueUtil::rect(*v);

				if(rect.size.width>0 && rect.size.height>0) {
					r.remove("rect");
					o = CCScale9Sprite::create(image.c_str(), rect);
				} else {
					o = CCScale9Sprite::create(image.c_str());
				}
			}
		}
		
		if(o==NULL) {
			o = CCScale9Sprite::create();
		}
		o->setup(cfg);
		return o;
	}
	if(strcmp(type,"CCEScrollView")==0) {
		CCEScrollViewBase* o = NULL;

		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;
			CCSize size;
			v = r.getNull("viewSize");
			if(v!=NULL)size = CCValueUtil::size(*v);

			if(size.width>0 && size.height>0) {
				r.remove("viewSize");

				CCNode* content = NULL;
				v = r.getNull("content");
				if(v!=NULL) {
					if(v->isObject()) {
						content = dynamic_cast<CCNode*>(v->objectValue());
					}
					r.remove("content");
				} else {
					if(r.beMap("vmode")) {
						v = r.get("contentSize");
						CCSize csz = CCValueUtil::size(*v);
						v = r.get("tileSize");
						CCSize tsz = CCValueUtil::size(*v);
						v = r.get("helper");
						r.pop();
						content = CCELayer4ScrollVMode::create(csz, tsz);
						content->bindMethod("vmodeHelper", *v);
					}
				}				
				o = CCEScrollView::create(size, content);

				v = r.get("touchEnable");
				if(v->booleanValue()) {					
					o->setTouchEnabled(true);
				}
			}
		}
		
		if(o!=NULL) {
			o->setup(cfg);
		}
		return o;
	}
	if(strcmp(type,"CCRenderTexture")==0) {
		CCRenderTexture* o = NULL;

		CCValueReader r(&cfg);
		if(r.isMap()) {
			CCValue* v;
			CCSize size;
			v = r.getNull("width");
			if(v!=NULL)size.width = v->floatValue();
			v = r.getNull("height");
			if(v!=NULL)size.height = v->floatValue();

			if(size.width>0 && size.height>0) {
				r.remove("width");
				r.remove("height");
				o = CCRenderTexture::create((int) size.width, (int) size.height);
			}
		}
		
		if(o!=NULL) {
			o->setup(cfg);
		}
		return o;
	}

	// actions
	if(cfg.isNull() || cfg.isArray()) {
		CCValueArray* arr = cfg.arrayValue();
		if(strcmp(type,"a.seq")==0) {
			CCArray* oarr = CCArray::create();
			if(arr!=NULL) {
				for(size_t i=0;i<arr->size();i++) {
					CCAction* obj = ccvpObject((*arr),i,CCAction);
					if(obj!=NULL) {
						oarr->addObject(obj);
					}
				}
			}
			return CCSequence::create(oarr);
		}		
		if(strcmp(type,"a.show")==0) {
			return CCShow::create();
		}
		if(strcmp(type,"a.hide")==0) {
			return CCHide::create();
		}
		if(strcmp(type,"a.toggleVisible")==0) {
			return CCToggleVisibility::create();
		}
		if(strcmp(type,"a.removeSelf")==0) {		
			if(arr!=NULL && arr->size()>0) {
				return CCRemoveSelf::create((*arr)[0].booleanValue());
			} else {
				return CCRemoveSelf::create();
			}
		}
		if(arr!=NULL) {
			if(strcmp(type,"a.call")==0) {		
				CCValue call = ccvp((*arr), 0);
				return CCECall::create(call);
			}
			if(strcmp(type,"a.delay")==0 || strcmp(type,"a.delayTime")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				return CCDelayTime::create(a0);				
			}
			if(strcmp(type,"a.flipX")==0) {		
				bool v = ccvpBoolean((*arr), 0);
				return CCFlipX::create(v);
			}
			if(strcmp(type,"a.flipY")==0) {		
				bool v = ccvpBoolean((*arr), 0);
				return CCFlipY::create(v);
			}
			if(strcmp(type,"a.place")==0) {		
				CCPoint pt = ccvpPoint((*arr), 0);
				return CCPlace::create(pt);
			}
			if(strcmp(type,"a.repeat")==0) {		
				CCFiniteTimeAction* a1 = ccvpObject((*arr), 0, CCFiniteTimeAction);
				int a2 = ccvpInt((*arr),1);
				if(a2<1)a2=1;
				return CCRepeat::create(a1, a2);
			}
			if(strcmp(type,"a.forever")==0) {		
				CCActionInterval* a1 = ccvpObject((*arr), 0, CCActionInterval);
				return CCRepeatForever::create(a1);
			}
			if(strcmp(type,"a.spawn")==0) {		
				CCArray* oarr = CCArray::create();
				if(arr!=NULL) {
					for(size_t i=0;i<arr->size();i++) {
						CCAction* obj = ccvpObject((*arr),i,CCAction);
						if(obj!=NULL) {
							oarr->addObject(obj);
						}
					}
				}
				return CCSpawn::create(oarr);
			}
			if(strcmp(type,"a.rotateTo")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				float a1 = ccvpFloat((*arr), 1);
				if(arr->size()>2) {
					float a2 = ccvpFloat((*arr), 2);
					return CCRotateTo::create(a0,a1,a2);
				} else {
					return CCRotateTo::create(a0,a1);
				}
			}
			if(strcmp(type,"a.rotateBy")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				float a1 = ccvpFloat((*arr), 1);
				if(arr->size()>2) {
					float a2 = ccvpFloat((*arr), 2);
					return CCRotateBy::create(a0,a1,a2);
				} else {
					return CCRotateBy::create(a0,a1);
				}
			}
			if(strcmp(type,"a.moveBy")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				CCPoint a1 = ccvpPoint((*arr), 1);
				return CCMoveBy::create(a0,a1);
			}
			if(strcmp(type,"a.moveTo")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				CCPoint a1 = ccvpPoint((*arr), 1);
				return CCMoveTo::create(a0,a1);
			}
			if(strcmp(type,"a.skewTo")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				float a1 = ccvpFloat((*arr), 1);
				float a2 = ccvpFloat((*arr), 2);
				return CCSkewTo::create(a0,a1,a2);
			}
			if(strcmp(type,"a.skewBy")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				float a1 = ccvpFloat((*arr), 1);
				float a2 = ccvpFloat((*arr), 2);
				return CCSkewBy::create(a0,a1,a2);
			}
			if(strcmp(type,"a.JumpBy")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				CCPoint a1 = ccvpPoint((*arr), 1);
				float a2 = ccvpFloat((*arr), 2);
				int a3 = ccvpInt((*arr),3);				
				return CCJumpBy::create(a0,a1,a2, (unsigned int) a3);
			}
			if(strcmp(type,"a.JumpTo")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				CCPoint a1 = ccvpPoint((*arr), 1);
				float a2 = ccvpFloat((*arr), 2);
				int a3 = ccvpInt((*arr),3);
				return CCJumpTo::create(a0,a1,a2, (unsigned int) a3);
			}
			if(strcmp(type,"a.scaleTo")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				float a1 = ccvpFloat((*arr), 1);
				if(arr->size()>2) {
					float a2 = ccvpFloat((*arr), 2);
					return CCScaleTo::create(a0,a1,a2);
				} else {
					return CCScaleTo::create(a0,a1);
				}
			}
			if(strcmp(type,"a.scaleBy")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				float a1 = ccvpFloat((*arr), 1);
				if(arr->size()>2) {
					float a2 = ccvpFloat((*arr), 2);
					return CCScaleBy::create(a0,a1,a2);
				} else {
					return CCScaleBy::create(a0,a1);
				}
			}
			if(strcmp(type,"a.blink")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				int a1 = ccvpInt((*arr), 1);
				return CCBlink::create(a0,a1);				
			}
			if(strcmp(type,"a.fadeIn")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				return CCFadeIn::create(a0);				
			}
			if(strcmp(type,"a.fadeOut")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				return CCFadeOut::create(a0);				
			}
			if(strcmp(type,"a.fadeTo")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				int a1 = ccvpInt((*arr), 1);
				return CCFadeTo::create(a0, (GLubyte) a1);
			}
			if(strcmp(type,"a.tintTo")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				int a1 = ccvpInt((*arr), 1);
				int a2 = ccvpInt((*arr), 2);
				int a3 = ccvpInt((*arr), 3);
				return CCTintTo::create(a0, (GLubyte) a1, (GLubyte) a2, (GLubyte) a3);
			}
			if(strcmp(type,"a.tintBy")==0) {		
				float a0 = ccvpFloat((*arr), 0);
				int a1 = ccvpInt((*arr), 1);
				int a2 = ccvpInt((*arr), 2);
				int a3 = ccvpInt((*arr), 3);
				return CCTintBy::create(a0, (GLubyte) a1, (GLubyte) a2, (GLubyte) a3);
			}
			if(strcmp(type,"a.reverseTime")==0) {		
				CCFiniteTimeAction* a0 = ccvpObject((*arr),0,CCFiniteTimeAction);
				return CCReverseTime::create(a0);
			}
			if(strcmp(type,"a.animate")==0) {				
				float d = ccvpFloat((*arr),0);
				CCArray* oarr = CCArray::create();
				if(arr->size()>1 && (*arr)[1].isArray()) {
					CCValueArray* frames = (*arr)[1].arrayValue();
					if(frames!=NULL) {
						for(size_t i=0;i<frames->size();i++) {
							CCSpriteFrame* obj = ccvpObject((*frames),i,CCSpriteFrame);
							if(obj!=NULL) {
								oarr->addObject(obj);
							}
						}
					}
				} else {
					for(size_t i=1;i<arr->size();i++) {
						CCSpriteFrame* obj = ccvpObject((*arr),i,CCSpriteFrame);
						if(obj!=NULL) {
							oarr->addObject(obj);
						}
					}
				}
				return CCAnimate::create(CCAnimation::createWithSpriteFrames(oarr, d));
			}
		}
	}
	CCLOG("WARN - unknow object type, %s", type);
	return  NULL;
}

