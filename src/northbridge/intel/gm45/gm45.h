/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *               2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __NORTHBRIDGE_INTEL_GM45_GM45_H__
#define __NORTHBRIDGE_INTEL_GM45_GM45_H__ 1

#include "southbridge/intel/i82801ix/i82801ix.h"

#ifndef __ACPI__

#include <stdint.h>

typedef enum {
	FSB_CLOCK_1067MHz	= 0,
	FSB_CLOCK_800MHz	= 1,
	FSB_CLOCK_667MHz	= 2,
} fsb_clock_t;

typedef enum { /* Steppings below B1 were pre-production,
		  conversion stepping A1 is... ?
		  We'll support B1, B2, B3, and conversion stepping A1. */
	STEPPING_A0 = 0,
	STEPPING_A1 = 1,
	STEPPING_A2 = 2,
	STEPPING_A3 = 3,
	STEPPING_B0 = 4,
	STEPPING_B1 = 5,
	STEPPING_B2 = 6,
	STEPPING_B3 = 7,
	STEPPING_CONVERSION_A1 = 9,
} stepping_t;

typedef enum {
	GMCH_GM45 = 0,
	GMCH_GM47,
	GMCH_GM49,
	GMCH_GE45,
	GMCH_GL40,
	GMCH_GL43,
	GMCH_GS40,
	GMCH_GS45,
	GMCH_PM45,
	GMCH_UNKNOWN
} gmch_gfx_t;

typedef enum {
	MEM_CLOCK_533MHz  = 0,
	MEM_CLOCK_400MHz  = 1,
	MEM_CLOCK_333MHz  = 2,
	MEM_CLOCK_1067MT  = 0,
	MEM_CLOCK_800MT   = 1,
	MEM_CLOCK_667MT   = 2,
} mem_clock_t;

typedef enum {
	DDR1 = 1,
	DDR2 = 2,
	DDR3 = 3,
} ddr_t;

typedef enum {
	CHANNEL_MODE_SINGLE,
	CHANNEL_MODE_DUAL_ASYNC,
	CHANNEL_MODE_DUAL_INTERLEAVED,
} channel_mode_t;

typedef enum { /* as in DDR3 spd */
	CHIP_WIDTH_x4	= 0,
	CHIP_WIDTH_x8	= 1,
	CHIP_WIDTH_x16	= 2,
	CHIP_WIDTH_x32	= 3,
} chip_width_t;

typedef enum { /* as in DDR3 spd */
	CHIP_CAP_256M	= 0,
	CHIP_CAP_512M	= 1,
	CHIP_CAP_1G	= 2,
	CHIP_CAP_2G	= 3,
	CHIP_CAP_4G	= 4,
	CHIP_CAP_8G	= 5,
	CHIP_CAP_16G	= 6,
} chip_capacity_t;

typedef struct {
	unsigned int	CAS;
	fsb_clock_t	fsb_clock;
	mem_clock_t	mem_clock;
	channel_mode_t	channel_mode;
	unsigned int	tRAS;
	unsigned int	tRP;
	unsigned int	tRCD;
	unsigned int	tRFC;
	unsigned int	tWR;
	unsigned int	tRD;
	unsigned int	tRRD;
	unsigned int	tFAW;
	unsigned int	tWL;
} timings_t;

typedef struct {
	unsigned int	card_type; /* 0x0: unpopulated,
				      0xa - 0xf: raw card type A - F */
	chip_width_t	chip_width;
	chip_capacity_t	chip_capacity;
	unsigned int	page_size; /* of whole DIMM in Bytes (4096 or 8192) */
	unsigned int	banks;
	unsigned int	ranks;
	unsigned int	rank_capacity_mb; /* per rank in Mega Bytes */
} dimminfo_t;

/* The setup is one DIMM per channel, so there's no need to find a
   common timing setup between multiple chips (but chip and controller
   still need to be coordinated */
typedef struct {
	stepping_t	stepping;
	int		txt_enabled;
	int		cores;
	gmch_gfx_t	gfx_type;
	int		gs45_low_power_mode; /* low power mode of GMCH_GS45 */
	int		max_ddr2_mhz;
	int		max_ddr3_mt;
	fsb_clock_t	max_fsb;
	int		max_fsb_mhz;
	int		max_render_mhz;

	int		spd_type;
	timings_t	selected_timings;
	dimminfo_t	dimms[2];
} sysinfo_t;
#define TOTAL_CHANNELS 2
#define CHANNEL_IS_POPULATED(dimms, idx) (dimms[idx].card_type != 0)
#define CHANNEL_IS_CARDF(dimms, idx) (dimms[idx].card_type == 0xf)
#define IF_CHANNEL_POPULATED(dimms, idx) if (dimms[idx].card_type != 0)
#define FOR_EACH_CHANNEL(idx) \
	for (idx = 0; idx < TOTAL_CHANNELS; ++idx)
#define FOR_EACH_POPULATED_CHANNEL(dimms, idx) \
	FOR_EACH_CHANNEL(idx) IF_CHANNEL_POPULATED(dimms, idx)

#define RANKS_PER_CHANNEL 4 /* Only two may be populated */
#define IF_RANK_POPULATED(dimms, ch, r) \
	if (dimms[ch].card_type && ((r) < dimms[ch].ranks))
#define FOR_EACH_RANK_IN_CHANNEL(r) \
	for (r = 0; r < RANKS_PER_CHANNEL; ++r)
#define FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, r) \
	FOR_EACH_RANK_IN_CHANNEL(r) IF_RANK_POPULATED(dimms, ch, r)
#define FOR_EACH_RANK(ch, r) \
	FOR_EACH_CHANNEL(ch) FOR_EACH_RANK_IN_CHANNEL(r)
#define FOR_EACH_POPULATED_RANK(dimms, ch, r) \
	FOR_EACH_RANK(ch, r) IF_RANK_POPULATED(dimms, ch, r)

#define DDR3_MAX_CAS 18

enum {
	VCO_2666 = 4,
	VCO_3200 = 0,
	VCO_4000 = 1,
	VCO_5333 = 2,
};

#endif

/* Offsets of read/write training results in CMOS.
   They will be restored upon S3 resumes. */
#define CMOS_READ_TRAINING	0x80 /* 16 bytes */
#define CMOS_WRITE_TRAINING	0x90 /* 16 bytes
					(could be reduced to 10 bytes) */


#define DEFAULT_MCHBAR		0xfed14000
#define DEFAULT_DMIBAR		0xfed18000
#define DEFAULT_EPBAR		0xfed19000
#define DEFAULT_HECIBAR		0xfed1a000

				/* 4 KB per PCIe device */
#define DEFAULT_PCIEXBAR	CONFIG_MMCONF_BASE_ADDRESS

#define IOMMU_BASE1 0xfed90000
#define IOMMU_BASE2 0xfed91000
#define IOMMU_BASE3 0xfed92000
#define IOMMU_BASE4 0xfed93000

/*
 * D0:F0
 */
#define D0F0_EPBAR_LO 0x40
#define D0F0_EPBAR_HI 0x44
#define D0F0_MCHBAR_LO 0x48
#define D0F0_MCHBAR_HI 0x4c
#define D0F0_GGC 0x52
#define D0F0_DEVEN 0x54
#define D0F0_PCIEXBAR_LO 0x60
#define D0F0_PCIEXBAR_HI 0x64
#define D0F0_DMIBAR_LO 0x68
#define D0F0_DMIBAR_HI 0x6c
#define D0F0_PMBASE 0x78
#define D0F0_PAM(x) (0x90+(x)) /* 0-6*/
#define D0F0_REMAPBASE 0x98
#define D0F0_REMAPLIMIT 0x9a
#define D0F0_SMRAM 0x9d
#define D0F0_ESMRAMC 0x9e
#define D0F0_TOM 0xa0
#define D0F0_TOUUD 0xa2
#define D0F0_TOLUD 0xb0
#define D0F0_SKPD 0xdc /* Scratchpad Data */
#define D0F0_CAPID0 0xe0

/*
 * D1:F0 PEG
 */
#define PEG_CAP 0xa2
#define SLOTCAP 0xb4
#define PEGLC 0xec
#define D1F0_VCCAP 0x104
#define D1F0_VC0RCTL 0x114

/*
 * Graphics frequencies
 */
#define GCFGC_PCIDEV		PCI_DEV(0, 2, 0)
#define GCFGC_OFFSET		0xf0
#define GCFGC_CR_SHIFT		0
#define GCFGC_CR_MASK		(0xf << GCFGC_CR_SHIFT)
#define GCFGC_CS_SHIFT		8
#define GCFGC_CS_MASK		(0xf << GCFGC_CS_SHIFT)
#define GCFGC_CD_SHIFT		12
#define GCFGC_CD_MASK		(0x1 << GCFGC_CD_SHIFT)
#define GCFGC_UPDATE_SHIFT	5
#define GCFGC_UPDATE		(0x1 << GCFGC_UPDATE_SHIFT)

/*
 * MCHBAR
 */

#define MCHBAR8(x) *((volatile u8 *)(DEFAULT_MCHBAR + x))
#define MCHBAR16(x) *((volatile u16 *)(DEFAULT_MCHBAR + x))
#define MCHBAR32(x) *((volatile u32 *)(DEFAULT_MCHBAR + x))

#define PMSTS_MCHBAR		0x0f14	/* Self refresh channel status */
#define PMSTS_WARM_RESET	(1 << 1)
#define PMSTS_BOTH_SELFREFRESH	(1 << 0)

#define CLKCFG_MCHBAR		0x0c00
#define CLKCFG_FSBCLK_SHIFT	0
#define CLKCFG_FSBCLK_MASK	(7 << CLKCFG_FSBCLK_SHIFT)
#define CLKCFG_MEMCLK_SHIFT	4
#define CLKCFG_MEMCLK_MASK	(7 << CLKCFG_MEMCLK_SHIFT)
#define CLKCFG_UPDATE		(1 << 12)

#define SSKPD_MCHBAR		0x0c1c
#define SSKPD_CLK_SHIFT		0
#define SSKPD_CLK_MASK		(7 << SSKPD_CLK_SHIFT)

#define DCC_MCHBAR		0x200
#define DCC_NO_CHANXOR		(1 << 10)
#define DCC_INTERLEAVED		(1 <<  1)
#define DCC_CMD_SHIFT		16
#define DCC_CMD_MASK		(7 << DCC_CMD_SHIFT)
#define DCC_CMD_NOP		(1 << DCC_CMD_SHIFT)
				/* For mode register mr0: */
#define DCC_SET_MREG		(3 << DCC_CMD_SHIFT)
				/* For extended mode registers mr1 to mr3: */
#define DCC_SET_EREG		(4 << DCC_CMD_SHIFT)
#define DCC_SET_EREG_SHIFT	21
#define DCC_SET_EREG_MASK	(DCC_CMD_MASK | (3 << DCC_SET_EREG_SHIFT))
#define DCC_SET_EREGx(x)	((DCC_SET_EREG |			   \
					((x - 1) << DCC_SET_EREG_SHIFT)) & \
				 DCC_SET_EREG_MASK)

/* Per channel DRAM Row Attribute registers (32-bit) */
#define CxDRA_MCHBAR(x)		(0x1208 + (x * 0x0100))
#define CxDRA_PAGESIZE_SHIFT(r)	(r * 4)		/* Per rank r */
#define CxDRA_PAGESIZE_MASKr(r)	(0x7 << CxDRA_PAGESIZE_SHIFT(r))
#define CxDRA_PAGESIZE_MASK	0x0000ffff
#define CxDRA_PAGESIZE(r, p)	/* for log2(dimm page size in bytes) p */ \
	(((p - 10) << CxDRA_PAGESIZE_SHIFT(r)) & CxDRA_PAGESIZE_MASKr(r))
#define CxDRA_BANKS_SHIFT(r)	((r * 3) + 16)
#define CxDRA_BANKS_MASKr(r)	(0x3 << CxDRA_BANKS_SHIFT(r))
#define CxDRA_BANKS_MASK	0x07ff0000
#define CxDRA_BANKS(r, b)	/* for number of banks b */ \
	((b << (CxDRA_BANKS_SHIFT(r) - 3)) & CxDRA_BANKS_MASKr(r))

/*
 * Per channel DRAM Row Boundary registers (32-bit)
 * Every two ranks share one register and must be programmed at the same time.
 * All registers (4 ranks per channel) have to be set.
 */
#define CxDRBy_MCHBAR(x, r)	(0x1200 + (x * 0x0100) + ((r/2) * 4))
#define CxDRBy_BOUND_SHIFT(r)	((r % 2) * 16)
#define CxDRBy_BOUND_MASK(r)	(0x1fc << CxDRBy_BOUND_SHIFT(r))
#define CxDRBy_BOUND_MB(r, b)	/* for boundary in MB b */ \
	(((b >> 5) << CxDRBy_BOUND_SHIFT(r)) & CxDRBy_BOUND_MASK(r))

#define CxDRC0_MCHBAR(x)	(0x1230 + (x * 0x0100))
#define CxDRC0_RANKEN0		(1 << 24)	/* Rank Enable */
#define CxDRC0_RANKEN1		(1 << 25)
#define CxDRC0_RANKEN2		(1 << 26)
#define CxDRC0_RANKEN3		(1 << 27)
#define CxDRC0_RANKEN(r)	(1 << (24 + r))
#define CxDRC0_RANKEN_MASK	(0xf << 24)
#define CxDRC0_RMS_SHIFT	8		/* Refresh Mode Select */
#define CxDRC0_RMS_MASK		(7 << CxDRC0_RMS_SHIFT)
#define CxDRC0_RMS_78US		(2 << CxDRC0_RMS_SHIFT)
#define CxDRC0_RMS_39US		(3 << CxDRC0_RMS_SHIFT)

#define CxDRC1_MCHBAR(x)	(0x1234 + (x * 0x0100))
#define CxDRC1_SSDS_SHIFT	24
#define CxDRC1_SSDS_MASK	(0xff << CxDRC1_SSDS_SHIFT)
#define CxDRC1_DS		(0x91 << CxDRC1_SSDS_SHIFT)
#define CxDRC1_SS		(0xb1 << CxDRC1_SSDS_SHIFT)
#define CxDRC1_NOTPOP(r)	(1 << (16 + r)) /* Write 1 for Not Populated */
#define CxDRC1_NOTPOP_MASK	(0xf << 16)
#define CxDRC1_MUSTWR		(3 << 11)

#define CxDRC2_MCHBAR(x)	(0x1238 + (x * 0x0100))
#define CxDRC2_NOTPOP(r)	(1 << (24 + r)) /* Write 1 for Not Populated */
#define CxDRC2_NOTPOP_MASK	(0xf << 24)
#define CxDRC2_MUSTWR		(1 << 12)
#define CxDRC2_CLK1067MT	(1 << 0)

