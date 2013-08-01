#include "SQLiteVFS.h"

#define SQLITE_HAS_CODEC
#include "../sqlite/sqlite3.h"

#include "../Classes/CCEUtil.h"

USING_NS_CC;

std::string error(const char* name, int rc)
{
	return StringUtil::format("%s fail [%d] '%s'",name, rc, sqlite3_errstr(rc));
}

std::string error(sqlite3* db, const char* name, int rc)
{
	if(rc!=SQLITE_ERROR)return error(name, rc);
	return StringUtil::format("%s error '%s'",name, sqlite3_errmsg(db));
}

// SQLiteVFS
SQLiteVFS::SQLiteVFS()
{
	
}

SQLiteVFS::~SQLiteVFS()
{
	close();
}

bool SQLiteVFS::addSQLite(const char* fileName,const char* key, bool writable)
{
	int rc = SQLITE_OK;
	SQLiteDB db;
	db.db = NULL;
	db.selectStmt = NULL;
	db.stmt = NULL;
	db.writable = writable;

	int flags = writable? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE: SQLITE_OPEN_READONLY;
	rc = sqlite3_open_v2(fileName, &db.db,flags,NULL);
	if(rc!=SQLITE_OK) {
		std::string msg = error("sqlite_open", rc);
		CCLOG(msg.c_str());
		return false;
	}	
	if(key!=NULL) {
		rc = sqlite3_key(db.db, key, strlen(key));
		if(rc!=SQLITE_OK) {
			std::string msg = error("sqlite_key", rc);
			CCLOG(msg.c_str());
			closeDB(&db);
			return false;
		}
	}
	if(writable) {
		const char* sql = "create table if not exists files(path TEXT PRIMARY KEY, content BLOB, time INTEGER);";
		char * pErrMsg = 0;
		sqlite3_exec( db.db, sql, NULL, 0, &pErrMsg );
		if(pErrMsg!=NULL) {
			std::string msg = StringUtil::format("create table fail '%s'", pErrMsg);
			CCLOG(msg.c_str());
			sqlite3_free(pErrMsg);
			closeDB(&db);
			return false;
		}
	}
	m_dbs.push_back(db);
	return true;
}

bool SQLiteVFS::prepareSelect(SQLiteDB* p, const char* path)
{	
	int rc = 0;
	if(p->selectStmt!=NULL) {
		sqlite3_reset(p->selectStmt);
	} else {
		const char* sql = "SELECT path,content,time FROM files WHERE path = ?";
		rc = sqlite3_prepare_v2(p->db, sql, strlen(sql), &p->selectStmt, NULL);
		if(rc!=SQLITE_OK) {
			std::string msg = error(p->db, "prepareSelect", rc);
			CCLOG(msg.c_str());
			return false;
		}
	}
	rc = sqlite3_bind_text(p->selectStmt, 1, path, strlen(path), SQLITE_TRANSIENT);
	if(rc!=SQLITE_OK) {
		std::string msg = error(p->db, "prepareSelect", rc);
		CCLOG(msg.c_str());
		return false;
	}
	return true;
}

bool SQLiteVFS::fileExists(const char* path)
{
	int rc = SQLITE_OK;
	std::vector<SQLiteDB>::const_iterator it = m_dbs.begin();
	while(it!=m_dbs.end()) {
		SQLiteDB* p = (SQLiteDB*) &*it;
		if(p->db!=NULL) {			
			do {
				if(!prepareSelect(p, path)) {
					break;
				}				
				rc = sqlite3_step(p->selectStmt);
				if(rc==SQLITE_ROW) {
					sqlite3_reset(p->selectStmt);
					return true;
				}
				if(rc!=SQLITE_DONE) {
					std::string msg = error(p->db, "fileExists.step", rc);
					CCLOG(msg.c_str());
					break;
				}
			}while(0);
		}
		it++;
	}
	return false;
}

int SQLiteVFS::fileSize(const char* path)
{
	int rc = SQLITE_OK;
	std::vector<SQLiteDB>::const_iterator it = m_dbs.begin();
	while(it!=m_dbs.end()) {
		SQLiteDB* p = (SQLiteDB*) &*it;
		if(p->db!=NULL) {			
			do {
				if(!prepareSelect(p, path)) {
					break;
				}				
				rc = sqlite3_step(p->selectStmt);
				if(rc==SQLITE_ROW) {
					int r = sqlite3_column_bytes(p->selectStmt, 1);
					sqlite3_reset(p->selectStmt);
					return r;
				}
				if(rc!=SQLITE_DONE) {
					std::string msg = error(p->db, "fileSize.step", rc);
					CCLOG(msg.c_str());
					break;
				}
			}while(0);
		}
		it++;
	}
	return -1;
}

