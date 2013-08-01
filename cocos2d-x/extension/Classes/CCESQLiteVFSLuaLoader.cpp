#include "CCESQLiteVFSLuaLoader.h"
#include "../sqlite/SQLiteVFS.h"

#include "CCEUtil.h"
#include "CCELuaApplication.h"

USING_NS_CC;

CCESQLiteVFSLuaLoader::CCESQLiteVFSLuaLoader()
{
	m_vfs = NULL;
}


CCESQLiteVFSLuaLoader::~CCESQLiteVFSLuaLoader(void)
{
	close();	
}

bool CCESQLiteVFSLuaLoader::addVFS(const char* fileName, const char* key, bool writable)
{
	if(m_vfs==NULL) {
		m_vfs = new SQLiteVFS();
	}
	return m_vfs->addSQLite(fileName, key, writable);
}

void CCESQLiteVFSLuaLoader::close()
{
	if(m_vfs!=NULL) {
		m_vfs->close();
		delete m_vfs;
		m_vfs = NULL;
	}
}

void CCESQLiteVFSLuaLoader::bind(CCELuaApplication* app)
{
	CC_ASSERT(m_vfs);
	app->setLoader((CCELuaLoader) CCESQLiteVFSLuaLoader::loader_callback, this);    
}

std::string CCESQLiteVFSLuaLoader::loader_callback(CCELuaApplication* host, void* data, const char* name)
{
	CCESQLiteVFSLuaLoader* th = (CCESQLiteVFSLuaLoader*) data;
	
	std::string path;
	path.append("/").append(name);
	StringUtil::replaceAll(path,".","/");
	path.append(".lua");

	int size;
	byte* buf = th->m_vfs->fileRead(path.c_str(), &size);
	if(buf!=NULL) {
		std::string r((char*)buf, size);
		delete[] buf;
		return r;
	}
	std::string msg = StringUtil::format("%s(%s) not exists", name, path.c_str());
	throw new std::string(msg);
}
