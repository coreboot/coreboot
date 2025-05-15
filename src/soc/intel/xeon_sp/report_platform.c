/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include <soc/bootblock.h>

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_COOPERLAKE_SP_A0, "Cooper Lake A0" },
	{ CPUID_COOPERLAKE_SP_A1, "Cooper Lake A1" },
	{ CPUID_SKYLAKE_SP_A0_A1, "SkyLake-SP A0/A1" },
	{ CPUID_SKYLAKE_SP_B0,    "SkyLake-SP B0" },
	{ CPUID_SKYLAKE_SP_4,     "SkyLake-SP 4" },
	{ CPUID_SAPPHIRERAPIDS_SP_A,  "Sapphire Rapids A" },
	{ CPUID_SAPPHIRERAPIDS_SP_B,  "Sapphire Rapids B" },
	{ CPUID_SAPPHIRERAPIDS_SP_C,  "Sapphire Rapids C" },
	{ CPUID_SAPPHIRERAPIDS_SP_D,  "Sapphire Rapids D" },
	{ CPUID_SAPPHIRERAPIDS_SP_E0, "Sapphire Rapids E0" },
	{ CPUID_SAPPHIRERAPIDS_SP_E2, "Sapphire Rapids E2" },
	{ CPUID_SAPPHIRERAPIDS_SP_E3, "Sapphire Rapids E3" },
	{ CPUID_SAPPHIRERAPIDS_SP_E4, "Sapphire Rapids E4" },
	{ CPUID_SAPPHIRERAPIDS_SP_Ex, "Sapphire Rapids Ex" },
	{ CPUID_EMERALDRAPIDS, "Emerald Rapids" },
};

static void report_cpu_info(void)
{
	u32 cpu_id, cpu_feature_flag;
	char cpu_name[49];
	int vt, txt, aes;
	static const char *const mode[] = {"NOT ", ""};
	const char *cpu_type = "Unknown";
	size_t i;

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

void report_platform_info(void)
{
	report_cpu_info();
}
