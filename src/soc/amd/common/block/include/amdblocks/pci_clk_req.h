/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_PCI_GPP_H
#define AMD_BLOCK_PCI_GPP_H

#include <soc/platform_descriptors.h>

enum gpp_clk_req {
	GPP_CLK_ON,  /* GPP clock always on; default */
	GPP_CLK_REQ, /* GPP clock controlled by corresponding #CLK_REQx pin */
	GPP_CLK_OFF, /* GPP clk off */
};

void pcie_gpp_dxio_update_clk_req_config(enum gpp_clk_req *gpp_clk_config,
					 size_t gpp_clk_config_num);

#endif
