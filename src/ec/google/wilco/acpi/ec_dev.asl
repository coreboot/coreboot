/* SPDX-License-Identifier: GPL-2.0-only */

Device (WLCO)
{
	Name (_HID, "GOOG000C")
	Name (_UID, 1)
	Name (_DDN, "Wilco EC Command Device")
#ifdef EC_WAKE_PIN
	Name (_PRW, Package () { EC_WAKE_PIN, 0x5 })
#endif

	Method (_STA)
	{
		Return (0xb)
	}

	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16,
		    CONFIG_EC_BASE_HOST_DATA,
		    CONFIG_EC_BASE_HOST_DATA,
		    4, 4)
		IO (Decode16,
		    CONFIG_EC_BASE_HOST_COMMAND,
		    CONFIG_EC_BASE_HOST_COMMAND,
		    4, 4)
		IO (Decode16,
		    CONFIG_EC_BASE_PACKET,
		    CONFIG_EC_BASE_PACKET,
		    16, 16)
	})

	Name (_PRS, ResourceTemplate ()
	{
		StartDependentFn (0, 0) {
			IO (Decode16,
			    CONFIG_EC_BASE_HOST_DATA,
			    CONFIG_EC_BASE_HOST_DATA,
			    4, 4)
			IO (Decode16,
			    CONFIG_EC_BASE_HOST_COMMAND,
			    CONFIG_EC_BASE_HOST_COMMAND,
			    4, 4)
			IO (Decode16,
			    CONFIG_EC_BASE_PACKET,
			    CONFIG_EC_BASE_PACKET,
			    16, 16)
		}
		EndDependentFn ()
	})
}

Device (WEVT)
{
	Name (_HID, "GOOG000D")
	Name (_UID, 1)
	Name (_DDN, "Wilco EC Event Interface")

	Method (_STA)
	{
		Return (0xB)
	}

	/* Get Event Buffer */
	Method (QSET, 0, Serialized)
	{
		/* Get count of event bytes */
		Local0 = R (QSEC)
		Name (QBUF, Buffer (Local0) {})

		/* Fill QS event buffer with Local0 bytes */
		For (Local1 = 0, Local1 < Local0, Local1++) {
			QBUF[Local1] = R (QSEB)
		}

		Printf ("QS = %o", QBUF)
		Return (QBUF)
	}
}
