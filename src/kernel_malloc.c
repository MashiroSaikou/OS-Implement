#include "kernel_malloc.h"
#include "kheap.h"
#include "page.h"
/*this is defined in link.ld
 *the pyhsical address of __end is the end of kernel
 */
extern uint32 __end;
extern heap_t* kheap;
extern page_dir_struct* vm_page_dir;
uint32 placement_address = (uint32)&__end;

static void* _malloc(uint32 m_size, size_t isAlign, uint32* phy_address);
/*
alloc extendly memory the size of which is m_size
beginning from placement_address
 */
void* _malloc_s(uint32 m_size)
{
	return _malloc(m_size, 0, NULL);
}

/*
alloc extendly memory like above but aligning with 4KB
 */
void* _malloc_a(uint32 m_size, size_t isAlign)
{
	return _malloc(m_size, isAlign, NULL);
}

void* _malloc_ap(uint32 m_size, size_t isAlign, uint32 *phy_address)
{
	return _malloc(m_size, isAlign, phy_address);
}

static void* _malloc(uint32 m_size, size_t isAlign, uint32 *phy_address) {
	void *addr = NULL;
	if (kheap == NULL) {
		if(isAlign == 1 && (placement_address & 0x00000FFF)) {
			placement_address = (placement_address & 0xFFFFF000)
								+ 0x1000;
		}
		if(phy_address != NULL) {
			*phy_address = placement_address;
		}
		placement_address += m_size;
		addr = (void*)(placement_address - m_size);
	}
	else {
		addr = alloc(m_size, isAlign, kheap);
		if(phy_address != NULL) {
			page_struct* page = get_page((uint32)addr, 0, vm_page_dir);
			*phy_address = page->frame*0x1000 + ((uint32)addr & 0x00000FFF);
		}
	}

	return addr;
}
void kfree(void* addr) {
	free(addr, kheap);
}


