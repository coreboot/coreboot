/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"

void dump_capid_values(void)
{
	static const char *const no_yes[] = {
		[0] = "No",
		[1] = "Yes",
	};
	static const char *const cdid_decoder[] = {
		[0] = "Desktop",
		[1] = "Server",
		[2] = "Mobile",
		[3] = "Marketing Spare",
	};
	static const char *const ddrsz_lut[] = {
		[0] = "16",
		[1] = "8",
		[2] = "2",
		[3] = "0.5",
	};
	static const char *const pegfx1_lut[] = {
		[0] = "Unlimited width",
		[1] = "Limited to x1",
	};
	static const char *const dmfc_lut[] = {
		[0] = "Unlimited",
		[1] = "Up to DDR3-2667",
		[2] = "Up to DDR3-2400",
		[3] = "Up to DDR3-2133",
		[4] = "Up to DDR3-1867",
		[5] = "Up to DDR3-1600",
		[6] = "Up to DDR3-1333",
		[7] = "Up to DDR3-1067",
	};
	static const char *const ddd_lut[] = {
		[0] = "Debug",
		[1] = "Production",
	};
	static const char *const pll100_lut[] = {
		[0] = "Not supported",
		[1] = "Up to DDR3-1400",
		[2] = "Up to DDR3-1600",
		[3] = "Up to DDR3-1800",
		[4] = "Up to DDR3-2000",
		[5] = "Up to DDR3-2200",
		[6] = "Up to DDR3-2400",
		[7] = "Unlimited",
	};
	static const char *const occtl_lut[] = {
		[0] = "DSKU",
		[1] = "SSKU",
	};

	const union pci_capid0_a_reg capid0_a = {
		.raw = pci_read_config32(HOST_BRIDGE, CAPID0_A)
	};
	const union pci_capid0_b_reg capid0_b = {
		.raw = pci_read_config32(HOST_BRIDGE, CAPID0_B)
	};
	printk(BIOS_DEBUG, "\n");
	printk(BIOS_DEBUG, "CAPID0_A: 0x%08x\n", capid0_a.raw);
	printk(BIOS_DEBUG, "    DDR3L 1.35V:        %s\n", no_yes[capid0_a.DDR3L_EN]);
	printk(BIOS_DEBUG, "    DDR Write Vref:     %s\n", no_yes[capid0_a.DDR_WRTVREF]);
	printk(BIOS_DEBUG, "    OC enabled (DSKU):  %s\n", no_yes[capid0_a.OC_ENABLED_DSKU]);
	printk(BIOS_DEBUG, "    DDR overclock:      %s\n", no_yes[capid0_a.DDR_OVERCLOCK]);
	printk(BIOS_DEBUG, "    Compatibility RID:  0x%01x\n", capid0_a.CRID);
	printk(BIOS_DEBUG, "    Capability DID:     %s\n", cdid_decoder[capid0_a.CDID]);
	printk(BIOS_DEBUG, "    DID override:       %s\n", no_yes[capid0_a.DIDOE]);
	printk(BIOS_DEBUG, "    Integrated GPU:     %s\n", no_yes[!capid0_a.IGD]);
	printk(BIOS_DEBUG, "    Dual channel:       %s\n", no_yes[!capid0_a.PDCD]);
	printk(BIOS_DEBUG, "    X2APIC support:     %s\n", no_yes[capid0_a.X2APIC_EN]);
	printk(BIOS_DEBUG, "    DIMMs per channel:  %u\n", capid0_a.DDPCD ? 1 : 2);
	printk(BIOS_DEBUG, "    Camarillo device:   %s\n", no_yes[!capid0_a.CDD]);
	printk(BIOS_DEBUG, "    Full ULT info:      %s\n", no_yes[!capid0_a.FUFRD]);
	printk(BIOS_DEBUG, "    DDR 1N mode:        %s\n", no_yes[!capid0_a.D1NM]);
	printk(BIOS_DEBUG, "    PCIe ratio:         %s\n", no_yes[!capid0_a.PCIE_RATIO_DIS]);
	printk(BIOS_DEBUG, "    Max channel size:   %s GiB\n", ddrsz_lut[capid0_a.DDRSZ]);
	printk(BIOS_DEBUG, "    PEG Gen2 support:   %s\n", no_yes[!capid0_a.PEGG2DIS]);
	printk(BIOS_DEBUG, "    DMI Gen2 support:   %s\n", no_yes[!capid0_a.DMIG2DIS]);
	printk(BIOS_DEBUG, "    VT-d support:       %s\n", no_yes[!capid0_a.VTDD]);
	printk(BIOS_DEBUG, "    ECC forced:         %s\n", no_yes[capid0_a.FDEE]);
	printk(BIOS_DEBUG, "    ECC supported:      %s\n", no_yes[!capid0_a.ECCDIS]);
	printk(BIOS_DEBUG, "    DMI width:          x%u\n", capid0_a.DW ? 2 : 4);
	printk(BIOS_DEBUG, "    Width upconfig:     %s\n", no_yes[!capid0_a.PELWUD]);
	printk(BIOS_DEBUG, "    PEG function 0:     %s\n", no_yes[!capid0_a.PEG10D]);
	printk(BIOS_DEBUG, "    PEG function 1:     %s\n", no_yes[!capid0_a.PEG11D]);
	printk(BIOS_DEBUG, "    PEG function 2:     %s\n", no_yes[!capid0_a.PEG12D]);
	printk(BIOS_DEBUG, "    Disp HD audio:      %s\n", no_yes[!capid0_a.DHDAD]);
	printk(BIOS_DEBUG, "\n");
	printk(BIOS_DEBUG, "CAPID0_B: 0x%08x\n", capid0_b.raw);
	printk(BIOS_DEBUG, "    PEG for GFX single: %s\n", pegfx1_lut[capid0_b.SPEGFX1]);
	printk(BIOS_DEBUG, "    PEG for GFX multi:  %s\n", pegfx1_lut[capid0_b.DPEGFX1]);
	printk(BIOS_DEBUG, "    133 MHz ref clock:  %s\n", dmfc_lut[capid0_b.DMFC]);
	printk(BIOS_DEBUG, "    Silicon mode:       %s\n", ddd_lut[capid0_b.DDD]);
	printk(BIOS_DEBUG, "    HDCP capable:       %s\n", no_yes[!capid0_b.HDCPD]);
	printk(BIOS_DEBUG, "    Num PEG lanes:      %u\n", capid0_b.PEGX16D ? 8 : 16);
	printk(BIOS_DEBUG, "    Add. GFX capable:   %s\n", no_yes[!capid0_b.ADDGFXCAP]);
	printk(BIOS_DEBUG, "    Add. GFX enable:    %s\n", no_yes[capid0_b.ADDGFXEN]);
	printk(BIOS_DEBUG, "    CPU Package Type:   %u\n", capid0_b.PKGTYP);
	printk(BIOS_DEBUG, "    PEG Gen3 support:   %s\n", no_yes[!capid0_b.PEGG3_DIS]);
	printk(BIOS_DEBUG, "    100 MHz ref clock:  %s\n", pll100_lut[capid0_b.PLL_REF100_CFG]);
	printk(BIOS_DEBUG, "    Soft Bin capable:   %s\n", no_yes[capid0_b.SOFTBIN]);
	printk(BIOS_DEBUG, "    Cache size:         %u\n", capid0_b.CACHESZ);
	printk(BIOS_DEBUG, "    SMT support:        %s\n", no_yes[capid0_b.SMT]);
	printk(BIOS_DEBUG, "    OC enabled (SSKU):  %s\n", no_yes[capid0_b.OC_ENABLED_SSKU]);
	printk(BIOS_DEBUG, "    OC controlled by:   %s\n", occtl_lut[capid0_b.OC_CTL_DSKU_DIS]);
	printk(BIOS_DEBUG, "\n");
}
