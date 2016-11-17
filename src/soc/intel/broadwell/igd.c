/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <arch/io.h>
#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <reg_script.h>
#include <drivers/intel/gma/i915_reg.h>
#include <soc/cpu.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <soc/intel/broadwell/chip.h>
#include <vboot/vbnv.h>
#include <soc/igd.h>

#define GT_RETRY 		1000
#define GT_CDCLK_337		0
#define GT_CDCLK_450		1
#define GT_CDCLK_540		2
#define GT_CDCLK_675		3

static u32 reg_em4;
static u32 reg_em5;

u32 igd_get_reg_em4(void) { return reg_em4; }
u32 igd_get_reg_em5(void) { return reg_em5; }

struct reg_script haswell_early_init_script[] = {
	/* Enable Force Wake */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa180, 0x00000020),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa188, 0x00010001),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, FORCEWAKE_ACK_HSW, 1, 1, GT_RETRY),

	/* Enable Counters */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa248, 0x00000016),

	/* GFXPAUSE settings */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa000, 0x00070020),

	/* ECO Settings */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0xa180, 0xff3fffff, 0x15000000),

	/* Enable DOP Clock Gating */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9424, 0x000003fd),

	/* Enable Unit Level Clock Gating */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9400, 0x00000080),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9404, 0x40401000),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9408, 0x00000000),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x940c, 0x02000001),

	/*
	 * RC6 Settings
	 */

	/* Wake Rate Limits */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa090, 0x00000000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa098, 0x03e80000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa09c, 0x00280000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa0a8, 0x0001e848),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa0ac, 0x00000019),

	/* Render/Video/Blitter Idle Max Count */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x02054, 0x0000000a),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x12054, 0x0000000a),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x22054, 0x0000000a),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x1a054, 0x0000000a),

	/* RC Sleep / RCx Thresholds */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa0b0, 0x00000000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa0b4, 0x000003e8),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa0b8, 0x0000c350),

	/* RP Settings */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa010, 0x000f4240),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa014, 0x12060000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa02c, 0x0000e808),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa030, 0x0003bd08),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa068, 0x000101d0),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa06c, 0x00055730),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0xa070, 0x0000000a),

	/* RP Control */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa024, 0x00000b92),

	/* HW RC6 Control */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa090, 0x88040000),

	/* Video Frequency Request */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa00c, 0x08000000),

	/* Set RC6 VIDs */
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x138124, (1 << 31), 0, GT_RETRY),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x138128, 0),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x138124, 0x80000004),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x138124, (1 << 31), 0, GT_RETRY),

	/* Enable PM Interrupts */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x4402c, 0x03000076),

	/* Enable RC6 in idle */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa094, 0x00040000),

	REG_SCRIPT_END
};

static const struct reg_script haswell_late_init_script[] = {
	/* Lock settings */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a248, (1 << 31)),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a004, (1 << 4)),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a080, (1 << 2)),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a180, (1 << 31)),

	/* Disable Force Wake */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa188, 0x00010000),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, FORCEWAKE_ACK_HSW, 1, 0, GT_RETRY),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa188, 0x00000001),

	/* Enable power well for DP and Audio */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x45400, (1 << 31)),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x45400,
		       (1 << 30), (1 << 30), GT_RETRY),

	REG_SCRIPT_END
};

static const struct reg_script broadwell_early_init_script[] = {
	/* Enable Force Wake */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa188, 0x00010001),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, FORCEWAKE_ACK_HSW, 1, 1, GT_RETRY),

	/* Enable push bus metric control and shift */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa248, 0x00000004),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa250, 0x000000ff),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa25c, 0x00000010),

	/* GFXPAUSE settings (set based on stepping) */

	/* ECO Settings */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa180, 0x45200000),

	/* Enable DOP Clock Gating */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9424, 0x000000fd),

	/* Enable Unit Level Clock Gating */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9400, 0x00000000),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9404, 0x40401000),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x9408, 0x00000000),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x940c, 0x02000001),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x1a054, 0x0000000a),

	/* Video Frequency Request */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa00c, 0x08000000),

	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x138158, 0x00000009),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x13815c, 0x0000000d),

	/*
	 * RC6 Settings
	 */

	/* Wake Rate Limits */
	REG_RES_RMW32(PCI_BASE_ADDRESS_0, 0x0a090, 0, 0),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a098, 0x03e80000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a09c, 0x00280000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a0a8, 0x0001e848),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a0ac, 0x00000019),

	/* Render/Video/Blitter Idle Max Count */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x02054, 0x0000000a),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x12054, 0x0000000a),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x22054, 0x0000000a),

	/* RC Sleep / RCx Thresholds */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a0b0, 0x00000000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a0b8, 0x00000271),

	/* RP Settings */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a010, 0x000f4240),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a014, 0x12060000),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a02c, 0x0000e808),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a030, 0x0003bd08),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a068, 0x000101d0),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a06c, 0x00055730),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a070, 0x0000000a),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a168, 0x00000006),

	/* RP Control */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa024, 0x00000b92),

	/* HW RC6 Control */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa090, 0x90040000),

	/* Set RC6 VIDs */
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x138124, (1 << 31), 0, GT_RETRY),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x138128, 0),
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x138124, 0x80000004),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x138124, (1 << 31), 0, GT_RETRY),

	/* Enable PM Interrupts */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x4402c, 0x03000076),

	/* Enable RC6 in idle */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa094, 0x00040000),

	REG_SCRIPT_END
};

static const struct reg_script broadwell_late_init_script[] = {
	/* Lock settings */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a248, (1 << 31)),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a000, (1 << 18)),
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x0a180, (1 << 31)),

	/* Disable Force Wake */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0xa188, 0x00010000),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, FORCEWAKE_ACK_HSW, 1, 0, GT_RETRY),

	/* Enable power well for DP and Audio */
	REG_RES_OR32(PCI_BASE_ADDRESS_0, 0x45400, (1 << 31)),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x45400,
		       (1 << 30), (1 << 30), GT_RETRY),

	REG_SCRIPT_END
};

u32 map_oprom_vendev(u32 vendev)
{
	return SA_IGD_OPROM_VENDEV;
}

static struct resource *gtt_res = NULL;

static unsigned long gtt_read(unsigned long reg)
{
	u32 val;
	val = read32(res2mmio(gtt_res, reg, 0));
	return val;

}

static void gtt_write(unsigned long reg, unsigned long data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
}

static inline void gtt_rmw(u32 reg, u32 andmask, u32 ormask)
{
	u32 val = gtt_read(reg);
	val &= andmask;
	val |= ormask;
	gtt_write(reg, val);
}

static int gtt_poll(u32 reg, u32 mask, u32 value)
{
	unsigned try = GT_RETRY;
	u32 data;

	while (try--) {
		data = gtt_read(reg);
		if ((data & mask) == value)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "GT init timeout\n");
	return 0;
}

static void igd_setup_panel(struct device *dev)
{
	config_t *conf = dev->chip_info;
	u32 reg32;

	/* Setup Digital Port Hotplug */
	reg32 = gtt_read(PCH_PORT_HOTPLUG);
	if (!reg32) {
		reg32 = (conf->gpu_dp_b_hotplug & 0x7) << 2;
		reg32 |= (conf->gpu_dp_c_hotplug & 0x7) << 10;
		reg32 |= (conf->gpu_dp_d_hotplug & 0x7) << 18;
		gtt_write(PCH_PORT_HOTPLUG, reg32);
	}

	/* Setup Panel Power On Delays */
	reg32 = gtt_read(PCH_PP_ON_DELAYS);
	if (!reg32) {
		reg32 = (conf->gpu_panel_port_select & 0x3) << 30;
		reg32 |= (conf->gpu_panel_power_up_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_on_delay & 0x1fff);
		gtt_write(PCH_PP_ON_DELAYS, reg32);
	}

	/* Setup Panel Power Off Delays */
	reg32 = gtt_read(PCH_PP_OFF_DELAYS);
	if (!reg32) {
		reg32 = (conf->gpu_panel_power_down_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_off_delay & 0x1fff);
		gtt_write(PCH_PP_OFF_DELAYS, reg32);
	}

	/* Setup Panel Power Cycle Delay */
	if (conf->gpu_panel_power_cycle_delay) {
		reg32 = gtt_read(PCH_PP_DIVISOR);
		reg32 &= ~0xff;
		reg32 |= conf->gpu_panel_power_cycle_delay & 0xff;
		gtt_write(PCH_PP_DIVISOR, reg32);
	}

	/* Enable Backlight if needed */
	if (conf->gpu_cpu_backlight) {
		gtt_write(BLC_PWM_CPU_CTL2, BLC_PWM2_ENABLE);
		gtt_write(BLC_PWM_CPU_CTL, conf->gpu_cpu_backlight);
	}
	if (conf->gpu_pch_backlight) {
		gtt_write(BLC_PWM_PCH_CTL1, BLM_PCH_PWM_ENABLE);
		gtt_write(BLC_PWM_PCH_CTL2, conf->gpu_pch_backlight);
	}
}

