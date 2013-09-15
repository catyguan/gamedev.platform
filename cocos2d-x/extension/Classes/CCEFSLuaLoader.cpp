#include "CCEFSLuaLoader.h"
#include "platform\CCFileSystem.h"

#include "CCEUtil.h"
#include "CCELuaApplication.h"

USING_NS_CC;

CCEFSLuaLoader::CCEFSLuaLoader()
{
	
}


CCEFSLuaLoader::~CCEFSLuaLoader(void)
{
	
}

void CCEFSLuaLoader::bind(CCELuaApplication* app)
{
	app->setLoader((CCELuaLoader) CCEFSLuaLoader::loader_callback, this);    
}

std::string CCEFSLuaLoader::loader_callback(CCELuaApplication* host, void* data, const char* name)
{
	CCEFSLuaLoader* th = (CCEFSLuaLoader*) data;
	
	std::string path;
	path.append("/").append(name);
	StringUtil::replaceAll(path,".","/");
	path.append(".lua");

	unsigned long size;
	unsigned char* buf = CCFileSystem::sharedFileSystem()->fileRead(kLua, path.c_str(), &size);
	if(buf!=NULL) {
		std::string r((char*)buf, size);
		delete[] buf;
		return r;
	}
	std::string msg = StringUtil::format("LUA - %s(%s) not exists", name, path.c_str());
	throw new std::string(msg);
}
