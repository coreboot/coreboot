/* SPDX-License-Identifier: GPL-2.0-only */

#include <fmap_config.h>

#if CONFIG(CBFS_VERIFICATION)
#error "walkcbfs_asm is not safe to use with CBFS verification!"
#endif

/* we use this instead of CBFS_HEADER_ALIGN because the latter is retired. */
#define CBFS_ALIGNMENT 64

#define CBFS_FILE_MAGIC 0
#define CBFS_FILE_LEN (CBFS_FILE_MAGIC + 8)
#define CBFS_FILE_TYPE (CBFS_FILE_LEN + 4)
#define CBFS_FILE_CHECKSUM (CBFS_FILE_TYPE + 4)
#define CBFS_FILE_OFFSET (CBFS_FILE_CHECKSUM + 4)

#define CBFS_FILE_STRUCTSIZE (CBFS_FILE_OFFSET + 4)

#define COREBOOT_CBFS_START (0xffffffff - CONFIG_ROM_SIZE + 1 + FMAP_SECTION_COREBOOT_START)

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

	movl $COREBOOT_CBFS_START, %ebx

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
	/* if addr <= COREBOOT_END - 1, continue */
#define FMAP_SECTION_COREBOOT_END (COREBOOT_CBFS_START - 1 + FMAP_SECTION_COREBOOT_SIZE)

	movl $FMAP_SECTION_COREBOOT_END, %ecx
	cmp %ecx, %ebx
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
