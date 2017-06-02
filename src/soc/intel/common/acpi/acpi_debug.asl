/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#if IS_ENABLED(CONFIG_ACPI_CONSOLE)

#include <soc/iomap.h>

Name (UFLG, IS_ENABLED(CONFIG_CONSOLE_SERIAL))

Method (LURT, 1, Serialized)
{
	If (LEqual(Arg0, 0)) { /* 0 = 0x3f8 */
		Store (0x3f8, Local0)
	} ElseIf (LEqual(Arg0, 1)) { /* 1 = 0x2f8 */
		Store (0x2f8, Local0)
	} ElseIf (LEqual(Arg0, 2)) { /* 2 = 0x3e8 */
		Store (0x3e8, Local0)
	} ElseIf (LEqual(Arg0, 3)) { /* 3 = 0x2e8 */
		Store (0x2e8, Local0)
	}
	Return (Local0)
}

Method (APRT, 1, Serialized)
{
	Name(OPDT, 0)
	Name(INDX, 0)
	Name(LENG, 0)
	Name(ADBG, Buffer(256) {0})

	If (LEqual(ObjectType(Arg0), 1)) { /* Integer */
		ToHexString(Arg0, Local0)
		Store(Local0, ADBG)
	} ElseIf (LEqual(ObjectType(Arg0), 2)) { /* String */
		Store(Arg0, ADBG)
	} ElseIf (LEqual(ObjectType(Arg0), 3)) { /* Buffer */
		ToHexString(Arg0, ADBG)
	} Else {
		Store("This type of object is not supported", ADBG)
	}

	While (LNotEqual(DeRefOf(Index(ADBG, INDX)), 0))
	{
		Increment (INDX)
	}
	Store (INDX, LENG) /* Length of the String */

#if CONFIG_DRIVERS_UART_8250MEM_32
	OperationRegion (UBAR, SystemMemory, UART_DEBUG_BASE_ADDRESS, 24)
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

	If (LEqual(UFLG, 0)) {
		/* Enable Baud Rate Divisor Latch, Set Word length to 8 bit*/
		Store (0x83, LCR)
		Store (0x01, IIR)
		Store (0x03, MCR)

		/* Configure baud rate to 115200 */
		Store (0x01, TDR)
		Store (0x00, IER)
		Store (0x03, LCR) /* Disable Baud Rate Divisor Latch */

		Increment (UFLG)
	}
	Store (0x00, INDX)
	While (LLess (INDX, LENG))
	{
		/* Wait for the transmitter t to be ready */
		While (1)
		{
			And (LSR, 0x20, OPDT)
			If (LNotEqual(OPDT, 0))
			{
				Break
			}
		}
		Store (DeRefOf (Index (ADBG, INDX)), TDR)
		Increment(INDX)
	}
} /* End of APRT */

#endif
