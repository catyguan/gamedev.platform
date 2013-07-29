#include <stdlib.h>
#include <memory.h>
#include "sqlite3.h"
#include "blowfish.h"

#define BLOWFISH_MODE	0

static void ext_createKey(unsigned char* key, int nkey)
{	
	int i;
	for(i=0;i<nkey;i++) {
		*key = *key+i+1;
	}
	return;
}

void* ext_createCodec(const void *pKey, int nKeyLen)
{
	Blowfish* codec = NULL;
	unsigned char key[MAX_KEY_SIZE];

	if(nKeyLen>MAX_KEY_SIZE)
	{
		return NULL;	
	}
	if (pKey == NULL || nKeyLen == 0)
    {
		return NULL;
    }
	memcpy(key, pKey, nKeyLen);
	ext_createKey(key, nKeyLen);
	
	codec = (Blowfish*) sqlite3_malloc(sizeof(Blowfish));
    if (codec == NULL)
    {
		return NULL;
    }
	
	if(BlowFishInit(codec, key, nKeyLen)<=0) {
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