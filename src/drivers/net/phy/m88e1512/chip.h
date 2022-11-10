/* SPDX-License-Identifier: GPL-2.0-only */

#include "m88e1512.h"

struct drivers_net_phy_m88e1512_config {
	bool configure_leds;		/* Enable LED customization */
	unsigned char led_0_ctrl;	/* LED[0] Control */
	unsigned char led_1_ctrl;	/* LED[1] Control */
	unsigned char led_2_ctrl;	/* LED[2] Control */
	bool enable_int;		/* INTn can be routed to LED[2] pin */
};
