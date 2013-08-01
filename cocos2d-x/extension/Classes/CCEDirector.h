#ifndef  __CCE_DIRECTOR_H__
#define  __CCE_DIRECTOR_H__

#include "cocos2d.h"

USING_NS_CC;

class CCELuaApplication;
class CCEDirector
{
public:
	CCEDirector(CCELuaApplication* app);
	virtual ~CCEDirector(void);

public:
	CCValue apiCall(std::string name, CCValueArray& ps);

	bool apiStartup();
	void apiShutdown();
	CCValue apiObjectCall(std::string name, CCValueArray& ps);
	CCValue apiObjectCall(std::string name);
	CCValue apiObjectCall(std::string name, CCValue p1);
	CCValue apiObjectCall(std::string name, CCValue p1, CCValue p2);
	CCValue apiObjectCall(std::string name, CCValue p1, CCValue p2, CCValue p3);	

public:
	void install();
    static CCEDirector* sharedDirector(void);
    static void purgeSharedDirector(void);

protected:
	CCELuaApplication* m_app;
};

#endif  // __CCE_DIRECTOR_H__