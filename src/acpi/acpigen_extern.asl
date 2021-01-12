/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we provide it runtime via NVBx and NVSx variables from acpigen.
 */

#if CONFIG(ACPI_SOC_NVS)
External (NVB0, IntObj)
External (NVS0, IntObj)
OperationRegion (GNVS, SystemMemory, NVB0, NVS0)
#endif

#if CONFIG(ACPI_HAS_DEVICE_NVS)
External (NVB1, IntObj)
External (NVS1, IntObj)
OperationRegion (DNVS, SystemMemory, NVB1, NVS1)
#endif

#if CONFIG(CHROMEOS)
External (NVB2, IntObj)
External (NVS2, IntObj)
OperationRegion (CNVS, SystemMemory, NVB2, NVS2)
#endif
