/* SPDX-License-Identifier: GPL-2.0-only */

/* Enable ACPI _SWS methods */
#include <soc/intel/common/acpi/acpi_wake_source.asl>

Name (_S0, Package () // mandatory system state
{
	0x00, 0x00, 0x00, 0x00
})

Name (_S5, Package ()  // mandatory system state
{
	0x07, 0x00, 0x00, 0x00
})

/* Port 80 POST */
OperationRegion (DBG0, SystemIO, 0x80, 0x02)
Field (DBG0, ByteAcc, Lock, Preserve)
{
	IO80, 8,
	IO81, 8
}

/* Virtual UART on 0x3f8 */
Scope(\_SB) {
	Device(COM1) {
		Name(_HID, EisaId("PNP0501"))
		Name(_UID, 0x01)
		Name(_STA,0x0F)
		Name(_CRS, ResourceTemplate() {
			IO(Decode16, 0x03F8, 0x03F8, 0x00, 0x08)
			IRQNoFlags() { 4 }
		})
	}
}

/*
 * The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method (_PTS, 1)
{
}

/* The _WAK method is called on system wakeup */

Method (_WAK, 1)
{
	Return (Package (){ 0, 0 })
}
