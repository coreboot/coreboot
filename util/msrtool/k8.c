/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2009 Marc Jones <marcj303@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "msrtool.h"

int k8_probe(const struct targetdef *target, const struct cpuid_t *id) {
	return (VENDOR_AMD == id->vendor) && (0xF == id->family);
}

/*
 * AMD BKDG Publication # 32559 Revision: 3.08 Issue Date: July 2007
 */
const struct msrdef k8_msrs[] = {
	{ 0xC0000080, MSRTYPE_RDWR, MSR2(0, 0), "EFER Register", "Extended Feature Enable Register", {
		{ 63, 32, RESERVED },
		{ 31, 18, RESERVED },
		{ 14, 1, "FFXSR:", "Fast FXSAVE/FRSTOR Enable", PRESENT_DEC, {
			{ MSR1(0), "FXSAVE/FRSTOR disabled" },
			{ MSR1(1), "FXSAVE/FRSTOR enabled" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, "LMSLE:", "Long Mode Segment Limit Enable", PRESENT_DEC, {
			{ MSR1(0), "Long mode segment limit check disabled" },
			{ MSR1(1), "Long mode segment limit check enabled" },
			{ BITVAL_EOT }
		}},
		{ 12, 1, "SVME:", "SVM Enable", PRESENT_DEC, {
			{ MSR1(0), "SVM features disabled" },
			{ MSR1(1), "SVM features enabled" },
			{ BITVAL_EOT }
		}},
		{ 11, 1, "NXE:", "No-Execute Page Enable", PRESENT_DEC, {
			{ MSR1(0), "NXE features disabled" },
			{ MSR1(1), "NXE features enabled" },
			{ BITVAL_EOT }
		}},
		{ 10, 1, "LMA:", "Long Mode Active", PRESENT_DEC, {
			{ MSR1(0), "Long Mode feature not active" },
			{ MSR1(1), "Long Mode feature active" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, RESERVED },
		{ 8, 1, "LME:", "Long Mode Enable", PRESENT_DEC, {
			{ MSR1(0), "Long Mode feature disabled" },
			{ MSR1(1), "Long Mode feature enabled" },
			{ BITVAL_EOT }
		}},
		{ 7, 7, RESERVED },
		{ 0, 1, "SYSCALL:", "System Call Extension Enable", PRESENT_DEC, {
			{ MSR1(0), "System Call feature disabled" },
			{ MSR1(1), "System Call feature enabled" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},

	{ 0xC0010010, MSRTYPE_RDWR, MSR2(0, 0), "SYSCFG Register", "This register controls the system configuration", {
		{ 63, 32, RESERVED },
		{ 31, 9, RESERVED },
		{ 22, 1, "Tom2ForceMemTypeWB:", "Top of Memory 2 Memory Type Write Back", PRESENT_DEC, {
			{ MSR1(0), "Tom2ForceMemTypeWB disabled" },
			{ MSR1(1), "Tom2ForceMemTypeWB enabled" },
			{ BITVAL_EOT }
		}},
		{ 21, 1, "MtrrTom2En:", "Top of Memory Address Register 2 Enable", PRESENT_DEC, {
			{ MSR1(0), "MtrrTom2En disabled" },
			{ MSR1(1), "MtrrTom2En enabled" },
			{ BITVAL_EOT }
		}},
		{ 20, 1, "MtrrVarDramEn:", "Top of Memory Address Register and I/O Range Register Enable", PRESENT_DEC, {
			{ MSR1(0), "MtrrVarDramEn disabled" },
			{ MSR1(1), "MtrrVarDramEn enabled" },
			{ BITVAL_EOT }
		}},
		{ 19, 1, "MtrrFixDramModEn:", "RdDram and WrDram Bits Modification Enable", PRESENT_DEC, {
			{ MSR1(0), "MtrrFixDramModEn disabled" },
			{ MSR1(1), "MtrrFixDramModEn enabled" },
			{ BITVAL_EOT }
		}},
		{ 18, 1, "MtrrFixDramEn:", "Fixed RdDram and WrDram Attributes Enable", PRESENT_DEC, {
			{ MSR1(0), "MtrrFixDramEn disabled" },
			{ MSR1(1), "MtrrFixDramEn enabled" },
			{ BITVAL_EOT }
		}},
		{ 17, 1, "SysUcLockEn:", "System Interface Lock Command Enable", PRESENT_DEC, {
			{ MSR1(0), "SysUcLockEn disabled" },
			{ MSR1(1), "SysUcLockEn enabled" },
			{ BITVAL_EOT }
		}},
		{ 16, 1, "ChxToDirtyDis:", "Change to Dirty Command Disable", PRESENT_DEC, {
			{ MSR1(0), "ChxToDirtyDis disabled" },
			{ MSR1(1), "ChxToDirtyDis enabled" },
			{ BITVAL_EOT }
		}},
		{ 15, 5, RESERVED },
		{ 10, 1, "SetDirtyEnO:", "SharedToDirty Command for O->M State Transition Enable", PRESENT_DEC, {
			{ MSR1(0), "SetDirtyEnO disabled" },
			{ MSR1(1), "SetDirtyEnO enabled" },
			{ BITVAL_EOT }
		}},
		{ 9, 1, "SetDirtyEnS:", "SharedToDirty Command for S->M State Transition Enable", PRESENT_DEC, {
			{ MSR1(0), "SetDirtyEnS disabled" },
			{ MSR1(1), "SetDirtyEnS enabled" },
			{ BITVAL_EOT }
		}},
		{ 8, 1, "SetDirtyEnE:", "CleanToDirty Command for E->M State Transition Enable", PRESENT_DEC, {
			{ MSR1(0), "SetDirtyEnE disabled" },
			{ MSR1(1), "SetDirtyEnE enabled" },
			{ BITVAL_EOT }
		}},
		{ 7, 3, "SysVicLimit:", "Outstanding Victim Bus Command Limit", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 4, 5, "SysAckLimit:", "Outstanding Bus Command Limit", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},

	{ 0xC0010015, MSRTYPE_RDWR, MSR2(0, 0), "HWCR Register", "This register controls the hardware configuration", {
		{ 63, 32, RESERVED },
		{ 31, 2, RESERVED },
		{ 29, 6, "START_FID:", "Status of the startup FID", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 23, 5, RESERVED },
		{ 18, 1, "MCi_STATUS_WREN:", "MCi Status Write Enable", PRESENT_DEC, {
			{ MSR1(0), "MCi_STATUS_WREN disabled" },
			{ MSR1(1), "MCi_STATUS_WREN enabled" },
			{ BITVAL_EOT }
		}},
		{ 17, 1, "WRAP32DIS:", "32-bit Address Wrap Disable", PRESENT_DEC, {
			{ MSR1(0), "WRAP32DIS clear" },
			{ MSR1(1), "WRAP32DIS set" },
			{ BITVAL_EOT }
		}},
		{ 16, 1, RESERVED },
		{ 15, 1, "SSEDIS:", "SSE Instructions Disable", PRESENT_DEC, {
			{ MSR1(0), "SSEDIS clear" },
			{ MSR1(1), "SSEDIS set" },
			{ BITVAL_EOT }
		}},
		{ 14, 1, "RSMSPCYCDIS:", "Special Bus Cycle On RSM Disable", PRESENT_DEC, {
			{ MSR1(0), "RSMSPCYCDIS clear" },
			{ MSR1(1), "RSMSPCYCDIS set" },
			{ BITVAL_EOT }
		}},
		{ 13, 1, "SMISPCYCDIS:", "Special Bus Cycle On SMI Disable", PRESENT_DEC, {
			{ MSR1(0), "SMISPCYCDIS clear" },
			{ MSR1(1), "SMISPCYCDIS set" },
			{ BITVAL_EOT }
		}},
		{ 12, 1, "HLTXSPCYCEN:", "Enable Special Bus Cycle On Exit From HLT", PRESENT_DEC, {
			{ MSR1(0), "HLTXSPCYCEN disabled" },
			{ MSR1(1), "HLTXSPCYCEN enabled" },
			{ BITVAL_EOT }
		}},
		{ 11, 4, RESERVED },
		{ 8, 1, "IGNNE_EM:", "IGNNE Port Emulation Enable", PRESENT_DEC, {
			{ MSR1(0), "IGNNE_EM disabled" },
			{ MSR1(1), "IGNNE_EM enabled" },
			{ BITVAL_EOT }
		}},
		{ 7, 1, "DISLOCK:", "Disable x86 LOCK prefix functionality", PRESENT_DEC, {
			{ MSR1(0), "DISLOCK clear" },
			{ MSR1(1), "DISLOCK set" },
			{ BITVAL_EOT }
		}},
		{ 6, 1, "FFDIS:", "TLB Flush Filter Disable", PRESENT_DEC, {
			{ MSR1(0), "FFDIS clear" },
			{ MSR1(1), "FFDIS set" },
			{ BITVAL_EOT }
		}},
		{ 5, 1, RESERVED },
		{ 4, 1, "INVD_WBINVD:", "INVD to WBINVD Conversion", PRESENT_DEC, {
			{ MSR1(0), "INVD_WBINVD disabled" },
			{ MSR1(1), "INVD_WBINVD enabled" },
			{ BITVAL_EOT }
		}},
		{ 3, 1, "TLBCACHEDIS:", "TLB Cacheable Memory Disable", PRESENT_DEC, {
			{ MSR1(0), "TLBCACHEDIS clear" },
			{ MSR1(1), "TLBCACHEDIS set" },
			{ BITVAL_EOT }
		}},
		{ 2, 1, RESERVED },
		{ 1, 1, "SLOWFENCE:", "Slow SFENCE Enable", PRESENT_DEC, {
			{ MSR1(0), "SLOWFENCE disabled" },
			{ MSR1(1), "SLOWFENCE enabled" },
			{ BITVAL_EOT }
		}},
		{ 0, 1, "SMMLOCK:", "SMM Configuration Lock", PRESENT_DEC, {
			{ MSR1(0), "SMMLOCK disabled" },
			{ MSR1(1), "SMMLOCK enabled" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},

	{ 0xC001001F, MSRTYPE_RDWR, MSR2(0, 0), "NB_CFG Register", "", {
		{ 63, 9, RESERVED },
		{ 54, 1, "InitApicIdCpuIdLo:", "CpuId and NodeId[2:0] bit field positions are swapped in the APICID", PRESENT_DEC, {
			{ MSR1(0), "CpuId and NodeId not swapped" },
			{ MSR1(1), "CpuId and NodeId swapped" },
			{ BITVAL_EOT }
		}},
		{ 53, 8, RESERVED },
		{ 45, 1, "DisUsSysMgtRqToNLdt:", "Disable Upstream System Management Rebroadcast", PRESENT_DEC, {
			{ MSR1(0), "Upstream Rebroadcast disabled" },
			{ MSR1(1), "Upstream Rebroadcast enabled" },
			{ BITVAL_EOT }
		}},
		{ 44, 1, RESERVED },
		{ 43, 1, "DisThmlPfMonSmiInt:", "Disable Performance Monitor SMI", PRESENT_DEC, {
			{ MSR1(0), "Performance Monitor SMI enabled" },
			{ MSR1(1), "Performance Monitor SMI disabled" },
			{ BITVAL_EOT }
		}},
		{ 42, 6, RESERVED },
		{ 36, 1, "DisDatMsk:", "Disables DRAM data masking function", PRESENT_DEC, {
			{ MSR1(0), "DRAM data masking enabled" },
			{ MSR1(1), "DRAM data masking disabled" },
			{ BITVAL_EOT }
		}},
		{ 35, 4, RESERVED },
		{ 31, 1, "DisCohLdtCfg:", "Disable Coherent HyperTransport Configuration Accesses", PRESENT_DEC, {
			{ MSR1(0), "Coherent HyperTransport Configuration enabled" },
			{ MSR1(1), "Coherent HyperTransport Configuration disabled" },
			{ BITVAL_EOT }
		}},
		{ 30, 21, RESERVED },
		{ 9, 1, "DisRefUseFreeBuf:", "Disable Display Refresh from Using Free List Buffers", PRESENT_DEC, {
			{ MSR1(0), "Display refresh requests enabled" },
			{ MSR1(1), "Display refresh requests disabled" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},

	{ 0xC001001A, MSRTYPE_RDWR, MSR2(0, 0), "TOP_MEM Register", "This register indicates the first byte of I/O above DRAM", {
		{ 63, 24, RESERVED },
		{ 39, 8, "TOM 39-32", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 9, "TOM 31-23", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 22, 23, RESERVED },
		{ BITS_EOT }
	}},

	{ 0xC001001D, MSRTYPE_RDWR, MSR2(0, 0), "TOP_MEM2 Register", "This register indicates the Top of Memory above 4GB", {
		{ 63, 24, RESERVED },
		{ 39, 8, "TOM2 39-32", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 9, "TOM2 31-23", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 22, 23, RESERVED },
		{ BITS_EOT }
	}},

	{ 0xC0010016, MSRTYPE_RDWR, MSR2(0, 0), "IORRBase0", "This register holds the base of the variable I/O range", {
		{ 63, 24, RESERVED },
		{ 39, 8, "BASE 27-20", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 20, "BASE 20-0", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 11, 6, RESERVED },
		{ 5, 1, "RdDram:", "Read from DRAM", PRESENT_DEC, {
			{ MSR1(0), "RdDram disabled" },
			{ MSR1(1), "RdDram enabled" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "WrDram:", "Write to DRAM", PRESENT_DEC, {
			{ MSR1(0), "WrDram disabled" },
			{ MSR1(1), "WrDram enabled" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},

	{ 0xC0010017, MSRTYPE_RDWR, MSR2(0, 0), "IORRMask0", "This register holds the mask of the variable I/O range", {
		{ 63, 24, RESERVED },
		{ 39, 8, "MASK 27-20", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 20, "MASK 20-0", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 11, 1, "V:", "Enables variable I/O range registers", PRESENT_DEC, {
			{ MSR1(0), "V I/O range disabled" },
			{ MSR1(1), "V I/O range enabled" },
			{ BITVAL_EOT }
		}},
		{ 10, 11, RESERVED },
		{ BITS_EOT }
	}},

	{ 0xC0010018, MSRTYPE_RDWR, MSR2(0, 0), "IORRBase1", "This register holds the base of the variable I/O range", {
		{ 63, 24, RESERVED },
		{ 39, 8, "BASE 27-20", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 20, "BASE 20-0", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 11, 6, RESERVED },
		{ 5, 1, "RdDram:", "Read from DRAM", PRESENT_DEC, {
			{ MSR1(0), "RdDram disabled" },
			{ MSR1(1), "RdDram enabled" },
			{ BITVAL_EOT }
		}},
		{ 4, 1, "WrDram:", "Write to DRAM", PRESENT_DEC, {
			{ MSR1(0), "WrDram disabled" },
			{ MSR1(1), "WrDram enabled" },
			{ BITVAL_EOT }
		}},
		{ BITS_EOT }
	}},

	{ 0xC0010019, MSRTYPE_RDWR, MSR2(0, 0), "IORRMask1", "This register holds the mask of the variable I/O range", {
		{ 63, 24, RESERVED },
		{ 39, 8, "MASK 27-20", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 31, 20, "MASK 20-0", "", PRESENT_HEX, {
			{ BITVAL_EOT }
		}},
		{ 11, 1, "V:", "Enables variable I/O range registers", PRESENT_DEC, {
			{ MSR1(0), "V I/O range disabled" },
			{ MSR1(1), "V I/O range enabled" },
			{ BITVAL_EOT }
		}},
		{ 10, 11, RESERVED },
		{ BITS_EOT }
	}},

	{ MSR_EOT }
};
