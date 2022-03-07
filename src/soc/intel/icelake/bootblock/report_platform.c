/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/bootblock.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <string.h>

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_ICELAKE_A0, "Icelake A0" },
	{ CPUID_ICELAKE_B0, "Icelake B0" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DID_INTEL_ICL_ID_U, "Icelake-U" },
	{ PCI_DID_INTEL_ICL_ID_U_2_2, "Icelake-U-2-2" },
	{ PCI_DID_INTEL_ICL_ID_Y, "Icelake-Y" },
	{ PCI_DID_INTEL_ICL_ID_Y_2, "Icelake-Y-2" },
};

static struct {
	u16 espiid;
	const char *name;
} pch_table[] = {
	{ PCI_DID_INTEL_ICL_BASE_U_ESPI, "Icelake-U Base" },
	{ PCI_DID_INTEL_ICL_BASE_Y_ESPI, "Icelake-Y Base" },
	{ PCI_DID_INTEL_ICL_U_PREMIUM_ESPI, "Icelake-U Premium" },
	{ PCI_DID_INTEL_ICL_U_SUPER_U_ESPI, "Icelake-U Super" },
	{ PCI_DID_INTEL_ICL_U_SUPER_U_ESPI_REV0, "Icelake-U Super REV0" },
	{ PCI_DID_INTEL_ICL_SUPER_Y_ESPI, "Icelake-Y Super" },
	{ PCI_DID_INTEL_ICL_Y_PREMIUM_ESPI, "Icelake-Y Premium" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DID_INTEL_ICL_GT0_ULT, "Icelake ULT GT0" },
	{ PCI_DID_INTEL_ICL_GT0_5_ULT, "Icelake ULT GT0.5" },
	{ PCI_DID_INTEL_ICL_GT1_ULT, "Icelake U GT1" },
	{ PCI_DID_INTEL_ICL_GT2_ULX_0, "Icelake Y GT2" },
	{ PCI_DID_INTEL_ICL_GT2_ULX_1, "Icelake Y GT2_1" },
	{ PCI_DID_INTEL_ICL_GT2_ULT_1, "Icelake U GT2_1" },
	{ PCI_DID_INTEL_ICL_GT2_ULX_2, "Icelake Y GT2_2" },
	{ PCI_DID_INTEL_ICL_GT2_ULT_2, "Icelake U GT2_2" },
	{ PCI_DID_INTEL_ICL_GT2_ULX_3, "Icelake Y GT2_3" },
	{ PCI_DID_INTEL_ICL_GT2_ULT_3, "Icelake U GT2_3" },
	{ PCI_DID_INTEL_ICL_GT2_ULX_4, "Icelake Y GT2_4" },
	{ PCI_DID_INTEL_ICL_GT2_ULT_4, "Icelake U GT2_4" },
	{ PCI_DID_INTEL_ICL_GT2_ULX_5, "Icelake Y GT2_5" },
	{ PCI_DID_INTEL_ICL_GT2_ULT_5, "Icelake U GT2_5" },
	{ PCI_DID_INTEL_ICL_GT3_ULT, "Icelake U GT3" },
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
	struct cpuid_result cpuidr;
	u32 i, index, cpu_id, cpu_feature_flag;
	const char cpu_not_found[] = "Platform info not available";
	const char *cpu_name = cpu_not_found; /* 48 bytes are reported */
	int vt, txt, aes;
	static const char *const mode[] = {"NOT ", ""};
	const char *cpu_type = "Unknown";
	u32 p[13];

	index = 0x80000000;
	cpuidr = cpuid(index);
	if (cpuidr.eax >= 0x80000004) {
		int j = 0;

		for (i = 2; i <= 4; i++) {
			cpuidr = cpuid(index + i);
			p[j++] = cpuidr.eax;
			p[j++] = cpuidr.ebx;
			p[j++] = cpuidr.ecx;
			p[j++] = cpuidr.edx;
		}
		p[12] = 0;
		cpu_name = (char *)p;

		/* Skip leading spaces in CPU name string */
		while (cpu_name[0] == ' ' && strlen(cpu_name) > 0)
			cpu_name++;
	}

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
