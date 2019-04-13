#include "common.h"

// Write a byte out to the specified port.
void outb(uint16 port, uint8 value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8 inb(uint16 port)
{
   uint8 ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

uint16 inw(uint16 port)
{
   uint16 ret;
   asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

void * memset(void * h, int v, size_t l){
	char* t_h;
	for(t_h = h; l > 0; l --) {
		*t_h++ = v;
	}
	return h;
}

void _panic(const char *fmt, ...)
{
  va_list ap;

  (void)printf("\n***** Kernel panic! *****\n");

  va_start(ap, fmt);
  (void)vprintf(fmt, ap);
  va_end(ap);

  (void)printf("\n*************************\n");

  for (;;)
    ;
  /* NOTREACHED */
}