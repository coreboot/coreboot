/* For starting linuxBIOS in protected mode */

#include <arch/cache_ram.h>

/* 	.section ".rom.text" */
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
	.word	gdt_end - gdt -1 /* compute the table limit */
	.long	gdt		 /* we know the offset */

gdt:
// selgdt 0
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

// selgdt 8
/* flat cache ram offset data segment */
	.word	0xffff, _cache_ram_seg_base_low	
	.byte	_cache_ram_seg_base_middle, 0x93, 0xcf
	.byte	_cache_ram_seg_base_high
	
// selgdt 0x10 
/* flat code segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x9b, 0xcf, 0x00	
	
//selgdt 0x18
/* flat data segment */
	.word	0xffff, 0x0000		
	.byte	0x00, 0x93, 0xcf, 0x00	

//selgdt 0x20
/* flat rom offset code segment */
	.word	0xffff, _rom_code_seg_base_low		
	.byte	_rom_code_seg_base_middle, 0x9b, 0xcf
	.byte	_rom_code_seg_base_high


#if defined(CONFIG_VGABIOS) && (CONFIG_VGABIOS == 1)
	// from monty:
	/* 0x00009a00,0000ffffULL,   20h: 16-bit 64k code at 0x00000000 */
        /* 0x00009200,0000ffffULL    28h: 16-bit 64k data at 0x00000000 */
// selgdt 0x28
/*16-bit 64k code at 0x00000000 */
	.word 0xffff, 0x0000
	.byte 0, 0x9a, 0, 0

// selgdt 0x30
/*16-bit 64k data at 0x00000000 */
	.word 0xffff, 0x0000
	.byte 0, 0x92, 0, 0
#endif // defined(CONFIG_VGABIOS) && (CONFIG_VGABIOS == 1)
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
	ljmp	$0x10, $__protected_start
	
__protected_start:
	intel_chip_post_macro(0x10)	/* post 10 */

	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

