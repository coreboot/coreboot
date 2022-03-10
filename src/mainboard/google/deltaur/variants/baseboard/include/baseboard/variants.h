/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stddef.h>
#include <vendorcode/google/chromeos/chromeos.h>

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_base_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_override_gpio_table(size_t *num);

const struct cros_gpio *variant_cros_gpios(size_t *num);

const struct mb_cfg *variant_memory_params(void);
void variant_memory_init(FSPM_UPD *mupd);

/* SKU ID structure */
typedef struct {
	int id;
	const char *name;
} sku_info;

/* Check if the device has a 360 sensor board present */
int has_360_sensor_board(void);

#endif /* __BASEBOARD_VARIANTS_H__ */
