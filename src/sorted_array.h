#ifndef SORTED_ARRAY_H
#define SORTED_ARRAY_H

#include "common.h"

typedef uint32 (*cmp_fun_t)(void*, void*);

typedef struct _sorted_array_t{
	void* *array;
	uint32 size;
	uint32 cur;
	cmp_fun_t cmp_fun;
}sorted_array_t;vo

/*function for comparing*/
uint32 cmp(void* a, void* b);

sorted_array_t create_sorted_array(uint32 size, cmp_fun_t cmp);
sorted_array_t place_sorted_array(void* addr, uint32 maxsize, cmp_fun_t cmp);

void free_sorted_array(sorted_array_t *array);
void insert_sorted_array(void* item, sorted_array_t *a);
void* find_sorted_array(uint32 index, sorted_array_t *a);
void remove_sorted_array(uint32 index, sorted_array_t *a);

#endif