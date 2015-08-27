#define BRIGHTNESS_UP \_SB.PCI0.GFX0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.DECB
#define ACPI_VIDEO_DEVICE \_SB.PCI0.GFX0
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x03,		// DSDT revision: ACPI v3.0
	"COREv4",	// OEM id
	"COREBOOT",	// OEM table id
	0x20141018	// OEM revision
)
{
	// Some generic macros
	#include "acpi/platform.asl"
	#include <cpu/intel/model_206ax/acpi/cpu.asl>
	#include <southbridge/intel/bd82x6x/acpi/platform.asl>
	/* global NVS and variables.  */
	#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>
	#include <southbridge/intel/bd82x6x/acpi/sleepstates.asl>

	Scope (\_SB) {
		Device (PCI0)
		{
		#include <northbridge/intel/sandybridge/acpi/sandybridge.asl>
		#include <southbridge/intel/bd82x6x/acpi/pch.asl>
		#include <southbridge/intel/bd82x6x/acpi/default_irq_route.asl>

			#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}
}
