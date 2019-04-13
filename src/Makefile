SOURCES=boot.o \
			main.o \
			monitor.o \
			common.o \
			descriptor_tables.o \
			isr.o \
			gdt_w.o \
			idt_w.o \
			interrupt.o\
			timer.o\
			printf.o\
			page.o\
			kernel_malloc.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -m32 -I.
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf

all: $(SOURCES) link 

clean:
	-rm *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<