/* SPDX-License-Identifier: GPL-2.0-only */

#ifdef IT8516E_FIRST_DATA
Device (PM1) {
	Name (_HID, EisaId("PNP0C02"))
	Name (_STR, Unicode("IT8516E PM Channel 1"))
	Name (_UID, SUPERIO_UID(PM, 1))

	/*
	 * The EC firmware exposes CPU temperature through ec ram
	 * on the first PM channel.
	 */

	#undef EC_DATA_IO
	#define EC_DATA_IO IT8516E_FIRST_DATA
	#undef EC_SC_IO
	#define EC_SC_IO IT8516E_FIRST_SC
	#include <ec/acpi/ec.asl>

	Method (_CRS)
	{
		/* Announce the used i/o ports to the OS */
		Return (ResourceTemplate () {
			IO (Decode16, IT8516E_FIRST_DATA, IT8516E_FIRST_DATA, 0x01, 0x01)
			IO (Decode16, IT8516E_FIRST_SC, IT8516E_FIRST_SC, 0x01, 0x01)
		})
	}

	/*
	 * Get CPU temperature from first PM channel (in 10th Kelvin)
	 */
	Method (CTK)
	{
		Local0 = EC_READ (0x52)
		If (And (Local0, EC_ERROR_MASK)) {
			Return (0)
		}
		Local0 *= 10
		Return (Local0 + 2732)	/* Return as 10th Kelvin */
	}
}
#endif

#ifdef IT8516E_SECOND_DATA
Device (PM2) {
	Name (_HID, EisaId("PNP0C02"))
	Name (_STR, Unicode("IT8516E PM Channel 2"))
	Name (_UID, SUPERIO_UID(PM, 2))

	/*
	 * The EC firmware exposes fan and GPIO control through the
	 * second PM channel.
	 */

	#undef EC_DATA_IO
	#define EC_DATA_IO IT8516E_SECOND_DATA
	#undef EC_SC_IO
	#define EC_SC_IO IT8516E_SECOND_SC
	#include <ec/acpi/ec.asl>

	Method (_CRS)
	{
		/* Announce the used i/o ports to the OS */
		Return (ResourceTemplate () {
			IO (Decode16, IT8516E_SECOND_DATA, IT8516E_SECOND_DATA, 0x01, 0x01)
			IO (Decode16, IT8516E_SECOND_SC, IT8516E_SECOND_SC, 0x01, 0x01)
		})
	}

	/*
	 * Get CPU temperature from second PM channel (in 10th Kelvin)
	 */
	Method (CTK)
	{
		Acquire (EC_MUTEX, 0xffff)
		Local0 = SEND_EC_COMMAND (0x20) /* GET_CPUTEMP */
		If (And (Local0, EC_ERROR_MASK)) {
			Release (EC_MUTEX)
			Return (0)
		}
		Local0 = RECV_EC_DATA ()	/* Temp low byte in 64th °C */
		If (And (Local0, EC_ERROR_MASK)) {
			Release (EC_MUTEX)
			Return (0)
		}
		Local1 = RECV_EC_DATA ()	/* Temp high byte in 64th °C */
		If (And (Local1, EC_ERROR_MASK)) {
			Release (EC_MUTEX)
			Return (0)
		}
		Release (EC_MUTEX)

		Local0 |= Local1 << 8
		Local0 *= 10 / 64				/* Convert to 10th °C */
		Return (Local0 + 2732)				/* Return as 10th Kelvin */
	}
}
#endif
