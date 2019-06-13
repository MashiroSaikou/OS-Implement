#ifndef TASK_H
#define TASK_H

#include "common.h"
#include "type.h"
#include "page.h"
#include "kernel_malloc.h"

extern page_dir_struct* cur_vm_page_dir;
extern page_dir_struct* vm_page_dir;
extern uint32 initial_esp;
extern uint32 read_eip();

typedef struct _context
{
    uint32 esp;
    uint32 ebp;
    uint32 ebx;
    uint32 esi;
    uint32 edi;
    uint32 sflags;
}context_t;

typedef enum task_state {
    TASK_UNINIT = 0,    // 未初始化
    TASK_SLEEPING = 1,  // 睡眠中
    TASK_RUNNABLE = 2,  // 可运行(也许正在运行)
    TASK_ZOMBIE = 3,    // 僵尸状态
} task_state_t;

typedef struct _pcb {
    int pid;
    uint32 esp,ebp;
    uint32 eip;
    volatile task_state_t state;
    context_t context; 
    page_dir_struct* page_dir;
    struct _pcb* next;
}pcb_t;

void init_multitask();
void task_switch();
int fork();
int getpid();
void print_task();
void print_curtask();
void move_stack(void* new_stack, uint32 size);
void task_exit_pid(int pid);
void task_exit();

#endif