/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <arch/cpu.h>
#include <cpu/intel/microcode.h>
#include <string.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include "haswell.h"

static void report_cpu_info(void)
{
	struct cpuid_result cpuidr;
	u32 i, index, cpu_id, cpu_feature_flag;
	char cpu_string[50], *cpu_name = cpu_string; /* 48 bytes are reported */
	int vt, txt, aes;
	const char *mode[] = {"NOT ", ""};

	index = 0x80000000;
	cpuidr = cpuid(index);
	if (cpuidr.eax < 0x80000004) {
		strcpy(cpu_string, "Platform info not available");
	} else {
		u32 *p = (u32 *)cpu_string;
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

	cpu_id = cpu_get_cpuid();
	printk(BIOS_DEBUG, "CPU id(%x) ucode:%08x %s\n", cpu_id,
		get_current_microcode_rev(), cpu_name);

	cpu_feature_flag = cpu_get_feature_flags_ecx();
	aes = (cpu_feature_flag & CPUID_AES) ? 1 : 0;
	txt = (cpu_feature_flag & CPUID_SMX) ? 1 : 0;
	vt = (cpu_feature_flag & CPUID_VMX) ? 1 : 0;
	printk(BIOS_DEBUG, "AES %ssupported, TXT %ssupported, VT %ssupported\n",
	       mode[aes], mode[txt], mode[vt]);
}

/* The PCI id name match comes from Intel document 472178 */
static struct {
	u16 dev_id;
	const char *dev_name;
} pch_table [] = {
	{0x8c41, "Mobile Engineering Sample"},
	{0x8c42, "Desktop Engineering Sample"},
	{0x8c44, "Z87"},
	{0x8c46, "Z85"},
	{0x8c49, "HM86"},
	{0x8c4a, "H87"},
	{0x8c4b, "HM87"},
	{0x8c4c, "Q85"},
	{0x8c4e, "Q87"},
	{0x8c4f, "QM87"},
	{0x8c50, "B85"},
	{0x8c52, "C222"},
	{0x8c54, "C224"},
	{0x8c56, "C226"},
	{0x8c5c, "H81"},
	{0x9c41, "LP Full Featured Engineering Sample"},
	{0x9c43, "LP Premium"},
	{0x9c45, "LP Mainstream"},
	{0x9c47, "LP Value"},
};

static void report_pch_info(void)
{
	int i;
	u16 dev_id = pci_read_config16(PCH_LPC_DEV, 2);

	const char *pch_type = "Unknown";
	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].dev_id == dev_id) {
			pch_type = pch_table[i].dev_name;
			break;
		}
	}
	printk (BIOS_DEBUG, "PCH type: %s, device id: %x, rev id %x\n",
		pch_type, dev_id, pci_read_config8(PCH_LPC_DEV, 8));
}

void report_platform_info(void)
{
	report_cpu_info();
	report_pch_info();
}
