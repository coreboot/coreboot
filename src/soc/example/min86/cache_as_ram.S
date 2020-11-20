/* SPDX-License-Identifier: GPL-2.0-only */

.section .init, "ax", @progbits

.global bootblock_pre_c_entry

.code32
bootblock_pre_c_entry:
	call	bootblock_c_entry_bist

.Lhlt:
	hlt
	jmp	.Lhlt
