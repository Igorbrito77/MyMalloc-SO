#ifndef MY_MALLOC_H_
#define MY_MALLOC_H_ 

#include <stdio.h>
#include <stdlib.h>


void* MyMalloc(unsigned int size); 
void* MyCalloc(unsigned int length, unsigned int size);
int MyMallocFree(void* ptr);
void MyMallocGerency(void);

#endif