/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20141018	// OEM revision
)
{
	#include <acpi/dsdt_top.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	/* SW SMI ports */
	OperationRegion (DPRT, SystemIO, 0xB2, 2)
	Field (DPRT, ByteAcc, Lock, Preserve)
	{
		SSMP, 8,
		SSDP, 8
	}

	Name (ESMI, 0xDD)	// NOTE: Could insert into SSDT at runtime
	/* Returns a non-zero integer if SMI function failed */
	Method (TRPS, 3, Serialized)
	{
		Printf ("SMIF: %o", ToHexString (Arg0))
		Printf ("Param0: %o", ToHexString (Arg1))
		Printf ("Param1: %o", ToHexString (Arg2))

		Local0 = Arg1
		Local0 |= (Arg2 << 4)
		Printf ("Local0: %o", ToHexString (Local0))

		SMIF = Arg0
		SSDP = Local0
		/* NOTE: To use a general IO trap, program the range
		   into a PCR_PSTH_TRPREGx. Otherwise, this is APM. */
		SSMP = ESMI
		Return (SMIF)
	}

	Device (\_SB.PCI0)
	{
		#include <soc/intel/skylake/acpi/systemagent.asl>
		#include <soc/intel/skylake/acpi/pch.asl>
		#include "acpi/brightness_levels.asl"
	}

	#include "acpi/mainboard.asl"
}
