/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8195_DEVAPC_H
#define SOC_MEDIATEK_MT8195_DEVAPC_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/devapc_common.h>

enum devapc_ao_offset {
	SYS0_D0_APC_0 = 0x0,
	SYS1_D0_APC_0 = 0x1000,
	SYS2_D0_APC_0 = 0x2000,
	DOM_REMAP_0_0 = 0x800,
	DOM_REMAP_1_0 = 0x810,
	DOM_REMAP_1_1 = 0x814,
	DOM_REMAP_2_0 = 0x820,
	MAS_DOM_0 = 0x0900,
	MAS_DOM_4 = 0x0910,
	MAS_SEC_0 = 0x0A00,
	AO_APC_CON = 0x0F00,
};

enum scp_offset {
	SCP_DOM = 0xA5080,
	ADSP_DOM = 0xA5088,
	ONETIME_LOCK = 0xA5104,
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
	DEVAPC_DEBUGSYS_INDEX = 17,
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

/******************************************************************************
 * Bit Field DEFINITION
 ******************************************************************************/
/* INFRA */
DEFINE_BIT(CPU_EB_SEC, 1)

DEFINE_BITFIELD(CPU_EB_DOM, 11, 8)	/* 1 */
DEFINE_BITFIELD(SCP_SSPM_DOM, 19, 16)	/* 2 */

/* PERI */
DEFINE_BITFIELD(SPM_DOM, 11, 8)		/* 1 */

/* PERI_PAR */
DEFINE_BIT(SSUSB_SEC, 21)
DEFINE_BIT(SSUSB2_SEC, 0)
DEFINE_BIT(SSUSB_P1_0_SEC, 1)
DEFINE_BIT(SSUSB_P1_1_SEC, 2)
DEFINE_BIT(SSUSB_P2_SEC, 3)
DEFINE_BIT(SSUSB_P3_SEC, 4)

DEFINE_BITFIELD(PCIE0_DOM, 11, 8)	/* 17 */
DEFINE_BITFIELD(PCIE1_DOM, 19, 16)	/* 18 */

#endif /* SOC_MEDIATEK_MT8195_DEVAPC_H */
