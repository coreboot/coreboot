/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <acpi/acpi_pm.h>
#include <console/console.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/acpi_wake_source.h>
#include <fsp/util.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>

#define SHOW_PATTRS 1

struct pattrs __global_pattrs;

static void detect_num_cpus(struct pattrs *attrs)
{
	int ecx = 0;

	while (1) {
		struct cpuid_result leaf_b;

		leaf_b = cpuid_ext(0xb, ecx);

		/*
		 * The SOC doesn't have hyperthreading so just determine the
		 * number of cores by from level type (ecx[15:8] == * 2).
		 */
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

static const char *const stepping_str[] = {
	"A0", "A1", "B0", "B1", "B2", "B3", "C0", "D1"
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
		attrs->stepping += STEP_D1;

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
		printk(BIOS_DEBUG, "Cpuid %08x cpus %d rid %02x step %s\n",
		       attrs->cpuid, attrs->num_cpus, attrs->revid,
		       (attrs->stepping >= ARRAY_SIZE(stepping_str)) ? "??" :
		       stepping_str[attrs->stepping]);
	}

	fill_in_msr(&attrs->platform_id, IA32_PLATFORM_ID);
	fill_in_msr(&attrs->platform_info, MSR_PLATFORM_INFO);

	/* Set IA core speed ratio and voltages */
	fill_in_msr(&msr, MSR_IACORE_RATIOS);
	attrs->iacore_ratios[IACORE_MIN] = (msr.lo >>  0) & 0x7f;
	attrs->iacore_ratios[IACORE_LFM] = (msr.lo >>  8) & 0x7f;
	attrs->iacore_ratios[IACORE_MAX] = (msr.lo >> 16) & 0x7f;
	fill_in_msr(&msr, MSR_IACORE_TURBO_RATIOS);
	attrs->iacore_ratios[IACORE_TURBO] = (msr.lo & 0xff); /* 1 core max */

	fill_in_msr(&msr, MSR_IACORE_VIDS);
	attrs->iacore_vids[IACORE_MIN] = (msr.lo >>  0) & 0x7f;
	attrs->iacore_vids[IACORE_LFM] = (msr.lo >>  8) & 0x7f;
	attrs->iacore_vids[IACORE_MAX] = (msr.lo >> 16) & 0x7f;
	fill_in_msr(&msr, MSR_IACORE_TURBO_VIDS);
	attrs->iacore_vids[IACORE_TURBO] = (msr.lo & 0xff); /* 1 core max */

	/* Set bus clock speed */
	attrs->bclk_khz = cpu_bus_freq_khz();
}

/* Save wake source information for calculating ACPI _SWS values */
int soc_fill_acpi_wake(const struct chipset_power_state *ps, uint32_t *pm1, uint32_t **gpe0)
{
	static uint32_t gpe0_sts;

	*pm1 = ps->pm1_sts & ps->pm1_en;

	gpe0_sts = ps->gpe0_sts & ps->gpe0_en;
	*gpe0 = &gpe0_sts;

	return 1;
}

void soc_init_pre_device(struct soc_intel_braswell_config *config)
{
	struct soc_gpio_config *gpio_config;

	fill_in_pattrs();

	/* Allow for SSE instructions to be executed. */
	write_cr4(read_cr4() | CR4_OSFXSR | CR4_OSXMMEXCPT);

	/* Perform silicon specific init. */
	intel_silicon_init();
	set_max_freq();

	/* Get GPIO initial states from mainboard */
	gpio_config = mainboard_get_gpios();
	setup_soc_gpios(gpio_config, config->enable_xdp_tap);
}
