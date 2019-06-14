#include "initrd.h"

initrd_header_t* initrd_header;
initrd_file_header_t* file_headers;
fs_node_t* initrd_root;
fs_node_t* initrd_dev;
fs_node_t* root_nodes;
int nroot_nodes;

struct _dirent dirent;

static uint32 initrd_read(fs_node_t* node, uint32 offset, uint32 size, uint8* buffer) {
    initrd_file_header_t header = file_headers[node->inode];
    if (offset > header.length) {
        return 0;
    }
    if (offset + size > header.length) {
        size = header.length - offset;
    }
    memcpy(buffer, (uint8*)(header.offset + offset), size);
    return size;
}

static uint32 initrd_write(fs_node_t* node, uint32 offset, uint32 size, uint8* buffer) {
    initrd_file_header_t header = file_headers[node->inode];
    if (offset > header.length) {
        return 0;
    }
    if (offset + size > header.length) {
        size = header.length - offset;
    }
    memcpy((uint8*)(header.offset + offset), buffer, size);
}

static dirent_t* initrd_readdir(fs_node_t* node, uint32 index) {
    if (node == initrd_root && index == 0) {
        strcpy(dirent.name, "dev");
        dirent.name[3] = '\0';
        dirent.ino = 0;
        return &dirent;
    }

    if (index - 1 >= nroot_nodes) {
        return 0;
    }
    strcpy(dirent.name, root_nodes[index-1].name);
    dirent.name[strlen(root_nodes[index-1].name)] = '\0';
    dirent.ino = root_nodes[index-1].inode;
    return &dirent;
}

fs_node_t* initrd_finddir(fs_node_t* node, char* name) {
    if (node == initrd_root && !strcmp(name, "dev")) {
        return initrd_dev;
    }
    int i;
    for (i = 0; i < nroot_nodes; i++) {
        if (!strcmp(name, root_nodes[i].name)) {
            return &root_nodes[i];
        }
    }
    return 0;
}

static fs_node_t* initrd_open(char* name) {
    fs_node_t* t_root = (fs_node_t*)_malloc_s(sizeof(fs_node_t)*(initrd_header->nfiles+1));
    memcpy(t_root, root_nodes, sizeof(fs_node_t)*initrd_header->nfiles);
    initrd_header->nfiles += 1;
    nroot_nodes = initrd_header->nfiles;

    int i = nroot_nodes - 1;

    strcpy(root_nodes[i].name, name);
    root_nodes[i].mask = 0;
    root_nodes[i].uid = 0;
    root_nodes[i].gid = 0;
    root_nodes[i].length = 0;
    root_nodes[i].inode = i;
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].read = &initrd_read;
    root_nodes[i].write = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].impl = 0;

    return &root_nodes[i];
}

static void initrd_close(char* name) {
    int i = 0; 
    for(i = 0; i < nroot_nodes; i++) {
        if (strcmp(name, root_nodes[i].name) == 0) break;
    }
    for(;i+1<nroot_nodes; i++) {
        root_nodes[i] = root_nodes[i+1];
    }
    fs_node_t* t_root = (fs_node_t*)_malloc_s(sizeof(fs_node_t)*(initrd_header->nfiles-1));
    memcpy(t_root, root_nodes, sizeof(fs_node_t)*(initrd_header->nfiles-1));
    initrd_header->nfiles -= 1;
    nroot_nodes = initrd_header->nfiles;
}

fs_node_t* initialise_initrd(uint32 loc) {
    initrd_header = (initrd_header_t*)loc;
    file_headers = (initrd_file_header_t*)(loc + sizeof(initrd_header_t));

    initrd_root = (fs_node_t*)_malloc_s(sizeof(fs_node_t));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = 0;
    initrd_root->uid = 0;
    initrd_root->gid = 0;
    initrd_root->inode = 0;
    initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = &initrd_open;
    initrd_root->close = &initrd_close;
    initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;
    initrd_root->ptr = 0;
    initrd_root->impl = 0;

    initrd_dev = (fs_node_t*)_malloc_s(sizeof(fs_node_t));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->mask = 0;
    initrd_dev->uid = 0;
    initrd_dev->gid = 0;
    initrd_dev->inode = 0;
    initrd_dev->length = 0;
    initrd_dev->flags = FS_DIRECTORY;
    initrd_dev->read = 0;
    initrd_dev->write = 0;
    initrd_dev->open = &initrd_open;
    initrd_dev->close = &initrd_close;
    initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;

    root_nodes = (fs_node_t*)_malloc_s(sizeof(fs_node_t)*initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;

    int i = 0;
    for (i = 0; i < nroot_nodes; i++) {
        file_headers[i].offset += loc;

        strcpy(root_nodes[i].name, file_headers[i].name);
        root_nodes[i].mask = 0;
        root_nodes[i].uid = 0;
        root_nodes[i].gid = 0;
        root_nodes[i].length = file_headers[i].length;
        root_nodes[i].inode = i;
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = 0;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = 0;
        root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }
    return initrd_root;
}

