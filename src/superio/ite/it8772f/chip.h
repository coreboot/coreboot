/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_ITE_IT8772F_CHIP_H
#define SUPERIO_ITE_IT8772F_CHIP_H

#include <superio/ite/common/env_ctrl_chip.h>

struct superio_ite_it8772f_config {
	struct ite_ec_config ec;

	/* GPIO SimpleIO register values via devicetree.cb */
	u8 gpio_set1;
	u8 gpio_set2;
	u8 gpio_set3;
	u8 gpio_set4;
	u8 gpio_set5;
	u8 gpio_set6;

	bool skip_keyboard;
};

#endif /* SUPERIO_ITE_IT8772F_CHIP_H */
