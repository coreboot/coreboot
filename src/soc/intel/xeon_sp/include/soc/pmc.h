/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_PMC_H_
#define _SOC_PMC_H_

/* PCI Configuration Space (D31:F2): PMC */
#define PMC_ACPI_CNT            0x44

#define  SCI_IRQ_SEL            (7 << 0)
#define  SCIS_IRQ9              0
#define  SCIS_IRQ10             1
#define  SCIS_IRQ11             2
#define  SCIS_IRQ20             4
#define  SCIS_IRQ21             5
#define  SCIS_IRQ22             6
#define  SCIS_IRQ23             7

#define SCI_IRQ_ADJUST          0

#endif
