[GLOBAL idt_w]

idt_w:
	mov eax, [esp + 4]
	lidt [eax]
	ret