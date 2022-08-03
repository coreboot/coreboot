/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <fsp/api.h>
#include <soc/ramstage.h>
#include <soc/vr_config.h>
#include <console/console.h>
#include <intelblocks/cpulib.h>

/*
 * VR Configurations for IA and GT domains for ADL-P SKU's.
 * Per doc#627345 ADL_P Partial Intel PlatformDesignStudio Rev 2.0.0, update PD
 *
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * |      SKU       | Setting   | AC LL | DC LL | ICC MAX | TDC Current | TDC Time |
 * |                |           |(mOhms)|(mOhms)|   (A)   |     (A)     |   (msec) |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-P 682(45W) |    IA     |  2.3  |  2.3  |   160   |      57     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    55   |      57     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-P 482(45W) |    IA     |  2.3  |  2.3  |   120   |      47     |  28000   |
 * +       442(45W) +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    55   |      47     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-P 682(28W) |    IA     |  2.3  |  2.3  |   109   |      40     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    55   |      40     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-P 482(28W) |    IA     |  2.3  |  2.3  |    85   |      32     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    55   |      32     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-P 282(15W) |    IA     |  2.8  |  2.8  |    80   |      20     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    40   |      20     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 */

/*
 * VR Configurations for IA and GT domains for ADL-N SKU's.
 * Per doc#646929 ADL N Platform Design Guide -> Power_Map_Rev1p0
 *
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * |      SKU       | Setting   | AC LL | DC LL | ICC MAX | TDC Current | TDC Time |
 * |                |           |(mOhms)|(mOhms)|   (A)   |     (A)     |   (msec) |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-N 081(15W) |    IA     |  4.7  |  4.7  |    53   |      22     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  6.5  |  6.5  |    29   |      22     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-N 081(7W)  |    IA     |  5.0  |  5.0  |    37   |      14     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  6.5  |  6.5  |    29   |      14     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-N 041(6W)  |    IA     |  5.0  |  5.0  |    37   |      12     |  28000   |
 * +  Pentium       +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  6.5  |  6.5  |    29   |      12     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-N 041(6W)  |    IA     |  5.0  |  5.0  |    37   |      12     |  28000   |
 * +  Celeron       +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  6.5  |  6.5  |    26   |      12     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | ADL-N 021(6W)  |    IA     |  5.0  |  5.0  |    27   |      10     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  6.5  |  6.5  |    23   |      10     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 */

/*
 * VR Configurations for IA and GT domains for RPL-P SKU's.
 * Per doc#686872 RPL UPH PDG - 2022, June 7th edition
 *
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * |      SKU       | Setting   | AC LL | DC LL | ICC MAX | TDC Current | TDC Time |
 * |                |           |(mOhms)|(mOhms)|   (A)   |     (A)     |   (msec) |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | RPL-P 682(45W) |    IA     |  2.3  |  2.3  |   160   |      86     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    55   |      86     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | RPL-P 482(28W) |    IA     |  2.3  |  2.3  |   102   |      54     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    55   |      54     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 * | RPL-P 282(15W) |    IA     |  2.8  |  2.8  |    80   |      41     |  28000   |
 * +                +-----------+-------+-------+---------+-------------+----------+
 * |                |    GT     |  3.2  |  3.2  |    40   |      41     |  28000   |
 * +----------------+-----------+-------+-------+---------+-------------+----------+
 */

struct vr_lookup {
	uint16_t mchid;
	uint8_t tdp;
	uint32_t conf[NUM_VR_DOMAINS];
};

static uint32_t load_table(const struct vr_lookup *tbl, const int tbl_entries, const int domain,
					const uint16_t mch_id, uint8_t tdp)
{
	for (size_t i = 0; i < tbl_entries; i++) {
		if (tbl[i].mchid != mch_id || tbl[i].tdp != tdp)
			continue;
		return tbl[i].conf[domain];
	}

	printk(BIOS_ERR, "Unknown MCH (0x%x) in %s\n", mch_id, __func__);
	return 0;
}

