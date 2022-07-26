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
#include <soc/pch.h>
#include <soc/pci_devs.h>

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_CANNONLAKE_A0, "Cannonlake A0" },
	{ CPUID_CANNONLAKE_B0, "Cannonlake B0" },
	{ CPUID_CANNONLAKE_C0, "Cannonlake C0" },
	{ CPUID_CANNONLAKE_D0, "Cannonlake D0" },
	{ CPUID_KABYLAKE_Y0, "Coffeelake D0" },
	{ CPUID_WHISKEYLAKE_V0, "Whiskeylake V0" },
	{ CPUID_WHISKEYLAKE_W0, "Whiskeylake W0" },
	{ CPUID_COFFEELAKE_U0, "Coffeelake U0 (6+2)" },
	{ CPUID_COFFEELAKE_B0, "Coffeelake B0" },
	{ CPUID_COFFEELAKE_P0, "Coffeelake P0" },
	{ CPUID_COFFEELAKE_R0, "Coffeelake R0" },
	{ CPUID_COMETLAKE_U_A0, "Cometlake-U A0 (6+2)" },
	{ CPUID_COMETLAKE_U_K0_S0, "Cometlake-U K0/S0 (6+2)/(4+2)" },
	{ CPUID_COMETLAKE_H_S_6_2_G0, "Cometlake-H/S G0 (6+2)" },
	{ CPUID_COMETLAKE_H_S_6_2_G1, "Cometlake-H/S G1 (6+2)" },
	{ CPUID_COMETLAKE_H_S_10_2_P0, "Cometlake-H/S P0 (10+2)" },
	{ CPUID_COMETLAKE_H_S_10_2_P1, "Cometlake-H/S P1 (10+2)" },
	{ CPUID_COMETLAKE_H_S_10_2_Q0, "Cometlake-H/S Q0 (10+2)" },
};

static struct {
	u16 mchid;
	const char *name;
} mch_table[] = {
	{ PCI_DID_INTEL_CNL_ID_U, "Cannonlake-U" },
	{ PCI_DID_INTEL_CNL_ID_Y, "Cannonlake-Y" },
	{ PCI_DID_INTEL_CFL_ID_U, "Coffeelake U (4+3e)" },
	{ PCI_DID_INTEL_CFL_ID_U_2, "Coffeelake U (2)" },
	{ PCI_DID_INTEL_WHL_ID_W_4, "Whiskeylake W (4+2)" },
	{ PCI_DID_INTEL_WHL_ID_W_2, "Whiskeylake W (2+2)" },
	{ PCI_DID_INTEL_CFL_ID_H, "Coffeelake-H" },
	{ PCI_DID_INTEL_CFL_ID_H_4, "Coffeelake-H (4)" },
	{ PCI_DID_INTEL_CFL_ID_H_8, "Coffeelake-H (8+2)" },
	{ PCI_DID_INTEL_CFL_ID_S, "Coffeelake-S" },
	{ PCI_DID_INTEL_CFL_ID_S_DT_2, "Coffeelake-S DT(2)" },
	{ PCI_DID_INTEL_CFL_ID_S_DT_4, "Coffeelake-S DT(4)" },
	{ PCI_DID_INTEL_CFL_ID_S_DT_8, "Coffeelake-S DT(8+2)" },
	{ PCI_DID_INTEL_CFL_ID_S_WS_4, "Coffeelake-S WS(4+2)" },
	{ PCI_DID_INTEL_CFL_ID_S_WS_6, "Coffeelake-S WS(6+2)" },
	{ PCI_DID_INTEL_CFL_ID_S_WS_8, "Coffeelake-S WS(8+2)" },
	{ PCI_DID_INTEL_CFL_ID_S_S_4, "Coffeelake-S S(4)" },
	{ PCI_DID_INTEL_CFL_ID_S_S_6, "Coffeelake-S S(6)" },
	{ PCI_DID_INTEL_CFL_ID_S_S_8, "Coffeelake-S S(8)" },
	{ PCI_DID_INTEL_CML_ULT, "CometLake-U (4+2)" },
	{ PCI_DID_INTEL_CML_ULT_2_2, "CometLake-U (2+2)" },
	{ PCI_DID_INTEL_CML_ULT_6_2, "CometLake-U (6+2)" },
	{ PCI_DID_INTEL_CML_ULX, "CometLake-ULX (4+2)" },
	{ PCI_DID_INTEL_CML_S, "CometLake-S (6+2)" },
	{ PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2, "CometLake-S G0/G1/P0/P1 (6+2)" },
	{ PCI_DID_INTEL_CML_S_P0P1_8_2, "CometLake-S P0/P1 (8+2)" },
	{ PCI_DID_INTEL_CML_S_P0P1_10_2, "CometLake-S P0/P1 (10+2)" },
	{ PCI_DID_INTEL_CML_S_G0G1_4, "CometLake-S G0/G1 (4+2)" },
	{ PCI_DID_INTEL_CML_S_G0G1_2, "CometLake-S G0/G1 (2+2)" },
	{ PCI_DID_INTEL_CML_H, "CometLake-H (6+2)" },
	{ PCI_DID_INTEL_CML_H_4_2, "CometLake-H (4+2)" },
	{ PCI_DID_INTEL_CML_H_8_2, "CometLake-H (8+2)" },
};

