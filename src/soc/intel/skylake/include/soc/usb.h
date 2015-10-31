/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef _SOC_USB_H_
#define _SOC_USB_H_

#include <stdint.h>

#define USB2_EMP_OFF		0
#define USB2_DE_EMP_ON		1
#define USB2_PRE_EMP_ON		2

#define USB2_FULL_BIT_PRE_EMP	0
#define USB2_HALF_BIT_PRE_EMP	1

#define USB2_BIAS_0MV		0
#define USB2_BIAS_11MV		1
#define USB2_BIAS_17MV		2
#define USB2_BIAS_28MV		3
#define USB2_BIAS_28MV2		4
#define USB2_BIAS_39MV		5
#define USB2_BIAS_45MV		6
#define USB2_BIAS_56MV		7

struct usb2_port_config {
	uint8_t enable;
	uint8_t tx_bias;
	uint8_t tx_emp_enable;
	uint8_t pre_emp_bias;
	uint8_t pre_emp_bit;
};

#define USB2_PORT_EMPTY { \
	.enable        = 0, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_EMP_OFF, \
	.pre_emp_bias  = USB2_BIAS_0MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/*
 * Standard USB Port based on length:
 * - External
 * - Back Panel
 * - OTG
 * - M.2
 * - Internal device down
 */

/* Max TX and Pre-emp settings */
#define USB2_PORT_MAX { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_56MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* 11.5"-12" */
#define USB2_PORT_LONG { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_39MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* 6"-11.5" */
#define USB2_PORT_MID { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* 3"-6" */
#define USB2_PORT_SHORT { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_39MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON | USB2_DE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_39MV, \
	.pre_emp_bit   = USB2_FULL_BIT_PRE_EMP, \
}

/* Type-C Port, no BC1.2 charge detect module / MUX */
#define USB2_PORT_TYPE_C { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Port with BC1.2 charge detect module / MUX */
#define USB2_PORT_BC12_MUX { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Internal Flex Cable, 3"-5" + cable + 2" card */
#define USB2_PORT_FLEX { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Docking, 3"-9" */
#define USB2_PORT_DOCKING_LONG { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Docking, 3"-6" */
#define USB2_PORT_DOCKING_SHORT { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_17MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON | USB2_DE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_45MV, \
	.pre_emp_bit   = USB2_FULL_BIT_PRE_EMP, \
}

/* 2:1 Detachable, 2"-4" on tablet + 2"-4" on base */
#define USB2_PORT_DETACHABLE_TABLET { \
	.enable        = 1, \
	.tx_bias       = USB2_BIAS_56MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

struct usb3_port_config {
	uint8_t enable;
	uint8_t tx_de_emp;
	uint8_t tx_downscale_amp;
};

#define USB3_PORT_EMPTY { \
	.enable           = 0, \
	.tx_de_emp        = 0x00, \
	.tx_downscale_amp = 0x00, \
}

#define USB3_PORT_DEFAULT { \
	.enable           = 1, \
	.tx_de_emp        = 0x29, \
	.tx_downscale_amp = 0x00, \
}

#endif
