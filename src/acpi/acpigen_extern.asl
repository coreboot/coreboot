/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Global ACPI memory region. This region is used for passing information
 * between coreboot (aka "the system bios"), ACPI, and the SMI handler.
 * Since we don't know where this will end up in memory at ACPI compile time,
 * we provide it runtime via NVBx and NVSx variables from acpigen.
 */

#if CONFIG(ACPI_SOC_NVS)
External (GNVS, OpRegionObj)
External (DNVS, OpRegionObj)
#endif
