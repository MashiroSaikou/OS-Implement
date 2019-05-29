#include "page.h"
#include "kheap.h"
#define INDEX_FROM_BITMAP(x) ((x)/8)
#define OFFSET_FROM_BOTMAP(x) ((x)%8)
extern uint32 placement_address;
extern heap_t* kheap;

const uint32 MEMORY_SIZE = 0x1000000; /*32MB*/
const uint32 PAGE_SIZE = 0x1000; /*4KB*/
const uint32 FRAME_COUNT = MEMORY_SIZE / PAGE_SIZE;

page_dir_struct* vm_page_dir;
page_dir_struct* cur_vm_page_dir;
uint8* frames;
uint32 frames_bitmap_size;

static void page_fault();
static page_table_struct* clone_table(page_table_struct* src, uint32* phys);

static void set_frame(uint32 frame_addr)
{
	uint32 frame_index = frame_addr / 0x1000;
	uint32 index = INDEX_FROM_BITMAP(frame_index);
	uint32 offset = OFFSET_FROM_BOTMAP(frame_index);
	frames[index] |= (0x80 >> offset); 
}

static void clear_frame(uint32 frame_addr)
{
	uint32 frame_index = frame_addr / 0x1000;
	uint32 index = INDEX_FROM_BITMAP(frame_index);
	uint8 offset = OFFSET_FROM_BOTMAP(frame_index);
	frames[index] &= ~(0x80 >> offset); 	
}

static uint32 isIn_frame(uint32 frame_addr)
{
	uint32 frame_index = frame_addr / 0x1000;
	uint32 index = INDEX_FROM_BITMAP(frame_index);
	uint8 offset = OFFSET_FROM_BOTMAP(frame_index);
	return (frames[index] & (0x80 >> offset));	
}

static uint32 first_frame()
{
	uint32 i;
	uint8 j;
	for (i = 0; i < frames_bitmap_size; i++){
		if (frames[i] == 0xFF) continue;
		
		for(j = 0; j < 8; j ++){
			if (((~(0x80 >> j))|frames[i]) != 0xFFFFFFFF){
				return i*8 + j;
			}
		}
	}
	return -1;
}

void alloc_frame(page_struct* page, int is_kernel, int is_writable)
{
	if (page->frame != 0){
		return;
	}
	else{
		uint32 frame_index = first_frame();
		if (frame_index == -1){
			printf("%s\n", "[warning]No free frame...");
		} 
		set_frame(frame_index*0x1000);
		page->valid = 1;
		page->rw = (is_writable) ? 1 : 0;
		page->user = (is_kernel) ? 0 : 1;
		page->frame = frame_index;
	}
}

void free_frame(page_struct* page)
{
	if (page->frame == 0x0000){
		return;
	}
	clear_frame(page->frame);
	page->frame = 0;
}

static void thirteenisr(registers_t regs)
{
	printf("fuck!!\n");
}

void init_paging()
{
	uint32 mem_end_page = MEMORY_SIZE;

	/*initialise the bitmap of frames*/
	frames_bitmap_size = FRAME_COUNT/8;
	frames = (uint8*)_malloc_s(frames_bitmap_size);
	memset(frames, 0, frames_bitmap_size);

	/*initialise the things about page*/
	uint32 phy;
	vm_page_dir = (page_dir_struct*)_malloc_ap(sizeof(page_dir_struct), &phy);
	memset(vm_page_dir, 0, sizeof(page_dir_struct));
	vm_page_dir->page_dir_phy = (uint32)vm_page_dir->page_table_phy;

	int i = 0;
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
		page_struct* page = get_page(i, 1, vm_page_dir);
	}

	heap_t* heap = (heap_t*)_malloc_s(sizeof(heap_t));

	/*map the memory of kernel to the vm_page*/
	i = 0;
	while (i < placement_address){
		alloc_frame(get_page(i, 1, vm_page_dir), 0, 1);
		i += PAGE_SIZE;
	}

	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
		alloc_frame(get_page(i, 0, vm_page_dir), 0, 1);
	}
	
	/* register the fault handle of page fault*/
	register_interrupt_handler(14, page_fault);

	/*switch page directory to kernel directory*/
	switch_page_directory(vm_page_dir);

	kheap = create_heap(heap, KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);

	cur_vm_page_dir = clone_dir(vm_page_dir);
	//printf("page:%x\n", cur_vm_page_dir);
	switch_page_directory(cur_vm_page_dir);
}

