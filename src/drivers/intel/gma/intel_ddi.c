/* SPDX-License-Identifier: MIT */

#include <types.h>
#include <console/console.h>
#include <drivers/intel/gma/i915.h>

/* HDMI/DVI modes ignore everything but the last 2 items. So we share
 * them for both DP and FDI transports, allowing those ports to
 * automatically adapt to HDMI connections as well.
 */
static  u32 hsw_ddi_translations_dp[] = {
	0x00FFFFFF, 0x0006000E,		/* DP parameters */
	0x00D75FFF, 0x0005000A,
	0x00C30FFF, 0x00040006,
	0x80AAAFFF, 0x000B0000,
	0x00FFFFFF, 0x0005000A,
	0x00D75FFF, 0x000C0004,
	0x80C30FFF, 0x000B0000,
	0x00FFFFFF, 0x00040006,
	0x80D75FFF, 0x000B0000,
	0x00FFFFFF, 0x00040006		/* HDMI parameters */
};

static  u32 hsw_ddi_translations_fdi[] = {
	0x00FFFFFF, 0x0007000E,		/* FDI parameters */
	0x00D75FFF, 0x000F000A,
	0x00C30FFF, 0x00060006,
	0x00AAAFFF, 0x001E0000,
	0x00FFFFFF, 0x000F000A,
	0x00D75FFF, 0x00160004,
	0x00C30FFF, 0x001E0000,
	0x00FFFFFF, 0x00060006,
	0x00D75FFF, 0x001E0000,
	0x00FFFFFF, 0x00040006		/* HDMI parameters */
};

/* On Haswell, DDI port buffers must be programmed with correct values
 * in advance. The buffer values are different for FDI and DP modes,
 * but the HDMI/DVI fields are shared among those. So we program the DDI
 * in either FDI or DP modes only, as HDMI connections will work with both
 * of those.
 */
static void intel_prepare_ddi_buffers(int port, int use_fdi_mode)
{
	u32 reg;
	int i;
	u32 *ddi_translations = ((use_fdi_mode) ?
				 hsw_ddi_translations_fdi :
				 hsw_ddi_translations_dp);

	printk(BIOS_SPEW, "Initializing DDI buffers for port %d in %s mode\n",
	       port,
	       use_fdi_mode ? "FDI" : "DP");

	for (i=0,reg=DDI_BUF_TRANS(port);i < ARRAY_SIZE(hsw_ddi_translations_fdi);i++) {
		gtt_write(reg,ddi_translations[i]);
		reg += 4;
	}
}

void intel_prepare_ddi(void)
{
	int port;
	u32 use_fdi = 1;

	for (port = PORT_A; port < PORT_E; port++)
		intel_prepare_ddi_buffers(port, !use_fdi);

	intel_prepare_ddi_buffers(PORT_E, use_fdi);
}
