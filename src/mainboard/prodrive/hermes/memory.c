/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/variants.h>
#include <soc/cnl_memcfg_init.h>

static const struct cnl_mb_cfg baseboard_memcfg_cfg = {
	/* Access memory info through SMBUS. */
	.spd[0] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xA0}
	},
	.spd[1] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xA2}
	},
	.spd[2] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xA4}
	},
	.spd[3] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xA6}
	},

	/* Baseboard uses 121, 81 and 100 rcomp resistors */
	.rcomp_resistor = {121, 81, 100},

	/* Baseboard Rcomp target values. */
	.rcomp_targets = {100, 40, 20, 20, 26},

	/* Baseboard is an interleaved design */
	.dq_pins_interleaved = 1,

	/* Baseboard is using config 2 for vref_ca */
	.vref_ca_config = 2,

	/* Disable Early Command Training */
	.ect = 0,
};

const struct cnl_mb_cfg *variant_memcfg_config(void)
{
	return &baseboard_memcfg_cfg;
}
