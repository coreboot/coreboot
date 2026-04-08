/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - register definitions
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS
 * Reference: coreboot GM45 (Cantiga) northbridge
 *
 * GM965 is the predecessor to GM45. Register layout is largely
 * identical at the MCHBAR/DMIBAR/EPBAR level. Key difference:
 * GM965 supports DDR2 only; GM45 adds DDR3.
 */

#ifndef _GM965_H_
#define _GM965_H_

#include <stdint.h>
#include <device/dram/ddr2.h>
#include <device/pci_ops.h>
#include <northbridge/intel/common/fixed_bars.h>

/* ================================================================== */
/* PCI Device IDs                                                     */
/* ================================================================== */

#define PCI_DID_GM965_MCH       0x2A00  /* Host Bridge */
#define PCI_DID_GM965_PEG       0x2A01  /* PCI Express Graphics */
#define PCI_DID_GM965_IGD       0x2A02  /* Integrated Graphics */
#define PCI_DID_GM965_IGD_1     0x2A03  /* IGD (alt) */

/* PCI Devices */
#define D0F0  PCI_DEV(0, 0, 0)  /* Host Bridge */
#define D1F0  PCI_DEV(0, 1, 0)  /* PEG */
#define D2F0  PCI_DEV(0, 2, 0)  /* IGD */
#define D2F1  PCI_DEV(0, 2, 1)  /* IGD Pipe B */

/* ================================================================== */
/* D0:F0 PCI Config Space                                             */
/* ================================================================== */

#define D0F0_EPBAR_LO        0x40
#define D0F0_EPBAR_HI        0x44
#define D0F0_MCHBAR_LO       0x48
#define D0F0_MCHBAR_HI       0x4c
#define D0F0_GGC             0x52  /* Graphics Memory Control */
#define D0F0_DEVEN           0x54  /* Device Enable */
#define D0F0_PCIEXBAR_LO     0x60
#define D0F0_PCIEXBAR_HI     0x64
#define D0F0_DMIBAR_LO       0x68
#define D0F0_DMIBAR_HI       0x6c
#define D0F0_PAM(x)          (0x90 + (x)) /* PAM0..PAM6 */
#define D0F0_REMAPBASE       0x98
#define D0F0_REMAPLIMIT      0x9a
#define D0F0_SMRAM           0x9d
#define D0F0_ESMRAMC         0x9e
#define D0F0_TOM             0xa0  /* Top of Memory */
#define D0F0_TOUUD           0xa2  /* Top of Upper Usable DRAM */
#define D0F0_TOLUD           0xb0  /* Top of Low Usable DRAM */
#define D0F0_SKPD            0xdc  /* Scratchpad */
#define D0F0_CAPID0          0xe0  /* Capability ID */

/* DEVEN bits */
#define DEVEN_D0F0	(1 << 0)
#define DEVEN_D1F0	(1 << 1)   /* PEG */
#define DEVEN_D2F0	(1 << 3)   /* IGD */
#define DEVEN_D2F1	(1 << 4)   /* IGD Pipe B */
#define DEVEN_D3F0	(1 << 6)
#define DEVEN_D3F1	(1 << 7)
#define DEVEN_D3F2	(1 << 8)
#define DEVEN_D3F3	(1 << 9)


/* D3:F0 Management Engine (ME) */
#define D3F0         PCI_DEV(0, 3, 0)
#define D3F0_BAR     0xFED10000

/*
 * D3:F0 HECI (Host Embedded Controller Interface) register offsets
 * relative to D3F0_BAR.  H_CSR and ME_CSR_HA share the same bit layout.
 * The vendor BIOS uses this interface to notify the ME before/after
 * a PLL frequency change so the ME can quiesce its clocked logic.
 * If ME firmware is absent the whole handshake is skipped and the PLL
 * still relocks via the CLKCFG hardware alone.
 */
#define ME_H_CB_WW    0x00  /* Host Circular Buffer Write Window */
#define ME_H_CSR      0x04  /* Host Control/Status Register */
#define ME_ME_CB_RW   0x08  /* ME Circular Buffer Read Window */
#define ME_ME_CSR_HA  0x0c  /* ME Control/Status Register (Host Access) */
/* Bit layout common to ME_H_CSR and ME_ME_CSR_HA */
#define ME_CSR_IE     (1u << 0)  /* Interrupt Enable */
#define ME_CSR_IS     (1u << 1)  /* Interrupt Status (w1c) */
#define ME_CSR_IG     (1u << 2)  /* Interrupt Generate */
#define ME_CSR_RDY    (1u << 3)  /* Ready */
#define ME_CSR_RST    (1u << 4)  /* Reset */

/* D2:F0 IGD config space */
#define D2F0_BSM             0x5c  /* Base of Stolen Memory */

/* GGC bits */
#define GGC_GMS_MASK      (0x7 << 4)   /* GFX UMA size, bits [6:4] */

/* IGD config */
#define GCFGC_OFFSET      0xf0
#define GCFGC_CR_MASK     (0xf << 0)
#define GCFGC_CS_MASK     (0xf << 8)
#define GCFGC_UPDATE      (1 << 5)

/* ================================================================== */
/* MCHBAR Register Offsets (base = 0xFED14000)                        */
/* ================================================================== */

/* --- Front Side Bus Power Management --- */
#define FSBPMC3              0x0040  /* Front Side Bus Power Management Control 3 */
#define FSBPMC5              0x0094  /* Front Side Bus Power Management Control 5 */

/* --- DCC (DRAM Channel Control) --- */
#define DCC_MCHBAR           0x0200  /* DRAM Channel Control */
#define DCC2_MCHBAR          0x0204  /* DRAM Channel Control 2 (bits 15:0 = ME size in MB) */
#define DCC_INTERLEAVED      (1 << 1)
#define DCC_NO_CHANXOR       (1 << 10)
#define DCC_CMD_SHIFT        16
#define DCC_CMD_MASK         (7 << 16)
#define DCC_CMD_NOP          (1 << 16)
#define DCC_CMD_ABP          (2 << 16)      /* All Banks Precharge */
#define DCC_SET_MREG         (3 << 16)      /* MR0 / Mode Register Set */
#define DCC_SET_EREG         (4 << 16)      /* Extended Mode Registers */
#define DCC_SET_EREG_SHIFT   21
#define DCC_SET_EREG_MASK    (DCC_CMD_MASK | (3 << 21))
#define DCC_SET_EREGx(x)    ((DCC_SET_EREG | (((x)-1) << 21)) & DCC_SET_EREG_MASK)
#define DCC_CMD_CBR          (6 << 16)      /* CBR Auto-Refresh */
#define DCC_CMD_NORMAL       (7 << 16)
#define DCC_INIT_COMPLETE    (1 << 19)                 /* Initialization Complete (IC) */
#define DCC_CMD_NORMAL_ANN   (DCC_CMD_NORMAL | DCC_INIT_COMPLETE)

/* --- Clock Crossing --- */
#define CLKCROSS_DATA3       0x0208
#define CLKCROSS_DATA2       0x020c
#define CLKCROSS_DATA1       0x0210
#define WRITE_CTRL           0x0218
#define MMARB0               0x0220  /* Main Memory Arbiter Control 0 */
#define MMARB1               0x0224  /* Main Memory Arbiter Control 1 */
#define SBTEST               0x0230  /* SB Test Register */
#define POST_JEDEC_TIM0      0x0238
#define POST_JEDEC_TIM1      0x023c

/* --- RCOMP --- */
#define RCOMP_CTRL           0x0400
#define RCOMP_STATUS         0x0404
#define RCOMP_CFG            0x040c
#define RCOMP_IO_CFG         0x0410
#define RCOMP_CFG2           0x0414
#define RCOMP_CFG3           0x0418
#define RCOMP_CFG4           0x041c
#define RCOMP_ODT0           0x04d0
#define RCOMP_ODT1           0x04d4
#define RCOMP_TABLES         0x0680  /* 9 groups x 64 bytes in MCHBAR */

/*
 * --- EPDunit / Address Decode (BIOS FFF02A7A + FFF03286) ---
 *
 * The EPDunit region (MCHBAR 0x0A00-0x0AA0) programs the memory
 * controller's secondary address decode and timing logic.  Without
 * this, the controller does not know how to route transactions to
 * channel 1.  The RAMINIT copy at FFF0xxxx programs these registers
 * between POST 0xFF41 and 0xFF42; the RAMINIT3 copy at FFFF3xxx
 * omits them entirely.
 *
 * The region includes:
 *   0x0A00-0x0A07: DRB copies from CxDRBy (halved), per-channel
 *   0x0A08-0x0A0B: DRA encoding for ch0 DIMMs (2 x 16-bit)
 *   0x0A34-0x0A3F: DRB/DRA copies for ch1
 *   0x0A10-0x0A33: Timing fields (CAS, tRP, tRCD, tRRD, tRFC, etc.)
 *   0x0A2E:        Capability/init state (bit 5 = 800MT flag)
 *   0x0A2F:        Channel population mask (bit 0=ch0, bit 1=ch1)
 *   0x0A30:        Mode control (bit 26 = channel enable)
 *   0x0A99-0x0A9F: CAS/rank-dependent scheduling
 *   0x0AA0:        Per-channel rank topology
 */
#define EPD_C0DRB01       0x0a00  /* ch0 rank0/1 boundaries (2x16-bit, halved) */
#define EPD_C0DRB23       0x0a04  /* ch0 rank2/3 boundaries (2x16-bit, halved) */
#define EPD_C0DRA01       0x0a08  /* ch0 slot0 DRA encoding  (2x8-bit) */
#define EPD_C0DRA23       0x0a0a  /* ch0 slot1 DRA encoding  (2x8-bit) */
#define EPD_C1DRB01       0x0a34  /* ch1 rank0/1 boundaries (2x16-bit, halved) */
#define EPD_C1DRB23       0x0a38  /* ch1 rank2/3 boundaries (2x16-bit, halved) */
#define EPD_C1DRA01       0x0a3c  /* ch1 slot0 DRA encoding  (2x8-bit) */
#define EPD_C1DRA23       0x0a3e  /* ch1 slot1 DRA encoding  (2x8-bit) */
#define EPD_10            0x0a10  /* control bits */
#define EPD_11            0x0a11  /* CAS + 8 field */
#define EPD_13            0x0a13  /* constant bit 2 */
#define EPD_14            0x0a14  /* CAS in low nibble */
#define EPD_15            0x0a15  /* CAS-dependent scheduling */
#define EPD_19            0x0a19  /* tWTR / tRCD+CAS / tRFC_mult */
#define EPD_1B            0x0a1b  /* CAS+4, BtB_WtR */
#define EPD_1C            0x0a1c  /* tRFC direct */
#define EPD_1D            0x0a1d  /* tRRD shifted */
#define EPD_1F            0x0a1f
#define EPD_20            0x0a20  /* BtB_WtR, tRP, constant */
#define EPD_22            0x0a22  /* tRTP + tRFC */
#define EPD_24            0x0a24  /* tRTP_precharge, constants */
#define EPD_28            0x0a28  /* rank mode / scheduling */
#define EPD_2C            0x0a2c  /* constant 0x12 */
#define EPD_2D            0x0a2d  /* mode bits */
#define EPD_2E            0x0a2e  /* capability / init state */
#define EPD_2F            0x0a2f  /* channel population mask */
#define EPD_30            0x0a30  /* mode control (bit 26) */
#define EPD_33            0x0a33
#define EPD_99            0x0a99  /* CAS/rank scheduling */
#define EPD_9C            0x0a9c  /* CAS/rank scheduling */
#define EPD_A0            0x0aa0  /* per-channel rank topology */

/* --- Clock Configuration --- */
#define CLKCFG_MCHBAR        0x0c00
#define CLKCFG_FSBCLK_SHIFT  0
#define CLKCFG_FSBCLK_MASK   (7 << 0)
#define CLKCFG_MEMCLK_SHIFT  4
#define CLKCFG_MEMCLK_MASK   (7 << 4)
#define CLKCFG_UPDATE        (1 << 12)

