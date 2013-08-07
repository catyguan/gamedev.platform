#ifndef  __CCE_APPLICATION_H__
#define  __CCE_APPLICATION_H__

#include "cocos2d.h"
#include <list>

USING_NS_CC;

typedef CCObject* (*CCE_CREATE_OBJECT)(const char* type, CCValueArray& params);

class CCEApplication : public CCObject
{
public:
    CCEApplication();
    virtual ~CCEApplication();

public:
	virtual CCObject* createObject(const char* type, CCValueArray& ps);

	void addObject(CCNode* node){addObject(node->getId().c_str(), node);}
	virtual void addObject(const char* id, CCObject* obj);
	CCObject* getObject(const char* id){return findChildById(id);};
	virtual void removeObject(const char* id);
	void removeObject(CCNode* node){removeObject(node->getId().c_str());};

	void addCreateObjectFunction(CCE_CREATE_OBJECT fun);

protected:	
	virtual CCObject* findChildById(const char* id);

public:   
	void install();
    static CCEApplication* sharedApp(void);
    static void purgeSharedApp(void);

protected:
	virtual void cleanup();

protected:
	CCDictionary objects;
	std::list<CCE_CREATE_OBJECT> coFunctionList;	

	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(createObject)
	CC_DECLARE_CALL(director)	
	CC_DECLARE_CALLS_END

};

#endif // __CCE_APPLICATION_H__

