/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpi_pm.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include <soc/device_nvs.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/msr.h>
#include <soc/nvs.h>
#include <soc/pattrs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/iosf.h>

#define SHOW_PATTRS 1

struct pattrs __global_pattrs;

static void detect_num_cpus(struct pattrs *attrs)
{
	int ecx = 0;

	while (1) {
		struct cpuid_result leaf_b;

		leaf_b = cpuid_ext(0xb, ecx);

		/* Bay Trail doesn't have hyperthreading so just determine the
		 * number of cores by from level type (ecx[15:8] == * 2). */
		if ((leaf_b.ecx & 0xff00) == 0x0200) {
			attrs->num_cpus = leaf_b.ebx & 0xffff;
			break;
		}
		ecx++;
	}
}

static inline void fill_in_msr(msr_t *msr, int idx)
{
	*msr = rdmsr(idx);
	if (SHOW_PATTRS) {
		printk(BIOS_DEBUG, "msr(%x) = %08x%08x\n",
		       idx, msr->hi, msr->lo);
	}
}

static const char *stepping_str[] = {
	"A0", "A1", "B0", "B1", "B2", "B3", "C0", "D0",
};

static void fill_in_pattrs(void)
{
	struct device *dev;
	msr_t msr;
	struct pattrs *attrs = (struct pattrs *)pattrs_get();

	attrs->cpuid = cpuid_eax(1);
	dev = pcidev_on_root(LPC_DEV, LPC_FUNC);
	attrs->revid = pci_read_config8(dev, REVID);
	/* The revision to stepping IDs have two values per metal stepping. */
	if (attrs->revid >= RID_D_STEPPING_START) {
		attrs->stepping = (attrs->revid - RID_D_STEPPING_START) / 2;
		attrs->stepping += STEP_D0;
	} else if (attrs->revid >= RID_C_STEPPING_START) {
		attrs->stepping = (attrs->revid - RID_C_STEPPING_START) / 2;
		attrs->stepping += STEP_C0;
	} else if (attrs->revid >= RID_B_STEPPING_START) {
		attrs->stepping = (attrs->revid - RID_B_STEPPING_START) / 2;
		attrs->stepping += STEP_B0;
	} else {
		attrs->stepping = (attrs->revid - RID_A_STEPPING_START) / 2;
		attrs->stepping += STEP_A0;
	}

	attrs->microcode_patch = intel_microcode_find();
	attrs->address_bits = cpuid_eax(0x80000008) & 0xff;
	detect_num_cpus(attrs);

	if (SHOW_PATTRS) {
		printk(BIOS_DEBUG, "BYT: cpuid %08x cpus %d rid %02x step %s\n",
		       attrs->cpuid, attrs->num_cpus, attrs->revid,
		       (attrs->stepping >= ARRAY_SIZE(stepping_str)) ? "??" :
		       stepping_str[attrs->stepping]);
	}

	fill_in_msr(&attrs->platform_id, IA32_PLATFORM_ID);
	fill_in_msr(&attrs->platform_info, MSR_PLATFORM_INFO);

	/* Set IA core speed ratio and voltages */
	msr = rdmsr(MSR_IACORE_RATIOS);
	attrs->iacore_ratios[IACORE_MIN] = msr.lo & 0x7f;
	attrs->iacore_ratios[IACORE_LFM] = (msr.lo >> 8) & 0x7f;
	attrs->iacore_ratios[IACORE_MAX] = (msr.lo >> 16) & 0x7f;
	msr = rdmsr(MSR_IACORE_TURBO_RATIOS);
	attrs->iacore_ratios[IACORE_TURBO] = (msr.lo & 0xff); /* 1 core max */

	msr = rdmsr(MSR_IACORE_VIDS);
	attrs->iacore_vids[IACORE_MIN] = msr.lo & 0x7f;
	attrs->iacore_vids[IACORE_LFM] = (msr.lo >> 8) & 0x7f;
	attrs->iacore_vids[IACORE_MAX] = (msr.lo >> 16) & 0x7f;
	msr = rdmsr(MSR_IACORE_TURBO_VIDS);
	attrs->iacore_vids[IACORE_TURBO] = (msr.lo & 0xff); /* 1 core max */

	/* Set bus clock speed */
	attrs->bclk_khz = bus_freq_khz();
}

size_t size_of_dnvs(void)
{
	return sizeof(struct device_nvs);
}

/* Save bit index for first enabled event in PM1_STS for \_SB._SWS */
static void pm_fill_gnvs(struct global_nvs *gnvs, const struct chipset_power_state *ps)
{
	uint16_t pm1;
	int index;

	pm1 = ps->pm1_sts & ps->pm1_en;

	/* Scan for first set bit in PM1 */
	for (index = 0; index < 16; index++) {
		if (pm1 & 1)
			break;
		pm1 >>= 1;
	}

	if (index < 16)
		gnvs->pm1i = index;
}

static void acpi_save_wake_source(void *unused)
{
	const struct chipset_power_state *ps;
	struct global_nvs *gnvs;

	if (acpi_reset_gnvs_for_wake(&gnvs) < 0)
		return;
	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_WAKE) < 0)
		return;

	pm_fill_gnvs(gnvs, ps);

	printk(BIOS_DEBUG, "ACPI System Wake Source is PM1 Index %d\n",
	       gnvs->pm1i);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, acpi_save_wake_source, NULL);

static void baytrail_enable_2x_refresh_rate(void)
{
	u32 reg;
	reg = iosf_dunit_read(0x8);
	reg = reg & ~0x7000;
	reg = reg | 0x2000;
	iosf_dunit_write(0x8, reg);
}

void baytrail_init_pre_device(struct soc_intel_baytrail_config *config)
{
	struct soc_gpio_config *gpio_config;

	fill_in_pattrs();

	if (!config->disable_ddr_2x_refresh_rate)
		baytrail_enable_2x_refresh_rate();

	/* Allow for SSE instructions to be executed. */
	write_cr4(read_cr4() | CR4_OSFXSR | CR4_OSXMMEXCPT);

	/* Run reference code. */
	baytrail_run_reference_code();

	/* Get GPIO initial states from mainboard */
	gpio_config = mainboard_get_gpios();
	setup_soc_gpios(gpio_config, config->enable_xdp_tap);

	baytrail_init_scc();
}
