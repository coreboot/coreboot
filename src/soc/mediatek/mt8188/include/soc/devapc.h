/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8188_DEVAPC_H
#define SOC_MEDIATEK_MT8188_DEVAPC_H

#include <device/mmio.h>
#include <soc/addressmap.h>

void dapc_init(void);

enum devapc_ao_offset {
	SYS0_D0_APC_0 = 0x00000,
	SYS1_D0_APC_0 = 0x01000,
	SYS2_D0_APC_0 = 0x02000,
	MAS_DOM_0 = 0x00900,
	MAS_SEC_0 = 0x00A00,
	AO_APC_CON = 0x00F00,
};

/******************************************************************************
 * STRUCTURE DEFINITION
 ******************************************************************************/
struct apc_infra_peri_dom_16 {
	unsigned char d_permission[16];
};

struct apc_infra_peri_dom_8 {
	unsigned char d_permission[8];
};

struct apc_infra_peri_dom_4 {
	unsigned char d_permission[4];
};

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
 /* TODO */

#endif /* SOC_MEDIATEK_MT8188_DEVAPC_H */
