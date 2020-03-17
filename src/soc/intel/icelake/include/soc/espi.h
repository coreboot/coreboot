/*
 * This file is part of the coreboot project.
 *
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

#ifndef _SOC_ICELAKE_ESPI_H_
#define _SOC_ICELAKE_ESPI_H_

#include <stdint.h>

/* PCI Configuration Space (D31:F0): ESPI */
#define SCI_IRQ_SEL		(7 << 0)
#define SCIS_IRQ9		0
#define SCIS_IRQ10		1
#define SCIS_IRQ11		2
#define SCIS_IRQ20		4
#define SCIS_IRQ21		5
#define SCIS_IRQ22		6
#define SCIS_IRQ23		7
#define SERIRQ_CNTL		0x64
#define ESPI_IO_DEC		0x80 /* IO Decode Ranges Register */
#define   COMA_RANGE		0x0 /* 0x3F8 - 0x3FF COM1*/
#define   COMB_RANGE		0x1 /* 0x2F8 - 0x2FF COM2*/
#define ESPI_GEN1_DEC		0x84 /* ESPI IF Generic Decode Range 1 */
#define ESPI_GEN2_DEC		0x88 /* ESPI IF Generic Decode Range 2 */
#define ESPI_GEN3_DEC		0x8c /* ESPI IF Generic Decode Range 3 */
#define ESPI_GEN4_DEC		0x90 /* ESPI IF Generic Decode Range 4 */
#define LGMR			0x98 /* ESPI Generic Memory Range */
#define PCCTL			0xE0 /* PCI Clock Control */
#define   CLKRUN_EN		(1 << 0)

/*
 * This function will help to differentiate between 2 PCH on single type of soc.
 * Since same soc may have LP series pch or H series PCH, we need to
 * differentiate by reading upper 8 bits of PCH device ids.
 *
 * Return:
 *	Return PCH_LP or PCH_H macro in case of respective device ID found.
 *	PCH_UNKNOWN_SERIES in case of invalid device ID.
 */
uint8_t get_pch_series(void);

#endif
