/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <string.h>
#include <arch/cpu.h>
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
	u32 r32 = MCHBAR32(MC_INIT_STATE_G);
	MCHBAR32(MC_INIT_STATE_G) = r32 |  (1 << 5);
	udelay(1);
	MCHBAR32(MC_INIT_STATE_G) = r32 & ~(1 << 5);
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
	reg = 0x4000;

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

		const dimm_attr *dimm = &dimms->dimm[channel][slot];
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
		MCHBAR32(GDCRCKPICODE_ch(channel)) = reg;

		/* Enable xover ctl & xover cmd */
		reg = get_XOVER_CMD(ctrl->rankmap[channel]);
		printram("XOVER CMD [%x] = %x\n", GDCRCMDPICODING_ch(channel), reg);
		MCHBAR32(GDCRCMDPICODING_ch(channel)) = reg;
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
		MCHBAR32_AND_OR(addr, ~(0xf << 10), (stretch << 12) | (stretch << 10));
		printk(RAM_DEBUG, "OTHP Workaround [%x] = %x\n", addr, MCHBAR32(addr));
	} else {
		addr = TC_OTHP_ch(channel);
		MCHBAR32_AND_OR(addr, ~(0xf << 16), (stretch << 16) | (stretch << 18));
		printk(RAM_DEBUG, "OTHP [%x] = %x\n", addr, MCHBAR32(addr));
	}
}

void dram_timing_regs(ramctr_timing *ctrl)
{
	u32 reg, addr, val32;
	int channel;

	FOR_ALL_CHANNELS {
		/* BIN parameters */
		reg = 0;
		reg |= (ctrl->tRCD <<  0);
		reg |= (ctrl->tRP  <<  4);
		reg |= (ctrl->CAS  <<  8);
		reg |= (ctrl->CWL  << 12);
		reg |= (ctrl->tRAS << 16);
		printram("DBP [%x] = %x\n", TC_DBP_ch(channel), reg);
		MCHBAR32(TC_DBP_ch(channel)) = reg;

		/* Regular access parameters */
		reg = 0;
		reg |= (ctrl->tRRD <<  0);
		reg |= (ctrl->tRTP <<  4);
		reg |= (ctrl->tCKE <<  8);
		reg |= (ctrl->tWTR << 12);
		reg |= (ctrl->tFAW << 16);
		reg |= (ctrl->tWR  << 24);
		reg |= (3 << 30);
		printram("RAP [%x] = %x\n", TC_RAP_ch(channel), reg);
		MCHBAR32(TC_RAP_ch(channel)) = reg;

		/* Other parameters */
		addr = TC_OTHP_ch(channel);
		reg = 0;
		reg |= (ctrl->tXPDLL << 0);
		reg |= (ctrl->tXP    << 5);
		reg |= (ctrl->tAONPD << 8);
		reg |= 0xa0000;
		printram("OTHP [%x] = %x\n", addr, reg);
		MCHBAR32(addr) = reg;

		/* Debug parameters - only applies to Ivy Bridge */
		if (IS_IVY_CPU(ctrl->cpu)) {
			reg = 0;

			/*
			 * If tXP and tXPDLL are very high, we need to increase them by one.
			 * This can only happen on Ivy Bridge, and when overclocking the RAM.
			 */
			if (ctrl->tXP >= 8)
				reg |= (1 << 12);

			if (ctrl->tXPDLL >= 32)
				reg |= (1 << 13);

			MCHBAR32(TC_DTP_ch(channel)) = reg;
		}

		MCHBAR32_OR(addr, 0x00020000);

		dram_odt_stretch(ctrl, channel);

		/*
		 * TC-Refresh timing parameters:
		 *   The tREFIx9 field should be programmed to minimum of 8.9 * tREFI (to allow
		 *   for possible delays from ZQ or isoc) and tRASmax (70us) divided by 1024.
		 */
		val32 = MIN((ctrl->tREFI * 89) / 10, (70000 << 8) / ctrl->tCK);

		reg = ((ctrl->tREFI & 0xffff) <<  0) |
		      ((ctrl->tRFC  & 0x01ff) << 16) | (((val32 / 1024) & 0x7f) << 25);

		printram("REFI [%x] = %x\n", TC_RFTP_ch(channel), reg);
		MCHBAR32(TC_RFTP_ch(channel)) = reg;

		MCHBAR32_OR(TC_RFP_ch(channel), 0xff);

		/* Self-refresh timing parameters */
		reg = 0;
		val32 = tDLLK;
		reg   = (reg & ~0x00000fff) | (val32 <<  0);
		val32 = ctrl->tXSOffset;
		reg   = (reg & ~0x0000f000) | (val32 << 12);
		val32 = tDLLK - ctrl->tXSOffset;
		reg   = (reg & ~0x03ff0000) | (val32 << 16);
		val32 = ctrl->tMOD - 8;
		reg   = (reg & ~0xf0000000) | (val32 << 28);
		printram("SRFTP [%x] = %x\n", TC_SRFTP_ch(channel), reg);
		MCHBAR32(TC_SRFTP_ch(channel)) = reg;
	}
}

void dram_dimm_mapping(ramctr_timing *ctrl)
{
	int channel;
	dimm_info *info = &ctrl->info;

	FOR_ALL_CHANNELS {
		dimm_attr *dimmA, *dimmB;
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
		MCHBAR32(MAD_DIMM(channel)) = ctrl->mad_dimm[channel] | ecc;
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
		reg = MCHBAR32(MAD_ZR);
		val = ch1size / 256;
		reg = (reg & ~0xff000000) | val << 24;
		reg = (reg & ~0x00ff0000) | (2 * val) << 16;
		MCHBAR32(MAD_ZR) = reg;
		MCHBAR32(MAD_CHNL) = 0x24;

	} else {
		reg = MCHBAR32(MAD_ZR);
		val = ch0size / 256;
		reg = (reg & ~0xff000000) | val << 24;
		reg = (reg & ~0x00ff0000) | (2 * val) << 16;
		MCHBAR32(MAD_ZR) = reg;
		MCHBAR32(MAD_CHNL) = 0x21;
	}
}

#define DEFAULT_PCI_MMIO_SIZE 2048

