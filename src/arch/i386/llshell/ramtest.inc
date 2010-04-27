	/*
	 * This is much more of a "Is my SDRAM properly configured?"
	 * test than a "Is my SDRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */

	jmp	rt_skip
#define RAMTEST 1
#if RAMTEST
	.section ".rom.data"

rt_test:	.string "Testing SDRAM : "
rt_fill:	.string "SDRAM fill:\r\n"
rt_verify:	.string "SDRAM verify:\r\n"
rt_toomany:	.string "Too many errors.\r\n"
rt_done:	.string "Done.\r\n"
	.previous
#endif

ramtest:
#if RAMTEST
	mov %eax, %esi
	mov %ebx, %edi
	mov %esp, %ebp

	CONSOLE_INFO_TX_STRING($rt_test)
	CONSOLE_INFO_TX_HEX32(%esi)
	CONSOLE_INFO_TX_CHAR($'-')
	CONSOLE_INFO_TX_HEX32(%edi)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')

	/* ============== Fill ram block ==== */

	CONSOLE_INFO_TX_STRING($rt_fill)

	mov %esi, %ebx
1:
	cmp $0, %bx
	jne 2f

	/* Display address being filled */
	/* CONSOLE_INFO_TX_HEX32(arg) will overwrite %ebx with arg */

	CONSOLE_INFO_TX_HEX32(%ebx)
	CONSOLE_INFO_TX_CHAR($'\r')
2:
#if	i786
	/* Use a non temporal store to go faster and
	 * to bypass the cache.
	 */
	movnti	%ebx, (%ebx)
#else
	mov %ebx, (%ebx)
#endif
	add $4, %ebx
	cmp %edi, %ebx
	jl 1b

	/* Display final address */

	CONSOLE_INFO_TX_HEX32(%edi)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')

	/* ========= Verify ram block ========== */

	CONSOLE_INFO_TX_STRING($rt_verify)
	mov %esi, %ebx

1:
	cmp $0, %bx
	jne 2f

	/* Display address being tested */

	CONSOLE_INFO_TX_HEX32(%ebx)
	CONSOLE_INFO_TX_CHAR($'\r')
2:
	cmp %ebx, (%ebx)
	jne 4f
3:
	add $4, %ebx
	cmp %edi, %ebx
	jl 1b

	/* Display final address */
	CONSOLE_INFO_TX_HEX32(%edi)
	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	jmp 6f

4:
	/* Display address with error */

	CONSOLE_INFO_TX_HEX32(%ebx)
	CONSOLE_INFO_TX_CHAR($':')

	/* Display data in address with error */

	/* CONSOLE_INFO_TX_HEX32(arg) will overwrite %ebx with arg */

	mov %ebx, %esi
	mov 0(%ebx), %eax
	CONSOLE_INFO_TX_HEX32(%eax)
	mov %esi, %ebx

	CONSOLE_INFO_TX_CHAR($'\r')
	CONSOLE_INFO_TX_CHAR($'\n')
	sub $1, %ecx
	jz 5f
	jmp 3b
5:
	CONSOLE_INFO_TX_STRING($rt_toomany)
	post_code(0xf1)
	jmp	.Lhlt

6:
	CONSOLE_INFO_TX_STRING($rt_done)
	mov %ebp, %esp

#endif
	RETSP

rt_skip:
