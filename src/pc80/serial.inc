#include <part/fallback_boot.h>


/* Base Address */
#ifndef TTYS0_BASE
#define TTYS0_BASE	0x3f8
#endif

/* Baud Rate */
#ifndef TTYS0_BAUD
#define TTYS0_BAUD 115200
#endif

#if ((115200%TTYS0_BAUD) != 0)
#error Bad ttys0 baud rate
#endif

/* Baud Rate Divisor */
#define TTYS0_DIV	(115200/TTYS0_BAUD)
#define TTYS0_DIV_LO	(TTYS0_DIV&0xFF)
#define TTYS0_DIV_HI	((TTYS0_DIV >> 8)&0xFF)

/* Line Control Settings */
#ifndef TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define TTYS0_LCS	0x3
#endif

/* Data */
#define TTYS0_RBR (TTYS0_BASE+0x00)

/* Control */
#define TTYS0_TBR TTYS0_RBR
#define TTYS0_IER (TTYS0_BASE+0x01)
#define TTYS0_IIR (TTYS0_BASE+0x02)
#define TTYS0_FCR TTYS0_IIR
#define TTYS0_LCR (TTYS0_BASE+0x03)
#define TTYS0_MCR (TTYS0_BASE+0x04)
#define TTYS0_DLL TTYS0_RBR
#define TTYS0_DLM TTYS0_IER

/* Status */
#define TTYS0_LSR (TTYS0_BASE+0x05)
#define TTYS0_MSR (TTYS0_BASE+0x06)
#define TTYS0_SCR (TTYS0_BASE+0x07)

#if USE_OPTION_TABLE == 1
.section ".rom.data"
	.type	 div,@object
	.size	 div,8
div:
.byte 1,2,3,5,12,24,48,96

.previous
#endif

	jmp	serial0

	/* uses:	ax, dx */
#define TTYS0_TX_AL		\
	mov	%al, %ah	; \
9:	mov	$TTYS0_LSR, %dx	; \
	inb	%dx, %al	; \
	test	$0x20, %al	; \
	je	9b		; \
	mov	$TTYS0_TBR, %dx	; \
	mov	%ah, %al	; \
	outb	%al, %dx


serial0:
	/* Set 115.2Kbps,8n1 */
	/* Set 8bit, 1 stop bit, no parity, DLAB */
	mov	$TTYS0_LCR, %dx
	mov	$(TTYS0_LCS | 0x80), %al
	out	%al, %dx

	/* set Baud Rate Divisor to 1 ==> 115200 Buad */
#if USE_OPTION_TABLE == 1
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	xorl	%edx,%edx
	inb	$0x71, %al
	sarb	$2,%al
	andb	$7,%al
	movb	%al,%dl
	movb	div(%edx),%al
	mov	$TTYS0_DLL, %dx
	out	%al, %dx
	mov	$TTYS0_DLM, %dx
	xorb	%al,%al
	out	%al, %dx
#else
	mov	$TTYS0_DLL, %dx
	mov	$TTYS0_DIV_LO, %al
	out	%al, %dx
	mov	$TTYS0_DLM, %dx
	mov	$TTYS0_DIV_HI, %al
	out	%al, %dx
#endif
	/* Disable DLAB */
	mov	$TTYS0_LCR, %dx
	mov	$(TTYS0_LCS & 0x7f), %al
	out	%al, %dx


