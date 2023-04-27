/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_NET_PHY_M88E1512_CHIP_H__
#define __DRIVERS_NET_PHY_M88E1512_CHIP_H__

#include "m88e1512.h"

struct drivers_net_phy_m88e1512_config {
	bool configure_leds;		/* Enable LED customization */
	unsigned char led_0_ctrl;	/* LED[0] Control */
	unsigned char led_1_ctrl;	/* LED[1] Control */
	unsigned char led_2_ctrl;	/* LED[2] Control */
	bool enable_int;		/* INTn can be routed to LED[2] pin */
	/* 1x, 2x,...8x is the number of times the PHY attempts to establish Gigabit link
	   before the PHY downshifts to the next highest speed. */
	unsigned char downshift_cnt;
	bool force_mos;			/* Force PMOS/NMOS manually */
	unsigned char pmos_val;		/* Set PMOS calibration value */
	unsigned char nmos_val;		/* Set NMOS calibration value */
};

#endif /* __DRIVERS_NET_PHY_M88E1512_CHIP_H__ */
