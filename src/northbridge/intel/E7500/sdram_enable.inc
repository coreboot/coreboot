jmp sdram_enable_out

ram_enable_1:	.string "Ram Enable 1\r\n"
ram_enable_2:	.string "Ram Enable 2\r\n"
ram_enable_3:	.string "Ram Enable 3\r\n"
ram_enable_4:	.string "Ram Enable 4\r\n"
ram_enable_5:	.string "Ram Enable 5\r\n"

	/* Estimate that SLOW_DOWN_IO takes about 50&76us*/
	/* delay for 200us */

#define DO_DELAY \
	movl $16, %edi		; \
1:	SLOW_DOWN_IO		; \
	decl %edi		; \
	jnz 1b
		

enable_sdram:
/*	now the fun begins.
	turn on the dram and wait a while (this from the intel book)
	turn power on and set the nop bit too
 */
	CONSOLE_DEBUG_TX_STRING($ram_enable_1)
	/* 1 & 2 Power up and start clocks */

	DO_DELAY
	DO_DELAY
	/* Apply NOP */

	SET_RAM_COMMAND(RAM_COMMAND_NOP)

	ASSERT_RAM_COMMAND() /* nop command */
	DO_DELAY

	/* 4 Precharge all */
	SET_RAM_COMMAND(RAM_COMMAND_PRECHARGE)
	ASSERT_RAM_COMMAND()
	
	/* wait until the all banks idle state... */

	CONSOLE_DEBUG_TX_STRING($ram_enable_2)
	DO_DELAY
	
	/* 5. Issue EMRS to enable DLL */
	SET_RAM_COMMAND(RAM_COMMAND_EMRS)
	movl (0x0000<<MD_SHIFT), %eax
	DO_DELAY
	
	/* 6. Reset DLL */
	SET_RAM_COMMAND(RAM_COMMAND_MRS)
#if 1
	movl (((1<<8)|(0<<7)|(5<<4)|(1<<3)|(2<<0))<<MD_SHIFT), %eax
#else
	movl (((1<<8)|(0<<7)|(2<<4)|(1<<3)|(3<<0))<<MD_SHIFT), %eax
#endif
	DO_DELAY
	
	/* 7 Precharge all */
	SET_RAM_COMMAND(RAM_COMMAND_PRECHARGE)
	ASSERT_RAM_COMMAND()
	
	DO_DELAY

	/* 8 Now we need 8 AUTO REFRESH / CBR cycles to be performed */
#if 0
	SET_RAM_COMMAND(RAM_COMMAND_CBR)
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
	ASSERT_RAM_COMMAND()
#else
	SET_RAM_COMMAND(RAM_COMMAND_CBR)
	ASSERT_RAM_COMMAND()
	DO_DELAY
	SET_RAM_COMMAND(RAM_COMMAND_CBR)
	ASSERT_RAM_COMMAND()
	DO_DELAY
#endif

	CONSOLE_DEBUG_TX_STRING($ram_enable_3)

	/* 9 mode register set */
	SET_RAM_COMMAND(RAM_COMMAND_MRS)
#if 1
	movl (((0<<8)|(0<<7)|(5<<4)|(1<<3)|(2<<0))<<MD_SHIFT), %eax
#else
	movl (((0<<8)|(0<<7)|(2<<4)|(1<<3)|(3<<0))<<MD_SHIFT), %eax
#endif
	DO_DELAY	
	
	/* MAx[14:0] lines,
	 * MAx[2:0 ] 010 == burst mode of 4
	 * MAx[3:3 ] 1 == interleave wrap type
	 * MAx[6:4 ] 110 == CAS# latency bits
	 * MAx[7:7 ] 0 == mode normal
	 * MAx[8:8 ] 0 == DLL reset no 
	 * MAx[13:13 ] 0 == MRS
	 * MAx[others] 0 == reserved
	 */

	CONSOLE_DEBUG_TX_STRING($ram_enable_4)

	/* normal operation */
	SET_RAM_COMMAND(RAM_COMMAND_NORMAL)

	CONSOLE_DEBUG_TX_STRING($ram_enable_5)
#if 0
1:	nop
	jmp 1b
#endif
	RET_LABEL(enable_sdram)

sdram_enable_out:
