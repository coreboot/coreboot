/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_PC80_TPM_CHIP_H
#define DRIVERS_PC80_TPM_CHIP_H

struct drivers_pc80_tpm_config {
	/*
	 * TPM Interrupt polarity:
	 *
	 *  High Level    0
	 *  Low Level     1
	 *  Rising Edge   2
	 *  Falling Edge  3
	 */
	u8 irq_polarity;
};

#endif /* DRIVERS_PC80_TPM_CHIP_H */
