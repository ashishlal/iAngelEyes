/*
 *  NTIMalloc.h
 *  iAngelEyes
 *
 *  Created by Ashish Lal on 07/06/10.
 *  Copyright 2010 NetTech India. All rights reserved.
 *
 */
#ifndef __NTI_MALLOC_H__
#define __NTI_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define MALLOC_DEBUG
	void *NTIMalloc(unsigned int sz, char *file, int line);
	void NTIFree(void *ptr, char *file, int line);
	int NTIMallocCheck(char *file, int line);
	int NTIMallocCheckPtr(void * ptr, char *file, int line);
	void NTIMallocPrint();

#ifdef __cplusplus
}
#endif
		
#if 0
#undef malloc
#undef free
#define malloc(x) NTIMalloc((x), __FILE__, __LINE__)
#define free(x) NTIFree((x), __FILE__, __LINE__)
#endif

#endif /* __NTI_MALLOC_H__ */