/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>
#include <console/console.h>
#include <soc/qcom_spmi.h>
#include <gpio.h>

void configure_vbif_qos(void)
{
	/* Global Inits */
	/* dynamic clock gating enabled */
	write32(&vbif_rt->vbif_clkon, 0x00000000);

	/* DDR AXI max bursts: RD=7+1, WR=7+1 */
	write32(&vbif_rt->vbif_ddr_out_max_burst, 0x00000707);

	/* enable SW override of AOOO per client */
	write32(&vbif_rt->vbif_out_axi_aooo_en, 0x0000FFFF);

	/* force AOOORD/AOOOWR for all clients */
	write32(&vbif_rt->vbif_out_axi_aooo, 0xFFFFFFFF);

	write32(&vbif_rt->vbif_out_axi_amemtype_conf0, 0x33333333);
	write32(&vbif_rt->vbif_out_axi_amemtype_conf1, 0x00333333);

	write32(&vbif_rt->qos_rp_remap[0].vbif_xinl_qos_rp_remap, 0x00000003);
	write32(&vbif_rt->qos_rp_remap[1].vbif_xinl_qos_rp_remap, 0x11111113);
	write32(&vbif_rt->qos_rp_remap[2].vbif_xinl_qos_rp_remap, 0x22222224);
	write32(&vbif_rt->qos_rp_remap[3].vbif_xinl_qos_rp_remap, 0x33333334);
	write32(&vbif_rt->qos_rp_remap[4].vbif_xinl_qos_rp_remap, 0x44444445);
	write32(&vbif_rt->qos_rp_remap[7].vbif_xinl_qos_rp_remap, 0x77777776);
	write32(&vbif_rt->qos_lvl_remap[0].vbif_xinl_qos_lvl_remap, 0x00000003);
	write32(&vbif_rt->qos_lvl_remap[1].vbif_xinl_qos_lvl_remap, 0x11111113);
	write32(&vbif_rt->qos_lvl_remap[2].vbif_xinl_qos_lvl_remap, 0x22222224);
	write32(&vbif_rt->qos_lvl_remap[3].vbif_xinl_qos_lvl_remap, 0x33333334);
	write32(&vbif_rt->qos_lvl_remap[4].vbif_xinl_qos_lvl_remap, 0x44444445);
	write32(&vbif_rt->qos_lvl_remap[5].vbif_xinl_qos_lvl_remap, 0x77777776);
}
