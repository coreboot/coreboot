jmp generic_sdram_enable_out

ram_enable_1:	.string "Ram Enable 1\r\n"
ram_enable_2:	.string "Ram Enable 2\r\n"
ram_enable_3:	.string "Ram Enable 3\r\n"
ram_enable_4:	.string "Ram Enable 4\r\n"
ram_enable_5:	.string "Ram Enable 5\r\n"

	/* Estimate that SLOW_DOWN_IO takes about 50&76us*/
	/* delay for 200us */

#define DO_DELAY \
	movl $4, %edi		; \
1:	SLOW_DOWN_IO		; \
	decl %edi		; \
	jnz 1b
		

enable_sdram:
/*	now the fun begins.
	turn on the dram and wait a while (this from the intel book)
	turn power on and set the nop bit too
 */
	TTYS0_TX_STRING($ram_enable_1)
	/* SDRAMC */
	SET_RAM_COMMAND(RAM_COMMAND_NOP)

	DO_DELAY

	ASSERT_RAM_COMMAND() /* nop command */

	/* Precharge all */
	SET_RAM_COMMAND(RAM_COMMAND_PRECHARGE)
	ASSERT_RAM_COMMAND()
	
	/* wait until the all banks idle state... */

	TTYS0_TX_STRING($ram_enable_2)

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

	TTYS0_TX_STRING($ram_enable_3)

	/* mode register set */
	SET_RAM_MODE_REGISTER
	
	/* MAx[14:0] lines,
	 * MAx[2:0 ] 010 == burst mode of 4
	 * MAx[3:3 ] 1 == interleave wrap type
	 * MAx[4:4 ] == CAS# latency bit
	 * MAx[6:5 ] == 01
	 * MAx[12:7] ==	0
	 */

	TTYS0_TX_STRING($ram_enable_4)

	/* normal operation */
	SET_RAM_COMMAND(RAM_COMMAND_NONE)

	TTYS0_TX_STRING($ram_enable_5)
	RET_LABEL(enable_sdram)

generic_sdram_enable_out:
