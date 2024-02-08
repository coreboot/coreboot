/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_PCI_GPP_H
#define AMD_BLOCK_PCI_GPP_H

#include <types.h>

/* FCH MISC Registers 0xfed80e00 */
#define GPP_CLK_CNTRL			0x00
#define GPP_CLK0_REQ_SHIFT		0
#define GPP_CLK1_REQ_SHIFT		2
#define GPP_CLK4_REQ_SHIFT		4
#define GPP_CLK2_REQ_SHIFT		6
#define GPP_CLK3_REQ_SHIFT		8
#define GPP_CLK5_REQ_SHIFT		10
#define GPP_CLK6_REQ_SHIFT		12
#define GPP_CLK_OUTPUT_COUNT		7
#define GPP_CLK_REQ_MASK(clk_shift)	(0x3 << (clk_shift))
#define GPP_CLK_REQ_ON(clk_shift)	(0x3 << (clk_shift))
#define GPP_CLK_REQ_EXT(clk_shift)	(0x1 << (clk_shift))
#define GPP_CLK_REQ_OFF(clk_shift)	(0x0 << (clk_shift))

enum gpp_clk_req {
	GPP_CLK_ON,  /* GPP clock always on; default */
	GPP_CLK_REQ, /* GPP clock controlled by corresponding #CLK_REQx pin */
	GPP_CLK_OFF, /* GPP clk off */
};

void pcie_gpp_dxio_update_clk_req_config(enum gpp_clk_req *gpp_clk_config,
					 size_t gpp_clk_config_num);

/* configure the general purpose PCIe clock outputs according to the devicetree settings */
void gpp_clk_setup_common(enum gpp_clk_req *gpp_clk_config,
			  size_t gpp_clk_config_num);

#endif
