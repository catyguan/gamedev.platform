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
	Blowfish* codec = NULL;
	if (pKey == NULL || nKeyLen == 0)
    {
		return NULL;
    }

	codec = (Blowfish*) sqlite3_malloc(sizeof(Blowfish));
    if (codec == NULL)
    {
		return NULL;
    }
	
	if(BlowFishInit(codec, (unsigned char*) pKey, nKeyLen)<=0) {
		sqlite3_free(codec);
		return NULL;
	}
	return codec;
}

// #define CONFUSION

int ext_encrypt(void * pData, unsigned int data_len, void* codec)
{
	Blowfish* bf = (Blowfish*) codec;
#ifdef CONFUSION
	unsigned char* p = (unsigned char*) pData;
	unsigned int i;
	unsigned char val;
	for (i = 0; i < data_len; i++)
	{
		val = ~(*p);
		*p = val;
		p++;
	}
#endif
	Encrypt(bf, (unsigned char*) pData, data_len, (unsigned char*) pData, data_len, 0);
	return 0;
}

int ext_decrypt(void * pData, unsigned int data_len, void* codec)
{
	Blowfish* bf = (Blowfish*) codec;
#ifdef CONFUSION
	unsigned char* p = (unsigned char*) pData;
	unsigned int i;
	unsigned char val;
	for (i = 0; i < data_len; i++)
	{
		val = ~(*p);
		*p = val;
		p++;
	}
#endif
	Decrypt(bf, (unsigned char*) pData, data_len, (unsigned char*) pData, data_len, 0);
	return 0;
}