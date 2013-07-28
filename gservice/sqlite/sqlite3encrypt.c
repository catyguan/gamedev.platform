#include <stdlib.h>
#include <memory.h>
#include "sqlite3.h"
#include "blowfish.h"

#ifndef DB_KEY_LENGTH_BYTE         /*密钥长度*/
#define DB_KEY_LENGTH_BYTE   16   /*密钥长度*/
#endif
 
#ifndef DB_KEY_PADDING             /*密钥位数不足时补充的字符*/
#define DB_KEY_PADDING       0x33 /*密钥位数不足时补充的字符*/
#endif

void* ext_createCodec(const void *pKey, int nKeyLen)
{
	unsigned char * hKey = NULL;
    int j;

	if (pKey == NULL || nKeyLen == 0)
    {
		return NULL;
    }

	hKey = (unsigned char*) sqlite3_malloc(DB_KEY_LENGTH_BYTE + 1);
    if (hKey == NULL)
    {
		return NULL;
    }
	
	hKey[DB_KEY_LENGTH_BYTE] = 0;
	if (nKeyLen < DB_KEY_LENGTH_BYTE)
    {
		memcpy(hKey, pKey, nKeyLen);
		j = DB_KEY_LENGTH_BYTE - nKeyLen;
		memset(hKey + nKeyLen, DB_KEY_PADDING, j);
	}
    else
    {
		memcpy(hKey, pKey, DB_KEY_LENGTH_BYTE);
	}
	return hKey;
}

int ext_encrypt(void * pData, unsigned int data_len, void* key)
{
	unsigned char* p = (unsigned char*) pData;
	unsigned int i;
	unsigned char val;
	for (i = 0; i < data_len; i++)
	{
		val = ~(*p);
		*p = val;
		p++;
	}
	return 0;
}

int ext_decrypt(void * pData, unsigned int data_len, void* key)
{
	unsigned char* p = (unsigned char*) pData;
	unsigned int i;
	unsigned char val;
	for (i = 0; i < data_len; i++)
	{
		val = ~(*p);
		*p = val;
		p++;
	}
	return 0;
}