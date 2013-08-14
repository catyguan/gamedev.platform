#ifndef  __CCE_CONFIG_H__
#define  __CCE_CONFIG_H__

#include "cocos2d.h"

USING_NS_CC;

class CCEConfig
{
public:
	CCEConfig();
	virtual ~CCEConfig(void);

public:
	static CCValue get(const char* name);
	static void set(const char* name, CCValue v);
	
	static bool isDebug(){return get("debug").booleanValue();};

protected:
	CCValueMap m_data;
};

#endif  // __CCE_DIRECTOR_H__