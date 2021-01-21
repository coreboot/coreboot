/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/i82371eb/i82371eb.h>

#define SUPERIO_PNP_BASE 0x3F0
#define SUPERIO_SHOW_UARTA
#define SUPERIO_SHOW_UARTB
#define SUPERIO_SHOW_FDC
#define SUPERIO_SHOW_LPT

#include <acpi/acpi.h>

DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	1
	)
{
	#include <acpi/dsdt_top.asl>
	/* \_SB scope defining the main processor is generated in SSDT. */

	OperationRegion(X80, SystemIO, 0x80, 1)
	Field(X80, ByteAcc, NoLock, Preserve)
	{
		P80, 8
	}

	/*
	 * Intel 82371EB (PIIX4E) datasheet, section 7.2.3, page 142
	 *
	 * 0: soft off/suspend to disk					S5
	 * 1: suspend to ram						S3
	 * 2: powered on suspend, context lost				S2
	 *    Note: 'context lost' means the CPU restarts at the reset
	 *          vector
	 * 3: powered on suspend, CPU context lost			S1
	 *    Note: Looks like 'CPU context lost' does _not_ mean the
	 *          CPU restarts at the reset vector. Most likely only
	 *          caches are lost, so both 0x3 and 0x4 map to ACPI S1
	 * 4: powered on suspend, context maintained			S1
	 * 5: working (clock control)					S0
	 * 6: reserved
	 * 7: reserved
	 */
	Name (\_S0, Package () { 0x05, 0x05, 0x00, 0x00 })
	Name (\_S1, Package () { 0x03, 0x03, 0x00, 0x00 })
	Name (\_S5, Package () { 0x00, 0x00, 0x00, 0x00 })

	OperationRegion (GPOB, SystemIO, DEFAULT_PMBASE+DEVCTL, 0x10)
	Field (GPOB, ByteAcc, NoLock, Preserve)
	{
		Offset (0x03),
		TO12,   1, /* Device trap 12 */
		Offset (0x08),
		FANM,   1, /* GPO0, meant for fan */
		Offset (0x09),
		PLED,   1, /* GPO8, meant for power LED. Per PIIX4 datasheet */
		    ,   3, /* this goes low when power is cut from its core. */
		    ,   2,
		    ,   16,
		MSG0,   1 /* GPO30, message LED */
	}

	/* Prepare To Sleep, Arg0 is target S-state */
	Method (\_PTS, 1, NotSerialized)
	{
		/* Disable fan, blink power LED, if not turning off */
		If (Arg0 != 0x05)
		{
		    FANM = 0
		    PLED = 0
		}

		/* Arms SMI for device 12 */
		TO12 = 1
		/* Put out a POST code */
		P80 = Arg0 | 0xF0
	}

	Method (\_WAK, 1, NotSerialized)
	{
		/* Re-enable fan, stop power led blinking */
		FANM = 1
		PLED = 1
		/* wake OK */
		Return(Package(0x02){0x00, 0x00})
	}

	/* Root of the bus hierarchy */
	Scope (\_SB)
	{
		#include <southbridge/intel/i82371eb/acpi/intx.asl>

		PCI_INTX_DEV(LNKA, \_SB.PCI0.PX40.PIRA, 1)
		PCI_INTX_DEV(LNKB, \_SB.PCI0.PX40.PIRB, 2)
		PCI_INTX_DEV(LNKC, \_SB.PCI0.PX40.PIRC, 3)
		PCI_INTX_DEV(LNKD, \_SB.PCI0.PX40.PIRD, 4)

		/* Top PCI device */
		Device (PCI0)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_UID, 0x00)
			Name (_BBN, 0x00)

			/* PCI Routing Table */
			Name (_PRT, Package () {
				Package (0x04) { 0x0001FFFF, 0, LNKA, 0 },
				Package (0x04) { 0x0001FFFF, 1, LNKB, 0 },
				Package (0x04) { 0x0001FFFF, 2, LNKC, 0 },
				Package (0x04) { 0x0001FFFF, 3, LNKD, 0 },

				Package (0x04) { 0x0004FFFF, 0, LNKA, 0 },
				Package (0x04) { 0x0004FFFF, 1, LNKB, 0 },
				Package (0x04) { 0x0004FFFF, 2, LNKC, 0 },
				Package (0x04) { 0x0004FFFF, 3, LNKD, 0 },

#if CONFIG(BOARD_ASUS_P2B_LS)
				Package (0x04) { 0x0006FFFF, 0, LNKD, 0 },
				Package (0x04) { 0x0006FFFF, 1, LNKA, 0 },
				Package (0x04) { 0x0006FFFF, 2, LNKB, 0 },
				Package (0x04) { 0x0006FFFF, 3, LNKC, 0 },
#endif
				Package (0x04) { 0x0009FFFF, 0, LNKD, 0 },
				Package (0x04) { 0x0009FFFF, 1, LNKA, 0 },
				Package (0x04) { 0x0009FFFF, 2, LNKB, 0 },
				Package (0x04) { 0x0009FFFF, 3, LNKC, 0 },

				Package (0x04) { 0x000AFFFF, 0, LNKC, 0 },
				Package (0x04) { 0x000AFFFF, 1, LNKD, 0 },
				Package (0x04) { 0x000AFFFF, 2, LNKA, 0 },
				Package (0x04) { 0x000AFFFF, 3, LNKB, 0 },

#if CONFIG(BOARD_ASUS_P2B_LS)
				Package (0x04) { 0x0007FFFF, 0, LNKC, 0 },
				Package (0x04) { 0x0007FFFF, 1, LNKD, 0 },
				Package (0x04) { 0x0007FFFF, 2, LNKA, 0 },
				Package (0x04) { 0x0007FFFF, 3, LNKB, 0 },
#endif
				Package (0x04) { 0x000BFFFF, 0, LNKB, 0 },
				Package (0x04) { 0x000BFFFF, 1, LNKC, 0 },
				Package (0x04) { 0x000BFFFF, 2, LNKD, 0 },
				Package (0x04) { 0x000BFFFF, 3, LNKA, 0 },

				Package (0x04) { 0x000CFFFF, 0, LNKA, 0 },
				Package (0x04) { 0x000CFFFF, 1, LNKB, 0 },
				Package (0x04) { 0x000CFFFF, 2, LNKC, 0 },
				Package (0x04) { 0x000CFFFF, 3, LNKD, 0 },

#if CONFIG(BOARD_ASUS_P3B_F)
				Package (0x04) { 0x000DFFFF, 0, LNKD, 0 },
				Package (0x04) { 0x000DFFFF, 1, LNKA, 0 },
				Package (0x04) { 0x000DFFFF, 2, LNKB, 0 },
				Package (0x04) { 0x000DFFFF, 3, LNKC, 0 },

				Package (0x04) { 0x000EFFFF, 0, LNKC, 0 },
				Package (0x04) { 0x000EFFFF, 1, LNKD, 0 },
				Package (0x04) { 0x000EFFFF, 2, LNKA, 0 },
				Package (0x04) { 0x000EFFFF, 3, LNKB, 0 },
#endif
			})
			#include <northbridge/intel/i440bx/acpi/sb_pci0_crs.asl>
			#include <southbridge/intel/i82371eb/acpi/isabridge.asl>

			/* Begin southbridge block */
			Device (PX40)
			{
				Name(_ADR, 0x00040000)
				OperationRegion (PIRQ, PCI_Config, 0x60, 0x04)
				Field (PIRQ, ByteAcc, NoLock, Preserve)
				{
					PIRA,   8,
					PIRB,   8,
					PIRC,   8,
					PIRD,   8
				}

				/* PNP Motherboard Resources */
				Device (SYSR)
				{
					Name (_HID, EisaId ("PNP0C02"))
					Name (_UID, 0x02)
					Method (_CRS, 0, NotSerialized)
					{
					Name (BUF1, ResourceTemplate ()
					{
						/* PM register ports */
						IO (Decode16, 0x0000, 0x0000, 0x01, 0x40, _Y06)
						/* SMBus register ports */
						IO (Decode16, 0x0000, 0x0000, 0x01, 0x10, _Y07)
						/* PIIX4E ports */
						/* Aliased DMA ports */
						IO (Decode16, 0x0010, 0x0010, 0x01, 0x10, )
						/* Aliased PIC ports */
						IO (Decode16, 0x0022, 0x0022, 0x01, 0x1E, )
						/* Aliased timer ports */
						IO (Decode16, 0x0050, 0x0050, 0x01, 0x04, )
						IO (Decode16, 0x0062, 0x0062, 0x01, 0x02, )
						IO (Decode16, 0x0065, 0x0065, 0x01, 0x0B, )
						IO (Decode16, 0x0074, 0x0074, 0x01, 0x0C, )
						IO (Decode16, 0x0091, 0x0091, 0x01, 0x03, )
						IO (Decode16, 0x00A2, 0x00A2, 0x01, 0x1E, )
						IO (Decode16, 0x00E0, 0x00E0, 0x01, 0x10, )
						IO (Decode16, 0x0294, 0x0294, 0x01, 0x04, )
						IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x02, )
						IO (Decode16, 0x04D0, 0x04D0, 0x01, 0x02, )
					})
					CreateWordField (BUF1, _Y06._MIN, PMLO)
					CreateWordField (BUF1, _Y06._MAX, PMRL)
					CreateWordField (BUF1, _Y07._MIN, SBLO)
					CreateWordField (BUF1, _Y07._MAX, SBRL)

					PMLO = \_SB.PCI0.PX43.PM00 & 0xFFFE
					SBLO = \_SB.PCI0.PX43.SB00 & 0xFFFE
					PMRL = PMLO
					SBRL = SBLO
					Return (BUF1)
					}
				}
				#include <southbridge/intel/i82371eb/acpi/i82371eb.asl>
			}
			Device (PX43)
			{
				Name (_ADR, 0x00040003)  // _ADR: Address
				OperationRegion (IPMU, PCI_Config, PMBA, 0x02)
				Field (IPMU, ByteAcc, NoLock, Preserve)
				{
				    PM00,   16
				}

				OperationRegion (ISMB, PCI_Config, SMBBA, 0x02)
				Field (ISMB, ByteAcc, NoLock, Preserve)
				{
				    SB00,   16
				}
			}

			#include <superio/winbond/w83977tf/acpi/superio.asl>
		}
	}

	/* ACPI Message */
	Scope (\_SI)
	{
		Method (_MSG, 1, NotSerialized)
		{
			If (Arg0 == 0)
			{
				MSG0 = 1
			}
			Else
			{
				MSG0 = 0
			}
		}
	}
}
