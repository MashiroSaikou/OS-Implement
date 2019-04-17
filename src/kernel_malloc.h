#ifndef MALLOC_H
#define MALLOC_H

#include "common.h"
uint32 _malloc_s(uint32 m_size);

uint32 _malloc_a(uint32 m_size, size_t isAlign);

uint32 _malloc_ap(uint32 m_size, size_t isAlign, uint32 *phy_address);

void kfree(void* addr);

#endif