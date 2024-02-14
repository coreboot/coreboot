/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8659E_H
#define SUPERIO_ITE_IT8659E_H

#define IT8659E_SP1  0x01 /* Com1 */
#define IT8659E_SP2  0x02 /* Com2 */
#define IT8659E_EC   0x04 /* Environment controller */
#define IT8659E_KBCK 0x05 /* PS/2 keyboard */
#define IT8659E_KBCM 0x06 /* PS/2 mouse */
#define IT8659E_GPIO 0x07 /* GPIO */
#define IT8659E_CIR  0x0A /* CIR */

/* GPIO Polarity Select: 1: Inverting, 0: Non-inverting */
#define GPIO_REG_POLARITY(x)	(0xb0 + (x))
#define   GPIO_POL_NO_INVERT	0
#define   GPIO_POL_INVERT	1

/* GPIO Internal Pull-up: 1: Enable, 0: Disable */
#define GPIO_REG_PULLUP(x)	(0xb8 + (x))
#define   GPIO_PULLUP_DIS		0
#define   GPIO_PULLUP_EN		1

/* GPIO Function Select: 1: Simple I/O, 0: Alternate function */
#define GPIO_REG_ENABLE(x)	(0xc0 + (x))
#define   GPIO_ALT_FN		0
#define   GPIO_SIMPLE_IO	1

/* GPIO Mode: 0: input mode, 1: output mode */
#define GPIO_REG_OUTPUT(x)	(0xc8 + (x))
#define   GPIO_INPUT_MODE	0
#define   GPIO_OUTPUT_MODE	1

#endif /* SUPERIO_ITE_IT8659E_H */
