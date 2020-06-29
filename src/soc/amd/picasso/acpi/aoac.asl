/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/acpimmio_map.h>

#define AOAC_DEVICE(DEV_NAME, DEV_ID, SX) \
	PowerResource(DEV_NAME, SX, 0) { \
		OperationRegion (AOAC, SystemMemory, ACPIMMIO_BASE(AOAC) + 0x40 + (DEV_ID << 1), 2) \
		Field (AOAC, ByteAcc, NoLock, Preserve) { \
			/* \
			 * Target Device State \
			 * \
			 * 0 = D0 - Uninitialized \
			 * 1 = D0 - Initialized \
			 * 2 = D1/D2/D3Hot \
			 * 3 = D3Cold \
			 * \
			 * This field is only used to cut off register access. It does not \
			 * control any power states. D3Cold is the only value that will \
			 * cut off register access. All other values will allow register \
			 * access and are purely informational. \
			 */ \
			TDS,	2, \
\
			DS,	1, /* Device State - Purely informational */ \
\
			/* \
			 * Power On Dev \
			 * \
			 * 1 = Perform hardware sequence to power on the device \
			 * 0 = Perform hardware sequence to power off the device \
			 * \
			 * This register is only valid when Is Software Control = 0. \
			 */ \
			POD,	1, \
\
			/* Software Power On Reset B */ \
			SPRB,	1, \
			/* Software Ref Clock OK */ \
			SRCO,	1, \
			/* Software Reset B */ \
			SRB,	1, \
			/* \
			 * Is Software Control \
			 * \
			 * 1 = Allow software to control Power On Reset B, \
			 *     Ref Clock OK, and Reset B. \
			 * 0 = Hardware control \
			 */ \
			ISWC,	1, \
\
			/* Power Reset B State */ \
			PRBS,	1, \
			/* Ref Clock OK State */ \
			RCOS,	1, \
			/* Reset B State */ \
			RBS,	1, \
			/* Device Off Gating State */ \
			DOGS,	1, \
			/* D3 Cold State */ \
			D3CS,	1, \
			/* Device Clock OK State */ \
			COS,	1, \
			/* State of device */ \
			STA0,	1, \
			/* State of device */ \
			STA1,	1, \
		} \
		Method(_STA) { \
			Local0 = (PRBS && RCOS && RBS) \
\
			Printf("AOAC.%s._STA: %o", #DEV_NAME, Local0) \
\
			If (Local0) { \
				Return (1) \
			} Else { \
				Return (0) \
			} \
		} \
		Method(_ON, 0, Serialized) { \
			Printf("AOAC.%s._ON", #DEV_NAME) \
\
			ISWC=0 \
			POD=1 \
\
			While (!PRBS || !RCOS || !RBS) { \
				Printf ("Waiting for device to power on") \
				Stall (100) \
			} \
\
			Printf("Done waiting") \
		} \
		Method(_OFF, 0, Serialized) { \
			Printf("AOAC.%s._OFF", #DEV_NAME) \
\
			ISWC=0 \
			POD=0 \
\
			While (PRBS || RCOS || RBS) { \
				Printf ("Waiting for device to power off") \
				Stall (100) \
			} \
\
			Printf("Done waiting") \
		} \
		Method(_RST, 0, Serialized) { \
			Printf("AOAC.%s._RST", #DEV_NAME) \
\
			ISWC=1 \
			SRB=1 \
\
			/* Assert the SwRstB signal for 200 us */ \
			Stall (200) \
\
			SRB=0 \
			ISWC=0 \
\
			While (!PRBS || !RCOS || !RBS) { \
				Printf ("Waiting for device to complete reset") \
				Stall (100) \
			} \
		} \
	}


Device (AOAC) {
	Name (_HID, EISAID("PNP0C02"))  // ID for Motherboard resources

	Method (_STA, 0x0, NotSerialized)
	{
		/*
		 * This case is used to indicate a valid device for which no
		 * device driver should be loaded (for example, a bridge
		 * device.) Children of this device may be present and valid.
		 * OSPM should continue enumeration below a device whose _STA
		 * returns this bit combination.
		 */
		Return (0x08)
	}

	AOAC_DEVICE(I2C2,	7, 0)
	AOAC_DEVICE(I2C3,	8, 0)
	AOAC_DEVICE(FUR0,	11, 0)
	AOAC_DEVICE(FUR1,	12, 0)
	AOAC_DEVICE(FUR2,	16, 0)
	AOAC_DEVICE(FUR3,	26, 0)
}
