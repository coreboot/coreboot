/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_SYSTEMAGENT_H_
#define _SOC_PANTHERLAKE_SYSTEMAGENT_H_

#include <intelblocks/systemagent.h>

/* Device 0:0.0 PCI configuration space */

#define SAFBAR		0x68
#define EPBAR		0x40
#define CAPID0_A	0xe4
#define  VTD_DISABLE	(1 << 23)

/* MCHBAR offsets */
#define VTDBAR			0x5410
#define GFXVTBAR		VTDBAR
#define  GFXVT_ENABLED		BIT(0)
#define  NONGFXVT_ENABLED	BIT(1)
#define  IOCVT_ENABLED		BIT(2)
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
	{ VTDBAR, VTD_BASE_ADDRESS, VTD_BASE_SIZE, "VTDBAR" },
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
#define LT_SECURITY_BASE_ADDR	0xFED20000
#define LT_SECURITY_SIZE	(384 * KiB)
#define APIC_SIZE		(1 * MiB)

uint64_t get_mmcfg_size(const struct device *dev);
uint64_t get_dsm_size(const struct device *dev);
uint64_t get_gsm_size(const struct device *dev);
uint64_t get_dpr_size(const struct device *dev);

#endif /* _SOC_PANTHERLAKE_SYSTEMAGENT_H_ */
