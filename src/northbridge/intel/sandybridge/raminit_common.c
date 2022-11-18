/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <northbridge/intel/sandybridge/chip.h>
#include <device/pci_def.h>
#include <delay.h>
#include <types.h>

#include "raminit_native.h"
#include "raminit_common.h"
#include "raminit_tables.h"
#include "sandybridge.h"

/* FIXME: no support for 3-channel chipsets */

static void sfence(void)
{
	asm volatile ("sfence");
}

/* Toggle IO reset bit */
static void toggle_io_reset(void)
{
	u32 r32 = mchbar_read32(MC_INIT_STATE_G);
	mchbar_write32(MC_INIT_STATE_G, r32 |  (1 << 5));
	udelay(1);
	mchbar_write32(MC_INIT_STATE_G, r32 & ~(1 << 5));
	udelay(1);
}

static u32 get_XOVER_CLK(u8 rankmap)
{
	return rankmap << 24;
}

static u32 get_XOVER_CMD(u8 rankmap)
{
	u32 reg;

	/* Enable xover cmd */
	reg = 1 << 14;

	/* Enable xover ctl */
	if (rankmap & 0x03)
		reg |= (1 << 17);

	if (rankmap & 0x0c)
		reg |= (1 << 26);

	return reg;
}

void dram_find_common_params(ramctr_timing *ctrl)
{
	size_t valid_dimms;
	int channel, slot;
	dimm_info *dimms = &ctrl->info;

	ctrl->cas_supported = (1 << (MAX_CAS - MIN_CAS + 1)) - 1;
	valid_dimms = 0;

	FOR_ALL_CHANNELS for (slot = 0; slot < 2; slot++) {

		const struct dimm_attr_ddr3_st *dimm = &dimms->dimm[channel][slot];
		if (dimm->dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3)
			continue;

		valid_dimms++;

		/* Find all possible CAS combinations */
		ctrl->cas_supported &= dimm->cas_supported;

		/* Find the smallest common latencies supported by all DIMMs */
		ctrl->tCK  = MAX(ctrl->tCK,  dimm->tCK);
		ctrl->tAA  = MAX(ctrl->tAA,  dimm->tAA);
		ctrl->tWR  = MAX(ctrl->tWR,  dimm->tWR);
		ctrl->tRCD = MAX(ctrl->tRCD, dimm->tRCD);
		ctrl->tRRD = MAX(ctrl->tRRD, dimm->tRRD);
		ctrl->tRP  = MAX(ctrl->tRP,  dimm->tRP);
		ctrl->tRAS = MAX(ctrl->tRAS, dimm->tRAS);
		ctrl->tRFC = MAX(ctrl->tRFC, dimm->tRFC);
		ctrl->tWTR = MAX(ctrl->tWTR, dimm->tWTR);
		ctrl->tRTP = MAX(ctrl->tRTP, dimm->tRTP);
		ctrl->tFAW = MAX(ctrl->tFAW, dimm->tFAW);
		ctrl->tCWL = MAX(ctrl->tCWL, dimm->tCWL);
		ctrl->tCMD = MAX(ctrl->tCMD, dimm->tCMD);
	}

	if (!ctrl->cas_supported)
		die("Unsupported DIMM combination. DIMMS do not support common CAS latency");

	if (!valid_dimms)
		die("No valid DIMMs found");
}

void dram_xover(ramctr_timing *ctrl)
{
	u32 reg;
	int channel;

	FOR_ALL_CHANNELS {
		/* Enable xover clk */
		reg = get_XOVER_CLK(ctrl->rankmap[channel]);
		printram("XOVER CLK [%x] = %x\n", GDCRCKPICODE_ch(channel), reg);
		mchbar_write32(GDCRCKPICODE_ch(channel), reg);

		/* Enable xover ctl & xover cmd */
		reg = get_XOVER_CMD(ctrl->rankmap[channel]);
		printram("XOVER CMD [%x] = %x\n", GDCRCMDPICODING_ch(channel), reg);
		mchbar_write32(GDCRCMDPICODING_ch(channel), reg);
	}
}

static void dram_odt_stretch(ramctr_timing *ctrl, int channel)
{
	u32 addr, stretch;

	stretch = ctrl->ref_card_offset[channel];
	/*
	 * ODT stretch:
	 * Delay ODT signal by stretch value. Useful for multi DIMM setups on the same channel.
	 */
	if (IS_SANDY_CPU(ctrl->cpu) && IS_SANDY_CPU_C(ctrl->cpu)) {
		if (stretch == 2)
			stretch = 3;

		addr = SCHED_SECOND_CBIT_ch(channel);
		mchbar_clrsetbits32(addr, 0xf << 10, stretch << 12 | stretch << 10);
		printk(RAM_DEBUG, "OTHP Workaround [%x] = %x\n", addr, mchbar_read32(addr));
	} else {
		addr = TC_OTHP_ch(channel);
		union tc_othp_reg tc_othp = {
			.raw = mchbar_read32(addr),
		};
		tc_othp.odt_delay_d0 = stretch;
		tc_othp.odt_delay_d1 = stretch;
		mchbar_write32(addr, tc_othp.raw);
		printk(RAM_DEBUG, "OTHP [%x] = %x\n", addr, mchbar_read32(addr));
	}
}

void dram_timing_regs(ramctr_timing *ctrl)
{
	int channel;

	/* BIN parameters */
	const union tc_dbp_reg tc_dbp = {
		.tRCD = ctrl->tRCD,
		.tRP  = ctrl->tRP,
		.tAA  = ctrl->CAS,
		.tCWL = ctrl->CWL,
		.tRAS = ctrl->tRAS,
	};

	/* Regular access parameters */
	const union tc_rap_reg tc_rap = {
		.tRRD = ctrl->tRRD,
		.tRTP = ctrl->tRTP,
		.tCKE = ctrl->tCKE,
		.tWTR = ctrl->tWTR,
		.tFAW = ctrl->tFAW,
		.tWR  = ctrl->tWR,
		.tCMD = 3,
	};

	/* Other parameters */
	const union tc_othp_reg tc_othp = {
		.tXPDLL  = MIN(ctrl->tXPDLL, 31),
		.tXP     = MIN(ctrl->tXP, 7),
		.tAONPD  = ctrl->tAONPD,
		.tCPDED  = 2,
		.tPRPDEN = 1,
	};

	/*
	 * If tXP and tXPDLL are very high, they no longer fit in the bitfields
	 * of the TC_OTHP register. If so, we set bits in TC_DTP to compensate.
	 * This can only happen on Ivy Bridge, and when overclocking the RAM.
	 */
	const union tc_dtp_reg tc_dtp = {
		.overclock_tXP    = ctrl->tXP >= 8,
		.overclock_tXPDLL = ctrl->tXPDLL >= 32,
	};

	/*
	 * TC-Refresh timing parameters:
	 *   The tREFIx9 field should be programmed to minimum of 8.9 * tREFI (to allow
	 *   for possible delays from ZQ or isoc) and tRASmax (70us) divided by 1024.
	 */
	const u32 val32 = MIN((ctrl->tREFI * 89) / 10, (70000 << 8) / ctrl->tCK);

	const union tc_rftp_reg tc_rftp = {
		.tREFI   = ctrl->tREFI,
		.tRFC    = ctrl->tRFC,
		.tREFIx9 = val32 / 1024,
	};

	/* Self-refresh timing parameters */
	const union tc_srftp_reg tc_srftp = {
		.tXSDLL     = tDLLK,
		.tXS_offset = ctrl->tXSOffset,
		.tZQOPER    = tDLLK - ctrl->tXSOffset,
		.tMOD       = ctrl->tMOD - 8,
	};

	FOR_ALL_CHANNELS {
		printram("DBP [%x] = %x\n", TC_DBP_ch(channel), tc_dbp.raw);
		mchbar_write32(TC_DBP_ch(channel), tc_dbp.raw);

		printram("RAP [%x] = %x\n", TC_RAP_ch(channel), tc_rap.raw);
		mchbar_write32(TC_RAP_ch(channel), tc_rap.raw);

		printram("OTHP [%x] = %x\n", TC_OTHP_ch(channel), tc_othp.raw);
		mchbar_write32(TC_OTHP_ch(channel), tc_othp.raw);

		if (IS_IVY_CPU(ctrl->cpu)) {
			/* Debug parameters - only applies to Ivy Bridge */
			mchbar_write32(TC_DTP_ch(channel), tc_dtp.raw);
		}

		dram_odt_stretch(ctrl, channel);

		printram("REFI [%x] = %x\n", TC_RFTP_ch(channel), tc_rftp.raw);
		mchbar_write32(TC_RFTP_ch(channel), tc_rftp.raw);

		union tc_rfp_reg tc_rfp = {
			.raw = mchbar_read32(TC_RFP_ch(channel)),
		};
		tc_rfp.oref_ri = 0xff;
		mchbar_write32(TC_RFP_ch(channel), tc_rfp.raw);

		printram("SRFTP [%x] = %x\n", TC_SRFTP_ch(channel), tc_srftp.raw);
		mchbar_write32(TC_SRFTP_ch(channel), tc_srftp.raw);
	}
}

void dram_dimm_mapping(ramctr_timing *ctrl)
{
	int channel;
	dimm_info *info = &ctrl->info;

	FOR_ALL_CHANNELS {
		struct dimm_attr_ddr3_st *dimmA, *dimmB;
		u32 reg = 0;

		if (info->dimm[channel][0].size_mb >= info->dimm[channel][1].size_mb) {
			dimmA = &info->dimm[channel][0];
			dimmB = &info->dimm[channel][1];
			reg |= (0 << 16);
		} else {
			dimmA = &info->dimm[channel][1];
			dimmB = &info->dimm[channel][0];
			reg |= (1 << 16);
		}

		if (dimmA && (dimmA->ranks > 0)) {
			reg |= (dimmA->size_mb / 256) <<  0;
			reg |= (dimmA->ranks - 1)     << 17;
			reg |= (dimmA->width / 8 - 1) << 19;
		}

		if (dimmB && (dimmB->ranks > 0)) {
			reg |= (dimmB->size_mb / 256) <<  8;
			reg |= (dimmB->ranks - 1)     << 18;
			reg |= (dimmB->width / 8 - 1) << 20;
		}

		/*
		 * Rank interleave: Bit 16 of the physical address space sets
		 * the rank to use in a dual single rank DIMM configuration.
		 * That results in every 64KiB being interleaved between two ranks.
		 */
		reg |= 1 << 21;
		/* Enhanced interleave */
		reg |= 1 << 22;

		if ((dimmA && (dimmA->ranks > 0)) || (dimmB && (dimmB->ranks > 0))) {
			ctrl->mad_dimm[channel] = reg;
		} else {
			ctrl->mad_dimm[channel] = 0;
		}
	}
}

void dram_dimm_set_mapping(ramctr_timing *ctrl, int training)
{
	int channel;
	u32 ecc;

	if (ctrl->ecc_enabled)
		ecc = training ? (1 << 24) : (3 << 24);
	else
		ecc = 0;

	FOR_ALL_CHANNELS {
		mchbar_write32(MAD_DIMM(channel), ctrl->mad_dimm[channel] | ecc);
	}

	if (ctrl->ecc_enabled)
		udelay(10);
}

void dram_zones(ramctr_timing *ctrl, int training)
{
	u32 reg, ch0size, ch1size;
	u8 val;
	reg = 0;
	val = 0;

	if (training) {
		ch0size = ctrl->channel_size_mb[0] ? 256 : 0;
		ch1size = ctrl->channel_size_mb[1] ? 256 : 0;
	} else {
		ch0size = ctrl->channel_size_mb[0];
		ch1size = ctrl->channel_size_mb[1];
	}

	if (ch0size >= ch1size) {
		reg = mchbar_read32(MAD_ZR);
		val = ch1size / 256;
		reg = (reg & ~0xff000000) | val << 24;
		reg = (reg & ~0x00ff0000) | (2 * val) << 16;
		mchbar_write32(MAD_ZR, reg);
		mchbar_write32(MAD_CHNL, 0x24);

	} else {
		reg = mchbar_read32(MAD_ZR);
		val = ch0size / 256;
		reg = (reg & ~0xff000000) | val << 24;
		reg = (reg & ~0x00ff0000) | (2 * val) << 16;
		mchbar_write32(MAD_ZR, reg);
		mchbar_write32(MAD_CHNL, 0x21);
	}
}

/*
 * Returns the ECC mode the NB is running at. It takes precedence over ECC capability.
 * The ME/PCU/.. has the ability to change this.
 * Return 0: ECC is optional
 * Return 1: ECC is forced
 */
bool get_host_ecc_forced(void)
{
	/* read Capabilities A Register */
	const u32 reg32 = pci_read_config32(HOST_BRIDGE, CAPID0_A);
	return !!(reg32 & (1 << 24));
}

/*
 * Returns the ECC capability.
 * The ME/PCU/.. has the ability to change this.
 * Return 0: ECC is disabled
 * Return 1: ECC is possible
 */
bool get_host_ecc_cap(void)
{
	/* read Capabilities A Register */
	const u32 reg32 = pci_read_config32(HOST_BRIDGE, CAPID0_A);
	return !(reg32 & (1 << 25));
}

#define DEFAULT_PCI_MMIO_SIZE 2048

void dram_memorymap(ramctr_timing *ctrl, int me_uma_size)
{
	u32 reg, val, reclaim, tom, gfxstolen, gttsize;
	size_t tsegbase, toludbase, remapbase, gfxstolenbase, mmiosize, gttbase;
	size_t tsegsize, touudbase, remaplimit, mestolenbase, tsegbasedelta;
	uint16_t ggc;

	mmiosize = DEFAULT_PCI_MMIO_SIZE;

	ggc = pci_read_config16(HOST_BRIDGE, GGC);
	if (!(ggc & 2)) {
		gfxstolen = ((ggc >> 3) & 0x1f) * 32;
		gttsize   = ((ggc >> 8) & 0x3);
	} else {
		gfxstolen = 0;
		gttsize   = 0;
	}

	tsegsize = CONFIG_SMM_TSEG_SIZE >> 20;

	tom = ctrl->channel_size_mb[0] + ctrl->channel_size_mb[1];

	mestolenbase = tom - me_uma_size;

	toludbase = MIN(4096 - mmiosize + gfxstolen + gttsize + tsegsize, tom - me_uma_size);

	gfxstolenbase = toludbase - gfxstolen;
	gttbase = gfxstolenbase - gttsize;

	tsegbase = gttbase - tsegsize;

	/* Round tsegbase down to nearest address aligned to tsegsize */
	tsegbasedelta = tsegbase & (tsegsize - 1);
	tsegbase &= ~(tsegsize - 1);

	gttbase -= tsegbasedelta;
	gfxstolenbase -= tsegbasedelta;
	toludbase -= tsegbasedelta;

	/* Test if it is possible to reclaim a hole in the RAM addressing */
	if (tom - me_uma_size > toludbase) {
		/* Reclaim is possible */
		reclaim    = 1;
		remapbase  = MAX(4096, tom - me_uma_size);
		remaplimit = remapbase + MIN(4096, tom - me_uma_size) - toludbase - 1;
		touudbase  = remaplimit + 1;
	} else {
		/* Reclaim not possible */
		reclaim   = 0;
		touudbase = tom - me_uma_size;
	}

	/* Update memory map in PCIe configuration space */
	printk(BIOS_DEBUG, "Update PCI-E configuration space:\n");

	/* TOM (top of memory) */
	reg = pci_read_config32(HOST_BRIDGE, TOM);
	val = tom & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", TOM, reg);
	pci_write_config32(HOST_BRIDGE, TOM, reg);

	reg = pci_read_config32(HOST_BRIDGE, TOM + 4);
	val = tom & 0xfffff000;
	reg = (reg & ~0x000fffff) | (val >> 12);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", TOM + 4, reg);
	pci_write_config32(HOST_BRIDGE, TOM + 4, reg);

	/* TOLUD (Top Of Low Usable DRAM) */
	reg = pci_read_config32(HOST_BRIDGE, TOLUD);
	val = toludbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", TOLUD, reg);
	pci_write_config32(HOST_BRIDGE, TOLUD, reg);

	/* TOUUD LSB (Top Of Upper Usable DRAM) */
	reg = pci_read_config32(HOST_BRIDGE, TOUUD);
	val = touudbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", TOUUD, reg);
	pci_write_config32(HOST_BRIDGE, TOUUD, reg);

	/* TOUUD MSB */
	reg = pci_read_config32(HOST_BRIDGE, TOUUD + 4);
	val = touudbase & 0xfffff000;
	reg = (reg & ~0x000fffff) | (val >> 12);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", TOUUD + 4, reg);
	pci_write_config32(HOST_BRIDGE, TOUUD + 4, reg);

	if (reclaim) {
		/* REMAP BASE */
		pci_write_config32(HOST_BRIDGE, REMAPBASE,     remapbase << 20);
		pci_write_config32(HOST_BRIDGE, REMAPBASE + 4, remapbase >> 12);

		/* REMAP LIMIT */
		pci_write_config32(HOST_BRIDGE, REMAPLIMIT,     remaplimit << 20);
		pci_write_config32(HOST_BRIDGE, REMAPLIMIT + 4, remaplimit >> 12);
	}
	/* TSEG */
	reg = pci_read_config32(HOST_BRIDGE, TSEGMB);
	val = tsegbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", TSEGMB, reg);
	pci_write_config32(HOST_BRIDGE, TSEGMB, reg);

	/* GFX stolen memory */
	reg = pci_read_config32(HOST_BRIDGE, BDSM);
	val = gfxstolenbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", BDSM, reg);
	pci_write_config32(HOST_BRIDGE, BDSM, reg);

	/* GTT stolen memory */
	reg = pci_read_config32(HOST_BRIDGE, BGSM);
	val = gttbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", BGSM, reg);
	pci_write_config32(HOST_BRIDGE, BGSM, reg);

	if (me_uma_size) {
		reg = pci_read_config32(HOST_BRIDGE, MESEG_MASK + 4);
		val = (0x80000 - me_uma_size) & 0xfffff000;
		reg = (reg & ~0x000fffff) | (val >> 12);
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", MESEG_MASK + 4, reg);
		pci_write_config32(HOST_BRIDGE, MESEG_MASK + 4, reg);

		/* ME base */
		reg = pci_read_config32(HOST_BRIDGE, MESEG_BASE);
		val = mestolenbase & 0xfff;
		reg = (reg & ~0xfff00000) | (val << 20);
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", MESEG_BASE, reg);
		pci_write_config32(HOST_BRIDGE, MESEG_BASE, reg);

		reg = pci_read_config32(HOST_BRIDGE, MESEG_BASE + 4);
		val = mestolenbase & 0xfffff000;
		reg = (reg & ~0x000fffff) | (val >> 12);
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", MESEG_BASE + 4, reg);
		pci_write_config32(HOST_BRIDGE, MESEG_BASE + 4, reg);

		/* ME mask */
		reg = pci_read_config32(HOST_BRIDGE, MESEG_MASK);
		val = (0x80000 - me_uma_size) & 0xfff;
		reg = (reg & ~0xfff00000) | (val << 20);
		reg = reg | ME_STLEN_EN;	/* Set ME memory enable */
		reg = reg | MELCK;		/* Set lock bit on ME mem */
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", MESEG_MASK, reg);
		pci_write_config32(HOST_BRIDGE, MESEG_MASK, reg);
	}
}

static void write_reset(ramctr_timing *ctrl)
{
	int channel, slotrank;

	/* Choose a populated channel */
	channel = (ctrl->rankmap[0]) ? 0 : 1;

	wait_for_iosav(channel);

	/* Choose a populated rank */
	slotrank = (ctrl->rankmap[channel] & 1) ? 0 : 2;

	iosav_write_zqcs_sequence(channel, slotrank, 3, 8, 0);

	/* This is actually using the IOSAV state machine as a timer */
	iosav_run_queue(channel, 1, 1);

	wait_for_iosav(channel);
}

void dram_jedecreset(ramctr_timing *ctrl)
{
	u32 reg;
	int channel;

	while (!(mchbar_read32(RCOMP_TIMER) & (1 << 16)))
		;
	do {
		reg = mchbar_read32(IOSAV_STATUS_ch(0));
	} while ((reg & 0x14) == 0);

	/* Set state of memory controller */
	reg = 0x112;
	mchbar_write32(MC_INIT_STATE_G, reg);
	mchbar_write32(MC_INIT_STATE, 0);
	reg |= 2;		/* DDR reset */
	mchbar_write32(MC_INIT_STATE_G, reg);

	/* Assert DIMM reset signal */
	mchbar_clrbits32(MC_INIT_STATE_G, 1 << 1);

	/* Wait 200us */
	udelay(200);

	/* Deassert DIMM reset signal */
	mchbar_setbits32(MC_INIT_STATE_G, 1 << 1);

	/* Wait 500us */
	udelay(500);

	/* Enable DCLK */
	mchbar_setbits32(MC_INIT_STATE_G, 1 << 2);

	/* XXX Wait 20ns */
	udelay(1);

	FOR_ALL_CHANNELS {
		/* Set valid rank CKE */
		reg = ctrl->rankmap[channel];
		mchbar_write32(MC_INIT_STATE_ch(channel), reg);

		/* Wait 10ns for ranks to settle */
		// udelay(0.01);

		reg = (reg & ~0xf0) | (ctrl->rankmap[channel] << 4);
		mchbar_write32(MC_INIT_STATE_ch(channel), reg);

		/* Write reset using a NOP */
		write_reset(ctrl);
	}
}

/*
 * DDR3 Rank1 Address mirror swap the following pins:
 * A3<->A4, A5<->A6, A7<->A8, BA0<->BA1
 */
static void ddr3_mirror_mrreg(int *bank, u32 *addr)
{
	*bank = ((*bank >> 1) & 1) | ((*bank << 1) & 2);
	*addr = (*addr & ~0x1f8) | ((*addr >> 1) & 0xa8) | ((*addr & 0xa8) << 1);
}

static void write_mrreg(ramctr_timing *ctrl, int channel, int slotrank, int reg, u32 val)
{
	wait_for_iosav(channel);

	if (ctrl->rank_mirror[channel][slotrank])
		ddr3_mirror_mrreg(&reg, &val);

	const struct iosav_ssq sequence[] = {
		/* DRAM command MRS */
		[0] = {
			.sp_cmd_ctrl = {
				.command = IOSAV_MRS,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 4,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = val,
				.rowbits = 6,
				.bank    = reg,
				.rank    = slotrank,
			},
		},
		/* DRAM command MRS */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_MRS,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 4,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = val,
				.rowbits = 6,
				.bank    = reg,
				.rank    = slotrank,
			},
		},
		/* DRAM command MRS */
		[2] = {
			.sp_cmd_ctrl = {
				.command = IOSAV_MRS,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = ctrl->tMOD,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = val,
				.rowbits = 6,
				.bank    = reg,
				.rank    = slotrank,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));

	iosav_run_once_and_wait(channel);
}

/* Obtain optimal power down mode for current configuration */
static enum power_down_mode get_power_down_mode(ramctr_timing *ctrl)
{
	if (ctrl->tXP > 8)
		return PDM_NONE;

	if (ctrl->tXPDLL > 32)
		return PDM_PPD;

	if (CONFIG(RAMINIT_ALWAYS_ALLOW_DLL_OFF) || get_platform_type() == PLATFORM_MOBILE)
		return PDM_DLL_OFF;

	return PDM_APD_PPD;
}

static u32 make_mr0(ramctr_timing *ctrl, u8 rank)
{
	u16 mr0reg, mch_cas, mch_wr;
	static const u8 mch_wr_t[12] = { 1, 2, 3, 4, 0, 5, 0, 6, 0, 7, 0, 0 };

	const enum power_down_mode power_down = get_power_down_mode(ctrl);

	const bool slow_exit = power_down == PDM_DLL_OFF || power_down == PDM_APD_DLL_OFF;

	/* Convert CAS to MCH register friendly */
	if (ctrl->CAS < 12) {
		mch_cas = (u16)((ctrl->CAS - 4) << 1);
	} else {
		mch_cas = (u16)(ctrl->CAS - 12);
		mch_cas = ((mch_cas << 1) | 0x1);
	}

	/* Convert tWR to MCH register friendly */
	mch_wr = mch_wr_t[ctrl->tWR - 5];

	/* DLL Reset - self clearing - set after CLK frequency has been changed */
	mr0reg = 1 << 8;

	mr0reg |= (mch_cas & 0x1) << 2;
	mr0reg |= (mch_cas & 0xe) << 3;
	mr0reg |= mch_wr << 9;

	/* Precharge PD - Use slow exit when DLL-off is used - mostly power-saving feature */
	mr0reg |= !slow_exit << 12;
	return mr0reg;
}

static void dram_mr0(ramctr_timing *ctrl, u8 rank, int channel)
{
	write_mrreg(ctrl, channel, rank, 0, make_mr0(ctrl, rank));
}

static odtmap get_ODT(ramctr_timing *ctrl, int channel)
{
	/* Get ODT based on rankmap */
	int dimms_per_ch = (ctrl->rankmap[channel] & 1) + ((ctrl->rankmap[channel] >> 2) & 1);

	if (dimms_per_ch == 1) {
		return (const odtmap){60,  60};
	} else {
		return (const odtmap){120, 30};
	}
}

static u32 encode_odt(u32 odt)
{
	switch (odt) {
	case 30:
		return (1 << 9) | (1 << 2);	/* RZQ/8, RZQ/4 */
	case 60:
		return (1 << 2);	/* RZQ/4 */
	case 120:
		return (1 << 6);	/* RZQ/2 */
	default:
	case 0:
		return 0;
	}
}

static u32 make_mr1(ramctr_timing *ctrl, u8 rank, int channel)
{
	odtmap odt;
	u32 mr1reg;

	odt = get_ODT(ctrl, channel);
	mr1reg = 2;

	mr1reg |= encode_odt(odt.rttnom);

	return mr1reg;
}

static void dram_mr1(ramctr_timing *ctrl, u8 rank, int channel)
{
	u16 mr1reg;

	mr1reg = make_mr1(ctrl, rank, channel);

	write_mrreg(ctrl, channel, rank, 1, mr1reg);
}

static void dram_mr2(ramctr_timing *ctrl, u8 rank, int channel)
{
	const u16 pasr = 0;
	const u16 cwl = ctrl->CWL - 5;
	const odtmap odt = get_ODT(ctrl, channel);

	int srt = 0;
	if (IS_IVY_CPU(ctrl->cpu) && ctrl->tCK >= TCK_1066MHZ)
		srt = ctrl->extended_temperature_range && !ctrl->auto_self_refresh;

	u16 mr2reg = 0;
	mr2reg |= pasr;
	mr2reg |= cwl << 3;
	mr2reg |= ctrl->auto_self_refresh << 6;
	mr2reg |= srt << 7;
	mr2reg |= (odt.rttwr / 60) << 9;

	write_mrreg(ctrl, channel, rank, 2, mr2reg);

	/* Program MR2 shadow */
	u32 reg32 = mchbar_read32(TC_MR2_SHADOW_ch(channel));

	reg32 &= 3 << 14 | 3 << 6;

	reg32 |= mr2reg & ~(3 << 6);

	if (srt)
		reg32 |= 1 << (rank / 2 + 6);

	if (ctrl->rank_mirror[channel][rank])
		reg32 |= 1 << (rank / 2 + 14);

	mchbar_write32(TC_MR2_SHADOW_ch(channel), reg32);
}

static void dram_mr3(ramctr_timing *ctrl, u8 rank, int channel)
{
	write_mrreg(ctrl, channel, rank, 3, 0);
}

void dram_mrscommands(ramctr_timing *ctrl)
{
	u8 slotrank;
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		FOR_ALL_POPULATED_RANKS {
			/* MR2 */
			dram_mr2(ctrl, slotrank, channel);

			/* MR3 */
			dram_mr3(ctrl, slotrank, channel);

			/* MR1 */
			dram_mr1(ctrl, slotrank, channel);

			/* MR0 */
			dram_mr0(ctrl, slotrank, channel);
		}
	}

	const struct iosav_ssq zqcl_sequence[] = {
		/* DRAM command NOP (without ODT nor chip selects) */
		[0] = {
			.sp_cmd_ctrl = {
				.command = IOSAV_NOP & ~(0xff << 8),
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 15,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 2,
				.rowbits = 6,
				.bank    = 0,
				.rank    = 0,
			},
		},
		/* DRAM command ZQCL */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_ZQCS,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = 400,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 1 << 10,
				.rowbits = 6,
				.bank    = 0,
				.rank    = 0,
			},
			.addr_update = {
				.inc_rank  = 1,
				.addr_wrap = 20,
			},
		},
	};
	iosav_write_sequence(BROADCAST_CH, zqcl_sequence, ARRAY_SIZE(zqcl_sequence));

	iosav_run_queue(BROADCAST_CH, 4, 0);

	FOR_ALL_CHANNELS {
		wait_for_iosav(channel);
	}

	/* Refresh enable */
	mchbar_setbits32(MC_INIT_STATE_G, 1 << 3);

	FOR_ALL_POPULATED_CHANNELS {
		mchbar_clrbits32(SCHED_CBIT_ch(channel), 1 << 21);

		wait_for_iosav(channel);

		slotrank = (ctrl->rankmap[channel] & 1) ? 0 : 2;

		wait_for_iosav(channel);

		iosav_write_zqcs_sequence(channel, slotrank, 4, 101, 31);

		iosav_run_once_and_wait(channel);
	}
}

static const u32 lane_base[] = {
	LANEBASE_B0, LANEBASE_B1, LANEBASE_B2, LANEBASE_B3,
	LANEBASE_B4, LANEBASE_B5, LANEBASE_B6, LANEBASE_B7,
	LANEBASE_ECC
};

/* Maximum delay for command, control, clock */
#define CCC_MAX_PI	(2 * QCLK_PI - 1)

void program_timings(ramctr_timing *ctrl, int channel)
{
	u32 reg_roundtrip_latency, reg_io_latency;
	int lane;
	int slotrank, slot;

	u32 ctl_delay[NUM_SLOTS] = { 0 };
	int cmd_delay = 0;

	/* Enable CLK XOVER */
	u32 clk_pi_coding = get_XOVER_CLK(ctrl->rankmap[channel]);
	u32 clk_logic_dly = 0;

	/*
	 * Compute command timing as abs() of the most negative PI code
	 * across all ranks. Use zero if none of the values is negative.
	 */
	FOR_ALL_POPULATED_RANKS {
		cmd_delay = MAX(cmd_delay, -ctrl->timings[channel][slotrank].pi_coding);
	}
	if (cmd_delay > CCC_MAX_PI) {
		printk(BIOS_ERR, "C%d command delay overflow: %d\n", channel, cmd_delay);
		cmd_delay = CCC_MAX_PI;
	}

	for (slot = 0; slot < NUM_SLOTS; slot++) {
		const int pi_coding_0 = ctrl->timings[channel][2 * slot + 0].pi_coding;
		const int pi_coding_1 = ctrl->timings[channel][2 * slot + 1].pi_coding;

		const u8 slot_map = (ctrl->rankmap[channel] >> (2 * slot)) & 3;

		if (slot_map & 1)
			ctl_delay[slot] += pi_coding_0 + cmd_delay;

		if (slot_map & 2)
			ctl_delay[slot] += pi_coding_1 + cmd_delay;

		/* If both ranks in a slot are populated, use the average */
		if (slot_map == 3)
			ctl_delay[slot] /= 2;

		if (ctl_delay[slot] > CCC_MAX_PI) {
			printk(BIOS_ERR, "C%dS%d control delay overflow: %d\n",
				channel, slot, ctl_delay[slot]);
			ctl_delay[slot] = CCC_MAX_PI;
		}
	}
	FOR_ALL_POPULATED_RANKS {
		int clk_delay = ctrl->timings[channel][slotrank].pi_coding + cmd_delay;

		/*
		 * Clock is a differential signal, whereas command and control are not.
		 * This affects its timing, and it is also why it needs a magic offset.
		 */
		clk_delay += ctrl->pi_code_offset;

		/* Can never happen with valid values */
		if (clk_delay < 0) {
			printk(BIOS_ERR, "C%dR%d clock delay underflow: %d\n",
				channel, slotrank, clk_delay);
			clk_delay = 0;
		}

		/* Clock can safely wrap around because it is a periodic signal */
		clk_delay %= CCC_MAX_PI + 1;

		clk_pi_coding |= (clk_delay % QCLK_PI) << (6 * slotrank);
		clk_logic_dly |= (clk_delay / QCLK_PI) << slotrank;
	}

	/* Enable CMD XOVER */
	union gdcr_cmd_pi_coding_reg cmd_pi_coding = {
		.raw = get_XOVER_CMD(ctrl->rankmap[channel]),
	};
	cmd_pi_coding.cmd_pi_code = cmd_delay % QCLK_PI;
	cmd_pi_coding.cmd_logic_delay = cmd_delay / QCLK_PI;

	cmd_pi_coding.ctl_pi_code_d0 = ctl_delay[0] % QCLK_PI;
	cmd_pi_coding.ctl_pi_code_d1 = ctl_delay[1] % QCLK_PI;
	cmd_pi_coding.ctl_logic_delay_d0 = ctl_delay[0] / QCLK_PI;
	cmd_pi_coding.ctl_logic_delay_d1 = ctl_delay[1] / QCLK_PI;

	mchbar_write32(GDCRCMDPICODING_ch(channel), cmd_pi_coding.raw);

	mchbar_write32(GDCRCKPICODE_ch(channel), clk_pi_coding);
	mchbar_write32(GDCRCKLOGICDELAY_ch(channel), clk_logic_dly);

	reg_io_latency = mchbar_read32(SC_IO_LATENCY_ch(channel));
	reg_io_latency &= ~0xffff;

	reg_roundtrip_latency = 0;

	FOR_ALL_POPULATED_RANKS {
		reg_io_latency |= ctrl->timings[channel][slotrank].io_latency << (4 * slotrank);

		reg_roundtrip_latency |=
		    ctrl->timings[channel][slotrank].roundtrip_latency << (8 * slotrank);

		FOR_ALL_LANES {
			const u16 rcven = ctrl->timings[channel][slotrank].lanes[lane].rcven;
			const u8 dqs_p = ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_p;
			const u8 dqs_n = ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_n;
			const union gdcr_rx_reg gdcr_rx = {
				.rcven_pi_code     = rcven % QCLK_PI,
				.rx_dqs_p_pi_code  = dqs_p,
				.rcven_logic_delay = rcven / QCLK_PI,
				.rx_dqs_n_pi_code  = dqs_n,
			};
			mchbar_write32(lane_base[lane] + GDCRRX(channel, slotrank),
				gdcr_rx.raw);

			const u16 tx_dqs = ctrl->timings[channel][slotrank].lanes[lane].tx_dqs;
			const int tx_dq = ctrl->timings[channel][slotrank].lanes[lane].tx_dq;
			const union gdcr_tx_reg gdcr_tx = {
				.tx_dq_pi_code      = tx_dq % QCLK_PI,
				.tx_dqs_pi_code     = tx_dqs % QCLK_PI,
				.tx_dqs_logic_delay = tx_dqs / QCLK_PI,
				.tx_dq_logic_delay  = tx_dq / QCLK_PI,
			};
			mchbar_write32(lane_base[lane] + GDCRTX(channel, slotrank),
				gdcr_tx.raw);
		}
	}
	mchbar_write32(SC_ROUNDT_LAT_ch(channel), reg_roundtrip_latency);
	mchbar_write32(SC_IO_LATENCY_ch(channel), reg_io_latency);
}

static void test_rcven(ramctr_timing *ctrl, int channel, int slotrank)
{
	wait_for_iosav(channel);

	/* Send a burst of 16 back-to-back read commands (4 DCLK apart) */
	iosav_write_read_mpr_sequence(channel, slotrank, ctrl->tMOD, 1, 3, 15, ctrl->CAS + 36);

	iosav_run_once_and_wait(channel);
}

static int does_lane_work(ramctr_timing *ctrl, int channel, int slotrank, int lane)
{
	u32 rcven = ctrl->timings[channel][slotrank].lanes[lane].rcven;

	return (mchbar_read32(lane_base[lane] +
		GDCRTRAININGRESULT(channel, (rcven / 32) & 1)) >> (rcven % 32)) & 1;
}

struct run {
	int middle;
	int end;
	int start;
	int all;
	int length;
};

static struct run get_longest_zero_run(int *seq, int sz)
{
	int i, ls;
	int bl = 0, bs = 0;
	struct run ret;

	ls = 0;
	for (i = 0; i < 2 * sz; i++)
		if (seq[i % sz]) {
			if (i - ls > bl) {
				bl = i - ls;
				bs = ls;
			}
			ls = i + 1;
		}
	if (bl == 0) {
		ret.middle = sz / 2;
		ret.start  = 0;
		ret.end    = sz;
		ret.length = sz;
		ret.all    = 1;
		return ret;
	}

	ret.start  = bs % sz;
	ret.end    = (bs + bl - 1) % sz;
	ret.middle = (bs + (bl - 1) / 2) % sz;
	ret.length = bl;
	ret.all    = 0;

	return ret;
}

#define RCVEN_COARSE_PI_LENGTH	(2 * QCLK_PI)

static void find_rcven_pi_coarse(ramctr_timing *ctrl, int channel, int slotrank, int *upperA)
{
	int rcven;
	int statistics[NUM_LANES][RCVEN_COARSE_PI_LENGTH];
	int lane;

	for (rcven = 0; rcven < RCVEN_COARSE_PI_LENGTH; rcven++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].rcven = rcven;
		}
		program_timings(ctrl, channel);

		test_rcven(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			statistics[lane][rcven] =
				!does_lane_work(ctrl, channel, slotrank, lane);
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(statistics[lane], RCVEN_COARSE_PI_LENGTH);
		ctrl->timings[channel][slotrank].lanes[lane].rcven = rn.middle;
		upperA[lane] = rn.end;
		if (upperA[lane] < rn.middle)
			upperA[lane] += 2 * QCLK_PI;

		printram("rcven: %d, %d, %d: % 4d-% 4d-% 4d\n",
			 channel, slotrank, lane, rn.start, rn.middle, rn.end);
	}
}

static void fine_tune_rcven_pi(ramctr_timing *ctrl, int channel, int slotrank, int *upperA)
{
	int rcven_delta;
	int statistics[NUM_LANES][51] = {0};
	int lane, i;

	for (rcven_delta = -25; rcven_delta <= 25; rcven_delta++) {

		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].rcven
				= upperA[lane] + rcven_delta + QCLK_PI;
		}
		program_timings(ctrl, channel);

		for (i = 0; i < 100; i++) {
			test_rcven(ctrl, channel, slotrank);
			FOR_ALL_LANES {
				statistics[lane][rcven_delta + 25] +=
					does_lane_work(ctrl, channel, slotrank, lane);
			}
		}
	}
	FOR_ALL_LANES {
		int last_zero, first_all;

		for (last_zero = -25; last_zero <= 25; last_zero++)
			if (statistics[lane][last_zero + 25])
				break;

		last_zero--;
		for (first_all = -25; first_all <= 25; first_all++)
			if (statistics[lane][first_all + 25] == 100)
				break;

		printram("lane %d: %d, %d\n", lane, last_zero, first_all);

		ctrl->timings[channel][slotrank].lanes[lane].rcven =
			(last_zero + first_all) / 2 + upperA[lane];

		printram("Aval: %d, %d, %d: % 4d\n", channel, slotrank,
			lane, ctrl->timings[channel][slotrank].lanes[lane].rcven);
	}
}

/*
 * Once the DQS high phase has been found (for each DRAM) the next stage
 * is to find out the round trip latency, by locating the preamble cycle.
 * This is achieved by trying smaller and smaller roundtrip values until
 * the strobe sampling is done on the preamble cycle.
 */
static int find_roundtrip_latency(ramctr_timing *ctrl, int channel, int slotrank, int *upperA)
{
	int works[NUM_LANES];
	int lane;

	while (1) {
		int all_works = 1, some_works = 0;

		program_timings(ctrl, channel);
		test_rcven(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			works[lane] = !does_lane_work(ctrl, channel, slotrank, lane);

			if (works[lane])
				some_works = 1;
			else
				all_works = 0;
		}

		/* If every lane is working, exit */
		if (all_works)
			return 0;

		/*
		 * If all bits are one (everyone is failing), decrement
		 * the roundtrip value by two, and do another iteration.
		 */
		if (!some_works) {
			/* Guard against roundtrip latency underflow */
			if (ctrl->timings[channel][slotrank].roundtrip_latency < 2) {
				printk(BIOS_EMERG, "Roundtrip latency underflow: %d, %d\n",
				       channel, slotrank);
				return MAKE_ERR;
			}
			ctrl->timings[channel][slotrank].roundtrip_latency -= 2;
			printram("4024 -= 2;\n");
			continue;
		}

		/*
		 * Else (if some lanes are failing), increase the rank's
		 * I/O latency by 2, and increase rcven logic delay by 2
		 * on the working lanes, then perform another iteration.
		 */
		ctrl->timings[channel][slotrank].io_latency += 2;
		printram("4028 += 2;\n");

		/* Guard against I/O latency overflow */
		if (ctrl->timings[channel][slotrank].io_latency >= 16) {
			printk(BIOS_EMERG, "I/O latency overflow: %d, %d\n",
			       channel, slotrank);
			return MAKE_ERR;
		}
		FOR_ALL_LANES if (works[lane]) {
			ctrl->timings[channel][slotrank].lanes[lane].rcven += 2 * QCLK_PI;
			upperA[lane] += 2 * QCLK_PI;
			printram("increment %d, %d, %d\n", channel, slotrank, lane);
		}
	}
	return 0;
}

static int get_logic_delay_delta(ramctr_timing *ctrl, int channel, int slotrank)
{
	int lane;
	u16 logic_delay_min = 7;
	u16 logic_delay_max = 0;

	FOR_ALL_LANES {
		const u16 logic_delay = ctrl->timings[channel][slotrank].lanes[lane].rcven >> 6;

		logic_delay_min = MIN(logic_delay_min, logic_delay);
		logic_delay_max = MAX(logic_delay_max, logic_delay);
	}

	if (logic_delay_max < logic_delay_min) {
		printk(BIOS_EMERG, "Logic delay max < min (%u < %u): %d, %d\n",
		       logic_delay_max, logic_delay_min, channel, slotrank);
	}

	assert(logic_delay_max >= logic_delay_min);

	return logic_delay_max - logic_delay_min;
}

static int align_rt_io_latency(ramctr_timing *ctrl, int channel, int slotrank, int prev)
{
	int latency_offset = 0;

	/* Get changed maxima */
	const int post = get_logic_delay_delta(ctrl, channel, slotrank);

	if (prev < post)
		latency_offset = +1;

	else if (prev > post)
		latency_offset = -1;

	else
		latency_offset = 0;

	ctrl->timings[channel][slotrank].io_latency += latency_offset;
	ctrl->timings[channel][slotrank].roundtrip_latency += latency_offset;
	printram("4024 += %d;\n", latency_offset);
	printram("4028 += %d;\n", latency_offset);

	return post;
}

static void compute_final_logic_delay(ramctr_timing *ctrl, int channel, int slotrank)
{
	u16 logic_delay_min = 7;
	int lane;

	FOR_ALL_LANES {
		const u16 logic_delay = ctrl->timings[channel][slotrank].lanes[lane].rcven >> 6;

		logic_delay_min = MIN(logic_delay_min, logic_delay);
	}

	if (logic_delay_min >= 2) {
		printk(BIOS_WARNING, "Logic delay %u greater than 1: %d %d\n",
			logic_delay_min, channel, slotrank);
	}

	FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].rcven -= logic_delay_min << 6;
	}
	ctrl->timings[channel][slotrank].io_latency -= logic_delay_min;
	printram("4028 -= %d;\n", logic_delay_min);
}

int receive_enable_calibration(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;
	int err;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		int all_high, some_high;
		int upperA[NUM_LANES];
		int prev;

		wait_for_iosav(channel);

		iosav_write_prea_sequence(channel, slotrank, ctrl->tRP, 0);

		iosav_run_once_and_wait(channel);

		const union gdcr_training_mod_reg training_mod = {
			.receive_enable_mode = 1,
			.training_rank_sel   = slotrank,
			.odt_always_on       = 1,
		};
		mchbar_write32(GDCRTRAININGMOD, training_mod.raw);

		ctrl->timings[channel][slotrank].io_latency = 4;
		ctrl->timings[channel][slotrank].roundtrip_latency = 55;
		program_timings(ctrl, channel);

		find_rcven_pi_coarse(ctrl, channel, slotrank, upperA);

		all_high = 1;
		some_high = 0;
		FOR_ALL_LANES {
			if (ctrl->timings[channel][slotrank].lanes[lane].rcven >= QCLK_PI)
				some_high = 1;
			else
				all_high = 0;
		}

		if (all_high) {
			ctrl->timings[channel][slotrank].io_latency--;
			printram("4028--;\n");
			FOR_ALL_LANES {
				ctrl->timings[channel][slotrank].lanes[lane].rcven -= QCLK_PI;
				upperA[lane] -= QCLK_PI;

			}
		} else if (some_high) {
			ctrl->timings[channel][slotrank].roundtrip_latency++;
			ctrl->timings[channel][slotrank].io_latency++;
			printram("4024++;\n");
			printram("4028++;\n");
		}

		program_timings(ctrl, channel);

		prev = get_logic_delay_delta(ctrl, channel, slotrank);

		err = find_roundtrip_latency(ctrl, channel, slotrank, upperA);
		if (err)
			return err;

		prev = align_rt_io_latency(ctrl, channel, slotrank, prev);

		fine_tune_rcven_pi(ctrl, channel, slotrank, upperA);

		prev = align_rt_io_latency(ctrl, channel, slotrank, prev);

		compute_final_logic_delay(ctrl, channel, slotrank);

		align_rt_io_latency(ctrl, channel, slotrank, prev);

		printram("4/8: %d, %d, % 4d, % 4d\n", channel, slotrank,
		       ctrl->timings[channel][slotrank].roundtrip_latency,
		       ctrl->timings[channel][slotrank].io_latency);

		printram("final results:\n");
		FOR_ALL_LANES
			printram("Aval: %d, %d, %d: % 4d\n", channel, slotrank, lane,
			    ctrl->timings[channel][slotrank].lanes[lane].rcven);

		mchbar_write32(GDCRTRAININGMOD, 0);

		toggle_io_reset();
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	return 0;
}

static void test_tx_dq(ramctr_timing *ctrl, int channel, int slotrank)
{
	int lane;

	FOR_ALL_LANES {
		mchbar_write32(IOSAV_By_ERROR_COUNT_ch(channel, lane), 0);
		mchbar_read32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
	}

	wait_for_iosav(channel);

	iosav_write_misc_write_sequence(ctrl, channel, slotrank,
		MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1), 4, 4, 500, 18);

	iosav_run_once_and_wait(channel);

	iosav_write_prea_act_read_sequence(ctrl, channel, slotrank);

	iosav_run_once_and_wait(channel);
}

static void tx_dq_threshold_process(int *data, const int count)
{
	int min = data[0];
	int max = min;
	int i;
	for (i = 1; i < count; i++) {
		if (min > data[i])
			min = data[i];

		if (max < data[i])
			max = data[i];
	}
	int threshold = min / 2 + max / 2;
	for (i = 0; i < count; i++)
		data[i] = data[i] > threshold;

	printram("threshold=%d min=%d max=%d\n", threshold, min, max);
}

static int tx_dq_write_leveling(ramctr_timing *ctrl, int channel, int slotrank)
{
	int tx_dq;
	int stats[NUM_LANES][MAX_TX_DQ + 1];
	int lane;

	wait_for_iosav(channel);

	iosav_write_prea_sequence(channel, slotrank, ctrl->tRP, 18);

	iosav_run_once_and_wait(channel);

	for (tx_dq = 0; tx_dq <= MAX_TX_DQ; tx_dq++) {
		FOR_ALL_LANES ctrl->timings[channel][slotrank].lanes[lane].tx_dq = tx_dq;
		program_timings(ctrl, channel);

		test_tx_dq(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			stats[lane][tx_dq] = mchbar_read32(
				IOSAV_By_ERROR_COUNT_ch(channel, lane));
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(stats[lane], ARRAY_SIZE(stats[lane]));

		if (rn.all || rn.length < 8) {
			printk(BIOS_EMERG, "tx_dq write leveling failed: %d, %d, %d\n",
			       channel, slotrank, lane);
			/*
			 * With command training not being done yet, the lane can be erroneous.
			 * Take the average as reference and try again to find a run.
			 */
			tx_dq_threshold_process(stats[lane], ARRAY_SIZE(stats[lane]));
			rn = get_longest_zero_run(stats[lane], ARRAY_SIZE(stats[lane]));

			if (rn.all || rn.length < 8) {
				printk(BIOS_EMERG, "tx_dq recovery failed\n");
				return MAKE_ERR;
			}
		}
		ctrl->timings[channel][slotrank].lanes[lane].tx_dq = rn.middle;
		printram("tx_dq: %d, %d, %d: % 4d-% 4d-% 4d\n",
			channel, slotrank, lane, rn.start, rn.middle, rn.end);
	}
	return 0;
}

static int get_precedening_channels(ramctr_timing *ctrl, int target_channel)
{
	int channel, ret = 0;

	FOR_ALL_POPULATED_CHANNELS if (channel < target_channel)
		 ret++;

	return ret;
}

/* Each cacheline is 64 bits long */
static void program_wdb_pattern_length(int channel, const unsigned int num_cachelines)
{
	mchbar_write8(IOSAV_DATA_CTL_ch(channel), num_cachelines / 8 - 1);
}

static void fill_pattern0(ramctr_timing *ctrl, int channel, u32 a, u32 b)
{
	unsigned int j;
	unsigned int channel_offset = get_precedening_channels(ctrl, channel) * 64;
	uintptr_t addr;

	for (j = 0; j < 16; j++) {
		addr = 0x04000000 + channel_offset + 4 * j;
		write32((void *)addr, j & 2 ? b : a);
	}

	sfence();

	program_wdb_pattern_length(channel, 8);
}

static int num_of_channels(const ramctr_timing *ctrl)
{
	int ret = 0;
	int channel;
	FOR_ALL_POPULATED_CHANNELS ret++;
	return ret;
}

static void fill_pattern1(ramctr_timing *ctrl, int channel)
{
	unsigned int j;
	unsigned int channel_offset = get_precedening_channels(ctrl, channel) * 64;
	unsigned int channel_step = 64 * num_of_channels(ctrl);
	uintptr_t addr;

	for (j = 0; j < 16; j++) {
		addr = 0x04000000 + channel_offset + j * 4;
		write32((void *)addr, 0xffffffff);
	}
	for (j = 0; j < 16; j++) {
		addr = 0x04000000 + channel_offset + channel_step + j * 4;
		write32((void *)addr, 0);
	}
	sfence();

	program_wdb_pattern_length(channel, 16);
}

#define TX_DQS_PI_LENGTH	(2 * QCLK_PI)

static int write_level_rank(ramctr_timing *ctrl, int channel, int slotrank)
{
	int tx_dqs;
	int statistics[NUM_LANES][TX_DQS_PI_LENGTH];
	int lane;

	const union gdcr_training_mod_reg training_mod = {
		.write_leveling_mode = 1,
		.training_rank_sel   = slotrank,
		.enable_dqs_wl       = 5,
		.odt_always_on       = 1,
		.force_drive_enable  = 1,
	};
	mchbar_write32(GDCRTRAININGMOD, training_mod.raw);

	u32 mr1reg = make_mr1(ctrl, slotrank, channel) | 1 << 7;
	int bank = 1;

	if (ctrl->rank_mirror[channel][slotrank])
		ddr3_mirror_mrreg(&bank, &mr1reg);

	wait_for_iosav(channel);

	iosav_write_jedec_write_leveling_sequence(ctrl, channel, slotrank, bank, mr1reg);

	for (tx_dqs = 0; tx_dqs < TX_DQS_PI_LENGTH; tx_dqs++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].tx_dqs = tx_dqs;
		}
		program_timings(ctrl, channel);

		iosav_run_once_and_wait(channel);

		FOR_ALL_LANES {
			statistics[lane][tx_dqs] =  !((mchbar_read32(lane_base[lane] +
				GDCRTRAININGRESULT(channel, (tx_dqs / 32) & 1)) >>
				(tx_dqs % 32)) & 1);
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(statistics[lane], TX_DQS_PI_LENGTH);
		/*
		 * tx_dq is a direct function of tx_dqs's 6 LSBs. Some tests increment the value
		 * of tx_dqs by a small value, which might cause the 6-bit value to overflow if
		 * it's close to 0x3f. Increment the value by a small offset if it's likely
		 * to overflow, to make sure it won't overflow while running tests and bricks
		 * the system due to a non matching tx_dq.
		 *
		 * TODO: find out why some tests (edge write discovery) increment tx_dqs.
		 */
		if ((rn.start & 0x3f) == 0x3e)
			rn.start += 2;
		else if ((rn.start & 0x3f) == 0x3f)
			rn.start += 1;

		ctrl->timings[channel][slotrank].lanes[lane].tx_dqs = rn.start;
		if (rn.all) {
			printk(BIOS_EMERG, "JEDEC write leveling failed: %d, %d, %d\n",
			       channel, slotrank, lane);

			return MAKE_ERR;
		}
		printram("tx_dqs: %d, %d, %d: % 4d-% 4d-% 4d\n",
				 channel, slotrank, lane, rn.start, rn.middle, rn.end);
	}
	return 0;
}

static int get_dqs_flyby_adjust(u64 val)
{
	int i;
	/* DQS is good enough */
	if (val == 0xffffffffffffffffLL)
		return 0;
	if (val >= 0xf000000000000000LL) {
		/* DQS is late, needs negative adjustment */
		for (i = 0; i < 8; i++)
			if (val << (8 * (7 - i) + 4))
				return -i;
	} else {
		/* DQS is early, needs positive adjustment */
		for (i = 0; i < 8; i++)
			if (val >> (8 * (7 - i) + 4))
				return i;
	}
	return 8;
}

static void train_write_flyby(ramctr_timing *ctrl)
{
	int channel, slotrank, lane, old;

	const union gdcr_training_mod_reg training_mod = {
		.dq_dqs_training_res = 1,
	};
	mchbar_write32(GDCRTRAININGMOD, training_mod.raw);

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern1(ctrl, channel);
	}
	FOR_ALL_POPULATED_CHANNELS FOR_ALL_POPULATED_RANKS {

		/* Reset read and write WDB pointers */
		mchbar_write32(IOSAV_DATA_CTL_ch(channel), 0x10001);

		wait_for_iosav(channel);

		iosav_write_misc_write_sequence(ctrl, channel, slotrank, 3, 1, 3, 3, 31);

		iosav_run_once_and_wait(channel);

		const struct iosav_ssq rd_sequence[] = {
			/* DRAM command PREA */
			[0] = {
				.sp_cmd_ctrl = {
					.command    = IOSAV_PRE,
					.ranksel_ap = 1,
				},
				.subseq_ctrl = {
					.cmd_executions = 1,
					.cmd_delay_gap  = 3,
					.post_ssq_wait  = ctrl->tRP,
					.data_direction = SSQ_NA,
				},
				.sp_cmd_addr = {
					.address = 1 << 10,
					.rowbits = 6,
					.bank    = 0,
					.rank    = slotrank,
				},
				.addr_update = {
					.addr_wrap = 18,
				},
			},
			/* DRAM command ACT */
			[1] = {
				.sp_cmd_ctrl = {
					.command    = IOSAV_ACT,
					.ranksel_ap = 1,
				},
				.subseq_ctrl = {
					.cmd_executions = 1,
					.cmd_delay_gap  = 3,
					.post_ssq_wait  = ctrl->tRCD,
					.data_direction = SSQ_NA,
				},
				.sp_cmd_addr = {
					.address = 0,
					.rowbits = 6,
					.bank    = 0,
					.rank    = slotrank,
				},
			},
			/* DRAM command RDA */
			[2] = {
				.sp_cmd_ctrl = {
					.command    = IOSAV_RD,
					.ranksel_ap = 3,
				},
				.subseq_ctrl = {
					.cmd_executions = 1,
					.cmd_delay_gap  = 3,
					.post_ssq_wait  = ctrl->tRP +
				ctrl->timings[channel][slotrank].roundtrip_latency +
				ctrl->timings[channel][slotrank].io_latency,
					.data_direction = SSQ_RD,
				},
				.sp_cmd_addr = {
					.address = 8,
					.rowbits = 6,
					.bank    = 0,
					.rank    = slotrank,
				},
			},
		};
		iosav_write_sequence(channel, rd_sequence, ARRAY_SIZE(rd_sequence));

		iosav_run_once_and_wait(channel);

		FOR_ALL_LANES {
			u64 res = mchbar_read32(lane_base[lane] + GDCRTRAININGRESULT1(channel));
			res |= ((u64)mchbar_read32(lane_base[lane] +
				GDCRTRAININGRESULT2(channel))) << 32;

			old = ctrl->timings[channel][slotrank].lanes[lane].tx_dqs;
			ctrl->timings[channel][slotrank].lanes[lane].tx_dqs +=
				get_dqs_flyby_adjust(res) * QCLK_PI;

			printram("High adjust %d:%016llx\n", lane, res);
			printram("Bval+: %d, %d, %d, % 4d -> % 4d\n", channel, slotrank, lane,
				old, ctrl->timings[channel][slotrank].lanes[lane].tx_dqs);
		}
	}
	mchbar_write32(GDCRTRAININGMOD, 0);
}

static void disable_refresh_machine(ramctr_timing *ctrl)
{
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		/* choose an existing rank */
		const int slotrank = !(ctrl->rankmap[channel] & 1) ? 2 : 0;

		iosav_write_zqcs_sequence(channel, slotrank, 4, 4, 31);

		iosav_run_once_and_wait(channel);

		mchbar_setbits32(SCHED_CBIT_ch(channel), 1 << 21);
	}

	/* Refresh disable */
	mchbar_clrbits32(MC_INIT_STATE_G, 1 << 3);

	FOR_ALL_POPULATED_CHANNELS {
		/* Execute the same command queue */
		iosav_run_once_and_wait(channel);
	}
}

/*
 * Compensate the skew between CMD/ADDR/CLK and DQ/DQS lanes.
 *
 * Since DDR3 uses a fly-by topology, the data and strobes signals reach the chips at different
 * times with respect to command, address and clock signals. By delaying either all DQ/DQS or
 * all CMD/ADDR/CLK signals, a full phase shift can be introduced. It is assumed that the
 * CLK/ADDR/CMD signals have the same routing delay.
 *
 * To find the required phase shift the DRAM is placed in "write leveling" mode. In this mode,
 * the DRAM-chip samples the CLK on every DQS edge and feeds back the sampled value on the data
 * lanes (DQ).
 */
static int jedec_write_leveling(ramctr_timing *ctrl)
{
	int channel, slotrank;

	disable_refresh_machine(ctrl);

	/* Enable write leveling on all ranks
	   Disable all DQ outputs
	   Only NOP is allowed in this mode */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS
		write_mrreg(ctrl, channel, slotrank, 1,
				make_mr1(ctrl, slotrank, channel) | 1 << 12 | 1 << 7);

	/* Needs to be programmed before I/O reset below */
	const union gdcr_training_mod_reg training_mod = {
		.write_leveling_mode = 1,
		.enable_dqs_wl       = 5,
		.odt_always_on       = 1,
		.force_drive_enable  = 1,
	};
	mchbar_write32(GDCRTRAININGMOD, training_mod.raw);

	toggle_io_reset();

	/* Set any valid value for tx_dqs, it gets corrected later */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		const int err = write_level_rank(ctrl, channel, slotrank);
		if (err)
			return err;
	}

	/* Disable write leveling on all ranks */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS
		write_mrreg(ctrl, channel, slotrank, 1, make_mr1(ctrl, slotrank, channel));

	mchbar_write32(GDCRTRAININGMOD, 0);

	FOR_ALL_POPULATED_CHANNELS
		wait_for_iosav(channel);

	/* Refresh enable */
	mchbar_setbits32(MC_INIT_STATE_G, 1 << 3);

	FOR_ALL_POPULATED_CHANNELS {
		mchbar_clrbits32(SCHED_CBIT_ch(channel), 1 << 21);
		mchbar_read32(IOSAV_STATUS_ch(channel));
		wait_for_iosav(channel);

		iosav_write_zqcs_sequence(channel, 0, 4, 101, 31);

		iosav_run_once_and_wait(channel);
	}

	toggle_io_reset();

	return 0;
}

