/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DENVERTON_NS_GPIO_H_
#define _SOC_DENVERTON_NS_GPIO_H_

#include <soc/gpio_defs.h>

#define GPIO_MISCCFG		0x10 /* Miscellaneous Configuration offset */

#define GPIO_MAX_NUM_PER_GROUP	32

#define NUM_NC_GPI_REGS	\
	(ALIGN_UP(V_PCH_GPIO_NC_PAD_MAX, GPIO_MAX_NUM_PER_GROUP) \
	/ GPIO_MAX_NUM_PER_GROUP)

#define NUM_SC_DFX_GPI_REGS	\
	(ALIGN_UP(V_PCH_GPIO_SC_DFX_PAD_MAX, GPIO_MAX_NUM_PER_GROUP) \
	/ GPIO_MAX_NUM_PER_GROUP)

#define NUM_SC0_GPI_REGS	\
	(ALIGN_UP(V_PCH_GPIO_SC0_PAD_MAX, GPIO_MAX_NUM_PER_GROUP) \
	/ GPIO_MAX_NUM_PER_GROUP)

#define NUM_SC1_GPI_REGS	\
	(ALIGN_UP(V_PCH_GPIO_SC1_PAD_MAX, GPIO_MAX_NUM_PER_GROUP) \
	/ GPIO_MAX_NUM_PER_GROUP)

#define NUM_GPI_STATUS_REGS (NUM_NC_GPI_REGS + NUM_SC_DFX_GPI_REGS +\
			     NUM_SC0_GPI_REGS + NUM_SC1_GPI_REGS)

#define GPIO_NUM_PAD_CFG_REGS   2 /* DW0, DW1 */

#include <intelblocks/gpio.h>/* intelblocks/gpio.h depends on definitions in
				lines above and soc/gpio_defs.h */

#endif /* _SOC_DENVERTON_NS_GPIO_H_ */
