jmp sdram_enable_out

#ifndef RAM_NOP
#error RAM_NOP not defined
#endif

#ifndef RAM_PRECHARGE
#error RAM_PRECHARGE not defined
#endif

#ifndef RAM_EMRS
#error RAM_EMRS not defined
#endif

#ifndef RAM_MRS
#error RAM_MRS not defined
#endif

#ifndef RAM_CBR
#error RAM_CBR not defined
#endif

#ifndef RAM_NORMAL
#error RAM_NORMAL not defined
#endif

#if ASM_CONSOLE_LOGLEVEL > BIOS_DEBUG
ram_enable_1:	.string "Ram Enable 1\r\n"
ram_enable_2:	.string "Ram Enable 2\r\n"
ram_enable_3:	.string "Ram Enable 3\r\n"
ram_enable_4:	.string "Ram Enable 4\r\n"
ram_enable_5:	.string "Ram Enable 5\r\n"
ram_enable_6:	.string "Ram Enable 6\r\n"
ram_enable_7:	.string "Ram Enable 7\r\n"
ram_enable_8:	.string "Ram Enable 8\r\n"
ram_enable_9:	.string "Ram Enable 9\r\n"
ram_enable_10:	.string "Ram Enable 10\r\n"
ram_enable_11:	.string "Ram Enable 11\r\n"
#endif

	/* Estimate that SLOW_DOWN_IO takes about 50&76us*/
	/* delay for 200us */

#define DO_DELAY \
	movl $16, %edi		; \
1:	SLOW_DOWN_IO		; \
	decl %edi		; \
	jnz 1b
		

#define EXTRA_DELAY DO_DELAY

enable_sdram:
	/* 1 & 2 Power up and start clocks */
	CONSOLE_DEBUG_TX_STRING($ram_enable_1)
	CONSOLE_DEBUG_TX_STRING($ram_enable_2)

	/* A 200us delay is needed */

	DO_DELAY
	EXTRA_DELAY

	/* 3. Apply NOP */
	CONSOLE_DEBUG_TX_STRING($ram_enable_3)
	RAM_NOP()
	EXTRA_DELAY

	/* 4 Precharge all */
	CONSOLE_DEBUG_TX_STRING($ram_enable_4)
	RAM_PRECHARGE()
	EXTRA_DELAY
	
	/* wait until the all banks idle state... */
	/* 5. Issue EMRS to enable DLL */
	CONSOLE_DEBUG_TX_STRING($ram_enable_5)
	RAM_EMRS()
	EXTRA_DELAY
	
	/* 6. Reset DLL */
	CONSOLE_DEBUG_TX_STRING($ram_enable_6)
	RAM_MRS(1)
	EXTRA_DELAY

	/* Ensure a 200us delay between the DLL reset in step 6 and the final
	 * mode register set in step 9.
	 * Infineon needs this before any other command is sent to the ram.
	 */
	DO_DELAY
	EXTRA_DELAY
	
	/* 7 Precharge all */
	CONSOLE_DEBUG_TX_STRING($ram_enable_7)
	RAM_PRECHARGE()
	EXTRA_DELAY
	
	/* 8 Now we need 2 AUTO REFRESH / CBR cycles to be performed */
	CONSOLE_DEBUG_TX_STRING($ram_enable_8)
	RAM_CBR()
	EXTRA_DELAY
	RAM_CBR()
	EXTRA_DELAY
	/* And for good luck 6 more CBRs */
	RAM_CBR()
	EXTRA_DELAY
	RAM_CBR()
	EXTRA_DELAY
	RAM_CBR()
	EXTRA_DELAY
	RAM_CBR()
	EXTRA_DELAY
	RAM_CBR()
	EXTRA_DELAY
	RAM_CBR()
	EXTRA_DELAY

	/* 9 mode register set */
	CONSOLE_DEBUG_TX_STRING($ram_enable_9)
	RAM_MRS(0)
	EXTRA_DELAY
	
	/* 10 DDR Receive FIFO RE-Sync */
	CONSOLE_DEBUG_TX_STRING($ram_enable_10)
	RAM_RESET_DDR_PTR()
	EXTRA_DELAY
	
	/* 11 normal operation */
	CONSOLE_DEBUG_TX_STRING($ram_enable_11)
	RAM_NORMAL()

	RET_LABEL(enable_sdram)

sdram_enable_out:
