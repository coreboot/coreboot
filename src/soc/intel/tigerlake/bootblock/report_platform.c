/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * This file is created based on Intel Tiger Lake Platform Stepping and IDs
 * Document number: 605534
 * Chapter number: 2, 4, 5, 6
 */

#include <arch/cpu.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/mp_init.h>
#include <soc/bootblock.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <string.h>

#define BIOS_SIGN_ID	0x8B

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_TIGERLAKE_A0, "Tigerlake A0" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DEVICE_ID_INTEL_TGL_ID_U, "Tigerlake-U-4-2" },
	{ PCI_DEVICE_ID_INTEL_TGL_ID_U_1, "Tigerlake-U-4-3e" },
	{ PCI_DEVICE_ID_INTEL_TGL_ID_Y, "Tigerlake-Y-4-2" },
	{ PCI_DEVICE_ID_INTEL_JSL_PRE_PROD, "Jasperlake Pre Prod" },
	{ PCI_DEVICE_ID_INTEL_JSL_EHL, "Jasperlake Elkhartlake" },
	{ PCI_DEVICE_ID_INTEL_EHL_ID_1, "Elkhartlake-1" },
};

static struct {
	u16 espiid;
	const char *name;
} pch_table[] = {
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_0, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_SUPER_U_ESPI, "Tigerlake-U Super SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_PREMIUM_U_ESPI, "Tigerlake-U Premium SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_BASE_U_ESPI, "Tigerlake-U Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_1, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_2, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_SUPER_Y_ESPI, "Tigerlake-Y Super SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_PREMIUM_Y_ESPI, "Tigerlake-Y Premium SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_3, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_4, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_5, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_6, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_7, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_8, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_9, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_10, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_11, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_12, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_13, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_14, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_15, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_16, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_17, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_18, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_19, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_20, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_21, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_22, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_23, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_24, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_25, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_TGP_ESPI_26, "Tigerlake-Base SKU" },
	{ PCI_DEVICE_ID_INTEL_JSP_PRE_PROD_ESPI_1, "Jasperlake Pre Prod" },
	{ PCI_DEVICE_ID_INTEL_JSP_PRE_PROD_ESPI_2, "Jasperlake Pre Prod" },
	{ PCI_DEVICE_ID_INTEL_MCC_ESPI_0, "Elkhartlake-0" },
	{ PCI_DEVICE_ID_INTEL_MCC_ESPI_1, "Elkhartlake-1" },
	{ PCI_DEVICE_ID_INTEL_MCC_BASE_ESPI, "Elkhartlake Base" },
	{ PCI_DEVICE_ID_INTEL_MCC_PREMIUM_ESPI, "Elkhartlake Premium" },
	{ PCI_DEVICE_ID_INTEL_MCC_SUPER_ESPI, "Elkhartlake Super" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DEVICE_ID_INTEL_TGL_GT0, "Tigerlake U GT0" },
	{ PCI_DEVICE_ID_INTEL_TGL_GT2_ULT, "Tigerlake U GT2" },
	{ PCI_DEVICE_ID_INTEL_TGL_GT2_ULX, "Tigerlake Y GT2" },
	{ PCI_DEVICE_ID_INTEL_TGL_GT3_ULT, "Tigerlake U GT3" },
	{ PCI_DEVICE_ID_INTEL_JSL_PRE_PROD_GT0, "Jasperlake Pre Prod GT0" },
	{ PCI_DEVICE_ID_INTEL_EHL_GT1_1, "Elkhartlake GT1 1" },
	{ PCI_DEVICE_ID_INTEL_EHL_GT2_1, "Elkhartlake GT2 1" },
	{ PCI_DEVICE_ID_INTEL_EHL_GT1_2, "Elkhartlake GT1 2" },
	{ PCI_DEVICE_ID_INTEL_EHL_GT2_2, "Elkhartlake GT2 2" },
	{ PCI_DEVICE_ID_INTEL_EHL_GT1_3, "Elkhartlake GT1 3" },
	{ PCI_DEVICE_ID_INTEL_EHL_GT2_3, "Elkhartlake GT2 3" },
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
	msr_t microcode_ver;
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

	microcode_ver.lo = 0;
	microcode_ver.hi = 0;
	wrmsr(BIOS_SIGN_ID, microcode_ver);
	cpu_id = cpu_get_cpuid();
	microcode_ver = rdmsr(BIOS_SIGN_ID);

	/* Look for string to match the name */
	for (i = 0; i < ARRAY_SIZE(cpu_table); i++) {
		if (cpu_table[i].cpuid == cpu_id) {
			cpu_type = cpu_table[i].name;
			break;
		}
	}

	printk(BIOS_DEBUG, "CPU: %s\n", cpu_name);
	printk(BIOS_DEBUG, "CPU: ID %x, %s, ucode: %08x\n",
	       cpu_id, cpu_type, microcode_ver.hi);

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
