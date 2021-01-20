/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <device/device.h>
#include <soc/romstage.h>
#include <stdint.h>

int variant_smbios_data(struct device *dev, int *handle,
			unsigned long *current);
void variant_romstage_entry(struct romstage_params *rp);
void lan_init(void);

void mainboard_fill_spd_data(struct pei_data *pei_data);
void mainboard_print_spd_info(uint8_t spd[]);

#define SPD_LEN			256

#endif
