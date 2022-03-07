/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <bootmode.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <reg_script.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/i915_reg.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <drivers/intel/gma/opregion.h>
#include <soc/pm.h>
#include <soc/systemagent.h>
#include <soc/intel/broadwell/chip.h>
#include <security/vboot/vbnv.h>
#include <soc/igd.h>
#include <types.h>

#define GT_RETRY		1000
enum {
	GT_CDCLK_DEFAULT = 0,
	GT_CDCLK_337,
	GT_CDCLK_450,
	GT_CDCLK_540,
	GT_CDCLK_675,
};

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

u32 gtt_read(u32 reg)
{
	u32 val;
	val = read32(res2mmio(gtt_res, reg, 0));
	return val;

}

void gtt_write(u32 reg, u32 data)
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

int gtt_poll(u32 reg, u32 mask, u32 value)
{	unsigned int try = GT_RETRY;
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

static void gma_setup_panel(struct device *dev)
{
	struct soc_intel_broadwell_config *conf = config_of(dev);
	const struct i915_gpu_panel_config *panel_cfg = &conf->panel_cfg;
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
		reg32 |= ((panel_cfg->up_delay_ms * 10) & 0x1fff) << 16;
		reg32 |= (panel_cfg->backlight_on_delay_ms * 10) & 0x1fff;
		gtt_write(PCH_PP_ON_DELAYS, reg32);
	}

	/* Setup Panel Power Off Delays */
	reg32 = gtt_read(PCH_PP_OFF_DELAYS);
	if (!reg32) {
		reg32 = ((panel_cfg->down_delay_ms * 10) & 0x1fff) << 16;
		reg32 |= (panel_cfg->backlight_off_delay_ms * 10) & 0x1fff;
		gtt_write(PCH_PP_OFF_DELAYS, reg32);
	}

	/* Setup Panel Power Cycle Delay */
	if (panel_cfg->cycle_delay_ms) {
		reg32 = gtt_read(PCH_PP_DIVISOR);
		reg32 &= ~0x1f;
		reg32 |= (DIV_ROUND_UP(panel_cfg->cycle_delay_ms, 100) + 1) & 0x1f;
		gtt_write(PCH_PP_DIVISOR, reg32);
	}

	/* So far all devices seem to use the PCH PWM function.
	   The CPU PWM registers are all zero after reset.      */
	if (panel_cfg->backlight_pwm_hz) {
		/* For Lynx Point-LP:
		   Reference clock is 24MHz. We can choose either a 16
		   or a 128 step increment. Use 16 if we would have less
		   than 100 steps otherwise. */
		const unsigned int refclock = 24 * MHz;
		const unsigned int hz_limit = refclock / 128 / 100;
		unsigned int pwm_increment, pwm_period;
		u32 south_chicken2;

		south_chicken2 = gtt_read(SOUTH_CHICKEN2);
		if (panel_cfg->backlight_pwm_hz > hz_limit) {
			pwm_increment = 16;
			south_chicken2 |= 1 << 5;
		} else {
			pwm_increment = 128;
			south_chicken2 &= ~(1 << 5);
		}
		gtt_write(SOUTH_CHICKEN2, south_chicken2);

		pwm_period = refclock / pwm_increment / panel_cfg->backlight_pwm_hz;
		printk(BIOS_INFO,
			"GMA: Setting backlight PWM frequency to %uMHz / %u / %u = %uHz\n",
			refclock / MHz, pwm_increment, pwm_period,
			DIV_ROUND_CLOSEST(refclock, pwm_increment * pwm_period));

		/* Start with a 50% duty cycle. */
		gtt_write(BLC_PWM_PCH_CTL2, pwm_period << 16 | pwm_period / 2);

		gtt_write(BLC_PWM_PCH_CTL1,
			(panel_cfg->backlight_polarity == GPU_BACKLIGHT_POLARITY_LOW) << 29 |
			BLM_PCH_OVERRIDE_ENABLE | BLM_PCH_PWM_ENABLE);
	}
}

static int igd_get_cdclk_haswell(u32 *const cdsel, int *const inform_pc,
				 struct device *const dev)
{
	const struct soc_intel_broadwell_config *const conf = config_of(dev);
	int cdclk = conf->cdclk;

	/* Check for ULX GT1 or GT2 */
	const int devid = pci_read_config16(dev, PCI_DEVICE_ID);
	const int cpu_is_ult = cpu_family_model() == HASWELL_FAMILY_ULT;
	const int gpu_is_ulx = devid == IGD_HASWELL_ULX_GT1 ||
				devid == IGD_HASWELL_ULX_GT2;

	/* Check for fixed fused clock */
	if (gtt_read(0x42014) & 1 << 24)
		cdclk = GT_CDCLK_450;

	/*
	 *    ULX defaults to 337MHz with possible override for 450MHz
	 *    ULT is fixed at 450MHz
	 * others default  to 540MHz with possible override for 450MHz
	 */
	if (gpu_is_ulx && cdclk <= GT_CDCLK_337)
		cdclk = GT_CDCLK_337;
	else if (gpu_is_ulx || cpu_is_ult ||
			cdclk == GT_CDCLK_337 || cdclk == GT_CDCLK_450)
		cdclk = GT_CDCLK_450;
	else
		cdclk = GT_CDCLK_540;

	*cdsel = cdclk != GT_CDCLK_450;
	*inform_pc = gpu_is_ulx;
	return cdclk;
}

static int igd_get_cdclk_broadwell(u32 *const cdsel, int *const inform_pc,
				   struct device *const dev)
{
	static const u32 cdsel_by_cdclk[] = { 0, 2, 0, 1, 3 };
	const struct soc_intel_broadwell_config *const conf = config_of(dev);
	int cdclk = conf->cdclk;

