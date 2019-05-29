// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials 
#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "page.h"
#include "kernel_malloc.h"
#include "common.h"
#include "kheap.h"
#include "multiboot.h"
#include "fs.h"
#include "initrd.h"
#include "task.h"

struct multboot;
uint32 initial_esp;
extern uint32 placement_address;

int main(struct multiboot *mboot_ptr, uint32 initial_stack)
{
  // All our initialisation calls will go in here.
	monitor_clear();
	monitor_write("hello, word!\n");
	initial_esp = initial_stack;
	init_descriptor_tables();

	asm volatile("sti");	
	init_timer(1000);
	uint32 initrd_location = *((uint32*)mboot_ptr->mods_addr);
	uint32 initrd_end = *(uint32*)(mboot_ptr->mods_addr+4);
	placement_address = initrd_end;
	
	init_paging();
	printf("init multitask\n");

	init_multitask();

	
	//int ret = fork();
	//printf("?????\n");
	// if (ret = 0) {
	// 	for(;;)
	// 		printf("this is child\n");
	// }
	// else {
	// 	for(;;)
	// 		printf("this is parent\n");
	// }
	//printf("parent\n");

	//fs_root = initialise_initrd(initrd_location);
	// printf("finish\n");
	

	// int i = 0;
	// dirent_t *node = 0;
	// while ((node = readdir_fs(fs_root, i)) != 0) {
	// 	printf("find file:%s\n", node->name);
	// 	fs_node_t *fsnode = finddir_fs(fs_root, node->name);

	// 	if((fsnode->flags&0x7) == FS_DIRECTORY) {
	// 		printf("(directory)\n");
	// 	}
	// 	else {
	// 		printf("contents:\"\n");
	// 		char buf[256];
	// 		uint32 sz = read_fs(fsnode, 0, 256, buf);
	// 		int j;
	// 		for (j = 0; j < sz; j++) {
	// 			printf("%c",buf[j]);
	// 		}
	// 		printf("\"\n");
	// 	}

	// 	i ++;
	// }
	
	return 0;
}