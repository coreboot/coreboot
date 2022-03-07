/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/bootblock.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>

static struct {
	uint32_t cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_ELKHARTLAKE_A0, "Elkhartlake A0" },
	{ CPUID_ELKHARTLAKE_B0, "Elkhartlake B0/B1" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DID_INTEL_EHL_ID_0, "Elkhartlake SKU-0" },
	{ PCI_DID_INTEL_EHL_ID_1, "Elkhartlake SKU-1" },
	{ PCI_DID_INTEL_EHL_ID_1A, "Elkhartlake SKU-1A" },
	{ PCI_DID_INTEL_EHL_ID_2, "Elkhartlake SKU-2" },
	{ PCI_DID_INTEL_EHL_ID_2_1, "Elkhartlake SKU-2" },
	{ PCI_DID_INTEL_EHL_ID_3, "Elkhartlake SKU-3" },
	{ PCI_DID_INTEL_EHL_ID_3A, "Elkhartlake SKU-3A" },
	{ PCI_DID_INTEL_EHL_ID_4, "Elkhartlake SKU-4" },
	{ PCI_DID_INTEL_EHL_ID_5, "Elkhartlake SKU-5" },
	{ PCI_DID_INTEL_EHL_ID_6, "Elkhartlake SKU-6" },
	{ PCI_DID_INTEL_EHL_ID_7, "Elkhartlake SKU-7" },
	{ PCI_DID_INTEL_EHL_ID_8, "Elkhartlake SKU-8" },
	{ PCI_DID_INTEL_EHL_ID_9, "Elkhartlake SKU-9" },
	{ PCI_DID_INTEL_EHL_ID_10, "Elkhartlake SKU-10" },
	{ PCI_DID_INTEL_EHL_ID_11, "Elkhartlake SKU-11" },
	{ PCI_DID_INTEL_EHL_ID_12, "Elkhartlake SKU-12" },
	{ PCI_DID_INTEL_EHL_ID_13, "Elkhartlake SKU-13" },
	{ PCI_DID_INTEL_EHL_ID_14, "Elkhartlake SKU-14" },
	{ PCI_DID_INTEL_EHL_ID_15, "Elkhartlake SKU-15" },
};

static struct {
	u16 espiid;
	const char *name;
} pch_table[] = {
	{ PCI_DID_INTEL_MCC_ESPI_0, "Elkhartlake-0" },
	{ PCI_DID_INTEL_MCC_ESPI_1, "Elkhartlake-1" },
	{ PCI_DID_INTEL_MCC_BASE_ESPI, "Elkhartlake Base" },
	{ PCI_DID_INTEL_MCC_PREMIUM_ESPI, "Elkhartlake Premium" },
	{ PCI_DID_INTEL_MCC_SUPER_ESPI, "Elkhartlake Super" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DID_INTEL_EHL_GT1_1, "Elkhartlake GT1-1" },
	{ PCI_DID_INTEL_EHL_GT2_1, "Elkhartlake GT2-1" },
	{ PCI_DID_INTEL_EHL_GT1_2, "Elkhartlake GT1-2" },
	{ PCI_DID_INTEL_EHL_GT1_2_1, "Elkhartlake GT1-2-1" },
	{ PCI_DID_INTEL_EHL_GT2_2, "Elkhartlake GT2-2" },
	{ PCI_DID_INTEL_EHL_GT1_3, "Elkhartlake GT1-3" },
	{ PCI_DID_INTEL_EHL_GT2_3, "Elkhartlake GT2-3" },
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
	uint32_t i, cpu_id, cpu_feature_flag;
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
	aes = (cpu_feature_flag & CPUID_AES) ? 1 : 0;
	txt = (cpu_feature_flag & CPUID_SMX) ? 1 : 0;
	vt = (cpu_feature_flag & CPUID_VMX) ? 1 : 0;
	printk(BIOS_DEBUG,
		"CPU: AES %ssupported, TXT %ssupported, VT %ssupported\n",
		mode[aes], mode[txt], mode[vt]);
}

static void report_mch_info(void)
{
	int i;
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

	printk(BIOS_DEBUG, "MCH: device id %04x (rev %02x) is %s\n",
		mchid, mch_revision, mch_type);
}

static void report_pch_info(void)
{
	int i;
	pci_devfn_t dev = PCH_DEV_ESPI;
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
	pci_devfn_t dev = SA_DEV_IGD;
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
