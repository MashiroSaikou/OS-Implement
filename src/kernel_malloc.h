#ifndef MALLOC_H
#define MALLOC_H

#include "common.h"
void* _malloc_s(uint32 m_size);

void* _malloc_a(uint32 m_size);

void* _malloc_ap(uint32 m_size, uint32 *phy_address);

void kfree(void* addr);

#endif