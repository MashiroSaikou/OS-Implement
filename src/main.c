// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials 
#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "page.h"
#include "kernel_malloc.h"
#include "common.h"
#include "kheap.h"

int main(struct multiboot *mboot_ptr)
{
  // All our initialisation calls will go in here.
	monitor_clear();
	monitor_write("hello, word!\n");
	init_descriptor_tables();
	init_paging();
	init_timer(1000);
	printf("finish paging\n");
	uint32 b = _malloc_s(1);
	uint32 c = _malloc_s(1);
	uint32 d = _malloc_s(1);
	printf("b:%x, c:%x, d:%x\n", b, c, d);
	kfree(b);
	
	return 0;
}