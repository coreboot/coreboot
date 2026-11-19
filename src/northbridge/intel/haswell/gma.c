/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <bootmode.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <cpu/intel/haswell/haswell.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/i915_reg.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <drivers/intel/gma/opregion.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

#include "chip.h"
#include "haswell.h"

/*
 * Some VGA option roms are used for several chipsets but they only have one PCI ID in their
 * header. If we encounter such an option rom, we need to do the mapping ourselves.
 */

u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	switch (vendev) {
	case 0x80860402:		/* HSW GT1 Desktop */
	case 0x80860406:		/* HSW GT1 Mobile */
	case 0x8086040a:		/* HSW GT1 Server */
	case 0x80860a06:		/* HSW GT1 ULT */
	case 0x80860a0e:		/* HSW GT1 ULX */

	case 0x80860412:		/* HSW GT2 Desktop */
	case 0x80860416:		/* HSW GT2 Mobile */
	case 0x8086041a:		/* HSW GT2 Server */
	case 0x8086041e:		/* HSW GT1.5 Desktop */
	case 0x80860a16:		/* HSW GT2 ULT */
	case 0x80860a1e:		/* HSW GT2 ULX */

	case 0x80860422:		/* HSW GT3 Desktop */
	case 0x80860426:		/* HSW GT3 Mobile */
	case 0x8086042a:		/* HSW GT3 Server */
	case 0x80860a26:		/* HSW GT3 ULT */

	case 0x80860d22:		/* HSW GT3e Desktop */
	case 0x80860d16:		/* HSW GT1 Mobile 4+3 */
	case 0x80860d26:		/* HSW GT2 Mobile 4+3, GT3e Mobile */
	case 0x80860d36:		/* HSW GT3 Mobile 4+3 */

	case 0x80861612:		/* BDW Halo GT2 */
	case 0x80861622:		/* BDW Halo/Desktop GT3 */

	case 0x80861606:		/* BDW ULT GT1 */
	case 0x80861616:		/* BDW ULT GT2 */
	case 0x80861626:		/* BDW ULT GT3 15W */
	case 0x8086162b:		/* BDW ULT GT3 28W */
	case 0x8086161e:		/* BDW ULX GT2 */

		new_vendev = 0x80860406;	/* HSW GT1 Mobile */
		break;
	}

	return new_vendev;
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

static inline void gtt_or(u32 reg, u32 ormask)
{
	gtt_rmw(reg, ~0ul, ormask);
}

