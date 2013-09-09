#ifndef  __CCO_FILE_H__
#define  __CCO_FILE_H__

#include "cocos2d.h"

USING_NS_CC;

class CCO4File : public CCObject
{
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(exists)
	CC_DECLARE_CALL(load)
	CC_DECLARE_CALL(save)
	CC_DECLARE_CALL(delete)
	CC_DECLARE_CALLS_END
};

/*
class SQLiteVFS;
class CCO4VFSFile : public CCObject
{
public:
	void init(SQLiteVFS* vfs);

protected:
	SQLiteVFS* m_vfs;
	
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(exists)
	CC_DECLARE_CALL(load)
	CC_DECLARE_CALL(save)
	CC_DECLARE_CALL(delete)
	CC_DECLARE_CALLS_END
};
*/

#endif