int write_training(ramctr_timing *ctrl)
{
	int channel, slotrank;
	int err;

	/*
	 * Set the DEC_WRD bit, required for the write flyby algorithm.
	 * Needs to be done before starting the write training procedure.
	 */
	FOR_ALL_POPULATED_CHANNELS
		mchbar_setbits32(TC_RWP_ch(channel), 1 << 27);

	printram("CPE\n");

	err = jedec_write_leveling(ctrl);
	if (err)
		return err;

	printram("CPF\n");

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern0(ctrl, channel, 0xaaaaaaaa, 0x55555555);
	}

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = tx_dq_write_leveling(ctrl, channel, slotrank);
		if (err)
			return err;
	}

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	/* measure and adjust tx_dqs timings */
	train_write_flyby(ctrl);

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	return 0;
}

static int test_command_training(ramctr_timing *ctrl, int channel, int slotrank)
{
	struct ram_rank_timings saved_rt = ctrl->timings[channel][slotrank];
	int tx_dq_delta;
	int lanes_ok = 0;
	int ctr = 0;
	int lane;

	for (tx_dq_delta = -5; tx_dq_delta <= 5; tx_dq_delta++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].tx_dq =
			    saved_rt.lanes[lane].tx_dq + tx_dq_delta;
		}
		program_timings(ctrl, channel);
		FOR_ALL_LANES {
			mchbar_write32(IOSAV_By_ERROR_COUNT(lane), 0);
		}

		/* Reset read WDB pointer */
		mchbar_write32(IOSAV_DATA_CTL_ch(channel), 0x1f);

		wait_for_iosav(channel);

		iosav_write_command_training_sequence(ctrl, channel, slotrank, ctr);

		/* Program LFSR for the RD/WR subsequences */
		mchbar_write32(IOSAV_n_ADDRESS_LFSR_ch(channel, 1), 0x389abcd);
		mchbar_write32(IOSAV_n_ADDRESS_LFSR_ch(channel, 2), 0x389abcd);

		iosav_run_once_and_wait(channel);

		FOR_ALL_LANES {
			u32 r32 = mchbar_read32(IOSAV_By_ERROR_COUNT_ch(channel, lane));

			if (r32 == 0)
				lanes_ok |= 1 << lane;
		}
		ctr++;
		if (lanes_ok == ((1 << ctrl->lanes) - 1))
			break;
	}

	ctrl->timings[channel][slotrank] = saved_rt;

	return lanes_ok != ((1 << ctrl->lanes) - 1);
}

static void fill_pattern5(ramctr_timing *ctrl, int channel, int patno)
{
	unsigned int i, j;
	unsigned int offset = get_precedening_channels(ctrl, channel) * 64;
	unsigned int step = 64 * num_of_channels(ctrl);
	uintptr_t addr;

	if (patno) {
		u8 base8 = 0x80 >> ((patno - 1) % 8);
		u32 base = base8 | (base8 << 8) | (base8 << 16) | (base8 << 24);
		for (i = 0; i < 32; i++) {
			for (j = 0; j < 16; j++) {
				u32 val = use_base[patno - 1][i] & (1 << (j / 2)) ? base : 0;

				if (invert[patno - 1][i] & (1 << (j / 2)))
					val = ~val;

				addr = (1 << 26) + offset + i * step + j * 4;
				write32((void *)addr, val);
			}
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(pattern); i++) {
			for (j = 0; j < 16; j++) {
				const u32 val = pattern[i][j];
				addr = (1 << 26) + offset + i * step + j * 4;
				write32((void *)addr, val);
			}
		}
		sfence();
	}

	program_wdb_pattern_length(channel, 256);
}

static void reprogram_320c(ramctr_timing *ctrl)
{
	disable_refresh_machine(ctrl);

	/* JEDEC reset */
	dram_jedecreset(ctrl);

	/* MRS commands */
	dram_mrscommands(ctrl);

	toggle_io_reset();
}

#define CT_MIN_PI	(-CCC_MAX_PI)
#define CT_MAX_PI	(+CCC_MAX_PI + 1)
#define CT_PI_LENGTH	(CT_MAX_PI - CT_MIN_PI + 1)

#define MIN_C320C_LEN 13

static int try_cmd_stretch(ramctr_timing *ctrl, int channel, int cmd_stretch)
{
	struct ram_rank_timings saved_timings[NUM_CHANNELS][NUM_SLOTRANKS];
	int slotrank;
	int command_pi;
	int stat[NUM_SLOTRANKS][CT_PI_LENGTH];
	int delta = 0;

	printram("Trying cmd_stretch %d on channel %d\n", cmd_stretch, channel);

	FOR_ALL_POPULATED_RANKS {
		saved_timings[channel][slotrank] = ctrl->timings[channel][slotrank];
	}

	ctrl->cmd_stretch[channel] = cmd_stretch;

	const union tc_rap_reg tc_rap = {
		.tRRD    = ctrl->tRRD,
		.tRTP    = ctrl->tRTP,
		.tCKE    = ctrl->tCKE,
		.tWTR    = ctrl->tWTR,
		.tFAW    = ctrl->tFAW,
		.tWR     = ctrl->tWR,
		.tCMD    = ctrl->cmd_stretch[channel],
	};
	mchbar_write32(TC_RAP_ch(channel), tc_rap.raw);

	if (ctrl->cmd_stretch[channel] == 2)
		delta = 2;
	else if (ctrl->cmd_stretch[channel] == 0)
		delta = 4;

	FOR_ALL_POPULATED_RANKS {
		ctrl->timings[channel][slotrank].roundtrip_latency -= delta;
	}

	for (command_pi = CT_MIN_PI; command_pi < CT_MAX_PI; command_pi++) {
		FOR_ALL_POPULATED_RANKS {
			ctrl->timings[channel][slotrank].pi_coding = command_pi;
		}
		program_timings(ctrl, channel);
		reprogram_320c(ctrl);
		FOR_ALL_POPULATED_RANKS {
			stat[slotrank][command_pi - CT_MIN_PI] =
				test_command_training(ctrl, channel, slotrank);
		}
	}
	FOR_ALL_POPULATED_RANKS {
		struct run rn = get_longest_zero_run(stat[slotrank], CT_PI_LENGTH - 1);

		ctrl->timings[channel][slotrank].pi_coding = rn.middle + CT_MIN_PI;
		printram("cmd_stretch: %d, %d: % 4d-% 4d-% 4d\n",
				 channel, slotrank, rn.start, rn.middle, rn.end);

		if (rn.all || rn.length < MIN_C320C_LEN) {
			FOR_ALL_POPULATED_RANKS {
				ctrl->timings[channel][slotrank] =
					saved_timings[channel][slotrank];
			}
			return MAKE_ERR;
		}
	}

	return 0;
}

/*
 * Adjust CMD phase shift and try multiple command rates.
 * A command rate of 2T doubles the time needed for address and command decode.
 */
int command_training(ramctr_timing *ctrl)
{
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern5(ctrl, channel, 0);
	}

	FOR_ALL_POPULATED_CHANNELS {
		int cmdrate, err;

		/*
		 * Dual DIMM per channel:
		 * Issue:
		 * While command training seems to succeed, raminit will fail in write training.
		 *
		 * Workaround:
		 * Skip 1T in dual DIMM mode, that's only supported by a few DIMMs.
		 * Only try 1T mode for XMP DIMMs that request it in dual DIMM mode.
		 *
		 * Single DIMM per channel:
		 * Try command rate 1T and 2T
		 */
		cmdrate = ((ctrl->rankmap[channel] & 0x5) == 0x5);
		if (ctrl->tCMD)
			/* XMP gives the CMD rate in clock ticks, not ns */
			cmdrate = MIN(DIV_ROUND_UP(ctrl->tCMD, 256) - 1, 1);

		for (; cmdrate < 2; cmdrate++) {
			err = try_cmd_stretch(ctrl, channel, cmdrate << 1);

			if (!err)
				break;
		}

		if (err) {
			printk(BIOS_EMERG, "Command training failed: %d\n", channel);
			return err;
		}

		printram("Using CMD rate %uT on channel %u\n", cmdrate + 1, channel);
	}

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	reprogram_320c(ctrl);
	return 0;
}

static int find_read_mpr_margin(ramctr_timing *ctrl, int channel, int slotrank, int *edges)
{
	int dqs_pi;
	int stats[NUM_LANES][MAX_EDGE_TIMING + 1];
	int lane;

	for (dqs_pi = 0; dqs_pi <= MAX_EDGE_TIMING; dqs_pi++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_p = dqs_pi;
			ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_n = dqs_pi;
		}
		program_timings(ctrl, channel);

		FOR_ALL_LANES {
			mchbar_write32(IOSAV_By_ERROR_COUNT_ch(channel, lane), 0);
			mchbar_read32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
		}

		wait_for_iosav(channel);

		iosav_write_read_mpr_sequence(
			channel, slotrank, ctrl->tMOD, 500, 4, 1, ctrl->CAS + 8);

		iosav_run_once_and_wait(channel);

		FOR_ALL_LANES {
			stats[lane][dqs_pi] = mchbar_read32(
				IOSAV_By_ERROR_COUNT_ch(channel, lane));
		}
	}

	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(stats[lane], MAX_EDGE_TIMING + 1);
		edges[lane] = rn.middle;

		if (rn.all) {
			printk(BIOS_EMERG, "Read MPR training failed: %d, %d, %d\n", channel,
			       slotrank, lane);
			return MAKE_ERR;
		}
		printram("eval %d, %d, %d: % 4d\n", channel, slotrank, lane, edges[lane]);
	}
	return 0;
}

static void find_predefined_pattern(ramctr_timing *ctrl, const int channel)
{
	int slotrank, lane;

	fill_pattern0(ctrl, channel, 0, 0);
	FOR_ALL_LANES {
		mchbar_write32(IOSAV_By_BW_MASK_ch(channel, lane), 0);
		mchbar_read32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
	}

	FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_n = 16;
		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_p = 16;
	}

	program_timings(ctrl, channel);

	FOR_ALL_POPULATED_RANKS {
		wait_for_iosav(channel);

		iosav_write_read_mpr_sequence(
			channel, slotrank, ctrl->tMOD, 3, 4, 1, ctrl->CAS + 8);

		iosav_run_once_and_wait(channel);
	}

	/* XXX: check any measured value ? */

	FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_n = 48;
		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_p = 48;
	}

	program_timings(ctrl, channel);

	FOR_ALL_POPULATED_RANKS {
		wait_for_iosav(channel);

		iosav_write_read_mpr_sequence(
			channel, slotrank, ctrl->tMOD, 3, 4, 1, ctrl->CAS + 8);

		iosav_run_once_and_wait(channel);
	}

	/* XXX: check any measured value ? */

	FOR_ALL_LANES {
		mchbar_write32(IOSAV_By_BW_MASK_ch(channel, lane),
			~mchbar_read32(IOSAV_By_BW_SERROR_ch(channel, lane)) & 0xff);
	}
}

int read_mpr_training(ramctr_timing *ctrl)
{
	int falling_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int rising_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane;
	int err;

	mchbar_write32(GDCRTRAININGMOD, 0);

	toggle_io_reset();

	FOR_ALL_POPULATED_CHANNELS {
		find_predefined_pattern(ctrl, channel);

		fill_pattern0(ctrl, channel, 0, 0xffffffff);
	}

	/*
	 * FIXME: Under some conditions, vendor BIOS sets both edges to the same value. It will
	 *        also use a single loop. It would seem that it is a debugging configuration.
	 */
	mchbar_write32(IOSAV_DC_MASK, 3 << 8);
	printram("discover falling edges:\n[%x] = %x\n", IOSAV_DC_MASK, 3 << 8);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = find_read_mpr_margin(ctrl, channel, slotrank,
			falling_edges[channel][slotrank]);
		if (err)
			return err;
	}

	mchbar_write32(IOSAV_DC_MASK, 2 << 8);
	printram("discover rising edges:\n[%x] = %x\n", IOSAV_DC_MASK, 2 << 8);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = find_read_mpr_margin(ctrl, channel, slotrank,
				    rising_edges[channel][slotrank]);
		if (err)
			return err;
	}

	mchbar_write32(IOSAV_DC_MASK, 0);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_n =
		    falling_edges[channel][slotrank][lane];
		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_p =
		    rising_edges[channel][slotrank][lane];
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	FOR_ALL_POPULATED_CHANNELS FOR_ALL_LANES {
		mchbar_write32(IOSAV_By_BW_MASK_ch(channel, lane), 0);
	}
	return 0;
}

