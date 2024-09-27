/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_PMC_H_
#define _SOC_SNOWRIDGE_PMC_H_

#define GEN_PMCON_A            0x1020
#define GBL_RST_STS            BIT(24)
#define MS4V                   BIT(18)
#define SUS_PWR_FLR            BIT(16)
#define PWR_FLR                BIT(14)
#define PER_SMI_SEL_MASK       (3 << 1)
#define SMI_RATE_64S           (0 << 1)
#define SMI_RATE_32S           BIT(1)
#define SMI_RATE_16S           (2 << 1)
#define SMI_RATE_8S            (3 << 1)
#define SLEEP_AFTER_POWER_FAIL BIT(0)
#define GEN_PMCON_B            0x1024
#define SLP_STR_POL_LOCK       BIT(18)
#define ACPI_BASE_LOCK         BIT(17)
#define SMI_LOCK               BIT(4)
#define ETR                    0x1048
#define CF9_LOCK               BIT(31)
#define CF9_GLB_RST            BIT(20)
#define PRSTS                  0x1810
#define PMSYNC_TPR_CFG         0x18C4
#define PCH2CPU_TPR_CFG_LOCK   BIT(31)
#define GPIO_GPE_CFG           0x1920
#define GPE0_DWX_MASK          0xf
#define GPE0_DW_SHIFT(x)       (4 * (x))
#define ACTL                   0x1BD8
#define SCI_IRQ_SEL            (7 << 0)
#define SCI_IRQ_ADJUST         0
#define SCIS_IRQ9              0
#define SCIS_IRQ10             1
#define SCIS_IRQ11             2
#define SCIS_IRQ20             4
#define SCIS_IRQ21             5
#define SCIS_IRQ22             6
#define SCIS_IRQ23             7

#endif // _SOC_SNOWRIDGE_PMC_H_
