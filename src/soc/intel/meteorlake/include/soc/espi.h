/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_METEORLAKE_ESPI_H_
#define _SOC_METEORLAKE_ESPI_H_

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
#define   COMA_RANGE		0x0 /* 0x3F8 - 0x3FF COM1 */
#define   COMB_RANGE		0x1 /* 0x2F8 - 0x2FF COM2 */
#define ESPI_GEN1_DEC		0x84 /* ESPI IF Generic Decode Range 1 */
#define ESPI_GEN2_DEC		0x88 /* ESPI IF Generic Decode Range 2 */
#define ESPI_GEN3_DEC		0x8c /* ESPI IF Generic Decode Range 3 */
#define ESPI_GEN4_DEC		0x90 /* ESPI IF Generic Decode Range 4 */
#define LGMR			0x98 /* ESPI Generic Memory Range */
#define PCCTL			0xE0 /* PCI Clock Control */
#define   CLKRUN_EN		(1 << 0)

#endif
