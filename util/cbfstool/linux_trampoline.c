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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#if 0
/* NOTE: THIS CODE MUST REMAIN POSITION INDEPENDENT
 *       IT SHOULDN'T USE THE STACK
 *       AND IN GENERAL EXPECT NOTHING BUT RAM TO WORK
 */
.code32
.data
#define HEADER_SIG 0x4f49424c // LBIO little endian
#define CB_TAG_FORWARD 0x11
#define CB_TAG_MEMORY 0x1
#define CB_TAG_FRAMEBUFFER 0x12

#define LINUX_PARAM_LOC 0x90000
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

/* finally: jump to kernel */
mov $LINUX_PARAM_LOC, %esi
jmp *(LINUX_PARAM_LOC + LINUX_ENTRY_OFFSET)


2:
hlt
jmp 2b

.trampoline_end:

.global trampoline_start, trampoline_size
trampoline_start:
.long .trampoline_start
trampoline_size:
.long .trampoline_end - .trampoline_start
#endif

/* The code above is hand-crafted to fit various contraints.
 * To simplify porting, the below matches the above.
 * When changing any code in here, compile the above as a .S
 * file, objcopy it to binary and paste the result below (minus
 * the last 8 bytes which are trampoline_start and trampoline_size).
 */
const unsigned char trampoline[] = {
0xfc, 0x31, 0xd2, 0xb9, 0x00, 0x00, 0x00, 0x00, 0xbb, 0x00, 0x00, 0x01, 0x00, 0x01, 0xcb, 0x8b,
0x01, 0x3d, 0x4c, 0x42, 0x49, 0x4f, 0x74, 0x07, 0x83, 0xc1, 0x10, 0x39, 0xcb, 0x75, 0xe9, 0x39,
0xcb, 0x74, 0x60, 0x8b, 0x59, 0x04, 0x01, 0xcb, 0x8b, 0x49, 0x14, 0x83, 0x3b, 0x11, 0x75, 0x05,
0x8b, 0x4b, 0x08, 0xeb, 0xd3, 0x83, 0x3b, 0x01, 0x75, 0x33, 0x8b, 0x43, 0x04, 0x83, 0xe8, 0x08,
0xc1, 0xe8, 0x02, 0x3d, 0xa0, 0x00, 0x00, 0x00, 0x7e, 0x05, 0xb8, 0xa0, 0x00, 0x00, 0x00, 0x89,
0xc6, 0xbf, 0x05, 0x00, 0x00, 0x00, 0xf7, 0xf7, 0xa3, 0xe8, 0x01, 0x09, 0x00, 0x89, 0xf0, 0x91,
0x8d, 0x73, 0x08, 0xbf, 0xd0, 0x02, 0x09, 0x00, 0xf3, 0xa5, 0x91, 0xeb, 0x05, 0x83, 0x3b, 0x12,
0x75, 0x00, 0x03, 0x5b, 0x04, 0x49, 0x75, 0xb3, 0xbe, 0x00, 0x00, 0x09, 0x00, 0xff, 0x25, 0x14,
0x02, 0x09, 0x00, 0xf4, 0xeb, 0xfd
};

void * const trampoline_start = &trampoline;
const unsigned long trampoline_size = sizeof trampoline;
