/* For starting linuxBIOS in protected mode */

#include <arch/cache_ram.h>

	.text
	.code32

/** This gdt has a 4 Gb code segment at 0x10, and a 4 GB data segment
 * at 0x18; these are Linux-compatible. 
 */

/** GDT. we have modified this from the original freebios to make it 
 * compatible with linux. This puts text at seg 0x10 and data at 0x18
 */

	.align	4
.globl EXT(gdtptr)
EXT(gdtptr):
	.word	4*8-1
	.long	gdt		/* we know the offset */

gdt:
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

	.word	0xffff, (CACHE_RAM_SEG_BASE & 0xffff)		/* flat offset data segment */
	.byte	((CACHE_RAM_SEG_BASE >> 16)& 0xff), 0x93, 0xcf
	.byte	((CACHE_RAM_SEG_BASE >> 24) & 0xff)
	
	.word	0xffff, 0x0000		/* flat code segment */
	.byte	0x00, 0x9b, 0xcf, 0x00	
	
	.word	0xffff, 0x0000		/* flat data segment */
	.byte	0x00, 0x93, 0xcf, 0x00	

/*
 *	When we come here we are in protected mode. We expand 
 *	the stack and copies the data segment from ROM to the
 *	memory.
 *
 *	After that, we call the chipset bootstrap routine that
 *	does what is left of the chipset initialization. 
 *
 *	NOTE aligned to 4 so that we are sure that the prefetch
 *	cache will be reloaded.
 */
	.align	4
.globl EXT(protected_start)
EXT(protected_start):

	lgdt	%cs:gdtptr
	ljmp	$0x10, $__protected_start
	
__protected_start:
	intel_chip_post_macro(0x10)	/* post 10 */

	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

