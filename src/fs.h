#ifndef FS_H
#define FS_H

#include "type.h"

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

typedef struct _fs_node fs_node_t;
typedef struct _dirent dirent_t;

typedef uint32 (*read_type_t)(fs_node_t*,uint32,uint32,uint8*);
typedef uint32 (*write_type_t)(fs_node_t*,uint32,uint32,uint8*);
typedef void (*open_type_t)(fs_node_t*);
typedef void (*close_type_t)(fs_node_t*);
typedef dirent_t* (*readdir_type_t)(fs_node_t*,uint32);
typedef fs_node_t * (*finddir_type_t)(fs_node_t*,char *name);

typedef struct _fs_node
{
  char name[128];
  uint32 mask;
  uint32 uid;
  uint32 gid;
  uint32 flags;
  uint32 inode;
  uint32 length;
  uint32 impl;
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir;
  finddir_type_t finddir;
  struct _fs_node* ptr; // Used by mountpoints and symlinks.
}fs_node_t;

typedef struct _dirent 
{
  char name[128]; // Filename.
  uint32 ino;     
}dirent_t;

extern fs_node_t* fs_root;

uint32 read_fs(fs_node_t* node, uint32 offset, uint32 size, uint8* buffer);
uint32 write_fs(fs_node_t* node, uint32 offset, uint32 size, uint8* buffer);
void open_fs(fs_node_t* node, uint8 read, uint8 write);
void close_fs(fs_node_t* node);
dirent_t* readdir_fs(fs_node_t* node, uint32 index);
fs_node_t* finddir_fs(fs_node_t* node, char* name);
#endif