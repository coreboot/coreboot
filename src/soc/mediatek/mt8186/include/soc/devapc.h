/* SPDX-License-Identifier: GPL-2.0-only */

/* This file is created based on MT8169_DEVICE_APC_REG_DEVAPC_external.docx */

#ifndef SOC_MEDIATEK_MT8186_DEVAPC_H
#define SOC_MEDIATEK_MT8186_DEVAPC_H

#include <device/mmio.h>
#include <soc/addressmap.h>

void dapc_init(void);

enum devapc_ao_offset {
	SYS0_D0_APC_0 = 0x0,
	DOM_REMAP_0_0 = 0xD00,
	DOM_REMAP_1_0 = 0xD04,
	MAS_DOM_0 = 0x0A00,
	MAS_DOM_1 = 0x0A04,
	MAS_DOM_3 = 0x0A0C,
	MAS_SEC_0 = 0x0B00,
	AO_APC_CON = 0x0F00,
	AUD_DOM_0 = 0x0900,
	AUD_SEC_0 = 0x0A00,
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
	DOM_NUM_INFRA_AO_SYS0 = 8,
	DOM_NUM_MM_AO_SYS0 = 4,
	DOM_NUM_AUD_AO_SYS0 = 16,
};

enum devapc_cfg_index {
	DEVAPC_DEBUGSYS_INDEX = 94,
};

/* PERM_ATTR MACRO */
#define DAPC_INFRA_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_8(__VA_ARGS__) } }
#define DAPC_MM_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_4(__VA_ARGS__) } }
#define DAPC_AUD_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }

/******************************************************************************
 * Variable DEFINITION
 ******************************************************************************/
#define MOD_NO_IN_1_DEVAPC		16
#define DOMAIN_OFT			0x100
#define IDX_OFT				0x4

/******************************************************************************
 * Bit Field DEFINITION
 ******************************************************************************/
DEFINE_BIT(SCP_SSPM_SEC, 21)
DEFINE_BITFIELD(SPM_DOM, 11, 8)
DEFINE_BITFIELD(SCP_DOM, 3, 0)
DEFINE_BITFIELD(ADSP_DOM, 3, 0)

/* Domain Remap */
DEFINE_BITFIELD(TWO_BIT_DOM_REMAP_0, 1, 0)
DEFINE_BITFIELD(TWO_BIT_DOM_REMAP_1, 3, 2)
DEFINE_BITFIELD(TWO_BIT_DOM_REMAP_2, 5, 4)
DEFINE_BITFIELD(TWO_BIT_DOM_REMAP_3, 7, 6)
DEFINE_BITFIELD(TWO_BIT_DOM_REMAP_4, 9, 8)
DEFINE_BITFIELD(TWO_BIT_DOM_REMAP_5, 11, 10)

#endif