static unsigned int get_mmio_size(void)
{
	const struct device *dev;
	const struct northbridge_intel_sandybridge_config *cfg = NULL;

	dev = pcidev_path_on_root(PCI_DEVFN(0, 0));
	if (dev)
		cfg = dev->chip_info;

	/* If this is zero, it just means devicetree.cb didn't set it */
	if (!cfg || cfg->pci_mmio_size == 0)
		return DEFAULT_PCI_MMIO_SIZE;
	else
		return cfg->pci_mmio_size;
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

void dram_memorymap(ramctr_timing *ctrl, int me_uma_size)
{
	u32 reg, val, reclaim, tom, gfxstolen, gttsize;
	size_t tsegbase, toludbase, remapbase, gfxstolenbase, mmiosize, gttbase;
	size_t tsegsize, touudbase, remaplimit, mestolenbase, tsegbasedelta;
	uint16_t ggc;

	mmiosize = get_mmio_size();

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
		// Reclaim not possible
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

	/*
	 * Execute command queue - why is bit 22 set here?!
	 *
	 * This is actually using the IOSAV state machine as a timer, so refresh is allowed.
	 */
	iosav_run_queue(channel, 1, 1);

	wait_for_iosav(channel);
}

void dram_jedecreset(ramctr_timing *ctrl)
{
	u32 reg;
	int channel;

	while (!(MCHBAR32(RCOMP_TIMER) & (1 << 16)))
		;
	do {
		reg = MCHBAR32(IOSAV_STATUS_ch(0));
	} while ((reg & 0x14) == 0);

	/* Set state of memory controller */
	reg = 0x112;
	MCHBAR32(MC_INIT_STATE_G) = reg;
	MCHBAR32(MC_INIT_STATE) = 0;
	reg |= 2;		/* DDR reset */
	MCHBAR32(MC_INIT_STATE_G) = reg;

	/* Assert DIMM reset signal */
	MCHBAR32_AND(MC_INIT_STATE_G, ~(1 << 1));

	/* Wait 200us */
	udelay(200);

	/* Deassert DIMM reset signal */
	MCHBAR32_OR(MC_INIT_STATE_G, 1 << 1);

	/* Wait 500us */
	udelay(500);

	/* Enable DCLK */
	MCHBAR32_OR(MC_INIT_STATE_G, 1 << 2);

	/* XXX Wait 20ns */
	udelay(1);

	FOR_ALL_CHANNELS {
		/* Set valid rank CKE */
		reg = ctrl->rankmap[channel];
		MCHBAR32(MC_INIT_STATE_ch(channel)) = reg;

		/* Wait 10ns for ranks to settle */
		// udelay(0.01);

		reg = (reg & ~0xf0) | (ctrl->rankmap[channel] << 4);
		MCHBAR32(MC_INIT_STATE_ch(channel)) = reg;

		/* Write reset using a NOP */
		write_reset(ctrl);
	}
}

static void write_mrreg(ramctr_timing *ctrl, int channel, int slotrank, int reg, u32 val)
{
	wait_for_iosav(channel);

	if (ctrl->rank_mirror[channel][slotrank]) {
		/* DDR3 Rank1 Address mirror
		   swap the following pins:
		   A3<->A4, A5<->A6, A7<->A8, BA0<->BA1 */
		reg = ((reg >> 1) & 1) | ((reg << 1) & 2);
		val = (val & ~0x1f8) | ((val >> 1) & 0xa8) | ((val & 0xa8) << 1);
	}

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
				.command    = IOSAV_MRS,
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

	/* Execute command queue */
	iosav_run_once(channel);
}

static u32 make_mr0(ramctr_timing *ctrl, u8 rank)
{
	u16 mr0reg, mch_cas, mch_wr;
	static const u8 mch_wr_t[12] = { 1, 2, 3, 4, 0, 5, 0, 6, 0, 7, 0, 0 };
	const size_t is_mobile = get_platform_type() == PLATFORM_MOBILE;

	/* Convert CAS to MCH register friendly */
	if (ctrl->CAS < 12) {
		mch_cas = (u16) ((ctrl->CAS - 4) << 1);
	} else {
		mch_cas = (u16) (ctrl->CAS - 12);
		mch_cas = ((mch_cas << 1) | 0x1);
	}

	/* Convert tWR to MCH register friendly */
	mch_wr = mch_wr_t[ctrl->tWR - 5];

	/* DLL Reset - self clearing - set after CLK frequency has been changed */
	mr0reg = 1 << 8;

	mr0reg |= (mch_cas & 0x1) << 2;
	mr0reg |= (mch_cas & 0xe) << 3;
	mr0reg |= mch_wr << 9;

	/* Precharge PD - Fast (desktop) 1 or slow (mobile) 0 - mostly power-saving feature */
	mr0reg |= !is_mobile << 12;
	return mr0reg;
}

static void dram_mr0(ramctr_timing *ctrl, u8 rank, int channel)
{
	write_mrreg(ctrl, channel, rank, 0, make_mr0(ctrl, rank));
}

static odtmap get_ODT(ramctr_timing *ctrl, u8 rank, int channel)
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
		return (1 << 9) | (1 << 2);	// RZQ/8, RZQ/4
	case 60:
		return (1 << 2);	// RZQ/4
	case 120:
		return (1 << 6);	// RZQ/2
	default:
	case 0:
		return 0;
	}
}

static u32 make_mr1(ramctr_timing *ctrl, u8 rank, int channel)
{
	odtmap odt;
	u32 mr1reg;

	odt = get_ODT(ctrl, rank, channel);
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
	u16 pasr, cwl, mr2reg;
	odtmap odt;
	int srt;

	pasr = 0;
	cwl = ctrl->CWL - 5;
	odt = get_ODT(ctrl, rank, channel);

	srt = ctrl->extended_temperature_range && !ctrl->auto_self_refresh;

	mr2reg = 0;
	mr2reg = (mr2reg & ~0x07) | pasr;
	mr2reg = (mr2reg & ~0x38) | (cwl << 3);
	mr2reg = (mr2reg & ~0x40) | (ctrl->auto_self_refresh << 6);
	mr2reg = (mr2reg & ~0x80) | (srt << 7);
	mr2reg |= (odt.rttwr / 60) << 9;

	write_mrreg(ctrl, channel, rank, 2, mr2reg);
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
				.command    = IOSAV_NOP & ~(0xff << 8),
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
				.address = 1024,
				.rowbits = 6,
				.bank    = 0,
				.rank    = 0,
			},
			.addr_update = {
				.inc_rank   = 1,
				.addr_wrap  = 20,
			},
		},
	};
	iosav_write_sequence(BROADCAST_CH, zqcl_sequence, ARRAY_SIZE(zqcl_sequence));

	/* Execute command queue on all channels. Do it four times. */
	iosav_run_queue(BROADCAST_CH, 4, 0);

	FOR_ALL_CHANNELS {
		/* Wait for ref drained */
		wait_for_iosav(channel);
	}

	/* Refresh enable */
	MCHBAR32_OR(MC_INIT_STATE_G, 1 << 3);

	FOR_ALL_POPULATED_CHANNELS {
		MCHBAR32_AND(SCHED_CBIT_ch(channel), ~(1 << 21));

		wait_for_iosav(channel);

		slotrank = (ctrl->rankmap[channel] & 1) ? 0 : 2;

		/* Drain */
		wait_for_iosav(channel);

		iosav_write_zqcs_sequence(channel, slotrank, 4, 101, 31);

		/* Execute command queue */
		iosav_run_once(channel);

		/* Drain */
		wait_for_iosav(channel);
	}
}

static const u32 lane_base[] = {
	LANEBASE_B0, LANEBASE_B1, LANEBASE_B2, LANEBASE_B3,
	LANEBASE_B4, LANEBASE_B5, LANEBASE_B6, LANEBASE_B7,
	LANEBASE_ECC
};

void program_timings(ramctr_timing *ctrl, int channel)
{
	u32 reg32, reg_roundtrip_latency, reg_pi_code, reg_logic_delay, reg_io_latency;
	int lane;
	int slotrank, slot;
	int full_shift = 0;
	u16 pi_coding_ctrl[NUM_SLOTS];

	FOR_ALL_POPULATED_RANKS {
		if (full_shift < -ctrl->timings[channel][slotrank].pi_coding)
			full_shift = -ctrl->timings[channel][slotrank].pi_coding;
	}

	for (slot = 0; slot < NUM_SLOTS; slot++)
		switch ((ctrl->rankmap[channel] >> (2 * slot)) & 3) {
		case 0:
		default:
			pi_coding_ctrl[slot] = 0x7f;
			break;
		case 1:
			pi_coding_ctrl[slot] =
			    ctrl->timings[channel][2 * slot + 0].pi_coding + full_shift;
			break;
		case 2:
			pi_coding_ctrl[slot] =
			    ctrl->timings[channel][2 * slot + 1].pi_coding + full_shift;
			break;
		case 3:
			pi_coding_ctrl[slot] =
			    (ctrl->timings[channel][2 * slot].pi_coding +
			    ctrl->timings[channel][2 * slot + 1].pi_coding) / 2 + full_shift;
			break;
		}

	/* Enable CMD XOVER */
	reg32 = get_XOVER_CMD(ctrl->rankmap[channel]);
	reg32 |= (pi_coding_ctrl[0] & 0x3f) <<  6;
	reg32 |= (pi_coding_ctrl[0] & 0x40) <<  9;
	reg32 |= (pi_coding_ctrl[1] & 0x7f) << 18;
	reg32 |= (full_shift & 0x3f) | ((full_shift & 0x40) << 6);

	MCHBAR32(GDCRCMDPICODING_ch(channel)) = reg32;

	/* Enable CLK XOVER */
	reg_pi_code = get_XOVER_CLK(ctrl->rankmap[channel]);
	reg_logic_delay = 0;

	FOR_ALL_POPULATED_RANKS {
		int shift = ctrl->timings[channel][slotrank].pi_coding + full_shift;
		int offset_pi_code;
		if (shift < 0)
			shift = 0;

		offset_pi_code = ctrl->pi_code_offset + shift;

		/* Set CLK phase shift */
		reg_pi_code |= (offset_pi_code & 0x3f) << (6 * slotrank);
		reg_logic_delay |= ((offset_pi_code >> 6) & 1) << slotrank;
	}

	MCHBAR32(GDCRCKPICODE_ch(channel)) = reg_pi_code;
	MCHBAR32(GDCRCKLOGICDELAY_ch(channel)) = reg_logic_delay;

	reg_io_latency = MCHBAR32(SC_IO_LATENCY_ch(channel));
	reg_io_latency &= ~0xffff;

	reg_roundtrip_latency = 0;

	FOR_ALL_POPULATED_RANKS {
		int post_timA_min_high = 7, pre_timA_min_high = 7;
		int post_timA_max_high = 0, pre_timA_max_high = 0;
		int shift_402x = 0;
		int shift = ctrl->timings[channel][slotrank].pi_coding + full_shift;

		if (shift < 0)
			shift = 0;

		FOR_ALL_LANES {
			post_timA_min_high = MIN(post_timA_min_high,
				(ctrl->timings[channel][slotrank].lanes[lane].
					timA + shift) >> 6);
			pre_timA_min_high = MIN(pre_timA_min_high,
				ctrl->timings[channel][slotrank].lanes[lane].
						timA >> 6);
			post_timA_max_high = MAX(post_timA_max_high,
				(ctrl->timings[channel][slotrank].lanes[lane].
					timA + shift) >> 6);
			pre_timA_max_high = MAX(pre_timA_max_high,
				ctrl->timings[channel][slotrank].lanes[lane].
						timA >> 6);
		}

		if (pre_timA_max_high - pre_timA_min_high <
		    post_timA_max_high - post_timA_min_high)
			shift_402x = +1;

		else if (pre_timA_max_high - pre_timA_min_high >
			 post_timA_max_high - post_timA_min_high)
			shift_402x = -1;

		reg_io_latency |=
		    (ctrl->timings[channel][slotrank].io_latency + shift_402x -
		     post_timA_min_high) << (4 * slotrank);

		reg_roundtrip_latency |=
		    (ctrl->timings[channel][slotrank].roundtrip_latency +
		     shift_402x) << (8 * slotrank);

		FOR_ALL_LANES {
			MCHBAR32(lane_base[lane] + GDCRRX(channel, slotrank)) =
			    (((ctrl->timings[channel][slotrank].lanes[lane].
			       timA + shift) & 0x3f)
			     |
			     ((ctrl->timings[channel][slotrank].lanes[lane].
			       rising + shift) << 8)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timA + shift -
				(post_timA_min_high << 6)) & 0x1c0) << 10)
			     | ((ctrl->timings[channel][slotrank].lanes[lane].
				falling + shift) << 20));

			MCHBAR32(lane_base[lane] + GDCRTX(channel, slotrank)) =
			    (((ctrl->timings[channel][slotrank].lanes[lane].
			       timC + shift) & 0x3f)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timB + shift) & 0x3f) << 8)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timB + shift) & 0x1c0) << 9)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timC + shift) & 0x40) << 13));
		}
	}
	MCHBAR32(SC_ROUNDT_LAT_ch(channel)) = reg_roundtrip_latency;
	MCHBAR32(SC_IO_LATENCY_ch(channel)) = reg_io_latency;
}

static void test_timA(ramctr_timing *ctrl, int channel, int slotrank)
{
	wait_for_iosav(channel);

	iosav_write_read_mpr_sequence(channel, slotrank, ctrl->tMOD, 1, 3, 15, ctrl->CAS + 36);

	/* Execute command queue */
	iosav_run_once(channel);

	wait_for_iosav(channel);
}

static int does_lane_work(ramctr_timing *ctrl, int channel, int slotrank, int lane)
{
	u32 timA = ctrl->timings[channel][slotrank].lanes[lane].timA;

	return (MCHBAR32(lane_base[lane] +
		GDCRTRAININGRESULT(channel, (timA / 32) & 1)) >> (timA % 32)) & 1;
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

static void discover_timA_coarse(ramctr_timing *ctrl, int channel, int slotrank, int *upperA)
{
	int timA;
	int statistics[NUM_LANES][128];
	int lane;

	for (timA = 0; timA < 128; timA++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timA = timA;
		}
		program_timings(ctrl, channel);

		test_timA(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			statistics[lane][timA] = !does_lane_work(ctrl, channel, slotrank, lane);
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(statistics[lane], 128);
		ctrl->timings[channel][slotrank].lanes[lane].timA = rn.middle;
		upperA[lane] = rn.end;
		if (upperA[lane] < rn.middle)
			upperA[lane] += 128;

		printram("timA: %d, %d, %d: 0x%02x-0x%02x-0x%02x\n",
			 channel, slotrank, lane, rn.start, rn.middle, rn.end);
	}
}

static void discover_timA_fine(ramctr_timing *ctrl, int channel, int slotrank, int *upperA)
{
	int timA_delta;
	int statistics[NUM_LANES][51];
	int lane, i;

	memset(statistics, 0, sizeof(statistics));

	for (timA_delta = -25; timA_delta <= 25; timA_delta++) {

		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timA
				= upperA[lane] + timA_delta + 0x40;
		}
		program_timings(ctrl, channel);

		for (i = 0; i < 100; i++) {
			test_timA(ctrl, channel, slotrank);
			FOR_ALL_LANES {
				statistics[lane][timA_delta + 25] +=
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

		ctrl->timings[channel][slotrank].lanes[lane].timA =
			(last_zero + first_all) / 2 + upperA[lane];

		printram("Aval: %d, %d, %d: %x\n", channel, slotrank,
			lane, ctrl->timings[channel][slotrank].lanes[lane].timA);
	}
}

static int discover_402x(ramctr_timing *ctrl, int channel, int slotrank, int *upperA)
{
	int works[NUM_LANES];
	int lane;

	while (1) {
		int all_works = 1, some_works = 0;

		program_timings(ctrl, channel);
		test_timA(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			works[lane] = !does_lane_work(ctrl, channel, slotrank, lane);

			if (works[lane])
				some_works = 1;
			else
				all_works = 0;
		}
		if (all_works)
			return 0;

		if (!some_works) {
			if (ctrl->timings[channel][slotrank].roundtrip_latency < 2) {
				printk(BIOS_EMERG, "402x discovery failed (1): %d, %d\n",
				       channel, slotrank);
				return MAKE_ERR;
			}
			ctrl->timings[channel][slotrank].roundtrip_latency -= 2;
			printram("4024 -= 2;\n");
			continue;
		}
		ctrl->timings[channel][slotrank].io_latency += 2;
		printram("4028 += 2;\n");

		if (ctrl->timings[channel][slotrank].io_latency >= 0x10) {
			printk(BIOS_EMERG, "402x discovery failed (2): %d, %d\n",
			       channel, slotrank);
			return MAKE_ERR;
		}
		FOR_ALL_LANES if (works[lane]) {
			ctrl->timings[channel][slotrank].lanes[lane].timA += 128;
			upperA[lane] += 128;
			printram("increment %d, %d, %d\n", channel, slotrank, lane);
		}
	}
	return 0;
}

struct timA_minmax {
	int timA_min_high, timA_max_high;
};

static void pre_timA_change(ramctr_timing *ctrl, int channel, int slotrank,
			    struct timA_minmax *mnmx)
{
	int lane;
	mnmx->timA_min_high = 7;
	mnmx->timA_max_high = 0;

	FOR_ALL_LANES {
		if (mnmx->timA_min_high >
		    (ctrl->timings[channel][slotrank].lanes[lane].timA >> 6))
			mnmx->timA_min_high =
			    (ctrl->timings[channel][slotrank].lanes[lane].timA >> 6);
		if (mnmx->timA_max_high <
		    (ctrl->timings[channel][slotrank].lanes[lane].timA >> 6))
			mnmx->timA_max_high =
			    (ctrl->timings[channel][slotrank].lanes[lane].timA >> 6);
	}
}

static void post_timA_change(ramctr_timing *ctrl, int channel, int slotrank,
			     struct timA_minmax *mnmx)
{
	struct timA_minmax post;
	int shift_402x = 0;

	/* Get changed maxima */
	pre_timA_change(ctrl, channel, slotrank, &post);

	if (mnmx->timA_max_high - mnmx->timA_min_high <
	    post.timA_max_high - post.timA_min_high)
		shift_402x = +1;

	else if (mnmx->timA_max_high - mnmx->timA_min_high >
		 post.timA_max_high - post.timA_min_high)
		shift_402x = -1;

	else
		shift_402x = 0;

	ctrl->timings[channel][slotrank].io_latency += shift_402x;
	ctrl->timings[channel][slotrank].roundtrip_latency += shift_402x;
	printram("4024 += %d;\n", shift_402x);
	printram("4028 += %d;\n", shift_402x);
}

/*
 * Compensate the skew between DQS and DQs.
 *
 * To ease PCB design, a small skew between Data Strobe signals and Data Signals is allowed.
 * The controller has to measure and compensate this skew for every byte-lane. By delaying
 * either all DQ signals or DQS signal, a full phase shift can be introduced. It is assumed
 * that one byte-lane's DQs signals have the same routing delay.
 *
 * To measure the actual skew, the DRAM is placed in "read leveling" mode. In read leveling
 * mode the DRAM-chip outputs an alternating periodic pattern. The memory controller iterates
 * over all possible values to do a full phase shift and issues read commands. With DQS and
 * DQ in phase the data being read is expected to alternate on every byte:
 *
 *   0xFF 0x00 0xFF ...
 *
 * Once the controller has detected this pattern a bit in the result register is set for the
 * current phase shift.
 */
int read_training(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;
	int err;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		int all_high, some_high;
		int upperA[NUM_LANES];
		struct timA_minmax mnmx;

		wait_for_iosav(channel);

		iosav_write_prea_sequence(channel, slotrank, ctrl->tRP, 0);

		/* Execute command queue */
		iosav_run_once(channel);

		MCHBAR32(GDCRTRAININGMOD) = (slotrank << 2) | 0x8001;

		ctrl->timings[channel][slotrank].io_latency = 4;
		ctrl->timings[channel][slotrank].roundtrip_latency = 55;
		program_timings(ctrl, channel);

		discover_timA_coarse(ctrl, channel, slotrank, upperA);

		all_high = 1;
		some_high = 0;
		FOR_ALL_LANES {
			if (ctrl->timings[channel][slotrank].lanes[lane].timA >= 0x40)
				some_high = 1;
			else
				all_high = 0;
		}

		if (all_high) {
			ctrl->timings[channel][slotrank].io_latency--;
			printram("4028--;\n");
			FOR_ALL_LANES {
				ctrl->timings[channel][slotrank].lanes[lane].timA -= 0x40;
				upperA[lane] -= 0x40;

			}
		} else if (some_high) {
			ctrl->timings[channel][slotrank].roundtrip_latency++;
			ctrl->timings[channel][slotrank].io_latency++;
			printram("4024++;\n");
			printram("4028++;\n");
		}

		program_timings(ctrl, channel);

		pre_timA_change(ctrl, channel, slotrank, &mnmx);

		err = discover_402x(ctrl, channel, slotrank, upperA);
		if (err)
			return err;

		post_timA_change(ctrl, channel, slotrank, &mnmx);
		pre_timA_change(ctrl, channel, slotrank, &mnmx);

		discover_timA_fine(ctrl, channel, slotrank, upperA);

		post_timA_change(ctrl, channel, slotrank, &mnmx);
		pre_timA_change(ctrl, channel, slotrank, &mnmx);

		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timA -=
					mnmx.timA_min_high * 0x40;
		}
		ctrl->timings[channel][slotrank].io_latency -= mnmx.timA_min_high;
		printram("4028 -= %d;\n", mnmx.timA_min_high);

		post_timA_change(ctrl, channel, slotrank, &mnmx);

		printram("4/8: %d, %d, %x, %x\n", channel, slotrank,
		       ctrl->timings[channel][slotrank].roundtrip_latency,
		       ctrl->timings[channel][slotrank].io_latency);

		printram("final results:\n");
		FOR_ALL_LANES
			printram("Aval: %d, %d, %d: %x\n", channel, slotrank, lane,
			    ctrl->timings[channel][slotrank].lanes[lane].timA);

		MCHBAR32(GDCRTRAININGMOD) = 0;

		toggle_io_reset();
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		MCHBAR32(IOSAV_By_BW_MASK_ch(channel, lane)) = 0;
	}
	return 0;
}

