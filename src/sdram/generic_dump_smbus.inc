dump_smbus_registers:
	movl	$((0 << 8) | 0x00), %ebx
dump_smbus_reg_dimm:
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	CONSOLE_DEBUG_TX_CHAR($'d')
	CONSOLE_DEBUG_TX_CHAR($'i')
	CONSOLE_DEBUG_TX_CHAR($'m')
	CONSOLE_DEBUG_TX_CHAR($'m')
	CONSOLE_DEBUG_TX_CHAR($' ')
	CONSOLE_DEBUG_TX_HEX8(%bl)
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
dump_smbus_reg_byte:
	CALLSP(smbus_read_byte)
	jz	dump_smbus_reg_next_dimm

	CONSOLE_DEBUG_TX_HEX8(%al)
	CONSOLE_DEBUG_TX_CHAR($' ')
	incb	%bh
	testb	$0x0F, %bh
	jnz	dump_smbus_reg_next_byte
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')

dump_smbus_reg_next_byte:
	cmpb	$0, %bh
	jne	dump_smbus_reg_byte

dump_smbus_reg_next_dimm:
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	xorb	%bh, %bh
	addb	$1, %bl
	cmpb	$0x80, %bl
	jne dump_smbus_reg_dimm
dump_smbus_registers_out:
