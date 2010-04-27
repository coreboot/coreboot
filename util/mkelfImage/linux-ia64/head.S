#define ASSEMBLY 1

	.text

#include "convert.h"

	.globl _start
	.proc _start
_start:
	alloc	loc0=ar.pfs,1,2,3,0 /* in, local, out, rotating */
	mov	loc1=rp
	mov	r14=ip	/* Get the address of _start, I'm in the first bundle */
	movl	r15=@gprel(_start)
	;;
	sub	gp=r14,r15	/* gp = _start - @gprel(_start), current value of gp */
	;;
	mov	out0=in0
	mov	out1=r28
	add	out2=@gprel(params),gp
	br.call.sptk.few rp=convert_params


	mov	r28=r8
	add	r15=@gprel(entry), gp
	;;
	ld8	r16=[r15]
	;;
	mov	b1=r16
	mov	ar.pfs=loc0
	mov	rp=loc1
	;;
	br.cond.sptk.few b1

	.size _start, . - _start
	.endp _start


#if 0

/* Base Address */
#define UART_BASE	0x00000000f8030000
#define UART_BAUD	9600

/* Data */
#define UART_RBR	0x00
#define UART_TBR	0x00
/* Control */
#define UART_IER	0x01
#define UART_IIR	0x02
#define UART_FCR	0x02
#define UART_LCR	0x03
#define UART_MCR	0x04

#define UART_DLL	0x00
#define UART_DLM	0x01
/* Status */
#define UART_LSR	0x05
#define UART_MSR	0x06
#define UART_SCR	0x07

#define UART_PHYS_BASE	(0x8000000000000000|UART_BASE)
#define UART_DIV	(115200/UART_BAUD)
#define UART_DIV_LO	(UART_DIV&0xFF)
#define UART_DIV_HI	((UART_DIV >> 8)&0xFF)

#if ((115200%UART_BAUD) != 0)
#error Bad uart baud rate
#endif

/* NOTE: As these are debugging functions, they do not consume any
 * space on the register stack, and instead rely entirely on
 * scratch registers for the registers they use.
 */
uart_init:
	/* set the UART_BASE */
	movl	r31=UART_PHYS_BASE
	;;

	/* disable interrupts */
	add	r30=UART_IER,r31
	mov	r29=0x00
	;;
	st1	[r30]=r29

	/* enable fifos */
	add	r30=UART_FCR,r31
	mov	r29=0x01
	;;
	st1	[r30]=r29

	/* Set Baud Rate Divisor to UART_BAUD */
	add	r30=UART_LCR,r31
	mov	r29=0x83
	;;
	st1	[r30]=r29

	add	r30=UART_DLL,r31
	mov	r29=UART_DIV_LO
	;;
	st1	[r30]=r29

	add	r30=UART_DLM,r31
	mov	r29=UART_DIV_HI
	;;
	st1	[r30]=r29

	add	r30=UART_LCR,r31
	mov	r29=0x03
	;;
	st1	[r30]=r29

	br.ret.sptk.few rp

	.proc	uart_tx_byte
	.globl  uart_tx_byte
uart_tx_byte:
	/* set the UART_PHYS_BASE */
	movl	r31=UART_PHYS_BASE
	;;
__uart_tx_byte:
	/* Wait until the UART can hold another byte */
	add	r30=UART_LSR,r31
	;;
9:	ld1.acq.nta	r29=[r30]
	;;
	and	r29=0x20,r29
	;;
	cmp.eq	p63,p0=0,r29
(p63)	br.cond.sptk.few	9b

	/* Transmit the byte */
	add	r30=UART_TBR,r31
	;;
	st1.rel.nta	[r30]=r32
	;;

	/* Wait until the UART is empty to be certain the byte is flushed */
	add	r30=UART_LSR,r31
	;;
9:	ld1.acq.nta	r29=[r30]
	;;
	and	r29=0x40,r29
	;;
	cmp.eq	p63,p0=0,r29
(p63)	br.cond.sptk.few	9b
	;;
	br.ret.sptk.few	rp
	.endp uart_tx_byte

__uart_tx_hex_char:
	cmp.ltu	p62,p63=9,r32
	;;
(p63)	add	r32=48,r32	/* digits*/
(p62)	add	r32=55,r32	/* letters */
	br.cond.sptk.few __uart_tx_byte

uart_tx_hex64:
	/* set the UART_bASE */
	movl	r31=UART_PHYS_BASE
	/* skip r28 */
	mov	r27=rp
	mov	r26=ar.pfs
	mov	r25=r32
	;;
	extr.u	r32=r25,60,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,56,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,52,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,48,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,44,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,40,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,36,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,32,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,28,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,24,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,20,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,16,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,12,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,8,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,4,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	extr.u	r32=r25,0,4
	br.call.sptk.few rp=__uart_tx_hex_char
	;;
	mov	ar.pfs = r26
	mov	rp = r27
	;;
	br.ret.sptk.few	rp
#endif

	.section ".trailer", "a"
	/* Constants set at build time, these are at the very end of my image */
	.balign 16
	.global params
params:
convert_magic:
	.quad	CONVERT_MAGIC
entry:
	.quad	0
initrd_start:
	.quad	0
initrd_size:
	.quad	0
cmdline:
	.asciz "BOOT_IMAGE=head.S console=ttyS0 ip=dhcp root=/dev/nfs"
	.org cmdline + 1024, 0
cmdline_end:
