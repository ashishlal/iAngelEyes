/*
 *  NTIMalloc.c
 *  iAngelEyes
 *
 *  Created by Ashish Lal on 07/06/10.
 *  Copyright 2010 NetTech India. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "NTIMalloc.h"

#define EXTRA_BYTES 100
#define MAX_ARRAY_SZ 1000
#define MAX_FILE_SZ 512

typedef struct MyMalloc {
	
	unsigned int index;
	void *ptr;
	unsigned int size;
	char allocatedAtFile[MAX_FILE_SZ];
	unsigned int allocatedAtLine;
	char freedAtFile[MAX_FILE_SZ];
	unsigned int freedAtLine;
	unsigned char isFree;
	
} MyMalloc_t;


MyMalloc_t MallocArray[MAX_ARRAY_SZ];
int init = 0;
unsigned int num=0;

unsigned char signature = {0xab};
unsigned char sigBytes[100] = {
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab
};
	
void *NTIMalloc(unsigned int sz, char *file, int line)
{
	unsigned int nextFreeIndex=0xffffffff;
	unsigned int mallocSz = (sz + EXTRA_BYTES);
	NTIMallocCheck((char *)__FILE__, __LINE__);
	if(!init) {
		init =1;
		for(int i=0; i < MAX_ARRAY_SZ; i++) {
			
			MallocArray[i].index=i;
			MallocArray[i].ptr = NULL;
			MallocArray[i].size = 0;
			memset((void *)(MallocArray[i].allocatedAtFile), 0, MAX_FILE_SZ);
			memset((void *)(MallocArray[i].freedAtFile), 0, MAX_FILE_SZ);
			MallocArray[i].allocatedAtLine = 0;
			MallocArray[i].freedAtLine = 0;
			MallocArray[i].isFree = 1;
		}
	}
	
	
	for(int i=0; i < MAX_ARRAY_SZ; i++) {
		
		if(MallocArray[i].isFree) {
			nextFreeIndex = i;
			break;
		}
	}
	
	if(nextFreeIndex < MAX_ARRAY_SZ) {
		
		void *ptr = malloc(mallocSz);
		int n = nextFreeIndex;
		if(!ptr) return NULL;
		unsigned char * signaturePtr = ((unsigned char *)ptr) + sz;
		memcpy((void *)signaturePtr, (void *)sigBytes, EXTRA_BYTES);
		
		MallocArray[n].size = sz;
		MallocArray[n].ptr = ptr;
		strcpy((char *)(MallocArray[n].allocatedAtFile), (const char *)file);
		MallocArray[n].allocatedAtLine = line;
		MallocArray[n].isFree = 0;
		num++;
		return ptr;
	}
	
	return NULL;
}

void NTIFree(void *ptr, char *file, int line)
{
	NTIMallocCheck((char *)__FILE__, __LINE__);
	for(int i=0; ((i < MAX_ARRAY_SZ) && (num)); i++) {
		
		unsigned int arrPtr = (unsigned int)(MallocArray[i].ptr);
		if((arrPtr) && (arrPtr == (unsigned int)ptr)) {
			
			
			if(!MallocArray[i].isFree) {
				
				
				MallocArray[i].size = 0;
				//MallocArray[n].ptr = ptr;
				strcpy((char *)(MallocArray[i].freedAtFile), (const char *)file);
				MallocArray[i].freedAtLine = line;
				MallocArray[i].isFree = 1;
				break;
			}
			else {
				printf("!!! Ptr (0x%02x, %d) was freed at %s:%d\n", arrPtr, MallocArray[i].size, 
					   MallocArray[i].freedAtFile, MallocArray[i].freedAtLine);
				printf("!!! Ptr (0x%02x, %d) was allocated at %s:%d\n", arrPtr, MallocArray[i].size, 
					   MallocArray[i].allocatedAtFile, MallocArray[i].allocatedAtLine);
				NTIMallocPrint();
				break;
			}
		}
	}
	return;
}

int NTIMallocCheck(char *file, int line)
{
#ifdef MALLOC_DEBUG
	unsigned int arrPtr =0;
	for(int i=0; i < MAX_ARRAY_SZ; i++) {
		
		arrPtr = (unsigned int)(MallocArray[i].ptr) ;
		if(arrPtr) {
			unsigned int sigPtr = ((unsigned int)arrPtr) + (MallocArray[i].size);
			if(!MallocArray[i].isFree) {
				
				if(!memcmp((const void *)sigPtr, (const void *)sigBytes, EXTRA_BYTES)) 
					return 1;
				else {
					printf("!!! Ptr (0x%02x, %d) is corrupted at %s:%d\n", arrPtr, MallocArray[i].size, file, line);
					printf("!!! Ptr (0x%02x, %d) was allocated at %s:%d\n", arrPtr, MallocArray[i].size, 
						   MallocArray[i].allocatedAtFile, MallocArray[i].allocatedAtLine);
					NTIMallocPrint();
					return 0;
				}
			}
#if 0
			else {
				printf("!!! Ptr (0x%02x, %d) was freed at %s:%d\n", arrPtr, MallocArray[i].size, MallocArray[i].file, MallocArray[i].line);
				NTIMallocPrint();
				return 0;
			}
#endif
		}
	}
#endif
	return 0;
}

int NTIMallocCheckPtr(void *ptr, char *file, int line)
{
	unsigned int arrPtr =0;
	for(int i=0; i < MAX_ARRAY_SZ; i++) {
		
		arrPtr = (unsigned int)(MallocArray[i].ptr) ;
		if((arrPtr) && (arrPtr == (unsigned int)ptr)) {
			unsigned int sigPtr = ((unsigned int)ptr) + (MallocArray[i].size);
			if(!MallocArray[i].isFree) {
				
				if(!memcmp((const void *)sigPtr, (const void *)sigBytes, EXTRA_BYTES)) 
					return 1;
				else {
					printf("!!! Ptr (0x%02x) is corrupted at %s:%d\n", arrPtr, file, line);
					NTIMallocPrint();
					return 0;
				}
			}
#if 0
			else {
				printf("!!! Ptr (0x%02x) was freed at %s:%d\n", arrPtr, MallocArray[i].file, MallocArray[i].line);
				return 0;
			}
#endif
		}
	}
	return 0;
}

void NTIMallocPrint()
{
	for(int i=0; i < MAX_ARRAY_SZ; i++) {
		
		if(MallocArray[i].ptr) {
			if(!MallocArray[i].isFree) {
				
				printf("Ptr (0x%02x, %d) allocated from %s:%d\n", (unsigned int)(MallocArray[i].ptr), MallocArray[i].size,
					   MallocArray[i].allocatedAtFile, MallocArray[i].allocatedAtLine);				
			}
			else {
				printf("!!! Ptr (0x%02x, %d) was allocated at %s:%d\n", (unsigned int)(MallocArray[i].ptr), MallocArray[i].size, 
					   MallocArray[i].allocatedAtFile, MallocArray[i].allocatedAtLine);
				printf("Ptr (0x%02x, %d) was freed at %s:%d\n", (unsigned int)(MallocArray[i].ptr), MallocArray[i].size,
					   MallocArray[i].freedAtFile, MallocArray[i].freedAtLine);
				return ;
			}
		}
	}
	return;
}

