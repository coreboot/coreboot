/*
 * X-Powers AXP 209 devicetree.cb interface
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef AXP209_CHIP_H
#define AXP209_CHIP_H

#include <types.h>

struct drivers_xpowers_axp209_config {
	u16 dcdc2_voltage_mv;	/**< DCDC2 converter voltage output */
	u16 dcdc3_voltage_mv;	/**< DCDC3 converter voltage output */
	u16 ldo2_voltage_mv;	/**< LDO2 regulator voltage output */
	u16 ldo3_voltage_mv;	/**< LDO3 regulator voltage output */
	u16 ldo4_voltage_mv;	/**< LDO4 regulator voltage output */
};

#endif				/* AXP209_CHIP_H */
