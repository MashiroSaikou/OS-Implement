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

void * memset(void* h, int v, size_t l){
	char* t_h;
	for(t_h = h; l > 0; l --) {
		*t_h = v;
    t_h ++;
	}
	return h;
}

void* memcpy(void* dest, const void* src, size_t count)
{
	char* dst8 = (char*)dest;
	char* src8 = (char*)src;

	while (count--)
		*dst8++ = *src8++;

	return dest;
}

size_t strlen(const char* s)
{
	size_t l = 0;
	for (l = 0; s[l] != '\0'; l++)
		;
	return (l);
}

int strcmp(const char* s1, const char* s2)
{
	// stolen from freebsd
  while (*s1 == *s2++)
  if (*s1++ == '\0')
    return (0);
  return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
}

char* strcpy(char* dest, const char* src)
{
  if (dest == NULL || src == NULL) _panic("strcpy error\n");
  char* d = dest;
  do {
    *d++ = *src++;
  } while (*src != '\0');
  return d;
}

char* strcat(char* dest, const char* src)
{
  if (dest == NULL || src == NULL) _panic("strcpy error\n");
  char* d = dest;
  while (*dest != '\0') {
      *dest = *dest++;
  }

  do {
      *dest++ = *src++;
  } while (*src != '\0');
  return d;
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