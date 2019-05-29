#include "sorted_array.h"
#include "kernel_malloc.h"
#include "page.h"
extern page_dir_struct* cur_vm_page_dir;
/*it is a descending comparison function*/
int cmp(void* a, void *b)
{
	return (a < b ? 1 : 0);
}

sorted_array_t create_sorted_array(uint32 size, cmp_fun_t cmp)
{
	sorted_array_t a;
	a.array = (void*)_malloc_s(size * sizeof(void*));
	if (a.array == NULL){
		_panic("error in create_sorted_array\n");
	}
	memset(a.array, 0, size);
	a.size = size;
	a.cur = 0;
	a.cmp_fun = cmp;

	return a;
}

sorted_array_t place_sorted_array(void* addr, uint32 maxsize, cmp_fun_t cmp)
{
	sorted_array_t a;
	a.array = addr;
	memset(a.array, 0, maxsize*sizeof(void*));
	a.size = maxsize;
	a.cur = 0;
	a.cmp_fun = cmp;
	return a;
}

void free_sorted_array(sorted_array_t *array)
{
	kfree(array->array);
}

void insert_sorted_array(void* item, sorted_array_t *a)
{
	uint32 iter = 0;
	while (iter < a->cur && a->cmp_fun(a->array[iter], item) < 0) {
		iter ++;
	}
	if (iter == a->cur){
		a->array[a->cur++] = item;
	}
	else {
		void* temp = a->array[iter];
		a->array[iter] = item;
		
		while (iter <= a->cur){
			void* temp_2 = a->array[++iter];
			a->array[iter] = temp;
			temp = temp_2;
		}
		a->cur += 1;
	}

}

void* find_sorted_array(uint32 index, sorted_array_t *a)
{
	if(a == NULL || a->array == NULL || index >= a->size){
		_panic("error in find item in sorted array\n");
	}

	return a->array[index]; 
}

void remove_sorted_array(uint32 index, sorted_array_t *a)
{
	uint32 iter;
	if (a == NULL || a->array == NULL){
		_panic("error in remove item in sorted array\n");
	}
	if (index >= a->size){
		return;
	}

	for (iter = index + 1; iter < a->size; iter++){
		a->array[iter-1] = a->array[iter];
	}
	a->size -= 1;
}