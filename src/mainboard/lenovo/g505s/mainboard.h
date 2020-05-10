/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * "The way things are connected" and a few setup options
 */

#ifndef _MAINBOARD_LENOVO_G505S_MAINBOARD_H
#define _MAINBOARD_LENOVO_G505S_MAINBOARD_H

/* What is connected to GEVENT pins */
#define EC_SCI_GEVENT		3
#define EC_LID_GEVENT		22
#define EC_SMI_GEVENT		23
#define PCIE_GEVENT		8

/* Any GEVENT pin can be mapped to any GPE. We try to keep the mapping 1:1, but
 * we make the distinction between GEVENT pin and SCI.
 */
#define EC_SCI_GPE		EC_SCI_GEVENT
#define EC_LID_GPE		EC_LID_GEVENT
#define PME_GPE			0x0b
#define PCIE_GPE		0x18

/* Enable PS/2 Keyboard and Mouse */
#define SIO_EC_ENABLE_PS2K

#endif /* _MAINBOARD_LENOVO_G505S_MAINBOARD_H */
