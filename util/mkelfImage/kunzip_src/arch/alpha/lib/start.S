.set noat
.set noreorder
.text

__original_registers:
	.quad 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

__entry:
	.quad entry

.globl __start
__start:
	br $27, __save_registers
__save_registers:
	lda $27, (__original_registers - __save_registers)($27)
	stq $0,    0($27)
	stq $1,    8($27)
	stq $2,   16($27)
	stq $3,   24($27)
	stq $4,   32($27)
	stq $5,   40($27)
	stq $6,   48($27)
	stq $7,   56($27)
	stq $8,   64($27)
	stq $9,   72($27)
	stq $10,  80($27)
	stq $11,  88($27)
	stq $12,  96($27)
	stq $13, 104($27)
	stq $14, 112($27)
	stq $15, 120($27)
	stq $16, 128($27)
	stq $17, 136($27)
	stq $18, 144($27)
	stq $19, 152($27)
	stq $20, 160($27)
	stq $21, 168($27)
	stq $22, 176($27)
	stq $23, 184($27)
	stq $24, 192($27)
	stq $25, 200($27)
	stq $26, 208($27)
	stq $28, 224($27)
	stq $29, 232($27)
	stq $30, 240($27)

__normal_start:
	ldgp $29, (__normal_start - __original_registers)($27)
	lda  $30, _estack
	jsr  $26, kunzip

.globl jmp_to_program_entry
jmp_to_program_entry:
	br   $27, __restore_registers
__restore_registers:
	lda  $27,(__original_registers - __restore_registers)($27)
	stq  $16, (__entry - __original_registers)($27)
	ldq $0,    0($27)
	ldq $1,    8($27)
	ldq $2,   16($27)
	ldq $3,   24($27)
	ldq $4,   32($27)
	ldq $5,   40($27)
	ldq $6,   48($27)
	ldq $7,   56($27)
	ldq $8,   64($27)
	ldq $9,   72($27)
	ldq $10,  80($27)
	ldq $11,  88($27)
	ldq $12,  96($27)
	ldq $13, 104($27)
	ldq $14, 112($27)
	ldq $15, 120($27)
	ldq $16, 128($27)
	ldq $17, 136($27)
	ldq $18, 144($27)
	ldq $19, 152($27)
	ldq $20, 160($27)
	ldq $21, 168($27)
	ldq $22, 176($27)
	ldq $23, 184($27)
	ldq $24, 192($27)
	ldq $25, 200($27)
	ldq $26, 208($27)
	ldq $28, 224($27)
	ldq $29, 232($27)
	ldq $30, 240($27)
	ldq $27, (__entry - __original_registers)($27)
	jsr $31, ($27)
