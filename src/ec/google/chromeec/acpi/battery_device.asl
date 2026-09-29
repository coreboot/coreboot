/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Battery device template, included from battery.asl once per battery.
 * The includer must define:
 *   BATTERY_NAME  = ACPI device name
 *   BATTERY_INDEX = EC battery index
 *   BATTERY_UID   = ACPI _UID
 */

Device (BATTERY_NAME)
{
	Name (_HID, EISAID ("PNP0C0A"))
	Name (_UID, BATTERY_UID)
	Name (_PCL, Package () { \_SB })

	Name (PBIF, Package () {
		0x00000001,  // 0x00: Power Unit: mAh
		0xFFFFFFFF,  // 0x01: Design Capacity
		0xFFFFFFFF,  // 0x02: Last Full Charge Capacity
		0x00000001,  // 0x03: Battery Technology: Rechargeable
		0xFFFFFFFF,  // 0x04: Design Voltage
		0x00000003,  // 0x05: Design Capacity of Warning
		0xFFFFFFFF,  // 0x06: Design Capacity of Low
		0x00000001,  // 0x07: Capacity Granularity 1
		0x00000001,  // 0x08: Capacity Granularity 2
		"",          // 0x09: Model Number
		"",          // 0x0a: Serial Number
		"LION",      // 0x0b: Battery Type
		""           // 0x0c: OEM Information
	})

	Name (PBIX, Package () {
		0x00000000,  // 0x00: Revision
		0x00000001,  // 0x01: Power Unit: mAh
		0xFFFFFFFF,  // 0x02: Design Capacity
		0xFFFFFFFF,  // 0x03: Last Full Charge Capacity
		0x00000001,  // 0x04: Battery Technology: Rechargeable
		0xFFFFFFFF,  // 0x05: Design Voltage
		0x00000003,  // 0x06: Design Capacity of Warning
		0xFFFFFFFF,  // 0x07: Design Capacity of Low
		0x00000000,  // 0x08: Cycle Count
		0x00018000,  // 0x09: Measurement Accuracy (98.3%?)
		0x000001F4,  // 0x0a: Max Sampling Time (500ms)
		0x0000000a,  // 0x0b: Min Sampling Time (10ms)
		0xFFFFFFFF,  // 0x0c: Max Averaging Interval
		0xFFFFFFFF,  // 0x0d: Min Averaging Interval
		0x00000001,  // 0x0e: Capacity Granularity 1
		0x00000001,  // 0x0f: Capacity Granularity 2
		"",          // 0x10 Model Number
		"",          // 0x11: Serial Number
		"LION",      // 0x12: Battery Type
		""           // 0x13: OEM Information
	})

	Name (PBST, Package () {
		0x00000000,  // 0x00: Battery State
		0xFFFFFFFF,  // 0x01: Battery Present Rate
		0xFFFFFFFF,  // 0x02: Battery Remaining Capacity
		0xFFFFFFFF,  // 0x03: Battery Present Voltage
	})
	Name (BSTP, 0)

	Method (_STA, 0, Serialized)
	{
		Return (BSTA (BATTERY_INDEX))
	}

	Method (_BIF, 0, Serialized)
	{
		Return (BBIF (BATTERY_INDEX, PBIF, PBIX))
	}

	Method (_BIX, 0, Serialized)
	{
		Return (BBIX (BATTERY_INDEX, PBIX))
	}

	Method (_BST, 0, Serialized)
	{
		Return (BBST (BATTERY_INDEX, PBST, RefOf (BSTP)))
	}
}

#undef BATTERY_NAME
#undef BATTERY_INDEX
#undef BATTERY_UID