static void test_timC(ramctr_timing *ctrl, int channel, int slotrank)
{
	int lane;

	FOR_ALL_LANES {
		MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane)) = 0;
		MCHBAR32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
	}

	wait_for_iosav(channel);

	iosav_write_misc_write_sequence(ctrl, channel, slotrank,
		MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1), 4, 4, 500, 18);

	/* Execute command queue */
	iosav_run_once(channel);

	wait_for_iosav(channel);

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
				.address = 1024,
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
				.cmd_executions = 8,
				.cmd_delay_gap  = MAX(ctrl->tRRD, (ctrl->tFAW >> 2) + 1),
				.post_ssq_wait  = ctrl->CAS,
				.data_direction = SSQ_NA,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_bank  = 1,
				.addr_wrap = 18,
			},
		},
		/* DRAM command RD */
		[2] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_RD,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 500,
				.cmd_delay_gap  = 4,
				.post_ssq_wait  = MAX(ctrl->tRTP, 8),
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 0,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.inc_addr_8 = 1,
				.addr_wrap  = 18,
			},
		},
		/* DRAM command PREA */
		[3] = {
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
				.address = 1024,
				.rowbits = 6,
				.bank    = 0,
				.rank    = slotrank,
			},
			.addr_update = {
				.addr_wrap  = 18,
			},
		},
	};
	iosav_write_sequence(channel, rd_sequence, ARRAY_SIZE(rd_sequence));

	/* Execute command queue */
	iosav_run_once(channel);

	wait_for_iosav(channel);
}

static void timC_threshold_process(int *data, const int count)
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

static int discover_timC(ramctr_timing *ctrl, int channel, int slotrank)
{
	int timC;
	int stats[NUM_LANES][MAX_TIMC + 1];
	int lane;

	wait_for_iosav(channel);

	iosav_write_prea_sequence(channel, slotrank, ctrl->tRP, 18);

	/* Execute command queue */
	iosav_run_once(channel);

	for (timC = 0; timC <= MAX_TIMC; timC++) {
		FOR_ALL_LANES ctrl->timings[channel][slotrank].lanes[lane].timC = timC;
		program_timings(ctrl, channel);

		test_timC(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			stats[lane][timC] = MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane));
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(stats[lane], ARRAY_SIZE(stats[lane]));

		if (rn.all || rn.length < 8) {
			printk(BIOS_EMERG, "timC discovery failed: %d, %d, %d\n",
			       channel, slotrank, lane);
			/*
			 * With command training not being done yet, the lane can be erroneous.
			 * Take the average as reference and try again to find a run.
			 */
			timC_threshold_process(stats[lane], ARRAY_SIZE(stats[lane]));
			rn = get_longest_zero_run(stats[lane], ARRAY_SIZE(stats[lane]));

			if (rn.all || rn.length < 8) {
				printk(BIOS_EMERG, "timC recovery failed\n");
				return MAKE_ERR;
			}
		}
		ctrl->timings[channel][slotrank].lanes[lane].timC = rn.middle;
		printram("timC: %d, %d, %d: 0x%02x-0x%02x-0x%02x\n",
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
	MCHBAR8(IOSAV_DATA_CTL_ch(channel)) = num_cachelines / 8 - 1;
}

static void fill_pattern0(ramctr_timing *ctrl, int channel, u32 a, u32 b)
{
	unsigned int j;
	unsigned int channel_offset = get_precedening_channels(ctrl, channel) * 0x40;

	for (j = 0; j < 16; j++)
		write32((void *)(0x04000000 + channel_offset + 4 * j), j & 2 ? b : a);

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
	unsigned int channel_offset = get_precedening_channels(ctrl, channel) * 0x40;
	unsigned int channel_step = 0x40 * num_of_channels(ctrl);

	for (j = 0; j < 16; j++)
		write32((void *)(0x04000000 + channel_offset + j * 4), 0xffffffff);

	for (j = 0; j < 16; j++)
		write32((void *)(0x04000000 + channel_offset + channel_step + j * 4), 0);

	sfence();

	program_wdb_pattern_length(channel, 16);
}

static void precharge(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;

	FOR_ALL_POPULATED_CHANNELS {
		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling = 16;
			ctrl->timings[channel][slotrank].lanes[lane].rising = 16;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_for_iosav(channel);

			iosav_write_read_mpr_sequence(
				channel, slotrank, ctrl->tMOD, 3, 4, 1, ctrl->CAS + 8);

			/* Execute command queue */
			iosav_run_once(channel);

			wait_for_iosav(channel);
		}

		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling = 48;
			ctrl->timings[channel][slotrank].lanes[lane].rising = 48;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_for_iosav(channel);

			iosav_write_read_mpr_sequence(
				channel, slotrank, ctrl->tMOD, 3, 4, 1, ctrl->CAS + 8);

			/* Execute command queue */
			iosav_run_once(channel);

			wait_for_iosav(channel);
		}
	}
}

