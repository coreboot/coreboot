.section ".text"
.globl _start
_start:
	bl	_hardwaremain

_hardwaremain: .word hardwaremain
@ .word hardwaremain


