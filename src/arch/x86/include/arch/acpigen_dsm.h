/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __ARCH_ACPIGEN_DSM_H__
#define __ARCH_ACPIGEN_DSM_H__

#include <stdint.h>

struct dsm_i2c_hid_config {
	uint8_t hid_desc_reg_offset;
};

void acpigen_write_dsm_i2c_hid(struct dsm_i2c_hid_config *config);

#endif /* __ARCH_ACPIGEN_DSM_H__ */
