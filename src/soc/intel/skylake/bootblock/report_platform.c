/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/mp_init.h>
#include <soc/bootblock.h>
#include <soc/cpu.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

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
	{ PCI_DEVICE_ID_INTEL_SKL_ID_H_4, "Skylake-H (4 Core)" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_H_EM, "Skylake-H Embedded" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_H_2, "Skylake-H (2 Core)" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_S_2, "Skylake-S (2 Core)" },
	{ PCI_DEVICE_ID_INTEL_SKL_ID_S_4, "Skylake-S (4 Core) / Skylake-DT" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_U, "Kabylake-U" },
	{ PCI_DEVICE_ID_INTEL_KBL_U_R, "Kabylake-R ULT"},
	{ PCI_DEVICE_ID_INTEL_KBL_ID_Y, "Kabylake-Y" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_H, "Kabylake-H" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_S, "Kabylake-S" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_DT, "Kabylake DT" },
	{ PCI_DEVICE_ID_INTEL_KBL_ID_DT_2, "Kabylake DT 2" },
};

static struct {
	u16 lpcid;
	const char *name;
} pch_table[] = {
	{ PCI_DEVICE_ID_INTEL_SPT_LP_SAMPLE, "Skylake LP Sample" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE, "Skylake-U Base" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_U_PREMIUM, "Skylake-U Premium" },
	{ PCI_DEVICE_ID_INTEL_SPT_LP_Y_PREMIUM, "Skylake-Y Premium" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_H110, "Skylake PCH-H H110" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_H170, "Skylake PCH-H H170" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_Z170, "Skylake PCH-H Z170" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_Q170, "Skylake PCH-H Q170" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_Q150, "Skylake PCH-H Q150" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_B150, "Skylake PCH-H B150" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_C236, "Skylake PCH-H C236" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_C232, "Skylake PCH-H C232" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_QM170, "Skylake PCH-H QM170" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_HM170, "Skylake PCH-H HM170" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_CM236, "Skylake PCH-H CM236" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_HM175, "Skylake PCH-H HM175" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_QM175, "Skylake PCH-H QM175" },
	{ PCI_DEVICE_ID_INTEL_SPT_H_CM238, "Skylake PCH-H CM238" },
	{ PCI_DEVICE_ID_INTEL_LWB_C621, "Lewisburg PCH C621" },
	{ PCI_DEVICE_ID_INTEL_LWB_C622, "Lewisburg PCH C622" },
	{ PCI_DEVICE_ID_INTEL_LWB_C624, "Lewisburg PCH C624" },
	{ PCI_DEVICE_ID_INTEL_LWB_C625, "Lewisburg PCH C625" },
	{ PCI_DEVICE_ID_INTEL_LWB_C626, "Lewisburg PCH C626" },
	{ PCI_DEVICE_ID_INTEL_LWB_C627, "Lewisburg PCH C627" },
	{ PCI_DEVICE_ID_INTEL_LWB_C628, "Lewisburg PCH C628" },
	{ PCI_DEVICE_ID_INTEL_LWB_C629, "Lewisburg PCH C629" },
	{ PCI_DEVICE_ID_INTEL_LWB_C624_SUPER, "Lewisburg PCH C624 Super SKU" },
	{ PCI_DEVICE_ID_INTEL_LWB_C627_SUPER_1, "Lewisburg PCH C627 Super SKU" },
	{ PCI_DEVICE_ID_INTEL_LWB_C621_SUPER, "Lewisburg PCH C621 Super SKU" },
	{ PCI_DEVICE_ID_INTEL_LWB_C627_SUPER_2, "Lewisburg PCH C627 Super SKU" },
	{ PCI_DEVICE_ID_INTEL_LWB_C628_SUPER, "Lewisburg PCH C628 Super SKU" },
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
	{ PCI_DEVICE_ID_INTEL_SKL_GT1F_DT2, "Skylake DT GT1F" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT1_SULTM, "Skylake ULT GT1" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT1F_SHALM, "Skylake HALO GT1F" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_DT2P1, "Skylake DT GT2" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_SULTM, "Skylake ULT GT2" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_SHALM, "Skylake HALO GT2" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_SWKSM, "Skylake Mobile Xeon GT2"},
	{ PCI_DEVICE_ID_INTEL_SKL_GT2_SULXM, "Skylake ULX GT2" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT3_SULTM, "Skylake ULT GT3" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT3E_SULTM_1, "Skylake ULT (15W) GT3E" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT3E_SULTM_2, "Skylake ULT (28W) GT3E" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT3FE_SSRVM, "Skylake Media Server GT3FE" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT4_SHALM, "Skylake HALO GT4" },
	{ PCI_DEVICE_ID_INTEL_SKL_GT4E_SWSTM, "Skylake Workstation GT4E" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT1F_DT2, "Kaby Lake DT GT1F" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT1_SULTM, "Kaby Lake ULT GT1" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT1_SHALM_1, "Kaby Lake HALO GT1" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT1_SHALM_2, "Kaby Lake HALO GT1" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT1_SSRVM, "Kaby Lake SRV GT1" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SSRVM, "Kaby Lake Media Server GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SWSTM, "Kaby Lake Workstation GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SULXM, "Kaby Lake ULX GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SULTM, "Kaby Lake ULT GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SULTMR, "Kaby Lake-R ULT GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_SHALM, "Kaby Lake HALO GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2_DT2P2, "Kaby Lake DT GT2" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT2F_SULTM, "Kaby Lake ULT GT2F" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT3E_SULTM_1, "Kaby Lake ULT (15W) GT3E" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT3E_SULTM_2, "Kaby Lake ULT (28W) GT3E" },
	{ PCI_DEVICE_ID_INTEL_KBL_GT4_SHALM, "Kaby Lake HALO GT4" },
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
