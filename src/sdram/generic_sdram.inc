jmp ram_initialize

	/* Estimate that SLOW_DOWN_IO takes about 50&76us*/
	/* delay for 200us */

#define DO_DELAY \
	movl $4, %edi		; \
1:	SLOW_DOWN_IO		; \
	decl %edi		; \
	jnz 1b
		
		

ram_1:		.string "Ram1\r\n"
ram_2:		.string "Ram2\r\n"
ram_3:		.string "Ram3\r\n"
ram_4:		.string "Ram4\r\n"
ram_5:		.string "Ram5\r\n"
ram_6:		.string "Ram6\r\n"
ram_7:		.string "Ram7\r\n"
ram_8:		.string "Ram8\r\n"
ram_9:		.string "Ram9\r\n"
ram_10:		.string "Ram10\r\n"
ram_11:		.string "Ram11\r\n"
ram_12:		.string "Ram12\r\n"
ram_13:		.string "Ram13\r\n"
ram_14:		.string "Ram14\r\n"
ram_15:		.string "Ram15\r\n"
ram_16:		.string "Ram16\r\n"
ram_17:		.string "Ram17\r\n"
ram_18:		.string "Ram18\r\n"
ram_19:		.string "Ram19\r\n"
ram_20:		.string "Ram20\r\n"
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


/*	now the fun begins.
	turn on the dram and wait a while (this from the intel book)
	turn power on and set the nop bit too
 */
	TTYS0_TX_STRING($ram_4)
	/* SDRAMC */
	SET_RAM_COMMAND(RAM_COMMAND_NOP)

	DO_DELAY

	ASSERT_RAM_COMMAND() /* nop command */

	/* Precharge all */
	SET_RAM_COMMAND(RAM_COMMAND_PRECHARGE)
	ASSERT_RAM_COMMAND()
	
	/* wait until the all banks idle state... */

	TTYS0_TX_STRING($ram_5)

	/* Now we need 8 AUTO REFRESH / CBR cycles to be performed */

	SET_RAM_COMMAND(RAM_COMMAND_CBR)
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()

	TTYS0_TX_STRING($ram_6)

	/* mode register set */
	SET_RAM_MODE_REGISTER
	
	/* MAx[14:0] lines,
	 * MAx[2:0 ] 010 == burst mode of 4
	 * MAx[3:3 ] 1 == interleave wrap type
	 * MAx[4:4 ] == CAS# latency bit
	 * MAx[6:5 ] == 01
	 * MAx[12:7] ==	0
	 */

	TTYS0_TX_STRING($ram_7)

	/* normal operation */
	SET_RAM_COMMAND(RAM_COMMAND_NONE)
		
	TTYS0_TX_STRING($ram_8)


	/* Finally enable refresh */
	/* DRAMC */
	ENABLE_REFRESH()

	TTYS0_TX_STRING($ram_9)
