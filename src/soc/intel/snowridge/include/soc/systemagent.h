/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_SYSTEMAGENT_H_
#define _SOC_SNOWRIDGE_SYSTEMAGENT_H_

#define PCIE_MMCFG_BASE  0x90
#define PCIE_MMCFG_LIMIT 0x98
#define TSEG             0xA8
#define TSEG_LIMIT       0xAC
#define TOCM             0xC0
#define TOUUD            0xC8
#define TOLUD            0xD0
#define MMIOL            0xD8

#define VTBAR                           0x180
#define VTD_CHIPSET_BASE_ADDRESS_ENABLE BIT(0)

#define DPR 0x290

#define VTD_ECAP   0x10
#define DEVICE_TLB BIT(2)

#endif // _SOC_SNOWRIDGE_SYSTEMAGENT_H_
