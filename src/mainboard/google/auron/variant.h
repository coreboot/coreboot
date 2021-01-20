/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <device/device.h>
#include <soc/romstage.h>
#include <stdint.h>

int variant_smbios_data(struct device *dev, int *handle,
			unsigned long *current);
void lan_init(void);

void fill_spd_for_index(uint8_t spd[], unsigned int index);

#define SPD_LEN			256

#endif
