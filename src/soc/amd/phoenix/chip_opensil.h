/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

#define DDI_DESCRIPTOR_COUNT	5

/* DDI display connector type */
enum ddi_connector_type {
	DDI_DP = 0,		// DP
	DDI_EDP,		// eDP
	DDI_SINGLE_LINK_DVI,	// Single Link DVI-D
	DDI_DUAL_LINK_DVI,	// Dual Link DVI-D
	DDI_HDMI,		// HDMI
	DDI_DP_TO_VGA,		// DP-to-VGA
	DDI_DP_TO_LVDS,		// DP-to-LVDS
	DDI_NUTMEG_DP_TO_VGA,	// Hudson-2 NutMeg DP-to-VGA
	DDI_SINGLE_LINK_DVI_I,	// Single Link DVI-I
	DDI_DP_W_TYPEC,		// DP with USB type C
	DDI_DP_WO_TYPEC,	// DP without USB type C
	DDI_EDP_TO_LVDS,	// eDP-to-LVDS translator chip without AMD SW init
	DDI_EDP_TO_LVDS_SW,	// eDP-to-LVDS translator which requires AMD SW init
	DDI_AUTO_DETECT,	// VBIOS auto detect connector type
	DDI_UNUSED_TYPE,	// UnusedType
	DDI_MAX_CONNECTOR_TYPE	// Not valid value, used to verify input
};

/* DDI Descriptor: used for configuring display outputs */
struct ddi_descriptor {
	uint8_t		connector_type;	// see ddi_connector_type
	uint8_t		aux_index;
	uint8_t		hdp_index;
};
