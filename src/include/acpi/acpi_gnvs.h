/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_GNVS_H__
#define __ACPI_GNVS_H__

#include <types.h>

void *acpi_get_gnvs(void);
void *gnvs_get_or_create(void);
void acpi_inject_nvsa(void);

void gnvs_assign_chromeos(void);

/* Platform code must implement these. */
size_t gnvs_size_of_array(void);
uint32_t *gnvs_cbmc_ptr(void);
void *gnvs_chromeos_ptr(void);

#endif
