#include "kernel_malloc.h"
/*this is defined in link.ld
 *the pyhsical address of __end is the end of kernel
 */
extern uint32 __end;
uint32 placement_address = (uint32)&__end;
/*
alloc extendly memory the size of which is m_size
beginning from placement_address
 */
uint32 _malloc_s(uint32 m_size)
{
	uint32 tmp_placement_address = placement_address;
	placement_address += m_size;
	return tmp_placement_address;
}

/*
alloc extendly memory like above but aligning with 4KB
 */
uint32 _malloc_a(uint32 m_size, size_t isAlign)
{
	if (isAlign == 1 && (placement_address & 0x00000FFF)){
		/*the address is not aligned with 4KB*/
		placement_address = (placement_address & 0xFFFFF000)
								+ 0x1000;
	}
	uint32 tmp_placement_address = placement_address;
	placement_address += m_size;
	return tmp_placement_address;
}

uint32 _malloc_ap(uint32 m_size, size_t isAlign, uint32 *phy_address)
{
	if (isAlign == 1 && (placement_address & 0x00000FFF)){
		/*the address is not aligned with 4KB*/
		placement_address = (placement_address & 0xFFFFF000)
								+ 0x1000;
	}
	if (phy_address != NULL){
		*phy_address = placement_address;
	}
	uint32 tmp_placement_address = placement_address;
	placement_address += m_size;
	return tmp_placement_address;
}



