#include "CCEDirector.h"
#include "CCELuaApplication.h"
#include "cocoa/CCValueSupport.h"

USING_NS_CC;

static CCEDirector* s_pSharedDirector = NULL;

CCEDirector::CCEDirector(CCELuaApplication* app)
{
	m_app = app;
}

CCEDirector::~CCEDirector(void)
{
	
}

void CCEDirector::install()
{
    CC_ASSERT(s_pSharedDirector==NULL);
	s_pSharedDirector = this;
}

CCEDirector* CCEDirector::sharedDirector(void)
{
	CC_ASSERT(s_pSharedDirector!=NULL);    
    return s_pSharedDirector;
}

void CCEDirector::purgeSharedDirector(void)
{
    if (s_pSharedDirector!=NULL)
    {
		CC_SAFE_RELEASE_NULL(s_pSharedDirector);
		// delete s_pSharedDirector;
    }
}

CCValue CCEDirector::apiCall(std::string name, CCValueArray& ps)
{	
	CCValueArray rs;
	if (!m_app->pcall(name.c_str(), ps, rs))
    {		
		return CCValue::nullValue();
	}
	if(rs.size()==0) {
		return CCValue::nullValue();
	}
	return rs[0];
}

bool CCEDirector::apiStartup()
{
	CCValueArray ps;
	CCValue r = apiCall("_API_application_startup", ps);
	return r.booleanValue();
}

void CCEDirector::apiShutdown()
{
	CCValueArray ps;
	apiCall("_API_application_shutdown", ps);
}

#include "CCEScene.h"
static CCScene* toSceneObject(CCValueArray& params,unsigned int idx)
{
	if(params.size()<=idx)return NULL;
	CCValue& v = params[0];
	if(!v.isObject()) {
		return NULL;
	}
	CCObject* o = v.objectValue();
	CCScene* r = dynamic_cast<CCScene*>(o);
	if(r!=NULL) {
		return r;
	}
	CCNode* node = dynamic_cast<CCNode*>(o);
	if(node!=NULL) {
		r = CCEScene::create();
		r->addChild(node);
		return r;
	}
	return NULL;
}

CCObject* CCEDirector::buildObject(CCValue& cfg)
{
	if(cfg.isArray()) {
		CCValueArray* arr = cfg.arrayValue();
		int c = arr->size();
		for(int i=0;i<c;i++) {
			CCValue& v = (*arr)[i];
			CCObject* obj = buildObject(v);
			if(obj!=NULL) {
				(*arr)[i] = CCValue::objectValue(obj);
			}			
		}	
		return NULL;
	}
	if(cfg.isMap()) {
		CCValueMap* map = cfg.mapValue();
		CCValueMapIterator it = map->begin();
		CCValueMap tmp;
		std::string type;
		for(;it!=map->end();it++) {
			if(it->second.isMap()) {
				const CCValue& v = it->second;
				CCObject* obj = buildObject((CCValue&) v);
				if(obj!=NULL) {
					tmp[it->first] = CCValue::objectValue(obj);
				}
			} else if(it->second.isArray()) {
				const CCValue& v = it->second;
				buildObject((CCValue&) v);
			} else {
				if(it->first.compare("_type")==0) {
					type = it->second.stringValue();
				}
			}
		}
		it = tmp.begin();
		for(;it!=tmp.end();it++) {
			(*map)[it->first] = it->second;
		}

		if(type.size()>0) {
			map->erase(map->find("_type"));
			return CCEApplication::sharedApp()->createObject(type.c_str(), cfg);
		}
	}
	return NULL;
}

CCAction* CCEDirector::buildAction(CCValue& cfg)
{
	if(cfg.isArray()) {
		CCValueArray* arr = cfg.arrayValue();		
		std::string type;
		for(size_t i=0;i<arr->size();i++) {
			if(i==0 && (*arr)[i].isString()) {
				type = (*arr)[i].stringValue();
				continue;
			}
			if((*arr)[i].isArray()) {
				CCAction* obj = buildAction((*arr)[i]);
				if(obj!=NULL) {
					(*arr)[i] = CCValue::objectValue(obj);
				}
			}
		}
		if(type.size()>0) {
			arr->erase(arr->begin());
			std::string id = "a."+type;
			CCObject* obj = CCEApplication::sharedApp()->createObject(id.c_str(), cfg);
			if(obj!=NULL) {
				CCAction* r = dynamic_cast<CCAction*>(obj);
				if(r!=NULL)return r;
			}
		}
	}
	return NULL;
}

