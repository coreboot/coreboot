/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Global TODO: (externally: Optimus GC6 and GPS)
 * - TRPS: This is SMI 0xDD, likely in SmmOemDriver. This SW SMI adds to and executes
 *         a table of function pointers produced throughout the OEM 'value-add' stack.
 *         - Arg0 - "SFUN" - is index into "$FNC" pointer table? It's easier to
 *           correlate *CommonService use: Offset 13 creates TRPS handlers.
 *         - Known functions:
 *           - 0x80 calls offset 0 in ACER_BOOT_DEVICE_SERVICE_PROTOCOL_GUID.
 *             - NB: efiXplorer can miss InstallProtocolInterface() when Interface is local
 *           - 0x81 toggles Intel Dynamic Acceleration in IA32_MISC_ENABLE MSR.
 *           - 0x82 does switch on "OSYS" to set EC byte. Suspect this is for OS features.
 *         (A CVE exists in the vendor code only if it never sets the offset in the buffer.)
 * - RBEC/WBEC/MBEC: This is SMI 0xDD, "functions" 0x10, 0x11 and 0x12 in SmmKbcDriver,
 *        added into SmmCommonService table at its protocol notify. Performs read, write
 *        and read-modify-write from buffer. We will use ACPI instead.
 * - WMI: This is likely SMI 0xD0 in A01WMISmmCallback. This SW SMI likely uses the WMI
 *        object and consumes the OEM 'value-add' stack for EC and presumably the A01*
 *        OEM/ODM 'value-add' stack. An SSDT contains the device and EC0 provides "GCMS"
 *        and "GOTS" method helpers.
 *
 * Generally, more information is needed.
 * TODO: Implement more board features: lid and touchpad trigger wake from S3,
 *       Fn-Ctrl swap, sticky Fn keys and always-on USB charger.
 */

Device (EC0)
{
	Name (_HID, EisaId ("PNP0C09") /* Embedded Controller Device */)  // _HID: Hardware ID
	Name (_GPE, 0x50)  // _GPE: General Purpose Events
	Name (\ECOK, 0)
#if CONFIG(EC_USE_LGMR)
	Name (LGMR, 0xFE800000)	// Static, may depend on EC configuration. Unsure which register.
#endif

	Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
	{
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	#define EC_SC_IO	0x66
	#define EC_DATA_IO	0x62
	#include <ec/acpi/ec.asl>

#if CONFIG(EC_USE_LGMR)
	OperationRegion (ECMB, SystemMemory, LGMR, 0x200)
#endif
	OperationRegion (RAM, EmbeddedControl, 0, 0xFF)
	Field (RAM, ByteAcc, Lock, Preserve)
	{
		CMDB, 8,	/* EC commands */
		ETID, 8,	/* Thermal page selector */
		EBID, 8,	/* Battery page selector */
		Offset (0x06),
		CMD2, 8,	/* param 2: UNUSED */
		CMD1, 8,	/* param 1: UNUSED */
		CMD0, 8,	/* param 0 to EC command */
		Offset (0x0A),
		    , 1,
		    , 1,
		Offset (0x10),
		EQEN, 1,	/* EQ enable */
		ETEE, 1,	/* TODO */
		Offset (0x4E),
		ISEN, 1,	/* TODO */
		Offset (0x4F),
		ECTP, 8,	/* Touchpad ID */
		Offset (0x51),
		    , 3,
		TPEN, 1,	/* Touchpad enable */
		Offset (0x52),
		WLEX, 1,	/* WLAN present */
		BTEX, 1,	/* Bluetooth present */
		EX3G, 1,	/* 3G */
		    , 3,
		RFEX, 1,	/* RF present */
/*
 * NOTE: Some reverse engineering, as well as corroborating vendor's hidden SetupUtility
 * options with the EC's memory space, suggests that offset 0x55 might be the battery
 * threshold
 * - TODO: Actually diff changes in modified vendor FW
 */
		Offset (0x57),
		    , 7,
		AHKB, 1,	/* Hotkey triggered */
		AHKE, 8,	/* Hotkey data */
		Offset (0x5C),
		Offset (0x5D),
		Offset (0x6C),
		PWLT, 1,	/* NVIDIA GPS: Panel? */
		    , 3,
		GCON, 1,	/* Enter Optimus GC6 */
		Offset (0x70),
		    , 1,
		ELID, 1,	/* Lid state */
		    , 3,
		EACS, 1,	/* AC state */
		Offset (0x71),
		WLEN, 1,	/* WLAN enable */
		BTEN, 1,	/* Bluetooth enable */
		    , 3,
		ISS3, 1,
		ISS4, 1,
		ISS5, 1,
		    , 4,
		EIDW, 1,	/* Device wake */
		Offset (0x74),
		    , 2,
		    , 1,
		TPEX, 1,	/* Touchpad present */
		Offset (0x75),
		BLST, 1,	/* Bluetooth state */
		LMIB, 1,	/* TODO */
		Offset (0x76),
		ECSS, 4,	/* EC Notify of power state */
		EOSS, 4,	/* EC Notify of power state */
		Offset (0x88),	/* TODO: Aliased to "EB0S" */
		EB0A, 1,
		    , 2,
		EB0R, 1,
		EB0L, 1,
		EB0F, 1,
		EB0N, 1,
		Offset (0x90),
		SCPM, 1,	/* Set cooling policy */
		Offset (0x92),	/* TODO: Aliased to "ETAF" */
		ESSF, 1,
		ECTT, 1,
		EDTT, 1,
		EOSD, 1,	/* Trip */
		EVTP, 1,
		ECP1, 1,
		    , 1,
		ECP2, 1,
		Offset (0xA8),
		ES0T, 8,	/* Temperature */
		ES1T, 8,	/* Temperature */
		Offset (0xD0),
		ESP0, 8,	/* Passive temp */
		ESC0, 8,	/* Critical temp */
		ESP1, 8,	/* Passive temp */
		ESC1, 8,	/* Critical temp */
	}
	/* Aliases several battery registers */
	Field (RAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x88),
		EB0S, 8,	/* Battery 0 state */
	}
	/* Aliases several thermal registers */
	Field (RAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x92),
		ETAF, 8,
	}

