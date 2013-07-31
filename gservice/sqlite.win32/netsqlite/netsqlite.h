#pragma once

#include <vcclr.h>
#define SQLITE_HAS_CODEC
#include "../../sqlite/sqlite3.h"

using namespace System;
using namespace System::Collections::Generic;

namespace netsqlite {

	public ref class SQLite
	{
	public:
		SQLite();
		~SQLite();

	public:		
		void open(String^ name);
		bool isOpen(){return m_db!=NULL;}
		void close();

		void key(String^ k);
		void rekey(String^ k);

		void prepare(String^ sql);
		void bindBlob(int idx, array<unsigned char,1>^ v);
		void bindDouble(int idx,double v);
		void bindInt(int idx, int v);
		void bindLong(int idx, Int64 v);
		void bindNull(int idx);
		void bindString(int idx, String^ v);
		void bindText(int idx, String^ v);
		bool queryNext(IDictionary<String^,Object^>^ rs, bool blobSize);		
		int execute();
		void reset();
		void closeStmt();

		void doExec(String^ sql);

	protected:
		void assertOpen();
		void assertStmt();

		String^ error();
		String^ error(int code);

	protected:
		sqlite3* m_db;
		sqlite3_stmt* m_stmt;
	};
}
