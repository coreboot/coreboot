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

/*
 * Putting any code before the gdt tables breaks things
 * because the lgdt instruction is hand assembled/hard coded.
 */

#include <cpu/p5/start32.inc>

	jmp 1f

	/* ========================================================== */
	
separator:	.string "\r\n============================================\r\n"
greeting:	.string	"ISR Packbot BIOS.\r\n"

#include "date.h"
date:		.string DATE

done:		.string "done.\r\n"
initram:	.string "Initializing SDRAM..."
initdata:	.string "Initializing DATA, clearing BSS and STACK..."
jumpmain:	.string "Jumping to intel_main()..."
delaytest:	.string "Delay test...  "

	/* ========================================================== */
	
1:

	intel_chip_post_macro(0x02)

#if  defined(ITE_SUPER_IO)
#include <superio/ITE/it8671f/superio.inc>
#elif defined(SMC_SUPER_IO)
#include <superio/SMC/fdc37n769/superio.inc>
#endif

	intel_chip_post_macro(0x03)

#ifdef SERIAL_CONSOLE
#include <pc80/serial.inc>
#endif

#include <ram/ramtest.inc>

	intel_chip_post_macro(0x04)

#ifdef SERIAL_CONSOLE
	TTYS0_TX_STRING($separator)
	TTYS0_TX_STRING($greeting)
	TTYS0_TX_STRING($date)
	TTYS0_TX_CHAR($'\n')
#endif
	intel_chip_post_macro(0x05)

	/* initialize the RAM */
	/* different for each motherboard */
	
#include <northbridge/intel/440bx/raminit.inc>

	intel_chip_post_macro(0x20)

#include <cpu/p6/earlymtrr.inc>
	
	intel_chip_post_macro(0x21)

	mov $0x00000000, %eax
	mov $0x0009ffff, %ebx
	mov $16, %ecx

	CALLSP(ramtest)

	intel_chip_post_macro(0x23)

	cmp $16, %ecx
	je 1f
	
	intel_chip_post_macro(0x24)

	jmp .Lhlt
1:

	intel_chip_post_macro(0x25)

	/* Figure out how much RAM is configured */
	
	CS_READ($0x67)
	mov $0, %ebx
	mov %al, %bl
	shl $23, %ebx
	sub $1, %ebx

	mov $0x00100000, %eax
	mov $16, %ecx

	CALLSP(ramtest)

	intel_chip_post_macro(0x26)

	cmp $16, %ecx
	je 1f
	intel_chip_post_macro(0x27)
	jmp .Lhlt
1:

	intel_chip_post_macro(0x30)

#ifdef SERIAL_CONSOLE
	TTYS0_TX_STRING($initdata)
#endif

/*
 *	Copy data into RAM and clear the BSS. Since these segments
 *	isn't really that big we just copy/clear using bytes, not
 *	double words.
 */

	cld				/* clear direction flag */
	leal	EXT(_ldata), %esi
	leal	EXT(_data), %edi
	movl	$EXT(_eldata), %ecx
	subl	%esi, %ecx
	jz	.Lnodata		/* should not happen */
	rep
	movsb
.Lnodata:
	intel_chip_post_macro(0x31)
	
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
	
#ifdef SERIAL_CONSOLE
	TTYS0_TX_STRING($done)
#endif
	intel_chip_post_macro(0x3f)

/*
 *	Now we are finished. Memory is up, data is copied and
 *	bss is cleared.   Now we call the ``main´´ routine and
 *	let it do the rest.
 */ 

#ifdef SERIAL_CONSOLE
	TTYS0_TX_STRING($jumpmain)
#endif

/* memory is up. Let's do the rest in C -- much easier. */
	/* set new stack */
	movl	$_PDATABASE, %esp
	call	EXT(intel_main)
	/*NOTREACHED*/
.Lhlt:	hlt
	jmp	.Lhlt

