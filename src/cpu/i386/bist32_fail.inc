

	jmp bist32_fail_0
bist32_fail:
	movl	%eax, %ebp

#if 1
#define SIO_BASE  0x2e
#define SIO_INDEX SIO_BASE
#define SIO_DATA  SIO_BASE+1
#define SIO_WRITE_CONFIG(value, reg) \
	movb	reg, %al		; \
	outb	%al, $(SIO_INDEX)	; \
	movb	value, %al		; \
	outb	%al, $(SIO_DATA)

#define SIO_READ_CONFIG(reg) \
	movb	reg, %al		; \
	outb	%al, $(SIO_INDEX)	; \
	inb	$(SIO_DATA), %al

#define SIO_SET_LOGICAL_DEVICE(device) \
	SIO_WRITE_CONFIG(device, $0x07)

	/* Enable serial 1 */
	SIO_SET_LOGICAL_DEVICE($3)
	SIO_WRITE_CONFIG($1, $0x30)
	SIO_WRITE_CONFIG($0x3, $0x60)
	SIO_WRITE_CONFIG($0xf8, $0x61)

#endif	
	CALLSP(serial_init)
	CONSOLE_DEBUG_TX_STRING($str_bist_failed)
	CONSOLE_DEBUG_TX_HEX32(%ebp)
	CONSOLE_DEBUG_TX_STRING($str_bist_newline)
	jmp .Lhlt

bist32_fail_0:

.section ".rom.data"
str_bist_failed:	.string "BIST failed: "
str_bist_newline:	.string "\r\n"
.previous

