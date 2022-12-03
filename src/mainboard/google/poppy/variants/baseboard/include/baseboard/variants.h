/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <stdint.h>

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

/*
 * Callback to get GPIOs to configure in romstage before memory training is
 * performed.
 */
const struct pad_config *variant_romstage_gpio_table(size_t *num);

/* Config gpio by different sku id */
const struct pad_config *variant_sku_gpio_table(size_t *num);

enum memory_type {
	MEMORY_LPDDR3,
	MEMORY_DDR4,
	MEMORY_COUNT,
};

struct memory_params {
	enum memory_type type;
	const void *dq_map;
	size_t dq_map_size;
	const void *dqs_map;
	size_t dqs_map_size;
	const void *rcomp_resistor;
	size_t rcomp_resistor_size;
	const void *rcomp_target;
	size_t rcomp_target_size;
	bool use_sec_spd;

	/* Enable SA overclocking mailbox commands */
	bool enable_sa_oc_support;

	/* The voltage offset applied to the SA in mV. 1000(mV) = Maximum */
	uint16_t sa_voltage_offset_val;

	/* This would be set to true if only have single DDR channel */
	bool single_channel;
};

void variant_memory_params(struct memory_params *p);
int variant_memory_sku(void);
void variant_devtree_update(void);
uint32_t variant_board_sku(void);
void variant_smi_sleep(u8 slp_typ);
void variant_final(void);

struct nhlt;
void variant_nhlt_init(struct nhlt *nhlt);
void variant_nhlt_oem_overrides(const char **oem_id, const char **oem_table_id,
				uint32_t *oem_revision);

struct google_chromeec_event_info;
/*
 * Read google_chromeec_event_info structure from variant to set different masks
 * on the EC e.g. SCI, S3, S5, S0ix, SMI.
 */
const struct google_chromeec_event_info *variant_get_event_info(void);

#endif /* __BASEBOARD_VARIANTS_H__ */
