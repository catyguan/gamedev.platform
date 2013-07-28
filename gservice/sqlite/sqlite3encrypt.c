#include <stdlib.h>
#include <memory.h>
#include "sqlite3.h"
#include "blowfish.h"

#define BLOWFISH_MODE	0

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

#define CONFUSION

int ext_encrypt(void * pData, unsigned int data_len, void* codec)
{
	Blowfish* bf = (Blowfish*) codec;
	Encrypt(bf, (unsigned char*) pData, data_len, (unsigned char*) pData, data_len, BLOWFISH_MODE);
#ifdef CONFUSION
	if(1) {
		unsigned char* p = (unsigned char*) pData;
		unsigned int i;
		unsigned char val;
		for (i = 0; i < data_len; i++)
		{
			val = *p;
			if(val!=0 && val!=0xFF) {
				*p = ~val;
			}
			p++;
		}
	}
#endif	
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
		val = *p;
		if(val!=0 && val!=0xFF) {
			*p = ~val;
		}
		p++;
	}
#endif
	Decrypt(bf, (unsigned char*) pData, data_len, (unsigned char*) pData, data_len, BLOWFISH_MODE);
	return 0;
}