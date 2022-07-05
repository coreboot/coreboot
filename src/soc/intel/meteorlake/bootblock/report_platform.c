/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_METEORLAKE_A0_1, "MeteorLake A0" },
	{ CPUID_METEORLAKE_A0_2, "MeteorLake A0" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DID_INTEL_MTL_M_ID, "MeteorLake M" },
	{ PCI_DID_INTEL_MTL_P_ID_1, "MeteorLake P" },
	{ PCI_DID_INTEL_MTL_P_ID_2, "MeteorLake P" },
	{ PCI_DID_INTEL_MTL_P_ID_3, "MeteorLake P" },
};

static struct {
	u16 espiid;
	const char *name;
} pch_table[] = {
	{ PCI_DID_INTEL_MTL_ESPI_0, "MeteorLake SOC" },
	{ PCI_DID_INTEL_MTL_ESPI_1, "MeteorLake SOC" },
	{ PCI_DID_INTEL_MTL_ESPI_2, "MeteorLake SOC" },
	{ PCI_DID_INTEL_MTL_ESPI_3, "MeteorLake SOC" },
	{ PCI_DID_INTEL_MTL_ESPI_4, "MeteorLake SOC" },
	{ PCI_DID_INTEL_MTL_ESPI_5, "MeteorLake SOC" },
	{ PCI_DID_INTEL_MTL_ESPI_6, "MeteorLake SOC" },
	{ PCI_DID_INTEL_MTL_ESPI_7, "MeteorLake SOC" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DID_INTEL_MTL_M_GT2, "MeteorLake-M GT2" },
	{ PCI_DID_INTEL_MTL_P_GT2_1, "MeteorLake-P GT2" },
	{ PCI_DID_INTEL_MTL_P_GT2_2, "MeteorLake-P GT2" },
	{ PCI_DID_INTEL_MTL_P_GT2_3, "MeteorLake-P GT2" },
};

static inline uint8_t get_dev_revision(pci_devfn_t dev)
{
	return pci_read_config8(dev, PCI_REVISION_ID);
}

static inline uint16_t get_dev_id(pci_devfn_t dev)
{
	return pci_read_config16(dev, PCI_DEVICE_ID);
}

static void report_cpu_info(void)
{
	u32 i, cpu_id, cpu_feature_flag;
	char cpu_name[49];
	int vt, txt, aes;
	static const char *const mode[] = {"NOT ", ""};
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

	printk(BIOS_DEBUG, "CPU: %s\n", cpu_name);
	printk(BIOS_DEBUG, "CPU: ID %x, %s, ucode: %08x\n",
	       cpu_id, cpu_type, get_current_microcode_rev());

	cpu_feature_flag = cpu_get_feature_flags_ecx();
	aes = !!(cpu_feature_flag & CPUID_AES);
	txt = !!(cpu_feature_flag & CPUID_SMX);
	vt = !!(cpu_feature_flag & CPUID_VMX);
	printk(BIOS_DEBUG,
		"CPU: AES %ssupported, TXT %ssupported, VT %ssupported\n",
		mode[aes], mode[txt], mode[vt]);
}

static void report_mch_info(void)
{
	int i;
	pci_devfn_t dev = PCI_DEV_ROOT;
	uint16_t mchid = get_dev_id(dev);
	const char *mch_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(mch_table); i++) {
		if (mch_table[i].mchid == mchid) {
			mch_type = mch_table[i].name;
			break;
		}
	}

	printk(BIOS_DEBUG, "MCH: device id %04x (rev %02x) is %s\n",
		mchid, get_dev_revision(dev), mch_type);
}

static void report_pch_info(void)
{
	int i;
	pci_devfn_t dev = PCI_DEV_ESPI;
	uint16_t espiid = get_dev_id(dev);
	const char *pch_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].espiid == espiid) {
			pch_type = pch_table[i].name;
			break;
		}
	}
	printk(BIOS_DEBUG, "PCH: device id %04x (rev %02x) is %s\n",
		espiid, get_dev_revision(dev), pch_type);
}

static void report_igd_info(void)
{
	int i;
	pci_devfn_t dev = PCI_DEV_IGD;
	uint16_t igdid = get_dev_id(dev);
	const char *igd_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(igd_table); i++) {
		if (igd_table[i].igdid == igdid) {
			igd_type = igd_table[i].name;
			break;
		}
	}
	printk(BIOS_DEBUG, "IGD: device id %04x (rev %02x) is %s\n",
		igdid, get_dev_revision(dev), igd_type);
}

void report_platform_info(void)
{
	report_cpu_info();
	report_mch_info();
	report_pch_info();
	report_igd_info();
}