	/* Check for ULX */
	const int devid = pci_read_config16(dev, PCI_DEVICE_ID);
	const int cpu_is_ult = cpu_family_model() == BROADWELL_FAMILY_ULT;
	const int gpu_is_ulx = devid == IGD_BROADWELL_Y_GT2;

	/* Inform power controller of upcoming frequency change */
	gtt_write(0x138128, 0);
	gtt_write(0x13812c, 0);
	gtt_write(0x138124, 0x80000018);

	/* Poll GT driver mailbox for run/busy clear */
	if (gtt_poll(0x138124, (1 << 31), (0 << 31))) {
		*inform_pc = 1;
	} else {
		cdclk = GT_CDCLK_450;
		*inform_pc = 0;
	}

	/* Check for fixed fused clock */
	if (gtt_read(0x42014) & 1 << 24)
		cdclk = GT_CDCLK_450;

	/*
	 *    ULX defaults to 450MHz with possible override up to 540MHz
	 *    ULT defaults to 540MHz with possible override up to 675MHz
	 * others default  to 675MHz with possible override for lower freqs
	 */
	if (cdclk == GT_CDCLK_337)
		cdclk = GT_CDCLK_337;
	else if (cdclk == GT_CDCLK_450 ||
			(gpu_is_ulx && cdclk == GT_CDCLK_DEFAULT))
		cdclk = GT_CDCLK_450;
	else if (cdclk == GT_CDCLK_540 || gpu_is_ulx ||
			(cpu_is_ult && cdclk == GT_CDCLK_DEFAULT))
		cdclk = GT_CDCLK_540;
	else
		cdclk = GT_CDCLK_675;

	*cdsel = cdsel_by_cdclk[cdclk];
	return cdclk;
}

static void igd_cdclk_init(struct device *dev, const int is_broadwell)
{
	u32 dpdiv, cdsel, cdval;
	int cdclk, inform_pc;

	if (is_broadwell)
		cdclk = igd_get_cdclk_broadwell(&cdsel, &inform_pc, dev);
	else
		cdclk = igd_get_cdclk_haswell(&cdsel, &inform_pc, dev);

	/* Set variables based on CD Clock setting */
	switch (cdclk) {
	case GT_CDCLK_337:
		cdval = 337;
		dpdiv = 169;
		reg_em4 = 16;
		reg_em5 = 225;
		break;
	case GT_CDCLK_450:
		cdval = 449;
		dpdiv = 225;
		reg_em4 = 4;
		reg_em5 = 75;
		break;
	case GT_CDCLK_540:
		cdval = 539;
		dpdiv = 270;
		reg_em4 = 4;
		reg_em5 = 90;
		break;
	case GT_CDCLK_675:
		cdval = 674;
		dpdiv = 338;
		reg_em4 = 8;
		reg_em5 = 225;
		break;
	default:
		return;
	}

	/* Set LPCLL_CTL CD Clock Frequency Select */
	gtt_rmw(0x130040, 0xf3ffffff, cdsel << 26);

	if (inform_pc) {
		/* Inform power controller of selected frequency */
		gtt_write(0x138128, cdsel);
		gtt_write(0x13812c, 0);
		gtt_write(0x138124, 0x80000017);
	}

	/* Program CD Clock Frequency */
	gtt_rmw(0x46200, 0xfffffc00, cdval);

	/* Set CPU DP AUX 2X bit clock dividers */
	gtt_rmw(0x64010, 0xfffff800, dpdiv);
	gtt_rmw(0x64810, 0xfffff800, dpdiv);
}

static void igd_init(struct device *dev)
{
	int is_broadwell = !!(cpu_family_model() == BROADWELL_FAMILY_ULT);
	u32 rp1_gfx_freq;

	intel_gma_init_igd_opregion();

	gtt_res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Early init steps */
	if (is_broadwell) {
		reg_script_run_on_dev(dev, broadwell_early_init_script);

		/* Set GFXPAUSE based on stepping */
		if (cpu_stepping() <= (CPUID_BROADWELL_ULT_E0 & 0xf) &&
		    systemagent_revision() <= 9) {
			gtt_write(0xa000, 0x300ff);
		} else {
			gtt_write(0xa000, 0x30020);
		}
	} else {
		reg_script_run_on_dev(dev, haswell_early_init_script);
	}

	/* Set RP1 graphics frequency */
	rp1_gfx_freq = (mchbar_read32(0x5998) >> 8) & 0xff;
	gtt_write(0xa008, rp1_gfx_freq << 24);

	/* Post VBIOS panel setup */
	gma_setup_panel(dev);

	/* Initialize PCI device, load/execute BIOS Option ROM */
	pci_dev_init(dev);

	/* Late init steps */
	igd_cdclk_init(dev, is_broadwell);
	if (is_broadwell)
		reg_script_run_on_dev(dev, broadwell_late_init_script);
	else
		reg_script_run_on_dev(dev, haswell_late_init_script);

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

	if (CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		int lightup_ok;
		gma_gfxinit(&lightup_ok);
		gfx_set_init_done(lightup_ok);
	}
}

static void gma_generate_ssdt(const struct device *dev)
{
	const struct soc_intel_broadwell_config *chip = dev->chip_info;

	drivers_intel_gma_displays_ssdt_generate(&chip->gfx);
}

static struct device_operations igd_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= igd_init,
	.acpi_fill_ssdt		= gma_generate_ssdt,
	.ops_pci		= &pci_dev_ops_pci,
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
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
