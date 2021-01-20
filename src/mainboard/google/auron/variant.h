/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <device/device.h>
#include <soc/romstage.h>

int variant_smbios_data(struct device *dev, int *handle,
			unsigned long *current);
void variant_romstage_entry(struct romstage_params *rp);
void lan_init(void);

void mainboard_fill_spd_data(struct pei_data *pei_data);

#define SPD_LEN			256

#define SPD_DRAM_TYPE		2
#define SPD_DRAM_DDR3		0x0b
#define SPD_DRAM_LPDDR3		0xf1
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_ORGANIZATION	7
#define SPD_BUS_DEV_WIDTH	8
#define SPD_PART_OFF		128
#define SPD_PART_LEN		18

#endif
