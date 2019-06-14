#include "timer.h"
#include "monitor.h"
#include "isr.h"
#include "common.h"
#include "task.h"
uint32 tick = 0;
static void timer_callback(registers_t reg)
{
	tick ++;
	//print_curtask();
	task_switch();
}

void init_timer(uint32 f)
{
	//monitor_write("5");
	asm volatile("cli");
	register_interrupt_handler(IRQ0, &timer_callback);
	uint32 divisor = 1193180/f;
	outb(0x43, 0x36);

	uint8 low = (uint8)(divisor & 0xFF);
	uint8 high = (uint8)((divisor>>8) & 0xFF);

	outb(0x40, low);
	outb(0x40, high);
	asm volatile("sti");
}

