.text

__original_registers:
	.long 0, 0, 0, 0, 0, 0 , 0 , 0
__entry:
	.long entry

.globl __start
__start:
	movl %eax, 0+__original_registers
	movl %ebx, 4+__original_registers
	movl %ecx, 8+__original_registers
	movl %edx, 12+__original_registers
	movl %esi, 16+__original_registers
	movl %edi, 20+__original_registers
	movl %esp, 24+__original_registers
	movl %ebp, 28+__original_registers

__normal_start:
	movl $_estack, %esp
	call kunzip

.globl jmp_to_program_entry
jmp_to_program_entry:
	movl 4(%esp), %eax
	movl %eax, __entry
	movl 0+__original_registers, %eax
	movl 4+__original_registers, %ebx
	movl 8+__original_registers, %ecx
	movl 12+__original_registers, %edx
	movl 16+__original_registers, %esi
	movl 20+__original_registers, %edi
	movl 24+__original_registers, %esp
	movl 28+__original_registers, %ebp
	jmp *__entry