byte* SQLiteVFS::fileRead(const char* path, int* size)
{
	int rc = SQLITE_OK;
	std::vector<SQLiteDB>::const_iterator it = m_dbs.begin();
	while(it!=m_dbs.end()) {
		SQLiteDB* p = (SQLiteDB*) &*it;
		if(p->db!=NULL) {			
			do {
				if(!prepareSelect(p, path)) {
					break;
				}				
				rc = sqlite3_step(p->selectStmt);
				if(rc==SQLITE_ROW) {
					int len = sqlite3_column_bytes(p->selectStmt, 1);
					byte* r = new byte[len];
					const void* data = sqlite3_column_blob(p->selectStmt, 1);
					memcpy(r, data, len);
					sqlite3_reset(p->selectStmt);
					*size = len;
					return r;
				}
				if(rc!=SQLITE_DONE) {
					std::string msg = error(p->db, "fileRead.step", rc);
					CCLOG(msg.c_str());
					break;
				}
			}while(0);
		}
		it++;
	}
	*size = 0;
	return NULL;
}

int SQLiteVFS::fileWrite(const char* path, byte* buff, int size, long time)
{
	int rc = SQLITE_OK;
	std::vector<SQLiteDB>::const_iterator it = m_dbs.begin();
	while(it!=m_dbs.end()) {
		SQLiteDB* p = (SQLiteDB*) &*it;
		if(p->db!=NULL && p->writable) {			
			do {
				if(p->stmt!=NULL) {
					sqlite3_finalize(p->stmt);
					p->stmt = NULL;
				}
				const char* sql = "INSERT OR REPLACE INTO files VALUES(?, ?, ?);";
				rc = sqlite3_prepare_v2(p->db, sql, strlen(sql), &p->stmt, NULL);
				if(rc!=SQLITE_OK) {
					std::string msg = error(p->db, "fileWrite.prepare", rc);
					CCLOG(msg.c_str());
					break;
				}	
				rc = sqlite3_bind_text(p->stmt, 1, path, strlen(path), SQLITE_TRANSIENT);
				if(rc!=SQLITE_OK) {
					std::string msg = error(p->db, "fileWrite.bind", rc);
					CCLOG(msg.c_str());
					break;
				}
				rc = sqlite3_bind_blob(p->stmt, 2, buff, size, SQLITE_TRANSIENT);
				if(rc!=SQLITE_OK) {
					std::string msg = error(p->db, "fileWrite.bind", rc);
					CCLOG(msg.c_str());
					break;
				}				
				rc = sqlite3_bind_int64(p->stmt, 3, time);
				if(rc!=SQLITE_OK) {
					std::string msg = error(p->db, "fileWrite.bind", rc);
					CCLOG(msg.c_str());
					break;
				}				
				rc = sqlite3_step(p->stmt);
				if(rc!=SQLITE_DONE) {
					std::string msg = error(p->db, "fileWrite.step", rc);
					CCLOG(msg.c_str());
					break;
				}
				return size;
			}while(0);
		}		
		it++;
	}
	return -1;
}

bool SQLiteVFS::fileDelete(const char* path,bool all)
{
	bool r = false;
	int rc = SQLITE_OK;
	std::vector<SQLiteDB>::const_iterator it = m_dbs.begin();
	while(it!=m_dbs.end()) {
		SQLiteDB* p = (SQLiteDB*) &*it;
		if(p->db!=NULL && p->writable) {			
			do {
				if(p->stmt!=NULL) {
					sqlite3_finalize(p->stmt);
					p->stmt = NULL;
				}
				const char* sql = "DELETE FROM files WHERE path = ?";
				rc = sqlite3_prepare_v2(p->db, sql, strlen(sql), &p->stmt, NULL);
				if(rc!=SQLITE_OK) {
					std::string msg = error(p->db, "fileDelete.prepare", rc);
					CCLOG(msg.c_str());
					break;
				}	
				rc = sqlite3_bind_text(p->stmt, 1, path, strlen(path), SQLITE_TRANSIENT);
				if(rc!=SQLITE_OK) {
					std::string msg = error(p->db, "fileDelete.bind", rc);
					CCLOG(msg.c_str());
					break;
				}				
				rc = sqlite3_step(p->stmt);
				if(rc!=SQLITE_DONE) {
					std::string msg = error(p->db, "fileDelete.step", rc);
					CCLOG(msg.c_str());
					break;
				}
				if(sqlite3_total_changes(p->db)!=0) {
					r = true;
				}
			}while(0);
		}
		if(!all)break;
		it++;
	}
	return r;
}

void SQLiteVFS::closeStmt(SQLiteDB* db)
{
	if(db->selectStmt!=NULL) {
		sqlite3_finalize(db->selectStmt);
		db->selectStmt = NULL;
	}
	if(db->stmt!=NULL) {
		sqlite3_finalize(db->stmt);
		db->stmt = NULL;
	}
}

void SQLiteVFS::closeDB(SQLiteDB* db)
{
	closeStmt(db);
	if(db->db!=NULL) {
		sqlite3_close(db->db);
		db->db = NULL;
	}
}

void SQLiteVFS::close()
{
	std::vector<SQLiteDB>::const_iterator it = m_dbs.begin();
	while(it!=m_dbs.end()) {
		closeDB((SQLiteDB*) &*it);
		it++;
	}
}
