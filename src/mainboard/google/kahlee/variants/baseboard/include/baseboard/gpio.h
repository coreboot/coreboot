/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#ifndef __ACPI__
#include <gpio.h>

# define MEM_CONFIG0		GPIO_139
# define MEM_CONFIG1		GPIO_142
# define MEM_CONFIG2		GPIO_131
# define MEM_CONFIG3		GPIO_132

/* SPI Write protect */
#define CROS_WP_GPIO		GPIO_122
#define GPIO_EC_IN_RW		GPIO_15

/* PCIe reset pins */
#define PCIE_0_RST		GPIO_70
#define PCIE_1_RST		0
#define PCIE_2_RST		GPIO_40
#define PCIE_3_RST		0

#endif /* _ACPI__ */

/* These define the GPE, not the GPIO. */
#define EC_SCI_GPI		GEVENT_3	/* AGPIO 22 -> GPE  3 */
#define EC_SMI_GPI		GEVENT_10	/* AGPIO 6  -> GPE 10 */
#define EC_WAKE_GPI		GEVENT_15	/* AGPIO 24 -> GPE 15 */

#endif /* __BASEBOARD_GPIO_H__ */
