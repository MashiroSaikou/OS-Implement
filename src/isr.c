#include "isr.h"
#include "common.h"
#include "monitor.h"

isr_t interrupt_handler[256];

void isr_handler(registers_t reg)
{
	printf("\n%d",reg.int_no);
	if (interrupt_handler[reg.int_no] != NULL) {
		isr_t handler = interrupt_handler[reg.int_no];
		handler(reg);
	} else {
		(void)printf("unhandled interrupt: %d\n", reg.int_no);
	}
}

void irq_handler(registers_t reg)
{
	if(reg.int_no > 39) {
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);
	char a = (reg.int_no);
	//monitor_write();
	if(interrupt_handler[reg.int_no] != 0)
	{
		isr_t handler = interrupt_handler[reg.int_no];
		handler(reg);
	}
}

void register_interrupt_handler(uint32 n, isr_t handler)
{
	interrupt_handler[n] = handler;
}