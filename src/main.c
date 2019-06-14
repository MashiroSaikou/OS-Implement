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
#include "keyboard.h"
#include "shell.h"

struct multboot;
uint32 initial_esp;
extern uint32 placement_address;

int main(struct multiboot *mboot_ptr, uint32 initial_stack)
{
  // All our initialisation calls will go in here.
	monitor_clear();
	monitor_write("hello, word!\n");
	initial_esp = initial_stack;
	printf("[*]initialising descriptor tables...\n");
	init_descriptor_tables();
	printf("[^]descriptor tables ready\n");

	asm volatile("sti");
	printf("[*]initialising timer...\n");	
	init_timer(100);
	printf("[^]timer ready\n");	

	uint32 initrd_location = *((uint32*)mboot_ptr->mods_addr);
	uint32 initrd_end = *(uint32*)(mboot_ptr->mods_addr+4);
	placement_address = initrd_end;
	
	printf("[*]initialising page...\n");
	init_paging();
	printf("[^]page ready\n");
	
	printf("[*]initialising multithread...\n");
	init_multitask();
	printf("[^]multithread raedy\n");

	printf("[*]initialising keyboard I/O...\n");
	init_keyboard_driver();
	printf("[^]keyboard ready\n");

	printf("[*]initialising VFS...\n");
	fs_root = initialise_initrd(initrd_location);
	printf("[^]VFS ready\n");

	printf("=============welcome to mashiro's OS==============\n");
	printf("=============p -a                      ==============\n");
	printf("=============f -a -c -n -d            ==============\n");
	printf("=============t -a -c -n -k            ==============\n");

	int ret = fork();
	if (ret == 0) {
		init_shell();
		for(;;);
	}
	else {
		for(;;);
	}

	// printf("?????\n");
	// char c = '\0';
	// printf("waiting char\n");
	// while ((c = keyboard_getchar()) == '\0');
	// printf("have char\n");
	// while(c != '\n') {
	// 	printf("%c", c);
	// 	c = keyboard_getchar();
	// }
	// printf("finish char\n");
	
	// int ret = fork();
	// if(ret == 0) {
	// 	init_shell();
	// 	for (;;);
	// }
	// else {
	//     int r = fork();
	// 	if (r == 0) {
	// 		for(;;);
	// 	}
	// 	else {
	// 		for(;;);
	// 	}
	// 	for (;;); //printf("parent\n");
	// }
	// printf("finish\n");
	

	// int i = 0;
	// dirent_t *node = 0;
	// while ((node = readdir_fs(fs_root, i)) != 0) {
	// 	printf("find file:%s\n", node->name);
	// 	// fs_node_t *fsnode = finddir_fs(fs_root, node->name);

	// 	// if((fsnode->flags&0x7) == FS_DIRECTORY) {
	// 	// 	printf("(directory)\n");
	// 	// }
	// 	// else {
	// 	// 	printf("contents:\"\n");
	// 	// 	char buf[256];
	// 	// 	uint32 sz = read_fs(fsnode, 0, 256, buf);
	// 	// 	int j;
	// 	// 	for (j = 0; j < sz; j++) {
	// 	// 		printf("%c",buf[j]);
	// 	// 	}
	// 	// 	printf("\"\n");
	// 	// }

	// 	i ++;
	// }
	// i = 0;
	// open_fs(fs_root, "mashiro.txt", 1, 1);
	// while ((node = readdir_fs(fs_root, i)) != 0) {
	// 	printf("find file:%s\n", node->name);

	// 	i ++;
	// }

	// close_fs(fs_root, "mashiro.txt");
	// i = 0;
	// while ((node = readdir_fs(fs_root, i)) != 0) {
	// 	printf("find file:%s\n", node->name);

	// 	i ++;
	// }
	return 0;
}