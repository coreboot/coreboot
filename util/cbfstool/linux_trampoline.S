/* linux_trampoline */
/* SPDX-License-Identifier: GPL-2.0-only */

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
#define CB_TAG_ACPI_RSDP 0x43

#define ACPI_RSDP_ADDR 0x70
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
jne .testAcpiRsdp

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
/* e820 and LB_TAG_MEMORY type don't fully match: remap unknown type to 2, reserved memory */
mov (LINUX_PARAM_LOC + E820_NR_OFFSET), %eax
mov $(LINUX_PARAM_LOC + E820_OFFSET), %edi
.test_e820_entry:
cmp $0, %eax
je .endScan
cmp $12, 16(%edi) /* type */
jng .next_e820_entry
/* Fixup the type to 2, reserved memory */
mov $2, 16(%edi)
.next_e820_entry:
dec %eax
add $20, %edi
jmp .test_e820_entry

.testAcpiRsdp:
cmp $CB_TAG_ACPI_RSDP, (%ebx)
jne .testFramebuffer

mov 8(%ebx), %eax
mov %eax, (LINUX_PARAM_LOC + ACPI_RSDP_ADDR)
mov 12(%ebx), %eax
mov %eax, (LINUX_PARAM_LOC + ACPI_RSDP_ADDR + 4)
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
movl  $0x0000ffff, 16(%eax)		// Set up the 2 new descriptors
movl  $0x00cf9b00, 20(%eax)
movl  $0x0000ffff, 24(%eax)
movl  $0x00cf9300, 28(%eax)
movb $0x2b, 0(%eax)			// Set the size
movl %eax, 2(%eax)			// Set pointer to new GDT
lgdt (%eax)				// Load it

/* finally: jump to kernel */
mov $LINUX_PARAM_LOC, %esi
jmp *(LINUX_PARAM_LOC + LINUX_ENTRY_OFFSET)


2:
hlt
jmp 2b
.trampoline_end:
