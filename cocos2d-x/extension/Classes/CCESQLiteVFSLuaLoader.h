#ifndef  __CCE_SQLITE_VSF_LUA_LOADER_H__
#define  __CCE_SQLITE_VSF_LUA_LOADER_H__

#include "cocos2d.h"

USING_NS_CC;

class CCELuaApplication;
class CCESQLiteVFSLuaLoader
{
public:
	CCESQLiteVFSLuaLoader();
	virtual ~CCESQLiteVFSLuaLoader(void);

public:
	bool addVFS(const char* fileName, const char* key, bool writable);
	void close();
	
	void bind(CCELuaApplication* app);	

	static std::string CALLBACK loader_callback(CCELuaApplication* host, void* data, const char* name);

public:

protected:
	class SQLiteVFS* m_vfs;
};

#endif  // __CCE_DIRECTOR_H__