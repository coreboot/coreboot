/*
 * "The way things are connected" and a few setup options
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef _MAINBOARD_HP_PAVILION_M6_1035DX_MAINBOARD_H
#define _MAINBOARD_HP_PAVILION_M6_1035DX_MAINBOARD_H

/* What is connected to GEVENT pins */
#define EC_SCI_GEVENT		3
#define EC_LID_GEVENT		22
#define EC_SMI_GEVENT		23
#define PCIE_GEVENT		8

/* Any GEVENT pin can be mapped to any GPE. We try to keep the mapping 1:1, but
 * we make the distinction between GEVENT pin and SCI.
 */
#define EC_SCI_GPE 		EC_SCI_GEVENT
#define EC_LID_GPE		EC_LID_GEVENT
#define PME_GPE			0x0b
#define PCIE_GPE		0x18

#endif /* _MAINBOARD_HP_PAVILION_M6_1035DX_MAINBOARD_H   */
