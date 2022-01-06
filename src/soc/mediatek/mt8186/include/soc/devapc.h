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
	MAS_SEC_0 = 0x0B00,
	AO_APC_CON = 0x0F00,
};

/******************************************************************************
 * STRUCTURE DEFINITION
 ******************************************************************************/
/* Common */
enum trans_type {
	NON_SECURE_TRANS = 0,
	SECURE_TRANS,
};

enum devapc_perm_type {
	NO_PROTECTION = 0,
	SEC_RW_ONLY,
	SEC_RW_NS_R,
	FORBIDDEN,
	PERM_NUM,
};

enum domain_id {
	DOMAIN_0 = 0,
	DOMAIN_1,
	DOMAIN_2,
	DOMAIN_3,
	DOMAIN_4,
	DOMAIN_5,
	DOMAIN_6,
	DOMAIN_7,
	DOMAIN_8,
	DOMAIN_9,
	DOMAIN_10,
	DOMAIN_11,
	DOMAIN_12,
	DOMAIN_13,
	DOMAIN_14,
	DOMAIN_15,
};

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
};

enum devapc_cfg_index {
	DEVAPC_DEBUGSYS_INDEX = 94,
};

/* PERM_ATTR MACRO */
#define DAPC_INFRA_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_8(__VA_ARGS__) } }
#define DAPC_MM_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_4(__VA_ARGS__) } }

/******************************************************************************
 * Variable DEFINITION
 ******************************************************************************/
#define MOD_NO_IN_1_DEVAPC		16

/******************************************************************************
 * Bit Field DEFINITION
 ******************************************************************************/
DEFINE_BIT(SCP_SSPM_SEC, 21)

#endif
