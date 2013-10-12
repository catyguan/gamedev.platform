#include "CCO4File.h"

#include "platform\CCFileUtils.h"
#include "cocoa/CCValueSupport.h"

#include "CCEUtil.h"
#include "platform\CCFileSystem.h"

// CCO4File
CC_BEGIN_CALLS(CCO4File, CCObject)
	CC_DEFINE_CALL(CCO4File, exists)
	CC_DEFINE_CALL(CCO4File, load)
	CC_DEFINE_CALL(CCO4File, save)
	CC_DEFINE_CALL(CCO4File, delete)
CC_END_CALLS(CCO4File, CCObject)

CCValue CCO4File::CALLNAME(exists)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);	
	CCFileSystem* fs = CCFileSystem::sharedFileSystem();
	return CCValue::booleanValue(fs->fileExists(kAppData, name.c_str()));
}

CCValue CCO4File::CALLNAME(load)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	CCFileSystem* fs = CCFileSystem::sharedFileSystem();
	unsigned long size = 0;
	unsigned char* buf = NULL;
	buf = fs->fileRead(kAppData, name.c_str(), &size);	
    
    if (! buf)
    {
        std::string msg = "file.load(";
        msg.append(name).append(") failed!");        
        CCLOG("%s", msg.c_str());
		return CCValue::stringValue("");
    }
    
	std::string r((char*) buf, size);
	delete[] buf;
	return CCValue::stringValue(r);
}

CCValue CCO4File::CALLNAME(save)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	std::string content = ccvpString(params, 1);
	
	unsigned long len = content.length();
	CCFileSystem* fs = CCFileSystem::sharedFileSystem();
	unsigned long wcount = fs->fileWrite(kAppData, name.c_str(), (unsigned char*) content.c_str(),len);
	
	bool r = wcount==len;	

	if(!r) {
		std::string msg = "file.save(";
        msg.append(name).append(") failed!");
        CCLOG("%s", msg.c_str());
	}
	return CCValue::booleanValue(r);
}

CCValue CCO4File::CALLNAME(delete)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	
	CCFileSystem* fs = CCFileSystem::sharedFileSystem();
	bool r = fs->fileDelete(kAppData, name.c_str());
	return CCValue::booleanValue(r);
}