static void test_timB(ramctr_timing *ctrl, int channel, int slotrank)
{
	/* enable DQs on this slotrank */
	write_mrreg(ctrl, channel, slotrank, 1, make_mr1(ctrl, slotrank, channel) | 1 << 7);

	wait_for_iosav(channel);

	const struct iosav_ssq sequence[] = {
		/* DRAM command NOP */
		[0] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_NOP,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->CWL + ctrl->tWLO,
				.data_direction = SSQ_WR,
			},
			.sp_cmd_addr = {
				.address = 8,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
		/* DRAM command NOP */
		[1] = {
			.sp_cmd_ctrl = {
				.command    = IOSAV_NOP_ALT,
				.ranksel_ap = 1,
			},
			.subseq_ctrl = {
				.cmd_executions = 1,
				.cmd_delay_gap  = 3,
				.post_ssq_wait  = ctrl->CAS + 38,
				.data_direction = SSQ_RD,
			},
			.sp_cmd_addr = {
				.address = 4,
				.rowbits = 0,
				.bank    = 0,
				.rank    = slotrank,
			},
		},
	};
	iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));

	/* Execute command queue */
	iosav_run_once(channel);

	wait_for_iosav(channel);

	/* disable DQs on this slotrank */
	write_mrreg(ctrl, channel, slotrank, 1,
		make_mr1(ctrl, slotrank, channel) | 1 << 12 | 1 << 7);
}

static int discover_timB(ramctr_timing *ctrl, int channel, int slotrank)
{
	int timB;
	int statistics[NUM_LANES][128];
	int lane;

	MCHBAR32(GDCRTRAININGMOD) = 0x108052 | (slotrank << 2);

	for (timB = 0; timB < 128; timB++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timB = timB;
		}
		program_timings(ctrl, channel);

		test_timB(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			statistics[lane][timB] =  !((MCHBAR32(lane_base[lane] +
				GDCRTRAININGRESULT(channel, (timB / 32) & 1)) >>
				(timB % 32)) & 1);
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(statistics[lane], 128);
		/*
		 * timC is a direct function of timB's 6 LSBs. Some tests increments the value
		 * of timB by a small value, which might cause the 6-bit value to overflow if
		 * it's close to 0x3f. Increment the value by a small offset if it's likely
		 * to overflow, to make sure it won't overflow while running tests and bricks
		 * the system due to a non matching timC.
		 *
		 * TODO: find out why some tests (edge write discovery) increment timB.
		 */
		if ((rn.start & 0x3f) == 0x3e)
			rn.start += 2;
		else if ((rn.start & 0x3f) == 0x3f)
			rn.start += 1;

		ctrl->timings[channel][slotrank].lanes[lane].timB = rn.start;
		if (rn.all) {
			printk(BIOS_EMERG, "timB discovery failed: %d, %d, %d\n",
			       channel, slotrank, lane);

			return MAKE_ERR;
		}
		printram("timB: %d, %d, %d: 0x%02x-0x%02x-0x%02x\n",
				 channel, slotrank, lane, rn.start, rn.middle, rn.end);
	}
	return 0;
}

static int get_timB_high_adjust(u64 val)
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
	MCHBAR32(GDCRTRAININGMOD) = 0x200;
	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern1(ctrl, channel);
	}
	FOR_ALL_POPULATED_CHANNELS FOR_ALL_POPULATED_RANKS {

		/* Reset read and write WDB pointers */
		MCHBAR32(IOSAV_DATA_CTL_ch(channel)) = 0x10001;

		wait_for_iosav(channel);

		iosav_write_misc_write_sequence(ctrl, channel, slotrank, 3, 1, 3, 3, 31);

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);

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
					.address = 1024,
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
			/* DRAM command RD */
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

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);
		FOR_ALL_LANES {
			u64 res = MCHBAR32(lane_base[lane] + GDCRTRAININGRESULT1(channel));
			res |= ((u64) MCHBAR32(lane_base[lane] +
				GDCRTRAININGRESULT2(channel))) << 32;
			old = ctrl->timings[channel][slotrank].lanes[lane].timB;
			ctrl->timings[channel][slotrank].lanes[lane].timB +=
				get_timB_high_adjust(res) * 64;

			printram("High adjust %d:%016llx\n", lane, res);
			printram("Bval+: %d, %d, %d, %x -> %x\n", channel, slotrank, lane,
				old, ctrl->timings[channel][slotrank].lanes[lane].timB);
		}
	}
	MCHBAR32(GDCRTRAININGMOD) = 0;
}

static void write_op(ramctr_timing *ctrl, int channel)
{
	int slotrank;

	wait_for_iosav(channel);

	/* choose an existing rank.  */
	slotrank = !(ctrl->rankmap[channel] & 1) ? 2 : 0;

	iosav_write_zqcs_sequence(channel, slotrank, 4, 4, 31);

	/* Execute command queue */
	iosav_run_once(channel);

	wait_for_iosav(channel);
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
int write_training(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;
	int err;

	FOR_ALL_POPULATED_CHANNELS
		MCHBAR32_OR(TC_RWP_ch(channel), 1 << 27);

	FOR_ALL_POPULATED_CHANNELS {
		write_op(ctrl, channel);
		MCHBAR32_OR(SCHED_CBIT_ch(channel), 1 << 21);
	}

	/* Refresh disable */
	MCHBAR32_AND(MC_INIT_STATE_G, ~(1 << 3));
	FOR_ALL_POPULATED_CHANNELS {
		write_op(ctrl, channel);
	}

	/* Enable write leveling on all ranks
	   Disable all DQ outputs
	   Only NOP is allowed in this mode */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS
		write_mrreg(ctrl, channel, slotrank, 1,
				make_mr1(ctrl, slotrank, channel) | 1 << 12 | 1 << 7);

	MCHBAR32(GDCRTRAININGMOD) = 0x108052;

	toggle_io_reset();

	/* Set any valid value for timB, it gets corrected later */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_timB(ctrl, channel, slotrank);
		if (err)
			return err;
	}

	/* Disable write leveling on all ranks */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS
		write_mrreg(ctrl, channel, slotrank, 1, make_mr1(ctrl, slotrank, channel));

	MCHBAR32(GDCRTRAININGMOD) = 0;

	FOR_ALL_POPULATED_CHANNELS
		wait_for_iosav(channel);

	/* Refresh enable */
	MCHBAR32_OR(MC_INIT_STATE_G, 1 << 3);

	FOR_ALL_POPULATED_CHANNELS {
		MCHBAR32_AND(SCHED_CBIT_ch(channel), ~(1 << 21));
		MCHBAR32(IOSAV_STATUS_ch(channel));
		wait_for_iosav(channel);

		iosav_write_zqcs_sequence(channel, 0, 4, 101, 31);

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);
	}

	toggle_io_reset();

	printram("CPE\n");
	precharge(ctrl);
	printram("CPF\n");

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		MCHBAR32_AND(IOSAV_By_BW_MASK_ch(channel, lane), 0);
	}

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern0(ctrl, channel, 0xaaaaaaaa, 0x55555555);
	}

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_timC(ctrl, channel, slotrank);
		if (err)
			return err;
	}

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	/* measure and adjust timB timings */
	train_write_flyby(ctrl);

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		MCHBAR32_AND(IOSAV_By_BW_MASK_ch(channel, lane), 0);
	}
	return 0;
}

static int test_command_training(ramctr_timing *ctrl, int channel, int slotrank)
{
	struct ram_rank_timings saved_rt = ctrl->timings[channel][slotrank];
	int timC_delta;
	int lanes_ok = 0;
	int ctr = 0;
	int lane;

	for (timC_delta = -5; timC_delta <= 5; timC_delta++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timC =
			    saved_rt.lanes[lane].timC + timC_delta;
		}
		program_timings(ctrl, channel);
		FOR_ALL_LANES {
			MCHBAR32(IOSAV_By_ERROR_COUNT(lane)) = 0;
		}

		/* Reset read WDB pointer */
		MCHBAR32(IOSAV_DATA_CTL_ch(channel)) = 0x1f;

		wait_for_iosav(channel);

		iosav_write_command_training_sequence(ctrl, channel, slotrank, ctr);

		/* Program LFSR for the RD/WR subsequences */
		MCHBAR32(IOSAV_n_ADDRESS_LFSR_ch(channel, 1)) = 0x389abcd;
		MCHBAR32(IOSAV_n_ADDRESS_LFSR_ch(channel, 2)) = 0x389abcd;

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);
		FOR_ALL_LANES {
			u32 r32 = MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane));

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
	unsigned int offset = get_precedening_channels(ctrl, channel) * 0x40;
	unsigned int step = 0x40 * num_of_channels(ctrl);

	if (patno) {
		u8 base8 = 0x80 >> ((patno - 1) % 8);
		u32 base = base8 | (base8 << 8) | (base8 << 16) | (base8 << 24);
		for (i = 0; i < 32; i++) {
			for (j = 0; j < 16; j++) {
				u32 val = use_base[patno - 1][i] & (1 << (j / 2)) ? base : 0;

				if (invert[patno - 1][i] & (1 << (j / 2)))
					val = ~val;

				write32((void *)((1 << 26) + offset + i * step + j * 4), val);
			}
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(pattern); i++) {
			for (j = 0; j < 16; j++) {
				const u32 val = pattern[i][j];
				write32((void *)((1 << 26) + offset + i * step + j * 4), val);
			}
		}
		sfence();
	}

	program_wdb_pattern_length(channel, 256);
}

