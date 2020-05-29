/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SPMI_H__
#define __SPMI_H__

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
	SLV_TYPE_MAX,
};

enum slv_type_id {
	BUCK_CPU_ID,
	BUCK_GPU_ID,
	SLV_TYPE_ID_MAX,
};

struct spmi_device {
	u32 slvid;
	enum slv_type type;
	enum slv_type_id type_id;
};
#endif /*__SPMI_H__*/
