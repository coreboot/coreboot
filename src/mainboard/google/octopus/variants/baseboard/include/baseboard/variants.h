/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BASEBOARD_VARIANTS_H
#define BASEBOARD_VARIANTS_H

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>

/* The next set of functions return the gpio table and fill in the number of
 * entries for each table. */
const struct pad_config *mainboard_early_bootblock_gpio_table(size_t *num);
const struct pad_config *baseboard_gpio_table(size_t *num);
const struct pad_config *variant_override_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_early_override_gpio_table(size_t *num);
const struct pad_config *variant_sleep_gpio_table(size_t *num, int slp_typ);
const struct pad_config *variant_romstage_gpio_table(size_t *num);

/* Baseboard default swizzle. Can be reused if swizzle is same. */
extern const struct lpddr4_swizzle_cfg baseboard_lpddr4_swizzle;
/* Return LPDDR4 configuration structure. */
const struct lpddr4_cfg *variant_lpddr4_config(void);
/* Return memory SKU for the board. */
size_t variant_memory_sku(void);

/* Seed the NHLT tables with the board specific information. */
struct nhlt;
void variant_nhlt_init(struct nhlt *nhlt);

/* Modify devictree settings during ramstage. */
struct device;
void variant_update_devtree(struct device *dev);
/**
 * variant_ext_usb_status() - Get status of externally visible USB ports
 * @port_type: Type of USB port i.e. USB2/USB3
 * @port_id: USB Port ID
 *
 * This function is supplied by the mainboard/variant to SoC's XHCI driver to
 * identify the status of externally visible USB ports.
 *
 * Return: true if the port is present, false if the port is absent.
 */
bool variant_ext_usb_status(unsigned int port_type, unsigned int port_id);

/* Get no touchscreen SKU ID. */
bool no_touchscreen_sku(uint32_t sku_id);

/* allow each variants to customize smi sleep flow. */
void variant_smi_sleep(u8 slp_typ);

/* LTE power off sequence:
 * GPIO_161 -> 30ms -> GPIO_117 -> 100ms -> GPIO_67 */
void power_off_lte_module(void);

#endif /* BASEBOARD_VARIANTS_H */