#define HPLLVCO_MCHBAR       0x0c0f

/* --- Scratchpad / Power Management --- */
#define SSKPD_MCHBAR         0x0c1c
#define MCHBAR_FFC           0x0ffc  /* Arbitration control */
#define PMSTS_MCHBAR         0x0f14
#define PMSTS_WARM_RESET     (1 << 1)
#define PMSTS_SELFREFRESH    (1 << 0)

/* --- PM Init registers (vendor bioscode_5.rom) --- */
#define PM_CTRL0             0x0040  /* PM control 0 */
#define PM_CTRL1             0x0044  /* PM control 1 */
#define PM_NOCARB            0x0090  /* Non-isoch arb (low 16 bits) */
#define PM_NOCARB_HI         0x0094  /* Non-isoch arb (high 32 bits) */
#define PM_SCHED             0x0b00  /* PM scheduling */
#define PM_SCHED_B90         0x0b90  /* PM scheduling 2 */
#define PM_BD8               0x0bd8  /* PM misc */
#define CLKCFG_C14           0x0c14  /* Clock config 2 */
#define CLKCFG_C16           0x0c16  /* Clock config 3 */
#define CLKCFG_C20           0x0c20  /* Clock config 4 */
#define HGIPMC2_LO           0x0c38  /* Host-Graphics Interface PM Control 2 (lower 16) */
#define HGIPMC2_HI           0x0c3a  /* Host-Graphics Interface PM Control 2 (upper 16) */
#define C2C3TT               0x0f00  /* C2 to C3 Transition Timer */
#define C3C4TT               0x0f04  /* C3 to C4 Transition Timer */
#define PM_F08               0x0f08  /* C-state transition timer */
#define PM_F10               0x0f10  /* PM control (bits 1,5 critical) */
#define PM_F60               0x0f60  /* PM mode */
#define PM_F80               0x0f80  /* PM enable (bit 31) */
#define GIPMC1               0x0fb0  /* Graphics Interface Power Management Control 1 */
#define FSBPMC1              0x0fb8  /* Front Side Bus Power Management Control 1 */
#define UPMC3                0x0fc0  /* Unit Power Management Control 3 */

/* --- IGD init registers (vendor bioscode_7.rom) --- */
#define IGD_HSYNC_VSYNC      0x0bd0  /* HSync/VSync control (byte 3 and +4) */

/* --- Per-Channel Registers (ch0 = base, ch1 = base + 0x100) --- */
#define CxDRBy_MCHBAR(ch, r)  (0x1200 + (ch)*0x100 + ((r)/2)*4)
#define CxDRA_MCHBAR(ch)      (0x1208 + (ch)*0x100)
#define CxDRA_HI(ch)          (0x120a + (ch)*0x100)  /* upper 16 bits of CxDRA */
#define CxDCLKDIS_MCHBAR(ch)  (0x120c + (ch)*0x100)
#define CxDRT0_MCHBAR(ch)     (0x1210 + (ch)*0x100)
#define CxDRT1_MCHBAR(ch)     (0x1214 + (ch)*0x100)
#define CxDRT2_MCHBAR(ch)     (0x1218 + (ch)*0x100)
#define CxDRT3_MCHBAR(ch)     (0x121c + (ch)*0x100)
#define CxDRT4_MCHBAR(ch)     (0x1220 + (ch)*0x100)
#define CxDRT5_MCHBAR(ch)     (0x1224 + (ch)*0x100)
#define CxDRT6_MCHBAR(ch)     (0x1228 + (ch)*0x100)
#define CxDRC0_MCHBAR(ch)     (0x1230 + (ch)*0x100)
#define CxDRC1_MCHBAR(ch)     (0x1234 + (ch)*0x100)
#define CxDRC2_MCHBAR(ch)     (0x1238 + (ch)*0x100)
#define CxODT_LOW(ch)         (0x1248 + (ch)*0x100)
#define CxODT_HIGH(ch)        (0x124c + (ch)*0x100)
#define CxAIT_LO(ch)          (0x1250 + (ch)*0x100)  /* Adaptive Idle Timer Control low */
#define CxAIT_HI(ch)          (0x1254 + (ch)*0x100)  /* Adaptive Idle Timer Control high */
#define CxODT_MISC(ch)        (0x1260 + (ch)*0x100)  /* ODT misc control */
#define CxODT_TIMING(ch)      (0x1268 + (ch)*0x100)  /* ODT timing */
#define CxPWR_THROTTLE0(ch)   (0x1270 + (ch)*0x100)  /* Power throttle 0 */
#define CxPWR_THROTTLE1(ch)   (0x1274 + (ch)*0x100)  /* Power throttle 1 */
#define CxODT_CTRL(ch)        (0x12a0 + (ch)*0x100)  /* ODT control */

/* CxDRC0 bits */
#define CxDRC0_RANKEN(r)     (1 << (24 + (r)))
#define CxDRC0_RANKEN_MASK   (0xf << 24)
#define CxDRC0_RMS_SHIFT     8		/* Refresh Mode Select */
#define CxDRC0_RMS_MASK      (7 << CxDRC0_RMS_SHIFT)
#define CxDRC0_RMS_78US      (2 << CxDRC0_RMS_SHIFT)
#define CxDRC0_RMS_39US      (3 << CxDRC0_RMS_SHIFT)

/* CxDRC1 bits */
#define CxDRC1_CKE_TRISTATE(r) (1 << (16 + (r)))  /* CKE tri-state enable per rank (set for unpopulated) */
#define CxDRC1_ADRTRIEN      (1 << 11)             /* Address tri-state enable */
#define CxDRC1_CSBTRIEN      (1 << 12)             /* CS# tri-state enable */

/* --- IO Init / Training --- */
#define IO_INIT_CFG          0x1400
#define IO_INIT_CLK_DEP      0x140c
#define IO_INIT_CFG2         0x1414  /* Clock-dependent IO config 2 */
#define IO_INIT_CFG3         0x1418  /* IO strobe config */
#define IO_INIT_CFG4         0x141c  /* IO strobe config 2 */
#define IO_INIT_CFG5         0x142c  /* IO misc config */
#define DRAM_TYPE_SELECT     0x1434
#define IO_INIT_CFG6         0x1438  /* IO misc config 2 */
#define IO_INIT_CFG7         0x1440  /* IO RCOMP code */
#define IO_RCOMP_CLK_EN      0x1444  /* RCOMP engine clock enable; bit 12 must be set while RCOMP runs */
#define CxWRTy_MCHBAR(ch, s)  (0x1470 + (ch)*0x100 + ((3-(s))*4))
#define CxTRAIN_CFG(ch)       (0x1484 + (ch)*0x100)  /* Training config */
#define CxTRAIN_PI(ch)        (0x1490 + (ch)*0x100)  /* PI settings (8 DWORDs) */
#define CxRECy_MCHBAR(ch, g)  (0x14a0 + (ch)*0x100 + ((3-(g))*4))
#define REC_DQS_LEVEL(ch)     (0x14ac + (ch)*0x100)  /* DQS readback, bit 30 */
#define REC_COARSE_LOW(ch)    (0x14b0 + (ch)*0x100)  /* Sub-coarse, bits [3:2] */
#define CxRDTy_MCHBAR(ch, l)  (0x14b0 + (ch)*0x100 + ((7-(l))*4))
#define RW_PTR_CTRL(ch)       (0x14f0 + (ch)*0x100)

/* Training enable */
#define TRAIN_ENABLE(ch)      (0x12a4 + (ch)*0x100)
#define TRAIN_ENABLE_BIT      (1 << 31)

/* --- Power Management / Thermal Throttle --- */
#define PM_CH_BASE(ch)        (0x1000 + (ch)*0x40)
#define PM_CH_CMD(ch)         (0x1001 + (ch)*0x40)
#define PM_CH_CTRL(ch)        (0x1007 + (ch)*0x40)
#define PM_CH_THRT0(ch)       (0x1010 + (ch)*0x40)
#define PM_CH_THRT1(ch)       (0x1014 + (ch)*0x40)
#define PM_CH_TSC0(ch)        (0x1018 + (ch)*0x40)
#define PM_CH_TSC1(ch)        (0x101c + (ch)*0x40)
#define PM_CH_TSC1B(ch)       (0x101d + (ch)*0x40)
#define PM_CH_EXTTS(ch)       (0x1058 + (ch)*0x40)
#define PM_THRT_CTRL          0x1070
#define PM_THRT_MODE          0x1080
#define THERMAL_ENABLE        0x10ef

/* --- I/O Scheduling / Arbitration --- */
#define IOSCHED_BASE          0x1120  /* 4 x 32-bit scheduling regs */
#define IOSCHED_0             0x1120
#define IOSCHED_1             0x1124
#define IOSCHED_2             0x1128
#define IOSCHED_3             0x112c
#define IOSCHED_MISC          0x11b0
#define IOSCHED_CC            0x11cc  /* Scheduling control */
#define IOSCHED_190           0x1190
#define IOSCHED_19E           0x119e

/* ================================================================== */
/* D1:F0 PEG (PCI Express Graphics) Config Space                      */
/* ================================================================== */

#define PEG_SSID             0x2c  /* Subsystem ID */
#define PEG_LCAP             0xa4  /* Link Capabilities */
#define PEG_LCTL             0xa8  /* Link Control */
#define PEG_LSTS             0xaa  /* Link Status */
#define PEG_SLOTCAP          0xb4  /* Slot Capabilities */
#define PEG_CAP              0xa2  /* PCI Express Capabilities */
#define PEG_PEGLC            0xec  /* PEG Lane Control */
#define PEG_NEGWIDTH         0x224 /* Negotiated link width control */
#define D1F0_VCCAP           0x104 /* VC Capability (ext VC count) */
#define D1F0_VC0RCTL         0x114 /* VC0 Resource Control */

/* PEG_CAP bits */
#define PEG_CAP_SLOT_IMPL    (1 << 8)  /* Slot Implemented */

/* PEG SLOTCAP fields */
#define SLOTCAP_SLOTNUM_SHIFT  17
#define SLOTCAP_POWER_SCALE_SHIFT 15
#define SLOTCAP_POWER_LIMIT_SHIFT 7

/* PEG_PEGLC bits */
#define PEGLC_GPE_EN_ALL     0x07  /* All GPE enables */

/* ================================================================== */
/* DMIBAR Register Offsets (base = 0xFED18000)                        */
/* ================================================================== */

#define DMIVCECH       0x000
#define DMIPVCCAP1     0x004
#define DMIVC0RCAP     0x010
#define DMIVC0RCTL     0x014
#define DMIVC0RSTS     0x01a
#define DMIVC1RCAP     0x01c
#define DMIVC1RCTL     0x020
#define DMIVC1RSTS     0x026
#define VC1NP          (1 << 1)
#define DMIESD         0x044
#define DMILE1D        0x050
#define DMILE1A        0x058
#define DMILE2D        0x060
#define DMILE2A        0x068
#define DMILCAP        0x084
#define DMILCTL        0x088
#define DMILSTS        0x08a
#define DMILCTL2       0x204  /* DMI link control 2 */

/* ================================================================== */
/* EPBAR Register Offsets (base = 0xFED19000)                         */
/* ================================================================== */

#define EPPVCCAP1      0x004
#define EPPVCCTL       0x00c
#define EPVC0RCAP      0x010
#define EPVC0RCTL      0x014
#define EPVC0RSTS      0x01a
#define EPVC1RCAP      0x01c
#define EPVC1RCTL      0x020
#define EPVC1RSTS      0x026
#define EPVC1MTS       0x028
#define EPVC1ITC       0x02c
#define EPVC1IST       0x038
#define EPESD          0x044
#define EPLE1D         0x050
#define EPLE1A         0x058
#define EPLE2D         0x060
#define EPLE2A         0x068
#define EP_PORTARB(x)  (0x100 + 4*(x))

/* ================================================================== */
/* Constants                                                          */
/* ================================================================== */

#define NUM_CHANNELS  2
#define NUM_BYTELANES 8

/* ================================================================== */
/* Helpers                                                            */
/* ================================================================== */

static inline uint8_t northbridge_stepping(void)
{
	return pci_s_read_config8(D0F0, 0x08);
}

/* ================================================================== */
/* Enumerations                                                       */
/* ================================================================== */

/*
 * FSB / memory clock indices.
 *
 * These are the vendor BIOS internal indices, which map 1:1 to the
 * raw CLKCFG register encodings (MCHBAR + 0xC00):
 *
 *   FSB  bits [2:0] (RO): 001 = FSB-533, 010 = FSB-800, 011 = FSB-667
 *   DRAM bits [6:4]:       011 = DDR2-533, 100 = DDR2-667
 *
 * GM965 supports DDR2-533 and DDR2-667 only (CAPID0 bit 30).
 * Values start at 1 so that 0 means "uninitialized" in the CMOS cache.
 */
typedef enum {
	FSB_CLOCK_533MHz  = 1,  /* CLKCFG 001 */
	FSB_CLOCK_800MHz  = 2,  /* CLKCFG 010 -- TODO only FSB tested */
	FSB_CLOCK_667MHz  = 3,  /* CLKCFG 011 */
} fsb_clock_t;

typedef enum {
	MEM_CLOCK_533MT   = 1,  /* CLKCFG 011 -- DDR2-533 (266 MHz) */
	MEM_CLOCK_667MT   = 2,  /* CLKCFG 100 -- DDR2-667 (333 MHz) */
} mem_clock_t;

typedef enum {
	CHANNEL_MODE_SINGLE           = 0,
	CHANNEL_MODE_DUAL_ASYNC       = 1,
	CHANNEL_MODE_DUAL_INTERLEAVED = 2,
} channel_mode_t;

/* ================================================================== */
/* CMOS Raminit Config Cache                                          */
/* ================================================================== */

/*
 * 16-byte config buffer at CMOS 0x80-0x8F, matching the Phoenix BIOS
 * 9-byte SPD cache (FFFF35D7/FFFF36E4) but extended with training results.
 *
 * On cold boot: SPD is read, timings computed, training runs, then the
 * results are packed into CMOS.  On warm boot / S3 resume: CMOS is read
 * to skip SPD reads and receive-enable training.
 *
 * Byte  Contents
 * ----  --------
 *  0    Magic (0x96 = valid GM965 raminit cache)
 *  1    fsb_clock[1:0] | mem_clock[3:2] | channel_mode[5:4]
 *  2    CAS
 *  3    tRAS
 *  4    tRP[3:0] | tRCD[7:4]
 *  5    tRFC
 *  6    tWR[3:0] | tRRD[7:4]
 *  7    tRTP
 *  8    Flags: ch0_present[0] ch0_dual[1] ch0_banks8[2]
 *              ch1_present[3] ch1_dual[4] ch1_banks8[5]
 *  9    ch0 geometry: cols[3:0] | rank_cap_log2[7:4]
 * 10    ch1 geometry: cols[3:0] | rank_cap_log2[7:4]
 * 11    ch0 training: coarse_high[3:0] | coarse_low[5:4]
 * 12    ch0 training: fine[3:0] | ch1 fine[7:4]
 * 13    ch1 training: coarse_high[3:0] | coarse_low[5:4]
 * 14    checksum (XOR of bytes 0-13)
 */
#define CMOS_RAMINIT_BASE     0x80
#define CMOS_RAMINIT_SIZE     15   /* bytes 0-14 */
#define CMOS_RAMINIT_MAGIC    0x96

/* ================================================================== */
/* Data Structures                                                    */
/* ================================================================== */

/* Per-DIMM information (decoded from SPD by common DDR2 library) */
typedef struct {
	int      present;            /* 1 if DIMM is populated */
	int      dual_rank;          /* 1 if dual-rank */
	int      x16;                /* 1 if x16 width */
	uint8_t  rows;               /* number of row address bits */
	uint8_t  cols;               /* number of column address bits */
	uint8_t  banks;              /* number of banks (4 or 8) */
	uint16_t page_size;          /* page size in bytes */
	uint16_t rank_capacity_mb;   /* capacity per rank in MB */
	/* CAS/frequency info (decoded by common DDR2 SPD library) */
	uint8_t  cas_supported;      /* bitmask of supported CAS latencies */
	uint32_t cycle_time[8];      /* tCK per CAS level in 1/256 ns */
	/* Timing parameters in 1/256 ns (decoded by common DDR2 SPD library) */
	uint32_t tRCD;
	uint32_t tRP;
	uint32_t tRAS;
	uint32_t tWR;
	uint32_t tRRD;
	uint32_t tRTP;
} dimminfo_t;

/* Computed timing parameters */
typedef struct {
	uint8_t   CAS;
	uint8_t   tRAS;
	uint8_t   tRP;
	uint8_t   tRCD;
	uint8_t   tRFC;
	uint8_t   tWR;
	uint8_t   tRRD;
	uint8_t   tRTP;
	fsb_clock_t     fsb_clock;
	mem_clock_t     mem_clock;
	channel_mode_t  channel_mode;
} timings_t;

/* System information */
typedef struct {
	/* Northbridge info */
	uint8_t    stepping;
	uint32_t   capid0;

	/* DIMM info */
	dimminfo_t dimms[4];         /* logical SPD slots; GM965 uses slots 0 and 2 */
	int        dimm_count;
	int        channels;         /* 1 or 2 */

	/* Computed timings */
	timings_t  timings;

	/* Memory map */
	uint32_t   tolud_mb;         /* Top of Low Usable DRAM in MB */
	uint32_t   tom_mb;           /* Total memory in MB */
	int        s3_resume;        /* 1 if resuming from S3 */

	/* Receive-enable training results */
	uint8_t    rec_coarse[NUM_CHANNELS];     /* per-channel coarse_high delay */
	uint8_t    rec_coarse_low[NUM_CHANNELS]; /* per-channel sub-coarse delay */
	uint8_t    rec_fine[NUM_CHANNELS];       /* per-channel fine delay */

	/* SPD I2C addresses for each slot (0 if empty) */
	uint8_t    spd_addr_map[4];

	/* Raw SPD data (full 128 bytes) for identification fields */
	u8         raw_spd[4][SPD_SIZE_MAX_DDR2];
} sysinfo_t;

/* ================================================================== */
/* Function Prototypes                                                */
/* ================================================================== */

/* northbridge/intel/gm965/early_init.c */
void gm965_early_init(void);

/* northbridge/intel/gm965/raminit.c */
void raminit(sysinfo_t *si);

/* northbridge/intel/gm965/raminit_receive_enable_calibration.c */
void receive_enable_training(sysinfo_t *si);
void raminit_program_training(sysinfo_t *si);

/* northbridge/intel/gm965/raminit_meminfo.c */
void setup_sdram_meminfo(const sysinfo_t *sysinfo);

/* northbridge/intel/gm965/dmi.c */
void gm965_dmi_init(void);

/* northbridge/intel/gm965/thermal.c */
void gm965_thermal_init(sysinfo_t *si);

/* northbridge/intel/gm965/pm.c */
void gm965_pm_init(const sysinfo_t *si);

/* northbridge/intel/gm965/igd.c */
void gm965_igd_init(const sysinfo_t *si);
u16 igd_compute_ggc(void);

/* northbridge/intel/gm965/memmap.c */
u32 decode_tseg_size(u8 esmramc);
u32 decode_igd_memory_size(u32 gms);

#include <device/device.h>

/* northbridge/intel/gm965/pcie.c */
void gm965_pcie_init(struct device *peg);

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long start,
					    struct acpi_rsdp *rsdp);

/* romstage mainboard hookups */
void mb_pre_raminit_setup(sysinfo_t *si); /* optional, weak */
void mb_post_raminit_setup(void); /* optional, weak */
void mainboard_lpc_decode(void); /* optional, weak */
void mainboard_get_spd_map(u8 spd_map[4]);

#endif /* _GM965_H_ */
