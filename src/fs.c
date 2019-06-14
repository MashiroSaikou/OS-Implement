#include "fs.h"

fs_node_t* fs_root = NULL;

uint32 read_fs(fs_node_t* node, uint32 offset, uint32 size, uint8* buffer) {
    if (node->read != 0) {
        return node->read(node, offset, size, buffer);
    }
    else {
        return 0;
    }
}

uint32 write_fs(fs_node_t* node, uint32 offset, uint32 size, uint8* buffer) {
    if (node->write != 0) {
        return node->write(node, offset, size, buffer);
    }
    else {
        return 0;
    }
}

fs_node_t* open_fs(fs_node_t* node, char* name, uint8 read, uint8 write) {
    if (node->open != 0) {
        return node->open(name);
    }
}

void close_fs(fs_node_t* node, char* name) {
    if (node->close != 0) {
        node->close(name);
    }
}

dirent_t* readdir_fs(fs_node_t* node, uint32 index) {
    if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0) {
        /* is dirent*/
        return node->readdir(node, index);
    }
    else {
        return 0;
    }
}

fs_node_t* finddir_fs(fs_node_t* node, char* name) {
    if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir != 0) {
        return node->finddir(node, name);
    }
    else {
        return 0;
    }
}

void print_all_file() {
    int i = 0;
    dirent_t *node = 0;
    while ((node = readdir_fs(fs_root, i)) != 0) {
		printf("find file:%s\n", node->name);
		i ++;
	}
}

void print_all_file_content() {
    int i = 0;
	dirent_t *node = 0;
	while ((node = readdir_fs(fs_root, i)) != 0) {
		printf("find file:%s\n", node->name);
		fs_node_t *fsnode = finddir_fs(fs_root, node->name);

		if((fsnode->flags&0x7) == FS_DIRECTORY) {
			printf("(directory)\n");
		}
		else {
			printf("contents:\"\n");
			char buf[256];
			uint32 sz = read_fs(fsnode, 0, 256, buf);
			int j;
			for (j = 0; j < sz; j++) {
				printf("%c",buf[j]);
			}
			printf("\"\n");
		}
		i ++;
	}
}