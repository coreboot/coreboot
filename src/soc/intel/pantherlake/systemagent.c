/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/msr.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <static.h>

/*
 * SoC implementation
 *
 * Add all known fixed memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_fixed_mmio_resources(struct device *dev, int *index)
{
	static const struct sa_mmio_descriptor soc_fixed_resources[] = {
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
		{ SAFBAR, SAF_BASE_ADDRESS, SAF_BASE_SIZE, "SAFBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
		{ REGBAR, REG_BASE_ADDRESS, REG_BASE_SIZE, "REGBAR" },
		/* First field (sa_mmio_descriptor.index) is not used, setting to 0: */
		{ 0, CRAB_ABORT_BASE_ADDR, CRAB_ABORT_SIZE, "CRAB_ABORT" },
		{ 0, LT_SECURITY_BASE_ADDR, LT_SECURITY_SIZE, "LT_SECURITY" },
		{ 0, IO_APIC_ADDR, APIC_SIZE, "APIC" },
		{ 0, PCH_PRESERVED_BASE_ADDRESS, PCH_PRESERVED_BASE_SIZE, "PCH_RESERVED" },
	};

	sa_add_fixed_mmio_resources(dev, index, soc_fixed_resources,
			ARRAY_SIZE(soc_fixed_resources));

	/* Add Vt-d resources if VT-d is enabled */
	if ((pci_read_config32(dev, CAPID0_A) & VTD_DISABLE))
		return;

	sa_add_fixed_mmio_resources(dev, index, soc_vtd_resources,
			ARRAY_SIZE(soc_vtd_resources));
}

/*
 * Set MMIO resource's fields
 */
static void set_mmio_resource(
	struct sa_mmio_descriptor *resource,
	uint64_t base,
	uint64_t size,
	const char *description)
{
	if (resource == NULL) {
		printk(BIOS_ERR, "%s: argument resource is NULL for %s\n",
		__func__, description);
		return;
	}
	resource->base = base;
	resource->size = size;
	resource->description = description;
}

int soc_get_uncore_prmmr_base_and_mask(uint64_t *prmrr_base,
	uint64_t *prmrr_mask)
{
	msr_t msr;
	msr = rdmsr(MSR_PRMRR_BASE_0);
	*prmrr_base = (uint64_t)msr.hi << 32 | msr.lo;
	msr = rdmsr(MSR_PRMRR_PHYS_MASK);
	*prmrr_mask = (uint64_t)msr.hi << 32 | msr.lo;
	return 0;
}

/*
 * SoC implementation
 *
 * Add all known configurable memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_configurable_mmio_resources(struct device *dev, int *resource_cnt)
{
	uint64_t size, base, tseg_base;
	int count = 0;
	struct sa_mmio_descriptor cfg_rsrc[6]; /* Increase size when adding more resources */

	/* MMCONF */
	size = sa_get_mmcfg_size();
	if (size > 0)
		set_mmio_resource(&(cfg_rsrc[count++]), CONFIG_ECAM_MMCONF_BASE_ADDRESS,
			size, "MMCONF");

	/* DSM */
	size = sa_get_dsm_size();
	if (size > 0) {
		base = pci_read_config32(dev, BDSM) & 0xFFF00000;
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "DSM");
	}

	/* TSEG */
	size = sa_get_tseg_size();
	tseg_base = sa_get_tseg_base();
	if (size > 0)
		set_mmio_resource(&(cfg_rsrc[count++]), tseg_base, size, "TSEG");

	/* PMRR */
	size = get_valid_prmrr_size();
	if (size > 0) {
		uint64_t mask;
		if (soc_get_uncore_prmmr_base_and_mask(&base, &mask) == 0) {
			base &= mask;
			set_mmio_resource(&(cfg_rsrc[count++]), base, size, "PMRR");
		} else {
			printk(BIOS_ERR, "SA: Failed to get PRMRR base and mask\n");
		}
	}

	/* GSM */
	size = sa_get_gsm_size();
	if (size > 0) {
		base = sa_get_gsm_base();
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "GSM");
	}

	/* DPR */
	size = sa_get_dpr_size();
	if (size > 0) {
		/* DPR just below TSEG: */
		base = tseg_base - size;
		set_mmio_resource(&(cfg_rsrc[count++]), base, size, "DPR");
	}

	/* Add all the above */
	sa_add_fixed_mmio_resources(dev, resource_cnt, cfg_rsrc, count);
}

static void configure_tdp(struct device *dev)
{
	struct soc_power_limits_config *soc_config;
	struct device *sa;
	uint16_t sa_pci_id;
	u8 tdp;
	size_t i;
	bool config_tdp = false;
	struct soc_intel_pantherlake_config *config;

	config = config_of_soc();

	/* Get System Agent PCI ID */
	sa = pcidev_path_on_root(PCI_DEVFN_ROOT);
	sa_pci_id = sa ? pci_read_config16(sa, PCI_DEVICE_ID) : 0xFFFF;

	if (sa_pci_id == 0xFFFF) {
		printk(BIOS_WARNING, "Unknown SA PCI Device!\n");
		return;
	}

	tdp = get_cpu_tdp();

	/*
	 * Choose power limits configuration based on the CPU SA PCI ID and
	 * CPU TDP value.
	 */
	for (i = 0; i < ARRAY_SIZE(cpuid_to_ptl); i++) {
		if (sa_pci_id == cpuid_to_ptl[i].cpu_id &&
				tdp == cpuid_to_ptl[i].cpu_tdp) {
			soc_config = &config->power_limits_config[cpuid_to_ptl[i].limits];
			set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
			config_tdp = true;
			printk(BIOS_DEBUG, "Configured power limits for SA PCI ID: 0x%4x\n",
				sa_pci_id);
			break;
		}
	}

	if (!config_tdp) {
		printk(BIOS_WARNING, "Skipped power limits configuration for SA PCI ID: 0x%4x\n",
			sa_pci_id);
		return;
	}
}

