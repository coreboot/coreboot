/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor SA Datasheet
 * Document number: 619503
 * Chapter number: 3
 */

#ifndef SOC_ALDERLAKE_SYSTEMAGENT_H
#define SOC_ALDERLAKE_SYSTEMAGENT_H

#include <intelblocks/systemagent.h>

/* Device 0:0.0 PCI configuration space */

#define EPBAR		0x40
#define DMIBAR		0x68
#define CAPID0_A	0xe4
#define  VTD_DISABLE	(1 << 23)

/* MCHBAR offsets */
#define GFXVTBAR		0x5400
#define EDRAMBAR		0x5408
#define VTVC0BAR		0x5410
#define REGBAR			0x5420
#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4
#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4
#define BIOS_RESET_CPL		0x5da8
#define IMRBASE			0x6a40
#define IMRLIMIT		0x6a48
#define IPUVTBAR		0x7880
#define TBTxBAR(x)		(0x7888 + (x) * 8)

#define MAX_TBT_PCIE_PORT	4

#define VTBAR_ENABLED		0x01
#define VTBAR_MASK		0x7ffffff000ull

static const struct sa_mmio_descriptor soc_vtd_resources[] = {
	{ GFXVTBAR, GFXVT_BASE_ADDRESS, GFXVT_BASE_SIZE, "GFXVTBAR" },
	{ IPUVTBAR, IPUVT_BASE_ADDRESS, IPUVT_BASE_SIZE, "IPUVTBAR" },
	{ TBTxBAR(0), TBT0_BASE_ADDRESS, TBT0_BASE_SIZE, "TBT0BAR" },
	{ TBTxBAR(1), TBT1_BASE_ADDRESS, TBT1_BASE_SIZE, "TBT1BAR" },
	{ TBTxBAR(2), TBT2_BASE_ADDRESS, TBT2_BASE_SIZE, "TBT2BAR" },
	{ TBTxBAR(3), TBT3_BASE_ADDRESS, TBT3_BASE_SIZE, "TBT3BAR" },
	{ VTVC0BAR, VTVC0_BASE_ADDRESS, VTVC0_BASE_SIZE, "VTVC0BAR" },
};

#define V_P2SB_CFG_IBDF_BUS	0
#define V_P2SB_CFG_IBDF_DEV	30
#define V_P2SB_CFG_IBDF_FUNC	7
#define V_P2SB_CFG_HBDF_BUS	0
#define V_P2SB_CFG_HBDF_DEV	30
#define V_P2SB_CFG_HBDF_FUNC	6

#define CRAB_ABORT_BASE_ADDR	0xFEB00000
#define CRAB_ABORT_SIZE		(512 * KiB)
#define TPM_BASE_ADDRESS	0xFED40000
#define TPM_SIZE		(64 * KiB)

#define LT_SECURITY_BASE_ADDR	0xFED50000
#define LT_SECURITY_SIZE	(128 * KiB)
#define APIC_SIZE		(1 * MiB)

#define MASK_PCIEXBAR_LENGTH	0x0000000E // bits [3:1]
#define PCIEXBAR_LENGTH_LSB	1 // used to shift right

#define DSM_BASE_ADDR_REG	0xB0
#define  MASK_DSM_LENGTH	0xFF00 // [15:8]
#define  MASK_DSM_LENGTH_LSB	8 // used to shift right
#define  MASK_GSM_LENGTH	0xC0 // [7:6]
#define  MASK_GSM_LENGTH_LSB	6 // used to shift right
#define DPR_REG			0x5C
#define  MASK_DPR_LENGTH	0xFF0 // [11:4]
#define  MASK_DPR_LENGTH_LSB	4 // used to shift right

uint64_t get_mmcfg_size(struct device *dev);
uint64_t get_dsm_size(struct device *dev);
uint64_t get_gsm_size(struct device *dev);
uint64_t get_dpr_size(struct device *dev);

#endif
