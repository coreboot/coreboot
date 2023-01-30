/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SDHCI_MSM_H__
#define __SDHCI_MSM_H__

#include "soc/sdhci.h"

/* SDHC specific defines */
#define SDCC_HC_VENDOR_SPECIFIC_FUNC3	0x250
#define VENDOR_SPEC_FUN3_POR_VAL	0x02226040
#define SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0	0x21C

struct sd_mmc_ctrlr *new_sdhci_msm_host(void *ioaddr);

#endif /* __SDHCI_MSM_H__ */
