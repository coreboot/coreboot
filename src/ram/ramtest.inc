
	/*
	 * This is much more of a "Is my SDRAM properly configured?"
	 * test than a "Is my SDRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
	
	jmp	rt_skip

rt_test:	.string "Testing SDRAM : "
rt_fill:	.string "SDRAM fill:\r\n"
rt_verify:	.string "SDRAM verify:\r\n"
rt_toomany:	.string "Too many errors.\r\n"
rt_done:	.string "Done.\r\n"
	
ramtest:
#ifdef RAMTEST
	mov %eax, %esi
	mov %ebx, %edi
	mov %esp, %ebp

#ifdef SERIAL_CONSOLE
	TTYS0_TX_STRING($rt_test)
	TTYS0_TX_HEX32(%esi)
	TTYS0_TX_CHAR($'-')
	TTYS0_TX_HEX32(%edi)
	TTYS0_TX_CHAR($'\r')
	TTYS0_TX_CHAR($'\n')

	/* ============== Fill ram block ==== */

	TTYS0_TX_STRING($rt_fill)
#endif

	mov %esi, %ebx
1:
	cmp $0, %bx
	jne 2f

#ifdef SERIAL_CONSOLE
	/* Display address being filled */
	/* TTYS0_TX_HEX32(arg) will overwrite %ebx with arg */
	
	TTYS0_TX_HEX32(%ebx)
	TTYS0_TX_CHAR($'\r')
#endif
2:
	mov %ebx, (%ebx)
	add $4, %ebx
	cmp %edi, %ebx
	jl 1b

#ifdef SERIAL_CONSOLE
	/* Display final address */

	TTYS0_TX_HEX32(%edi)
	TTYS0_TX_CHAR($'\r')
	TTYS0_TX_CHAR($'\n')

	/* ========= Verify ram block ========== */

	TTYS0_TX_STRING($rt_verify)
#endif	
	mov %esi, %ebx

1:
	cmp $0, %bx
	jne 2f

#ifdef SERIAL_CONSOLE
	/* Display address being tested */

	TTYS0_TX_HEX32(%ebx)
	TTYS0_TX_CHAR($'\r')
#endif

2:
	cmp %ebx, (%ebx)
	jne 4f
3:
	add $4, %ebx
	cmp %edi, %ebx
	jl 1b

#ifdef SERIAL_CONSOLE
	/* Display final address */

	TTYS0_TX_HEX32(%edi)
	TTYS0_TX_CHAR($'\r')
	TTYS0_TX_CHAR($'\n')
#endif
	jmp 6f

4:
#ifdef SERIAL_CONSOLE
	/* Display address with error */

	TTYS0_TX_HEX32(%ebx)
	TTYS0_TX_CHAR($':')

	/* Display data in address with error */

	/* TTYS0_TX_HEX32(arg) will overwrite %ebx with arg */

	mov %ebx, %esi
	mov 0(%ebx), %eax
	TTYS0_TX_HEX32(%eax)
	mov %esi, %ebx

	TTYS0_TX_CHAR($'\r')
	TTYS0_TX_CHAR($'\n')
#endif
	sub $1, %ecx
	jz 5f
	jmp 3b
5:	
#ifdef SERIAL_CONSOLE
	TTYS0_TX_STRING($rt_toomany)
#endif
	intel_chip_post_macro(0xf1)
	jmp	.Lhlt

6:
#ifdef SERIAL_CONSOLE
	TTYS0_TX_STRING($rt_done)
#endif
	mov %ebp, %esp
#endif
	RETSP

rt_skip:
