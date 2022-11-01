/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>

#include <AGESA.h>

/* Fields were removed from the structure and we cannot add them back
 * without new builds of the binaryPI blobs.
 */
#if CONFIG(NORTHBRIDGE_AMD_PI_00730F01)

#define HAS_ACPI_SRAT	TRUE
#define HAS_ACPI_SLIT	TRUE
#else
#define HAS_ACPI_SRAT	FALSE
#define HAS_ACPI_SLIT	FALSE
#endif

/* We will reference AmdLateParams later to copy ACPI tables. */
static AMD_LATE_PARAMS *AmdLateParams;

void agesawrapper_setlateinitptr(void *Late)
{
	AmdLateParams = Late;
}

void completion_InitLate(struct sysinfo *cb, AMD_LATE_PARAMS *Late)
{
	AmdLateParams = Late;
}

void *agesawrapper_getlateinitptr(int pick)
{
	ASSERT(AmdLateParams != NULL);

	switch (pick) {
	case PICK_DMI:
		return AmdLateParams->DmiTable;
	case PICK_PSTATE:
		return AmdLateParams->AcpiPState;
#if HAS_ACPI_SRAT
	case PICK_SRAT:
		return AmdLateParams->AcpiSrat;
#endif
#if HAS_ACPI_SLIT
	case PICK_SLIT:
		return AmdLateParams->AcpiSlit;
#endif
	case PICK_WHEA_MCE:
		return AmdLateParams->AcpiWheaMce;
	case PICK_WHEA_CMC:
		return AmdLateParams->AcpiWheaCmc;
	case PICK_ALIB:
		return AmdLateParams->AcpiAlib;
	case PICK_IVRS:
		return AmdLateParams->AcpiIvrs;
	case PICK_CRAT:
		return AmdLateParams->AcpiCrat;
	case PICK_CDIT:
		return AmdLateParams->AcpiCdit;
	default:
		return NULL;
	}
	return NULL;
}
