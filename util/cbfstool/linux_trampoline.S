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

.global trampoline_start, trampoline_end
trampoline_start:

mov $0, %ecx

.headerSearch:
mov $0x10000, %edx
add %ecx, %edx
mov (%ecx), %eax
cmp $HEADER_SIG, %eax
je .headerSearchDone // found the header
add $16, %ecx
cmp %ecx, %edx
jne .headerSearch

.headerSearchDone:
cmp %ecx, %edx // reached the end == not found anything?
je 2f // give up

// we assume the checksum is okay, no test
mov 4(%ecx), %edx
add %ecx, %edx // edx = cb_header + header_bytes
mov 20(%ecx), %ecx // ecx = table_entries

.tableScan:
cmp $CB_TAG_FORWARD, (%edx)
jne .testMemory

/* forward tag: assume 32bit pointer */
mov 8(%edx), %ecx
jmp .headerSearch

.testMemory:
cmp $CB_TAG_MEMORY, (%edx)
jne .testFramebuffer

/* memory tag: copy e820 map and entry count. also determine alt_mem_k */
mov 4(%edx), %eax
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
lea 8(%edx), %esi /* e820 data source */
mov $(LINUX_PARAM_LOC + E820_OFFSET), %edi
rep movsl
xchg %eax, %ecx
jmp .endScan

.testFramebuffer:
cmp $CB_TAG_FRAMEBUFFER, (%edx)
jne .endScan
/* TODO: handle framebuffer tag */

.endScan:
add 4(%edx), %edx
dec %ecx
jnz .tableScan

/* finally: jump to kernel */
jmp *(LINUX_PARAM_LOC + LINUX_ENTRY_OFFSET)


2:
hlt
jmp 2b

trampoline_end:
