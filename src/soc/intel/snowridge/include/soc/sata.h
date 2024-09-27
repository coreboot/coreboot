/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_SATA_H_
#define _SOC_SNOWRIDGE_SATA_H_

#define SATA_ABAR     0x24
#define SATA_MAP      0x90
#define SATA_MAP_AHCI (0 << 6)
#define SATA_MAP_RAID (1 << 6)

#define SATA_GHC    0x04
#define SATA_GHC_AE BIT(31)

#endif // _SOC_SNOWRIDGE_SATA_H_
