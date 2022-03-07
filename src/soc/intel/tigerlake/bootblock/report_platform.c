/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Tiger Lake Platform Stepping and IDs
 * Document number: 605534
 * Chapter number: 2, 4, 5, 6
 */

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
	{ CPUID_TIGERLAKE_A0, "Tigerlake A0" },
	{ CPUID_TIGERLAKE_B0, "Tigerlake B0" },
	{ CPUID_TIGERLAKE_R0, "Tigerlake R0" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DID_INTEL_TGL_ID_U_2_2, "Tigerlake-U-2-2" },
	{ PCI_DID_INTEL_TGL_ID_U_4_2, "Tigerlake-U-4-2" },
	{ PCI_DID_INTEL_TGL_ID_Y_2_2, "Tigerlake-Y-2-2" },
	{ PCI_DID_INTEL_TGL_ID_Y_4_2, "Tigerlake-Y-4-2" },
	{ PCI_DID_INTEL_TGL_ID_H_6_1, "Tigerlake-H-6-1" },
	{ PCI_DID_INTEL_TGL_ID_H_8_1, "Tigerlake-H-8-1" },
};

static struct {
	u16 espiid;
	const char *name;
} pch_table[] = {
	{ PCI_DID_INTEL_TGP_ESPI_0, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_SUPER_U_ESPI, "Tigerlake-U Super SKU" },
	{ PCI_DID_INTEL_TGP_PREMIUM_U_ESPI, "Tigerlake-U Premium SKU" },
	{ PCI_DID_INTEL_TGP_BASE_U_ESPI, "Tigerlake-U Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_1, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_2, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_SUPER_Y_ESPI, "Tigerlake-Y Super SKU" },
	{ PCI_DID_INTEL_TGP_PREMIUM_Y_ESPI, "Tigerlake-Y Premium SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_3, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_4, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_5, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_6, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_7, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_8, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_9, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_10, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_11, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_12, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_13, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_14, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_15, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_16, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_17, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_18, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_19, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_20, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_21, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_22, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_23, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_24, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_25, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_ESPI_26, "Tigerlake-Base SKU" },
	{ PCI_DID_INTEL_TGP_H_ESPI_B560, "Tigerlake-H B560" },
	{ PCI_DID_INTEL_TGP_H_ESPI_H510, "Tigerlake-H H510" },
	{ PCI_DID_INTEL_TGP_H_ESPI_H570, "Tigerlake-H H570" },
	{ PCI_DID_INTEL_TGP_H_ESPI_Q570, "Tigerlake-H Q570" },
	{ PCI_DID_INTEL_TGP_H_ESPI_W580, "Tigerlake-H W580" },
	{ PCI_DID_INTEL_TGP_H_ESPI_Z590, "Tigerlake-H Z590" },
	{ PCI_DID_INTEL_TGP_H_ESPI_HM570, "Tigerlake-H HM570" },
	{ PCI_DID_INTEL_TGP_H_ESPI_QM580, "Tigerlake-H QM580" },
	{ PCI_DID_INTEL_TGP_H_ESPI_WM590, "Tigerlake-H WM590" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DID_INTEL_TGL_GT0, "Tigerlake U GT0" },
	{ PCI_DID_INTEL_TGL_GT1_H_32, "Tigerlake H GT1 32EU" },
	{ PCI_DID_INTEL_TGL_GT1_H_16, "Tigerlake H GT1 16EU" },
	{ PCI_DID_INTEL_TGL_GT2_ULT, "Tigerlake U GT2" },
	{ PCI_DID_INTEL_TGL_GT2_ULX, "Tigerlake Y GT2" },
	{ PCI_DID_INTEL_TGL_GT3_ULT, "Tigerlake U GT3" },
	{ PCI_DID_INTEL_TGL_GT2_ULT_1, "Tigerlake U GT2 1" },
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
