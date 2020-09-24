/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard DSDT inside the PCI device
 * "Northbridge Miscellaneous Control (Northbridge function 3)" and it
 * will expose the temperature sensor of the processor as a thermal
 * zone.
 *
 * Families 10 through 14 and some family 15 CPUs are supported.
 *
 * If, for example, the NB Misc. Control device is on 0:18.3, include
 * the following:
 *
 * Scope (\_SB.PCI0) {
 *   Device (K10M) {
 *     Name (_ADR, 0x00180003)
 *     #include <soc/amd/common/acpi/thermal_zone.asl>
 *   }
 * }
 *
 * Do not include this if the board is affected by erratum 319 as the
 * thermal sensor of Socket F/AM2+ processors may be unreliable.
 * (Erratum 319 affects AM2+ boards, AM3 and later should be fine)
 */

#ifndef K10TEMP_HOT_OFFSET
# define K10TEMP_HOT_OFFSET	50
#endif

#define K10TEMP_KELVIN_OFFSET	2732
#define K10TEMP_TLIMIT_OFFSET	520

OperationRegion (TCFG, PCI_Config, 0x64, 0x4)
Field (TCFG, ByteAcc, NoLock, Preserve) {
	HTCE, 1, /* Hardware thermal control enable */
	, 15,
	TLMT, 7, /* (LimitTmp - 52) / 0.5 */
	, 9,
}

OperationRegion (TCTL, PCI_Config, 0xa4, 0x4)
Field (TCTL, ByteAcc, NoLock, Preserve) {
	, 21,
	TNOW, 11, /* CurTmp / 0.125 */
}

ThermalZone (TZ00) {
	Name (_STR, Unicode ("AMD CPU Core Thermal Sensor"))

	Method (_STA) {
		If (HTCE == 1) {
			Return (0x0F)
		}
		Return (0)
	}

	Method (_TMP) {	/* Current temp in tenths degree Kelvin. */
		Local0 = TNOW * 10
		Local0 >>= 3
		Return (Local0 + K10TEMP_KELVIN_OFFSET)
	}

	/*
	 * TLMT indicates threshold where HTC become active. That is the processor will limit
	 * P-State and power consumption in order to cool down.
	 */
	Method (_PSV) {	/* Passive temp in tenths degree Kelvin. */
		Local0 = TLMT * 10
		Local0 >>= 1
		Local0 += K10TEMP_TLIMIT_OFFSET
		Return (Local0 + K10TEMP_KELVIN_OFFSET)
	}

	Method (_HOT) {	/* Hot temp in tenths degree Kelvin. */
		Return (_PSV + K10TEMP_HOT_OFFSET)
	}

	Method (_CRT) {	/* Critical temp in tenths degree Kelvin. */
		Return (_HOT + K10TEMP_HOT_OFFSET)
	}
}