static void reprogram_320c(ramctr_timing *ctrl)
{
	int channel, slotrank;

	FOR_ALL_POPULATED_CHANNELS {
		wait_for_iosav(channel);

		/* Choose an existing rank */
		slotrank = !(ctrl->rankmap[channel] & 1) ? 2 : 0;

		iosav_write_zqcs_sequence(channel, slotrank, 4, 4, 31);

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);
		MCHBAR32_OR(SCHED_CBIT_ch(channel), 1 << 21);
	}

	/* refresh disable */
	MCHBAR32_AND(MC_INIT_STATE_G, ~(1 << 3));
	FOR_ALL_POPULATED_CHANNELS {
		wait_for_iosav(channel);

		/* choose an existing rank.  */
		slotrank = !(ctrl->rankmap[channel] & 1) ? 2 : 0;

		iosav_write_zqcs_sequence(channel, slotrank, 4, 4, 31);

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);
	}

	/* JEDEC reset */
	dram_jedecreset(ctrl);

	/* MRS commands */
	dram_mrscommands(ctrl);

	toggle_io_reset();
}

#define CT_MIN_PI	-127
#define CT_MAX_PI	128
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

	MCHBAR32(TC_RAP_ch(channel)) =
		  (ctrl->tRRD <<  0)
		| (ctrl->tRTP <<  4)
		| (ctrl->tCKE <<  8)
		| (ctrl->tWTR << 12)
		| (ctrl->tFAW << 16)
		| (ctrl->tWR  << 24)
		| (ctrl->cmd_stretch[channel] << 30);

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
		printram("cmd_stretch: %d, %d: 0x%02x-0x%02x-0x%02x\n",
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
		 * While c320c discovery seems to succeed raminit will fail in write training.
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
			printk(BIOS_EMERG, "c320c discovery failed\n");
			return err;
		}

		printram("Using CMD rate %uT on channel %u\n", cmdrate + 1, channel);
	}

	FOR_ALL_POPULATED_CHANNELS
	program_timings(ctrl, channel);

	reprogram_320c(ctrl);
	return 0;
}

static int discover_edges_real(ramctr_timing *ctrl, int channel, int slotrank, int *edges)
{
	int edge;
	int stats[NUM_LANES][MAX_EDGE_TIMING + 1];
	int lane;

	for (edge = 0; edge <= MAX_EDGE_TIMING; edge++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].rising  = edge;
			ctrl->timings[channel][slotrank].lanes[lane].falling = edge;
		}
		program_timings(ctrl, channel);

		FOR_ALL_LANES {
			MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane)) = 0;
			MCHBAR32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
		}

		wait_for_iosav(channel);

		iosav_write_read_mpr_sequence(
			channel, slotrank, ctrl->tMOD, 500, 4, 1, ctrl->CAS + 8);

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);

		FOR_ALL_LANES {
			stats[lane][edge] = MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane));
		}
	}

	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(stats[lane], MAX_EDGE_TIMING + 1);
		edges[lane] = rn.middle;

		if (rn.all) {
			printk(BIOS_EMERG, "edge discovery failed: %d, %d, %d\n", channel,
			       slotrank, lane);
			return MAKE_ERR;
		}
		printram("eval %d, %d, %d: %02x\n", channel, slotrank, lane, edges[lane]);
	}
	return 0;
}

int discover_edges(ramctr_timing *ctrl)
{
	int falling_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int rising_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane;
	int err;

	MCHBAR32(GDCRTRAININGMOD) = 0;

	toggle_io_reset();

	FOR_ALL_POPULATED_CHANNELS FOR_ALL_LANES {
		MCHBAR32(IOSAV_By_BW_MASK_ch(channel, lane)) = 0;
	}

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern0(ctrl, channel, 0, 0);
		FOR_ALL_LANES {
			MCHBAR32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
		}

		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling = 16;
			ctrl->timings[channel][slotrank].lanes[lane].rising = 16;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_for_iosav(channel);

			iosav_write_read_mpr_sequence(
				channel, slotrank, ctrl->tMOD, 3, 4, 1, ctrl->CAS + 8);

			/* Execute command queue */
			iosav_run_once(channel);

			wait_for_iosav(channel);
		}

		/* XXX: check any measured value ? */

		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling = 48;
			ctrl->timings[channel][slotrank].lanes[lane].rising  = 48;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_for_iosav(channel);

			iosav_write_read_mpr_sequence(
				channel, slotrank, ctrl->tMOD, 3, 4, 1, ctrl->CAS + 8);

			/* Execute command queue */
			iosav_run_once(channel);

			wait_for_iosav(channel);
		}

		/* XXX: check any measured value ? */

		FOR_ALL_LANES {
			MCHBAR32(IOSAV_By_BW_MASK_ch(channel, lane)) =
				~MCHBAR32(IOSAV_By_BW_SERROR_ch(channel, lane)) & 0xff;
		}

		fill_pattern0(ctrl, channel, 0, 0xffffffff);
	}

	/*
	 * FIXME: Under some conditions, vendor BIOS sets both edges to the same value. It will
	 *        also use a single loop. It would seem that it is a debugging configuration.
	 */
	MCHBAR32(IOSAV_DC_MASK) = 0x300;
	printram("discover falling edges:\n[%x] = %x\n", IOSAV_DC_MASK, 0x300);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_real(ctrl, channel, slotrank,
			falling_edges[channel][slotrank]);
		if (err)
			return err;
	}

	MCHBAR32(IOSAV_DC_MASK) = 0x200;
	printram("discover rising edges:\n[%x] = %x\n", IOSAV_DC_MASK, 0x200);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_real(ctrl, channel, slotrank,
				    rising_edges[channel][slotrank]);
		if (err)
			return err;
	}

	MCHBAR32(IOSAV_DC_MASK) = 0;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].falling =
		    falling_edges[channel][slotrank][lane];
		ctrl->timings[channel][slotrank].lanes[lane].rising =
		    rising_edges[channel][slotrank][lane];
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		MCHBAR32(IOSAV_By_BW_MASK_ch(channel, lane)) = 0;
	}
	return 0;
}

