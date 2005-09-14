	.section .rodata.optionrom
	.globl	_vgarom_start
_vgarom_start:
	.incbin	"../../../../../src/mainboard/intel/xe7501devkit/vga.rom"
	.globl	_vgarom_end
_vgarom_end:
