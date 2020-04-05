/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _DENVERTON_NS_P2SB_H_
#define _DENVERTON_NS_P2SB_H_

/* Generate MP-table IRQ numbers for PCI devices. */
#define IO_APIC0 1

/* P2SB Bridge Registers (D31:F1) */
#define PCH_P2SB_DEV PCI_DEV(0, P2SB_DEV, P2SB_FUNC)

/* IO/MEM BARs */
#define SBREG_LO 0x10
#define MASK_SBREG_LO 0xff000000
#define SBREG_HI 0x14
#define MASK_SBREG_HI 0xffffffff

/* ITSS PCRs (PID:ITSS) */
#define PCR_ITSS_PIRQA_ROUT 0x3100
#define PCR_ITSS_PIRQB_ROUT 0x3101
#define PCR_ITSS_PIRQC_ROUT 0x3102
#define PCR_ITSS_PIRQD_ROUT 0x3103
#define PCR_ITSS_PIRQE_ROUT 0x3104
#define PCR_ITSS_PIRQF_ROUT 0x3105
#define PCR_ITSS_PIRQG_ROUT 0x3106
#define PCR_ITSS_PIRQH_ROUT 0x3107

#define PCR_ITSS_PIR00 0x3140
#define PCR_ITSS_PIR01 0x3142
#define PCR_ITSS_PIR02 0x3144
#define PCR_ITSS_PIR03 0x3146
#define PCR_ITSS_PIR04 0x3148
#define PCR_ITSS_PIR05 0x314A
#define PCR_ITSS_PIR06 0x314C
#define PCR_ITSS_PIR07 0x314E
#define PCR_ITSS_PIR08 0x3150
#define PCR_ITSS_PIR09 0x3152
#define PCR_ITSS_PIR10 0x3154
#define PCR_ITSS_PIR11 0x3156
#define PCR_ITSS_PIR12 0x3158

#define PCH_PCR_ITSS_GIC 0x31FC ///< General Interrupt Control
///< Max IRQ entry size, 1 = 24 entry size, 0 = 120 entry size
#define PCH_PCR_ITSS_GIC_MAX_IRQ_24 \
	(1 << 9)
#define PCH_PCR_ITSS_GIC_AME (1 << 17) ///< Alternate Access Mode Enable
#define PCH_PCR_ITSS_GIC_SPS (1 << 16) ///< Shutdown Policy Select
#define PCH_PCR_ITSS_IPC0 0x3200       ///< Interrupt Polarity Control 0
#define PCH_PCR_ITSS_IPC1 0x3204       ///< Interrupt Polarity Control 1
#define PCH_PCR_ITSS_IPC2 0x3208       ///< Interrupt Polarity Control 2
#define PCH_PCR_ITSS_IPC3 0x320C       ///< Interrupt Polarity Control 3

#endif /* _DENVERTON_NS_P2SB_H_ */
