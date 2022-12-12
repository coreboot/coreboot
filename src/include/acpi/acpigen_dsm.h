/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_ACPIGEN_DSM_H__
#define __ACPI_ACPIGEN_DSM_H__

#include <stdint.h>

struct dsm_i2c_hid_config {
	uint8_t hid_desc_reg_offset;
};

void acpigen_write_dsm_i2c_hid(struct dsm_i2c_hid_config *config);

struct dsm_usb_config {
	uint8_t usb_lpm_incapable;
};
void acpigen_write_dsm_usb(struct dsm_usb_config *config);

#endif /* __ACPI_ACPIGEN_DSM_H__ */
