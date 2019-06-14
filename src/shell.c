#include "shell.h"

static uint32 page_test(char *v) {
    //int i = 1;
    //printf("%c\n", v[3]);
    if (v[1] == ' ' && v[2] == '-') {
        switch (v[3])
        {
        case 'a':
            /* 显示内存页面使用情况*/
            print_pages_status();
            break;
        default:
            return -1;
            break;
        }
        return 1;
    }
    else {
        return -1;
    }

}

static uint32 thread_test(char* v) {
    if (v[1] == ' ' && v[2] == '-') {
        switch (v[3])
        {
        case 'a':
            /* 显示进程使用情况*/
            print_task();
            break;
        case 'c':
            print_curtask();
            break;
        case 'n':
            create_task();
            break;
        case 'k':
            if(v[5] >'9' || v[5] < '2') break;
            task_exit_pid(v[5]-'0');
            break;
        default:
            return -1;
            break;
        }
        return 1;
    }
    else {
        return -1;
    }
}

extern fs_node_t* fs_root;
static uint32 file_test(char* v) {
    if (v[1] == ' ' && v[2] == '-') {
        char* f;
        switch (v[3])
        {
        case 'n':
            f = (char*)_malloc_s(sizeof(char)*256);
            memcpy(f, &v[5], 250);
            open_fs(fs_root, f, 1, 1);
            break;
        case 'd':
            f = (char*)_malloc_s(sizeof(char)*256);
            memcpy(f, &v[5], 250);
            close_fs(fs_root, f);
            break;
        case 'a':
            print_all_file();
            break;
        case 'c':
            print_all_file_content();
            break;
        default:
            return -1;
            break;
        }
        return 1;
    }
    else {
        return -1;
    }
}

uint32 test_process(char *v) {

    uint32 error = 0;
    switch (v[0])
    {
    case 'p':
        /*页测试*/
        printf("page test:\n");
        error = page_test(v);
        return error;
        break;
    case 'f':
        printf("file test:\n");
        error = file_test(v);
        return error;
        break;
    case 't':
        printf("thread test:\n");
        error = thread_test(v);
        break;
    default:
        return -1;
        break;
    }
}

void init_shell() {
    printf("\n");
    char *v = (char*)_malloc_s(256*sizeof(char));
    memset(v, '\0', 256*sizeof(char));
    while(1) {
        scanf("\nmashiro>>$ ", v);
        printf("\n");
        //printf("%c, %c, %c, %c\n", v[0], v[1], v[2], v[3]);
        uint32 error = test_process(v);
        if (error == -1) {
            printf("unknown commond...\n");
            continue;
        }
        memset(v, '\0', 256*sizeof(char));
    }
}