	mov $0x00000000, %eax
	mov $0x0009ffff, %ebx
	mov $16, %ecx

	CALLSP(ramtest)
