/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SKLKBL_THINKPAD_VARIANT_H_
#define _SKLKBL_THINKPAD_VARIANT_H_

void variant_config_gpios(void);

uint8_t variant_memory_sku(void);

void ssdt_add_dgpu(const struct device *dev);

void dgpu_detect(void);

#endif