static int find_agrsv_read_margin(ramctr_timing *ctrl, int channel, int slotrank, int *edges)
{
	const int rd_vref_offsets[] = { 0, 0xc, 0x2c };

	u32 raw_stats[MAX_EDGE_TIMING + 1];
	int lower[NUM_LANES];
	int upper[NUM_LANES];
	int lane, i, read_pi, pat;

	FOR_ALL_LANES {
		lower[lane] = 0;
		upper[lane] = MAX_EDGE_TIMING;
	}

	for (i = 0; i < ARRAY_SIZE(rd_vref_offsets); i++) {
		const union gdcr_training_mod_reg training_mod = {
			.vref_gen_ctl = rd_vref_offsets[i],
		};
		mchbar_write32(GDCRTRAININGMOD_ch(channel), training_mod.raw);
		printram("[%x] = 0x%08x\n", GDCRTRAININGMOD_ch(channel), training_mod.raw);

		for (pat = 0; pat < NUM_PATTERNS; pat++) {
			fill_pattern5(ctrl, channel, pat);
			printram("using pattern %d\n", pat);

			for (read_pi = 0; read_pi <= MAX_EDGE_TIMING; read_pi++) {
				FOR_ALL_LANES {
					ctrl->timings[channel][slotrank].lanes[lane]
						.rx_dqs_p = read_pi;
					ctrl->timings[channel][slotrank].lanes[lane]
						.rx_dqs_n = read_pi;
				}
				program_timings(ctrl, channel);

				FOR_ALL_LANES {
					mchbar_write32(IOSAV_By_ERROR_COUNT_ch(channel, lane),
							0);
					mchbar_read32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
				}
				wait_for_iosav(channel);

				iosav_write_data_write_sequence(ctrl, channel, slotrank);

				iosav_run_once_and_wait(channel);

				FOR_ALL_LANES {
					mchbar_read32(IOSAV_By_ERROR_COUNT_ch(channel, lane));
				}

				/* FIXME: This register only exists on Ivy Bridge */
				raw_stats[read_pi] = mchbar_read32(
					IOSAV_BYTE_SERROR_C_ch(channel));
			}

			FOR_ALL_LANES {
				int stats[MAX_EDGE_TIMING + 1];
				struct run rn;

				for (read_pi = 0; read_pi <= MAX_EDGE_TIMING; read_pi++)
					stats[read_pi] = !!(raw_stats[read_pi] & (1 << lane));

				rn = get_longest_zero_run(stats, MAX_EDGE_TIMING + 1);

				printram("edges: %d, %d, %d: % 4d-% 4d-% 4d, "
					 "% 4d-% 4d\n", channel, slotrank, i, rn.start,
					 rn.middle, rn.end, rn.start + ctrl->edge_offset[i],
					 rn.end - ctrl->edge_offset[i]);

				lower[lane] = MAX(rn.start + ctrl->edge_offset[i], lower[lane]);
				upper[lane] = MIN(rn.end   - ctrl->edge_offset[i], upper[lane]);

				edges[lane] = (lower[lane] + upper[lane]) / 2;
				if (rn.all || (lower[lane] > upper[lane])) {
					printk(BIOS_EMERG, "Aggressive read training failed: "
						"%d, %d, %d\n", channel, slotrank, lane);

					return MAKE_ERR;
				}
			}
		}
	}

	/* Restore nominal Vref after training */
	mchbar_write32(GDCRTRAININGMOD_ch(channel), 0);
	printram("CPA\n");
	return 0;
}

int aggressive_read_training(ramctr_timing *ctrl)
{
	int falling_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int  rising_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane, err;

	/*
	 * FIXME: Under some conditions, vendor BIOS sets both edges to the same value. It will
	 *        also use a single loop. It would seem that it is a debugging configuration.
	 */
	mchbar_write32(IOSAV_DC_MASK, 3 << 8);
	printram("discover falling edges aggressive:\n[%x] = %x\n", IOSAV_DC_MASK, 3 << 8);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = find_agrsv_read_margin(ctrl, channel, slotrank,
					falling_edges[channel][slotrank]);
		if (err)
			return err;
	}

	mchbar_write32(IOSAV_DC_MASK, 2 << 8);
	printram("discover rising edges aggressive:\n[%x] = %x\n", IOSAV_DC_MASK, 2 << 8);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = find_agrsv_read_margin(ctrl, channel, slotrank,
					 rising_edges[channel][slotrank]);
		if (err)
			return err;
	}

	mchbar_write32(IOSAV_DC_MASK, 0);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_n =
				falling_edges[channel][slotrank][lane];

		ctrl->timings[channel][slotrank].lanes[lane].rx_dqs_p =
				rising_edges[channel][slotrank][lane];
	}

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	return 0;
}

static void test_aggressive_write(ramctr_timing *ctrl, int channel, int slotrank)
{
	wait_for_iosav(channel);

	iosav_write_aggressive_write_read_sequence(ctrl, channel, slotrank);

	iosav_run_once_and_wait(channel);
}

static void set_write_vref(const int channel, const u8 wr_vref)
{
	mchbar_clrsetbits32(GDCRCMDDEBUGMUXCFG_Cz_S(channel), 0x3f << 24, wr_vref << 24);
	udelay(2);
}

int aggressive_write_training(ramctr_timing *ctrl)
{
	const u8 wr_vref_offsets[3] = { 0, 0x0f, 0x2f };
	int i, pat;

	int lower[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int upper[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane;

	/* Changing the write Vref is only supported on some Ivy Bridge SKUs */
	if (!IS_IVY_CPU(ctrl->cpu))
		return 0;

	if (!(pci_read_config32(HOST_BRIDGE, CAPID0_A) & CAPID_WRTVREF))
		return 0;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		lower[channel][slotrank][lane] = 0;
		upper[channel][slotrank][lane] = MAX_TX_DQ;
	}

	/* Only enable IOSAV_n_SPECIAL_COMMAND_ADDR optimization on later steppings */
	const bool enable_iosav_opt = IS_IVY_CPU_D(ctrl->cpu) || IS_IVY_CPU_E(ctrl->cpu);

	if (enable_iosav_opt)
		mchbar_write32(MCMNTS_SPARE, 1);

	printram("Aggressive write training:\n");

	for (i = 0; i < ARRAY_SIZE(wr_vref_offsets); i++) {
		FOR_ALL_POPULATED_CHANNELS {
			set_write_vref(channel, wr_vref_offsets[i]);

			for (pat = 0; pat < NUM_PATTERNS; pat++) {
				FOR_ALL_POPULATED_RANKS {
					int tx_dq;
					u32 raw_stats[MAX_TX_DQ + 1];
					int stats[MAX_TX_DQ + 1];

					/* Make sure rn.start < rn.end */
					stats[MAX_TX_DQ] = 1;

					fill_pattern5(ctrl, channel, pat);

					for (tx_dq = 0; tx_dq < MAX_TX_DQ; tx_dq++) {
						FOR_ALL_LANES {
							ctrl->timings[channel][slotrank]
								.lanes[lane].tx_dq = tx_dq;
						}
						program_timings(ctrl, channel);

						test_aggressive_write(ctrl, channel, slotrank);

						raw_stats[tx_dq] = mchbar_read32(
							IOSAV_BYTE_SERROR_C_ch(channel));
					}
					FOR_ALL_LANES {
						struct run rn;
						for (tx_dq = 0; tx_dq < MAX_TX_DQ; tx_dq++) {
							stats[tx_dq] = !!(raw_stats[tx_dq]
									& (1 << lane));
						}

						rn = get_longest_zero_run(stats, MAX_TX_DQ + 1);
						if (rn.all) {
							printk(BIOS_EMERG, "Aggressive "
								"write training failed: "
								"%d, %d, %d\n", channel,
								slotrank, lane);

							return MAKE_ERR;
						}
						printram("tx_dq: %d, %d, %d: "
							 "% 4d-% 4d-% 4d, "
							 "% 4d-% 4d\n", channel, slotrank,
							 i, rn.start, rn.middle, rn.end,
							 rn.start + ctrl->tx_dq_offset[i],
							 rn.end   - ctrl->tx_dq_offset[i]);

						lower[channel][slotrank][lane] =
							MAX(rn.start + ctrl->tx_dq_offset[i],
							    lower[channel][slotrank][lane]);

						upper[channel][slotrank][lane] =
							MIN(rn.end - ctrl->tx_dq_offset[i],
							    upper[channel][slotrank][lane]);

					}
				}
			}
		}
	}

	FOR_ALL_CHANNELS {
		/* Restore nominal write Vref after training */
		set_write_vref(channel, 0);
	}

	/* Disable IOSAV_n_SPECIAL_COMMAND_ADDR optimization */
	if (enable_iosav_opt)
		mchbar_write32(MCMNTS_SPARE, 0);

	printram("CPB\n");

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		printram("tx_dq %d, %d, %d: % 4d\n", channel, slotrank, lane,
		       (lower[channel][slotrank][lane] +
			upper[channel][slotrank][lane]) / 2);

		ctrl->timings[channel][slotrank].lanes[lane].tx_dq =
		    (lower[channel][slotrank][lane] +
		     upper[channel][slotrank][lane]) / 2;
	}
	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}
	return 0;
}

void normalize_training(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;
	int mat;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		int delta;
		mat = 0;
		FOR_ALL_LANES mat =
		    MAX(ctrl->timings[channel][slotrank].lanes[lane].rcven, mat);
		printram("normalize %d, %d, %d: mat %d\n",
		    channel, slotrank, lane, mat);

		delta = (mat >> 6) - ctrl->timings[channel][slotrank].io_latency;
		printram("normalize %d, %d, %d: delta %d\n",
		    channel, slotrank, lane, delta);

		ctrl->timings[channel][slotrank].roundtrip_latency += delta;
		ctrl->timings[channel][slotrank].io_latency += delta;
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}
}

int channel_test(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;

	slotrank = 0;
	FOR_ALL_POPULATED_CHANNELS
		if (mchbar_read32(MC_INIT_STATE_ch(channel)) & 0xa000) {
			printk(BIOS_EMERG, "Mini channel test failed (1): %d\n", channel);
			return MAKE_ERR;
		}
	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern0(ctrl, channel, 0x12345678, 0x98765432);
	}

	for (slotrank = 0; slotrank < 4; slotrank++)
		FOR_ALL_CHANNELS
			if (ctrl->rankmap[channel] & (1 << slotrank)) {
		FOR_ALL_LANES {
			mchbar_write32(IOSAV_By_ERROR_COUNT(lane), 0);
			mchbar_write32(IOSAV_By_BW_SERROR_C(lane), 0);
		}
		wait_for_iosav(channel);

		iosav_write_memory_test_sequence(ctrl, channel, slotrank);

		iosav_run_once_and_wait(channel);

		FOR_ALL_LANES
			if (mchbar_read32(IOSAV_By_ERROR_COUNT_ch(channel, lane))) {
				printk(BIOS_EMERG, "Mini channel test failed (2): %d, %d, %d\n",
				       channel, slotrank, lane);
				return MAKE_ERR;
			}
	}
	return 0;
}