/* DRAM Timing registers (32-bit each) */
#define CxDRT0_MCHBAR(x)	(0x1210 + (x * 0x0100))
#define CxDRT0_BtB_WtP_SHIFT	26
#define CxDRT0_BtB_WtP_MASK	(0x1f << CxDRT0_BtB_WtP_SHIFT)
#define CxDRT0_BtB_WtR_SHIFT	20
#define CxDRT0_BtB_WtR_MASK	(0x1f << CxDRT0_BtB_WtR_SHIFT)
#define CxDRT1_MCHBAR(x)	(0x1214 + (x * 0x0100))
#define CxDRT2_MCHBAR(x)	(0x1218 + (x * 0x0100))
#define CxDRT3_MCHBAR(x)	(0x121c + (x * 0x0100))
#define CxDRT4_MCHBAR(x)	(0x1220 + (x * 0x0100))
#define CxDRT5_MCHBAR(x)	(0x1224 + (x * 0x0100))
#define CxDRT6_MCHBAR(x)	(0x1228 + (x * 0x0100))

/* Clock disable registers (32-bit each) */
#define CxDCLKDIS_MCHBAR(x)	(0x120c + (x * 0x0100))
#define CxDCLKDIS_MASK		3
#define CxDCLKDIS_ENABLE	3 /* Always enable both clock pairs. */

/* On-Die-Termination registers (2x 32-bit per channel) */
#define CxODT_HIGH(x)		(0x124c + (x * 0x0100))
#define CxODT_LOW(x)		(0x1248 + (x * 0x0100))

/* Write Training registers. */
#define CxWRTy_MCHBAR(ch, s)	(0x1470 + (ch * 0x0100) + ((3 - s) * 4))

#define CxGTEW(x)		(0x1270+(x*0x100))
#define CxGTC(x)		(0x1274+(x*0x100))
#define CxDTPEW(x)		(0x1278+(x*0x100))
#define CxDTAEW(x)		(0x1280+(x*0x100))
#define CxDTC(x)		(0x1288+(x*0x100))


/*
 * DMIBAR
 */

#define DMIBAR8(x) *((volatile u8 *)(DEFAULT_DMIBAR + x))
#define DMIBAR16(x) *((volatile u16 *)(DEFAULT_DMIBAR + x))
#define DMIBAR32(x) *((volatile u32 *)(DEFAULT_DMIBAR + x))

#define DMIVC0RCTL 0x14
#define DMIVC1RCTL 0x20
#define DMIVC1RSTS 0x26
#define DMIESD  0x44
#define DMILE1D 0x50
#define DMILE1A 0x58
#define DMILE2D 0x60
#define DMILE2A 0x68


/*
 * EPBAR
 */

#define EPBAR8(x) *((volatile u8 *)(DEFAULT_EPBAR + x))
#define EPBAR16(x) *((volatile u16 *)(DEFAULT_EPBAR + x))
#define EPBAR32(x) *((volatile u32 *)(DEFAULT_EPBAR + x))

#define EPESD 0x44
#define EPLE1D 0x50
#define EPLE1A 0x58
#define EPLE2D 0x60


#ifndef __ACPI__
void gm45_early_init(void);
void gm45_early_reset(void);

void enter_raminit_or_reset(void);
void get_gmch_info(sysinfo_t *);
void raminit(sysinfo_t *, int s3resume);
void raminit_thermal(const sysinfo_t *);
void init_igd(const sysinfo_t *, int no_igd, int no_peg);
void init_pm(const sysinfo_t *);

int raminit_read_vco_index(void);
u32 raminit_get_rank_addr(unsigned int channel, unsigned int rank);

void raminit_rcomp_calibration(stepping_t stepping);
void raminit_reset_readwrite_pointers(void);
void raminit_receive_enable_calibration(const timings_t *, const dimminfo_t *);
void raminit_write_training(const mem_clock_t, const dimminfo_t *, int s3resume);
void raminit_read_training(const dimminfo_t *, int s3resume);

void gm45_late_init(stepping_t);

u32 decode_igd_memory_size(u32 gms);
u32 decode_igd_gtt_size(u32 gsm);
u32 get_top_of_ram(void);

void init_iommu(void);
#endif

#endif
