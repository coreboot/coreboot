/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>

/*
 * Psys_pmax considerations
 *
 * Given the hardware design in dibbi, the serial shunt resistor is 0.01ohm.
 * The full scale of hardware PSYS signal 1.6v maps to system current 6.009A
 * instead of real system power. The equation is shown below:
 * PSYS = 1.6v ~= (0.01ohm x 6.009A) x 50 (INA213, gain 50V/V) x R631/(R631 + R638)
 * R631/(R631 + R638) = 0.5325 = 36K / (36K + 31.6K)
 *
 * The Psys_pmax is a SW setting which tells IMVP9.1 the mapping between system input
 * current and the actual system power. Since there is no voltage information
 * from PSYS, different voltage input would map to different Psys_pmax settings:
 * For Type-C 15V, the Psys_pmax should be 15v x 6.009A = 90.135W
 * For Type-C 20V, the Psys_pmax should be 20v x 6.009A = 120.18W
 * For a barrel jack, the Psys_pmax should be 19v x 6.009A = 114.171W
 *
 * Imagine that there is a type-c 100W (20V/5A) connected to DUT w/ full loading,
 * and the Psys_pmax setting is 120W. Then IMVP9.1 can calculate the current system
 * power = 120W * 5A / 6.009A = 100W, which is the actual system power.
 */
const struct psys_config psys_config = {
	.efficiency = 97,
	.psys_imax_ma = 6009,
	.bj_volts_mv = 19000,
	.bj_power_w = 65,
};

void variant_devtree_update(void)
{
	variant_update_psys_power_limits(&psys_config);
}
