/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <device/resource.h>
#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>

/* SD/eMMC capabilities bypass control register */
#define MMC_CAP_BYP		0x810
#define  MMC_CAP_BYP_EN		0x5A
/* SD/eMMC capabilities bypass register 1 */
#define MMC_CAP_BYP_REG1	0x814
#define  MMC_CAP_BYP_SDR50	(1 << 13)
#define  MMC_CAP_BYP_SDR104	(1 << 14)
#define  MMC_CAP_BYP_DDR50	(1 << 15)

/* The following 2 functions return the gpio table and fill in the number
 * of entries for each table. */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

/* This function returns SPD related FSP-M mainboard configs */
const struct mb_cfg *variant_memcfg_config(void);

/* This function can select half-populate DRAM */
bool half_populated(void);

/* The following functions perform board specific things. */
void disable_sdr_modes(struct resource *res);
void variant_mainboard_init(void);
void variant_mainboard_final(void);

#endif /*__BASEBOARD_VARIANTS_H__ */
