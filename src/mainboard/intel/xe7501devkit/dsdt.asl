/* This is a dummy dsdt. Normal ACPI requires a DSDT, but in this case, ACPI
   is just a workaround for QNX. It would be nice to eventually have a real
   dsdt here.
   Note: It will not be hooked up at runtime. It won't even get linked.
   But we still need this file. */

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv2",	// OEM id
	"COREBOOT",	// OEM table id
	0x20090419	// OEM revision
)
{
}
