/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Lenovo IdeaPad embedded controller (ACPI interface).
 *
 * Reverse engineered on the IdeaPad 320S-13IKB (Nuvoton NPCE388NB0DX EC
 * on the Compal LA-E541P board). The same board and EC also serve the
 * IdeaPad 320S-14IKB, 320S-15ISK and 520S-14IKBR, and the NPCE388
 * appears in other Compal-built IdeaPads of the era (500S, Yoga 700).
 * Boards reusing this code must check the EC RAM layout against their
 * hardware first.
 *
 * Standard ACPI EC (PNP0C09) on the LPC bus, host interface at I/O ports
 * 0x62 (data) and 0x66 (command/status). The SCI source and its _GPE number
 * are board specific and provided by the including mainboard via:
 *   #define IDEAPAD_EC_GPE <number>
 *
 * The embedded-controller RAM field offsets below were established by
 * observing live EC RAM against the kernel-reported battery/AC values
 * (see the board's reverse-engineering notes); they are factual hardware
 * descriptions, not derived from vendor firmware tables.
 */

#ifndef IDEAPAD_EC_GPE
#error "IDEAPAD_EC_GPE must be defined by the mainboard before including this file"
#endif

Device (H_EC)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, One)
	Name (_GPE, IDEAPAD_EC_GPE)

	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, 0x0062, 0x0062, 0x00, 0x01)
		IO (Decode16, 0x0066, 0x0066, 0x00, 0x01)
	})

	/* Set when the EC operation region becomes accessible (see _REG). */
	Name (ECOK, Zero)

	/*
	 * Embedded-controller RAM. Only the fields used by the ACPI objects
	 * are named; offsets are little-endian. Capacity fields are reported
	 * by the EC in 10 mWh units, voltage in mV, present rate in mA.
	 */
	OperationRegion (ERAM, EmbeddedControl, 0x00, 0x100)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x01),
		    , 5,
		ACAC, 1,	/* AC adapter present */
		Offset (0x05),
		OSTP, 8,	/* OS type: 0x04 = ACPI/OS mode */
		Offset (0x16),
		BVTG, 16,	/* battery present voltage (mV) */
		BPRT, 16,	/* battery present rate (mA, signed) */
		BRSC, 8,	/* battery relative capacity (%) */
		Offset (0x1C),
		BRMC, 16,	/* battery remaining capacity (x10 mWh) */
		Offset (0x26),
		BFCC, 16,	/* battery last full charge capacity (x10 mWh) */
		BDCC, 16,	/* battery design capacity (x10 mWh) */
		BDVG, 16,	/* battery design voltage (mV) */
		Offset (0x30),
		BMFG, 64,	/* battery manufacturer (ASCII) */
		Offset (0x8C),
		BMOD, 128,	/* battery model name (ASCII) */
		Offset (0xA8),
		BSER, 128,	/* battery serial/barcode (ASCII) */
	}

	Method (_REG, 2, NotSerialized)
	{
		If ((Arg0 == 0x03)) {
			ECOK = Arg1
			If ((Arg1 == One)) {
				/* Tell the EC a modern ACPI OS is running. */
				OSTP = 0x04
			}
		}
	}

	#include "ac.asl"
	#include "battery.asl"
	#include "lid.asl"
	#include "vpc.asl"

	/*
	 * EC query (_Qxx) event handlers. The EC raises an SCI and the OS
	 * issues a query; the handler notifies the affected ACPI object.
	 * Query numbers are EC-firmware constants for this controller family.
	 */
	Method (_Q0A, 0, NotSerialized) { Notify (BAT0, 0x80) }	/* battery status */
	Method (_Q0B, 0, NotSerialized) { Notify (BAT0, 0x81) }	/* battery info */
	Method (_Q0C, 0, NotSerialized) { LIDW (Zero) }		/* lid close */
	Method (_Q0D, 0, NotSerialized) { LIDW (One) }		/* lid open */
	Method (_QC6, 0, NotSerialized) { Notify (VPC0, 0x80) }	/* VPC hotkey */
	Method (_QCA, 0, NotSerialized) { Notify (VPC0, 0x80) }	/* VPC perf mode */
	Method (_QCB, 0, NotSerialized) { Notify (VPC0, 0x80) }	/* VPC perf mode */
}