#define GTT_RETRY 1000
int gtt_poll(u32 reg, u32 mask, u32 value)
{
	unsigned int try = GTT_RETRY;
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

static int gtt_pcode_write(u32 mbox, u32 val)
{
	if (!gtt_poll(GEN6_PCODE_MAILBOX, GEN6_PCODE_READY, 0))
		return 0;

	gtt_write(GEN6_PCODE_DATA, val);
	gtt_write(GEN6_PCODE_DATA1, 0);
	gtt_write(GEN6_PCODE_MAILBOX, GEN6_PCODE_READY | mbox);

	return gtt_poll(GEN6_PCODE_MAILBOX, GEN6_PCODE_READY, 0);
}

static void gma_pm_init_haswell(void)
{
	printk(BIOS_DEBUG, "GT Power Management Init\n");

	/* Note: we unconditionally enable Render Standby */

	/* Enable Force Wake */
	gtt_or(0xa180, 1 << 5);
	gtt_write(0xa188, 0x00010001);
	gtt_poll(FORCEWAKE_ACK_HSW, 1 << 0, 1 << 0);

	/* Enable counters except Power Meter */
	gtt_write(0xa248, 0x00000016);

	/* GFXPAUSE settings (C0 and later) */
	gtt_write(0xa000, 0x00070020);

	/* ECO settings (C0 and later), Render Standby */
	gtt_rmw(0xa180, 0xff3fffff, 0x15000000);

	/* Enable DOP Clock Gating */
	gtt_write(0x9424, 0x000003fd);

	/* Enable Unit Level Clock Gating */
	gtt_write(0x9400, 0x00000080);
	gtt_write(0x9404, 0x40401000);
	gtt_write(0x9408, 0x00000000);
	gtt_write(0x940c, 0x02000001);

	/* Set RP1 graphics frequency */
	gtt_write(0xa008, 0x08000000);

	/* Wake Rate Limits */
	gtt_write(0xa090, 0x00000000);
	gtt_write(0xa098, 0x03e80000);
	gtt_write(0xa09c, 0x00280000);
	gtt_write(0xa0a8, 0x0001e848);
	gtt_write(0xa0ac, 0x00000019);

	/* Render/Video/Blitter Idle Max Count */
	gtt_write(0x02054, 0x0000000a);
	gtt_write(0x12054, 0x0000000a);
	gtt_write(0x22054, 0x0000000a);
	gtt_write(0x1a054, 0x0000000a);

	/* RC Sleep / RCx Thresholds */
	gtt_write(0xa0b0, 0x00000000);
	gtt_write(0xa0b4, 0x000003e8);
	gtt_write(0xa0b8, 0x0000c350);

	/* RP Settings */
	gtt_write(0xa010, 0x000f4240);
	gtt_write(0xa014, 0x12060000);
	gtt_write(0xa02c, 0x0000e808);
	gtt_write(0xa030, 0x0003bd08);
	gtt_write(0xa068, 0x000101d0);
	gtt_write(0xa06c, 0x00055730);
	gtt_write(0xa070, 0x0000000a);

	/* RP Control */
	gtt_write(0xa024, 0x00000b92);

	/* HW RC6 Control */
	gtt_write(0xa090, 0x88040000);

	/* Video Frequency Request */
	gtt_write(0xa00c, 0x08000000);

	/* Set RC6 VIDs */
	gtt_pcode_write(GEN6_PCODE_WRITE_RC6VIDS, 0);

	/* Enable PM Interrupts */
	gtt_write(GEN6_PMIER, GEN6_PM_MBOX_EVENT | GEN6_PM_THERMAL_EVENT |
		  GEN6_PM_RP_DOWN_TIMEOUT | GEN6_PM_RP_UP_THRESHOLD |
		  GEN6_PM_RP_DOWN_THRESHOLD | GEN6_PM_RP_UP_EI_EXPIRED |
		  GEN6_PM_RP_DOWN_EI_EXPIRED);

	/* Enable RC6 in idle */
	gtt_write(0xa094, 0x00040000);

	/* PM Lock Settings */
	gtt_or(0xa248, 1ul << 31);
	gtt_or(0xa004, 1 << 4);
	gtt_or(0xa080, 1 << 2);
	gtt_or(0xa180, 1ul << 31);
}

static u8 systemagent_revision(void)
{
	return pci_read_config8(pcidev_on_root(0, 0), PCI_REVISION_ID);
}

static void gma_pm_init_broadwell(void)
{
	printk(BIOS_DEBUG, "GT Power Management Init\n");

	/* Note: we unconditionally enable Render Standby */

	/* Enable Force Wake */
	gtt_write(0xa188, 0x00010001);
	gtt_poll(FORCEWAKE_ACK_HSW, 1 << 0, 1 << 0);

	/* Enable push bus metric control and shift */
	gtt_write(0xa248, 0x00000004);
	gtt_write(0xa250, 0x000000ff);
	gtt_write(0xa25c, 0x00000010);

	/* Set GFXPAUSE based on stepping */
	if (cpu_stepping() <= (CPUID_BROADWELL_ULT_E0 & 0xf) &&
		haswell_is_ult() && systemagent_revision() <= 9) {
		gtt_write(0xa000, 0x300ff);
	} else {
		gtt_write(0xa000, 0x30020);
	}

	/* ECO settings */
	gtt_write(0xa180, 0x45200000);

	/* Enable DOP Clock Gating */
	gtt_write(0x9424, 0x000000fd);

	/* Enable Unit Level Clock Gating */
	gtt_write(0x9400, 0x00000000);
	gtt_write(0x9404, 0x40401000);
	gtt_write(0x9408, 0x00000000);
	gtt_write(0x940c, 0x02000001);
	gtt_write(0x1a054, 0x0000000a);

	/* Set RP1 graphics frequency */
	const u32 rp1_gfx_freq = (mchbar_read32(0x5998) >> 8) & 0xff;
	gtt_write(0xa008, rp1_gfx_freq << 24);

	/* Video Frequency Request */
	gtt_write(0xa00c, 0x08000000);

	gtt_write(0x138158, 0x00000009);
	gtt_write(0x13815c, 0x0000000d);

	/* Wake Rate Limits */
	gtt_write(0xa090, 0x00000000);
	gtt_write(0xa098, 0x03e80000);
	gtt_write(0xa09c, 0x00280000);
	gtt_write(0xa0a8, 0x0001e848);
	gtt_write(0xa0ac, 0x00000019);

	/* Render/Video/Blitter Idle Max Count */
	gtt_write(0x02054, 0x0000000a);
	gtt_write(0x12054, 0x0000000a);
	gtt_write(0x22054, 0x0000000a);

	/* RC Sleep / RCx Thresholds */
	gtt_write(0xa0b0, 0x00000000);
	gtt_write(0xa0b8, 0x00000271);

	/* RP Settings */
	gtt_write(0xa010, 0x000f4240);
	gtt_write(0xa014, 0x12060000);
	gtt_write(0xa02c, 0x0000e808);
	gtt_write(0xa030, 0x0003bd08);
	gtt_write(0xa068, 0x000101d0);
	gtt_write(0xa06c, 0x00055730);
	gtt_write(0xa070, 0x0000000a);
	gtt_write(0xa168, 0x00000006);

	/* RP Control */
	gtt_write(0xa024, 0x00000b92);

	/* HW RC6 Control */
	gtt_write(0xa090, 0x90040000);

	/* Set RC6 VIDs */
	gtt_pcode_write(GEN6_PCODE_WRITE_RC6VIDS, 0);

	/* Enable PM Interrupts */
	gtt_write(0x4402c, 0x03000076);

	/* Enable RC6 in idle */
	gtt_write(0xa094, 0x00040000);

	/* PM Lock Settings */
	gtt_or(0xa248, 1ul << 31);
	gtt_or(0xa000, 1 << 18);
	gtt_or(0xa180, 1ul << 31);
}

static void gma_setup_panel(struct device *dev)
{
	struct northbridge_intel_haswell_config *conf = config_of(dev);
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

	/* Enforce the PCH PWM function, as so does Linux.
	   The CPU PWM controls are disabled after reset.  */
	if (panel_cfg->backlight_pwm_hz) {
		/* Reference clock is either 24MHz or 135MHz. We can choose
		   either a 16 or a 128 step increment. Use 16 if we would
		   have less than 100 steps otherwise. */
		const unsigned int refclock = CONFIG(INTEL_LYNXPOINT_LP) ? 24*MHz : 135*MHz;
		const unsigned int hz_limit = refclock / 128 / 100;
		unsigned int pwm_increment, pwm_period;
		u32 south_chicken2;

		south_chicken2 = gtt_read(SOUTH_CHICKEN2);
		if (panel_cfg->backlight_pwm_hz > hz_limit) {
			pwm_increment = 16;
			south_chicken2 |= LPT_PWM_GRANULARITY;
		} else {
			pwm_increment = 128;
			south_chicken2 &= ~LPT_PWM_GRANULARITY;
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

	/*
	 * DDI-A params set:
	 * bit 0: Display detected (RO)
	 * bit 4: DDI A supports 4 lanes and DDI E is not used
	 * bit 7: DDI buffer is idle
	 */
	reg32 = DDI_BUF_IS_IDLE | DDI_INIT_DISPLAY_DETECTED;
	if (!conf->gpu_ddi_e_connected)
		reg32 |= DDI_A_4_LANES;
	gtt_write(DDI_BUF_CTL_A, reg32);
}

enum gpu_type {
	GPU_TYPE_ULX,
	GPU_TYPE_ULT,
	GPU_TYPE_TRAD,
};

static enum gpu_type get_gpu_type(struct device *dev)
{
	if (haswell_is_ult()) {
		const u16 devid = pci_read_config16(dev, PCI_DEVICE_ID);
		switch (devid) {
		case 0x0a0e:
		case 0x0a1e:
		case 0x161e:
			return GPU_TYPE_ULX;
		default:
			return GPU_TYPE_ULT;
		}
	} else {
		return GPU_TYPE_TRAD;
	}
}

enum gt_cdclk {
	GT_CDCLK_DEFAULT = 0,
	GT_CDCLK_337,
	GT_CDCLK_450,
	GT_CDCLK_540,
	GT_CDCLK_675,
};

static enum gt_cdclk get_cdclk(bool is_broadwell, enum gpu_type type)
{
	/* TODO: Make this a config option? */
	enum gt_cdclk cdclk = GT_CDCLK_DEFAULT;

	/* If CDCLK frequency selection is not supported, 450 MHz is forced */
	if (gtt_read(HSW_FUSE_STRAP) & HSW_CDCLK_LIMIT)
		return GT_CDCLK_450;

	/*
	 * BDW ULT/ULX requires extra cooling to run at the highest frequency
	 * TODO: Make this a devicetree setting?
	 */
	const bool bdw_ult_high_power = false;

	enum gt_cdclk lower_cdclk;
	enum gt_cdclk upper_cdclk;
	switch (type) {
	case GPU_TYPE_ULX:
		if (is_broadwell) {
			lower_cdclk = GT_CDCLK_337;
			upper_cdclk = bdw_ult_high_power ? GT_CDCLK_540 : GT_CDCLK_450;
		} else {
			lower_cdclk = GT_CDCLK_337;
			upper_cdclk = GT_CDCLK_450;
		}
		break;
	case GPU_TYPE_ULT:
		if (is_broadwell) {
			lower_cdclk = GT_CDCLK_337;
			upper_cdclk = bdw_ult_high_power ? GT_CDCLK_675 : GT_CDCLK_540;
		} else {
			lower_cdclk = GT_CDCLK_450;
			upper_cdclk = GT_CDCLK_450;
		}
		break;
	case GPU_TYPE_TRAD:
		if (is_broadwell) {
			lower_cdclk = GT_CDCLK_337;
			upper_cdclk = GT_CDCLK_675;
		} else {
			lower_cdclk = GT_CDCLK_450;
			upper_cdclk = GT_CDCLK_540;
		}
		break;
	default:
		lower_cdclk = GT_CDCLK_450;
		upper_cdclk = GT_CDCLK_450;
		break;
	}

	if (cdclk == GT_CDCLK_DEFAULT)
		cdclk = upper_cdclk;

	/* Clamp CDCLK to supported range */
	return MAX(lower_cdclk, MIN(cdclk, upper_cdclk));
}

static u32 cdsel_from_cdclk(bool is_broadwell, enum gt_cdclk cdclk)
{
	if (!is_broadwell)
		return cdclk != GT_CDCLK_450;

	switch (cdclk) {
		default:
		case GT_CDCLK_450: return 0;
		case GT_CDCLK_540: return 1;
		case GT_CDCLK_337: return 2;
		case GT_CDCLK_675: return 3;
	}
}

#define HSW_PCODE_DE_WRITE_FREQ_REQ		0x17
#define BDW_PCODE_DISPLAY_FREQ_CHANGE_REQ	0x18

static void gma_cdclk_init(struct device *dev, bool is_broadwell)
{
	if (is_broadwell) {
		/* Inform power controller of upcoming frequency change */
		if (!gtt_pcode_write(BDW_PCODE_DISPLAY_FREQ_CHANGE_REQ, 0)) {
			printk(BIOS_ERR, "Failed to inform pcode about cdclk change\n");
			return;
		}
	}

	const enum gpu_type type = get_gpu_type(dev);
	const enum gt_cdclk cdclk = get_cdclk(is_broadwell, type);

	assert(cdclk >= GT_CDCLK_337);

	const u32 cdsel = cdsel_from_cdclk(is_broadwell, cdclk);

	/*
	 * Set CD Clock Frequency Select
	 * TODO: For BDW, should we switch CDCLK source to FCLK before
	 * updating the CDCLK frequency selector, or is it a non-issue
	 * when done so early?
	 */
	gtt_rmw(LCPLL_CTL, ~LCPLL_CLK_FREQ_MASK, cdsel << 26);

	if (is_broadwell || type == GPU_TYPE_ULX) {
		/* Inform power controller of selected frequency */
		gtt_pcode_write(HSW_PCODE_DE_WRITE_FREQ_REQ, cdsel);
	}

	if (is_broadwell) {
		u32 cdval, dpdiv;
		switch (cdclk) {
		case GT_CDCLK_337:
			cdval = 337;
			dpdiv = 169;
			break;
		case GT_CDCLK_450:
			cdval = 449;
			dpdiv = 225;
			break;
		case GT_CDCLK_540:
			cdval = 539;
			dpdiv = 270;
			break;
		case GT_CDCLK_675:
			cdval = 674;
			dpdiv = 338;
			break;
		default:
			return;
		}

		/* Program CD Clock Frequency */
		gtt_rmw(0x46200, 0xfffffc00, cdval);

		/* Set CPU DP AUX 2X bit clock dividers */
		gtt_rmw(0x64010, 0xfffff800, dpdiv);
		gtt_rmw(0x64810, 0xfffff800, dpdiv);
	}
}

static void gma_pm_init_post_vbios(bool is_broadwell)
{
	/* Disable Force Wake */
	gtt_write(0xa188, 0x00010000);
	gtt_poll(FORCEWAKE_ACK_HSW, 1 << 0, 0 << 0);
	if (!is_broadwell)
		gtt_write(0xa188, 0x00000001);
}

/* Enable SCI to ACPI _GPE._L06 */
static void gma_enable_swsci(void)
{
	u16 reg16;

	/* Clear DMISCI status */
	reg16 = inw(get_pmbase() + TCO1_STS);
	reg16 &= DMISCI_STS;
	outw(reg16, get_pmbase() + TCO1_STS);

	/* Clear and enable ACPI TCO SCI */
	enable_tco_sci();
}

static void gma_func0_init(struct device *dev)
{
	const bool is_broadwell = cpu_is_broadwell();

	int lightup_ok = 0;

	intel_gma_init_igd_opregion();

	gtt_res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	/* Init graphics power management */
	if (is_broadwell)
		gma_pm_init_broadwell();
	else
		gma_pm_init_haswell();

	/* Enable power well for DP and Audio */
	gtt_write(HSW_PWR_WELL_CTL1, HSW_PWR_WELL_ENABLE);
	gtt_poll(HSW_PWR_WELL_CTL1, HSW_PWR_WELL_STATE, HSW_PWR_WELL_STATE);

	/* Pre panel init */
	gma_setup_panel(dev);

	gma_cdclk_init(dev, is_broadwell);

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	int vga_disable = (pci_read_config16(dev, GGC) & 2) >> 1;

	if (CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		if (vga_disable) {
			printk(BIOS_INFO,
			       "IGD is not decoding legacy VGA MEM and IO: skipping NATIVE graphic init\n");
		} else {
			printk(BIOS_SPEW, "NATIVE graphics, run native enable\n");
			gma_gfxinit(&lightup_ok);
			gfx_set_init_done(1);

			/* libgfxinit drops the power well for eDP-only; mini-HD needs it */
			if (is_dev_enabled(pcidev_on_root(3, 0))) {
				gtt_rmw(HSW_PWR_WELL_CTL1, ~0u, HSW_PWR_WELL_ENABLE);
				gtt_poll(HSW_PWR_WELL_CTL1, HSW_PWR_WELL_STATE, HSW_PWR_WELL_STATE);
			}
		}
	}

	if (!lightup_ok) {
		printk(BIOS_SPEW, "FUI did not run; using VBIOS\n");
		pci_dev_init(dev);
	}

	printk(BIOS_DEBUG, "GT Power Management Init (post VBIOS)\n");

	gma_pm_init_post_vbios(is_broadwell);

	gma_enable_swsci();
}

static void gma_generate_ssdt(const struct device *dev)
{
	const struct northbridge_intel_haswell_config *chip = dev->chip_info;

	drivers_intel_gma_displays_ssdt_generate(&chip->gfx);
}

static void gma_func0_disable(struct device *dev)
{
	/* Disable VGA decode */
	pci_or_config16(pcidev_on_root(0, 0), GGC, 1 << 1);
}

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func0_init,
	.acpi_fill_ssdt		= gma_generate_ssdt,
	.vga_disable            = gma_func0_disable,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x0402, /* HSW Desktop GT1 */
	0x0412, /* HSW Desktop GT2 */
	0x041e, /* HSW Desktop GT1.5 */
	0x0422, /* HSW Desktop GT3 */
	0x0d22, /* HSW Desktop GT3e */
	0x0406, /* HSW Mobile GT1 */
	0x0416, /* HSW Mobile GT2 */
	0x0426, /* HSW Mobile GT3 */
	0x040a, /* HSW Server GT1 */
	0x041a, /* HSW Server GT2 */
	0x042a, /* HSW Server GT3 */
	0x0d16, /* HSW Mobile 4+3 GT1 */
	0x0d26, /* HSW Mobile 4+3 GT2, Mobile GT3e */
	0x0d36, /* HSW Mobile 4+3 GT3 */
	0x0a06, /* HSW ULT GT1 */
	0x0a16, /* HSW ULT GT2 */
	0x0a26, /* HSW ULT GT3 */
	0x0a0e, /* HSW ULX GT1 */
	0x0a1e, /* HSW ULX GT2 */
	0x1612, /* BDW Halo GT2 */
	0x1622, /* BDW Halo/Desktop GT3 */
	0x1606, /* BDW ULT GT1 */
	0x1616, /* BDW ULT GT2 */
	0x1626, /* BDW ULT GT3 15W */
	0x162b, /* BDW ULT GT3 28W */
	0x161e, /* BDW ULX GT2 */
	0,
};

static const struct pci_driver gma_func0_driver __pci_driver = {
	.ops     = &gma_func0_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
