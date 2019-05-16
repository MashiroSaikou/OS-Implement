#include "kheap.h"
#include "page.h"

extern page_dir_struct* vm_page_dir;

heap_t* kheap = NULL;
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

static int header_cmp_less(void *a, void *b) {
	return ((header_t*)a)->size < ((header_t*)b)->size ? 1:0;
}

heap_t* create_heap(heap_t *heap,
					uint32 start, 
					uint32 end, 
					uint32 max, 
					uint8 supervisor, 
					uint8 readonly) 
{
	heap->index = place_sorted_array((void*)start, HEAP_INDEX_SIZE, &header_cmp_less);
	start += sizeof(void*) * HEAP_INDEX_SIZE;
	if (start & 0x00000FFF != 0) {
		start &= 0xFFFFF000;
		start += 0x1000;
	}
	
	heap->start_address = start;
	heap->end_address = end;
	heap->max_address = max;
	heap->readonly = readonly;
	heap->supervisor = supervisor;

	header_t* hole = (header_t*)start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_sorted_array((void*)hole, &heap->index);
	
	return heap;
}

static void expand(uint32 new_size, heap_t* heap) {
	if (new_size & 0x00000FFF != 0) {
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}

	uint32 old_size = heap->end_address - heap->start_address;
	uint32 i = old_size;
	while (i < new_size) {
		alloc_frame(get_page(heap->start_address + i, 1, vm_page_dir),
						(heap->supervisor) ? 1:0, (heap->readonly)? 1:0);
		i += 0x1000;
	}
	heap->end_address = heap->start_address + new_size;
}

static uint32 contract(uint32 new_size, heap_t* heap) {
	if (new_size & 0x00000FFF != 0) {
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}

	if (new_size < HEAP_MIN_SIZE) {
		new_size = HEAP_MIN_SIZE;
	}

	uint32 old_size = heap->end_address - heap->start_address;
	uint32 i = old_size - 0x1000;
	while (new_size < i) {
		free_frame(get_page(heap->start_address + i, 0, vm_page_dir));
				i -= 0x1000;
	}

	heap->end_address = heap->start_address + new_size;

	return new_size;
}

void* alloc(uint32 size, uint8 page_align, heap_t* heap) {
	uint32 new_size = size + sizeof(header_t) + sizeof(footer_t);
	int32 iter = find_hole_fit_first(new_size, page_align, heap);

	if (iter == -1) {
		/*no free space*/
		uint32 old_len = heap->end_address - heap->start_address;
		uint32 old_end_address = heap->end_address;

		expand(old_len + new_size, heap);
		uint32 new_len = heap->end_address - heap->start_address;

		iter = 0;

		uint32 idx = -1;
		uint32 value = 0x0;
		while (iter < heap->index.size) {
			uint32 tmp = (uint32)find_sorted_array(iter, &heap->index);
			if (tmp > value) {
				value = tmp;
				idx = iter;
			}
			iter ++;
		}

		if (idx == -1) {
			header_t *header = (header_t*)old_end_address;
			header->magic = HEAP_MAGIC;
			header->size = new_len - old_len;
			header->is_hole = 1;
			footer_t *footer = (footer_t*)(old_end_address + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
			insert_sorted_array((void*)header, &heap->index);
		}
		else {
			header_t *header = find_sorted_array(idx, &heap->index);
			header->size += new_len - old_len;
			footer_t *footer = (footer_t*)((uint32)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC; 
		}

		return alloc(size, page_align, heap);
	}	

	header_t* origin_hole_header = (header_t*)find_sorted_array(iter, &heap->index);
	uint32 origin_hole_pos = (uint32)origin_hole_header;
	uint32 origin_hole_size = origin_hole_header->size;
	if (origin_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
		/* the remain space is to small to use*/
		size += origin_hole_size - new_size;
		new_size = origin_hole_size;
	}

	if (page_align && (origin_hole_pos & 0x00000FFF) != 0) {
		uint32 new_location = origin_hole_pos + 0x1000 - 
								(origin_hole_pos & 0x00000FFF) - sizeof(header_t);
		header_t *hole_header = (header_t*)origin_hole_pos;
		hole_header->size = 0x1000 - sizeof(header_t) - (origin_hole_pos & 0x00000FFF);
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		footer_t *hole_footer = (footer_t*)(new_location - sizeof(footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole_header;
		origin_hole_pos = new_location;
		origin_hole_size = origin_hole_size - hole_header->size;
	}
	else {
		remove_sorted_array(iter, &heap->index);
	}

	header_t *block_header = (header_t*)origin_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;
	footer_t *block_footer = (footer_t*)(origin_hole_pos + origin_hole_size - sizeof(footer_t));
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block_header;

	/* the size of hole is bigger than what we need*/
	if (origin_hole_size > new_size) {
		header_t *hole_header = (header_t*)(origin_hole_pos + new_size);
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		hole_header->size = origin_hole_size - new_size;
		footer_t* hole_footer = (footer_t*)(origin_hole_pos + origin_hole_size - sizeof(footer_t));
		if ((uint32)hole_footer < heap->end_address) {
			hole_footer->magic = HEAP_MAGIC;
			hole_footer->header = hole_header;
		}

		insert_sorted_array((void*)hole_header, &heap->index);
	}

	return (void*)((uint32)block_header + sizeof(header_t));
}


void free(void* p, heap_t *heap) {
	if (p = 0) return;

	header_t *header = (header_t*)((uint32)p - sizeof(header_t));
	footer_t *footer = (footer_t*)((uint32)header + header->size - sizeof(footer_t));

	header->is_hole = 1;

	char do_add = 1;

	/* if the last hole is free, so we can contract them.*/
	footer_t *test_footer = (footer_t*)((uint32)header - sizeof(footer_t));
	if(test_footer->header->is_hole == 1 && test_footer->magic == HEAP_MAGIC) {
		uint32 cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	}

	/* if the next hole is free , same as the above*/
	header_t* test_header = (header_t*)((uint32)footer + sizeof(footer_t));
	if(test_header->is_hole == 1 && test_header->magic == HEAP_MAGIC) {
		uint32 cache_size = test_header->size;
		footer = (footer_t*)((uint32)test_header + test_header->size - sizeof(footer_t));
		footer->header = header;
		header->size += cache_size;
		do_add = 0;

		uint32 iter = 0;
		while (iter < heap->index.size &&
				(find_sorted_array(iter, &heap->index) != (void*)test_header)) {
			iter ++;
		}

		remove_sorted_array(iter, &heap->index);
	}

	if ((uint32)footer + sizeof(footer_t) == heap->end_address) {
		uint32 old_len = heap->end_address - heap->start_address;
		uint32 new_len = contract((uint32)header- heap->start_address, heap);

		if (header->size - (old_len - new_len) > 0) {
			header->size -= (old_len - new_len);
			footer = (footer_t*)((uint32)header + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
		}
		else {
			uint32 iter = 0;
			while (iter < heap->index.size &&
					(find_sorted_array(iter, &heap->index) != (void*)test_header)) {
				iter ++;
			}
			if (iter < heap->index.size) {
				remove_sorted_array(iter, &heap->index);
			}
		}
	}

	if (do_add == 1) insert_sorted_array((void*)header, &heap->index);
}

