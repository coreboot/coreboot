/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <soc/iomap.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725
)
{
	#include <acpi/dsdt_top.asl>
	#include "acpi/platform.asl"

	Name(_S0, Package() { 0x00, 0x00, 0x00, 0x00 })
	Name(_S5, Package() { 0x07, 0x00, 0x00, 0x00 })

	Scope (\_SB)
	{
		Device (PCI0)
		{
			#include <soc/intel/xeon_sp/acpi/southcluster.asl>
			#include <soc/intel/common/block/acpi/acpi/lpc.asl>

		}

		Device (UNC0)
		{
			Name (_HID, EisaId ("PNP0A03"))
			Name (_UID, 0x3F)
			Method (_BBN, 0, NotSerialized)
			{
				Return (0xff)
			}

			Method (_STA, 0, NotSerialized)
			{
				Return (0xf)
			}

			Name (_CRS, ResourceTemplate ()
			{
				WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
					0x0000,             // Granularity
					0x00FF,             // Range Minimum
					0x00FF,             // Range Maximum
					0x0000,             // Translation Offset
					0x0001,             // Length
					,, )
			})

		}
	}

}
