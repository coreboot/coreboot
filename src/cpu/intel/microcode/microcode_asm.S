/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * input %esp: return address (not pointer to return address!)
 * clobber the content of eax, ebx, ecx, edx, esi, edi, and ebp
 */

#include <cpu/x86/post_code.h>
#include <cpu/x86/msr.h>

#define CBFS_FILE_MAGIC 0
#define CBFS_FILE_LEN (CBFS_FILE_MAGIC + 8)
#define CBFS_FILE_TYPE (CBFS_FILE_LEN + 4)
#define CBFS_FILE_CHECKSUM (CBFS_FILE_TYPE + 4)
#define CBFS_FILE_OFFSET (CBFS_FILE_CHECKSUM + 4)

#define HEADER_VER_OFFSET 0
#define UPDATE_VER_OFFSET 4
#define DATE_OFFSET 8
#define PROCESSOR_SIG_OFFSET 12
#define CHKSUM_OFFSET 16
#define LOADER_REV_OFFSET 20
#define PROCESSOR_FLAG 24
#define DATA_SIZE_OFFSET 28
#define TOTAL_OFFSET 32
#define HEADER_SIZE 48

/*
 * The microcode header is 48 bytes wide and has the following
 * structure:
 *   Header Version      : 32bit
 *   Update Revision     : 32bit
 *   Date                : 32bit
 *   Processor Signature : 32bit
 *   Checksum            : 32bit
 *   Loader Revision     : 32bit
 *   Processor Flags     : 32bit
 *   Data Size           : 32bit
 *   Total Size          : 32bit
 *   Reserved            : 96bit
 *
 * We only check if the Processor signature and flags match and check
 * if the revision of the update is newer than what is installed
 */

.code32
.section .text
.global update_bsp_microcode

update_bsp_microcode:
	/* Keep return address */
	movl	%esp, %edx
	/* find microcodes in cbfs */
	leal	microcode_name, %esi
	movl	$1f, %esp
	jmp	walkcbfs_asm

1:
	/* restore return address */
	movl	%edx, %esp

	cmpl	$0, %eax
	je	end_microcode_update
	movl	CBFS_FILE_OFFSET(%eax), %ebx
	bswap	%ebx
	addl	%eax, %ebx
	movl	%ebx, %esi

	movl	CBFS_FILE_LEN(%eax), %edi
	bswap	%edi
	addl	%esi, %edi

	/*
	 * Microcode revision -> %ebx
	 * Processor flags -> %ebp
	 * Current installed microcode revision -> %edx
	 */

	/* Processor flags
	 * rdmsr 0x17
	 * pf = 1 << ((msr.hi >> 18) & 7) */
	movl	$IA32_PLATFORM_ID, %ecx
	rdmsr
	shr	$18, %edx
	andl	$7, %edx
	movl	$1, %eax
	/* needs to be %cl for shl */
	movl	%edx, %ecx
	shl	%cl, %eax
	movl	%eax, %ebp

	/* Fetch the current microcode revision*/
	xorl	%eax, %eax
	xorl	%edx, %edx
	movl	$IA32_BIOS_SIGN_ID, %ecx
	wrmsr
	movl	$0x1, %eax
	cpuid

	/* Processor family+model signature=cpuid_eax(1) */
	movl	%eax, %ebx

	movl	$IA32_BIOS_SIGN_ID, %ecx
	rdmsr

check_microcode_entry:
	/* Test if header revision is non zero */
	cmpl	$0, HEADER_VER_OFFSET(%esi)
	je	end_microcode_update

	/* Processor family+model signature=cpuid_eax(1) */
	cmpl	PROCESSOR_SIG_OFFSET(%esi), %ebx
	jne	next_entry

	/* Processor flags */
	test	PROCESSOR_FLAG(%esi), %ebp
	jz	next_entry

	/* Check if revision is higher than current */
	cmpl	UPDATE_VER_OFFSET(%esi), %edx
	/* Don't upgrade if already greater or equal */
	jge	end_microcode_update

	/* Do actual update */
	movl	%esi, %eax
	addl	$HEADER_SIZE, %eax
	xorl	%edx, %edx
	movl	$IA32_BIOS_UPDT_TRIG, %ecx
	wrmsr

	jmp	end_microcode_update

next_entry:
	movl	TOTAL_OFFSET(%esi), %eax
	cmpl	$0, %eax
	jne	1f
	/* Newer microcode updates include a size field, whereas older
	 * containers set it at 0 and are exactly 2048 bytes long */
	addl	$2048, %esi
	jmp	check_end
1:
	addl	%eax, %esi

check_end:
	cmpl	%esi, %edi
	ja	check_microcode_entry

end_microcode_update:
	jmp	*%esp

microcode_name:
	.string	"cpu_microcode_blob.bin"

_update_bsp_microcode_end:
