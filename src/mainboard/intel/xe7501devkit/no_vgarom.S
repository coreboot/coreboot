	.section .rodata.optionrom
	.globl	_vgarom_start
_vgarom_start:
	.word 0xFFFF			// Invalid option ROM signature
	.globl	_vgarom_end
_vgarom_end:
