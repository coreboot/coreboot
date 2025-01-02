/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_COMMON_DEFINES_H
#define __MAINBOARD_COMMON_DEFINES_H

#include <superio/ite/it8772f/it8772f.h>

#define IT8772F_BASE	0x2e
#define EC_DEV		PNP_DEV(IT8772F_BASE, IT8772F_EC)
#define GPIO_DEV	PNP_DEV(IT8772F_BASE, IT8772F_GPIO)

#endif /* __MAINBOARD_COMMON_DEFINES_H */
