// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials 
#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "page.h"
#include "kernel_malloc.h"
#include "common.h"

int main(struct multiboot *mboot_ptr)
{
  // All our initialisation calls will go in here.
	monitor_clear();
	monitor_write("hello, word!\n");
	init_descriptor_tables();
	init_paging();

	uint32 b = _malloc_s(8);
	printf("b:%x\n", b);
	kfree(b);
	//init_timer(1000);


	// uint32 *ptr = (uint32*)0xA0000000;
	// uint32 do_page_fault = *ptr;
	return 0;
}