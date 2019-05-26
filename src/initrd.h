#ifndef INITRD_H
#define INITRD_H

#include "common.h"
#include "fs.h"

typedef struct _initrd_header_t
{
    uint32 nfiles;
}initrd_header_t;

typedef struct _initrd_file_header_t
{
    uint8 magic;
    char name[64];
    uint32 offset;
    uint32 length;
}initrd_file_header_t;

fs_node_t* initialise_initrd(uint32 loc);

#endif