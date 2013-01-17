#include "CCO4File.h"

#include "platform\CCFileUtils.h"
#include "cocoa/CCValueSupport.h"

CC_BEGIN_CALLS(CCO4File, CCObject)
	CC_DEFINE_CALL(CCO4File, getStoreFile)
CC_END_CALLS(CCO4File, CCObject)

CCValue CCO4File::CALLNAME(getStoreFile)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	std::string path = CCFileUtils::sharedFileUtils()->getWriteablePath();	
	std::string r = path+name;
	return CCValue::stringValue(r);
}
