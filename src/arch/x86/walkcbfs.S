/* SPDX-License-Identifier: GPL-2.0-only */

#define CBFS_HEADER_PTR 0xfffffffc

#define CBFS_HEADER_MAGIC 0
#define CBFS_HEADER_VERSION (CBFS_HEADER_MAGIC + 4)
#define CBFS_HEADER_ROMSIZE (CBFS_HEADER_VERSION + 4)
#define CBFS_HEADER_BOOTBLOCKSIZE (CBFS_HEADER_ROMSIZE + 4)
#define CBFS_HEADER_ALIGN (CBFS_HEADER_BOOTBLOCKSIZE + 4)
#define CBFS_HEADER_OFFSET (CBFS_HEADER_ALIGN + 4)

/* we use this instead of CBFS_HEADER_ALIGN because the latter is retired. */
#define CBFS_ALIGNMENT 64

#define CBFS_FILE_MAGIC 0
#define CBFS_FILE_LEN (CBFS_FILE_MAGIC + 8)
#define CBFS_FILE_TYPE (CBFS_FILE_LEN + 4)
#define CBFS_FILE_CHECKSUM (CBFS_FILE_TYPE + 4)
#define CBFS_FILE_OFFSET (CBFS_FILE_CHECKSUM + 4)

#define CBFS_FILE_STRUCTSIZE (CBFS_FILE_OFFSET + 4)

.code32
.section .init
.global walkcbfs_asm

/*
 * input %esi: filename
 * input %esp: return address (not pointer to return address!)
 * output %eax: pointer to CBFS header
 * clobbers %ebx, %ecx, %edi
 */
walkcbfs_asm:
	cld

	mov CBFS_HEADER_PTR, %eax
	mov CBFS_HEADER_ROMSIZE(%eax), %ecx
	bswap %ecx
	mov $0, %ebx
	sub %ecx, %ebx	/* ROM base address in ebx */
	mov CBFS_HEADER_OFFSET(%eax), %ecx
	bswap %ecx
	add %ecx, %ebx	/* address where we start looking for LARCHIVEs */

	/* determine filename length */
	mov $0, %eax
1:
	cmpb $0, (%eax,%esi)
	jz 2f
	add $1, %eax
	jmp 1b
2:
	add $1, %eax
walker:
	mov 0(%ebx), %edi /* Check for LARCHIVE header */
	cmp %edi, filemagic
	jne searchfile
	mov 4(%ebx), %edi
	cmp %edi, filemagic+4
	jne searchfile

	/* LARCHIVE header found */
	mov %ebx, %edi
	add $CBFS_FILE_STRUCTSIZE, %edi /* edi = address of first byte after
					 * struct cbfs_file
					 */
	mov %eax, %ecx
	repe cmpsb
	/* zero flag set if strings are equal */
	jnz tryharder

	/* we found it! */
	mov %ebx, %eax
	jmp *%esp

tryharder:
	sub %ebx, %edi
	sub $CBFS_FILE_STRUCTSIZE, %edi /* edi = # of walked bytes */
	sub %edi, %esi /* esi = start of filename */

	/* ebx = ecx = (current+offset+len+ALIGN-1) & ~(ALIGN-1) */
	mov CBFS_FILE_OFFSET(%ebx), %ecx
	bswap %ecx
	add %ebx, %ecx
	mov CBFS_FILE_LEN(%ebx), %edi
	bswap %edi
	add %edi, %ecx
	/* round by 64 bytes */
	add $(CBFS_ALIGNMENT - 1), %ecx
	and $~(CBFS_ALIGNMENT - 1), %ecx

	/* if oldaddr >= addr, leave */
	cmp %ebx, %ecx
	jbe out

	mov %ecx, %ebx

check_for_exit:
	/* look if we should exit: did we pass into the bootblock already? */
	mov CBFS_HEADER_PTR, %ecx
	mov CBFS_HEADER_BOOTBLOCKSIZE(%ecx), %ecx
	bswap %ecx
	not %ecx
	add $1, %ecx

	cmp %ecx, %ebx
	/* if bootblockstart >= addr (==we're still in the data area),
	 * jump back
	 */
	jbe walker

out:
	mov $0, %eax
	jmp *%esp


searchfile:
	/* if filemagic isn't found, move forward 64 bytes */
	add $CBFS_ALIGNMENT, %ebx
	jmp check_for_exit

filemagic:
	.ascii "LARCHIVE"
