/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <device/device.h>
#include <soc/romstage.h>

int variant_smbios_data(struct device *dev, int *handle,
			unsigned long *current);
void variant_romstage_entry(struct romstage_params *rp);
void lan_init(void);

#endif
