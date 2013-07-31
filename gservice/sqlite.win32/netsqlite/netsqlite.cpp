// 这是主 DLL 文件。

#include "stdafx.h"

#include <string>
#include "netsqlite.h"

using namespace System::IO;
using namespace Runtime::InteropServices;

std::string marshalString(System::String^ s) {      
	array<Byte>^ ar = System::Text::Encoding::UTF8->GetBytes(s);
	IntPtr p = Marshal::AllocHGlobal(ar->Length);
	Marshal::Copy(ar, 0, p, ar->Length);
	std::string r((const char*) p.ToPointer(),ar->Length);
	Marshal::FreeHGlobal(p);
	return r;
}
String^ marshalString(const char* s, int len)
{	
	array< Byte >^ byteArray = gcnew array< Byte >(len);
	Marshal::Copy((IntPtr)(char*)s,byteArray, 0, len);
	return System::Text::Encoding::UTF8->GetString(byteArray);
}
String^ marshalString(const char* s)
{
	int len = strlen(s);
	return marshalString(s, len);
}

namespace netsqlite {
	SQLite::SQLite()
	{
		m_db = NULL;
		m_stmt = NULL;
	}

	SQLite::~SQLite()
	{
		close();
	}
	void SQLite::assertOpen()
	{
		if(!isOpen())throw gcnew IOException("sqlite not open");
	}
	void SQLite::assertStmt()
	{
		if(m_stmt==NULL)throw gcnew IOException("stmt not prepare");
	}
	String^ SQLite::error()
	{
		assertOpen();
		return marshalString(sqlite3_errmsg(m_db));
	}
	String^ SQLite::error(int code)
	{
		if(code==SQLITE_ERROR)return error();
		return marshalString(sqlite3_errstr(code));
	}

	void SQLite::open(String^ name)
	{
		if(isOpen()) {
			throw gcnew IOException("sqlite already open");
		}	
		std::string sname = marshalString(name);
		pin_ptr<sqlite3*> p = &m_db;
		int rc = sqlite3_open(sname.c_str(), p);
		if(rc!=SQLITE_OK) {
			if(m_db!=NULL)close();
			throw gcnew IOException(error(rc));
		}
	}

	void SQLite::closeStmt()
	{
		if(m_stmt!=NULL) {
			sqlite3_finalize(m_stmt);
			m_stmt = NULL;
		}
	}

	void SQLite::close()
	{
		closeStmt();
		if(m_db!=NULL) {
			sqlite3_close(m_db);
			m_db = NULL;
		}
	}

	void SQLite::key(String^ k)
	{
		assertOpen();
		std::string skey = marshalString(k);
		int rc = sqlite3_key(m_db, skey.c_str(), skey.length());
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}

	void SQLite::rekey(String^ k)
	{
		assertOpen();
		std::string skey = marshalString(k);
		int rc = sqlite3_rekey(m_db, skey.c_str(), skey.length());
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}

	void SQLite::prepare(String^ sql)
	{
		assertOpen();
		if(m_stmt!=NULL)throw gcnew IOException("stmt already prepare");
		std::string ssql = marshalString(sql);
		pin_ptr<sqlite3_stmt*> p = &m_stmt;
		int rc = sqlite3_prepare_v2(m_db, ssql.c_str(), ssql.length(), p, NULL);
		if(rc!=SQLITE_OK) {
			if(m_stmt!=NULL)closeStmt();
			throw gcnew IOException(error(rc));
		}
	}

	void SQLite::bindBlob(int idx, array<unsigned char,1>^ v)
	{
		assertStmt();
		if(v==nullptr) {
			bindNull(idx);
			return;
		}
		int size = v->Length;
		IntPtr intPtr = Marshal::AllocHGlobal(size);
		Marshal::Copy(v, 0, intPtr, size);
		int rc = sqlite3_bind_blob(m_stmt,idx, (const char*) (void*) intPtr, size, SQLITE_TRANSIENT);
		Marshal::FreeHGlobal(intPtr);
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}
	void SQLite::bindDouble(int idx,double v)
	{
		assertStmt();
		int rc = sqlite3_bind_double(m_stmt,idx, v);
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}
	void SQLite::bindInt(int idx, int v)
	{
		assertStmt();
		int rc = sqlite3_bind_int(m_stmt,idx, v);
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}
	void SQLite::bindLong(int idx, Int64 v)
	{
		int rc = sqlite3_bind_int64(m_stmt,idx, v);
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}
	void SQLite::bindNull(int idx)
	{
		assertStmt();
		int rc = sqlite3_bind_null(m_stmt, idx);
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}
	void SQLite::bindString(int idx, String^ v)
	{
		bindText(idx, v);
	}
	void SQLite::bindText(int idx, String^ v)
	{
		assertStmt();
		if(v==nullptr) {
			bindNull(idx);
			return;
		}
		std::string sv = marshalString(v);
		int rc = sqlite3_bind_text(m_stmt,idx,sv.c_str(), sv.length(), SQLITE_TRANSIENT);
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}
	bool SQLite::queryNext(IDictionary<String^,Object^>^ rs, bool blobSize)
	{
		assertStmt();
		int rc = sqlite3_step(m_stmt);
		if(!(rc==SQLITE_ROW || rc==SQLITE_DONE)) {
			if(rc==SQLITE_ERROR) {
				throw gcnew IOException(error());
			} else {
				throw gcnew IOException(error(rc));
			}
		}
		if(rc==SQLITE_DONE) {
			return false;
		}
		if(rc==SQLITE_ROW) {
			if(rs!=nullptr) {
				int nCol = sqlite3_column_count(m_stmt);
				for(int i=0; i<nCol; i++){
					String^ name = marshalString(sqlite3_column_name(m_stmt, i));
					switch(sqlite3_column_type(m_stmt,i))
					{
					case SQLITE_INTEGER:
						rs->Add(name, sqlite3_column_int64(m_stmt,i));
						break;
					case SQLITE_FLOAT:
						rs->Add(name, sqlite3_column_double(m_stmt,i));
						break;
					case SQLITE_TEXT: {
							int slen = sqlite3_column_bytes(m_stmt, i);
							String^ sval = marshalString((const char*) sqlite3_column_text(m_stmt,i), slen);
							rs->Add(name, sval);
							break;
						}
					case SQLITE_BLOB: {
							int blen = sqlite3_column_bytes(m_stmt, i);
							if(blobSize) {
								rs->Add(name, blen);
							} else {
								array< Byte >^ byteArray = gcnew array< Byte >(blen);
								Marshal::Copy((IntPtr)(char*)sqlite3_column_blob(m_stmt,i),byteArray, 0, blen);
								rs->Add(name, byteArray);
							}
							break;
						}
					case SQLITE_NULL:
						rs->Add(name,nullptr);
						break;						
					}
				}				
			}	
			return true;
		}
		throw gcnew IOException(error(rc));
	}

	int SQLite::execute()
	{
		assertStmt();
		int rc = sqlite3_step(m_stmt);
		if(rc==SQLITE_DONE) {
			return sqlite3_total_changes(m_db);
		}
		throw gcnew IOException(error(rc));
	}

	void SQLite::reset()
	{
		assertStmt();
		int rc = sqlite3_reset(m_stmt);
		if(rc!=SQLITE_OK) {
			throw gcnew IOException(error(rc));
		}
	}

	void SQLite::doExec(String^ sql)
	{
		prepare(sql);
		execute();
		closeStmt();
	}
}