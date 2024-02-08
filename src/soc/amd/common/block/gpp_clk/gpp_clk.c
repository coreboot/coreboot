/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/pci_clk_req.h>
#include <types.h>

/* configure the general purpose PCIe clock outputs according to the devicetree settings */
void gpp_clk_setup_common(enum gpp_clk_req *gpp_clk_config, size_t gpp_clk_config_num)
{
	/* look-up table to be able to iterate over the PCIe clock output settings */
	const uint8_t gpp_clk_shift_lut[GPP_CLK_OUTPUT_COUNT] = {
		GPP_CLK0_REQ_SHIFT,
		GPP_CLK1_REQ_SHIFT,
		GPP_CLK2_REQ_SHIFT,
		GPP_CLK3_REQ_SHIFT,
		GPP_CLK4_REQ_SHIFT,
		GPP_CLK5_REQ_SHIFT,
		GPP_CLK6_REQ_SHIFT,
	};

	uint32_t gpp_clk_ctl = misc_read32(GPP_CLK_CNTRL);

	pcie_gpp_dxio_update_clk_req_config(gpp_clk_config, gpp_clk_config_num);
	for (int i = 0; i < GPP_CLK_OUTPUT_COUNT; i++) {
		gpp_clk_ctl &= ~GPP_CLK_REQ_MASK(gpp_clk_shift_lut[i]);
		/*
		 * The remapping of values is done so that the default of the enum used for the
		 * devicetree settings is the clock being enabled, so that a missing devicetree
		 * configuration for this will result in an always active clock and not an
		 * inactive PCIe clock output. Only the configuration for the clock outputs
		 * available on the package is provided via the devicetree; the rest is
		 * switched off unconditionally.
		 */
		switch (i < gpp_clk_config_num ? gpp_clk_config[i] : GPP_CLK_OFF) {
		case GPP_CLK_REQ:
			gpp_clk_ctl |= GPP_CLK_REQ_EXT(gpp_clk_shift_lut[i]);
			break;
		case GPP_CLK_OFF:
			gpp_clk_ctl |= GPP_CLK_REQ_OFF(gpp_clk_shift_lut[i]);
			break;
		case GPP_CLK_ON:
		default:
			gpp_clk_ctl |= GPP_CLK_REQ_ON(gpp_clk_shift_lut[i]);
		}
	}

	misc_write32(GPP_CLK_CNTRL, gpp_clk_ctl);
}
