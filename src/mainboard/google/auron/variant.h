/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <device/device.h>
#include <soc/romstage.h>
#include <stdint.h>

int variant_smbios_data(struct device *dev, int *handle,
			unsigned long *current);
void lan_init(void);

unsigned int variant_get_spd_index(void);
bool variant_is_dual_channel(const unsigned int spd_index);

#endif
