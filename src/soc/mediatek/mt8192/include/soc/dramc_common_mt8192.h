/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8192_DRAMC_COMMON_MT8192_H__
#define __SOC_MEDIATEK_MT8192_DRAMC_COMMON_MT8192_H__

enum {
	FSP_0 = 0,
	FSP_1,
	FSP_MAX,
};

typedef enum {
	DRAM_DFS_SHU0 = 0,
	DRAM_DFS_SHU1,
	DRAM_DFS_SHU2,
	DRAM_DFS_SHU3,
	DRAM_DFS_SHU4,
	DRAM_DFS_SHU5,
	DRAM_DFS_SHU6,
	DRAM_DFS_SHU_MAX
} dram_dfs_shu;

typedef enum {
	ODT_OFF = 0,
	ODT_ON,
	ODT_MAX
} dram_odt_state;

typedef enum {
	DBI_OFF = 0,
	DBI_ON
} dbi_mode;

enum {
	CKE_FIXOFF = 0,
	CKE_FIXON,
	CKE_DYNAMIC
};

enum {
	CA_NUM_LP4 = 6,
	DQ_DATA_WIDTH = 16,
	DQS_BIT_NUMBER = 8,
	DQS_NUMBER = (DQ_DATA_WIDTH / DQS_BIT_NUMBER),
};
#define BYTE_NUM		DQS_NUMBER

/* DONOT change the sequence of pinmux */
typedef enum {
	PINMUX_DSC = 0,
	PINMUX_LPBK,
	PINMUX_EMCP,
	PINMUX_MAX
} dram_pinmux_type;

enum {
	CBT_R0_R1_NORMAL = 0,
	CBT_R0_R1_BYTE,
	CBT_R0_NORMAL_R1_BYTE,
	CBT_R0_BYTE_R1_NORMAL
};

#endif	/* __SOC_MEDIATEK_MT8192_DRAMC_COMMON_MT8192_H__ */
