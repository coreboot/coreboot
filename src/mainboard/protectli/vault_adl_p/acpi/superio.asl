/* SPDX-License-Identifier: GPL-2.0-only */

#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define IT8659E_SHOW_UARTA
#define IT8659E_SHOW_UARTB

#define IT8659E_EC_IO0
#define IT8659E_EC_IO1
#define IT8659E_SHOW_EC

#define IT8659E_GPIO_IO0
#define IT8659E_GPIO_IO1
#define IT8659E_SHOW_GPIO

#include <superio/ite/it8659e/acpi/superio.asl>
