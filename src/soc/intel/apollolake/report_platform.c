/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_APOLLOLAKE_A0, "Apollolake A0" },
	{ CPUID_APOLLOLAKE_B0, "Apollolake B0" },
	{ CPUID_APOLLOLAKE_E0, "Apollolake E0" },
	{ CPUID_GLK_A0, "Geminilake A0" },
	{ CPUID_GLK_B0, "Geminilake B0" },
	{ CPUID_GLK_R0, "Geminilake R0" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DID_INTEL_GLK_NB, "Geminilake" },
	{ PCI_DID_INTEL_APL_NB, "Apollolake" },
};

static struct {
	u16 lpcid;
	const char *name;
} pch_table[] = {
	{ PCI_DID_INTEL_APL_LPC, "Apollolake" },
	{ PCI_DID_INTEL_GLK_LPC, "Geminilake" },
	{ PCI_DID_INTEL_GLK_ESPI, "Geminilake" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DID_INTEL_APL_IGD_HD_505, "Apollolake HD 505" },
	{ PCI_DID_INTEL_APL_IGD_HD_500, "Apollolake HD 500" },
	{ PCI_DID_INTEL_GLK_IGD, "Geminilake" },
	{ PCI_DID_INTEL_GLK_IGD_EU12, "Geminilake EU12" },
};

static uint8_t get_dev_revision(pci_devfn_t dev)
{
	return pci_read_config8(dev, PCI_REVISION_ID);
}

static uint16_t get_dev_id(pci_devfn_t dev)
{
	return pci_read_config16(dev, PCI_DEVICE_ID);
}

static void report_cpu_info(void)
{
	uint32_t i, cpu_id, cpu_feature_flag;
	char cpu_name[49];
	const char *support = "Supported";
	const char *no_support = "Not Supported";
	const char *cpu_type = "Unknown";

	fill_processor_name(cpu_name);
	cpu_id = cpu_get_cpuid();

	/* Look for string to match the name */
	for (i = 0; i < ARRAY_SIZE(cpu_table); i++) {
		if (cpu_table[i].cpuid == cpu_id) {
			cpu_type = cpu_table[i].name;
			break;
		}
	}

	printk(BIOS_INFO, "CPU: %s\n", cpu_name);
	printk(BIOS_INFO, "CPU: ID %x, %s, ucode: %08x\n", cpu_id, cpu_type,
			get_current_microcode_rev());

	cpu_feature_flag = cpu_get_feature_flags_ecx();
	printk(BIOS_INFO, "CPU: AES %s, TXT %s, VT %s\n",
			(cpu_feature_flag & CPUID_AES) ? support : no_support,
			(cpu_feature_flag & CPUID_SMX) ? support : no_support,
			(cpu_feature_flag & CPUID_VMX) ? support : no_support);
}

static void report_mch_info(void)
{
	uint32_t i;
	pci_devfn_t dev = SA_DEV_ROOT;
	uint16_t mchid = get_dev_id(dev);
	uint8_t mch_revision = get_dev_revision(dev);
	const char *mch_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(mch_table); i++) {
		if (mch_table[i].mchid == mchid) {
			mch_type = mch_table[i].name;
			break;
		}
	}

	printk(BIOS_INFO, "MCH: device id %04x (rev %02x) is %s\n",
			mchid, mch_revision, mch_type);
}

static void report_pch_info(void)
{
	uint32_t i;
	pci_devfn_t dev = PCH_DEV_LPC;
	uint16_t lpcid = get_dev_id(dev);
	const char *pch_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].lpcid == lpcid) {
			pch_type = pch_table[i].name;
			break;
		}
	}
	printk(BIOS_INFO, "PCH: device id %04x (rev %02x) is %s\n",
			lpcid, get_dev_revision(dev), pch_type);
}

static void report_igd_info(void)
{
	uint32_t i;
	pci_devfn_t dev = SA_DEV_IGD;
	uint16_t igdid = get_dev_id(dev);
	const char *igd_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(igd_table); i++) {
		if (igd_table[i].igdid == igdid) {
			igd_type = igd_table[i].name;
			break;
		}
	}
	printk(BIOS_INFO, "IGD: device id %04x (rev %02x) is %s\n",
		igdid, get_dev_revision(dev), igd_type);
}

void report_platform_info(void)
{
	report_cpu_info();
	report_mch_info();
	report_pch_info();
	report_igd_info();
}
