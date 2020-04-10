/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __OCP_DMI_H
#define __OCP_DMI_H

#include <cpu/x86/msr.h>
#include <device/device.h>
#include <smbios.h>

#define TBF "To Be Filled By O.E.M."

extern msr_t xeon_sp_ppin[];

/* Override SMBIOS type 11 OEM string 1 to string 6 */
void ocp_oem_smbios_strings(struct device *dev, struct smbios_type11 *t);

/* This function allows adding the same repeated string to the table */
int smbios_add_oem_string(u8 *start, const char *str);

#endif