static struct {
	u16 lpcid;
	const char *name;
} pch_table[] = {
	{ PCI_DID_INTEL_CNL_BASE_U_LPC, "Cannonlake-U Base" },
	{ PCI_DID_INTEL_CNL_U_PREMIUM_LPC, "Cannonlake-U Premium" },
	{ PCI_DID_INTEL_CNL_Y_PREMIUM_LPC, "Cannonlake-Y Premium" },
	{ PCI_DID_INTEL_CNP_H_LPC_H310, "Cannonlake-H H310" },
	{ PCI_DID_INTEL_CNP_H_LPC_H370, "Cannonlake-H H370" },
	{ PCI_DID_INTEL_CNP_H_LPC_Z390, "Cannonlake-H Z390" },
	{ PCI_DID_INTEL_CNP_H_LPC_Q370, "Cannonlake-H Q370" },
	{ PCI_DID_INTEL_CNP_H_LPC_B360, "Cannonlake-H B360" },
	{ PCI_DID_INTEL_CNP_H_LPC_C246, "Cannonlake-H C246" },
	{ PCI_DID_INTEL_CNP_H_LPC_C242, "Cannonlake-H C242" },
	{ PCI_DID_INTEL_CNP_H_LPC_QM370, "Cannonlake-H QM370" },
	{ PCI_DID_INTEL_CNP_H_LPC_HM370, "Cannonlake-H HM370" },
	{ PCI_DID_INTEL_CNP_H_LPC_CM246, "Cannonlake-H CM246" },
	{ PCI_DID_INTEL_CMP_SUPER_U_LPC, "Cometlake-U Super" },
	{ PCI_DID_INTEL_CMP_PREMIUM_Y_LPC, "Cometlake-Y Premium" },
	{ PCI_DID_INTEL_CMP_PREMIUM_U_LPC, "Cometlake-U Premium" },
	{ PCI_DID_INTEL_CMP_BASE_U_LPC, "Cometlake-U Base" },
	{ PCI_DID_INTEL_CMP_SUPER_Y_LPC, "Cometlake-Y Super" },
	{ PCI_DID_INTEL_CMP_H_LPC_HM470, "Cometlake-H HM470" },
	{ PCI_DID_INTEL_CMP_H_LPC_WM490, "Cometlake-H WM490" },
	{ PCI_DID_INTEL_CMP_H_LPC_QM480, "Cometlake-H QM480" },
	{ PCI_DID_INTEL_CMP_H_LPC_W480, "Cometlake-H W480" },
	{ PCI_DID_INTEL_CMP_H_LPC_H470, "Cometlake-H H470" },
	{ PCI_DID_INTEL_CMP_H_LPC_Z490, "Cometlake-H Z490" },
	{ PCI_DID_INTEL_CMP_H_LPC_Q470, "Cometlake-H Q470" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ PCI_DID_INTEL_CNL_GT2_ULX_1, "Cannonlake ULX GT2" },
	{ PCI_DID_INTEL_CNL_GT2_ULX_2, "Cannonlake ULX GT1.5" },
	{ PCI_DID_INTEL_CNL_GT2_ULX_3, "Cannonlake ULX GT1" },
	{ PCI_DID_INTEL_CNL_GT2_ULX_4, "Cannonlake ULX GT0.5" },
	{ PCI_DID_INTEL_CNL_GT2_ULT_1, "Cannonlake ULT GT2" },
	{ PCI_DID_INTEL_CNL_GT2_ULT_2, "Cannonlake ULT GT1.5" },
	{ PCI_DID_INTEL_CNL_GT2_ULT_3, "Cannonlake ULT GT1" },
	{ PCI_DID_INTEL_CNL_GT2_ULT_4, "Cannonlake ULT GT0.5" },
	{ PCI_DID_INTEL_CFL_GT2_ULT, "Coffeelake ULT GT2" },
	{ PCI_DID_INTEL_WHL_GT1_ULT_1, "Whiskeylake ULT GT1" },
	{ PCI_DID_INTEL_WHL_GT2_ULT_1, "Whiskeylake ULT GT2" },
	{ PCI_DID_INTEL_CFL_H_GT2, "Coffeelake-H GT2" },
	{ PCI_DID_INTEL_CFL_H_XEON_GT2, "Coffeelake-H Xeon GT2" },
	{ PCI_DID_INTEL_CFL_S_GT1_1, "Coffeelake-S GT1" },
	{ PCI_DID_INTEL_CFL_S_GT1_2, "Coffeelake-S GT1" },
	{ PCI_DID_INTEL_CFL_S_GT2_1, "Coffeelake-S GT2" },
	{ PCI_DID_INTEL_CFL_S_GT2_2, "Coffeelake-S GT2" },
	{ PCI_DID_INTEL_CFL_S_GT2_3, "Coffeelake-S GT2" },
	{ PCI_DID_INTEL_CFL_S_GT2_4, "Coffeelake-S GT2" },
	{ PCI_DID_INTEL_CFL_S_GT2_5, "Coffeelake-S GT2" },
	{ PCI_DID_INTEL_CML_GT1_ULT_1, "CometLake ULT GT1" },
	{ PCI_DID_INTEL_CML_GT1_ULT_2, "CometLake ULT GT1" },
	{ PCI_DID_INTEL_CML_GT2_ULT_1, "CometLake ULT GT2" },
	{ PCI_DID_INTEL_CML_GT2_ULT_2, "CometLake ULT GT2" },
	{ PCI_DID_INTEL_CML_GT1_ULT_3, "CometLake ULT GT1" },
	{ PCI_DID_INTEL_CML_GT1_ULT_4, "CometLake ULT GT1" },
	{ PCI_DID_INTEL_CML_GT2_ULT_3, "CometLake ULT GT2" },
	{ PCI_DID_INTEL_CML_GT2_ULT_4, "CometLake ULT GT2" },
	{ PCI_DID_INTEL_CML_GT2_ULT_5, "CometLake ULT GT2" },
	{ PCI_DID_INTEL_CML_GT2_ULT_6, "CometLake ULT GT2" },
	{ PCI_DID_INTEL_CML_GT1_ULX_1, "CometLake ULX GT1" },
	{ PCI_DID_INTEL_CML_GT2_ULX_1, "CometLake ULX GT2" },
	{ PCI_DID_INTEL_CML_GT1_S_1, "CometLake S GT1" },
	{ PCI_DID_INTEL_CML_GT1_S_2, "CometLake S GT1" },
	{ PCI_DID_INTEL_CML_GT2_S_1, "CometLake S GT2" },
	{ PCI_DID_INTEL_CML_GT2_S_2, "CometLake S GT2" },
	{ PCI_DID_INTEL_CML_GT2_S_G0, "CometLake S GT2 G0" },
	{ PCI_DID_INTEL_CML_GT2_S_P0, "CometLake S GT2 P0" },
	{ PCI_DID_INTEL_CML_GT1_H_1, "CometLake H GT1" },
	{ PCI_DID_INTEL_CML_GT1_H_2, "CometLake H GT1" },
	{ PCI_DID_INTEL_CML_GT2_H_1, "CometLake H GT2" },
	{ PCI_DID_INTEL_CML_GT2_H_2, "CometLake H GT2" },
	{ PCI_DID_INTEL_CML_GT2_H_R0, "CometLake H GT2 R0" },
	{ PCI_DID_INTEL_CML_GT2_H_R1, "CometLake H GT2 R1" },

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
