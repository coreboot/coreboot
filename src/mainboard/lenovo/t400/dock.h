/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef THINKPAD_T400_DOCK_H
#define THINKPAD_T400_DOCK_H

#ifndef __ACPI__
int pc87382_early(void);

void dock_connect(void);
void dock_disconnect(void);
int dock_present(void);
void dock_info(void);
#endif

/* pc87382 */
#define DLPC_CONTROL	0x164c
#define DLPC_GPIO_BASE	0x1680

#define DLPC_GPDO0		(DLPC_GPIO_BASE + 0x0)
#define DLPC_GPDI0		(DLPC_GPIO_BASE + 0x1)
#define		D_PLTRST	0x01
#define		D_LPCPD		0x02

#define DLPC_GPDO2		(DLPC_GPIO_BASE + 0x8)
#define DLPC_GPDI2		(DLPC_GPIO_BASE + 0x9)

 /* Pullups on all GPIOs, dock pulls ID pins low */
#define DOCK_TYPE_2504 1
#define DOCK_TYPE_2505 2
#define DOCK_TYPE_NONE 7

/* pc87384 */
#define SUPERIO_DEV 0x2e
#define DOCK_GPIO_BASE	0x1620

#endif
