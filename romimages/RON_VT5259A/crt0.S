/*
 * $ $
 *
 */

#include <asm.h>
#include <intel.h>

#include <pciconf.h>

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

#include "intel_start32.S"

/* turn on serial */
#include "VIA_VT82C686A.S"

#include "serial.S"

	TTYS0_TX_STRING($ttyS0_test)
	
	/* initialize the RAM */
	/* different for each motherboard */
	
#include "intel_pm133ram.S"

#ifdef RAMTEST
#include "ramtest.S"

#include <cpu/p6/earlymtrr.inc>
	mov $0x00000000, %eax
	mov $0x0009ffff, %ebx
	mov $16, %ecx

	CALLSP(ramtest)
#endif	
/*
 *	Copy data into RAM and clear the BSS. Since these segments
 *	isn't really that big we just copy/clear using bytes, not
 *	double words.
 */
	intel_chip_post_macro(0x11)	/* post 11 */
	TTYS0_TX_STRING($str_after_ram)

	cld				/* clear direction flag */
	leal	EXT(_ldata), %esi
	leal	EXT(_data), %edi
	movl	$EXT(_eldata), %ecx
	subl	%esi, %ecx
	jz	.Lnodata		/* should not happen */
	rep
	movsb
.Lnodata:
	intel_chip_post_macro(0x12)	/* post 12 */
	TTYS0_TX_STRING($str_after_copy)
	
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
	TTYS0_TX_STRING($str_pre_main)

        /* set new stack */
        movl    $_PDATABASE, %esp
/* memory is up. Let's do the rest in C -- much easier. */
	call	EXT(intel_main)
	/*NOTREACHED*/
.Lhlt:	hlt
	jmp	.Lhlt

ttyS0_test:	.string "\r\n\r\nHello world!!\r\n"
str_after_ram:		.string "Ram Initialize?\r\n"
str_after_copy:		.string "after copy?\r\n"
str_pre_main:	.string "before main\r\n"
newline:	.string "\r\n"


