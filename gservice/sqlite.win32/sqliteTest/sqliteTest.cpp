// sqliteTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#define SQLITE_HAS_CODEC
#include "../../sqlite/sqlite3.h"

static int _callback_exec(void * notused,int argc, char ** argv, char ** aszColName)
{
    int i;
    for ( i=0; i<argc; i++ )
    {
        printf( "%s = %s\r\n", aszColName[i], argv[i] == 0 ? "NUL" : argv[i] );
    }
    return 0;
}

void test(int newdb)
{
	const char * sSQL;
    char * pErrMsg = 0;
    int ret = 0;
    sqlite3 * db = 0;

	if(newdb) {
		::DeleteFileA("encrypt.db");
	}

    //�������ݿ�
    ret = sqlite3_open("encrypt.db", &db);
    //�������
	if(newdb) {
		ret = sqlite3_key( db, "dcg", 3 );
	} else {
		ret = sqlite3_key(db, "abc", 3);
	}
	
    //���ڴ����ݿ��д�����
    sSQL = "create table class(name varchar(20), student);";
    sqlite3_exec( db, sSQL, _callback_exec, 0, &pErrMsg );
    //��������
    sSQL = "insert into class values('mem_52911', 'zhaoyun');";
    sqlite3_exec( db, sSQL, _callback_exec, 0, &pErrMsg );

	if(newdb) {
		ret = sqlite3_rekey( db, "abc", 3);
	}

    //ȡ�����ݲ���ʾ
    sSQL = "select * from class;";
    sqlite3_exec( db, sSQL, _callback_exec, 0, &pErrMsg );

    //�ر����ݿ�
    sqlite3_close(db);
    
	db = 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(1) {
		printf("[new db]\n");
		test(1);
	}
	if(1) {
		printf("[load db]\n");
		test(0);
	}
	if(1) {
		::DeleteFileA("encrypt.db");	    
	}

	getchar();
	return 0;
}

