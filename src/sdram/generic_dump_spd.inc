dump_spd_registers:
	movl $((0 << 8) | SMBUS_MEM_DEVICE_START), %ebx
dump_spd_reg_dimm:
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	CONSOLE_DEBUG_TX_CHAR($'d')
	CONSOLE_DEBUG_TX_CHAR($'i')
	CONSOLE_DEBUG_TX_CHAR($'m')
	CONSOLE_DEBUG_TX_CHAR($'m')
	CONSOLE_DEBUG_TX_CHAR($' ')
	movb %bl, %al
	CALLSP(ttys0_tx_hex8)	
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
dump_spd_reg_byte:
	CALLSP(smbus_read_byte)
	jz dump_spd_reg_next_dimm

	CALLSP(ttys0_tx_hex8)
	CONSOLE_DEBUG_TX_CHAR($' ')
	incb %bh
	testb $0x0F, %bh
	jnz dump_spd_reg_next_byte
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')

dump_spd_reg_next_byte:
	cmpb $0, %bh
	jne dump_spd_reg_byte

dump_spd_reg_next_dimm:
	CONSOLE_DEBUG_TX_CHAR($'\r')
	CONSOLE_DEBUG_TX_CHAR($'\n')
	xorb %bh, %bh
	add $SMBUS_MEM_DEVICE_INC, %bl
	cmpb $(SMBUS_MEM_DEVICE_END + SMBUS_MEM_DEVICE_INC), %bl
	jne dump_spd_reg_dimm
dump_spd_registers_out:
