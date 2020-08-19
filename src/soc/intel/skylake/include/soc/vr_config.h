/* SPDX-License-Identifier: GPL-2.0-only */

/* VR Settings for each domain */

#ifndef _SOC_VR_CONFIG_H_
#define _SOC_VR_CONFIG_H_

#include <fsp/api.h>

struct vr_config {

	/*
	 * The below settings will take effect when this is set to 1
	 * for that domain.
	 */
	int vr_config_enable;

	/* Power State X current cutoff in 1/4 Amp increments
	 * Range is 0-128A
	 */
	int psi1threshold;
	int psi2threshold;
	int psi3threshold;

	/* Enable power state 3/4 for different domains */
	int psi3enable;
	int psi4enable;

	/*
	 * Imon slope correction. Specified in 1/100 increment
	 * values. Range is 0-200. 125 = 1.25
	 */
	int imon_slope;

	/*
	 * Imon offset correction. Units 1/4, Range 0-255.
	 * Value of 100 = 100/4 = 25 offset.
	 */
	int imon_offset;

	/* VR Icc Max limit. 0-255A in 1/4 A units. 400 = 100A */
	int icc_max;

	/* VR Voltage Limit. Range is 0-7999mV */
	int voltage_limit;

	/* AC and DC Loadline in 1/100 mOhms. Range is 0-6249 */
	int ac_loadline;
	int dc_loadline;
};

#define VR_CFG_AMP(i) ((i) * 4)
#define VR_CFG_MOHMS(i) (uint16_t)((i) * 100)

/* VrConfig Settings for 4 domains
 * 0 = System Agent, 1 = IA Core,
 * 2 = GT unsliced,  3 = GT sliced
 */
enum vr_domain {
	VR_SYSTEM_AGENT,
	VR_IA_CORE,
	VR_GT_UNSLICED,
	VR_GT_SLICED,
	NUM_VR_DOMAINS
};

#define VR_CFG_ALL_DOMAINS_ICC(sa, ia, gt_unsl, gt_sl)	\
	{							\
		[VR_SYSTEM_AGENT] = VR_CFG_AMP(sa),		\
		[VR_IA_CORE]      = VR_CFG_AMP(ia),		\
		[VR_GT_UNSLICED]  = VR_CFG_AMP(gt_unsl),	\
		[VR_GT_SLICED]    = VR_CFG_AMP(gt_sl),		\
	}

#define VR_CFG_ALL_DOMAINS_LOADLINE(sa, ia, gt_unsl, gt_sl)	\
	{							\
		[VR_SYSTEM_AGENT] = VR_CFG_MOHMS(sa),		\
		[VR_IA_CORE]      = VR_CFG_MOHMS(ia),		\
		[VR_GT_UNSLICED]  = VR_CFG_MOHMS(gt_unsl),	\
		[VR_GT_SLICED]    = VR_CFG_MOHMS(gt_sl),	\
	}

void fill_vr_domain_config(void *params,
			int domain, const struct vr_config *cfg);
#endif
