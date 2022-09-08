/* SPDX-License-Identifier: GPL-2.0-only */

/* VR Settings for each domain */

#ifndef _SOC_VR_CONFIG_H_
#define _SOC_VR_CONFIG_H_

#include <fsp/api.h>

struct vr_config {

	/* The below settings will take effect when this is set to 1 for that domain. */
	bool vr_config_enable;

	/* AC and DC Loadline.
	   They are in 1/100 mOhms (ie. 1250 = 12.50 mOhms) and range is 0-6249. */
	uint16_t ac_loadline;
	uint16_t dc_loadline;

	/* VR Icc Max limit.
	   Range is from 0-255A in 1/4 A units (400 = 100A). */
	uint16_t icc_max;

	/* Thermal Design Current time window.
	   Defined in milli seconds and range 1ms to 448s. */
	uint32_t tdc_timewindow;

	/* Thermal Design Current current limit.
	   Defined in 1/8A units and range is 0-4095. 1000 = 125A. */
	uint16_t tdc_currentlimit;

	/* Power State 1/2/3 Threshold Current.
	   Defined in 1/4A units and range is 0-128A */
	uint16_t psi1threshold;
	uint16_t psi2threshold;
	uint16_t psi3threshold;
};

#define VR_CFG_AMP(i) (uint16_t)((i) * 4)
#define VR_CFG_MOHMS(i) (uint16_t)((i) * 100)
#define VR_CFG_TDC_AMP(i) (uint16_t)((i) * 8)

/* VrConfig Settings for 4 domains
 * 0 = IA core, 1 = GT
 */
enum vr_domain {
	VR_DOMAIN_IA,
	VR_DOMAIN_GT,
	NUM_VR_DOMAINS
};

#define VR_CFG_ALL_DOMAINS_LOADLINE(ia, gt)		\
	{						\
		[VR_DOMAIN_IA] = VR_CFG_MOHMS(ia),	\
		[VR_DOMAIN_GT] = VR_CFG_MOHMS(gt),	\
	}

#define VR_CFG_ALL_DOMAINS_ICC(ia, gt)			\
	{						\
		[VR_DOMAIN_IA] = VR_CFG_AMP(ia),	\
		[VR_DOMAIN_GT] = VR_CFG_AMP(gt),	\
	}

#define VR_CFG_ALL_DOMAINS_TDC(ia, gt)	\
	{				\
		[VR_DOMAIN_IA] = ia,	\
		[VR_DOMAIN_GT] = gt,	\
	}

#define VR_CFG_ALL_DOMAINS_TDC_CURRENT(ia, gt)		\
	{						\
		[VR_DOMAIN_IA] = VR_CFG_TDC_AMP(ia),	\
		[VR_DOMAIN_GT] = VR_CFG_TDC_AMP(gt),	\
	}

void fill_vr_domain_config(FSP_S_CONFIG *s_cfg, int domain, const struct vr_config *cfg);
#endif
