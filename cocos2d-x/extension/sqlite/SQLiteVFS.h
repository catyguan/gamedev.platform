#ifndef  __SQLITE_VFS_H__
#define  __SQLITE_VFS_H__

#include "cocos2d.h"

#include <vector>

USING_NS_CC;

typedef struct _SQLiteDB {
	struct sqlite3* db;
	struct sqlite3_stmt* selectStmt;
	struct sqlite3_stmt* stmt;
	bool writable;
} SQLiteDB;

class SQLiteVFS
{
public:
	SQLiteVFS();
	~SQLiteVFS();
    
public:
	bool addSQLite(const char* fileName, const char* key, bool writable);
	bool fileExists(const char* path);
	int fileSize(const char* path);
	byte* fileRead(const char* path, int* size);
	int fileWrite(const char* path, byte* buff, int size,long time);
	bool fileDelete(const char* path,bool all);
	void close();

protected:
	bool prepareSelect(SQLiteDB* p, const char* path);
	void closeStmt(SQLiteDB* db);
	void closeDB(SQLiteDB* db);

protected:
	std::vector<SQLiteDB> m_dbs;
};

#endif