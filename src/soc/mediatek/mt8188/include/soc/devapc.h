/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8188_DEVAPC_H
#define SOC_MEDIATEK_MT8188_DEVAPC_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/devapc_common.h>

enum devapc_ao_offset {
	SYS0_D0_APC_0 = 0x00000,
	SYS1_D0_APC_0 = 0x01000,
	SYS2_D0_APC_0 = 0x02000,
	DOM_REMAP_0_0 = 0x00800,
	DOM_REMAP_0_1 = 0x00804,
	DOM_REMAP_2_0 = 0x00820,
	MAS_DOM_0 = 0x00900,
	MAS_DOM_1 = 0x00904,
	MAS_SEC_0 = 0x00A00,
	AO_APC_CON = 0x00F00,
};

enum scp_offset {
	SCP_DOM0 = 0xA0900,
	SCP_DOM1 = 0xA0904,
	SCP_DOM2 = 0xA0908,
	ONETIME_LOCK = 0xA5104,
};

enum sub_infracfg_ao_mem_offset {
	INFRA_AO_SEC_MFG_HYP = 0xFB4,
	INFRA_AO_SEC_MFG_HYP2 = 0x68,
};

/******************************************************************************
 * STRUCTURE DEFINITION
 ******************************************************************************/
enum devapc_sys_dom_num {
	DOM_NUM_INFRA_AO_SYS0 = 16,
	DOM_NUM_INFRA_AO_SYS1 = 4,
	DOM_NUM_INFRA_AO_SYS2 = 4,
	DOM_NUM_PERI_AO_SYS0 = 16,
	DOM_NUM_PERI_AO_SYS1 = 8,
	DOM_NUM_PERI2_AO_SYS0 = 16,
	DOM_NUM_PERI_PAR_AO_SYS0 = 16,
};

enum devapc_cfg_index {
	DEVAPC_DEBUGSYS_INDEX = 14,
};

enum mfg_dom {
	MFG_S_D6 = 0x16,
	MFG_NS_D6 = 0x6,
};

/* PERM_ATTR MACRO */
#define DAPC_INFRA_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }
#define DAPC_INFRA_AO_SYS1_ATTR(...)	{ { DAPC_PERM_ATTR_4(__VA_ARGS__) } }
#define DAPC_INFRA_AO_SYS2_ATTR(...)	{ { DAPC_PERM_ATTR_4(__VA_ARGS__) } }
#define DAPC_PERI_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }
#define DAPC_PERI_AO_SYS1_ATTR(...)	{ { DAPC_PERM_ATTR_8(__VA_ARGS__) } }
#define DAPC_PERI2_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }
#define DAPC_PERI_PAR_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }

/******************************************************************************
 * Variable DEFINITION
 ******************************************************************************/
#define MOD_NO_IN_1_DEVAPC		16
#define DOMAIN_OFT			0x40
#define IDX_OFT				0x4
#define MFG_HPY_OFT			0

/******************************************************************************
 * Bit Field DEFINITION
 ******************************************************************************/
/* INFRA */
DEFINE_BIT(CPU_EB_SEC, 1)
DEFINE_BIT(SCP_SSPM_SEC, 2)

DEFINE_BITFIELD(CPU_EB_DOM, 11, 8)	/* 1 */
DEFINE_BITFIELD(SCP_SSPM_DOM, 19, 16)	/* 2 */

/* PERI */
DEFINE_BITFIELD(SPM_DOM, 3, 0)		/* 0 */

/* PERI_PAR */
DEFINE_BITFIELD(PCIE0_DOM, 27, 24)	/* 19 */

/* FMEM */
DEFINE_BITFIELD(MFG_M0_DOM, 19, 16)	/* 6 */

/* INFRACFG_AO SEC MFG HYP */
DEFINE_BITFIELD(OSID0, 4, 0)
DEFINE_BITFIELD(OSID1, 9, 5)
DEFINE_BITFIELD(OSID2, 14, 10)
DEFINE_BITFIELD(OSID3, 19, 15)
DEFINE_BITFIELD(FM_EN, 24, 20)
DEFINE_BITFIELD(SEC_EN, 29, 25)
DEFINE_BIT(REMAP_EN, 31)

#endif /* SOC_MEDIATEK_MT8188_DEVAPC_H */
