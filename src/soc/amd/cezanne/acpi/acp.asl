/* SPDX-License-Identifier: GPL-2.0-only */
/* ACP Audio Configuration */
Scope (\_SB.PCI0.GP41) {
	Device (ACPD) {
		/* Device addressing for ACP (Audio Coprocessor) */
		Name (_ADR, 0x05)  /* Device 0, Function 5 */

		Name (STAT, 0x3) /* Decoding Resources, Hide from UI */
		Method (_STA, 0x0, NotSerialized)
		{
			Return (STAT)
		}

		External (\_SB.PCI0.GNB.SMNA, FieldUnitObj)
		External (\_SB.PCI0.GNB.SMND, FieldUnitObj)

		/* Read SMN register and store value into ACPI variable */
		Method (SMNR, 1, Serialized)
		{
			\_SB.PCI0.GNB.SMNA = Arg0
			Return (\_SB.PCI0.GNB.SMND)
		}

		/* Write value of ACPI variable into SMN register */
		Method (SMNW, 2, Serialized)
		{
			\_SB.PCI0.GNB.SMNA = Arg0
			\_SB.PCI0.GNB.SMND = Arg1
		}

		/*
		 * SMN and mailbox interface using the SMN OperationRegion on the host bridge
		 *
		 * Provide both SMN read/write methods for direct SMN register access and the MSG0
		 * method which is used by some ACP drivers to access two different mailbox interfaces
		 * in the hardware. One mailbox interface is used to configure the ACP's clock source,
		 * the other one is used to notify the PSP that the DSP firmware has been loaded, so
		 * that the PSP can validate the firmware and set the qualifier bit to enable running
		 * it.
		 */
		Method (MSG0, 3, Serialized)
		{
			If (Arg2 != 0x09)
			{
				/* mailbox access */
				^SMNW (0x00058A74, Arg0)
				^SMNW (0x00058A54, Arg1)
				^SMNW (0x00058A14, Arg2)
				Local0 = ^SMNR (0x00058A74)
				While (Local0 == 0)
				{
					Local0 = ^SMNR (0x00058A74)
				}

				Local1 = ^SMNR (0x00058A54)
				Return (Local1)
			}
			Else
			{
				/* PSP mailbox access */
				Name (MBOX, Buffer (0x04) {
					0x20, 0x57, 0x91, 0x7B
				})
				CreateWordField (MBOX, Zero, STAT)
				CreateByteField (MBOX, 0x02, CMDI)
				CreateBitField (MBOX, 0x1F, REDY)
				Local0 = ^SMNR (0x03810570)
				MBOX = Local0
				While (((REDY != 1) || (CMDI != 0)))
				{
					Local0 = ^SMNR (0x03810570)
					MBOX = Local0
				}

				Local0 = Zero
				MBOX = Local0
				CMDI = 0x33
				Local0 = MBOX
				^SMNW (0x03810570, Local0)
				Sleep (1)
				Local0 = ^SMNR (0x03810570)
				MBOX = Local0
				While (CMDI != 0)
				{
					Local0 = ^SMNR (0x03810570)
					MBOX = Local0
				}

				Return (Local0)
			}
		}
	}
}
