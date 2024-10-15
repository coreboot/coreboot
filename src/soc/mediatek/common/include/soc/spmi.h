/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_SPMI_H__
#define __SOC_MEDIATEK_SPMI_H__

#include <types.h>

enum spmi_master {
	SPMI_MASTER_0,
	SPMI_MASTER_1,
	SPMI_MASTER_2,
	SPMI_MASTER_3,
};

enum spmi_slave {
	SPMI_SLAVE_0,
	SPMI_SLAVE_1,
	SPMI_SLAVE_2,
	SPMI_SLAVE_3,
	SPMI_SLAVE_4,
	SPMI_SLAVE_5,
	SPMI_SLAVE_6,
	SPMI_SLAVE_7,
	SPMI_SLAVE_8,
	SPMI_SLAVE_9,
	SPMI_SLAVE_10,
	SPMI_SLAVE_11,
	SPMI_SLAVE_12,
	SPMI_SLAVE_13,
	SPMI_SLAVE_14,
	SPMI_SLAVE_15,
	SPMI_SLAVE_MAX,
};

enum slv_type {
	BUCK_CPU,
	BUCK_GPU,
	BUCK_MD,
	BUCK_RF,
	MAIN_PMIC,
	BUCK_VPU,
	SUB_PMIC,
	CLOCK_PMIC,
	SECOND_PMIC,
	BUCK_APU,
	SLV_TYPE_MAX,
};

enum slv_type_id {
	BUCK_RF_ID = 1,
	BUCK_MD_ID = 3,
	MAIN_PMIC_ID = 5,
	BUCK_CPU_ID = 6,
	BUCK_GPU_ID = 7,
	BUCK_VPU_ID = 8,
	BUCK_APU_ID = 9,
	SUB_PMIC_ID = 10,
	CLOCK_PMIC_ID = 11,
	SECOND_PMIC_ID = 12,
	SLV_TYPE_ID_MAX,
};

struct spmi_device {
	enum spmi_slave slvid;
	enum spmi_master mstid;
	enum slv_type type;
	enum slv_type_id type_id;
	u16 hwcid_addr;
	u8 hwcid_val;
	u8 hwcid_mask;
};

#endif /* __SOC_MEDIATEK_SPMI_H__ */