void channel_scrub(ramctr_timing *ctrl)
{
	int channel, slotrank, row, rowsize;
	u8 bank;

	FOR_ALL_POPULATED_CHANNELS {
		wait_for_iosav(channel);
		fill_pattern0(ctrl, channel, 0, 0);
	}

	/*
	 * During runtime the "scrubber" will periodically scan through the memory in the
	 * physical address space, to identify and fix CRC errors.
	 * The following loops writes to every DRAM address, setting the ECC bits to the
	 * correct value. A read from this location will no longer return a CRC error,
	 * except when a bit has toggled due to external events.
	 * The same could be achieved by writing to the physical memory map, but it's
	 * much more difficult due to SMM remapping, ME stolen memory, GFX stolen memory,
	 * and firmware running in x86_32.
	 */
	FOR_ALL_POPULATED_CHANNELS FOR_ALL_POPULATED_RANKS {
		rowsize = 1 << ctrl->info.dimm[channel][slotrank >> 1].row_bits;
		for (bank = 0; bank < 8; bank++) {
			for (row = 0; row < rowsize; row += 16) {

				u8 gap = MAX((ctrl->tFAW >> 2) + 1, ctrl->tRRD);
				const struct iosav_ssq sequence[] = {
					/*
					 * DRAM command ACT
					 *  Opens the row for writing.
					 */
					[0] = {
						.sp_cmd_ctrl = {
							.command    = IOSAV_ACT,
							.ranksel_ap = 1,
						},
						.subseq_ctrl = {
							.cmd_executions = 1,
							.cmd_delay_gap  = gap,
							.post_ssq_wait  = ctrl->tRCD,
							.data_direction = SSQ_NA,
						},
						.sp_cmd_addr = {
							.address = row,
							.rowbits = 6,
							.bank    = bank,
							.rank    = slotrank,
						},
						.addr_update = {
							.inc_addr_1 = 1,
							.addr_wrap  = 18,
						},
					},
					/*
					 * DRAM command WR
					 *  Writes (128 + 1) * 8 (burst length) * 8 (bus width)
					 *  bytes.
					 */
					[1] = {
						.sp_cmd_ctrl = {
							.command    = IOSAV_WR,
							.ranksel_ap = 1,
						},
						.subseq_ctrl = {
							.cmd_executions = 129,
							.cmd_delay_gap  = 4,
							.post_ssq_wait  = ctrl->tWTR +
									  ctrl->CWL + 8,
							.data_direction = SSQ_WR,
						},
						.sp_cmd_addr = {
							.address = row,
							.rowbits = 0,
							.bank    = bank,
							.rank    = slotrank,
						},
						.addr_update = {
							.inc_addr_8 = 1,
							.addr_wrap  = 9,
						},
					},
					/*
					 * DRAM command PRE
					 *  Closes the row.
					 */
					[2] = {
						.sp_cmd_ctrl = {
							.command    = IOSAV_PRE,
							.ranksel_ap = 1,
						},
						.subseq_ctrl = {
							.cmd_executions = 1,
							.cmd_delay_gap  = 4,
							.post_ssq_wait  = ctrl->tRP,
							.data_direction = SSQ_NA,
						},
						.sp_cmd_addr = {
							.address = 0,
							.rowbits = 6,
							.bank    = bank,
							.rank    = slotrank,
						},
						.addr_update = {
							.addr_wrap = 18,
						},
					},
				};
				iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));

				iosav_run_queue(channel, 16, 0);

				wait_for_iosav(channel);
			}
		}
	}
}

void set_scrambling_seed(ramctr_timing *ctrl)
{
	int channel;

	/* FIXME: we hardcode seeds. Do we need to use some PRNG for them? I don't think so. */
	static u32 seeds[NUM_CHANNELS][3] = {
		{0x00009a36, 0xbafcfdcf, 0x46d1ab68},
		{0x00028bfa, 0x53fe4b49, 0x19ed5483}
	};
	FOR_ALL_POPULATED_CHANNELS {
		mchbar_clrbits32(SCHED_CBIT_ch(channel), 1 << 28);
		mchbar_write32(SCRAMBLING_SEED_1_ch(channel),    seeds[channel][0]);
		mchbar_write32(SCRAMBLING_SEED_2_HI_ch(channel), seeds[channel][1]);
		mchbar_write32(SCRAMBLING_SEED_2_LO_ch(channel), seeds[channel][2]);
	}
}

void set_wmm_behavior(const u32 cpu)
{
	if (IS_SANDY_CPU(cpu) && (IS_SANDY_CPU_D0(cpu) || IS_SANDY_CPU_D1(cpu))) {
		mchbar_write32(SC_WDBWM, 0x141d1519);
	} else {
		mchbar_write32(SC_WDBWM, 0x551d1519);
	}
}

void prepare_training(ramctr_timing *ctrl)
{
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		/* Always drive command bus */
		mchbar_setbits32(TC_RAP_ch(channel), 1 << 29);
	}

	udelay(1);

	FOR_ALL_POPULATED_CHANNELS {
		wait_for_iosav(channel);
	}
}

void set_read_write_timings(ramctr_timing *ctrl)
{
	/* Use a larger delay when running fast to improve stability */
	const u32 tRWDRDD_inc = ctrl->tCK <= TCK_1066MHZ ? 4 : 2;

	int channel, slotrank;

	FOR_ALL_POPULATED_CHANNELS {
		int min_pi = 10000;
		int max_pi = -10000;

		FOR_ALL_POPULATED_RANKS {
			max_pi = MAX(ctrl->timings[channel][slotrank].pi_coding, max_pi);
			min_pi = MIN(ctrl->timings[channel][slotrank].pi_coding, min_pi);
		}

		const u32 tWRDRDD = (max_pi - min_pi > 51) ? 0 : ctrl->ref_card_offset[channel];

		const u32 val = (ctrl->pi_coding_threshold < max_pi - min_pi) ? 3 : 2;

		dram_odt_stretch(ctrl, channel);

		const union tc_rwp_reg tc_rwp = {
			.tRRDR   = 0,
			.tRRDD   = val,
			.tWWDR   = val,
			.tWWDD   = val,
			.tRWDRDD = ctrl->ref_card_offset[channel] + tRWDRDD_inc,
			.tWRDRDD = tWRDRDD,
			.tRWSR   = 2,
			.dec_wrd = 1,
		};
		mchbar_write32(TC_RWP_ch(channel), tc_rwp.raw);
	}
}

void set_normal_operation(ramctr_timing *ctrl)
{
	int channel;
	FOR_ALL_POPULATED_CHANNELS {
		mchbar_write32(MC_INIT_STATE_ch(channel), 1 << 12 | ctrl->rankmap[channel]);
		mchbar_clrbits32(TC_RAP_ch(channel), 1 << 29);
	}
}

/* Encode the watermark latencies in a suitable format for graphics drivers consumption */
static int encode_wm(int ns)
{
	return (ns + 499) / 500;
}

/* FIXME: values in this function should be hardware revision-dependent */
void final_registers(ramctr_timing *ctrl)
{
	int channel;
	int t1_cycles = 0, t1_ns = 0, t2_ns;
	int t3_ns;
	u32 r32;

	/* FIXME: This register only exists on Ivy Bridge */
	mchbar_write32(WMM_READ_CONFIG, 0x46);

	FOR_ALL_CHANNELS {
		union tc_othp_reg tc_othp = {
			.raw = mchbar_read32(TC_OTHP_ch(channel)),
		};
		tc_othp.tCPDED = 1;
		mchbar_write32(TC_OTHP_ch(channel), tc_othp.raw);
	}

	/* 64 DCLKs until idle, decision per rank */
	mchbar_write32(PM_PDWN_CONFIG, get_power_down_mode(ctrl) << 8 | 64);

	FOR_ALL_CHANNELS
		mchbar_write32(PM_TRML_M_CONFIG_ch(channel), 0x00000aaa);

	mchbar_write32(PM_BW_LIMIT_CONFIG, 0x5f7003ff);
	mchbar_write32(PM_DLL_CONFIG, 0x00073000 | ctrl->mdll_wake_delay);

	FOR_ALL_CHANNELS {
		switch (ctrl->rankmap[channel]) {
			/* Unpopulated channel */
		case 0:
			mchbar_write32(PM_CMD_PWR_ch(channel), 0);
			break;
			/* Only single-ranked dimms */
		case 1:
		case 4:
		case 5:
			mchbar_write32(PM_CMD_PWR_ch(channel), 0x00373131);
			break;
			/* Dual-ranked dimms present */
		default:
			mchbar_write32(PM_CMD_PWR_ch(channel), 0x009b6ea1);
			break;
		}
	}

	mchbar_write32(MEM_TRML_ESTIMATION_CONFIG, 0xca9171e5);
	mchbar_clrsetbits32(MEM_TRML_THRESHOLDS_CONFIG, 0x00ffffff, 0x00e4d5d0);
	mchbar_clrbits32(MEM_TRML_INTERRUPT, 0x1f);

	FOR_ALL_CHANNELS {
		union tc_rfp_reg tc_rfp = {
			.raw = mchbar_read32(TC_RFP_ch(channel)),
		};
		tc_rfp.refresh_2x_control = 1;
		mchbar_write32(TC_RFP_ch(channel), tc_rfp.raw);
	}

	mchbar_setbits32(MC_INIT_STATE_G, 1 << 0);
	mchbar_setbits32(MC_INIT_STATE_G, 1 << 7);
	mchbar_write32(BANDTIMERS_SNB, 0xfa);

	/* Find a populated channel */
	FOR_ALL_POPULATED_CHANNELS
		break;

	t1_cycles = (mchbar_read32(TC_ZQCAL_ch(channel)) >> 8) & 0xff;
	r32 = mchbar_read32(PM_DLL_CONFIG);
	if (r32 & (1 << 17))
		t1_cycles += (r32 & 0xfff);
	t1_cycles += mchbar_read32(TC_SRFTP_ch(channel)) & 0xfff;
	t1_ns = t1_cycles * ctrl->tCK / 256 + 544;
	if (!(r32 & (1 << 17)))
		t1_ns += 500;

	t2_ns = 10 * ((mchbar_read32(SAPMTIMERS) >> 8) & 0xfff);
	if (mchbar_read32(SAPMCTL) & 8) {
		t3_ns  = 10 * ((mchbar_read32(BANDTIMERS_IVB) >> 8) & 0xfff);
		t3_ns += 10 * (mchbar_read32(SAPMTIMERS2_IVB) & 0xff);
	} else {
		t3_ns = 500;
	}

	/* The graphics driver will use these watermark values */
	printk(BIOS_DEBUG, "t123: %d, %d, %d\n", t1_ns, t2_ns, t3_ns);
	mchbar_clrsetbits32(SSKPD, 0x3f3f3f3f,
		((encode_wm(t1_ns) + encode_wm(t2_ns)) << 16) | (encode_wm(t1_ns) << 8) |
		((encode_wm(t3_ns) + encode_wm(t2_ns) + encode_wm(t1_ns)) << 24) | 0x0c);
}

void restore_timings(ramctr_timing *ctrl)
{
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		const union tc_rap_reg tc_rap = {
			.tRRD    = ctrl->tRRD,
			.tRTP    = ctrl->tRTP,
			.tCKE    = ctrl->tCKE,
			.tWTR    = ctrl->tWTR,
			.tFAW    = ctrl->tFAW,
			.tWR     = ctrl->tWR,
			.tCMD    = ctrl->cmd_stretch[channel],
		};
		mchbar_write32(TC_RAP_ch(channel), tc_rap.raw);
	}

	udelay(1);

	FOR_ALL_POPULATED_CHANNELS {
		wait_for_iosav(channel);
	}

	FOR_ALL_POPULATED_CHANNELS
		mchbar_setbits32(TC_RWP_ch(channel), 1 << 27);

	FOR_ALL_POPULATED_CHANNELS {
		udelay(1);
		mchbar_setbits32(SCHED_CBIT_ch(channel), 1 << 21);
	}

	printram("CPE\n");

	mchbar_write32(GDCRTRAININGMOD, 0);
	mchbar_write32(IOSAV_DC_MASK, 0);

	printram("CP5b\n");

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	u32 reg, addr;

	/* Poll for RCOMP */
	while (!(mchbar_read32(RCOMP_TIMER) & (1 << 16)))
		;

	do {
		reg = mchbar_read32(IOSAV_STATUS_ch(0));
	} while ((reg & 0x14) == 0);

	/* Set state of memory controller */
	mchbar_write32(MC_INIT_STATE_G, 0x116);
	mchbar_write32(MC_INIT_STATE, 0);

	/* Wait 500us */
	udelay(500);

	FOR_ALL_CHANNELS {
		/* Set valid rank CKE */
		reg = 0;
		reg = (reg & ~0x0f) | ctrl->rankmap[channel];
		addr = MC_INIT_STATE_ch(channel);
		mchbar_write32(addr, reg);

		/* Wait 10ns for ranks to settle */
		// udelay(0.01);

		reg = (reg & ~0xf0) | (ctrl->rankmap[channel] << 4);
		mchbar_write32(addr, reg);

		/* Write reset using a NOP */
		write_reset(ctrl);
	}

	/* MRS commands */
	dram_mrscommands(ctrl);

	printram("CP5c\n");

	mchbar_write32(GDCRTRAININGMOD_ch(0), 0);

	FOR_ALL_CHANNELS {
		mchbar_clrbits32(GDCRCMDDEBUGMUXCFG_Cz_S(channel), 0x3f << 24);
		udelay(2);
	}
}