#if CONFIG(EC_USE_LGMR)
	Field (ECMB, ByteAcc, Lock, Preserve)
	{
		Offset (0x02),
		    , 1,
		MLID, 1,
		    , 3,
		MACS, 1,
		Offset (0x06),
		MBTP, 8,
		Offset (0x08),
		MB0S, 8,
		Offset (0x20),
		MS0T, 8,
		MS1T, 8,
		MS2T, 8,
		MS3T, 8,
		MS4T, 8,
		MS5T, 8,
		Offset (0x53),
		MCSS, 1,
		MCTT, 1,
		MDTT, 1,
		MOSD, 1,
		MVTP, 1,
		Offset (0x54),
		MSP0, 8,
		MSC0, 8,
		MCC0, 8,
		MSC1, 8,
	}
#endif

	Method (_REG, 2, NotSerialized)  // _REG: Region Availability
	{
		If (Arg0 == 3)
		{
			ECOK = Arg1	// OS can clear region availability
			If (Arg1 == 1)	// On initialise
			{
				TINI ()
				EOSS = 0x05
				/* OSYS retrieved by SMM, Arg3 is unused */
//				TRPS (0x82, 1, 0)

				/*
				 * Other pages return valid data too, but this seems to be
				 * the page we are expecting - persistently in ectool dump
				 * with vendor firmware
				 * FIXME: Contents of other pages?
				 */
				ETID = 0x20
			}
		}

		/* iGFX RC method call stripped */
	}

	Method (TINI, 0, NotSerialized)
	{
		If (ECOK)
		{
			ETAF = 0
			ETEE = 1
		}
		Else
		{
			EC_WRITE (0x92, 0)	// ETAF = 0
			MBEC (0x10, 0xFD, 0x02)	// ETEE = 1
		}
	}

	Name (RFST, 0)	/* RF state */
	Method (ECPS, 1, NotSerialized)  // _PTS: Prepare To Sleep
	{
		ECSS = Arg0
		/* OSYS retrieved by SMM */
//		TRPS (0x82, 0x02, Arg0)
		If ((Arg0 == 3) || (Arg0 == 4))
		{
			RFST = RFEX
		}
	}

	Method (ECWK, 1, NotSerialized)  // _WAK: Wake
	{
		EQEN = 1
		EOSS = Arg0
		TINI ()
		Notify (BAT0, 0x81) // Information Change
		/* OSYS retrieved by SMM */
//		TRPS (0x82, 0x03, Arg0)
		If ((Arg0 == 3) || (Arg0 == 4))
		{
			RFEX = RFST
			Notify (SLPB, 0x02) // Device Wake
		}
		/* iGFX RC method call stripped */
	}

	Method (MBEC, 3, Serialized)
	{
		Local0 = EC_READ (Arg0)
		Local0 &= Arg1
		Local0 |= Arg2
		EC_WRITE (Arg0, Local0)
	}

	/* Graphical hotkey */
	Method (_Q19, 0, NotSerialized)
	{
		Printf ("Graphical hotkey display switching not implemented in coreboot!")
	}

	/* Increase brightness */
	Method (_Q1C, 0, NotSerialized)
	{
		^^^GFX0.INCB ()
	}

	/* Decrease brightness */
	Method (_Q1D, 0, NotSerialized)
	{
		^^^GFX0.DECB ()
	}

	/* Hotkeys */
	Method (_Q2C, 0, NotSerialized)
	{
		If (LMIB)
		{
			If (!AHKB)	/* Else, WMI clears its buffer? */
			{
				Local1 = AHKE
				If ((Local1 > 0) && (Local1 < 0x80))
				{
					Printf ("Hotkeys - TODO: Airplane mode?")
					/* WMI -> "GCMS" method */
				}
				ElseIf ((Local1 > 0x80) && (Local1 < 0xA0))
				{
					/* TODO: Not working when called by HID mode. What does WMI do here? */
					TPEN ^= 1
				}
			}
		}
	}

	Method (_Q36, 0, NotSerialized)
	{
		If (ECOK)
		{
			EOSD = 1	// Thermal trip
		}
		Else
		{
			MBEC (0x92, 0xF7, 0x08)	// EOSD = 1
		}

		Sleep (500)
		Notify (\_TZ.TZ01, 0x80) // Thermal Status Change
		Notify (\_TZ.TZ00, 0x80) // Thermal Status Change
	}

	Method (_Q3F, 0, NotSerialized)
	{
		/* Arg3 is unused */
//		TRPS (0x80, 0, 0)
	}

	Method (_Q40, 0, NotSerialized)
	{
		Notify (BAT0, 0x81) // Information Change
	}

	Method (_Q41, 0, NotSerialized)
	{
		Notify (BAT0, 0x81) // Information Change
	}

	/* Battery status change */
	Method (_Q48, 0, NotSerialized)
	{
		Notify (BAT0, 0x80)
	}

	/* Battery critical? */
	Method (_Q4C, 0, NotSerialized)
	{
		If (B0ST)
		{
			Notify (BAT0, 0x80) // Status Change
		}
	}

	/* AC status change: present */
	Method (_Q50, 0, NotSerialized)
	{
		Notify (ADP1, 0x80)
	}

	/* AC status change: not present */
	Method (_Q51, 0, NotSerialized)
	{
		Notify (ADP1, 0x80)
	}

	/* Lid status change: open */
	Method (_Q52, 0, NotSerialized)
	{
		Notify (LID0, 0x80)
	}

	/* Lid status change: close */
	Method (_Q53, 0, NotSerialized)
	{
		Notify (LID0, 0x80)
	}

	Method (_Q60, 0, NotSerialized)
	{
		Printf ("EC Query (0x60): WMI")
	}

	Method (_Q61, 0, NotSerialized)
	{
		Printf ("EC Query (0x61): WMI")
	}

	Method (_Q62, 0, NotSerialized)
	{
		Printf ("EC Query (0x62): Optimus GC6 or NVIDIA GPS")
	}

	Method (_Q63, 0, NotSerialized)
	{
		Printf ("EC Query (0x63): Optimus GC6 or NVIDIA GPS")
	}

	Method (_Q67, 0, NotSerialized)
	{
		Printf ("EC Query (0x67): NVIDIA GPS")
	}

	Method (_Q68, 0, NotSerialized)
	{
		Printf ("EC Query (0x68): NVIDIA GPS")
	}

	Method (_Q6C, 0, NotSerialized)
	{
		/* Arg3 is unused */
//		TRPS (0x81, 0, 0)
	}

	Method (_Q6D, 0, NotSerialized)
	{
		/* Arg3 is unused */
//		TRPS (0x81, 1, 0)
	}

	#include "ac.asl"
	#include "battery.asl"
}
