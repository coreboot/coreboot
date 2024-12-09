/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_INTEL_COMMON_BLOCK_POWER_LIMIT_H_
#define _SOC_INTEL_COMMON_BLOCK_POWER_LIMIT_H_

#define MCH_PKG_POWER_LIMIT_LO		0x59a0
#define MCH_PKG_POWER_LIMIT_HI		0x59a4
#define MCH_DDR_POWER_LIMIT_LO		0x58e0
#define MCH_DDR_POWER_LIMIT_HI		0x58e4

#define MSR_VR_CURRENT_CONFIG		0x601
#define MSR_PL3_CONTROL			0x615
#define MSR_PLATFORM_POWER_LIMIT	0x65c

/* Default power limit value in secs */
#define MOBILE_SKU_PL1_TIME_SEC		28

#define MILLIWATTS_TO_WATTS		1000

struct soc_power_limits_config {
	/* PL1 Override value in Watts */
	uint16_t tdp_pl1_override;
	/* PL2 Override value in Watts */
	uint16_t tdp_pl2_override;
	/* SysPL2 Value in Watts */
	uint16_t tdp_psyspl2;
	/* SysPL3 Value in Watts */
	uint16_t tdp_psyspl3;
	/* SysPL3 window size */
	uint32_t tdp_psyspl3_time;
	/* SysPL3 duty cycle */
	uint32_t tdp_psyspl3_dutycycle;
	/* PL4 Value in Watts */
	uint16_t tdp_pl4;
	/* Estimated maximum platform power in Watts */
	uint16_t psys_pmax;
};

/* Configure power limits for turbo mode */
void set_power_limits(u8 power_limit_1_time,
		struct soc_power_limits_config *config);

u8 get_cpu_tdp(void);

struct cpu_tdp_power_limits {
	uint16_t mch_id;
	uint8_t cpu_tdp;
	uint32_t power_limits_index;
	unsigned int pl1_min_power;
	unsigned int pl1_max_power;
	unsigned int pl2_min_power;
	unsigned int pl2_max_power;
	unsigned int pl4_power;
};

/* Modify Power Limit devictree settings during ramstage */
void variant_update_cpu_power_limits(const struct cpu_tdp_power_limits *limits,
		size_t num_entries);

#endif /* _SOC_INTEL_COMMON_BLOCK_POWER_LIMIT_H_ */
