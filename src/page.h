#ifndef PAGE_H
#define PAGE_H

#include "common.h"
#include "isr.h"
#include "kernel_malloc.h"

/*page enrty*/
typedef struct _page_struct
{
	uint32 valid : 1;
	uint32 rw : 1;
	uint32 user : 1;
	uint32 accessed : 1;
	uint32 dirty : 1;
	uint32 unused : 7;
	uint32 frame : 20;
}page_struct;

typedef struct _page_table_struct
{
	page_struct pages[1024];
}page_table_struct;

typedef struct _page_dir_struct
{
	page_table_struct* page_table[1024];

	uint32 page_table_phy[1024];

	uint32 page_dir_phy;
}page_dir_struct;

void init_paging();
page_struct* get_page(uint32 addr, int make, page_dir_struct* dir);
void alloc_frame(page_struct* page, int is_kernel, int is_writable);
void free_frame(page_struct* page);
void switch_page_directory(page_dir_struct *dir);
#endif							