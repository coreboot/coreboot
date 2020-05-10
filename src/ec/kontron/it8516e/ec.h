/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_KONTRON_IT8516E_EC_H
#define EC_KONTRON_IT8516E_EC_H

enum { /* PNP logical device numbers */
	IT8516E_LDN_UART1	= 0x01,
	IT8516E_LDN_UART2	= 0x02,
	IT8516E_LDN_SWUC	= 0x04,
	IT8516E_LDN_MOUSE	= 0x05,
	IT8516E_LDN_KBD		= 0x06,
	IT8516E_LDN_SMFI	= 0x0f,
	IT8516E_LDN_BRAM	= 0x10,
	IT8516E_LDN_PM1		= 0x11,
	IT8516E_LDN_PM2		= 0x12,
	IT8516E_LDN_PM3		= 0x17,
};

enum it8516e_fan_modes { /* Possible modes of fan control */
	IT8516E_MODE_AUTO	= 0x80,
	IT8516E_MODE_PWM	= 0x01,
	IT8516E_MODE_SPEED	= 0x02,
	IT8516E_MODE_THERMAL	= 0x03,
};

enum it8516e_systemp_types { /* Possible sources of system temperature */
	IT8516E_SYSTEMP_NONE	= 0x00,
	IT8516E_SYSTEMP_AMD	= 0x01,
	IT8516E_SYSTEMP_LM75_90	= 0x02,
	IT8516E_SYSTEMP_GPIO16	= 0x03,
	IT8516E_SYSTEMP_LM75_9e	= 0x04,

	/* add any new types above this line. */
	IT8516E_SYSTEMP_LASTPLUSONE,
};

#endif
