#include "CCEFileUtilsVFS.h"
#include "../sqlite/SQLiteVFS.h"

using namespace std;

USING_NS_CC;

CCEFileUtilsVFS::CCEFileUtilsVFS(SQLiteVFS* vfs)
{
	m_vfs = vfs;
}

CCEFileUtilsVFS::~CCEFileUtilsVFS()
{
	
}

void CCEFileUtilsVFS::install()
{
	if(s_sharedFileUtils!=NULL) {
		CC_SAFE_DELETE(s_sharedFileUtils);
	}
    s_sharedFileUtils = this;
    this->init();
}

bool CCEFileUtilsVFS::init()
{
    m_strDefaultResRootPath = "/";
    return CCFileUtils::init();
}

unsigned char* CCEFileUtilsVFS::getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{
	unsigned char * pBuffer = NULL;
	CCAssert(m_vfs,"invalid VFS");
    CCAssert(pszFileName != NULL && pSize != NULL && pszMode != NULL, "Invalid parameters.");
    *pSize = 0;
    do
    {
        // read the file from vfs
        std::string fullPath = fullPathForFilename(pszFileName);
		int size = 0;
		byte* tmp = m_vfs->fileRead(fullPath.c_str(), &size);		
		if(tmp!=NULL) {
			*pSize = size;
			pBuffer = new unsigned char[size];
			memcpy(pBuffer, tmp, size);
			delete[] tmp;
		}
    } while (0);
    
    if (! pBuffer)
    {
        std::string msg = "Get data from file(";
        msg.append(pszFileName).append(") failed!");
        
        CCLOG("%s", msg.c_str());
    }
    return pBuffer;
}

unsigned char* CCEFileUtilsVFS::getFileDataFromZip(const char* pszZipFilePath, const char* pszFileName, unsigned long * pSize)
{
	CCAssert(0, "not implements");
	return NULL;
}

bool CCEFileUtilsVFS::isFileExist(const std::string& strFilePath)
{
    if (0 == strFilePath.length())
    {
        return false;
    }
    CCAssert(m_vfs, "invalid VFS");
    std::string strPath = strFilePath;
    if (!isAbsolutePath(strPath))
    { // Not absolute path, add the default root path at the beginning.
        strPath.insert(0, m_strDefaultResRootPath);
    }
	return m_vfs->fileExists(strPath.c_str());
}

string CCEFileUtilsVFS::getWritablePath()
{
	return "/data/";    
}


