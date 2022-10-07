/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_I945_H
#define NORTHBRIDGE_INTEL_I945_H

#include <northbridge/intel/common/fixed_bars.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

#define DEFAULT_X60BAR		0xfed13000

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Display defines for the interrupt 15h handler */
#define INT15_5F35_CL_DISPLAY_DEFAULT		0
#define INT15_5F35_CL_DISPLAY_CRT		(1 << 0)
#define INT15_5F35_CL_DISPLAY_TV		(1 << 1)
#define INT15_5F35_CL_DISPLAY_EFP		(1 << 2)
#define INT15_5F35_CL_DISPLAY_LCD		(1 << 3)
#define INT15_5F35_CL_DISPLAY_CRT2		(1 << 4)
#define INT15_5F35_CL_DISPLAY_TV2		(1 << 5)
#define INT15_5F35_CL_DISPLAY_EFP2		(1 << 6)
#define INT15_5F35_CL_DISPLAY_LCD2		(1 << 7)

/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#define EPBAR		0x40
#define MCHBAR		0x44
#define PCIEXBAR	0x48
#define DMIBAR		0x4c
#define X60BAR		0x60

#define GGC		0x52			/* GMCH Graphics Control */

#define DEVEN		0x54			/* Device Enable */
#define  DEVEN_D0F0 (1 << 0)
#define  DEVEN_D1F0 (1 << 1)
#define  DEVEN_D2F0 (1 << 3)
#define  DEVEN_D2F1 (1 << 4)

#ifndef BOARD_DEVEN
#define BOARD_DEVEN (DEVEN_D0F0 | DEVEN_D2F0 | DEVEN_D2F1)
#endif /* BOARD_DEVEN */

#define PAM0		0x90
#define PAM1		0x91
#define PAM2		0x92
#define PAM3		0x93
#define PAM4		0x94
#define PAM5		0x95
#define PAM6		0x96

#define LAC		0x97	/* Legacy Access Control */
#define TOLUD		0x9c	/* Top of Low Used Memory */
#define SMRAM		0x9d	/* System Management RAM Control */
#define ESMRAMC		0x9e	/* Extended System Management RAM Control */

#define TOM		0xa0

#define SKPAD		0xdc	/* Scratchpad Data */

/* Device 0:1.0 PCI configuration space (PCI Express) */

#define PCISTS1		0x06	/* 16bit */
#define SSTS1		0x1e	/* 16bit */
#define PEG_CAP		0xa2	/* 16bit */
#define DSTS		0xaa	/* 16bit */
#define SLOTCAP		0xb4	/* 32bit */
#define SLOTSTS		0xba	/* 16bit */
#define PEG_LC		0xec	/* 32bit */
#define PVCCAP1		0x104	/* 32bit */
#define VC0RCTL		0x114	/* 32bit */
#define LE1D		0x150	/* 32bit */
#define LE1A		0x158	/* 64bit */
#define UESTS		0x1c4	/* 32bit */
#define CESTS		0x1d0	/* 32bit */
#define PEGTC		0x204	/* 32bit */
#define PEGCC		0x208	/* 32bit */
#define PEGSTS		0x214	/* 32bit */

/* Device 0:2.0 PCI configuration space (Graphics Device) */
#define IGD_DEV		PCI_DEV(0, 2, 0)

#define GMADR		0x18
#define GTTADR		0x1c
#define BSM		0x5c
#define GCFC		0xf0	/* Graphics Clock Frequency & Gating Control */

/* Chipset Control Registers */
#define FSBPMC3		0x40	/* 32bit */
#define FSBPMC4		0x44	/* 32bit */
#define FSBSNPCTL	0x48	/* 32bit */
#define SLPCTL		0x90	/* 32bit */

#define C0DRB0		0x100	/*  8bit */
#define C0DRB1		0x101	/*  8bit */
#define C0DRB2		0x102	/*  8bit */
#define C0DRB3		0x103	/*  8bit */
#define C0DRA0		0x108	/*  8bit */
#define C0DRA2		0x109	/*  8bit */
#define C0DCLKDIS	0x10c	/*  8bit */
#define C0BNKARC	0x10e	/* 16bit */
#define C0DRT0		0x110	/* 32bit */
#define C0DRT1		0x114	/* 32bit */
#define C0DRT2		0x118	/* 32bit */
#define C0DRT3		0x11c	/* 32bit */
#define C0DRC0		0x120	/* 32bit */
#define C0DRC1		0x124	/* 32bit */
#define C0DRC2		0x128	/* 32bit */
#define C0AIT		0x130	/* 64bit */
#define C0DCCFT		0x138	/* 64bit */
#define C0GTEW		0x140	/* 32bit */
#define C0GTC		0x144	/* 32bit */
#define C0DTPEW		0x148	/* 64bit */
#define C0DTAEW		0x150	/* 64bit */
#define C0DTC		0x158	/* 32bit */
#define C0DMC		0x164	/* 32bit */
#define C0ODT		0x168	/* 64bit */

#define C1DRB0		0x180	/*  8bit */
#define C1DRB1		0x181	/*  8bit */
#define C1DRB2		0x182	/*  8bit */
#define C1DRB3		0x183	/*  8bit */
#define C1DRA0		0x188	/*  8bit */
#define C1DRA2		0x189	/*  8bit */
#define C1DCLKDIS	0x18c	/*  8bit */
#define C1BNKARC	0x18e	/* 16bit */
#define C1DRT0		0x190	/* 32bit */
#define C1DRT1		0x194	/* 32bit */
#define C1DRT2		0x198	/* 32bit */
#define C1DRT3		0x19c	/* 32bit */
#define C1DRC0		0x1a0	/* 32bit */
#define C1DRC1		0x1a4	/* 32bit */
#define C1DRC2		0x1a8	/* 32bit */
#define C1AIT		0x1b0	/* 64bit */
#define C1DCCFT		0x1b8	/* 64bit */
#define C1GTEW		0x1c0	/* 32bit */
#define C1GTC		0x1c4	/* 32bit */
#define C1DTPEW		0x1c8	/* 64bit */
#define C1DTAEW		0x1d0	/* 64bit */
#define C1DTC		0x1d8	/* 32bit */
#define C1DMC		0x1e4	/* 32bit */
#define C1ODT		0x1e8	/* 64bit */

#define DCC		0x200	/* 32bit */
#define CCCFT		0x208	/* 64bit */
#define WCC		0x218	/* 32bit */
#define MMARB0		0x220	/* 32bit */
#define MMARB1		0x224	/* 32bit */
#define SBTEST		0x230	/* 32bit */
#define SBOCC		0x238	/* 32bit */
#define ODTC		0x284	/* 32bit */
#define SMVREFC		0x2a0	/* 32bit */
#define DRTST		0x2a8	/* 32bit */
#define REPC		0x2e0	/* 32bit */
#define DQSMT		0x2f4	/* 16bit */
#define RCVENMT		0x2f8	/* 32bit */

#define C0R0B00DQST	0x300	/* 64bit */

#define C0WL0REOST	0x340	/*  8bit */
#define C0WL1REOST	0x341	/*  8bit */
#define C0WL2REOST	0x342	/*  8bit */
#define C0WL3REOST	0x343	/*  8bit */
#define WDLLBYPMODE	0x360	/* 16bit */
#define C0WDLLCMC	0x36c	/* 32bit */
#define C0HCTC		0x37c	/*  8bit */

#define C1R0B00DQST	0x380	/* 64bit */

#define C1WL0REOST	0x3c0	/*  8bit */
#define C1WL1REOST	0x3c1	/*  8bit */
#define C1WL2REOST	0x3c2	/*  8bit */
#define C1WL3REOST	0x3c3	/*  8bit */
#define C1WDLLCMC	0x3ec	/* 32bit */
#define C1HCTC		0x3fc	/*  8bit */

#define GBRCOMPCTL	0x400	/* 32bit */

#define SMSRCTL		0x408	/* XXX who knows */
#define C0DRAMW		0x40c	/* 16bit */
#define G1SC		0x410	/*  8bit */
#define G2SC		0x418	/*  8bit */
#define G3SC		0x420	/*  8bit */
#define G4SC		0x428	/*  8bit */
#define G5SC		0x430	/*  8bit */
#define G6SC		0x438	/*  8bit */

#define C1DRAMW		0x48c	/* 16bit */
#define G7SC		0x490	/*  8bit */
#define G8SC		0x498	/*  8bit */

#define G1SRPUT		0x500	/* 256bit */
#define G1SRPDT		0x520	/* 256bit */
#define G2SRPUT		0x540	/* 256bit */
#define G2SRPDT		0x560	/* 256bit */
#define G3SRPUT		0x580	/* 256bit */
#define G3SRPDT		0x5a0	/* 256bit */
#define G4SRPUT		0x5c0	/* 256bit */
#define G4SRPDT		0x5e0	/* 256bit */
#define G5SRPUT		0x600	/* 256bit */
#define G5SRPDT		0x620	/* 256bit */
#define G6SRPUT		0x640	/* 256bit */
#define G6SRPDT		0x660	/* 256bit */
#define G7SRPUT		0x680	/* 256bit */
#define G7SRPDT		0x6a0	/* 256bit */
#define G8SRPUT		0x6c0	/* 256bit */
#define G8SRPDT		0x6e0	/* 256bit */

/* Clock Controls */
#define CLKCFG		0xc00	/* 32bit */
#define UPMC1		0xc14	/* 16bit */
#define CPCTL		0xc16	/* 16bit */
#define SSKPD		0xc1c	/* 16bit (scratchpad) */
#define UPMC2		0xc20	/* 16bit */
#define UPMC4		0xc30	/* 32bit */
#define PLLMON		0xc34	/* 32bit */
#define HGIPMC2		0xc38	/* 32bit */

/* Thermal Management Controls */
#define TSC1		0xc88	/*  8bit */
#define TSS1		0xc8a	/*  8bit */
#define TR1		0xc8b	/*  8bit */
#define TSTTP1		0xc8c	/* 32bit */
#define TCO1		0xc92	/*  8bit */
#define THERM1_1	0xc94	/*  8bit */
#define TCOF1		0xc96	/*  8bit */
#define TIS1		0xc9a	/* 16bit */
#define TSTTP1_2	0xc9c	/* 32bit */
#define IUB		0xcd0	/* 32bit */
#define TSC0_1		0xcd8	/*  8bit */
#define TSS0		0xcda	/*  8bit */
#define TR0		0xcdb	/*  8bit */
#define TSTTP0_1	0xcdc	/* 32bit */
#define TCO0		0xce2	/*  8bit */
#define THERM0_1	0xce4	/*  8bit */
#define TCOF0		0xce6	/*  8bit */
#define TIS0		0xcea	/* 16bit */
#define TSTTP0_2	0xcec	/* 32bit */
#define TERRCMD		0xcf0	/*  8bit */
#define TSMICMD		0xcf1	/*  8bit */
#define TSCICMD		0xcf2	/*  8bit */
#define TINTRCMD	0xcf3	/*  8bit */
#define EXTTSCS		0xcff	/*  8bit */
#define DFT_STRAP1	0xe08	/* 32bit */

/* ACPI Power Management Controls */

#define MIPMC3		0xbd8	/* 32bit */

#define C2C3TT		0xf00	/* 32bit */
#define C3C4TT		0xf04	/* 32bit */

#define MIPMC4		0xf08	/* 16bit */
#define MIPMC5		0xf0a	/* 16bit */
#define MIPMC6		0xf0c	/* 16bit */
#define MIPMC7		0xf0e	/* 16bit */
#define PMCFG		0xf10	/* 32bit */
#define SLFRCS		0xf14	/* 32bit */
#define GIPMC1		0xfb0	/* 32bit */
#define FSBPMC1		0xfb8	/* 32bit */
#define UPMC3		0xfc0	/* 32bit */
#define ECO		0xffc	/* 32bit */

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPPVCCAP1	0x004	/* 32bit */
#define EPPVCCAP2	0x008	/* 32bit */

#define EPVC0RCAP	0x010	/* 32bit */
#define EPVC0RCTL	0x014	/* 32bit */
#define EPVC0RSTS	0x01a	/* 16bit */

#define EPVC1RCAP	0x01c	/* 32bit */
#define EPVC1RCTL	0x020	/* 32bit */
#define EPVC1RSTS	0x026	/* 16bit */

#define EPVC1MTS	0x028	/* 32bit */
#define EPVC1IST	0x038	/* 64bit */

#define EPESD		0x044	/* 32bit */

#define EPLE1D		0x050	/* 32bit */
#define EPLE1A		0x058	/* 64bit */
#define EPLE2D		0x060	/* 32bit */
#define EPLE2A		0x068	/* 64bit */

#define PORTARB		0x100	/* 256bit */

/*
 * DMIBAR
 */

#define DMIVCECH	0x000	/* 32bit */
#define DMIPVCCAP1	0x004	/* 32bit */
#define DMIPVCCAP2	0x008	/* 32bit */

#define DMIPVCCCTL	0x00c	/* 16bit */

#define DMIVC0RCAP	0x010	/* 32bit */
#define DMIVC0RCTL0	0x014	/* 32bit */
#define DMIVC0RSTS	0x01a	/* 16bit */

#define DMIVC1RCAP	0x01c	/* 32bit */
#define DMIVC1RCTL	0x020	/* 32bit */
#define DMIVC1RSTS	0x026	/* 16bit */

#define DMILE1D		0x050	/* 32bit */
#define DMILE1A		0x058	/* 64bit */
#define DMILE2D		0x060	/* 32bit */
#define DMILE2A		0x068	/* 64bit */

#define DMILCAP		0x084	/* 32bit */
#define DMILCTL		0x088	/* 16bit */
#define DMILSTS		0x08a	/* 16bit */

#define DMICTL1		0x0f0	/* 32bit */
#define DMICTL2		0x0fc	/* 32bit */

#define DMICC		0x208	/* 32bit */

#define DMIDRCCFG	0xeb4	/* 32bit */

int i945_silicon_revision(void);
void i945_early_initialization(void);
void i945_late_initialization(int s3resume);

/* debugging functions */
void print_pci_devices(void);
void dump_pci_device(unsigned int dev);
void dump_pci_devices(void);
void dump_spd_registers(u8 spd_map[4]);
void sdram_dump_mchbar_registers(void);

u32 decode_igd_memory_size(u32 gms);
u32 decode_tseg_size(const u8 esmramc);

/* Romstage mainboard callbacks */
/* Optional: Override the default LPC config. */
void mainboard_lpc_decode(void);
/* Optional: mainboard specific init after console init and before raminit. */
void mainboard_pre_raminit_config(int s3_resume);
/* Mainboard specific RCBA init. Happens after raminit. */
void mainboard_late_rcba_config(void);
/* Optional: mainboard callback to get SPD map */
void mainboard_get_spd_map(u8 spd_map[4]);

#endif /* __ACPI__ */

#endif /* NORTHBRIDGE_INTEL_I945_H */
