/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ELAN7B18UH - HID over SPI
 * See "HID Over SPI Protocol Specification" section 5.2 - ACPI enum
 * https://www.microsoft.com/en-us/download/details.aspx?id=103325

 * NOTES:
 * 1. Make sure TOUCH_SCR_IRQ is defined according to the board's schematics
 * 2. Include this file under the scope for the correct SPI, e.g.:
 *   Scope (\_SB.PCI0.SPI0)
 *   {
 *      #include <variant/acpi/hid_spi_elan.asl>
 *   }
 */

Device(TCSC) {
	Name (_HID, "ELAN9006")	/* VendorID + DeviceID - <VVVVdddd> */
	Name (_CID, "PNP0C51")	/* Compatible ID - PNP0C51 */
	Name (_SUB, "ELAN0732")	/* SubVendorID + SubSystemID - <VVVVssss> */
	Name (_HRV, 0x0001)	/* Hardware Revision Number - 2 bytes */

	Method (_STA, 0)
	{
		Return (0xf)
	}

	Name (_CRS, ResourceTemplate () {
		SpiSerialBusV2 (
			0,			/* DeviceSelection */
			PolarityLow,		/* DeviceSelectionPolarity */
			FourWireMode,		/* WireMode */
			8,			/* DataBitLength */
			ControllerInitiated,	/* SlaveMode */
			0x0B71B00,		/* ConnectionSpeed (12MHz) */
			ClockPolarityLow,	/* ClockPolarity */
			ClockPhaseFirst,	/* ClockPhase */
			"\\_SB.PCI0.SPI0",	/* ResourceSource */
			0,			/* ResourceSourceIndex */
			ResourceConsumer,	/* ResourceUsage */
		)
		Interrupt(ResourceConsumer, Edge, ActiveLow, Shared)
		{
			TOUCH_SCR_IRQ
		}
	})

	/* _DSM - Device-Specific Method */
	/* Arg0: UUID Unique function identifier */
	/* Arg1: Integer Revision ID - Will be 3 for HidSpi V1 */
	/* Arg2: Integer Function Index (0 = Return Supported Functions) */
	/* Arg3: Package Parameters */

	Method (_DSM, 4) {
		/* HIDSPI UUID */
		If(Arg0 == ToUUID("6E2AC436-0FCF-41AF-A265-B32A220DCFAB")) {

			/* Switch on the function index */
			switch(ToInteger(Arg2)) {
				case(0) {
				/* Switch on the revision level */
					switch(ToInteger(Arg1)) {
						case (3) {
							/* HidSpi v1: Functions 0-6 */
							Return(Buffer(One) { 0x7F })
						}
						default {
							/* Unsupported revision */
							Return(Buffer(One) { 0x00 })
						}
					}
				}
				case(1) {
					/* Elan Input Report Header address - 0x1000*/
					Return (0x1000)
				}
				case(2) {
					/* Elan Input Report Body address - 0x1100*/
					Return (0x1100)
				}
				case(3) {
					/* Elan Output Report Header address - 0x2000 */
					Return (0x2000)
				}
				case(4) {
					/* Read opcode */
					Return (Buffer(1) {0x0B})
				}
				case(5) {
					/* Write opcode */
					Return (Buffer(1) {0x02})
				}
				case(6) {
					/* Flags */
					Return (0x0000)
				}
				default {
					/* Unsupported function index */
					Return (Buffer() {0})
				}
			}
		}
		else {
			/* No functions are supported for this UUID. */
			return (Buffer() {0})
		}
	}

	Method(_RST, 0, Serialized) {
		/* De-assert the reset signal for 20ms */
		CTXS(GPP_C01)
		Sleep(20)
		STXS(GPP_C01)
	}

	Name (_PR0, Package (0x01)  /* Power Resources for D0 */
	{
		PR00
	})

	Name (_PR3, Package (0x01)  /* Power Resources for D3hot */
	{
		PR00
	})

	PowerResource (PR00, 0x00, 0x0000)
	{
		Method (_STA, 0, NotSerialized)  /* Status */
		{
			Return (1)
		}

		Method (_ON, 0, Serialized)  /* Power On */
		{
			CTXS (GPP_C01)
			STXS (GPP_C00)
			Sleep (1)
			STXS (GPP_C01)
			Sleep (20)
			STXS (GPP_C06)
		}

		Method (_OFF, 0, Serialized)  /* Power Off */
		{
			CTXS (GPP_C06)
			Sleep (2)
			CTXS (GPP_C01)
			Sleep (2)
			CTXS (GPP_C00)
		}
	}
}