static int discover_edges_write_real(ramctr_timing *ctrl, int channel, int slotrank, int *edges)
{
	int edge;
	u32 raw_stats[MAX_EDGE_TIMING + 1];
	int stats[MAX_EDGE_TIMING + 1];
	const int reg3000b24[] = { 0, 0xc, 0x2c };
	int lane, i;
	int lower[NUM_LANES];
	int upper[NUM_LANES];
	int pat;

	FOR_ALL_LANES {
		lower[lane] = 0;
		upper[lane] = MAX_EDGE_TIMING;
	}

	for (i = 0; i < 3; i++) {
		MCHBAR32(GDCRTRAININGMOD_ch(channel)) = reg3000b24[i] << 24;
		printram("[%x] = 0x%08x\n", GDCRTRAININGMOD_ch(channel), reg3000b24[i] << 24);

		for (pat = 0; pat < NUM_PATTERNS; pat++) {
			fill_pattern5(ctrl, channel, pat);
			printram("using pattern %d\n", pat);

			for (edge = 0; edge <= MAX_EDGE_TIMING; edge++) {
				FOR_ALL_LANES {
					ctrl->timings[channel][slotrank].lanes[lane].
						rising = edge;
					ctrl->timings[channel][slotrank].lanes[lane].
						falling = edge;
				}
				program_timings(ctrl, channel);

				FOR_ALL_LANES {
					MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane)) = 0;
					MCHBAR32(IOSAV_By_BW_SERROR_C_ch(channel, lane));
				}
				wait_for_iosav(channel);

				iosav_write_data_write_sequence(ctrl, channel, slotrank);

				/* Execute command queue */
				iosav_run_once(channel);

				wait_for_iosav(channel);
				FOR_ALL_LANES {
					MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane));
				}

				/* FIXME: This register only exists on Ivy Bridge */
				raw_stats[edge] = MCHBAR32(IOSAV_BYTE_SERROR_C_ch(channel));
			}

			FOR_ALL_LANES {
				struct run rn;
				for (edge = 0; edge <= MAX_EDGE_TIMING; edge++)
					stats[edge] = !!(raw_stats[edge] & (1 << lane));

				rn = get_longest_zero_run(stats, MAX_EDGE_TIMING + 1);

				printram("edges: %d, %d, %d: 0x%02x-0x%02x-0x%02x, "
					 "0x%02x-0x%02x\n", channel, slotrank, i, rn.start,
					 rn.middle, rn.end, rn.start + ctrl->edge_offset[i],
					 rn.end - ctrl->edge_offset[i]);

				lower[lane] = MAX(rn.start + ctrl->edge_offset[i], lower[lane]);
				upper[lane] = MIN(rn.end   - ctrl->edge_offset[i], upper[lane]);

				edges[lane] = (lower[lane] + upper[lane]) / 2;
				if (rn.all || (lower[lane] > upper[lane])) {
					printk(BIOS_EMERG, "edge write discovery failed: "
						"%d, %d, %d\n", channel, slotrank, lane);

					return MAKE_ERR;
				}
			}
		}
	}

	MCHBAR32(GDCRTRAININGMOD_ch(0)) = 0;
	printram("CPA\n");
	return 0;
}

int discover_edges_write(ramctr_timing *ctrl)
{
	int falling_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int  rising_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane, err;

	/*
	 * FIXME: Under some conditions, vendor BIOS sets both edges to the same value. It will
	 *        also use a single loop. It would seem that it is a debugging configuration.
	 */
	MCHBAR32(IOSAV_DC_MASK) = 0x300;
	printram("discover falling edges write:\n[%x] = %x\n", IOSAV_DC_MASK, 0x300);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_write_real(ctrl, channel, slotrank,
					falling_edges[channel][slotrank]);
		if (err)
			return err;
	}

	MCHBAR32(IOSAV_DC_MASK) = 0x200;
	printram("discover rising edges write:\n[%x] = %x\n", IOSAV_DC_MASK, 0x200);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_write_real(ctrl, channel, slotrank,
					 rising_edges[channel][slotrank]);
		if (err)
			return err;
	}

	MCHBAR32(IOSAV_DC_MASK) = 0;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].falling =
				falling_edges[channel][slotrank][lane];

		ctrl->timings[channel][slotrank].lanes[lane].rising =
				rising_edges[channel][slotrank][lane];
	}

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		MCHBAR32(IOSAV_By_BW_MASK_ch(channel, lane)) = 0;
	}
	return 0;
}

static void test_timC_write(ramctr_timing *ctrl, int channel, int slotrank)
{
	wait_for_iosav(channel);

	iosav_write_aggressive_write_read_sequence(ctrl, channel, slotrank);

	/* Execute command queue */
	iosav_run_once(channel);

	wait_for_iosav(channel);
}

int discover_timC_write(ramctr_timing *ctrl)
{
	const u8 rege3c_b24[3] = { 0, 0x0f, 0x2f };
	int i, pat;

	int lower[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int upper[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		lower[channel][slotrank][lane] = 0;
		upper[channel][slotrank][lane] = MAX_TIMC;
	}

	/*
	 * Enable IOSAV_n_SPECIAL_COMMAND_ADDR optimization.
	 * FIXME: This must only be done on Ivy Bridge.
	 */
	MCHBAR32(MCMNTS_SPARE) = 1;
	printram("discover timC write:\n");

	for (i = 0; i < 3; i++)
		FOR_ALL_POPULATED_CHANNELS {

			/* FIXME: Setting the Write VREF must only be done on Ivy Bridge */
			MCHBAR32_AND_OR(GDCRCMDDEBUGMUXCFG_Cz_S(channel),
					~0x3f000000, rege3c_b24[i] << 24);

			udelay(2);

			for (pat = 0; pat < NUM_PATTERNS; pat++) {
				FOR_ALL_POPULATED_RANKS {
					int timC;
					u32 raw_stats[MAX_TIMC + 1];
					int stats[MAX_TIMC + 1];

					/* Make sure rn.start < rn.end */
					stats[MAX_TIMC] = 1;

					fill_pattern5(ctrl, channel, pat);

					for (timC = 0; timC < MAX_TIMC; timC++) {
						FOR_ALL_LANES {
							ctrl->timings[channel][slotrank]
								.lanes[lane].timC = timC;
						}
						program_timings(ctrl, channel);

						test_timC_write (ctrl, channel, slotrank);

						/* FIXME: Another IVB-only register! */
						raw_stats[timC] = MCHBAR32(
							IOSAV_BYTE_SERROR_C_ch(channel));
					}
					FOR_ALL_LANES {
						struct run rn;
						for (timC = 0; timC < MAX_TIMC; timC++) {
							stats[timC] = !!(raw_stats[timC]
									& (1 << lane));
						}

						rn = get_longest_zero_run(stats, MAX_TIMC + 1);
						if (rn.all) {
							printk(BIOS_EMERG,
								"timC write discovery failed: "
								"%d, %d, %d\n", channel,
								slotrank, lane);

							return MAKE_ERR;
						}
						printram("timC: %d, %d, %d: "
							 "0x%02x-0x%02x-0x%02x, "
							 "0x%02x-0x%02x\n", channel, slotrank,
							 i, rn.start, rn.middle, rn.end,
							 rn.start + ctrl->timC_offset[i],
							 rn.end   - ctrl->timC_offset[i]);

						lower[channel][slotrank][lane] =
							MAX(rn.start + ctrl->timC_offset[i],
							    lower[channel][slotrank][lane]);

						upper[channel][slotrank][lane] =
							MIN(rn.end - ctrl->timC_offset[i],
							    upper[channel][slotrank][lane]);

					}
				}
			}
		}

	FOR_ALL_CHANNELS {
		/* FIXME: Setting the Write VREF must only be done on Ivy Bridge */
		MCHBAR32_AND(GDCRCMDDEBUGMUXCFG_Cz_S(channel), ~0x3f000000);
		udelay(2);
	}

	/*
	 * Disable IOSAV_n_SPECIAL_COMMAND_ADDR optimization.
	 * FIXME: This must only be done on Ivy Bridge.
	 */
	MCHBAR32(MCMNTS_SPARE) = 0;

	printram("CPB\n");

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		printram("timC %d, %d, %d: %x\n", channel, slotrank, lane,
		       (lower[channel][slotrank][lane] +
			upper[channel][slotrank][lane]) / 2);

		ctrl->timings[channel][slotrank].lanes[lane].timC =
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
		    MAX(ctrl->timings[channel][slotrank].lanes[lane].timA, mat);
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
		if (MCHBAR32(MC_INIT_STATE_ch(channel)) & 0xa000) {
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
			MCHBAR32(IOSAV_By_ERROR_COUNT(lane)) = 0;
			MCHBAR32(IOSAV_By_BW_SERROR_C(lane)) = 0;
		}
		wait_for_iosav(channel);

		iosav_write_memory_test_sequence(ctrl, channel, slotrank);

		/* Execute command queue */
		iosav_run_once(channel);

		wait_for_iosav(channel);
		FOR_ALL_LANES
			if (MCHBAR32(IOSAV_By_ERROR_COUNT_ch(channel, lane))) {
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
							.addr_wrap  = 18,
						},
					},
				};
				iosav_write_sequence(channel, sequence, ARRAY_SIZE(sequence));

				/* Execute command queue */
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
		MCHBAR32(SCHED_CBIT_ch(channel)) &= ~(1 << 28);
		MCHBAR32(SCRAMBLING_SEED_1_ch(channel))    = seeds[channel][0];
		MCHBAR32(SCRAMBLING_SEED_2_HI_ch(channel)) = seeds[channel][1];
		MCHBAR32(SCRAMBLING_SEED_2_LO_ch(channel)) = seeds[channel][2];
	}
}

void set_wmm_behavior(const u32 cpu)
{
	if (IS_SANDY_CPU(cpu) && (IS_SANDY_CPU_D0(cpu) || IS_SANDY_CPU_D1(cpu))) {
		MCHBAR32(SC_WDBWM) = 0x141d1519;
	} else {
		MCHBAR32(SC_WDBWM) = 0x551d1519;
	}
}

