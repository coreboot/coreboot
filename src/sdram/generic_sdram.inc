jmp ram_initialize

ram_1:		.string "Ram1\r\n"
ram_2:		.string "Ram2\r\n"
ram_3:		.string "Ram3\r\n"
ram_4:		.string "Ram4\r\n"
ram_5:		.string "Ram5\r\n"
ram_6:		.string "Ram6\r\n"

no_memory_str:	.string "ERROR: No memory!"
crlf:		.string "\r\n"

no_memory:	CONSOLE_ERR_TX_STRING($no_memory_str)
mem_stop:
		CONSOLE_ERR_TX_STRING($crlf)
1:
		hlt /* stick here.. */
		jmp 1b

no_memory_smbus_str:	.string "ERROR: No memory - smbus went bad!"

no_memory_bad_smbus:	CONSOLE_ERR_TX_STRING($no_memory_smbus_str)
		CONSOLE_ERR_TX_STRING($crlf)
1:
		hlt /* stick here.. */
		jmp 1b

ram_initialize:
	/* this sets up the dram. */

	CONSOLE_DEBUG_TX_STRING($ram_1)

	/* Now, set registers we can set once to reasonable values  */

	CALL_LABEL(ram_set_registers)

	CONSOLE_DEBUG_TX_STRING($ram_2)
	
	/* Now set those things we can auto detect */
	CALL_LABEL(ram_set_spd_registers)

	CONSOLE_DEBUG_TX_STRING($ram_3)

	/* Now that everything is setup enable the SDRAM
	 * Some chipsets do the work for us while on others
	 * we need to do the work by hand.
	 */
	CALL_LABEL(enable_sdram)

	CONSOLE_DEBUG_TX_STRING($ram_4)

	FIRST_NORMAL_REFERENCE()
	CONSOLE_DEBUG_TX_STRING($ram_5)

	/* Finally enable refresh */
	ENABLE_REFRESH()

	SPECIAL_FINISHUP()

	CONSOLE_DEBUG_TX_STRING($ram_6)

