.section ".text"
.globl _start
_start:
	ldr	pc, _hardwaremain	@ jump to C code

_hardwaremain: .word hardwaremain
@ .word hardwaremain