static void igd_cdclk_init_haswell(struct device *dev)
{
	config_t *conf = dev->chip_info;
	int cdclk = conf->cdclk;
	int devid = pci_read_config16(dev, PCI_DEVICE_ID);
	int gpu_is_ulx = 0;
	u32 dpdiv, lpcll;

	/* Check for ULX GT1 or GT2 */
	if (devid == 0x0a0e || devid == 0x0a1e)
		gpu_is_ulx = 1;

	/* 675MHz is not supported on haswell */
	if (cdclk == GT_CDCLK_675)
		cdclk = GT_CDCLK_337;

	/* If CD clock is fixed or ULT then set to 450MHz */
	if ((gtt_read(0x42014) & 0x1000000) || cpu_is_ult())
		cdclk = GT_CDCLK_450;

	/* 540MHz is not supported on ULX */
	if (gpu_is_ulx && cdclk == GT_CDCLK_540)
		cdclk = GT_CDCLK_337;

	/* 337.5MHz is not supported on non-ULT/ULX */
	if (!gpu_is_ulx && !cpu_is_ult() && cdclk == GT_CDCLK_337)
		cdclk = GT_CDCLK_450;

	/* Set variables based on CD Clock setting */
	switch (cdclk) {
	case GT_CDCLK_337:
		dpdiv = 169;
		lpcll = (1 << 26);
		reg_em4 = 16;
		reg_em5 = 225;
		break;
	case GT_CDCLK_450:
		dpdiv = 225;
		lpcll = 0;
		reg_em4 = 4;
		reg_em5 = 75;
		break;
	case GT_CDCLK_540:
		dpdiv = 270;
		lpcll = (1 << 26);
		reg_em4 = 4;
		reg_em5 = 90;
		break;
	default:
		return;
	}

	/* Set LPCLL_CTL CD Clock Frequency Select */
	gtt_rmw(0x130040, 0xf3ffffff, lpcll);

	/* ULX: Inform power controller of selected frequency */
	if (gpu_is_ulx) {
		if (cdclk == GT_CDCLK_450)
			gtt_write(0x138128, 0x00000000); /* 450MHz */
		else
			gtt_write(0x138128, 0x00000001); /* 337.5MHz */
		gtt_write(0x13812c, 0x00000000);
		gtt_write(0x138124, 0x80000017);
	}

	/* Set CPU DP AUX 2X bit clock dividers */
	gtt_rmw(0x64010, 0xfffff800, dpdiv);
	gtt_rmw(0x64810, 0xfffff800, dpdiv);
}

static void igd_cdclk_init_broadwell(struct device *dev)
{
	config_t *conf = dev->chip_info;
	int cdclk = conf->cdclk;
	u32 dpdiv, lpcll, pwctl, cdset;

	/* Inform power controller of upcoming frequency change */
	gtt_write(0x138128, 0);
	gtt_write(0x13812c, 0);
	gtt_write(0x138124, 0x80000018);

	/* Poll GT driver mailbox for run/busy clear */
	if (!gtt_poll(0x138124, (1 << 31), (0 << 31)))
		cdclk = GT_CDCLK_450;

	if (gtt_read(0x42014) & 0x1000000) {
		/* If CD clock is fixed then set to 450MHz */
		cdclk = GT_CDCLK_450;
	} else {
		/* Program CD clock to highest supported freq */
		if (cpu_is_ult())
			cdclk = GT_CDCLK_540;
		else
			cdclk = GT_CDCLK_675;
	}

	/* CD clock frequency 675MHz not supported on ULT */
	if (cpu_is_ult() && cdclk == GT_CDCLK_675)
		cdclk = GT_CDCLK_540;

	/* Set variables based on CD Clock setting */
	switch (cdclk) {
	case GT_CDCLK_337:
		cdset = 337;
		lpcll = (1 << 27);
		pwctl = 2;
		dpdiv = 169;
		reg_em4 = 16;
		reg_em5 = 225;
		break;
	case GT_CDCLK_450:
		cdset = 449;
		lpcll = 0;
		pwctl = 0;
		dpdiv = 225;
		reg_em4 = 4;
		reg_em5 = 75;
		break;
	case GT_CDCLK_540:
		cdset = 539;
		lpcll = (1 << 26);
		pwctl = 1;
		dpdiv = 270;
		reg_em4 = 4;
		reg_em5 = 90;
		break;
	case GT_CDCLK_675:
		cdset = 674;
		lpcll = (1 << 26) | (1 << 27);
		pwctl = 3;
		dpdiv = 338;
		reg_em4 = 8;
		reg_em5 = 225;
	default:
		return;
	}

	/* Set LPCLL_CTL CD Clock Frequency Select */
	gtt_rmw(0x130040, 0xf3ffffff, lpcll);

	/* Inform power controller of selected frequency */
	gtt_write(0x138128, pwctl);
	gtt_write(0x13812c, 0);
	gtt_write(0x138124, 0x80000017);

	/* Program CD Clock Frequency */
	gtt_rmw(0x46200, 0xfffffc00, cdset);

	/* Set CPU DP AUX 2X bit clock dividers */
	gtt_rmw(0x64010, 0xfffff800, dpdiv);
	gtt_rmw(0x64810, 0xfffff800, dpdiv);
}

