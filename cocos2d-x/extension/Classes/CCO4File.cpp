#include "CCO4File.h"

#include "platform\CCFileUtils.h"
#include "cocoa/CCValueSupport.h"

#include "CCEUtil.h"

CC_BEGIN_CALLS(CCO4File, CCObject)
	CC_DEFINE_CALL(CCO4File, exists)
	CC_DEFINE_CALL(CCO4File, load)
	CC_DEFINE_CALL(CCO4File, save)
	CC_DEFINE_CALL(CCO4File, delete)
CC_END_CALLS(CCO4File, CCObject)

std::string vpath(std::string name)
{
	if(name.length()>1 && name[0]=='/') {
		name = name.substr(1);
	}
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();
	return path+name;
}

CCValue CCO4File::CALLNAME(exists)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	name = vpath(name);
	CCFileUtils* fs = CCFileUtils::sharedFileUtils();
	return CCValue::booleanValue(fs->isFileExist(name));
}

CCValue CCO4File::CALLNAME(load)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	name = vpath(name);
	
	int size = 0;
	char* buf = NULL;
	do {
		// read the file from hardware
		FILE *fp = fopen(name.c_str(), "rb");
		CC_BREAK_IF(!fp);
        
		fseek(fp,0,SEEK_END);
		size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		buf = new char[size];
		fread(buf,sizeof(char), size,fp);
		fclose(fp);
    } while (0);
    
    if (! buf)
    {
        std::string msg = "file.load(";
        msg.append(name).append(") failed!");        
        CCLOG("%s", msg.c_str());
    }
    
	std::string r(buf, size);
	delete[] buf;
	return CCValue::stringValue(r);
}

CCValue CCO4File::CALLNAME(save)(CCValueArray& params)
{
	std::string name = ccvpString(params,0);
	std::string content = ccvpString(params, 1);
	name = vpath(name);
	
	// read the file from hardware
	FILE *fp = fopen(name.c_str(), "wb");
	bool r = false;
	if(fp) {
		size_t wcount = fwrite(content.c_str(), content.length(),1, fp);
		fclose(fp);	
		r = true;
	}

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
	name = vpath(name);
	BOOL r = ::DeleteFileA(name.c_str());
	return CCValue::booleanValue(r==TRUE);
}