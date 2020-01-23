/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_F81216H_CHIP_H
#define SUPERIO_FINTEK_F81216H_CHIP_H

#include <stdint.h>

/* Member variables are defined in devicetree.cb. */
struct superio_fintek_f81216h_config {
	/**
	 *  KEY1   KEY0   Enter key
	 *  0      0      0x77 (default)
	 *  0      1      0xA0
	 *  1      0      0x87
	 *  1      1      0x67
	 *
	 *  See page 17 of data sheet.
	 */
	uint8_t conf_key_mode;
};

#endif /* SUPERIO_FINTEK_F81216H_CHIP_H */
