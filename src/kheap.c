#include "kheap.h"
/*
 * find the first fit hole one by one
 * notice the whether it is oage aligned
 */
static int32 find_hole_fit_first(uint32 size, 
							uint8 page_align,
							heap_t* heap)
{
	uint32 iter = 0;
	while (iter < heap->index.size){
		header_t *header = (header_t*)find_sorted_array(iter, &heap->index);

		if (page_align > 0){
			uint32 location = (uint32)header;
			uint32 offset = 0;
			if ((location + sizeof(header_t)) & 0x00000FFF != 0) {
				/* not page align*/
				offset = 0x1000 - (location + sizeof(header_t)) % 0x1000;
			}

			uint32 hole_size_canused = (uint32)header->size - offset;
			if (size <= hole_size_canused) {
				break;
			}
		}
		else if (size <= header->size) {
			break;
		}

		iter++;
	}

	if (iter == heap->index.size) {
		/*not fit hole*/
		return -1;
	}
	else
		return iter;
}


heap_t* create_heap(uint32 start, 
					uint32 end, 
					uint32 max, 
					uint8 supervisor, 
					uint8 readonly) {
	heap_t *heap = (heap_t*)_malloc_s(sizeof(heap_t));

	heap->index = place_sorted_array((void*)start, HEAP_INDEX_SIZE, &head_t_cmp_less);
	start += sizeof(void*) * HEAP_INDEX_SIZE;

	if (start & 0x00000FFF != 0) {
		start &= 0x00000FFF;
		start += 0x1000;
	}

	heap->start_address = start;
	heap->end_address = end;
	heap->max_address = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;

	head_t* hole = (head_t*)start;
	hole->size = end_address - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_sorted_array((void*)hole, &heap->index);

	return heap;
}