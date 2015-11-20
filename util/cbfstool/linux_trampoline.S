/*
 * linux_trampoline
 *
 * Copyright (C) 2013 Patrick Georgi <patrick@georgi-clan.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* NOTE: THIS CODE MUST REMAIN POSITION INDEPENDENT
 *       IT SHOULDN'T USE THE STACK
 *       AND IN GENERAL EXPECT NOTHING BUT RAM TO WORK
 */
.code32
.data

#include "linux_trampoline.h"
#define HEADER_SIG 0x4f49424c // LBIO little endian
#define CB_TAG_FORWARD 0x11
#define CB_TAG_MEMORY 0x1
#define CB_TAG_FRAMEBUFFER 0x12

#define E820_NR_OFFSET 0x1e8
#define LINUX_ENTRY_OFFSET 0x214
#define E820_OFFSET 0x2d0

.trampoline_start:

cld
xor %edx, %edx
mov $0, %ecx

.headerSearch:
mov $0x10000, %ebx
add %ecx, %ebx
mov (%ecx), %eax
cmp $HEADER_SIG, %eax
je .headerSearchDone // found the header
add $16, %ecx
cmp %ecx, %ebx
jne .headerSearch

.headerSearchDone:
cmp %ecx, %ebx // reached the end == not found anything?
je 2f // give up

// we assume the checksum is okay, no test
mov 4(%ecx), %ebx
add %ecx, %ebx // ebx = cb_header + header_bytes
mov 20(%ecx), %ecx // ecx = table_entries

.tableScan:
cmp $CB_TAG_FORWARD, (%ebx)
jne .testMemory

/* forward tag: assume 32bit pointer */
mov 8(%ebx), %ecx
jmp .headerSearch

.testMemory:
cmp $CB_TAG_MEMORY, (%ebx)
jne .testFramebuffer

/* memory tag: copy e820 map and entry count. also determine alt_mem_k */
mov 4(%ebx), %eax
sub $8, %eax
shr $2, %eax /* eax = number of dwords of e820 data */
cmp $(32 * 5), %eax /* linux wants at most 32 entries of 5 dwords */
jng 1f
mov $(32 * 5), %eax /* only copy 32 entries */
1:
mov %eax, %esi
mov $5, %edi
div %edi
mov %eax, (LINUX_PARAM_LOC + E820_NR_OFFSET)
mov %esi, %eax
xchg %eax, %ecx
lea 8(%ebx), %esi /* e820 data source */
mov $(LINUX_PARAM_LOC + E820_OFFSET), %edi
rep movsl
xchg %eax, %ecx
jmp .endScan

.testFramebuffer:
cmp $CB_TAG_FRAMEBUFFER, (%ebx)
jne .endScan
/* TODO: handle framebuffer tag */

.endScan:
add 4(%ebx), %ebx
dec %ecx
jnz .tableScan

/* Setup basic code and data segment selectors for Linux
**
** Flat code segment descriptor:
**   selector: 0x10
**   base    : 0x00000000
**   limit   : 0xFFFFFFFF
**   type    : code, execute, read
**
** Flat data segment descriptor:
**   selector: 0x18
**   base    : 0x00000000
**   limit   : 0xFFFFFFFF
**   type    : data, read/write
**
** Use TRAMPOLINE_ENTRY_LOC as a scratchpad.
*/
mov  $TRAMPOLINE_ENTRY_LOC, %eax
sgdt (%eax)
mov  2(%eax), %ebx
movl  $0x0000ffff, 16(%ebx)
movl  $0x00cf9b00, 20(%ebx)
movl  $0x0000ffff, 24(%ebx)
movl  $0x00cf9300, 28(%ebx)

/* finally: jump to kernel */
mov $LINUX_PARAM_LOC, %esi
jmp *(LINUX_PARAM_LOC + LINUX_ENTRY_OFFSET)


2:
hlt
jmp 2b

.trampoline_end:
