/* For starting linuxBIOS in protected mode */

#include <arch/rom_segs.h>
#include <arch/cache_ram.h>

/* 	.section ".rom.text" */
	.code32

	.align	4
.globl EXT(gdtptr)

gdt:
EXT(gdtptr):
	.word	gdt_end - gdt -1 /* compute the table limit */
	.long	gdt		 /* we know the offset */
	.word	0

/* flat code segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x9b, 0xcf, 0x00	
	
/* flat data segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x93, 0xcf, 0x00	

#if 0  /* FIXME make the cache as ram code work again */
/* flat rom offset code segment */
	.word	0xffff, _rom_code_seg_base_low		
	.byte	_rom_code_seg_base_middle, 0x9b, 0xcf
	.byte	_rom_code_seg_base_high

/* flat cache ram offset data segment */
	.word	0xffff, _cache_ram_seg_base_low	
	.byte	_cache_ram_seg_base_middle, 0x93, 0xcf
	.byte	_cache_ram_seg_base_high
#endif

gdt_end:
	

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
	ljmp	$ROM_CODE_SEG, $__protected_start
	
__protected_start:
	intel_chip_post_macro(0x10)	/* post 10 */

	movw	$ROM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

