
	jmp	preram_start

preram_1:          .string "About to initialize ram\r\n"
preram_d:          .string "Debugging messages on\r\n"
preram_2:		   .string "Ram1\r\n"

preram_start:
	CONSOLE_INFO_TX_STRING($preram_1);
	CONSOLE_DEBUG_TX_STRING($preram_d);

	xorl %cx, %cx
	CALLSP(dumpdev)

#	jmp ram_initialize
preram_out:
