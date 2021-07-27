/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/qcom_qup_se.h>

struct qup qup[12] = {
	[0] = { .regs = (void *)QUP_SERIAL0_BASE,
		.pin = { GPIO(34), GPIO(35), GPIO(36), GPIO(37) },
		.func = { GPIO34_FUNC_QUP0_L0, GPIO35_FUNC_QUP0_L1,
			GPIO36_FUNC_QUP0_L2, GPIO37_FUNC_QUP0_L3 }
		},
	[1] = { .regs = (void *)QUP_SERIAL1_BASE,
		.pin = { GPIO(0), GPIO(1), GPIO(2), GPIO(3),
			GPIO(12), GPIO(94) },
		.func = { GPIO0_FUNC_QUP0_L0, GPIO1_FUNC_QUP0_L1,
			GPIO2_FUNC_QUP0_L2, GPIO3_FUNC_QUP0_L3,
			GPIO12_FUNC_QUP0_L4, GPIO94_FUNC_QUP0_L5 }
		},
	[2] = { .regs = (void *)QUP_SERIAL2_BASE,
		.pin = { GPIO(15), GPIO(16) },
		.func = { GPIO15_FUNC_QUP0_L0, GPIO16_FUNC_QUP0_L1 }
		},
	[3] = { .regs = (void *)QUP_SERIAL3_BASE,
		.pin = { GPIO(38), GPIO(39), GPIO(40), GPIO(41) },
		.func = { GPIO38_FUNC_QUP0_L0, GPIO39_FUNC_QUP0_L1,
			GPIO40_FUNC_QUP0_L2, GPIO41_FUNC_QUP0_L3 }
		},
	[4] = { .regs = (void *)QUP_SERIAL4_BASE,
		.pin = { GPIO(115), GPIO(116) },
		.func = { GPIO115_FUNC_QUP0_L0, GPIO116_FUNC_QUP0_L1 }
		},
	[5] = { .regs = (void *)QUP_SERIAL5_BASE,
		.pin = { GPIO(25), GPIO(26), GPIO(27), GPIO(28) },
		.func = { GPIO25_FUNC_QUP0_L0, GPIO26_FUNC_QUP0_L1,
			GPIO27_FUNC_QUP0_L2, GPIO28_FUNC_QUP0_L3 }
		},
	[6] = { .regs = (void *)QUP_SERIAL6_BASE,
		.pin = { GPIO(59), GPIO(60), GPIO(61), GPIO(62),
			GPIO(68), GPIO(72) },
		.func = { GPIO59_FUNC_QUP1_L0, GPIO60_FUNC_QUP1_L1,
			GPIO61_FUNC_QUP1_L2, GPIO62_FUNC_QUP1_L3,
			GPIO68_FUNC_QUP1_L4, GPIO72_FUNC_QUP1_L5 }
		},
	[7] = { .regs = (void *)QUP_SERIAL7_BASE,
		.pin = { GPIO(6), GPIO(7) },
		.func = { GPIO6_FUNC_QUP1_L0, GPIO7_FUNC_QUP1_L1 }
		},
	[8] = { .regs = (void *)QUP_SERIAL8_BASE,
		.pin = { GPIO(42), GPIO(43), GPIO(44), GPIO(45) },
		.func = { GPIO42_FUNC_QUP1_L0, GPIO43_FUNC_QUP1_L1,
			GPIO44_FUNC_QUP1_L2, GPIO45_FUNC_QUP1_L3 }
		},
	[9] = { .regs = (void *)QUP_SERIAL9_BASE,
		.pin = { GPIO(46), GPIO(47) },
		.func = { GPIO46_FUNC_QUP1_L0, GPIO47_FUNC_QUP1_L1 }
		},
	[10] = { .regs = (void *)QUP_SERIAL10_BASE,
		.pin = { GPIO(86), GPIO(87), GPIO(88), GPIO(89),
			GPIO(90), GPIO(91) },
		.func = { GPIO86_FUNC_QUP1_L0, GPIO87_FUNC_QUP1_L1,
			GPIO88_FUNC_QUP1_L2, GPIO89_FUNC_QUP1_L3,
			GPIO90_FUNC_QUP1_L4, GPIO91_FUNC_QUP1_L5 }
		},
	[11] = { .regs = (void *)QUP_SERIAL11_BASE,
		.pin = { GPIO(53), GPIO(54), GPIO(55), GPIO(56) },
		.func = { GPIO53_FUNC_QUP1_L0, GPIO54_FUNC_QUP1_L1,
			GPIO55_FUNC_QUP1_L2, GPIO56_FUNC_QUP1_L3 }
		},
};
