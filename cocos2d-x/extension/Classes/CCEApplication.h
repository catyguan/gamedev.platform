#ifndef  __CCE_APPLICATION_H__
#define  __CCE_APPLICATION_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEApplication : public CCObject
{
public:
    CCEApplication();
    virtual ~CCEApplication();

public:
	virtual CCObject* createObject(const char* name, const char* type, CCValueArray& ps);

protected:
	virtual CCObject* findChildById(const char* id);

public:   
	void install();
    static CCEApplication* sharedApp(void);
    static void purgeSharedApp(void);    

protected:	

	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(pushScene)
	CC_DECLARE_CALL(replaceScene)
	CC_DECLARE_CALL(popScene)
	CC_DECLARE_CALLS_END

};

#endif // __CCE_APPLICATION_H__

