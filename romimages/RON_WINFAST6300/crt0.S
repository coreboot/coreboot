/*
 * $ $
 *
 */

#include <asm.h>
#include <intel.h>

#include <pciconf.h>
#include <northsouthbridge/sis/630/param.h>
/*
 * This is the entry code (the mkrom(8) utility makes a jumpvector
 * to this adddess. 
 *
 * When we get here we are in x86 real mode.
 *
 *	%cs	= 0xf000	%ip	= 0x0000
 *	%ds	= 0x0000	%es	= 0x0000
 *	%dx	= 0x0yxx	(y = 3 for i386, 5 for pentium, 6 for P6,
 *					where x is undefined)
 *	%fl	= 0x0002
 */
 	.text
	.code16

#include <cpu/p5/start32.inc>

	/* initialize the RAM */
	/* different for each motherboard */
#include <northsouthbridge/sis/630/raminit.inc>

/*
 *	Copy data into RAM and clear the BSS. Since these segments
 *	isn't really that big we just copy/clear using bytes, not
 *	double words.
 */
	intel_chip_post_macro(0x11)		/* post 11 */

	cld				/* clear direction flag */

	/* copy data segment from FLASH ROM to RAM */
	leal	EXT(_ldata), %esi
	leal	EXT(_data), %edi
	movl	$EXT(_eldata), %ecx
	subl	%esi, %ecx
	jz	.Lnodata		/* should not happen */
	rep
	movsb
.Lnodata:
	intel_chip_post_macro(0x12)		/* post 12 */
	
	/** clear stack */
	xorl	%edi, %edi
	movl	$_PDATABASE, %ecx
	xorl	%eax, %eax
	rep
	stosb

	/** clear bss */
	leal	EXT(_bss), %edi
	movl	$EXT(_ebss), %ecx
	subl	%edi, %ecx
	jz	.Lnobss
	xorl	%eax, %eax
	rep
	stosb
.Lnobss:
	
/*
 *	Now we are finished. Memory is up, data is copied and
 *	bss is cleared.   Now we call the ``main´´ routine and
 *	let it do the rest.
 */ 
	intel_chip_post_macro(0xfe)	/* post fe */

/* memory is up. Let's do the rest in C -- much easier. */

	/* set new stack */
	movl	$_PDATABASE, %esp

	call	EXT(intel_main)
	/*NOTREACHED*/
.Lhlt:	hlt
	jmp	.Lhlt

