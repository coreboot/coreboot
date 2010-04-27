
jmp	console0

#define __STR(X) #X
#define STR(X) __STR(X)


#undef STR
	/* uses:	ax, dx */
#if defined(SERIAL_CONSOLE)
#define __CONSOLE_INLINE_TX_AL TTYS0_TX_AL
#else
#define __CONSOLE_INLINE_TX_AL
#endif

	/* uses:	esp, ax, dx */
#define __CONSOLE_TX_CHAR(byte)	\
	mov	byte, %al	; \
	CALLSP(console_tx_al)

	/* uses:	 ax, dx */
#define __CONSOLE_INLINE_TX_CHAR(byte)	\
	mov	byte, %al	; \
	__CONSOLE_INLINE_TX_AL

	/* uses:	esp, ax, edx */
#define __CONSOLE_TX_HEX8(byte)	\
	mov	byte, %al	; \
	CALLSP(console_tx_hex8)

	/* uses:	 byte, ax, dx */
#define __CONSOLE_INLINE_TX_HEX8(byte)	\
	movb	byte, %dl	; \
	shll	$16, %edx	; \
	shr	$4, %al		; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL	; \
	shrl	$16, %edx	; \
	movb	%dl, %al	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL

	/* uses:	esp, eax, ebx, dx */
#define __CONSOLE_TX_HEX32(lword)	\
	mov	lword, %eax	; \
	CALLSP(console_tx_hex32)

	/* uses:	eax, lword, dx */
#define __CONSOLE_INLINE_TX_HEX32(lword)	\
	mov	lword, %eax	; \
	shr	$28, %eax	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$24, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$20, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$16, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$12, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$8, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL		; \
				; \
	mov	lword, %eax	; \
	shr	$4, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL		; \
				; \
	mov	lword, %eax	; \
	and	$0x0f, %al	; \
	add	$'0', %al	; \
	cmp	$'9', %al	; \
	jle	9f		; \
	add	$39, %al	; \
9:				; \
	__CONSOLE_INLINE_TX_AL


	/* uses:	 esp, ebx, ax, dx */
#define __CONSOLE_TX_STRING(string)	\
	mov	string, %ebx	; \
	CALLSP(console_tx_string)

	/* uses:	 ebx, ax, dx */
#define __CONSOLE_INLINE_TX_STRING(string)	\
	movl	string, %ebx	; \
10:	movb	(%ebx), %al	; \
	incl	%ebx		; \
	testb	%al, %al	; \
	jz	11f		; \
	__CONSOLE_INLINE_TX_AL	; \
	jmp	10b		; \
11:


#define CONSOLE_EMERG_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_EMERG_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_EMERG_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_EMERG_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_EMERG_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_EMERG_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_EMERG_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_EMERG_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_ALERT_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_ALERT_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_ALERT_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_ALERT_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_ALERT_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_ALERT_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_ALERT_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_ALERT_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_CRIT_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_CRIT_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_CRIT_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_CRIT_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_CRIT_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_CRIT_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_CRIT_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_CRIT_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_ERR_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_ERR_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_ERR_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_ERR_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_ERR_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_ERR_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_ERR_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_ERR_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_WARNING_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_WARNING_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_WARNING_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_WARNING_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_WARNING_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_WARNING_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_WARNING_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_WARNING_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_NOTICE_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_NOTICE_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_NOTICE_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_NOTICE_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_NOTICE_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_NOTICE_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_NOTICE_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_NOTICE_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_INFO_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_INFO_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_INFO_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_INFO_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_INFO_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_INFO_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_INFO_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_INFO_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_DEBUG_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_DEBUG_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_DEBUG_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_DEBUG_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_DEBUG_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_DEBUG_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_DEBUG_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_DEBUG_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

#define CONSOLE_SPEW_TX_CHAR(byte)            __CONSOLE_TX_CHAR(byte)
#define CONSOLE_SPEW_INLINE_TX_CHAR(byte)     __CONSOLE_INLINE_TX_CHAR(byte)
#define CONSOLE_SPEW_TX_HEX8(byte)            __CONSOLE_TX_HEX8(byte)
#define CONSOLE_SPEW_INLINE_TX_HEX8(byte)     __CONSOLE_INLINE_TX_HEX8(byte)
#define CONSOLE_SPEW_TX_HEX32(lword)          __CONSOLE_TX_HEX32(lword)
#define CONSOLE_SPEW_INLINE_TX_HEX32(lword)   __CONSOLE_INLINE_TX_HEX32(lword)
#define CONSOLE_SPEW_TX_STRING(string)        __CONSOLE_TX_STRING(string)
#define CONSOLE_SPEW_INLINE_TX_STRING(string) __CONSOLE_INLINE_TX_STRING(string)

	/* uses:	esp, ax, dx */
console_tx_al:
	__CONSOLE_INLINE_TX_AL
	RETSP

	/* uses:	 esp, ax, edx */
console_tx_hex8:
	__CONSOLE_INLINE_TX_HEX8(%al)
	RETSP


	/* uses:	 esp, ebx, eax, dx */
console_tx_hex32:
	mov	%eax, %ebx
	shr	$28, %eax
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL

	mov	%ebx, %eax
	shr	$24, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL

	mov	%ebx, %eax
	shr	$20, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL

	mov	%ebx, %eax
	shr	$16, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL

	mov	%ebx, %eax
	shr	$12, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL

	mov	%ebx, %eax
	shr	$8, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL

	mov	%ebx, %eax
	shr	$4, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL

	mov	%ebx, %eax
	and	$0x0f, %al
	add	$'0', %al
	cmp	$'9', %al
	jle	9f
	add	$39, %al
9:
	__CONSOLE_INLINE_TX_AL
	RETSP

	/* Uses esp, ebx, ax, dx  */

console_tx_string:
	mov	(%ebx), %al
	inc	%ebx
	cmp	$0, %al
	jne	9f
	RETSP
9:
	__CONSOLE_INLINE_TX_AL
	jmp	console_tx_string

console0:
