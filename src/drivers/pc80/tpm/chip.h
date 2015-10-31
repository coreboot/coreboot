/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef DRIVERS_PC80_TPM_CHIP_H
#define DRIVERS_PC80_TPM_CHIP_H

typedef struct drivers_pc80_tpm_config {
	/*
	 * TPM Interrupt polarity:
	 *
	 *  High Level    0
	 *  Low Level     1
	 *  Rising Edge   2
	 *  Falling Edge  3
	 */
	u8 irq_polarity;
} tpm_config_t;

#endif /* DRIVERS_PC80_TPM_CHIP_H */
