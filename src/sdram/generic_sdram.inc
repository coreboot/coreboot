jmp ram_initialize

ram_1:		.string "Ram1\r\n"
ram_2:		.string "Ram2\r\n"
ram_3:		.string "Ram3\r\n"
ram_4:		.string "Ram4\r\n"
ram_5:		.string "Ram5\r\n"
ram_6:		.string "Ram6\r\n"


no_memory_str:	.string "No memory!\r\n"

no_memory:	TTYS0_TX_STRING($no_memory_str)
1:		hlt /* stick here.. */
		jmp 1b

ram_initialize:
	/* this sets up the dram. */

	TTYS0_TX_STRING($ram_1)

	/* Now, set registers we can set once to reasonable values  */

	CALL_LABEL(ram_set_registers)

	TTYS0_TX_STRING($ram_2)
	
	/* Now set those things we can auto detect */
	CALL_LABEL(ram_set_spd_registers)

	TTYS0_TX_STRING($ram_3)

	/* Now that everything is setup enable the SDRAM
	 * Some chipsets do the work for us while on others
	 * we need to do the work by hand.
	 */
	CALL_LABEL(enable_sdram)

	TTYS0_TX_STRING($ram_4)

	FIRST_NORMAL_REFERENCE()
	TTYS0_TX_STRING($ram_5)

	/* Finally enable refresh */
	ENABLE_REFRESH()

	SPECIAL_FINISHUP()

	TTYS0_TX_STRING($ram_6)

