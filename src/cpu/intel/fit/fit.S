/* SPDX-License-Identifier: GPL-2.0-only */

.section ".fit_pointer", "a", @progbits
	.code32
.global fit_pointer
fit_pointer:
.long fit_table
.long 0

.section .text
.align 16
.global fit_table
.global fit_table_end
fit_table:
/* Address for type 0 is '_FIT_   ' */
.long 0x5449465f
.long 0x2020205f
/*
 * There is 1 entry in the table. Other tools will have to update the size
 * and checksum when adding entries.
 */
.long 0x00000001
/* Version */
.word 0x0100
/* Type 0 with checksum valid. */
.byte 0x80
/* Checksum byte - must add to zero. */
.byte 0x7d
.fill CONFIG_CPU_INTEL_NUM_FIT_ENTRIES*16
fit_table_end:
