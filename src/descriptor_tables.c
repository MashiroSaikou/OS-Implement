#include "descriptor_tables.h"
extern void gdt_w(uint32);
extern void idt_w(uint32);

static void gdt_init(void);
static void gdt_set(uint32 index, 
						 uint32 base, 
						 uint32 limit, 
						 uint8 access, 
						 uint8 gran);

static void idt_init(void);
static void idt_set(uint32 index,
						 uint32 base,
						 uint16 seg_selector,
						 uint8 flags);

static gdt_entry GDT_ENTRY[5];
static gdt_ptr GDT_PTR;
static idt_entry IDT_ENTRY[256];
static idt_ptr IDT_PTR;


void init_descriptor_tables()
{
	gdt_init();
	idt_init();
	//_panic("idt and gdt finish\n");
}

static void gdt_init()
{
	GDT_PTR.limit = sizeof(GDT_ENTRY) - 1;
	GDT_PTR.base = (uint32)&GDT_ENTRY;

	gdt_set(0, 0, 0, 0, 0);
	gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); //kernel mode
	gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); //usr mode
	//printf("%x\n", GDT_PTR.base);
	gdt_w((uint32)&GDT_PTR);
}

static void gdt_set(uint32 index, 
						 uint32 base, 
						 uint32 limit, 
						 uint8 access, 
						 uint8 gran)
{
	GDT_ENTRY[index].base_low = base & 0xFFFF;
	GDT_ENTRY[index].base_middle = (base>>16) & 0xFFFF;
	GDT_ENTRY[index].base_high = (base>>24) & 0xFF;

	GDT_ENTRY[index].limit_low = limit & 0xFFFF;
	GDT_ENTRY[index].granularity = (limit>>16) & 0x0F;
	GDT_ENTRY[index].granularity |= gran & 0xF0;
	GDT_ENTRY[index].access = access;
}

static void idt_init()
{
	IDT_PTR.limit = sizeof(IDT_ENTRY) - 1;
	IDT_PTR.base = (uint32)&IDT_ENTRY;

	memset(&IDT_ENTRY, 0, sizeof(IDT_ENTRY));

	/*
	 *8259A interrrupt set
	 *with icw1~4
	 */
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	
	idt_set( 0,  (uint32)isr0, 0x08, 0x8E);
	idt_set( 1,  (uint32)isr1, 0x08, 0x8E);
	idt_set( 2,  (uint32)isr2, 0x08, 0x8E);
	idt_set( 3,  (uint32)isr3, 0x08, 0x8E);
	idt_set( 4,  (uint32)isr4, 0x08, 0x8E);
	idt_set( 5,  (uint32)isr5, 0x08, 0x8E);
	idt_set( 6,  (uint32)isr6, 0x08, 0x8E);
	idt_set( 7,  (uint32)isr7, 0x08, 0x8E);
	idt_set( 8,  (uint32)isr8, 0x08, 0x8E);
	idt_set( 9,  (uint32)isr9, 0x08, 0x8E);
	idt_set(10, (uint32)isr10, 0x08, 0x8E);
	idt_set(11, (uint32)isr11, 0x08, 0x8E);
	idt_set(12, (uint32)isr12, 0x08, 0x8E);
	idt_set(13, (uint32)isr13, 0x08, 0x8E);
	idt_set(14, (uint32)isr14, 0x08, 0x8E);
	idt_set(15, (uint32)isr15, 0x08, 0x8E);
	idt_set(16, (uint32)isr16, 0x08, 0x8E);
	idt_set(17, (uint32)isr17, 0x08, 0x8E);
	idt_set(18, (uint32)isr18, 0x08, 0x8E);
	idt_set(19, (uint32)isr19, 0x08, 0x8E);
	idt_set(20, (uint32)isr20, 0x08, 0x8E);
	idt_set(21, (uint32)isr21, 0x08, 0x8E);
	idt_set(22, (uint32)isr22, 0x08, 0x8E);
	idt_set(23, (uint32)isr23, 0x08, 0x8E);
	idt_set(24, (uint32)isr24, 0x08, 0x8E);
	idt_set(25, (uint32)isr25, 0x08, 0x8E);
	idt_set(26, (uint32)isr26, 0x08, 0x8E);
	idt_set(27, (uint32)isr27, 0x08, 0x8E);
	idt_set(28, (uint32)isr28, 0x08, 0x8E);
	idt_set(29, (uint32)isr29, 0x08, 0x8E);
	idt_set(30, (uint32)isr30, 0x08, 0x8E);
	idt_set(31, (uint32)isr31, 0x08, 0x8E);
	
	idt_set(32, (uint32)irq0, 0x08, 0x8E);
	idt_set(33, (uint32)irq1, 0x08, 0x8E);
	idt_set(34, (uint32)irq2, 0x08, 0x8E);
	idt_set(35, (uint32)irq3, 0x08, 0x8E);
	idt_set(36, (uint32)irq4, 0x08, 0x8E);
	idt_set(37, (uint32)irq5, 0x08, 0x8E);
	idt_set(38, (uint32)irq6, 0x08, 0x8E);
	idt_set(39, (uint32)irq7, 0x08, 0x8E);
	idt_set(40, (uint32)irq8, 0x08, 0x8E);
	idt_set(41, (uint32)irq9, 0x08, 0x8E);
	idt_set(42, (uint32)irq10, 0x08, 0x8E);
	idt_set(43, (uint32)irq11, 0x08, 0x8E);
	idt_set(44, (uint32)irq12, 0x08, 0x8E);
	idt_set(45, (uint32)irq13, 0x08, 0x8E);
	idt_set(46, (uint32)irq14, 0x08, 0x8E);
	idt_set(47, (uint32)irq15, 0x08, 0x8E);

	idt_w((uint32)&IDT_PTR);
}

static void idt_set(uint32 index,
						 uint32 base,
						 uint16 seg_selector,
						 uint8 flags)
{
	IDT_ENTRY[index].base_low = base & 0xFFFF;
	IDT_ENTRY[index].base_high = (base >> 16) & 0xFFFF;
	IDT_ENTRY[index].seg_selector = seg_selector;
	IDT_ENTRY[index].flags = flags;
	IDT_ENTRY[index].always0 = 0;
}