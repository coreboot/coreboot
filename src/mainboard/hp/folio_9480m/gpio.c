/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/lynxpoint/lp_gpio.h>

const struct pch_lp_gpio_map mainboard_gpio_map[] = {
	[0]  = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[1]  = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[2]  = LP_GPIO_OUT_LOW,
	[3]  = LP_GPIO_OUT_HIGH,
	[4]  = LP_GPIO_OUT_HIGH,
	[5]  = LP_GPIO_OUT_HIGH,
	[6]  = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[7]  = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[8]  = LP_GPIO_OUT_HIGH,
	[9]  = LP_GPIO_OUT_HIGH,
	[10] = LP_GPIO_OUT_HIGH,
	[11] = LP_GPIO_OUT_HIGH,
	[12] = LP_GPIO_NATIVE,
	[13] = LP_GPIO_OUT_HIGH,
	[14] = LP_GPIO_OUT_HIGH,
	[15] = LP_GPIO_OUT_HIGH,
	[16] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL,
		 .route = GPIO_ROUTE_SMI },
	[17] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[18] = LP_GPIO_OUT_HIGH,
	[19] = LP_GPIO_NATIVE,
	[20] = LP_GPIO_NATIVE,
	[21] = LP_GPIO_NATIVE,
	[22] = LP_GPIO_OUT_HIGH,
	[23] = LP_GPIO_OUT_HIGH,
	[24] = LP_GPIO_OUT_HIGH,
	[25] = LP_GPIO_OUT_HIGH,
	[26] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[27] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[28] = LP_GPIO_OUT_HIGH,
	[29] = LP_GPIO_OUT_HIGH,
	[30] = LP_GPIO_NATIVE,
	[31] = LP_GPIO_NATIVE,
	[32] = LP_GPIO_NATIVE,
	[33] = LP_GPIO_NATIVE,
	[34] = LP_GPIO_OUT_HIGH,
	[35] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[36] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL,
		 .route = GPIO_ROUTE_SMI },
	[37] = LP_GPIO_NATIVE,
	[38] = LP_GPIO_NATIVE,
	[39] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL,
		 .route = GPIO_ROUTE_SMI },
	[40] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL,
		 .route = GPIO_ROUTE_SMI },
	[41] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[42] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[43] = LP_GPIO_OUT_HIGH,
	[44] = LP_GPIO_OUT_LOW,
	[45] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT | GPIO_IRQ_LEVEL },
	[46] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[47] = LP_GPIO_OUT_HIGH,
	[48] = LP_GPIO_OUT_LOW,
	[49] = LP_GPIO_OUT_HIGH,
	[50] = LP_GPIO_OUT_HIGH,
	[51] = LP_GPIO_OUT_HIGH,
	[52] = LP_GPIO_OUT_HIGH,
	[53] = LP_GPIO_OUT_HIGH,
	[54] = LP_GPIO_OUT_LOW,
	[55] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL,
		 .pirq = GPIO_PIRQ_APIC_ROUTE },
	[56] = LP_GPIO_OUT_HIGH,
	[57] = LP_GPIO_OUT_LOW,
	[58] = LP_GPIO_OUT_HIGH,
	[59] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[60] = LP_GPIO_OUT_HIGH,
	[61] = LP_GPIO_OUT_LOW,
	[62] = LP_GPIO_NATIVE,
	[63] = LP_GPIO_NATIVE,
	[64] = LP_GPIO_OUT_HIGH,
	[65] = LP_GPIO_OUT_LOW,
	[66] = LP_GPIO_OUT_HIGH,
	[67] = LP_GPIO_OUT_HIGH,
	[68] = LP_GPIO_OUT_HIGH,
	[69] = LP_GPIO_OUT_HIGH,
	[70] = LP_GPIO_OUT_LOW,
	[71] = LP_GPIO_NATIVE,
	[72] = LP_GPIO_NATIVE,
	[73] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[74] = LP_GPIO_NATIVE,
	[75] = LP_GPIO_NATIVE,
	[76] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[77] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[78] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL,
		 .route = GPIO_ROUTE_SMI },
	[79] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[80] = LP_GPIO_OUT_LOW,
	[81] = LP_GPIO_NATIVE,
	[82] = LP_GPIO_OUT_HIGH,
	[83] = LP_GPIO_OUT_HIGH,
	[84] = LP_GPIO_OUT_HIGH,
	[85] = LP_GPIO_OUT_HIGH,
	[86] = LP_GPIO_OUT_HIGH,
	[87] = LP_GPIO_OUT_HIGH,
	[88] = LP_GPIO_OUT_HIGH,
	[89] = LP_GPIO_OUT_HIGH,
	[90] = LP_GPIO_OUT_HIGH,
	[91] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[92] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[93] = { .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL },
	[94] = LP_GPIO_OUT_HIGH,
	LP_GPIO_END
};