static void igd_init(struct device *dev)
{
	int is_broadwell = !!(cpu_family_model() == BROADWELL_FAMILY_ULT);
	u32 rp1_gfx_freq;

	/* IGD needs to be Bus Master */
	u32 reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	/* Wait for any configured pre-graphics delay */
	if (!acpi_is_wakeup_s3()) {
#if IS_ENABLED(CONFIG_CHROMEOS)
		if (display_init_required() || vboot_wants_oprom())
			mdelay(CONFIG_PRE_GRAPHICS_DELAY);
#else
		mdelay(CONFIG_PRE_GRAPHICS_DELAY);
#endif
	}

	/* Early init steps */
	if (is_broadwell) {
		reg_script_run_on_dev(dev, broadwell_early_init_script);

		/* Set GFXPAUSE based on stepping */
		if (cpu_stepping() <= (CPUID_BROADWELL_E0 & 0xf) &&
		    systemagent_revision() <= 9) {
			gtt_write(0xa000, 0x300ff);
		} else {
			gtt_write(0xa000, 0x30020);
		}
	} else {
		reg_script_run_on_dev(dev, haswell_early_init_script);
	}

	/* Set RP1 graphics frequency */
	rp1_gfx_freq = (MCHBAR32(0x5998) >> 8) & 0xff;
	gtt_write(0xa008, rp1_gfx_freq << 24);

	/* Post VBIOS panel setup */
	igd_setup_panel(dev);

	/* Initialize PCI device, load/execute BIOS Option ROM */
	pci_dev_init(dev);

	/* Late init steps */
	if (is_broadwell) {
		igd_cdclk_init_broadwell(dev);
		reg_script_run_on_dev(dev, broadwell_late_init_script);
	} else {
		igd_cdclk_init_haswell(dev);
		reg_script_run_on_dev(dev, haswell_late_init_script);
	}

	if (gfx_get_init_done()) {
		/*
		 * Work around VBIOS issue that is not clearing first 64
		 * bytes of the framebuffer during VBE mode set.
		 */
		struct resource *fb = find_resource(dev, PCI_BASE_ADDRESS_2);
		memset((void *)((u32)fb->base), 0, 64);
	}

	if (!gfx_get_init_done() && !acpi_is_wakeup_s3()) {
		/*
		 * Enable DDI-A if the Option ROM did not execute:
		 *
		 * bit 0: Display detected (RO)
		 * bit 4: DDI A supports 4 lanes and DDI E is not used
		 * bit 7: DDI buffer is idle
		 */
		gtt_write(DDI_BUF_CTL_A, DDI_BUF_IS_IDLE | DDI_A_4_LANES |
			  DDI_INIT_DISPLAY_DETECTED);
	}
}

static struct device_operations igd_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.init			= &igd_init,
	.ops_pci		= &broadwell_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	IGD_HASWELL_ULT_GT1,
	IGD_HASWELL_ULT_GT2,
	IGD_HASWELL_ULT_GT3,
	IGD_BROADWELL_U_GT1,
	IGD_BROADWELL_U_GT2,
	IGD_BROADWELL_U_GT3_15W,
	IGD_BROADWELL_U_GT3_28W,
	IGD_BROADWELL_Y_GT2,
	IGD_BROADWELL_H_GT2,
	IGD_BROADWELL_H_GT3,
	0,
};

static const struct pci_driver igd_driver __pci_driver = {
	.ops	 = &igd_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
