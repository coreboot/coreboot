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


	/* uses:	esp, ax, dx */
#define TTYS0_TX_CHAR(byte)	\
	mov	byte, %al	; \
	CALLSP(ttys0_tx_al)

	/* uses:	 ax, dx */
#define TTYS0_INLINE_TX_CHAR(byte)	\
	mov	byte, %al	; \
	TTYS0_TX_AL

	/* uses:	esp, ax, dx */
#define TTYS0_TX_HEX8(byte)	\
	mov	byte, %al	; \
	CALLSP(ttys0_tx_hex8)

	/* uses:	 ax, dx */
#define TTYS0_INLINE_TX_HEX8(byte)	\
	mov	byte, %al	; \
	mov	$TTYS0_SCR, %dx	; \
	outb	%al, %dx	; \
	shr	$4, %al		; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
	mov	$TTYS0_SCR, %dx	; \
	inb	%dx, %al	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
	mov	$TTYS0_SCR, %dx	; \
	inb	%dx, %al

	/* uses:	esp, eax, ebx, dx */
#define TTYS0_TX_HEX32(lword)	\
	mov	lword, %eax	; \
	CALLSP(ttys0_tx_hex32)

	/* uses:	eax, lword, dx */
#define TTYS0_INLINE_TX_HEX32(lword)	\
	mov	lword, %eax	; \
	shr	$28, %eax	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$24, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$20, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$16, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$12, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$8, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$4, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL		; \
				; \
	mov	lword, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	TTYS0_TX_AL


	/* uses:	 esp, ebx, ax, dx */
#define TTYS0_TX_STRING(string)	\
	mov	string, %ebx	; \
	CALLSP(ttys0_tx_string)

	/* uses:	 ebx, ax, dx */
#define TTYS0_INLINE_TX_STRING(string)	\
	movl	string, %ebx	; \
10:	movb	(%ebx), %al	; \
	incl	%ebx		; \
	testb	%al, %al	; \
	jz	11f		; \
	TTYS0_TX_AL		; \
	jmp	10b		; \
11:	


	/* uses:	 esp, ax, dx */
ttys0_tx_al:
	TTYS0_TX_AL
	RETSP

	/* uses:	 esp, ax, dx */
ttys0_tx_hex8:
	mov	$TTYS0_SCR, %dx
	outb	%al, %dx
	shr	$4, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL
	mov	$TTYS0_SCR, %dx
	inb	%dx, %al
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL
	RETSP

	/* uses:	 esp, ebx, eax, dx */
ttys0_tx_hex32:
	mov	%eax, %ebx
	shr	$28, %eax
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL

	mov	%ebx, %eax
	shr	$24, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL

	mov	%ebx, %eax
	shr	$20, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL

	mov	%ebx, %eax
	shr	$16, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL

	mov	%ebx, %eax
	shr	$12, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL

	mov	%ebx, %eax
	shr	$8, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL

	mov	%ebx, %eax
	shr	$4, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL

	mov	%ebx, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	TTYS0_TX_AL
	RETSP

	/* Uses esp, ebx, ax, dx  */

ttys0_tx_string:
	mov	(%ebx), %al
	inc	%ebx
	cmp	$0, %al
	jne	9f
	RETSP
9:	
	TTYS0_TX_AL
	jmp	ttys0_tx_string

serial0:
	/* Set 115.2Kbps,8n1 */
	/* Set 8bit, 1 stop bit, no parity, DLAB */
	mov	$TTYS0_LCR, %dx
	mov	$(TTYS0_LCS | 0x80), %al
	out	%al, %dx

	/* set Baud Rate Divisor to 1 ==> 115200 Buad */
	mov	$TTYS0_DLL, %dx
	mov	$TTYS0_DIV_LO, %al
	out	%al, %dx
	mov	$TTYS0_DLM, %dx
	mov	$TTYS0_DIV_HI, %al
	out	%al, %dx

	/* Disable DLAB */
	mov	$TTYS0_LCR, %dx
	mov	$(TTYS0_LCS & 0x7f), %al
	out	%al, %dx

	TTYS0_TX_STRING($ttyS0_test)

