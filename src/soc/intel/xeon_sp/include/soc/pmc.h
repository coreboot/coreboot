/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_PMC_H_
#define _SOC_PMC_H_

 /* PCI Configuration Space (D31:F2): PMC */
#define ABASE			0x40
#define ACTL			0x44
#define PMC_ACPI_CNT 0x44
#define  PWRM_EN		(1 << 8)
#define  ACPI_EN		(1 << 7)
#define  SCI_IRQ_SEL		(7 << 0)
#define SCI_IRQ_ADJUST		0

#define  SCIS_IRQ9		0
#define  SCIS_IRQ10		1
#define  SCIS_IRQ11		2
#define  SCIS_IRQ20		4
#define  SCIS_IRQ21		5
#define  SCIS_IRQ22		6
#define  SCIS_IRQ23		7
#define PWRMBASE		0x48
#define GEN_PMCON_A		0xa0
#define  SMI_LOCK		(1 << 4)
#define GEN_PMCON_B		0xa4
#define  SLP_STR_POL_LOCK	(1 << 18)
#define  ACPI_BASE_LOCK		(1 << 17)


#endif
