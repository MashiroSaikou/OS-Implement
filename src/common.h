#ifndef COMMON_H
#define COMMON_H
#include "type.h"

/*
 *the following function is implemented
 *with asm volatile,because we can't use 
 *the standard C library
 */
void outb(uint16 port, uint8 value); /*write a byte to the port*/
uint8 inb(uint16 port); /*read a byte from the port*/
uint16 inw(uint16 port); /*read a word from the port*/

#endif