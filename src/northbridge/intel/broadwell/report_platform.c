/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/haswell/haswell.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>
#include <string.h>

/* FIXME: Needs an update */
static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_HASWELL_A0,       "Haswell A0" },
	{ CPUID_HASWELL_B0,       "Haswell B0" },
	{ CPUID_HASWELL_C0,       "Haswell C0" },
	{ CPUID_HASWELL_ULT_B0,   "Haswell ULT B0" },
	{ CPUID_HASWELL_ULT_C0,   "Haswell ULT C0 or D0" },
	{ CPUID_CRYSTALWELL_C0,   "Haswell Perf Halo" },
	{ CPUID_BROADWELL_ULT_C0, "Broadwell C0" },
	{ CPUID_BROADWELL_ULT_D0, "Broadwell D0" },
	{ CPUID_BROADWELL_ULT_E0, "Broadwell E0 or F0" },
};

static struct {
	u8 revid;
	const char *name;
} mch_rev_table[] = {
	{ MCH_BROADWELL_REV_D0, "Broadwell D0" },
	{ MCH_BROADWELL_REV_E0, "Broadwell E0" },
	{ MCH_BROADWELL_REV_F0, "Broadwell F0" },
};

static struct {
	u16 lpcid;
	const char *name;
} pch_table[] = {
	{ PCH_LPT_LP_SAMPLE,     "LynxPoint LP Sample" },
	{ PCH_LPT_LP_PREMIUM,    "LynxPoint LP Premium" },
	{ PCH_LPT_LP_MAINSTREAM, "LynxPoint LP Mainstream" },
	{ PCH_LPT_LP_VALUE,      "LynxPoint LP Value" },
	{ PCH_WPT_HSW_U_SAMPLE,  "Haswell U Sample" },
	{ PCH_WPT_BDW_U_SAMPLE,  "Broadwell U Sample" },
	{ PCH_WPT_BDW_U_PREMIUM, "Broadwell U Premium" },
	{ PCH_WPT_BDW_U_BASE,    "Broadwell U Base" },
	{ PCH_WPT_BDW_Y_SAMPLE,  "Broadwell Y Sample" },
	{ PCH_WPT_BDW_Y_PREMIUM, "Broadwell Y Premium" },
	{ PCH_WPT_BDW_Y_BASE,    "Broadwell Y Base" },
	{ PCH_WPT_BDW_H,         "Broadwell H" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ IGD_HASWELL_ULT_GT1,     "Haswell ULT GT1" },
	{ IGD_HASWELL_ULT_GT2,     "Haswell ULT GT2" },
	{ IGD_HASWELL_ULT_GT3,     "Haswell ULT GT3" },
	{ IGD_BROADWELL_U_GT1,     "Broadwell U GT1" },
	{ IGD_BROADWELL_U_GT2,     "Broadwell U GT2" },
	{ IGD_BROADWELL_U_GT3_15W, "Broadwell U GT3 (15W)" },
	{ IGD_BROADWELL_U_GT3_28W, "Broadwell U GT3 (28W)" },
	{ IGD_BROADWELL_Y_GT2,     "Broadwell Y GT2" },
	{ IGD_BROADWELL_H_GT2,     "Broadwell U GT2" },
	{ IGD_BROADWELL_H_GT3,     "Broadwell U GT3" },
};

static void report_cpu_info(void)
{
	struct cpuid_result cpuidr;
	u32 i, index, cpu_id, cpu_feature_flag;
	char cpu_string[50], *cpu_name = cpu_string; /* 48 bytes are reported */
	int vt, txt, aes;
	const char *mode[] = {"NOT ", ""};
	const char *cpu_type = "Unknown";

	index = 0x80000000;
	cpuidr = cpuid(index);
	if (cpuidr.eax < 0x80000004) {
		strcpy(cpu_string, "Platform info not available");
	} else {
		u32 *p = (u32 *)cpu_string;
		for (i = 2; i <= 4 ; i++) {
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
	printk(BIOS_DEBUG, "CPU: AES %ssupported, TXT %ssupported, "
	       "VT %ssupported\n", mode[aes], mode[txt], mode[vt]);
}

static void report_mch_info(void)
{
	int i;
	u16 mch_device = pci_read_config16(HOST_BRIDGE, PCI_DEVICE_ID);
	u8 mch_revision = pci_read_config8(HOST_BRIDGE, PCI_REVISION_ID);
	const char *mch_type = "Unknown";

	/* Look for string to match the revision for Broadwell U/Y */
	if (mch_device == MCH_BROADWELL_ID_U_Y) {
		for (i = 0; i < ARRAY_SIZE(mch_rev_table); i++) {
			if (mch_rev_table[i].revid == mch_revision) {
				mch_type = mch_rev_table[i].name;
				break;
			}
		}
	}

	printk(BIOS_DEBUG, "MCH: device id %04x (rev %02x) is %s\n",
	       mch_device, mch_revision, mch_type);
}

static void report_pch_info(void)
{
	int i;
	u16 lpcid = pch_type();
	const char *pch_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].lpcid == lpcid) {
			pch_type = pch_table[i].name;
			break;
		}
	}
	printk(BIOS_DEBUG, "PCH: device id %04x (rev %02x) is %s\n",
	       lpcid, pch_revision(), pch_type);
}

static void report_igd_info(void)
{
	int i;
	u16 igdid = pci_read_config16(SA_DEV_IGD, PCI_DEVICE_ID);
	const char *igd_type = "Unknown";

	for (i = 0; i < ARRAY_SIZE(igd_table); i++) {
		if (igd_table[i].igdid == igdid) {
			igd_type = igd_table[i].name;
			break;
		}
	}
	printk(BIOS_DEBUG, "IGD: device id %04x (rev %02x) is %s\n",
	       igdid, pci_read_config8(SA_DEV_IGD, PCI_REVISION_ID), igd_type);
}

void report_platform_info(void)
{
	report_cpu_info();
	report_mch_info();
	report_pch_info();
	report_igd_info();
}
