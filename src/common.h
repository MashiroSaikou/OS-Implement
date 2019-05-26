#ifndef COMMON_H
#define COMMON_H
#include "type.h"
#include "stdarg.h"
#include "kernel_malloc.h"
/*
 *the following function is implemented
 *with asm volatile,because we can't use 
 *the standard C library
 */

void outb(uint16 port, uint8 value); /*write a byte to the port*/
uint8 inb(uint16 port); /*read a byte from the port*/
uint16 inw(uint16 port); /*read a word from the port*/
void * memset(void* h, int v, size_t l);
void * memcpy(void* dest, const void* src, size_t count);
size_t	strlen(const char* s);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
int	strcmp(const char* s1, const char* s2);
int	printf(const char* fmt, ...);
int	vprintf(const char* fmt, va_list ap);
void _panic(const char* fmt, ...);

#endif