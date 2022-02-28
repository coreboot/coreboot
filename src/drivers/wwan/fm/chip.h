/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_WWAN_FM_CHIP_H__
#define __DRIVERS_WWAN_FM_CHIP_H__

struct drivers_wwan_fm_config {
	const char *name;
	const char *desc;
	/* GPIO used for FULL_CARD_POWER_OFF# */
	struct acpi_gpio fcpo_gpio;

	/* GPIO used for RESET# */
	struct acpi_gpio reset_gpio;

	/* GPIO used for PERST# */
	struct acpi_gpio perst_gpio;

	/* GPIO used for wake */
	struct acpi_gpio wake_gpio;

	/* Pointer to the corresponding RTD3 */
	DEVTREE_CONST struct device *rtd3dev;

	/* Add `DmaProperty` in _DSD */
	bool add_acpi_dma_property;
};

#endif /* __DRIVERS_WWAN_FM_CHIP_H__ */
