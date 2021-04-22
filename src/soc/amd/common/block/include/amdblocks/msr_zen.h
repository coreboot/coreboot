/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_MSR_ZEN_H
#define AMD_BLOCK_MSR_ZEN_H

/*
 * The MCAX CTL/STATUS/ADDR/MISC0 registers are aliases for the legacy MCA registers starting
 * at 0x00000400 which can still be used instead of the MCAX ones. Each MCAX bank has 16
 * registers while the legacy MCA banks only had 4 registers each.
 */
#define MCAX_MSR_BASE		0xc0002000
#define  MCAX_BANK_SIZE		0x10
#define  MCAX_CTL_OFFSET	0x0
#define  MCAX_STATUS_OFFSET	0x1
#define  MCAX_ADDR_OFFSET	0x2
#define  MCAX_MISC0_OFFSET	0x3
#define  MCAX_CONFIG_OFFSET	0x4
#define  MCAX_IPID_OFFSET	0x5
#define  MCAX_SYND_OFFSET	0x6
#define  MCAX_RESERVED_OFFSET	0x7
#define  MCAX_DESTAT_OFFSET	0x8
#define  MCAX_DEADDR_OFFSET	0x9
#define  MCAX_MISC1_OFFSET	0xa
#define  MCAX_MISC2_OFFSET	0xb
#define  MCAX_MISC3_OFFSET	0xc
#define  MCAX_MISC4_OFFSET	0xd
#define MCAX_MSR(bank, offset)	(MCAX_MSR_BASE + (bank) * MCAX_BANK_SIZE + (offset))
#define MCAX_CTL_MSR(bank)	MCAX_MSR(bank, MCAX_CTL_OFFSET)
#define MCAX_STATUS_MSR(bank)	MCAX_MSR(bank, MCAX_STATUS_OFFSET)
#define MCAX_ADDR_MSR(bank)	MCAX_MSR(bank, MCAX_ADDR_OFFSET)
#define MCAX_MISC0_MSR(bank)	MCAX_MSR(bank, MCAX_MISC0_OFFSET)
#define MCAX_CONFIG_MSR(bank)	MCAX_MSR(bank, MCAX_CONFIG_OFFSET)
#define MCAX_IPID_MSR(bank)	MCAX_MSR(bank, MCAX_IPID_OFFSET)
#define MCAX_SYND_MSR(bank)	MCAX_MSR(bank, MCAX_SYND_OFFSET)
#define MCAX_DESTAT_MSR(bank)	MCAX_MSR(bank, MCAX_DESTAT_OFFSET)
#define MCAX_DEADDR_MSR(bank)	MCAX_MSR(bank, MCAX_DEADDR_OFFSET)
#define MCAX_MISC1_MSR(bank)	MCAX_MSR(bank, MCAX_MISC1_OFFSET)
#define MCAX_MISC2_MSR(bank)	MCAX_MSR(bank, MCAX_MISC2_OFFSET)
#define MCAX_MISC3_MSR(bank)	MCAX_MSR(bank, MCAX_MISC3_OFFSET)
#define MCAX_MISC4_MSR(bank)	MCAX_MSR(bank, MCAX_MISC4_OFFSET)

/*
 * The MCA CTL_MASK moved to a new location in the fam 17h+ CPUs and accessing the legacy
 * CTL_MASK MSR starting at 0xc0010044 on fam17h+ CPUs will cause a general protection fault.
 */
#define MCA_CTL_MASK_MSR_0	0xc0010400
#define MCA_CTL_MASK_MSR(bank)	(MCA_CTL_MASK_MSR_0 + (bank))

#endif /* AMD_BLOCK_MSR_ZEN_H */
