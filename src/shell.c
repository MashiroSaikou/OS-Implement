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
        /* 显示内存页面使用情况*/
        print_task();
        break;
    case 'c':
        print_curtask();
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
        printf("file test\n");
        break;
    case 't':
        printf("thread test\n");
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