/* Per the power map from #613643, update ADL-P 6+8+2 (28W) VR configuration */
static const struct vr_lookup vr_config_ll[] = {
	{ PCI_DID_INTEL_ADL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 45, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_4, 45, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 45, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 28, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, VR_CFG_ALL_DOMAINS_LOADLINE(2.8, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, VR_CFG_ALL_DOMAINS_LOADLINE(2.8, 3.2) },
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, VR_CFG_ALL_DOMAINS_LOADLINE(2.8, 3.2) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 15, VR_CFG_ALL_DOMAINS_LOADLINE(4.7, 6.5) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 7, VR_CFG_ALL_DOMAINS_LOADLINE(5.0, 6.5) },
	{ PCI_DID_INTEL_ADL_N_ID_2, 6, VR_CFG_ALL_DOMAINS_LOADLINE(5.0, 6.5) },
	{ PCI_DID_INTEL_ADL_N_ID_3, 6, VR_CFG_ALL_DOMAINS_LOADLINE(5.0, 6.5) },
	{ PCI_DID_INTEL_ADL_N_ID_4, 6, VR_CFG_ALL_DOMAINS_LOADLINE(5.0, 6.5) },
	{ PCI_DID_INTEL_RPL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_RPL_P_ID_2, 28, VR_CFG_ALL_DOMAINS_LOADLINE(2.3, 3.2) },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, VR_CFG_ALL_DOMAINS_LOADLINE(2.8, 3.2) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  150, VR_CFG_ALL_DOMAINS_LOADLINE(1.1, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  125, VR_CFG_ALL_DOMAINS_LOADLINE(1.1, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  65,  VR_CFG_ALL_DOMAINS_LOADLINE(1.1, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  35,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  125, VR_CFG_ALL_DOMAINS_LOADLINE(1.1, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  65,  VR_CFG_ALL_DOMAINS_LOADLINE(1.1, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  35,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_8,  125, VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 65,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 35,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 60,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 58,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 35,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 46,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 35,  VR_CFG_ALL_DOMAINS_LOADLINE(1.7, 4.0) },
};

static const struct vr_lookup vr_config_icc[] = {
	{ PCI_DID_INTEL_ADL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_ICC(120, 55) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 45, VR_CFG_ALL_DOMAINS_ICC(160, 55) },
	{ PCI_DID_INTEL_ADL_P_ID_4, 45, VR_CFG_ALL_DOMAINS_ICC(160, 55) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 45, VR_CFG_ALL_DOMAINS_ICC(120, 55) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, VR_CFG_ALL_DOMAINS_ICC(109, 55) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, VR_CFG_ALL_DOMAINS_ICC(85, 55) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 28, VR_CFG_ALL_DOMAINS_ICC(85, 55) },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, VR_CFG_ALL_DOMAINS_ICC(80, 40) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, VR_CFG_ALL_DOMAINS_ICC(80, 40) },
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, VR_CFG_ALL_DOMAINS_ICC(80, 40) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 15, VR_CFG_ALL_DOMAINS_ICC(53, 29) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 7, VR_CFG_ALL_DOMAINS_ICC(37, 29) },
	{ PCI_DID_INTEL_ADL_N_ID_2, 6, VR_CFG_ALL_DOMAINS_ICC(37, 29) },
	{ PCI_DID_INTEL_ADL_N_ID_3, 6, VR_CFG_ALL_DOMAINS_ICC(37, 26) },
	{ PCI_DID_INTEL_ADL_N_ID_4, 6, VR_CFG_ALL_DOMAINS_ICC(27, 23) },
	{ PCI_DID_INTEL_RPL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_ICC(160, 55) },
	{ PCI_DID_INTEL_RPL_P_ID_2, 28, VR_CFG_ALL_DOMAINS_ICC(102, 55) },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, VR_CFG_ALL_DOMAINS_ICC(80, 40) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  150, VR_CFG_ALL_DOMAINS_ICC(280, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  125, VR_CFG_ALL_DOMAINS_ICC(280, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  65,  VR_CFG_ALL_DOMAINS_ICC(240, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  35,  VR_CFG_ALL_DOMAINS_ICC(154, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  125, VR_CFG_ALL_DOMAINS_ICC(240, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  65,  VR_CFG_ALL_DOMAINS_ICC(220, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  35,  VR_CFG_ALL_DOMAINS_ICC(145, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_8,  125, VR_CFG_ALL_DOMAINS_ICC(175, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 65,  VR_CFG_ALL_DOMAINS_ICC(151, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 35,  VR_CFG_ALL_DOMAINS_ICC(100, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 60,  VR_CFG_ALL_DOMAINS_ICC(110, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 58,  VR_CFG_ALL_DOMAINS_ICC(110, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 35,  VR_CFG_ALL_DOMAINS_ICC(90, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 46,  VR_CFG_ALL_DOMAINS_ICC(49, 30) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 35,  VR_CFG_ALL_DOMAINS_ICC(37, 30) },
};

static const struct vr_lookup vr_config_tdc_timewindow[] = {
	{ PCI_DID_INTEL_ADL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 45, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_4, 45, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 45, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 28, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 15, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 7, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_N_ID_2, 6, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_N_ID_3, 6, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_N_ID_4, 6, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_RPL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_RPL_P_ID_2, 28, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, VR_CFG_ALL_DOMAINS_TDC(28000, 28000) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  150, VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  125, VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  65,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  35,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  125, VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  65,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  35,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_8,  125, VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 65,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 35,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 60,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 58,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 35,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 46,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 35,  VR_CFG_ALL_DOMAINS_TDC(56000, 56000) },
};

static const struct vr_lookup vr_config_tdc_currentlimit[] = {
	{ PCI_DID_INTEL_ADL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_TDC_CURRENT(47, 47) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 45, VR_CFG_ALL_DOMAINS_TDC_CURRENT(57, 57) },
	{ PCI_DID_INTEL_ADL_P_ID_4, 45, VR_CFG_ALL_DOMAINS_TDC_CURRENT(57, 57) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 45, VR_CFG_ALL_DOMAINS_TDC_CURRENT(47, 47) },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, VR_CFG_ALL_DOMAINS_TDC_CURRENT(40, 40) },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, VR_CFG_ALL_DOMAINS_TDC_CURRENT(32, 32) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 28, VR_CFG_ALL_DOMAINS_TDC_CURRENT(32, 32) },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, VR_CFG_ALL_DOMAINS_TDC_CURRENT(20, 20) },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, VR_CFG_ALL_DOMAINS_TDC_CURRENT(20, 20) },
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, VR_CFG_ALL_DOMAINS_TDC_CURRENT(20, 20) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 15, VR_CFG_ALL_DOMAINS_TDC_CURRENT(22, 22) },
	{ PCI_DID_INTEL_ADL_N_ID_1, 7, VR_CFG_ALL_DOMAINS_TDC_CURRENT(14, 14) },
	{ PCI_DID_INTEL_ADL_N_ID_2, 6, VR_CFG_ALL_DOMAINS_TDC_CURRENT(12, 12) },
	{ PCI_DID_INTEL_ADL_N_ID_3, 6, VR_CFG_ALL_DOMAINS_TDC_CURRENT(12, 12) },
	{ PCI_DID_INTEL_ADL_N_ID_4, 6, VR_CFG_ALL_DOMAINS_TDC_CURRENT(10, 10) },
	{ PCI_DID_INTEL_RPL_P_ID_1, 45, VR_CFG_ALL_DOMAINS_TDC_CURRENT(86, 86) },
	{ PCI_DID_INTEL_RPL_P_ID_2, 28, VR_CFG_ALL_DOMAINS_TDC_CURRENT(54, 54) },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, VR_CFG_ALL_DOMAINS_TDC_CURRENT(41, 41) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  150, VR_CFG_ALL_DOMAINS_TDC_CURRENT(132, 132) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  125, VR_CFG_ALL_DOMAINS_TDC_CURRENT(132, 132) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  65,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(89, 89) },
	{ PCI_DID_INTEL_ADL_S_ID_1,  35,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(56, 56) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  125, VR_CFG_ALL_DOMAINS_TDC_CURRENT(109, 109) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  65,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(77, 77) },
	{ PCI_DID_INTEL_ADL_S_ID_3,  35,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(49, 49) },
	{ PCI_DID_INTEL_ADL_S_ID_8,  125, VR_CFG_ALL_DOMAINS_TDC_CURRENT(96, 96) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 65,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(66, 66) },
	{ PCI_DID_INTEL_ADL_S_ID_10, 35,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(44, 44) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 60,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(56, 56) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 58,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(59, 59) },
	{ PCI_DID_INTEL_ADL_S_ID_11, 35,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(40, 40) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 46,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(39, 39) },
	{ PCI_DID_INTEL_ADL_S_ID_12, 35,  VR_CFG_ALL_DOMAINS_TDC_CURRENT(30, 30) },
};

void fill_vr_domain_config(FSP_S_CONFIG *s_cfg,
		int domain, const struct vr_config *chip_cfg)
{
	const struct vr_config *cfg;

	if (domain < 0 || domain >= NUM_VR_DOMAINS)
		return;

	/* Use device tree override if requested */
	if (chip_cfg->vr_config_enable) {
		cfg = chip_cfg;

		if (cfg->ac_loadline)
			s_cfg->AcLoadline[domain] = cfg->ac_loadline;
		if (cfg->dc_loadline)
			s_cfg->DcLoadline[domain] = cfg->dc_loadline;
		if (cfg->icc_max)
			s_cfg->IccMax[domain] = cfg->icc_max;
		s_cfg->TdcTimeWindow[domain] = cfg->tdc_timewindow;
		s_cfg->TdcCurrentLimit[domain] = cfg->tdc_currentlimit;
	} else {
		uint8_t tdp = get_cpu_tdp();
		struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
		uint16_t mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;

		s_cfg->AcLoadline[domain] = load_table(vr_config_ll, ARRAY_SIZE(vr_config_ll),
							domain, mch_id, tdp);
		s_cfg->DcLoadline[domain] = load_table(vr_config_ll, ARRAY_SIZE(vr_config_ll),
							domain, mch_id, tdp);
		s_cfg->IccMax[domain] = load_table(vr_config_icc, ARRAY_SIZE(vr_config_icc),
							domain, mch_id, tdp);
		s_cfg->TdcTimeWindow[domain] = load_table(vr_config_tdc_timewindow,
							ARRAY_SIZE(vr_config_tdc_timewindow),
							domain, mch_id, tdp);
		s_cfg->TdcCurrentLimit[domain] = load_table(vr_config_tdc_currentlimit,
							ARRAY_SIZE(vr_config_tdc_currentlimit),
							domain, mch_id, tdp);
	}

	/* Check TdcTimeWindow and TdcCurrentLimit,
	   Set TdcEnable and Set VR TDC Input current to root mean square */
	if (s_cfg->TdcTimeWindow[domain] != 0 && s_cfg->TdcCurrentLimit[domain] != 0) {
		s_cfg->TdcEnable[domain] = 1;
		s_cfg->Irms[domain] = 1;
	}
}
