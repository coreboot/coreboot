/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(ACPI_CONSOLE)

#include <soc/iomap.h>

Name (UFLG, CONFIG(CONSOLE_SERIAL))

Method (LURT, 1, Serialized)
{
	If (Arg0 == 0) { /* 0 = 0x3f8 */
		Local0 = 0x3f8
	} ElseIf (Arg0 == 1) { /* 1 = 0x2f8 */
		Local0 = 0x2f8
	} ElseIf (Arg0 == 2) { /* 2 = 0x3e8 */
		Local0 = 0x3e8
	} ElseIf (Arg0 == 3) { /* 3 = 0x2e8 */
		Local0 = 0x2e8
	}
	Return (Local0)
}

#if CONFIG(DRIVERS_UART_8250MEM_32)
OperationRegion (UBAR, SystemMemory,
			CONFIG_CONSOLE_UART_BASE_ADDRESS, 24)
Field (UBAR, AnyAcc, NoLock, Preserve)
{
	TDR, 8,	/* Transmit Data Register BAR + 0x000 */
		, 24,
	IER, 8,	/* Interrupt Enable Register BAR + 0x004 */
		, 24,
	IIR, 8,	/* Interrupt Identification Register BAR + 0x008 */
		, 24,
	LCR, 8,	/* Line Control Register BAR + 0x00C */
		, 24,
	MCR, 8,	/* Modem Control Register BAR + 0x010 */
		, 24,
	LSR, 8,	/* Line Status Register BAR + 0x014 */
		, 24
}
#else
OperationRegion (UBAR, SystemIO, LURT (CONFIG_UART_FOR_CONSOLE), 6)
Field (UBAR, ByteAcc, NoLock, Preserve)
{
	TDR, 8,	/* Transmit Data Register IO Port + 0x0 */
	IER, 8,	/* Interrupt Enable Register IO Port + 0x1 */
	IIR, 8,	/* Interrupt Identification Register IO Port + 0x2 */
	LCR, 8,	/* Line Control Register IO Port + 0x3 */
	MCR, 8,	/* Modem Control Register IO Port + 0x4 */
	LSR, 8	/* Line Status Register IO Port + 0x5 */
}
#endif

Method (APRT, 1, Serialized)
{
	Name(OPDT, 0)
	Name(INDX, 0)
	Name(LENG, 0)
	Name(ADBG, Buffer(256) {0})

	If (ObjectType(Arg0) == 1) { /* Integer */
		ToHexString(Arg0, Local0)
		ADBG = Local0
	} ElseIf (ObjectType(Arg0) == 2) { /* String */
		ADBG = Arg0
	} ElseIf (ObjectType(Arg0) == 3) { /* Buffer */
		ToHexString(Arg0, ADBG)
	} Else {
		ADBG = "This type of object is not supported"
	}

	While (DeRefOf(ADBG[INDX]) != 0)
	{
		INDX++
	}
	LENG = INDX /* Length of the String */

	If (UFLG == 0) {
		/* Enable Baud Rate Divisor Latch, Set Word length to 8 bit*/
		LCR = 0x83
		IIR = 0x01
		MCR = 0x03

		/* Configure baud rate to 115200 */
		TDR = 0x01
		IER = 0x00
		LCR = 0x03 /* Disable Baud Rate Divisor Latch */

		UFLG++
	}
	INDX = 0x00
	While (INDX < LENG)
	{
		/* Wait for the transmitter t to be ready */
		While (1)
		{
			OPDT = LSR & 0x20
			If (OPDT != 0)
			{
				Break
			}
		}
		TDR = DeRefOf (ADBG[INDX])
		INDX++
	}
} /* End of APRT */

#endif
