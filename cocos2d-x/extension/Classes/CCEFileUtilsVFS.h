#ifndef __CC_FILEUTILS_VFS_H__
#define __CC_FILEUTILS_VFS_H__

#include "platform/CCFileUtils.h"
#include <string>
#include <vector>

USING_NS_CC;

class SQLiteVFS;
class CCEFileUtilsVFS : public CCFileUtils
{
    friend class CCFileUtils;
public:
    CCEFileUtilsVFS(SQLiteVFS* vfs);
	virtual ~CCEFileUtilsVFS();

public:
	void install();

public:
    /* override funtions */
	virtual bool init();
	virtual unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);
	virtual unsigned char* getFileDataFromZip(const char* pszZipFilePath, const char* pszFileName, unsigned long * pSize);
    virtual std::string getWritablePath();
    virtual bool isFileExist(const std::string& strFilePath);
    
protected:
	SQLiteVFS* m_vfs;
};

#endif    // __CC_FILEUTILS_WIN32_H__