void prepare_training(ramctr_timing *ctrl)
{
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		/* Always drive command bus */
		MCHBAR32_OR(TC_RAP_ch(channel), 1 << 29);
	}

	udelay(1);

	FOR_ALL_POPULATED_CHANNELS {
		wait_for_iosav(channel);
	}
}

void set_read_write_timings(ramctr_timing *ctrl)
{
	int channel, slotrank;

	FOR_ALL_POPULATED_CHANNELS {
		u32 b20, b4_8_12;
		int min_pi = 10000;
		int max_pi = -10000;

		FOR_ALL_POPULATED_RANKS {
			max_pi = MAX(ctrl->timings[channel][slotrank].pi_coding, max_pi);
			min_pi = MIN(ctrl->timings[channel][slotrank].pi_coding, min_pi);
		}

		b20 = (max_pi - min_pi > 51) ? 0 : ctrl->ref_card_offset[channel];

		b4_8_12 = (ctrl->pi_coding_threshold < max_pi - min_pi) ? 0x3330 : 0x2220;

		dram_odt_stretch(ctrl, channel);

		MCHBAR32(TC_RWP_ch(channel)) = (1 << 27) | (2 << 24) | (b20 << 20) |
			((ctrl->ref_card_offset[channel] + 2) << 16) | b4_8_12;
	}
}

void set_normal_operation(ramctr_timing *ctrl)
{
	int channel;
	FOR_ALL_POPULATED_CHANNELS {
		MCHBAR32(MC_INIT_STATE_ch(channel)) = (1 << 12) | ctrl->rankmap[channel];
		MCHBAR32_AND(TC_RAP_ch(channel), ~(1 << 29));
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
	const bool is_mobile = get_platform_type() == PLATFORM_MOBILE;

	int channel;
	int t1_cycles = 0, t1_ns = 0, t2_ns;
	int t3_ns;
	u32 r32;

	/* FIXME: This register only exists on Ivy Bridge */
	MCHBAR32(WMM_READ_CONFIG) = 0x46;

	FOR_ALL_CHANNELS
		MCHBAR32_AND_OR(TC_OTHP_ch(channel), ~(3 << 12), 1 << 12);

	if (is_mobile)
		/* APD - DLL Off, 64 DCLKs until idle, decision per rank */
		MCHBAR32(PM_PDWN_CONFIG) = 0x00000740;
	else
		/* APD - PPD,     64 DCLKs until idle, decision per rank */
		MCHBAR32(PM_PDWN_CONFIG) = 0x00000340;

	FOR_ALL_CHANNELS
		MCHBAR32(PM_TRML_M_CONFIG_ch(channel)) = 0x00000aaa;

	MCHBAR32(PM_BW_LIMIT_CONFIG) = 0x5f7003ff;	// OK
	MCHBAR32(PM_DLL_CONFIG) = 0x00073000 | ctrl->mdll_wake_delay; // OK

	FOR_ALL_CHANNELS {
		switch (ctrl->rankmap[channel]) {
			/* Unpopulated channel */
		case 0:
			MCHBAR32(PM_CMD_PWR_ch(channel)) = 0;
			break;
			/* Only single-ranked dimms */
		case 1:
		case 4:
		case 5:
			MCHBAR32(PM_CMD_PWR_ch(channel)) = 0x00373131;
			break;
			/* Dual-ranked dimms present */
		default:
			MCHBAR32(PM_CMD_PWR_ch(channel)) = 0x009b6ea1;
			break;
		}
	}

	MCHBAR32(MEM_TRML_ESTIMATION_CONFIG) = 0xca9171e5;
	MCHBAR32_AND_OR(MEM_TRML_THRESHOLDS_CONFIG, ~0x00ffffff, 0x00e4d5d0);
	MCHBAR32_AND(MEM_TRML_INTERRUPT, ~0x1f);

	FOR_ALL_CHANNELS
		MCHBAR32_AND_OR(TC_RFP_ch(channel), ~(3 << 16), 1 << 16);

	MCHBAR32_OR(MC_INIT_STATE_G, 1 << 0);
	MCHBAR32_OR(MC_INIT_STATE_G, 1 << 7);
	MCHBAR32(BANDTIMERS_SNB) = 0xfa;

	/* Find a populated channel */
	FOR_ALL_POPULATED_CHANNELS
		break;

	t1_cycles = (MCHBAR32(TC_ZQCAL_ch(channel)) >> 8) & 0xff;
	r32 = MCHBAR32(PM_DLL_CONFIG);
	if (r32 & (1 << 17))
		t1_cycles += (r32 & 0xfff);
	t1_cycles += MCHBAR32(TC_SRFTP_ch(channel)) & 0xfff;
	t1_ns = t1_cycles * ctrl->tCK / 256 + 544;
	if (!(r32 & (1 << 17)))
		t1_ns += 500;

	t2_ns = 10 * ((MCHBAR32(SAPMTIMERS) >> 8) & 0xfff);
	if (MCHBAR32(SAPMCTL) & 8) {
		t3_ns  = 10 * ((MCHBAR32(BANDTIMERS_IVB) >> 8) & 0xfff);
		t3_ns += 10 * (MCHBAR32(SAPMTIMERS2_IVB) & 0xff);
	} else {
		t3_ns = 500;
	}

	/* The graphics driver will use these watermark values */
	printk(BIOS_DEBUG, "t123: %d, %d, %d\n", t1_ns, t2_ns, t3_ns);
	MCHBAR32_AND_OR(SSKPD, ~0x3f3f3f3f,
		((encode_wm(t1_ns) + encode_wm(t2_ns)) << 16) | (encode_wm(t1_ns) << 8) |
		((encode_wm(t3_ns) + encode_wm(t2_ns) + encode_wm(t1_ns)) << 24) | 0x0c);
}

void restore_timings(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;

	FOR_ALL_POPULATED_CHANNELS {
		MCHBAR32(TC_RAP_ch(channel)) =
			  (ctrl->tRRD <<  0)
			| (ctrl->tRTP <<  4)
			| (ctrl->tCKE <<  8)
			| (ctrl->tWTR << 12)
			| (ctrl->tFAW << 16)
			| (ctrl->tWR  << 24)
			| (ctrl->cmd_stretch[channel] << 30);
	}

	udelay(1);

	FOR_ALL_POPULATED_CHANNELS {
		wait_for_iosav(channel);
	}

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		MCHBAR32(IOSAV_By_BW_MASK_ch(channel, lane)) = 0;
	}

	FOR_ALL_POPULATED_CHANNELS
		MCHBAR32_OR(TC_RWP_ch(channel), 1 << 27);

	FOR_ALL_POPULATED_CHANNELS {
		udelay(1);
		MCHBAR32_OR(SCHED_CBIT_ch(channel), 1 << 21);
	}

	printram("CPE\n");

	MCHBAR32(GDCRTRAININGMOD) = 0;
	MCHBAR32(IOSAV_DC_MASK) = 0;

	printram("CP5b\n");

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	u32 reg, addr;

	/* Poll for RCOMP */
	while (!(MCHBAR32(RCOMP_TIMER) & (1 << 16)))
		;

	do {
		reg = MCHBAR32(IOSAV_STATUS_ch(0));
	} while ((reg & 0x14) == 0);

	/* Set state of memory controller */
	MCHBAR32(MC_INIT_STATE_G) = 0x116;
	MCHBAR32(MC_INIT_STATE)   = 0;

	/* Wait 500us */
	udelay(500);

	FOR_ALL_CHANNELS {
		/* Set valid rank CKE */
		reg = 0;
		reg = (reg & ~0x0f) | ctrl->rankmap[channel];
		addr = MC_INIT_STATE_ch(channel);
		MCHBAR32(addr) = reg;

		/* Wait 10ns for ranks to settle */
		// udelay(0.01);

		reg = (reg & ~0xf0) | (ctrl->rankmap[channel] << 4);
		MCHBAR32(addr) = reg;

		/* Write reset using a NOP */
		write_reset(ctrl);
	}

	/* MRS commands */
	dram_mrscommands(ctrl);

	printram("CP5c\n");

	MCHBAR32(GDCRTRAININGMOD_ch(0)) = 0;

	FOR_ALL_CHANNELS {
		MCHBAR32_AND(GDCRCMDDEBUGMUXCFG_Cz_S(channel), ~0x3f000000);
		udelay(2);
	}
}