void CCEDirector::pause(CCObject* obj, bool children)
{
	CCDirector* dir = CCDirector::sharedDirector();
	if(obj==NULL) {
		obj = dir->currentScene();		
	}	
	CCNode* node = dynamic_cast<CCNode*>(obj);
	if(node!=NULL) {		
		if(node->attribute("nopause").booleanValue()) {
			return;
		}
		node->pauseSchedulerAndActions();
	} else {
		dir->getScheduler()->pauseTarget(obj);
	}
	if(children) {		
		if(node!=NULL) {
			CCObject* pObj;
			CCARRAY_FOREACH(node->getChildren(), pObj)
			{
				pause(pObj, children);
			}
		}
	}
}

void CCEDirector::resume(CCObject* obj, bool children)
{
	CCDirector* dir = CCDirector::sharedDirector();
	if(obj==NULL) {
		obj = dir->currentScene();
	}
	CCNode* node = dynamic_cast<CCNode*>(obj);
	if(node!=NULL) {
		node->resumeSchedulerAndActions();
	} else {
		dir->getScheduler()->resumeTarget(obj);
	}
	if(children) {		
		if(node!=NULL) {
			CCObject* pObj;
			CCARRAY_FOREACH(node->getChildren(), pObj)
			{
				resume(pObj, children);
			}
		}
	}
}


#include "CCELayoutUtil.h"
void CCEDirector::layout(CCNode* node, bool deep)
{
	LayoutUtil::layout(node, deep);
}

CC_BEGIN_CALLS(CCEDirector, CCObject)	
	CC_DEFINE_CALL(CCEDirector, winSize)
	CC_DEFINE_CALL(CCEDirector, layout)
	CC_DEFINE_CALL(CCEDirector,createObject)
	CC_DEFINE_CALL(CCEDirector,buildObject)
	CC_DEFINE_CALL(CCEDirector,buildAction)
	CC_DEFINE_CALL(CCEDirector, scene)
	CC_DEFINE_CALL(CCEDirector, runScene)
	CC_DEFINE_CALL(CCEDirector, pushScene)
	CC_DEFINE_CALL(CCEDirector, replaceScene)
	CC_DEFINE_CALL(CCEDirector, popScene)
	CC_DEFINE_CALL(CCEDirector, contentScaleFactor)
	CC_DEFINE_CALL(CCEDirector, createNodeFrame)
	CC_DEFINE_CALL(CCEDirector, pause)
	CC_DEFINE_CALL(CCEDirector, resume)
	CC_DEFINE_CALL(CCEDirector, resetApplication)
CC_END_CALLS(CCEDirector, CCObject)

CCValue CCEDirector::CALLNAME(winSize)(CCValueArray& params) {
	CCSize sz = CCDirector::sharedDirector()->getWinSize();
	return CCValueUtil::size(sz.width, sz.height);
}
CCValue CCEDirector::CALLNAME(layout)(CCValueArray& params) {
	CCNode* node = ccvpObject(params,0,CCNode);	
	bool deep = true;
	if(params.size()>1)deep = params[1].booleanValue();
	if(node==NULL) {
		throw new std::string("param 1 expect CCNode");
	}
	layout(node, deep);
	return CCValue::nullValue();
}
CCValue CCEDirector::CALLNAME(createObject)(CCValueArray& params) {
	std::string type;
	if(params.size()<1) {
		throw new std::string("miss type");
	}
	type = params[0].stringValue();
	CCObject* obj;
	if(params.size()>1 && params[1].isMap()) {
		obj = CCEApplication::sharedApp()->createObject(type.c_str(), params[1]);
	} else {
		CCValue tmp;
		obj = CCEApplication::sharedApp()->createObject(type.c_str(), tmp);
	}
	if(obj==NULL) {
		return CCValue::nullValue();
	}
	return CCValue::objectValue(obj);
}
CCValue CCEDirector::CALLNAME(buildObject)(CCValueArray& params) {
	if(params.size()>0) {
		return CCValue::objectValue(buildObject(params[0]));
	}
	return CCValue::nullValue();
}
CCValue CCEDirector::CALLNAME(buildAction)(CCValueArray& params) {
	if(params.size()>0) {
		return CCValue::objectValue(buildAction(params[0]));
	}
	return CCValue::nullValue();
}
CCValue CCEDirector::CALLNAME(scene)(CCValueArray& params) {
	CCScene* s = CCDirector::sharedDirector()->currentScene();
	return CCValue::objectValue(s);
}