union pcode_mailbox_command {
	struct {
		uint32_t command: 8;
		uint32_t param1: 8;
		uint32_t param2: 13;
		uint32_t reserved: 2;
		/*
		 * Run/Busy bit. This bit is set by BIOS to indicate the mailbox buffer is
		 * ready. pcode will clear this bit after the message is
		 * consumed.
		 */
		uint32_t runbusy: 1;
	} fields;
	uint32_t data;
};

union pcode_scaling_factor {
	struct {
		/* Core scaling factor */
		uint32_t scaling_factor: 16;
		/*
		 * Number of modules with consecutive module id sharing the same scaling
		 * factor
		 */
		uint32_t num_equivalent_module: 8;
		uint32_t reserved: 8;
	} fields;
	uint32_t data;
};

#define MAILBOX_WAIT_TIMEOUT_US			1000
#define PCODE_READ_CORE_SCALING_FACTOR_CMD	0x21

static bool poll_mailbox_ready(void)
{
	union pcode_mailbox_command cmd;
	size_t i;

	for (i = 0; i < MAILBOX_WAIT_TIMEOUT_US; i++) {
		cmd.data = MCHBAR32(PCODE_MAILBOX_INTERFACE);
		if (!cmd.fields.runbusy)
			return true;
		udelay(1);
	}
	return false;
}

static u16 u88_to_scaling_factor(u16 u88)
{
	unsigned int tmp = (u88 & 0xff) * 100;
	unsigned int fraction = tmp / (1 << 8);

	/* Rounding */
	if (((tmp & 0xff) << 1) >= (1 << 8))
		fraction++;
	return ((u88 >> 8) * 100) + fraction;
}

/*
 * The following function sends commands to the pcode mailbox interface to read the core scaling
 * factors for performance and efficient cores.
 *
 * The READ_CORE_SCALING_FACTOR command takes a module ID as a parameter. The function iterates
 * over all the CPU devices to identify module IDs of different core types (efficient and
 * performance).
 *
 * If no efficient cores are present, no efficient factor is returned.
 *
 * Return values:
 * - CB_ERR_ARG: If any of the input pointers were NULL.
 * - CB_ERR: If there was a generic error while reading the scaling factors.
 * - CB_SUCCESS: If the scaling factors were read successfully.
 */
enum cb_err soc_read_core_scaling_factors(u16 *performance, u16 *efficient)
{
	extern struct cpu_info cpu_infos[];
	union pcode_mailbox_command cmd = {
		.fields = {
			.command = PCODE_READ_CORE_SCALING_FACTOR_CMD,
			.runbusy = 1
		}
	};
	union pcode_scaling_factor res;
	bool has_efficient_core = false;

	if (!performance || !efficient)
		return CB_ERR_ARG;

	for (size_t i = 0; i < CONFIG_MAX_CPUS; i++) {
		struct device *cpu = cpu_infos[i].cpu;

		if (!cpu)
			continue;

		if (cpu->path.apic.core_type != CPU_TYPE_PERF)
			has_efficient_core = true;

		if (cpu->path.apic.core_type == CPU_TYPE_PERF && *performance)
			continue;

		cmd.fields.param1 = cpu->path.apic.module_id;

		if (!poll_mailbox_ready()) {
			printk(BIOS_ERR, "pcode mailbox is busy\n");
			return CB_ERR;
		}

		MCHBAR32(PCODE_MAILBOX_INTERFACE) = cmd.data;

		if (!poll_mailbox_ready()) {
			printk(BIOS_ERR, "pcode command mailbox not completing in time\n");
			return CB_ERR;
		}

		res.data = MCHBAR32(PCODE_MAILBOX_DATA);

		if (cpu->path.apic.core_type == CPU_TYPE_PERF)
			*performance = u88_to_scaling_factor(res.fields.scaling_factor);
		else
			*efficient = u88_to_scaling_factor(res.fields.scaling_factor);

		if (*performance && *efficient)
			break;
	}

	if (!*performance) {
		printk(BIOS_ERR, "Could not read performance scaling factor\n");
		return CB_ERR;
	}
	if (has_efficient_core && !*efficient) {
		printk(BIOS_ERR, "Could not read efficient scaling factor\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}

/*
 * SoC implementation
 *
 * Perform System Agent Initialization during ramstage phase.
 */
void soc_systemagent_init(struct device *dev)
{
	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Configure TDP */
	configure_tdp(dev);
}

uint32_t soc_systemagent_max_chan_capacity_mib(u8 capid0_a_ddrsz)
{
	switch (capid0_a_ddrsz) {
	case 1:
		return 8192;
	case 2:
		return 4096;
	case 3:
		return 2048;
	default:
		return 65536;
	}
}
