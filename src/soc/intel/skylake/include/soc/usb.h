/* SPDX-License-Identifier: GPL-2.0-only */

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
	uint8_t ocpin;
	uint8_t tx_bias;
	uint8_t tx_emp_enable;
	uint8_t pre_emp_bias;
	uint8_t pre_emp_bit;
};

/* USB Overcurrent pins definition */
enum {
	OC0,
	OC1,
	OC2,
	OC3,
	OC4,
	OC5,
	OC6,
	OC7,
	OC_SKIP = 8, /* Skip OC programming */
};

#define USB2_PORT_EMPTY { \
	.enable        = 0, \
	.ocpin         = OC_SKIP, \
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
#define USB2_PORT_MAX(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_56MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* 11.5"-12" */
#define USB2_PORT_LONG(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_39MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* 6"-11.5" */
#define USB2_PORT_MID(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* 3"-6" */
#define USB2_PORT_SHORT(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_39MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON | USB2_DE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_39MV, \
	.pre_emp_bit   = USB2_FULL_BIT_PRE_EMP, \
}

/* Type-C Port, no BC1.2 charge detect module / MUX */
#define USB2_PORT_TYPE_C(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Port with BC1.2 charge detect module / MUX */
#define USB2_PORT_BC12_MUX(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Internal Flex Cable, 3"-5" + cable + 2" card */
#define USB2_PORT_FLEX(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Docking, 3"-9" */
#define USB2_PORT_DOCKING_LONG(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_0MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

/* Docking, 3"-6" */
#define USB2_PORT_DOCKING_SHORT(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_17MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON | USB2_DE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_45MV, \
	.pre_emp_bit   = USB2_FULL_BIT_PRE_EMP, \
}

/* 2:1 Detachable, 2"-4" on tablet + 2"-4" on base */
#define USB2_PORT_DETACHABLE_TABLET(pin) { \
	.enable        = 1, \
	.ocpin         = pin, \
	.tx_bias       = USB2_BIAS_56MV, \
	.tx_emp_enable = USB2_PRE_EMP_ON, \
	.pre_emp_bias  = USB2_BIAS_56MV, \
	.pre_emp_bit   = USB2_HALF_BIT_PRE_EMP, \
}

struct usb3_port_config {
	uint8_t enable;
	uint8_t ocpin;
	uint8_t tx_de_emp;
	uint8_t tx_downscale_amp;
};

#define USB3_PORT_EMPTY { \
	.enable           = 0, \
	.ocpin            = OC_SKIP, \
	.tx_de_emp        = 0x00, \
	.tx_downscale_amp = 0x00, \
}

#define USB3_PORT_DEFAULT(pin) { \
	.enable           = 1, \
	.ocpin            = pin, \
	.tx_de_emp        = 0x29, \
	.tx_downscale_amp = 0x00, \
}

#endif
