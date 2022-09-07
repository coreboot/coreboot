/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <soc/cnl_memcfg_init.h>
#include <string.h>

static const struct cnl_mb_cfg baseboard_memcfg = {
	/* Baseboard uses 121, 81 and 100 rcomp resistors */
	.rcomp_resistor = {121, 81, 100},

	/* Baseboard Rcomp target values */
	.rcomp_targets = {100, 40, 20, 20, 26},

	/* Set CaVref config to 2 */
	.vref_ca_config = 2,

	/* Enable Early Command Training */
	.ect = 1,
};

void __weak variant_memory_params(struct cnl_mb_cfg *bcfg)
{
	memcpy(bcfg, &baseboard_memcfg, sizeof(baseboard_memcfg));
}