page_struct* get_page(uint32 addr, int make, page_dir_struct* dir) {
	addr /= PAGE_SIZE; /*first and second index with 20bits*/
	uint32 index = addr/1024; /*first index with 10bits*/
	if (dir->page_table[index] != NULL) {
		/* if second index is exist*/
		return &(dir->page_table[index]->pages[addr % 1024]);
	}
	else if(make) {
		/*second index is not exist so we need to make one*/
		uint32 temp_address_phy;
		dir->page_table[index] = (page_table_struct*)_malloc_ap(sizeof(page_table_struct),
																	&temp_address_phy);
		memset(dir->page_table[index], 0, sizeof(page_table_struct));
		dir->page_table_phy[index] = temp_address_phy | 0x7;
		return &(dir->page_table[index]->pages[addr % 1024]);
	}
	else {
		return NULL;
	}
}

void switch_page_directory(page_dir_struct *dir)
{
	cur_vm_page_dir = dir;
	int cr0;
	asm volatile("cli");
	asm volatile("mov %0, %%cr3":: "r"(dir->page_dir_phy));
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	asm volatile("mov %0, %%cr0":: "r"(cr0));
	asm volatile("sti");
}



static void page_fault(registers_t regs)
{
	// A page fault has occurred.
	// The faulting address is stored in the CR2 register.
	uint32 faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	//printf("%x\n", faulting_address);
	// The error code gives us details of what happened.
	int present   = !(regs.err_code & 0x1); // Page not present
	int rw = regs.err_code & 0x2;           // Write operation?
	int us = regs.err_code & 0x4;           // Processor was in user-mode?
	int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
	int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

	// Output an error message.
	// printf("Page fault! ( ");
	// if (present) {printf("present ");}
	// if (rw) {printf("read-only ");}
	// if (us) {printf("user-mode ");}
	// if (reserved) {printf("reserved ");}
	// printf(") at 0x");
	printf("page fault:%x\n", faulting_address);
	for(;;);
}

page_dir_struct* clone_dir(page_dir_struct* src) {
	uint32 phyAddr;
	
	page_dir_struct* newdir = (page_dir_struct*)_malloc_ap(sizeof(page_dir_struct), &phyAddr);
	memset(newdir, 0, sizeof(page_dir_struct));
	printf("%x, %x\n", cur_vm_page_dir->page_table, newdir);
	printf("%x, %x\n", cur_vm_page_dir->page_table_phy[2], phyAddr);
	uint32 offset = (uint32)newdir->page_table_phy - (uint32)newdir;
	newdir->page_dir_phy = phyAddr + 0x1000;	// 			if(cur_vm_page_dir->page_table[j]->pages[k].frame != 0)
	

	int i = 0;
	for (i = 0; i < 1024; i++) {
		if (src->page_table[i] == NULL) continue;
		//printf("here:%d, %x, %x\n",i, vm_page_dir->page_table[i], src->page_table[i]);
		
		if (vm_page_dir->page_table[i] == src->page_table[i]) {
			newdir->page_table[i] = src->page_table[i];
			newdir->page_table_phy[i] = src->page_table_phy[i];
		}
		else {
			//printf("here\n");
			uint32 phys;
			newdir->page_table[i] = clone_table(src->page_table[i], &phys);
			newdir->page_table_phy[i] = phys | 0x07;
		}
	}
	return newdir;
}

static page_table_struct* clone_table(page_table_struct* src, uint32* phys) {
	page_table_struct* table = (page_table_struct*)_malloc_ap(sizeof(page_table_struct), phys);
	memset(table, 0, sizeof(page_table_struct));
	
	int i = 0;
	for (i = 0; i < 1024; i++) {
		//printf("f:%d \n", src->pages[i].frame);
		if (!src->pages[i].frame) continue;

		alloc_frame(&table->pages[i], 0, 0);
		if (src->pages[i].valid) table->pages[i].valid = 1;
		if (src->pages[i].rw)      table->pages[i].rw = 1;
		if (src->pages[i].user)    table->pages[i].user = 1;
		if (src->pages[i].accessed)table->pages[i].accessed = 1;
		if (src->pages[i].dirty)   table->pages[i].dirty = 1;

		copy_page_phy(src->pages[i].frame*0x1000, table->pages[i].frame*0x1000);
	}

	return table;
}