CCValue CCEDirector::CALLNAME(runScene)(CCValueArray& params) {
	CCScene* s = toSceneObject(params,0);	
	if(s==NULL) {
		throw new std::string("param 1 expect CCScene");
	}
	CCDirector* d = CCDirector::sharedDirector();
	d->replaceScene(s);	
	return CCValue::booleanValue(true);
}

CCValue CCEDirector::CALLNAME(pushScene)(CCValueArray& params) {
	CCScene* s = toSceneObject(params,0);	
	if(s==NULL) {
		throw new std::string("param 1 expect CCScene");
	}
	CCDirector::sharedDirector()->pushScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEDirector::CALLNAME(replaceScene)(CCValueArray& params)
{
	CCScene* s = toSceneObject(params,0);	
	if(s==NULL) {
		throw new std::string("param 1 expect CCScene");
	}
	CCDirector::sharedDirector()->replaceScene(s);
	return CCValue::booleanValue(true);
}

CCValue CCEDirector::CALLNAME(popScene)(CCValueArray& params)
{	
	CCDirector::sharedDirector()->popScene();
	return CCValue::nullValue();
}

CCValue CCEDirector::CALLNAME(contentScaleFactor)(CCValueArray& params) {
	return CCValue::numberValue(CCDirector::sharedDirector()->getContentScaleFactor());
}

CCValue CCEDirector::CALLNAME(pause)(CCValueArray& params) {
	CCObject* obj = ccvpObject(params, 0, CCObject);
	bool children = ccvpBoolean(params, 1);
	pause(obj, children);
	return CCValue::nullValue();
}

CCValue CCEDirector::CALLNAME(resume)(CCValueArray& params) {
	CCObject* obj = ccvpObject(params, 0, CCObject);
	bool children = ccvpBoolean(params, 1);
	resume(obj, children);
	return CCValue::nullValue();
}

#include "CCERectsNode.h"
ccColor4B sColors[] = { ccc4(255,0,0,255), ccc4(0,255,0,255),ccc4(0,0,255,255) };
int CCEDirector::buildNodeFrame(CCNode* rectsNode, CCNode* node, int size, int idx)
{
	CCPoint pt = node->convertToWorldSpace(CCPointZero);
	CCSize sz = node->getContentSize();

	CCRect rect(pt.x, pt.y, sz.width, sz.height);
	// CCLOG("nodeFrame %s - %f, %f, %f, %f,", node->getId().c_str(), pt.x, pt.y, sz.width, sz.height);
	CCRect rb = CCRectMake(rect.getMinX()-size/2, rect.getMinY()-size/2, sz.width+size, size);
	CCRect rr = CCRectMake(rect.getMaxX()-size/2, rect.getMinY()-size/2, size, sz.height+size);
	CCRect rt = CCRectMake(rect.getMinX()-size/2, rect.getMaxY()-size/2, sz.width+size, size);
	CCRect rl = CCRectMake(rect.getMinX()-size/2, rect.getMinY()-size/2, size, sz.height+size);

	ccColor4B clr = sColors[idx%3];
	idx++;

	CCERectsNode* obj = (CCERectsNode*) rectsNode;
	obj->add(rb, clr);
	obj->add(rr, clr);
	obj->add(rt, clr);
	obj->add(rl, clr);

	CCArray* children = node->getChildren();
	if(children!=NULL) {
		CCObject* child;
        CCARRAY_FOREACH(children, child)
        {
            CCNode* pChild = (CCNode*) child;
            idx = buildNodeFrame(rectsNode, pChild, size, idx);
        }
	}
	return idx;
}

CCValue CCEDirector::CALLNAME(createNodeFrame)(CCValueArray& params) {
	CCNode* obj = ccvpObject(params, 0, CCNode);
	int size = ccvpInt(params, 1);
	if(size==0)size=4;
	
	CCERectsNode* rectsNode = CCERectsNode::create();
	if(obj!=NULL) {
		buildNodeFrame(rectsNode, obj, size, 0);
	}

	return CCValue::objectValue(rectsNode);
}

CCValue CCEDirector::CALLNAME(resetApplication)(CCValueArray& params) {
	CCApplication::sharedApplication()->resetApplication();
	return CCValue::nullValue();
}