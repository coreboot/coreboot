/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>
#include <device/mmio.h>

static void disp_config_main_path_connection(enum disp_path_sel path)
{
	/* Main path: OVL0->RDMA0->DVO0 */
	write32(&mmsys_cfg->disp_ovl0_bgclr_mout_en, 0x1);
	write32(&mmsys_cfg->disp_ovl0_out0_mout_en, 0x2);
	write32(&mmsys_cfg->disp_rdma0_sel_in, 0x1);
	write32(&mmsys_cfg->disp_rdma0_rsz0_sout_sel, 0x0);
	write32(&mmsys_cfg->ovl_pq_out_cross1_mout_en, 0x10);
	write32(&mmsys_cfg->comp_out_cross4_mout_en, 0x02);
	printk(BIOS_DEBUG,
	       "%s: bgclr_mout_en: %#x ovl0_mout_en: %#x sel_in: %#x sout_sel: %#x"
	       "cross1_mout_en: %#x cross4_mout_en: %#x\n",
	       __func__, read32(&mmsys_cfg->disp_ovl0_bgclr_mout_en),
	       read32(&mmsys_cfg->disp_ovl0_out0_mout_en),
	       read32(&mmsys_cfg->disp_rdma0_sel_in),
	       read32(&mmsys_cfg->disp_rdma0_rsz0_sout_sel),
	       read32(&mmsys_cfg->ovl_pq_out_cross1_mout_en),
	       read32(&mmsys_cfg->comp_out_cross4_mout_en));
}

static void disp_config_main_path_mutex(enum disp_path_sel path)
{
	write32(&disp_mutex->mutex[0].mod, MUTEX_MOD_MAIN_PATH);

	/* Clock source from DVO0 */
	write32(&disp_mutex->mutex[0].ctl,
		MUTEX_SOF_DVO | (MUTEX_SOF_DVO << 7));
	write32(&disp_mutex->mutex[0].en, BIT(0));
	printk(BIOS_DEBUG, "%s: mutex_mod: %#x ctl %#x\n",
	       __func__, read32(&disp_mutex->mutex[0].mod), read32(&disp_mutex->mutex[0].ctl));
}

static void ovl_layer_smi_id_en(u32 idx)
{
	setbits32(&disp_ovl[idx]->datapath_con, BIT(0));
}

static void ovl_layer_gclast_en(u32 idx)
{
	setbits32(&disp_ovl[idx]->datapath_con, BIT(24));
	setbits32(&disp_ovl[idx]->datapath_con, BIT(25));
}

static void ovl_layer_output_clamp_en(u32 idx)
{
	setbits32(&disp_ovl[idx]->datapath_con, BIT(26));
}

static void ovl_layer_en(u32 idx)
{
	setbits32(&disp_ovl[idx]->en, BIT(0));
}

static void main_disp_path_setup(u32 width, u32 height, u32 vrefresh, enum disp_path_sel path)
{
	u32 idx;
	const u32 pixel_clk = width * height * vrefresh;

	for (idx = 0; idx < MAIN_PATH_OVL_NR; idx++) {
		/* Set OVL background color to blue */
		ovl_set_roi(idx, width, height, idx ? 0 : 0xff0000ff);
		ovl_layer_smi_id_en(idx);
		ovl_layer_gclast_en(idx);
		ovl_layer_output_clamp_en(idx);
		ovl_layer_en(idx);
	}

	rdma_config(width, height, pixel_clk, 5 * KiB);
	disp_config_main_path_connection(path);
	disp_config_main_path_mutex(path);
}

static void disp_clock_on(void)
{
	clrbits32(&mmsys_cfg->mmsys_cg_con0, CG_CON0_ALL);
	clrbits32(&mmsys_cfg->mmsys_cg_con1, CG_CON0_ALL);
	printk(BIOS_DEBUG, "%s: disp_clock: [%#x %#x]\n",
	       __func__, read32(&mmsys_cfg->mmsys_cg_con0), read32(&mmsys_cfg->mmsys_cg_con1));
}

void mtk_ddp_init(void)
{
	int i;

	disp_clock_on();

	printk(BIOS_DEBUG, "%s: shadow: %#x %#x, secure before: [%#x %#x %#x] larb: %x\n",
	       __func__,
	       read32(&mmsys_cfg->disp_bypass_mux_shadow),
	       read32(&mmsys_cfg->disp_crossbar_con),
	       read32(&mmsys_cfg->mmsys_security_disable),
	       read32(&mmsys_cfg->mmsys_security_disable1),
	       read32(&mmsys_cfg->mmsys_security_disable2),
	       read32(&smi_larb0->port_l0_ovl_rdma[0]));

	/* Turn off M4U port */
	for (i = 0; i < RDMA_PORT_NR; i++) {
		write32(&smi_larb0->port_l0_ovl_rdma[i], 0);
		write32(&smi_larb1->port_l0_ovl_rdma[i], 0);
	}
	/* disable shadow */
	write32(&mmsys_cfg->disp_bypass_mux_shadow, 0x1);
	write32(&mmsys_cfg->disp_crossbar_con, 0x00FF0000);
	/* disable secure mode */
	write32(&mmsys_cfg->mmsys_security_disable, 0xFFFFFFFF);
	write32(&mmsys_cfg->mmsys_security_disable1, 0xFFFFFFFF);
	write32(&mmsys_cfg->mmsys_security_disable2, 0xFFFFFFFF);
	printk(BIOS_DEBUG, "%s: shadow: %#x %#x, secure: [%#x %#x %#x] larb: %#x\n",
	       __func__,
	       read32(&mmsys_cfg->disp_bypass_mux_shadow),
	       read32(&mmsys_cfg->disp_crossbar_con),
	       read32(&mmsys_cfg->mmsys_security_disable),
	       read32(&mmsys_cfg->mmsys_security_disable1),
	       read32(&mmsys_cfg->mmsys_security_disable2),
	       read32(&smi_larb0->port_l0_ovl_rdma[0]));
}

void mtk_ddp_mode_set(const struct edid *edid, enum disp_path_sel path)
{
	u32 fmt = OVL_INFMT_RGBA8888;
	u32 bpp = edid->framebuffer_bits_per_pixel / 8;
	u32 width = edid->mode.ha;
	u32 height = edid->mode.va;
	u32 vrefresh_hz = edid->mode.refresh;

	printk(BIOS_INFO, "%s: display resolution: %dx%d@%d bpp %d\n",
	       __func__, width, height, vrefresh_hz, bpp);

	if (!vrefresh_hz) {
		if (!width || !height)
			vrefresh_hz = 60;
		else
			vrefresh_hz = edid->mode.pixel_clock * 1000 /
				   ((width + edid->mode.hbl) * (height + edid->mode.vbl));

		printk(BIOS_WARNING, "%s: vrefresh is not provided; using %d\n",
		       __func__, vrefresh_hz);
	}

	main_disp_path_setup(width, height, vrefresh_hz, path);
	rdma_start();
	ovl_layer_config(fmt, bpp, width, height);
}
