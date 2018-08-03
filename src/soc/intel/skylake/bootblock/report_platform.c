/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <arch/cpu.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/mp_init.h>
#include <soc/bootblock.h>
#include <soc/cpu.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <string.h>

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_SKYLAKE_C0, "Skylake C0" },
	{ CPUID_SKYLAKE_D0, "Skylake D0" },
	{ CPUID_SKYLAKE_HQ0, "Skylake H Q0" },
	{ CPUID_SKYLAKE_HR0, "Skylake H R0" },
	{ CPUID_KABYLAKE_G0, "Kabylake G0" },
	{ CPUID_KABYLAKE_H0, "Kabylake H0" },
	{ CPUID_KABYLAKE_Y0, "Kabylake Y0" },
	{ CPUID_KABYLAKE_HA0, "Kabylake H A0" },
	{ CPUID_KABYLAKE_HB0, "Kabylake H B0" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DEVICE_ID_INTEL_SKL_ID_U, "Skylake-U" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_Y, "Skylake-Y" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_ULX, "Skylake-ULX" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_H, "Skylake-H" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_H_EM, "Skylake-H Embedded" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_U, "Kabylake-U" },
	{ PCI_DEVICE_ID_INTEL_KBL_U_R, "Kabylake-R ULT"},
	{ PCI_DEVICE_ID_INTEL_KBL_ID_Y, "Kabylake-Y" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_H, "Kabylake-H" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_S, "Kabylake-S" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_DT, "Kabylake DT" },
};

static struct {
	u16 lpcid;
	const char *name;
} pch_table[] = {
	{ PCI_DEVICE_ID_INTEL_SPT_LP_SAMPLE, "Skylake LP Sample" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE, "Skylake-U Base" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM, "Skylake-U Premium" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM, "Skylake-Y Premium" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_PREMIUM, "Skylake PCH-H Premium" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_C236, "Skylake PCH-H C236" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_QM170, "Skylake PCH-H QM170" },
	{ PCI_DEVICE_ID_INTEL_KBP_H_Q270, "Kabylake-H Q270" },
	{ PCI_DEVICE_ID_INTEL_KBP_H_H270, "Kabylake-H H270" },
	{ PCI_DEVICE_ID_INTEL_KBP_H_Z270, "Kabylake-H Z270" },
	{ PCI_DEVICE_ID_INTEL_KBP_H_B250, "Kabylake-H B250" },
	{ PCI_DEVICE_ID_INTEL_KBP_H_Q250, "Kabylake-H Q250" },
	{ PCI_DEVICE_ID_INTEL_KBP_LP_U_PREMIUM, "Kabylake-U Premium" },
	{ PCI_DEVICE_ID_INTEL_KBP_LP_Y_PREMIUM, "Kabylake-Y Premium" },
	{ PCI_DEVICE_ID_INTEL_KBP_LP_SUPER_SKU, "Kabylake Super Sku" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM_HDCP22,
			"Kabylake-Y iHDCP 2.2 Premium" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM_HDCP22,
			"Kabylake-U iHDCP 2.2 Premium" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE_HDCP22,
			"Kabylake-U iHDCP 2.2 Base" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DEVICE_ID_INTEL_SKL_GT1_SULTM, "Skylake ULT GT1"},
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_SULXM, "Skylake ULX GT2" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_SULTM, "Skylake ULT GT2" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_SHALM, "Skylake HALO GT2" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT4_SHALM, "Skylake HALO GT4" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT1_SULTM, "Kabylake ULT GT1"},
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SULXM, "Kabylake ULX GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SULTM, "Kabylake ULT GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SULTMR, "Kabylake-R ULT GT2"},
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SHALM, "Kabylake HALO GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_DT2P2, "Kabylake DT GT2" },
	{ PCI_DEVICE_ID_INTEL_AML_GT2_ULX, "Amberlake ULX GT2" },
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
	u32 i, index;
	char cpu_string[50], *cpu_name = cpu_string; /* 48 bytes are reported */
	int vt, txt, aes;
	msr_t microcode_ver;
	static const char *const mode[] = {"NOT ", ""};
	const char *cpu_type = "Unknown";

	index = 0x80000000;
	cpuidr = cpuid(index);
	if (cpuidr.eax < 0x80000004) {
		strcpy(cpu_string, "Platform info not available");
	} else {
		u32 *p = (u32 *) cpu_string;
		for (i = 2; i <= 4; i++) {
			cpuidr = cpuid(index + i);
			*p++ = cpuidr.eax;
			*p++ = cpuidr.ebx;
			*p++ = cpuidr.ecx;
			*p++ = cpuidr.edx;
		}
	}
	/* Skip leading spaces in CPU name string */
	while (cpu_name[0] == ' ')
		cpu_name++;

	microcode_ver.lo = 0;
	microcode_ver.hi = 0;
	wrmsr(0x8B, microcode_ver);
	cpuidr = cpuid(1);
	microcode_ver = rdmsr(0x8b);

	/* Look for string to match the name */
	for (i = 0; i < ARRAY_SIZE(cpu_table); i++) {
		if (cpu_table[i].cpuid == cpuidr.eax) {
			cpu_type = cpu_table[i].name;
			break;
		}
	}

	printk(BIOS_DEBUG, "CPU: %s\n", cpu_name);
	printk(BIOS_DEBUG, "CPU: ID %x, %s, ucode: %08x\n",
	       cpuidr.eax, cpu_type, microcode_ver.hi);

	aes = (cpuidr.ecx & (1 << 25)) ? 1 : 0;
	txt = (cpuidr.ecx & (1 << 6)) ? 1 : 0;
	vt = (cpuidr.ecx & (1 << 5)) ? 1 : 0;
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
	pci_devfn_t dev = PCH_DEV_LPC;
	uint16_t lpcid = get_dev_id(dev);
	const char *pch_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].lpcid == lpcid) {
			pch_type = pch_table[i].name;
			break;
		}
	}
	printk(BIOS_DEBUG, "PCH: device id %04x (rev %02x) is %s\n",
	       lpcid, get_dev_revision(dev), pch_type);
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

/*
 * Dump in the log memory controller configuration as read from the memory
 * controller registers.
 */
void report_memory_config(void)
{
	u32 addr_decoder_common, addr_decode_ch[2];
	int i;

	addr_decoder_common = MCHBAR32(0x5000);
	addr_decode_ch[0] = MCHBAR32(0x5004);
	addr_decode_ch[1] = MCHBAR32(0x5008);

	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (MCHBAR32(0x5e04) * 13333 * 2 + 50)/100);
	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       addr_decoder_common & 3,
	       (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (i = 0; i < ARRAY_SIZE(addr_decode_ch); i++) {
		u32 ch_conf = addr_decode_ch[i];
		printk(BIOS_DEBUG, "memcfg channel[%d] config (%8.8x):\n",
		       i, ch_conf);
		printk(BIOS_DEBUG, "   enhanced interleave mode %s\n",
		       ((ch_conf >> 22) & 1) ? "on" : "off");
		printk(BIOS_DEBUG, "   rank interleave %s\n",
		       ((ch_conf >> 21) & 1) ? "on" : "off");
		printk(BIOS_DEBUG, "   DIMMA %d MB width %s %s rank%s\n",
		       ((ch_conf >> 0) & 0xff) * 256,
		       ((ch_conf >> 19) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 17) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? "" : ", selected");
		printk(BIOS_DEBUG, "   DIMMB %d MB width %s %s rank%s\n",
		       ((ch_conf >> 8) & 0xff) * 256,
		       ((ch_conf >> 19) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 18) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? ", selected" : "");
	}
}
