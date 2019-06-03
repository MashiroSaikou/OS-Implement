#include "task.h"

volatile pcb_t* cur_task = NULL;
volatile pcb_t* head_task = NULL;
uint32 next_pid = 1;

void move_stack(void* new_stack, uint32 size) {
    uint32 i;
    for (i = (uint32) new_stack; i >= ((uint32)new_stack-size); i-=0x1000) {
        printf("mashiro\n");
        alloc_frame(get_page(i, 1, cur_vm_page_dir), 0, 1);
    }
    //printf("mashiro\n");
    uint32 page_dir_addr;
    asm volatile("mov %%cr3, %0" : "=r" (page_dir_addr));
    asm volatile("mov %0, %%cr3" : : "r"(page_dir_addr));
    uint32 old_stack_p;
    asm volatile("mov %%esp, %0" : "=r" (old_stack_p));
    uint32 old_base_p;
    asm volatile("mov %%ebp, %0" : "=r" (old_base_p));

    uint32 offset = (uint32)new_stack - initial_esp;
    uint32 new_stack_p = old_stack_p + offset;
    uint32 new_base_p = old_base_p + offset;
    
    memcpy((void*)new_stack_p, (void*)old_stack_p, initial_esp-old_stack_p);

    for (i = (uint32)new_stack; i > (uint32)new_stack - size; i -= 4) {
        uint32 tmp = *(uint32*)i;
        if ((old_stack_p < tmp) && (tmp < initial_esp)) {
            tmp = tmp + offset;
            uint32* tmp2 = (uint32*)i;
            *tmp2 = tmp;
        }
    }
    
    asm volatile("mov %0, %%esp" : : "r" (new_stack_p));
    asm volatile("mov %0, %%ebp" : : "r" (new_base_p));
}

void init_multitask() {
    asm volatile("cli");
   
    move_stack((void*)0xE0000000, 0x2000);

    cur_task = head_task = (pcb_t*)_malloc_s(sizeof(pcb_t));
    
    cur_task->pid = next_pid++; //??
    cur_task->esp = cur_task->ebp = 0;
    cur_task->eip = 0;
    //cur_task->context = {0};
    cur_task->page_dir = cur_vm_page_dir;
    cur_task->next = NULL;
    cur_task->state = TASK_RUNNABLE;
    asm volatile("sti");
}

void print_task() {
    pcb_t* t = head_task;
    while(t!=NULL) {
        printf("%d, ",t->pid);
        t = t->next;
    }
    printf("\n");
}

int fork() {
    asm volatile("cli");
    
    pcb_t* parent_task = (pcb_t*)cur_task;
    printf("fine\n");
    
    page_dir_struct *dir = clone_dir(cur_vm_page_dir);
    printf("phy:%x\n",dir->page_dir_phy);
    pcb_t* new_task = (pcb_t*)_malloc_s(sizeof(pcb_t));

    new_task->pid = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_dir = dir;
    new_task->next = NULL;
    new_task->state = TASK_RUNNABLE;
    
    pcb_t* tmp_task = (pcb_t*)head_task;
    while (tmp_task->next) {
        tmp_task = tmp_task->next;
    }
    tmp_task->next = new_task;
    
    uint32 eip = read_eip();

    if (cur_task == parent_task) {
        uint32 esp; asm volatile("mov %%esp, %0" : "=r"(esp));
        uint32 ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;
        printf("newfork:s;%x, b:%x, i:%x", esp, ebp, eip);
        asm volatile("sti");

        return new_task->pid;
    }
    else {
        asm volatile("sti");
        return 0;
    }
}

int getpid() {
    return cur_task->pid;
}

void task_switch() {
    if (!cur_task) return;

    //printf("now pid:%d\t", cur_task->pid); 
    uint32 esp, ebp, eip;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    
    eip = read_eip();
    if (eip == 0x12345) return;
    
    cur_task->eip = eip;
    cur_task->esp = esp;
    cur_task->ebp = ebp;
    //printf("old:s;%x, b:%x, i:%x\n", esp, ebp, eip);
    cur_task = cur_task->next;
    if(!cur_task) cur_task = head_task;
    
    esp = cur_task->esp;
    ebp = cur_task->ebp;
    eip = cur_task->eip;
    //printf("new:s;%x, b:%x, i:%x\n", esp, ebp, eip);
    cur_vm_page_dir = cur_task->page_dir;
    //for(;;);
    //printf("dir:%x\n", cur_vm_page_dir->page_dir_phy);
    // asm volatile("cli");
    // asm volatile("mov %0, %%esp" : : "r"(esp));
    // asm volatile("mov %0, %%ebp" : : "r"(ebp));
    // asm volatile("mov %0, %%cr3" : : "r"(cur_vm_page_dir->page_dir_phy));
    // asm volatile("mov $0x12345, %%eax" : :);
    // asm volatile("mov %0, %%ecx" : : "r"(eip));
    // asm volatile("sti");
    // //for(;;);
    // asm volatile("jmp *%%ecx" : :);
    asm volatile("         \
    cli;                 \
    mov %0, %%cr3;       \
    mov %1, %%esp;       \
    mov %2, %%ebp;       \
    mov %3, %%ecx;       \
    mov $0x12345, %%eax; \
    sti;                 \
    jmp *%%ecx;           " : : "r"(cur_vm_page_dir->page_dir_phy), "r"(esp), "r"(ebp), "r"(eip));
}