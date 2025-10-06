/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef OPENSIL_DRIVER_H
#define OPENSIL_DRIVER_H

#include <acpi/acpi.h>
#include <device/device.h>
#include <smbios.h>

/* Set up openSIL env and call TP1 */
void amd_opensil_silicon_init(void);
/* Set global and per-device MPIO configurations */
void configure_mpio(void);
/* Add the memory map to dev, starting at index idx, returns last use idx */
void amd_opensil_add_memmap(struct device *dev, unsigned long *idx);
/* Fill in FADT from openSIL */
void amd_opensil_fill_fadt_io_ports(acpi_fadt_t *fadt);
/* Marshalls dimm info from SIL_DMI_INFO into CBMEM_ID_MEMINFO */
void opensil_smbios_fill_cbmem_meminfo(void);
/* Fill DIMM locators using OpenSIL DMI info, returns 0 on success */
int opensil_smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t);

#endif /* OPENSIL_DRIVER_H */
