/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <cf9_reset.h>
#include <device/mmio.h>
#include <device/device.h>
#include <lib.h>
#include <pc80/mc146818rtc.h>
#include <spd.h>
#include <string.h>
#include "raminit.h"
#include "i945.h"
#include "chip.h"
#include <device/dram/ddr2.h>
#include <timestamp.h>

/* Debugging macros. */
#if CONFIG(DEBUG_RAM_SETUP)
#define PRINTK_DEBUG(x...)	printk(BIOS_DEBUG, x)
#else
#define PRINTK_DEBUG(x...)
#endif

#define RAM_INITIALIZATION_COMPLETE	(1 << 19)

#define RAM_COMMAND_SELF_REFRESH	(0x0 << 16)
#define RAM_COMMAND_NOP			(0x1 << 16)
#define RAM_COMMAND_PRECHARGE		(0x2 << 16)
#define RAM_COMMAND_MRS			(0x3 << 16)
#define RAM_COMMAND_EMRS		(0x4 << 16)
#define RAM_COMMAND_CBR			(0x6 << 16)
#define RAM_COMMAND_NORMAL		(0x7 << 16)

#define RAM_EMRS_1			(0x0 << 21)
#define RAM_EMRS_2			(0x1 << 21)
#define RAM_EMRS_3			(0x2 << 21)

#define DEFAULT_PCI_MMIO_SIZE		768
static int get_dimm_spd_address(struct sys_info *sysinfo, int device)
{
	if (sysinfo->spd_addresses)
		return sysinfo->spd_addresses[device];
	else
		return 0x50 + device;

}

static __attribute__((noinline)) void do_ram_command(u32 command)
{
	u32 reg32;

	reg32 = mchbar_read32(DCC);
	reg32 &= ~((3 << 21) | (1 << 20) | (1 << 19) | (7 << 16));
	reg32 |= command;

	/* Also set Init Complete */
	if (command == RAM_COMMAND_NORMAL)
		reg32 |= RAM_INITIALIZATION_COMPLETE;

	PRINTK_DEBUG("   Sending RAM command 0x%08x", reg32);

	mchbar_write32(DCC, reg32);	/* This is the actual magic */

	PRINTK_DEBUG("...done\n");

	udelay(1);
}

static void ram_read32(uintptr_t offset)
{
	PRINTK_DEBUG("   RAM read: %08x\n", offset);

	read32((void *)offset);
}

void sdram_dump_mchbar_registers(void)
{
	int i;
	printk(BIOS_DEBUG, "Dumping MCHBAR Registers\n");

	for (i = 0; i < 0xfff; i += 4) {
		if (mchbar_read32(i) == 0)
			continue;
		printk(BIOS_DEBUG, "0x%04x: 0x%08x\n", i, mchbar_read32(i));
	}
}

static int memclk(void)
{
	int offset = CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM) ? 1 : 0;

	switch (((mchbar_read32(CLKCFG) >> 4) & 7) - offset) {
	case 1: return 400;
	case 2: return 533;
	case 3: return 667;
	default:
		printk(BIOS_DEBUG, "%s: unknown register value %x\n", __func__,
			((mchbar_read32(CLKCFG) >> 4) & 7) - offset);
	}
	return -1;
}

static u16 fsbclk(void)
{
	if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM)) {
		switch (mchbar_read32(CLKCFG) & 7) {
		case 0: return 400;
		case 1: return 533;
		case 3: return 667;
		default:
			printk(BIOS_DEBUG, "%s: unknown register value %x\n", __func__,
				mchbar_read32(CLKCFG) & 7);
		}
		return 0xffff;
	} else if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GC)) {
		switch (mchbar_read32(CLKCFG) & 7) {
		case 0: return 1066;
		case 1: return 533;
		case 2: return 800;
		default:
			printk(BIOS_DEBUG, "%s: unknown register value %x\n", __func__,
				mchbar_read32(CLKCFG) & 7);
		}
		return 0xffff;
	}
}

static int sdram_capabilities_max_supported_memory_frequency(void)
{
	u32 reg32;

#if CONFIG_MAXIMUM_SUPPORTED_FREQUENCY
	return CONFIG_MAXIMUM_SUPPORTED_FREQUENCY;
#endif

	reg32 = pci_read_config32(HOST_BRIDGE, 0xe4); /* CAPID0 + 4 */
	reg32 &= (7 << 0);

	switch (reg32) {
	case 4: return 400;
	case 3: return 533;
	case 2: return 667;
	}
	/* Newer revisions of this chipset rather support faster memory clocks,
	 * so if it's a reserved value, return the fastest memory clock that we
	 * know of and can handle
	 */
	return 667;
}

/**
 * @brief determine whether chipset is capable of dual channel interleaved mode
 *
 * @return 1 if interleaving is supported, 0 otherwise
 */
static int sdram_capabilities_interleave(void)
{
	u32 reg32;

	reg32 = pci_read_config32(HOST_BRIDGE, 0xe4); /* CAPID0 + 4 */
	reg32 >>= 25;
	reg32 &= 1;

	return (!reg32);
}

/**
 * @brief determine whether chipset is capable of two memory channels
 *
 * @return 1 if dual channel operation is supported, 0 otherwise
 */
static int sdram_capabilities_dual_channel(void)
{
	u32 reg32;

	reg32 = pci_read_config32(HOST_BRIDGE, 0xe4); /* CAPID0 + 4 */
	reg32 >>= 24;
	reg32 &= 1;

	return (!reg32);
}

static int sdram_capabilities_enhanced_addressing_xor(void)
{
	u8 reg8;

	reg8 = pci_read_config8(HOST_BRIDGE, 0xe5); /* CAPID0 + 5 */
	reg8 &= (1 << 7);

	return (!reg8);
}

#define GFX_FREQUENCY_CAP_166MHZ	0x04
#define GFX_FREQUENCY_CAP_200MHZ	0x03
#define GFX_FREQUENCY_CAP_250MHZ	0x02
#define GFX_FREQUENCY_CAP_ALL		0x00

static int sdram_capabilities_core_frequencies(void)
{
	u8 reg8;

	reg8 = pci_read_config8(HOST_BRIDGE, 0xe5); /* CAPID0 + 5 */
	reg8 &= (1 << 3) | (1 << 2) | (1 << 1);
	reg8 >>= 1;

	return reg8;
}

static void sdram_detect_errors(struct sys_info *sysinfo)
{
	u8 reg8;
	u8 do_reset = 0;

	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_2);

	if (reg8 & ((1 << 7) | (1 << 2))) {
		if (reg8 & (1 << 2)) {
			printk(BIOS_DEBUG, "SLP S4# Assertion Width Violation.\n");
			/* Write back clears bit 2 */
			pci_write_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_2, reg8);
			do_reset = 1;

		}

		if (reg8 & (1 << 7)) {
			printk(BIOS_DEBUG, "DRAM initialization was interrupted.\n");
			reg8 &= ~(1 << 7);
			pci_write_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_2, reg8);
			do_reset = 1;
		}

		/* Set SLP_S3# Assertion Stretch Enable */
		reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3);
		reg8 |= (1 << 3);
		pci_write_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3, reg8);

		if (do_reset) {
			printk(BIOS_DEBUG, "Reset required.\n");
			full_reset();
		}
	}

	/* Set DRAM initialization bit in ICH7 */
	pci_or_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_2, 1 << 7);

	/* clear self refresh status if check is disabled or not a resume */
	if (!CONFIG(CHECK_SLFRCS_ON_RESUME) || sysinfo->boot_path != BOOT_PATH_RESUME) {
		mchbar_setbits8(SLFRCS, 3);
	} else {
		/* Validate self refresh config */
		if (((sysinfo->dimm[0] != SYSINFO_DIMM_NOT_POPULATED) ||
		     (sysinfo->dimm[1] != SYSINFO_DIMM_NOT_POPULATED)) &&
		    !(mchbar_read8(SLFRCS) & (1 << 0))) {
			do_reset = 1;
		}
		if (((sysinfo->dimm[2] != SYSINFO_DIMM_NOT_POPULATED) ||
		     (sysinfo->dimm[3] != SYSINFO_DIMM_NOT_POPULATED)) &&
		    !(mchbar_read8(SLFRCS) & (1 << 1))) {
			do_reset = 1;
		}
	}

	if (do_reset) {
		printk(BIOS_DEBUG, "Reset required.\n");
		full_reset();
	}
}

struct timings {
	u32 min_tCLK_cas[8];
	u32 min_tRAS;
	u32 min_tRP;
	u32 min_tRCD;
	u32 min_tWR;
	u32 min_tRFC;
	u32 max_tRR;
	u8 cas_mask;
};

/**
 * @brief loop over dimms and save maximal timings
 */
static void gather_common_timing(struct sys_info *sysinfo, struct timings *saved_timings)
{

	int i, j;
	u8 raw_spd[SPD_SIZE_MAX_DDR2];
	u8 dimm_mask = 0;

	memset(saved_timings, 0, sizeof(*saved_timings));
	saved_timings->max_tRR = UINT32_MAX;
	saved_timings->cas_mask = SPD_CAS_LATENCY_DDR2_3 | SPD_CAS_LATENCY_DDR2_4
				| SPD_CAS_LATENCY_DDR2_5;

	/**
	 * i945 supports two DIMMs, in two configurations:
	 *
	 * - single channel with two DIMMs
	 * - dual channel with one DIMM per channel
	 *
	 * In practice dual channel mainboards have their SPD at 0x50/0x52
	 * whereas single channel configurations have their SPD at 0x50/0x51.
	 *
	 * The capability register knows a lot about the channel configuration
	 * but for now we stick with the information we gather via SPD.
	 */

	printk(BIOS_DEBUG, "This mainboard supports ");
	if (sdram_capabilities_dual_channel()) {
		sysinfo->dual_channel = 1;
		printk(BIOS_DEBUG, "Dual Channel Operation.\n");
	} else {
		sysinfo->dual_channel = 0;
		printk(BIOS_DEBUG, "only Single Channel Operation.\n");
	}

	for (i = 0; i < (2 * DIMM_SOCKETS); i++) {
		int device = get_dimm_spd_address(sysinfo, i), bytes_read;
		struct dimm_attr_ddr2_st dimm_info;

		/* Initialize the socket information with a sane value */
		sysinfo->dimm[i] = SYSINFO_DIMM_NOT_POPULATED;

		/* Dual Channel not supported, but Channel 1? Bail out */
		if (!sdram_capabilities_dual_channel() && (i >> 1))
			continue;

		if (smbus_read_byte(device, SPD_MEMORY_TYPE) !=
					SPD_MEMORY_TYPE_SDRAM_DDR2) {
			printk(BIOS_DEBUG, "DDR II Channel %d Socket %d: N/A\n",
				(i >> 1), (i & 1));
			continue;
		}

		/*
		 * spd_decode_ddr2() needs a 128-byte sized array but
		 * only the first 64 bytes contain data needed for raminit.
		 */

		bytes_read = i2c_eeprom_read(device, 0, 64, raw_spd);
		printk(BIOS_DEBUG, "Reading SPD using i2c block operation.\n");
		if (CONFIG(DEBUG_RAM_SETUP) && bytes_read > 0)
			hexdump(raw_spd, bytes_read);
		if (bytes_read != 64) {
			/* Try again with SMBUS byte read */
			printk(BIOS_DEBUG, "i2c block operation failed,"
				" trying smbus byte operation.\n");
			for (j = 0; j < 64; j++)
				raw_spd[j] = smbus_read_byte(device, j);
			if (CONFIG(DEBUG_RAM_SETUP))
				hexdump(raw_spd, 64);
		}

		if (spd_decode_ddr2(&dimm_info, raw_spd) != SPD_STATUS_OK) {
			printk(BIOS_WARNING, "Encountered problems with SPD, "
				"skipping this DIMM.\n");
			continue;
		}

		if (CONFIG(DEBUG_RAM_SETUP))
			dram_print_spd_ddr2(&dimm_info);

		if (dimm_info.flags.is_ecc)
			die("\nError: ECC memory not supported by this chipset\n");

		if (spd_dimm_is_registered_ddr2(dimm_info.dimm_type))
			die("\nError: Registered memory not supported by this chipset\n");

		printk(BIOS_DEBUG, "DDR II Channel %d Socket %d: ", (i >> 1), (i & 1));
		/**
		 * There are 5 different possible populations for a DIMM socket:
		 * 0. x16 double ranked (X16DS)
		 * 1. x8 double ranked  (X8DS)
		 * 2. x16 single ranked (X16SS)
		 * 3. x8 double stacked (X8DDS)
		 * 4. Unpopulated
		 */
		switch (dimm_info.width) {
		case 8:
			switch (dimm_info.ranks) {
			case 2:
				printk(BIOS_DEBUG, "x8DDS\n");
				sysinfo->dimm[i] = SYSINFO_DIMM_X8DDS;
				break;
			case 1:
				printk(BIOS_DEBUG, "x8DS\n");
				sysinfo->dimm[i] = SYSINFO_DIMM_X8DS;
				break;
			default:
				printk(BIOS_DEBUG, "Unsupported.\n");
			}
			break;
		case 16:
			switch (dimm_info.ranks) {
			case 2:
				printk(BIOS_DEBUG, "x16DS\n");
				sysinfo->dimm[i] = SYSINFO_DIMM_X16DS;
				break;
			case 1:
				printk(BIOS_DEBUG, "x16SS\n");
				sysinfo->dimm[i] = SYSINFO_DIMM_X16SS;
				break;
			default:
				printk(BIOS_DEBUG, "Unsupported.\n");
			}
			break;
		default:
			die("Unsupported DDR-II memory width.\n");
		}

		/* Is the current DIMM a stacked DIMM? */
		if (dimm_info.flags.stacked)
			sysinfo->package = SYSINFO_PACKAGE_STACKED;

		if (!dimm_info.flags.bl8)
			die("Only DDR-II RAM with burst length 8 is supported.\n");

		if (dimm_info.ranksize_mb < 128)
			die("DDR-II rank size smaller than 128MB is not supported.\n");

		sysinfo->banksize[i * 2] = dimm_info.ranksize_mb / 32;
		printk(BIOS_DEBUG, "DIMM %d side 0 = %d MB\n", i, sysinfo->banksize[i * 2] * 32);
		if (dimm_info.ranks == 2) {
			sysinfo->banksize[(i * 2) + 1] =
				dimm_info.ranksize_mb / 32;
			printk(BIOS_DEBUG, "DIMM %d side 1 = %d MB\n",
				i, sysinfo->banksize[(i * 2) + 1] * 32);
		}

		sysinfo->rows[i] = dimm_info.row_bits;
		sysinfo->cols[i] = dimm_info.col_bits;
		sysinfo->banks[i] = dimm_info.banks;

		/* int min_tRAS, min_tRP, min_tRCD, min_tWR, min_tRFC; */
		saved_timings->min_tRAS = MAX(saved_timings->min_tRAS, dimm_info.tRAS);
		saved_timings->min_tRP = MAX(saved_timings->min_tRP, dimm_info.tRP);
		saved_timings->min_tRCD = MAX(saved_timings->min_tRCD, dimm_info.tRCD);
		saved_timings->min_tWR = MAX(saved_timings->min_tWR, dimm_info.tWR);
		saved_timings->min_tRFC = MAX(saved_timings->min_tRFC, dimm_info.tRFC);
		saved_timings->max_tRR = MIN(saved_timings->max_tRR, dimm_info.tRR);
		saved_timings->cas_mask &= dimm_info.cas_supported;
		for (j = 0; j < 8; j++) {
			if (!(saved_timings->cas_mask & (1 << j)))
				saved_timings->min_tCLK_cas[j] = 0;
			else
				saved_timings->min_tCLK_cas[j] = MAX(dimm_info.cycle_time[j],
						saved_timings->min_tCLK_cas[j]);
		}
		dimm_mask |= (1 << i);
	}
	if (!dimm_mask)
		die("No memory installed.\n");

	if (!(dimm_mask & ((1 << DIMM_SOCKETS) - 1)))
		/* FIXME: Possibly does not boot in this case */
		printk(BIOS_INFO, "Channel 0 has no memory populated.\n");
}

static void choose_tclk(struct sys_info *sysinfo, struct timings *saved_timings)
{
	u32 ctrl_min_tclk;
	int try_cas;

	ctrl_min_tclk = 2 * 256 * 1000 / sdram_capabilities_max_supported_memory_frequency();
	normalize_tck(&ctrl_min_tclk);

	try_cas = spd_get_msbs(saved_timings->cas_mask);

	while (saved_timings->cas_mask & (1 << try_cas) && try_cas > 0) {
		sysinfo->cas = try_cas;
		sysinfo->tclk = saved_timings->min_tCLK_cas[try_cas];
		if (sysinfo->tclk >= ctrl_min_tclk &&
		    saved_timings->min_tCLK_cas[try_cas] !=
		    saved_timings->min_tCLK_cas[try_cas - 1])
			break;
		try_cas--;
	}

	normalize_tck(&sysinfo->tclk);

	if ((sysinfo->cas < 3) || (sysinfo->tclk == 0))
		die("Could not find common memory frequency and CAS\n");

	/*
	 * The loop can still results in a timing too fast for the
	 * memory controller.
	 */
	if (sysinfo->tclk < ctrl_min_tclk)
		sysinfo->tclk = ctrl_min_tclk;

	switch (sysinfo->tclk) {
	case TCK_200MHZ:
		sysinfo->memory_frequency = 400;
		break;
	case TCK_266MHZ:
		sysinfo->memory_frequency = 533;
		break;
	case TCK_333MHZ:
		sysinfo->memory_frequency = 667;
		break;
	}

	printk(BIOS_DEBUG,
		"Memory will be driven at %dMT with CAS=%d clocks\n",
		sysinfo->memory_frequency, sysinfo->cas);
}

static void derive_timings(struct sys_info *sysinfo, struct timings *saved_timings)
{
	sysinfo->tras = DIV_ROUND_UP(saved_timings->min_tRAS, sysinfo->tclk);
	if (sysinfo->tras > 0x18)
		die("DDR-II Module does not support this frequency (tRAS error)\n");

	sysinfo->trp = DIV_ROUND_UP(saved_timings->min_tRP, sysinfo->tclk);
	if (sysinfo->trp > 6)
		die("DDR-II Module does not support this frequency (tRP error)\n");

	sysinfo->trcd = DIV_ROUND_UP(saved_timings->min_tRCD, sysinfo->tclk);
	if (sysinfo->trcd > 6)
		die("DDR-II Module does not support this frequency (tRCD error)\n");

	sysinfo->twr = DIV_ROUND_UP(saved_timings->min_tWR, sysinfo->tclk);
	if (sysinfo->twr > 5)
		die("DDR-II Module does not support this frequency (tWR error)\n");

	sysinfo->trfc = DIV_ROUND_UP(saved_timings->min_tRFC, sysinfo->tclk);

	printk(BIOS_DEBUG, "tRAS = %d cycles\n", sysinfo->tras);
	printk(BIOS_DEBUG, "tRP  = %d cycles\n", sysinfo->trp);
	printk(BIOS_DEBUG, "tRCD = %d cycles\n", sysinfo->trcd);
	printk(BIOS_DEBUG, "tWR  = %d cycles\n", sysinfo->twr);
	printk(BIOS_DEBUG, "tRFC = %d cycles\n", sysinfo->trfc);

	/* Refresh is slower than 15.6us, use 15.6us */
	/* tRR is decoded in units of 1/256us */

#define T_RR_7_8US 2000000
#define T_RR_15_6US 4000000
#define REFRESH_7_8US	1
#define REFRESH_15_6US	0

	if (saved_timings->max_tRR < T_RR_7_8US)
		die("DDR-II module has unsupported refresh value\n");
	else if (saved_timings->max_tRR < T_RR_15_6US)
		sysinfo->refresh = REFRESH_7_8US;
	else
		sysinfo->refresh = REFRESH_15_6US;
	printk(BIOS_DEBUG, "Refresh: %s\n", sysinfo->refresh ? "7.8us" : "15.6us");
}

/**
 * @brief Get generic DIMM parameters.
 * @param sysinfo Central memory controller information structure
 *
 * This function gathers several pieces of information for each system DIMM:
 *  o DIMM width (x8 / x16)
 *  o DIMM rank (single ranked / dual ranked)
 *
 *  Also, some non-supported scenarios are detected.
 */

static void sdram_get_dram_configuration(struct sys_info *sysinfo)
{
	struct timings saved_timings;

	gather_common_timing(sysinfo, &saved_timings);
	choose_tclk(sysinfo, &saved_timings);
	derive_timings(sysinfo, &saved_timings);
}

static void sdram_program_dram_width(struct sys_info *sysinfo)
{
	u16 c0dramw = 0, c1dramw = 0;
	int i, idx;

	if (sysinfo->dual_channel)
		idx = 2;
	else
		idx = 1;

	for (i = 0; i < DIMM_SOCKETS; i++) { /* Channel 0 */
		switch (sysinfo->dimm[i]) {
		case SYSINFO_DIMM_X16DS:
			c0dramw |= (0x0000) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_X8DS:
			c0dramw |= (0x0001) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_X16SS:
			c0dramw |= (0x0000) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_X8DDS:
			c0dramw |= (0x0005) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_NOT_POPULATED:
			c0dramw |= (0x0000) << 4 * (i % 2);
			break;
		}
	}
	for (i = DIMM_SOCKETS; i < idx * DIMM_SOCKETS; i++) { /* Channel 1 */
		switch (sysinfo->dimm[i]) {
		case SYSINFO_DIMM_X16DS:
			c1dramw |= (0x0000) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_X8DS:
			c1dramw |= (0x0010) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_X16SS:
			c1dramw |= (0x0000) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_X8DDS:
			c1dramw |= (0x0050) << 4 * (i % 2);
			break;
		case SYSINFO_DIMM_NOT_POPULATED:
			c1dramw |= (0x0000) << 4 * (i % 2);
			break;
		}
	}

	mchbar_write16(C0DRAMW, c0dramw);
	mchbar_write16(C1DRAMW, c1dramw);
}

static void sdram_write_slew_rates(u32 offset, const u32 *slew_rate_table)
{
	int i;

	for (i = 0; i < 16; i++)
		mchbar_write32(offset + (i * 4), slew_rate_table[i]);
}

static const u32 dq2030[] = {
	0x08070706, 0x0a090908, 0x0d0c0b0a, 0x12100f0e,
	0x1a181614, 0x22201e1c, 0x2a282624, 0x3934302d,
	0x0a090908, 0x0c0b0b0a, 0x0e0d0d0c, 0x1211100f,
	0x19171513, 0x211f1d1b, 0x2d292623, 0x3f393531
};

static const u32 dq2330[] = {
	0x08070706, 0x0a090908, 0x0d0c0b0a, 0x12100f0e,
	0x1a181614, 0x22201e1c, 0x2a282624, 0x3934302d,
	0x0a090908, 0x0c0b0b0a, 0x0e0d0d0c, 0x1211100f,
	0x19171513, 0x211f1d1b, 0x2d292623, 0x3f393531
};

static const u32 cmd2710[] = {
	0x07060605, 0x0f0d0b09, 0x19171411, 0x1f1f1d1b,
	0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f,
	0x1110100f, 0x0f0d0b09, 0x19171411, 0x1f1f1d1b,
	0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f
};

static const u32 cmd3210[] = {
	0x0f0d0b0a, 0x17151311, 0x1f1d1b19, 0x1f1f1f1f,
	0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f,
	0x18171615, 0x1f1f1c1a, 0x1f1f1f1f, 0x1f1f1f1f,
	0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f, 0x1f1f1f1f
};

static const u32 clk2030[] = {
	0x0e0d0d0c, 0x100f0f0e, 0x100f0e0d, 0x15131211,
	0x1d1b1917, 0x2523211f, 0x2a282927, 0x32302e2c,
	0x17161514, 0x1b1a1918, 0x1f1e1d1c, 0x23222120,
	0x27262524, 0x2d2b2928, 0x3533312f, 0x3d3b3937
};

static const u32 ctl3215[] = {
	0x01010000, 0x03020101, 0x07060504, 0x0b0a0908,
	0x100f0e0d, 0x14131211, 0x18171615, 0x1c1b1a19,
	0x05040403, 0x07060605, 0x0a090807, 0x0f0d0c0b,
	0x14131211, 0x18171615, 0x1c1b1a19, 0x201f1e1d
};

static const u32 ctl3220[] = {
	0x05040403, 0x07060505, 0x0e0c0a08, 0x1a171411,
	0x2825221f, 0x35322f2b, 0x3e3e3b38, 0x3e3e3e3e,
	0x09080807, 0x0b0a0a09, 0x0f0d0c0b, 0x1b171311,
	0x2825221f, 0x35322f2b, 0x3e3e3b38, 0x3e3e3e3e
};

static const u32 nc[] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000
};

enum {
	DQ2030,
	DQ2330,
	CMD2710,
	CMD3210,
	CLK2030,
	CTL3215,
	CTL3220,
	NC,
};

static const u8 dual_channel_slew_group_lookup[] = {
	DQ2030, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, NC,      CTL3215, NC,      CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD2710,
	DQ2030, CMD3210, NC,      CTL3215, NC,      CLK2030, NC,     NC,

	DQ2030, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, CTL3215, NC,      CLK2030, NC,      DQ2030, CMD3210,
	DQ2030, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, CTL3215, NC,      CLK2030, NC,      DQ2030, CMD2710,
	DQ2030, CMD3210, CTL3215, NC,      CLK2030, NC,      NC,     NC,

	DQ2030, CMD3210, NC,      CTL3215, NC,      CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, NC,      CTL3215, NC,      CLK2030, DQ2030, CMD3210,
	DQ2030, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD2710,
	DQ2030, CMD3210, NC,      CTL3215, NC,      CLK2030, NC,     NC,

	DQ2030, CMD2710, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD3210,
	DQ2030, CMD2710, CTL3215, NC,      CLK2030, NC,      DQ2030, CMD3210,
	DQ2030, CMD2710, CTL3215, CTL3215, CLK2030, CLK2030, DQ2030, CMD3210,
	DQ2030, CMD2710, CTL3215, NC,      CLK2030, NC,      DQ2030, CMD2710,
	DQ2030, CMD2710, CTL3215, NC,      CLK2030, NC,      NC,     NC,

	NC,     NC,      NC,      CTL3215, NC,      CLK2030, DQ2030, CMD3210,
	NC,     NC,      CTL3215, NC,      CLK2030, NC,      DQ2030, CMD3210,
	NC,     NC,      NC,      CTL3215, NC,      CLK2030, DQ2030, CMD3210,
	NC,     NC,      CTL3215, NC,      CLK2030, CLK2030, DQ2030, CMD2710
};

static const u8 single_channel_slew_group_lookup[] = {
	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, NC,      CTL3215, NC,      CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, NC,      CTL3215, NC,      CLK2030, NC,     NC,

	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, NC,      CLK2030, NC,      DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, NC,      CLK2030, NC,      DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, NC,      CLK2030, NC,      NC,     NC,

	DQ2330, CMD3210, NC,      CTL3215, NC,      CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, NC,      CTL3215, NC,      CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, NC,      CTL3215, NC,      CLK2030, NC,     NC,

	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, NC,      CLK2030, NC,      DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, CTL3215, CLK2030, CLK2030, DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, NC,      CLK2030, NC,      DQ2330, CMD3210,
	DQ2330, CMD3210, CTL3215, NC,      CLK2030, NC,      NC,     NC,

	DQ2330, NC,      NC,      CTL3215, NC,      CLK2030, DQ2030, CMD3210,
	DQ2330, NC,      CTL3215, NC,      CLK2030, NC,      DQ2030, CMD3210,
	DQ2330, NC,      NC,      CTL3215, NC,      CLK2030, DQ2030, CMD3210,
	DQ2330, NC,      CTL3215, NC,      CLK2030, CLK2030, DQ2030, CMD3210
};

static const u32 *slew_group_lookup(int dual_channel, int index)
{
	const u8 *slew_group;
	/* Dual Channel needs different tables. */
	if (dual_channel)
		slew_group = dual_channel_slew_group_lookup;
	else
		slew_group = single_channel_slew_group_lookup;

	switch (slew_group[index]) {
	case DQ2030:	return dq2030;
	case DQ2330:	return dq2330;
	case CMD2710:	return cmd2710;
	case CMD3210:	return cmd3210;
	case CLK2030:	return clk2030;
	case CTL3215:	return ctl3215;
	case CTL3220:	return ctl3220;
	case NC:	return nc;
	}

	return nc;
}

#if CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM)
/* Strength multiplier tables */
static const u8 dual_channel_strength_multiplier[] = {
	0x44, 0x11, 0x11, 0x11, 0x44, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x11, 0x11, 0x44, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x00, 0x11, 0x00, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x11, 0x11, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x11, 0x00, 0x11, 0x00, 0x44, 0x00, 0x00,
	0x44, 0x11, 0x11, 0x11, 0x44, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x11, 0x00, 0x44, 0x00, 0x44, 0x11,
	0x44, 0x11, 0x11, 0x11, 0x44, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x11, 0x00, 0x44, 0x00, 0x44, 0x22,
	0x44, 0x11, 0x11, 0x00, 0x44, 0x00, 0x00, 0x00,
	0x44, 0x11, 0x00, 0x11, 0x00, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x11, 0x11, 0x44, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x00, 0x11, 0x00, 0x44, 0x44, 0x11,
	0x44, 0x11, 0x11, 0x11, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x11, 0x00, 0x11, 0x00, 0x44, 0x00, 0x00,
	0x44, 0x22, 0x11, 0x11, 0x44, 0x44, 0x44, 0x11,
	0x44, 0x22, 0x11, 0x00, 0x44, 0x00, 0x44, 0x11,
	0x44, 0x22, 0x11, 0x11, 0x44, 0x44, 0x44, 0x11,
	0x44, 0x22, 0x11, 0x00, 0x44, 0x00, 0x44, 0x22,
	0x44, 0x22, 0x11, 0x00, 0x44, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x11, 0x00, 0x44, 0x44, 0x11,
	0x00, 0x00, 0x11, 0x00, 0x44, 0x00, 0x44, 0x11,
	0x00, 0x00, 0x00, 0x11, 0x00, 0x44, 0x44, 0x11,
	0x00, 0x00, 0x11, 0x00, 0x44, 0x44, 0x44, 0x22
};

static const u8 single_channel_strength_multiplier[] = {
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x00, 0x11, 0x00, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x00, 0x11, 0x00, 0x44, 0x00, 0x00,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x00, 0x44, 0x00, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x00, 0x44, 0x00, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x00, 0x44, 0x00, 0x00, 0x00,
	0x33, 0x11, 0x00, 0x11, 0x00, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x00, 0x11, 0x00, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x00, 0x11, 0x00, 0x44, 0x00, 0x00,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x00, 0x44, 0x00, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x11, 0x44, 0x44, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x00, 0x44, 0x00, 0x33, 0x11,
	0x33, 0x11, 0x11, 0x00, 0x44, 0x00, 0x00, 0x00,
	0x33, 0x00, 0x00, 0x11, 0x00, 0x44, 0x33, 0x11,
	0x33, 0x00, 0x11, 0x00, 0x44, 0x00, 0x33, 0x11,
	0x33, 0x00, 0x00, 0x11, 0x00, 0x44, 0x33, 0x11,
	0x33, 0x00, 0x11, 0x00, 0x44, 0x44, 0x33, 0x11
};
#elif CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GC)
static const u8 dual_channel_strength_multiplier[] = {
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x33,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x33,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x33,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x33,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x22,
	0x44, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x33
};

static const u8 single_channel_strength_multiplier[] = {
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x44, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x55, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x44, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x55, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x44, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x88, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x44, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x55, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x55, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x88, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x55, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x88, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x22, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00,
	0x44, 0x33, 0x00, 0x00, 0x44, 0x44, 0x44, 0x00
};
#endif

static void sdram_rcomp_buffer_strength_and_slew(struct sys_info *sysinfo)
{
	const u8 *strength_multiplier;
	int idx, dual_channel;

	/* Set Strength Multipliers */

	/* Dual Channel needs different tables. */
	if (sdram_capabilities_dual_channel()) {
		printk(BIOS_DEBUG, "Programming Dual Channel RCOMP\n");
		strength_multiplier = dual_channel_strength_multiplier;
		dual_channel = 1;
		idx = 5 * sysinfo->dimm[0] + sysinfo->dimm[2];
	} else {
		printk(BIOS_DEBUG, "Programming Single Channel RCOMP\n");
		strength_multiplier = single_channel_strength_multiplier;
		dual_channel = 0;
		idx = 5 * sysinfo->dimm[0] + sysinfo->dimm[1];
	}

	printk(BIOS_DEBUG, "Table Index: %d\n", idx);

	mchbar_write8(G1SC, strength_multiplier[idx * 8 + 0]);
	mchbar_write8(G2SC, strength_multiplier[idx * 8 + 1]);
	mchbar_write8(G3SC, strength_multiplier[idx * 8 + 2]);
	mchbar_write8(G4SC, strength_multiplier[idx * 8 + 3]);
	mchbar_write8(G5SC, strength_multiplier[idx * 8 + 4]);
	mchbar_write8(G6SC, strength_multiplier[idx * 8 + 5]);
	mchbar_write8(G7SC, strength_multiplier[idx * 8 + 6]);
	mchbar_write8(G8SC, strength_multiplier[idx * 8 + 7]);

	/* Channel 0 */
	sdram_write_slew_rates(G1SRPUT, slew_group_lookup(dual_channel, idx * 8 + 0));
	sdram_write_slew_rates(G2SRPUT, slew_group_lookup(dual_channel, idx * 8 + 1));
	if ((slew_group_lookup(dual_channel, idx * 8 + 2) != nc) &&
	    (sysinfo->package == SYSINFO_PACKAGE_STACKED))

		sdram_write_slew_rates(G3SRPUT, ctl3220);
	else
		sdram_write_slew_rates(G3SRPUT, slew_group_lookup(dual_channel, idx * 8 + 2));

	sdram_write_slew_rates(G4SRPUT, slew_group_lookup(dual_channel, idx * 8 + 3));
	sdram_write_slew_rates(G5SRPUT, slew_group_lookup(dual_channel, idx * 8 + 4));
	sdram_write_slew_rates(G6SRPUT, slew_group_lookup(dual_channel, idx * 8 + 5));

	/* Channel 1 */
	if (sysinfo->dual_channel) {
		sdram_write_slew_rates(G7SRPUT, slew_group_lookup(dual_channel, idx * 8 + 6));
		sdram_write_slew_rates(G8SRPUT, slew_group_lookup(dual_channel, idx * 8 + 7));
	} else {
		sdram_write_slew_rates(G7SRPUT, nc);
		sdram_write_slew_rates(G8SRPUT, nc);
	}
}

static void sdram_enable_rcomp(void)
{
	u32 reg32;
	/* Enable Global Periodic RCOMP */
	udelay(300);
	reg32 = mchbar_read32(GBRCOMPCTL);
	reg32 &= ~(1 << 23);
	mchbar_write32(GBRCOMPCTL, reg32);
}

static void sdram_program_dll_timings(struct sys_info *sysinfo)
{
	u32 channeldll = 0;
	int i;

	printk(BIOS_DEBUG, "Programming DLL Timings...\n");

	mchbar_clrbits16(DQSMT, 3 << 12 | 1 << 10 | 0xf << 0);
	mchbar_setbits16(DQSMT, 1 << 13 | 0xc << 0);

	/* We drive both channels with the same speed */
	if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM)) {
		switch (sysinfo->memory_frequency) {
		case 400:
			channeldll = 0x26262626;
			break;
		case 533:
			channeldll = 0x22222222;
			break;
		case 667:
			channeldll = 0x11111111;
			break;
		}
	} else if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GC)) {
		switch (sysinfo->memory_frequency) {
		case 400:
			channeldll = 0x33333333;
			break;
		case 533:
			channeldll = 0x24242424;
			break;
		case 667:
			channeldll = 0x25252525;
			break;
		}
	}

	for (i = 0; i < 4; i++) {
		mchbar_write32(C0R0B00DQST + (i * 0x10) + 0, channeldll);
		mchbar_write32(C0R0B00DQST + (i * 0x10) + 4, channeldll);
		mchbar_write32(C1R0B00DQST + (i * 0x10) + 0, channeldll);
		mchbar_write32(C1R0B00DQST + (i * 0x10) + 4, channeldll);
		if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GC)) {
			mchbar_write8(C0R0B00DQST + (i * 0x10) + 8, channeldll & 0xff);
			mchbar_write8(C1R0B00DQST + (i * 0x10) + 8, channeldll & 0xff);
		}
	}
}

static void sdram_force_rcomp(void)
{
	u32 reg32;
	u8 reg8;

	reg32 = mchbar_read32(ODTC);
	reg32 |= (1 << 28);
	mchbar_write32(ODTC, reg32);

	reg32 = mchbar_read32(SMSRCTL);
	reg32 |= (1 << 0);
	mchbar_write32(SMSRCTL, reg32);

	/* Start initial RCOMP */
	reg32 = mchbar_read32(GBRCOMPCTL);
	reg32 |= (1 << 8);
	mchbar_write32(GBRCOMPCTL, reg32);

	reg8 = i945_silicon_revision();
	if ((reg8 == 0 && (mchbar_read32(DCC) & (3 << 0)) == 0) || (reg8 == 1)) {
		reg32 = mchbar_read32(GBRCOMPCTL);
		reg32 |= (3 << 5);
		mchbar_write32(GBRCOMPCTL, reg32);
	}
}

static void sdram_initialize_system_memory_io(struct sys_info *sysinfo)
{
	u8 reg8;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing System Memory IO...\n");
	/* Enable Data Half Clock Pushout */
	reg8 = mchbar_read8(C0HCTC);
	reg8 &= ~0x1f;
	reg8 |= (1 << 0);
	mchbar_write8(C0HCTC, reg8);

	reg8 = mchbar_read8(C1HCTC);
	reg8 &= ~0x1f;
	reg8 |= (1 << 0);
	mchbar_write8(C1HCTC, reg8);

	mchbar_clrbits16(WDLLBYPMODE, 1 << 9 | 1 << 6 | 1 << 4 | 1 << 3 | 1 << 1);
	mchbar_setbits16(WDLLBYPMODE, 1 << 8 | 1 << 7 | 1 << 5 | 1 << 2 | 1 << 0);

	mchbar_write8(C0WDLLCMC, 0);
	mchbar_write8(C1WDLLCMC, 0);

	/* Program RCOMP Settings */
	sdram_program_dram_width(sysinfo);

	sdram_rcomp_buffer_strength_and_slew(sysinfo);

	/* Indicate that RCOMP programming is done */
	reg32 = mchbar_read32(GBRCOMPCTL);
	reg32 &= ~((1 << 29) | (1 << 26) | (3 << 21) | (3 << 2));
	reg32 |= (3 << 27) | (3 << 0);
	mchbar_write32(GBRCOMPCTL, reg32);

	mchbar_setbits32(GBRCOMPCTL, 1 << 10);

	/* Program DLL Timings */
	sdram_program_dll_timings(sysinfo);

	/* Force RCOMP cycle */
	sdram_force_rcomp();
}

static void sdram_enable_system_memory_io(struct sys_info *sysinfo)
{
	u32 reg32;

	printk(BIOS_DEBUG, "Enabling System Memory IO...\n");

	reg32 = mchbar_read32(RCVENMT);
	reg32 &= ~(0x3f << 6);
	mchbar_write32(RCVENMT, reg32); /* [11:6] = 0 */

	reg32 |= (1 << 11) | (1 << 9);
	mchbar_write32(RCVENMT, reg32);

	reg32 = mchbar_read32(DRTST);
	reg32 |= (1 << 3) | (1 << 2);
	mchbar_write32(DRTST, reg32);

	reg32 = mchbar_read32(DRTST);
	reg32 |= (1 << 6) | (1 << 4);
	mchbar_write32(DRTST, reg32);

	asm volatile ("nop; nop;" ::: "memory");

	reg32 = mchbar_read32(DRTST);

	/* Is channel 0 populated? */
	if (sysinfo->dimm[0] != SYSINFO_DIMM_NOT_POPULATED ||
	    sysinfo->dimm[1] != SYSINFO_DIMM_NOT_POPULATED)
		reg32 |= (1 << 7) | (1 << 5);
	else
		reg32 |= (1 << 31);

	/* Is channel 1 populated? */
	if (sysinfo->dimm[2] != SYSINFO_DIMM_NOT_POPULATED ||
	    sysinfo->dimm[3] != SYSINFO_DIMM_NOT_POPULATED)
		reg32 |= (1 << 9) | (1 << 8);
	else
		reg32 |= (1 << 30);

	mchbar_write32(DRTST, reg32);

	/* Activate DRAM Channel IO Buffers */
	if (sysinfo->dimm[0] != SYSINFO_DIMM_NOT_POPULATED ||
	    sysinfo->dimm[1] != SYSINFO_DIMM_NOT_POPULATED) {
		reg32 = mchbar_read32(C0DRC1);
		reg32 |= (1 << 8);
		mchbar_write32(C0DRC1, reg32);
	}
	if (sysinfo->dimm[2] != SYSINFO_DIMM_NOT_POPULATED ||
	    sysinfo->dimm[3] != SYSINFO_DIMM_NOT_POPULATED) {
		reg32 = mchbar_read32(C1DRC1);
		reg32 |= (1 << 8);
		mchbar_write32(C1DRC1, reg32);
	}
}

static int sdram_program_row_boundaries(struct sys_info *sysinfo)
{
	int i;
	int cum0, cum1, tolud, tom, pci_mmio_size;
	const struct device *dev;
	const struct northbridge_intel_i945_config *cfg = NULL;

	printk(BIOS_DEBUG, "Setting RAM size...\n");

	cum0 = 0;
	for (i = 0; i < 2 * DIMM_SOCKETS; i++) {
		cum0 += sysinfo->banksize[i];
		mchbar_write8(C0DRB0 + i, cum0);
	}

	/* Assume we continue in Channel 1 where we stopped in Channel 0 */
	cum1 = cum0;

	/* Exception: Interleaved starts from the beginning */
	if (sysinfo->interleaved)
		cum1 = 0;

	for (i = 0; i < 2 * DIMM_SOCKETS; i++) {
		cum1 += sysinfo->banksize[i + 4];
		mchbar_write8(C1DRB0 + i, cum1);
	}

	/* Set TOLUD Top Of Low Usable DRAM */
	if (sysinfo->interleaved)
		tolud = (cum0 + cum1) << 1;
	else
		tolud = (cum1 ? cum1 : cum0) << 1;

	/* The TOM register has a different format */
	tom = tolud >> 3;

	/* Limit the value of TOLUD to leave some space for PCI memory. */
	dev = pcidev_on_root(0, 0);
	if (dev)
		cfg = dev->chip_info;

	/* Don't use pci mmio sizes smaller than 768M */
	if (!cfg || cfg->pci_mmio_size <= DEFAULT_PCI_MMIO_SIZE)
		pci_mmio_size = DEFAULT_PCI_MMIO_SIZE;
	else
		pci_mmio_size = cfg->pci_mmio_size;

	tolud = MIN(((4096 - pci_mmio_size) / 128) << 3, tolud);

	pci_write_config8(HOST_BRIDGE, TOLUD, tolud);

	printk(BIOS_DEBUG, "C0DRB = 0x%08x\n", mchbar_read32(C0DRB0));
	printk(BIOS_DEBUG, "C1DRB = 0x%08x\n", mchbar_read32(C1DRB0));
	printk(BIOS_DEBUG, "TOLUD = 0x%04x\n", pci_read_config8(HOST_BRIDGE, TOLUD));

	pci_write_config16(HOST_BRIDGE, TOM, tom);

	return 0;
}

static int sdram_set_row_attributes(struct sys_info *sysinfo)
{
	int i;
	u16 dra0 = 0, dra1 = 0, dra = 0;

	printk(BIOS_DEBUG, "Setting row attributes...\n");
	for (i = 0; i < 2 * DIMM_SOCKETS; i++) {
		u8 columnsrows;

		if (sysinfo->dimm[i] == SYSINFO_DIMM_NOT_POPULATED)
			continue;

		columnsrows = (sysinfo->rows[i] & 0x0f) | (sysinfo->cols[i] & 0xf) << 4;

		switch (columnsrows) {
		case 0x9d:
			dra = 2;
			break;
		case 0xad:
			dra = 3;
			break;
		case 0xbd:
			dra = 4;
			break;
		case 0xae:
			dra = 3;
			break;
		case 0xbe:
			dra = 4;
			break;
		default:
			die("Unsupported Rows/Columns. (DRA)");
		}

		/* Double Sided DIMMs? */
		if (sysinfo->banksize[(2 * i) + 1] != 0)
			dra = (dra << 4) | dra;

		if (i < DIMM_SOCKETS)
			dra0 |= (dra << (i * 8));
		else
			dra1 |= (dra << ((i - DIMM_SOCKETS) * 8));
	}

	mchbar_write16(C0DRA0, dra0);
	mchbar_write16(C1DRA0, dra1);

	printk(BIOS_DEBUG, "C0DRA = 0x%04x\n", dra0);
	printk(BIOS_DEBUG, "C1DRA = 0x%04x\n", dra1);

	return 0;
}

static void sdram_set_bank_architecture(struct sys_info *sysinfo)
{
	u32 off32;
	int i;

	mchbar_clrbits16(C1BNKARC, 0xff);
	mchbar_clrbits16(C0BNKARC, 0xff);

	off32 = C0BNKARC;
	for (i = 0; i < 2 * DIMM_SOCKETS; i++) {
		/* Switch to second channel */
		if (i == DIMM_SOCKETS)
			off32 = C1BNKARC;

		if (sysinfo->dimm[i] == SYSINFO_DIMM_NOT_POPULATED)
			continue;

		if (sysinfo->banks[i] != 8)
			continue;

		printk(BIOS_SPEW, "DIMM%d has 8 banks.\n", i);

		if (i & 1)
			mchbar_setbits16(off32, 5 << 4);
		else
			mchbar_setbits16(off32, 5 << 0);
	}
}

static void sdram_program_refresh_rate(struct sys_info *sysinfo)
{
	u32 reg32;

	if (sysinfo->refresh == REFRESH_7_8US)
		reg32 = (2 << 8); /* Refresh enabled at 7.8us */
	else
		reg32 = (1 << 8); /* Refresh enabled at 15.6us */

	mchbar_clrbits32(C0DRC0, 7 << 8);
	mchbar_setbits32(C0DRC0, reg32);

	mchbar_clrbits32(C1DRC0, 7 << 8);
	mchbar_setbits32(C1DRC0, reg32);
}

static void sdram_program_cke_tristate(struct sys_info *sysinfo)
{
	u32 reg32;
	int i;

	reg32 = mchbar_read32(C0DRC1);

	for (i = 0; i < 4; i++) {
		if (sysinfo->banksize[i] == 0)
			reg32 |= (1 << (16 + i));
	}

	reg32 |= (1 << 12);

	reg32 |= (1 << 11);
	mchbar_write32(C0DRC1, reg32);

	/* Do we have to do this if we're in Single Channel Mode? */
	reg32 = mchbar_read32(C1DRC1);

	for (i = 4; i < 8; i++) {
		if (sysinfo->banksize[i] == 0)
			reg32 |= (1 << (12 + i));
	}

	reg32 |= (1 << 12);

	reg32 |= (1 << 11);
	mchbar_write32(C1DRC1, reg32);
}

static void sdram_program_odt_tristate(struct sys_info *sysinfo)
{
	u32 reg32;
	int i;

	reg32 = mchbar_read32(C0DRC2);

	for (i = 0; i < 4; i++) {
		if (sysinfo->banksize[i] == 0)
			reg32 |= (1 << (24 + i));
	}
	mchbar_write32(C0DRC2, reg32);

	reg32 = mchbar_read32(C1DRC2);

	for (i = 4; i < 8; i++) {
		if (sysinfo->banksize[i] == 0)
			reg32 |= (1 << (20 + i));
	}
	mchbar_write32(C1DRC2, reg32);
}

static void sdram_set_timing_and_control(struct sys_info *sysinfo)
{
	u32 reg32, tRD_min;
	u32 tWTR;
	u32 temp_drt;
	int i, page_size;

	static const u8 cas_table[] = {
		2, 1, 0, 3
	};

	reg32 = mchbar_read32(C0DRC0);
	reg32 |= (1 << 2);	/* Burst Length 8 */
	reg32 &= ~((1 << 13) | (1 << 12));
	mchbar_write32(C0DRC0, reg32);

	reg32 = mchbar_read32(C1DRC0);
	reg32 |= (1 << 2);	/* Burst Length 8 */
	reg32 &= ~((1 << 13) | (1 << 12));
	mchbar_write32(C1DRC0, reg32);

	if (!sysinfo->dual_channel && sysinfo->dimm[1] != SYSINFO_DIMM_NOT_POPULATED) {
		reg32 = mchbar_read32(C0DRC0);
		reg32 |= (1 << 15);
		mchbar_write32(C0DRC0, reg32);
	}

	sdram_program_refresh_rate(sysinfo);

	sdram_program_cke_tristate(sysinfo);

	sdram_program_odt_tristate(sysinfo);

	/* Calculate DRT0 */

	temp_drt = 0;

	/* B2B Write Precharge (same bank) = CL-1 + BL/2 + tWR */
	reg32 = (sysinfo->cas - 1) + (BURSTLENGTH / 2) + sysinfo->twr;
	temp_drt |= (reg32 << 28);

	/* Write Auto Precharge (same bank) = CL-1 + BL/2 + tWR + tRP */
	reg32 += sysinfo->trp;
	temp_drt |= (reg32 << 4);

	if (sysinfo->memory_frequency == 667)
		tWTR = 3; /* 667MHz */
	else
		tWTR = 2; /* 400 and 533 */

	/* B2B Write to Read Command Spacing */
	reg32 = (sysinfo->cas - 1) + (BURSTLENGTH / 2) + tWTR;
	temp_drt |= (reg32 << 24);

	/* CxDRT0 [23:22], [21:20], [19:18] [16] have fixed values */
	temp_drt |= ((1 << 22) | (3 << 20) | (1 << 18) | (0 << 16));

	/*
	 * tRD is the delay the memory controller is waiting on the FSB,
	 * in mclk domain.
	 * This parameter is important for stability and performance.
	 * Those values might not be optimal but seem stable.
	 */
	tRD_min = sysinfo->cas;
	switch (sysinfo->fsb_frequency) {
	case 533:
		break;
	case 667:
		tRD_min += 1;
		break;
	case 800:
		tRD_min += 2;
		break;
	case 1066:
		tRD_min += 3;
		break;
	}

	temp_drt |= (tRD_min << 11);

	/* Read Auto Precharge to Activate */

	temp_drt |= (8 << 0);

	mchbar_write32(C0DRT0, temp_drt);
	mchbar_write32(C1DRT0, temp_drt);

	/* Calculate DRT1 */

	temp_drt = mchbar_read32(C0DRT1) & 0x00020088;

	/* DRAM RASB Precharge */
	temp_drt |= (sysinfo->trp - 2) << 0;

	/* DRAM RASB to CASB Delay */
	temp_drt |= (sysinfo->trcd - 2) << 4;

	/* CASB Latency */
	temp_drt |= (cas_table[sysinfo->cas - 3]) << 8;

	/* Refresh Cycle Time */
	temp_drt |= (sysinfo->trfc) << 10;

	/* Pre-All to Activate Delay */
	temp_drt |= (0 << 16);

	/* Precharge to Precharge Delay stays at 1 clock */
	temp_drt |= (0 << 18);

	/* Activate to Precharge Delay */
	temp_drt |= (sysinfo->tras << 19);

	/* Read to Precharge (tRTP) */
	if (sysinfo->memory_frequency == 667)
		temp_drt |= (1 << 28);
	else
		temp_drt |= (0 << 28);

	/* Determine page size */
	reg32 = 0;
	page_size = 1; /* Default: 1k pagesize */
	for (i = 0; i < 2*DIMM_SOCKETS; i++) {
		if (sysinfo->dimm[i] == SYSINFO_DIMM_X16DS ||
		    sysinfo->dimm[i] == SYSINFO_DIMM_X16SS)
			page_size = 2; /* 2k pagesize */
	}

	if (sysinfo->memory_frequency == 533 && page_size == 2)
		reg32 = 1;
	if (sysinfo->memory_frequency == 667)
		reg32 = page_size;

	temp_drt |= (reg32 << 30);

	mchbar_write32(C0DRT1, temp_drt);
	mchbar_write32(C1DRT1, temp_drt);

	/* Program DRT2 */
	reg32 = mchbar_read32(C0DRT2);
	reg32 &= ~(1 << 8);
	mchbar_write32(C0DRT2, reg32);

	reg32 = mchbar_read32(C1DRT2);
	reg32 &= ~(1 << 8);
	mchbar_write32(C1DRT2, reg32);

	/* Calculate DRT3 */
	temp_drt = mchbar_read32(C0DRT3) & ~0x07ffffff;

	/* Get old tRFC value */
	reg32 = mchbar_read32(C0DRT1) >> 10;
	reg32 &= 0x3f;

	/* 788nS - tRFC */
	switch (sysinfo->memory_frequency) {
	case 400: /* 5nS */
		reg32 = ((78800 / 500) - reg32) & 0x1ff;
		reg32 |= (0x8c << 16) | (0x0c << 10); /* 1 us */
		break;
	case 533: /* 3.75nS */
		reg32 = ((78800 / 375) - reg32) & 0x1ff;
		reg32 |= (0xba << 16) | (0x10 << 10); /* 1 us */
		break;
	case 667: /* 3nS */
		reg32 = ((78800 / 300) - reg32) & 0x1ff;
		reg32 |= (0xe9 << 16) | (0x14 << 10); /* 1 us */
		break;
	}

	temp_drt |= reg32;

	mchbar_write32(C0DRT3, temp_drt);
	mchbar_write32(C1DRT3, temp_drt);
}

static void sdram_set_channel_mode(struct sys_info *sysinfo)
{
	u32 reg32;

	printk(BIOS_DEBUG, "Setting mode of operation for memory channels...");

	if (sdram_capabilities_interleave() &&
	    ((sysinfo->banksize[0] + sysinfo->banksize[1] +
	      sysinfo->banksize[2] + sysinfo->banksize[3]) ==
	     (sysinfo->banksize[4] + sysinfo->banksize[5] +
	      sysinfo->banksize[6] + sysinfo->banksize[7]))) {
		/* Both channels equipped with DIMMs of the same size */
		sysinfo->interleaved = 1;
	} else {
		sysinfo->interleaved = 0;
	}

	reg32 = mchbar_read32(DCC);
	reg32 &= ~(7 << 0);

	if (sysinfo->interleaved) {
		/* Dual Channel Interleaved */
		printk(BIOS_DEBUG, "Dual Channel Interleaved.\n");
		reg32 |= (1 << 1);
	} else if (sysinfo->dimm[0] == SYSINFO_DIMM_NOT_POPULATED &&
		   sysinfo->dimm[1] == SYSINFO_DIMM_NOT_POPULATED) {
		/* Channel 1 only */
		printk(BIOS_DEBUG, "Single Channel 1 only.\n");
		reg32 |= (1 << 2);
	} else if (sdram_capabilities_dual_channel() &&
		   (sysinfo->dimm[2] != SYSINFO_DIMM_NOT_POPULATED ||
		    sysinfo->dimm[3] != SYSINFO_DIMM_NOT_POPULATED)) {
		/* Dual Channel Asymmetric */
		printk(BIOS_DEBUG, "Dual Channel Asymmetric.\n");
		reg32 |= (1 << 0);
	} else {
		/* All bits 0 means Single Channel 0 operation */
		printk(BIOS_DEBUG, "Single Channel 0 only.\n");
	}

	/* Now disable channel XORing */
	reg32 |= (1 << 10);

	mchbar_write32(DCC, reg32);

	PRINTK_DEBUG("DCC = 0x%08x\n", mchbar_read32(DCC));
}

static void sdram_program_pll_settings(struct sys_info *sysinfo)
{
	mchbar_write32(PLLMON, 0x80800000);

	sysinfo->fsb_frequency = fsbclk();
	if (sysinfo->fsb_frequency == 0xffff)
		die("Unsupported FSB speed");

	/* Program CPCTL according to FSB speed */
	/* Only write the lower byte */
	switch (sysinfo->fsb_frequency) {
	case 400:
		mchbar_write8(CPCTL, 0x90);
		break;
	case 533:
		mchbar_write8(CPCTL, 0x95);
		break;
	case 667:
		mchbar_write8(CPCTL, 0x8d);
		break;
	}

	mchbar_clrbits16(CPCTL, 1 << 11);

	mchbar_read16(CPCTL); /* Read back register to activate settings */
}

static void sdram_program_graphics_frequency(struct sys_info *sysinfo)
{
	u8 reg8;
	u8 freq, second_vco, voltage;

#define CRCLK_166MHz	0x00
#define CRCLK_200MHz	0x01
#define CRCLK_250MHz	0x03
#define CRCLK_400MHz	0x05

#define CDCLK_200MHz	0x00
#define CDCLK_320MHz	0x40

#define VOLTAGE_1_05	0x00
#define VOLTAGE_1_50	0x01

	printk(BIOS_DEBUG, "Setting Graphics Frequency...\n");

	printk(BIOS_DEBUG, "FSB: %d MHz ", sysinfo->fsb_frequency);

	voltage = VOLTAGE_1_05;
	if (mchbar_read32(DFT_STRAP1) & (1 << 20))
		voltage = VOLTAGE_1_50;
	printk(BIOS_DEBUG, "Voltage: %s ", (voltage == VOLTAGE_1_05) ? "1.05V" : "1.5V");

	/* Gate graphics hardware for frequency change */
	reg8 = (1 << 3) | (1 << 1); /* disable crclk, gate cdclk */
	pci_write_config8(IGD_DEV, GCFC + 1, reg8);

	/* Get graphics frequency capabilities */
	reg8 = sdram_capabilities_core_frequencies();

	freq = CRCLK_250MHz;
	switch (reg8) {
	case GFX_FREQUENCY_CAP_ALL:
		if (voltage == VOLTAGE_1_05)
			freq = CRCLK_250MHz;
		else
			freq = CRCLK_400MHz; /* 1.5V requires 400MHz */
		break;
	case GFX_FREQUENCY_CAP_250MHZ:
		freq = CRCLK_250MHz;
		break;
	case GFX_FREQUENCY_CAP_200MHZ:
		freq = CRCLK_200MHz;
		break;
	case GFX_FREQUENCY_CAP_166MHZ:
		freq = CRCLK_166MHz;
		break;
	}

	if (freq != CRCLK_400MHz) {
		/* What chipset are we? Force 166MHz for GMS */
		reg8 = (pci_read_config8(HOST_BRIDGE, 0xe7) & 0x70) >> 4;
		if (reg8 == 2)
			freq = CRCLK_166MHz;
	}

	printk(BIOS_DEBUG, "Render: ");
	switch (freq) {
	case CRCLK_166MHz:
		printk(BIOS_DEBUG, "166MHz");
		break;
	case CRCLK_200MHz:
		printk(BIOS_DEBUG, "200MHz");
		break;
	case CRCLK_250MHz:
		printk(BIOS_DEBUG, "250MHz");
		break;
	case CRCLK_400MHz:
		printk(BIOS_DEBUG, "400MHz");
		break;
	}

	if (i945_silicon_revision() == 0)
		sysinfo->mvco4x = 1;
	else
		sysinfo->mvco4x = 0;

	second_vco = 0;

	if (voltage == VOLTAGE_1_50) {
		second_vco = 1;
	} else if ((i945_silicon_revision() > 0) && (freq == CRCLK_250MHz)) {
		u16 mem = sysinfo->memory_frequency;
		u16 fsb = sysinfo->fsb_frequency;

		if ((fsb == 667 && mem == 533) ||
		    (fsb == 533 && mem == 533) ||
		    (fsb == 533 && mem == 400)) {
			second_vco = 1;
		}

		if (fsb == 667 && mem == 533)
			sysinfo->mvco4x = 1;
	}

	if (second_vco)
		sysinfo->clkcfg_bit7 = 1;
	else
		sysinfo->clkcfg_bit7 = 0;

	/* Graphics Core Render Clock */
	pci_update_config16(IGD_DEV, GCFC, ~((7 << 0) | (1 << 13)), freq);

	/* Graphics Core Display Clock */
	reg8 = pci_read_config8(IGD_DEV, GCFC);
	reg8 &= ~((1 << 7) | (7 << 4));

	if (voltage == VOLTAGE_1_05) {
		reg8 |= CDCLK_200MHz;
		printk(BIOS_DEBUG, " Display: 200MHz\n");
	} else {
		reg8 |= CDCLK_320MHz;
		printk(BIOS_DEBUG, " Display: 320MHz\n");
	}
	pci_write_config8(IGD_DEV, GCFC, reg8);

	reg8 = pci_read_config8(IGD_DEV, GCFC + 1);

	reg8 |= (1 << 3) | (1 << 1);
	pci_write_config8(IGD_DEV, GCFC + 1, reg8);

	reg8 |= 0x0f;
	pci_write_config8(IGD_DEV, GCFC + 1, reg8);

	/* Ungate core render and display clocks */
	reg8 &= 0xf0;
	pci_write_config8(IGD_DEV, GCFC + 1, reg8);
}

static void sdram_program_memory_frequency(struct sys_info *sysinfo)
{
	u32 clkcfg;
	u8 offset = CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM) ? 1 : 0;

	printk(BIOS_DEBUG, "Setting Memory Frequency... ");

	clkcfg = mchbar_read32(CLKCFG);

	printk(BIOS_DEBUG, "CLKCFG = 0x%08x, ", clkcfg);

	clkcfg &= ~((1 << 12) | (1 << 7) | (7 << 4));

	if (sysinfo->mvco4x) {
		printk(BIOS_DEBUG, "MVCO 4x, ");
		clkcfg &= ~(1 << 12);
	}

	if (sysinfo->clkcfg_bit7) {
		printk(BIOS_DEBUG, "second VCO, ");
		clkcfg |= (1 << 7);
	}

	switch (sysinfo->memory_frequency) {
	case 400:
		clkcfg |= ((1 + offset) << 4);
		break;
	case 533:
		clkcfg |= ((2 + offset) << 4);
		break;
	case 667:
		clkcfg |= ((3 + offset) << 4);
		break;
	default:
		die("Target Memory Frequency Error");
	}

	if (mchbar_read32(CLKCFG) == clkcfg) {
		printk(BIOS_DEBUG, "ok (unchanged)\n");
		return;
	}

	mchbar_write32(CLKCFG, clkcfg);

	/*
	 * Make sure the following code is in the cache before we execute it.
	 * TODO: Experiments (i945GM) without any cache_code/delay_update
	 * _seem_ to work even when XIP is disabled. Also on Pentium 4
	 * the code is not cached at all by default.
	 */
	asm volatile (
		"	jmp cache_code\n"
		"vco_update:\n"
		: /* No outputs */
		: /* No inputs */
		: "memory"
	);

	pci_and_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_2, (u8)~(1 << 7));

	clkcfg &= ~(1 << 10);
	mchbar_write32(CLKCFG, clkcfg);
	clkcfg |= (1 << 10);
	mchbar_write32(CLKCFG, clkcfg);

	asm volatile (
		"	movl $0x100, %%ecx\n"
		"delay_update:\n"
		"	nop\n"
		"	nop\n"
		"	nop\n"
		"	nop\n"
		"	loop delay_update\n"
		: /* No outputs */
		: /* No inputs */
		: "%ecx", "memory"
		);

	clkcfg &= ~(1 << 10);
	mchbar_write32(CLKCFG, clkcfg);

	asm volatile (
		"	jmp out\n"
		"cache_code:\n"
		"	jmp vco_update\n"
		"out:\n"
		: /* No outputs */
		: /* No inputs */
		: "memory"
	);

	printk(BIOS_DEBUG, "CLKCFG = 0x%08x, ", mchbar_read32(CLKCFG));
	printk(BIOS_DEBUG, "ok\n");
}

static void sdram_program_clock_crossing(void)
{
	int idx = 0;

	/**
	 * We add the indices according to our clocks from CLKCFG.
	 */
#if CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM)
	static const u32 data_clock_crossing[] = {
		0x00100401, 0x00000000, /* DDR400 FSB400 */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0x08040120, 0x00000000,	/* DDR400 FSB533 */
		0x00100401, 0x00000000, /* DDR533 FSB533 */
		0x00010402, 0x00000000, /* DDR667 FSB533 - fake values */

		0x04020120, 0x00000010,	/* DDR400 FSB667 */
		0x10040280, 0x00000040, /* DDR533 FSB667 */
		0x00100401, 0x00000000, /* DDR667 FSB667 */

		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
	};

	static const u32 command_clock_crossing[] = {
		0x04020208, 0x00000000, /* DDR400 FSB400 */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0x00060108, 0x00000000,	/* DDR400 FSB533 */
		0x04020108, 0x00000000, /* DDR533 FSB533 */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0x00040318, 0x00000000,	/* DDR400 FSB667 */
		0x04020118, 0x00000000, /* DDR533 FSB667 */
		0x02010804, 0x00000000, /* DDR667 FSB667 */

		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
	};

#elif CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GC)
	/* i945 G/P */
	static const u32 data_clock_crossing[] = {
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0x10080201, 0x00000000,	/* DDR400 FSB533 */
		0x00100401, 0x00000000, /* DDR533 FSB533 */
		0x00010402, 0x00000000, /* DDR667 FSB533 - fake values */

		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0x04020108, 0x00000000, /* DDR400 FSB800 */
		0x00020108, 0x00000000, /* DDR533 FSB800 */
		0x00080201, 0x00000000, /* DDR667 FSB800 */

		0x00010402, 0x00000000, /* DDR400 FSB1066 */
		0x04020108, 0x00000000, /* DDR533 FSB1066 */
		0x08040110, 0x00000000, /* DDR667 FSB1066 */
	};

	static const u32 command_clock_crossing[] = {
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0x00010800, 0x00000402,	/* DDR400 FSB533 */
		0x01000400, 0x00000200, /* DDR533 FSB533 */
		0x00020904, 0x00000000, /* DDR667 FSB533 - fake values */

		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */
		0xffffffff, 0xffffffff, /*  nonexistent  */

		0x02010804, 0x00000000, /* DDR400 FSB800 */
		0x00010402, 0x00000000, /* DDR533 FSB800 */
		0x04020130, 0x00000008, /* DDR667 FSB800 */

		0x00020904, 0x00000000, /* DDR400 FSB1066 */
		0x02010804, 0x00000000, /* DDR533 FSB1066 */
		0x180601c0, 0x00000020, /* DDR667 FSB1066 */
	};
#endif

	printk(BIOS_DEBUG, "Programming Clock Crossing...");

	printk(BIOS_DEBUG, "MEM=");
	switch (memclk()) {
	case 400:
		printk(BIOS_DEBUG, "400");
		idx += 0;
		break;
	case 533:
		printk(BIOS_DEBUG, "533");
		idx += 2;
		break;
	case 667:
		printk(BIOS_DEBUG, "667");
		idx += 4;
		break;
	default:
		printk(BIOS_DEBUG, "RSVD %x", memclk());
		return;
	}

	printk(BIOS_DEBUG, " FSB=");
	switch (fsbclk()) {
	case 400:
		printk(BIOS_DEBUG, "400");
		idx += 0;
		break;
	case 533:
		printk(BIOS_DEBUG, "533");
		idx += 6;
		break;
	case 667:
		printk(BIOS_DEBUG, "667");
		idx += 12;
		break;
	case 800:
		printk(BIOS_DEBUG, "800");
		idx += 18;
		break;
	case 1066:
		printk(BIOS_DEBUG, "1066");
		idx += 24;
		break;
	default:
		printk(BIOS_DEBUG, "RSVD %x\n", fsbclk());
		return;
	}

	if (command_clock_crossing[idx] == 0xffffffff)
		printk(BIOS_DEBUG, "Invalid MEM/FSB combination!\n");

	mchbar_write32(CCCFT + 0, command_clock_crossing[idx]);
	mchbar_write32(CCCFT + 4, command_clock_crossing[idx + 1]);

	mchbar_write32(C0DCCFT + 0, data_clock_crossing[idx]);
	mchbar_write32(C0DCCFT + 4, data_clock_crossing[idx + 1]);
	mchbar_write32(C1DCCFT + 0, data_clock_crossing[idx]);
	mchbar_write32(C1DCCFT + 4, data_clock_crossing[idx + 1]);

	printk(BIOS_DEBUG, "... ok\n");
}

static void sdram_disable_fast_dispatch(void)
{
	u32 reg32;

	reg32 = mchbar_read32(FSBPMC3);
	reg32 |= (1 << 1);
	mchbar_write32(FSBPMC3, reg32);

	reg32 = mchbar_read32(SBTEST);
	reg32 |= (3 << 1);
	mchbar_write32(SBTEST, reg32);
}

static void sdram_pre_jedec_initialization(void)
{
	u32 reg32;

	reg32 = mchbar_read32(WCC);
	reg32 &= 0x113ff3ff;
	reg32 |= (4 << 29) | (3 << 25) | (1 << 10);
	mchbar_write32(WCC, reg32);

	mchbar_setbits32(SMVREFC, 1 << 6);

	mchbar_clrbits32(MMARB0, 3 << 17);
	mchbar_setbits32(MMARB0, 1 << 21 | 1 << 16);

	mchbar_clrbits32(MMARB1, 7 << 8);
	mchbar_setbits32(MMARB1, 3 << 8);

	/* Adaptive Idle Timer Control */
	mchbar_write32(C0AIT + 0, 0x000006c4);
	mchbar_write32(C0AIT + 4, 0x871a066d);

	mchbar_write32(C1AIT + 0, 0x000006c4);
	mchbar_write32(C1AIT + 4, 0x871a066d);
}

#define EA_DUALCHANNEL_XOR_BANK_RANK_MODE	(0xd4 << 24)
#define EA_DUALCHANNEL_XOR_BANK_MODE		(0xf4 << 24)
#define EA_DUALCHANNEL_BANK_RANK_MODE		(0xc2 << 24)
#define EA_DUALCHANNEL_BANK_MODE		(0xe2 << 24)
#define EA_SINGLECHANNEL_XOR_BANK_RANK_MODE	(0x91 << 24)
#define EA_SINGLECHANNEL_XOR_BANK_MODE		(0xb1 << 24)
#define EA_SINGLECHANNEL_BANK_RANK_MODE		(0x80 << 24)
#define EA_SINGLECHANNEL_BANK_MODE		(0xa0 << 24)

static void sdram_enhanced_addressing_mode(struct sys_info *sysinfo)
{
	u32 chan0 = 0, chan1 = 0;
	bool chan0_dualsided, chan1_dualsided, chan0_populated, chan1_populated;

	chan0_populated = (sysinfo->dimm[0] != SYSINFO_DIMM_NOT_POPULATED ||
			   sysinfo->dimm[1] != SYSINFO_DIMM_NOT_POPULATED);
	chan1_populated = (sysinfo->dimm[2] != SYSINFO_DIMM_NOT_POPULATED ||
			   sysinfo->dimm[3] != SYSINFO_DIMM_NOT_POPULATED);
	chan0_dualsided = (sysinfo->banksize[1] || sysinfo->banksize[3]);
	chan1_dualsided = (sysinfo->banksize[5] || sysinfo->banksize[7]);

	if (sdram_capabilities_enhanced_addressing_xor()) {
		if (!sysinfo->interleaved) {
			/* Single Channel & Dual Channel Asymmetric */
			if (chan0_populated) {
				if (chan0_dualsided)
					chan0 = EA_SINGLECHANNEL_XOR_BANK_RANK_MODE;
				else
					chan0 = EA_SINGLECHANNEL_XOR_BANK_MODE;
			}
			if (chan1_populated) {
				if (chan1_dualsided)
					chan1 = EA_SINGLECHANNEL_XOR_BANK_RANK_MODE;
				else
					chan1 = EA_SINGLECHANNEL_XOR_BANK_MODE;
			}
		} else {
			/* Interleaved has always both channels populated */
			if (chan0_dualsided)
				chan0 = EA_DUALCHANNEL_XOR_BANK_RANK_MODE;
			else
				chan0 = EA_DUALCHANNEL_XOR_BANK_MODE;

			if (chan1_dualsided)
				chan1 = EA_DUALCHANNEL_XOR_BANK_RANK_MODE;
			else
				chan1 = EA_DUALCHANNEL_XOR_BANK_MODE;
		}
	} else {
		if (!sysinfo->interleaved) {
			/* Single Channel & Dual Channel Asymmetric */
			if (chan0_populated) {
				if (chan0_dualsided)
					chan0 = EA_SINGLECHANNEL_BANK_RANK_MODE;
				else
					chan0 = EA_SINGLECHANNEL_BANK_MODE;
			}
			if (chan1_populated) {
				if (chan1_dualsided)
					chan1 = EA_SINGLECHANNEL_BANK_RANK_MODE;
				else
					chan1 = EA_SINGLECHANNEL_BANK_MODE;
			}
		} else {
			/* Interleaved has always both channels populated */
			if (chan0_dualsided)
				chan0 = EA_DUALCHANNEL_BANK_RANK_MODE;
			else
				chan0 = EA_DUALCHANNEL_BANK_MODE;

			if (chan1_dualsided)
				chan1 = EA_DUALCHANNEL_BANK_RANK_MODE;
			else
				chan1 = EA_DUALCHANNEL_BANK_MODE;
		}
	}

	mchbar_clrbits32(C0DRC1, 0xff << 24);
	mchbar_setbits32(C0DRC1, chan0);
	mchbar_clrbits32(C1DRC1, 0xff << 24);
	mchbar_setbits32(C1DRC1, chan1);
}

static void sdram_post_jedec_initialization(struct sys_info *sysinfo)
{
	u32 reg32;

	/* Enable Channel XORing for Dual Channel Interleave */
	if (sysinfo->interleaved) {
		reg32 = mchbar_read32(DCC);
		reg32 &= ~(1 << 10);
		reg32 |= (1 << 9);
		mchbar_write32(DCC, reg32);
	}

	/* DRAM mode optimizations */
	sdram_enhanced_addressing_mode(sysinfo);

	reg32 = mchbar_read32(FSBPMC3);
	reg32 &= ~(1 << 1);
	mchbar_write32(FSBPMC3, reg32);

	reg32 = mchbar_read32(SBTEST);
	reg32 &= ~(1 << 2);
	mchbar_write32(SBTEST, reg32);

	reg32 = mchbar_read32(SBOCC);
	reg32 &= 0xffbdb6ff;
	reg32 |= (0xbdb6 << 8) | (1 << 0);
	mchbar_write32(SBOCC, reg32);
}

static void sdram_power_management(struct sys_info *sysinfo)
{
	u16 reg16;
	u32 reg32;
	bool integrated_graphics = true;
	int i;

	if (!(pci_read_config8(HOST_BRIDGE, DEVEN) & (DEVEN_D2F0 | DEVEN_D2F1)))
		integrated_graphics = false;

	reg32 = mchbar_read32(C0DRT2);
	reg32 &= 0xffffff00;
	/* Idle timer = 8 clocks, CKE idle timer = 16 clocks */
	reg32 |= (1 << 5) | (1 << 4);
	mchbar_write32(C0DRT2, reg32);

	reg32 = mchbar_read32(C1DRT2);
	reg32 &= 0xffffff00;
	/* Idle timer = 8 clocks, CKE idle timer = 16 clocks */
	reg32 |= (1 << 5) | (1 << 4);
	mchbar_write32(C1DRT2, reg32);

	reg32 = mchbar_read32(C0DRC1);

	reg32 |= (1 << 12) | (1 << 11);
	mchbar_write32(C0DRC1, reg32);

	reg32 = mchbar_read32(C1DRC1);

	reg32 |= (1 << 12) | (1 << 11);
	mchbar_write32(C1DRC1, reg32);

	if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM)) {
		if (i945_silicon_revision() > 1) {
			/* FIXME bits 5 and 0 only if PCIe graphics is disabled */
			u16 peg_bits = (1 << 5) | (1 << 0);

			mchbar_write16(UPMC1, 0x1010 | peg_bits);
		} else {
			/* FIXME bits 5 and 0 only if PCIe graphics is disabled */
			u16 peg_bits = (1 << 5) | (1 << 0);

			/* Rev 0 and 1 */
			mchbar_write16(UPMC1, 0x0010 | peg_bits);
		}
	}

	reg16 = mchbar_read16(UPMC2);
	reg16 &= 0xfc00;
	reg16 |= 0x0100;
	mchbar_write16(UPMC2, reg16);

	mchbar_write32(UPMC3, 0x000f06ff);

	for (i = 0; i < 5; i++) {
		mchbar_clrbits32(UPMC3, 1 << 16);
		mchbar_setbits32(UPMC3, 1 << 16);
	}

	mchbar_write32(GIPMC1, 0x8000000c);

	reg16 = mchbar_read16(CPCTL);
	reg16 &= ~(7 << 11);
	if (i945_silicon_revision() > 2)
		reg16 |= (6 << 11);
	else
		reg16 |= (4 << 11);
	mchbar_write16(CPCTL, reg16);

#if 0
	if ((mchbar_read32(ECO) & (1 << 16)) != 0) {
#else
	if (i945_silicon_revision() != 0) {
#endif
		switch (sysinfo->fsb_frequency) {
		case 667:
			mchbar_write32(HGIPMC2, 0x0d590d59);
			break;
		case 533:
			mchbar_write32(HGIPMC2, 0x155b155b);
			break;
		}
	} else {
		switch (sysinfo->fsb_frequency) {
		case 667:
			mchbar_write32(HGIPMC2, 0x09c409c4);
			break;
		case 533:
			mchbar_write32(HGIPMC2, 0x0fa00fa0);
			break;
		}
	}

	mchbar_write32(FSBPMC1, 0x8000000c);

	reg32 = mchbar_read32(C2C3TT);
	reg32 &= 0xffff0000;
	switch (sysinfo->fsb_frequency) {
	case 667:
		reg32 |= 0x0600;
		break;
	case 533:
		reg32 |= 0x0480;
		break;
	}
	mchbar_write32(C2C3TT, reg32);

	reg32 = mchbar_read32(C3C4TT);
	reg32 &= 0xffff0000;
	switch (sysinfo->fsb_frequency) {
	case 667:
		reg32 |= 0x0b80;
		break;
	case 533:
		reg32 |= 0x0980;
		break;
	}
	mchbar_write32(C3C4TT, reg32);

	if (i945_silicon_revision() == 0)
		mchbar_clrbits32(ECO, 1 << 16);
	else
		mchbar_setbits32(ECO, 1 << 16);

	mchbar_clrbits32(FSBPMC3, 1 << 29);

	mchbar_setbits32(FSBPMC3, 1 << 21);

	mchbar_clrbits32(FSBPMC3, 1 << 19);

	mchbar_clrbits32(FSBPMC3, 1 << 13);

	reg32 = mchbar_read32(FSBPMC4);
	reg32 &= ~(3 << 24);
	reg32 |= (2 << 24);
	mchbar_write32(FSBPMC4, reg32);

	mchbar_setbits32(FSBPMC4, 1 << 21);

	mchbar_setbits32(FSBPMC4, 1 << 5);

	if ((i945_silicon_revision() < 2)) { /* || cpuid() = 0x6e8 */
		/* stepping 0 and 1 or CPUID 6e8 */
		mchbar_clrbits32(FSBPMC4, 1 << 4);
	} else {
		mchbar_setbits32(FSBPMC4, 1 << 4);
	}

	pci_or_config8(HOST_BRIDGE, 0xfc, 1 << 4);

	pci_or_config8(IGD_DEV, 0xc1, 1 << 2);

	if (integrated_graphics) {
		mchbar_write16(MIPMC4, 0x04f8);
		mchbar_write16(MIPMC5, 0x04fc);
		mchbar_write16(MIPMC6, 0x04fc);
	} else {
		mchbar_write16(MIPMC4, 0x64f8);
		mchbar_write16(MIPMC5, 0x64fc);
		mchbar_write16(MIPMC6, 0x64fc);
	}

	reg32 = mchbar_read32(PMCFG);
	reg32 &= ~(3 << 17);
	reg32 |= (2 << 17);
	mchbar_write32(PMCFG, reg32);

	mchbar_setbits32(PMCFG, 1 << 4);

	reg32 = mchbar_read32(UPMC4);
	reg32 &= 0xffffff00;
	reg32 |= 0x01;
	mchbar_write32(UPMC4, reg32);

	mchbar_clrbits32(0xb18, 1 << 21);
}

static void sdram_thermal_management(void)
{

	mchbar_write8(TCO1, 0);
	mchbar_write8(TCO0, 0);

	/* The Thermal Sensors for DIMMs at 0x50, 0x52 are at I2C addr 0x30/0x32. */

	/* TODO This is not implemented yet. Volunteers? */
}

static void sdram_save_receive_enable(void)
{
	int i;
	u32 reg32;
	u8 values[4];

	/* The following values are stored to an unused CMOS area and restored instead of
	 * recalculated in case of an S3 resume.
	 *
	 * C0WL0REOST [7:0]		-> 8 bit
	 * C1WL0REOST [7:0]		-> 8 bit
	 * RCVENMT    [11:8] [3:0]	-> 8 bit
	 * C0DRT1     [27:24]		-> 4 bit
	 * C1DRT1     [27:24]		-> 4 bit
	 */

	values[0] = mchbar_read8(C0WL0REOST);
	values[1] = mchbar_read8(C1WL0REOST);

	reg32 = mchbar_read32(RCVENMT);
	values[2] = (u8)((reg32 >> (8 - 4)) & 0xf0) | (reg32 & 0x0f);

	reg32 = mchbar_read32(C0DRT1);
	values[3] = (reg32 >> 24) & 0x0f;
	reg32 = mchbar_read32(C1DRT1);
	values[3] |= (reg32 >> (24 - 4)) & 0xf0;

	/* coreboot only uses bytes 0 - 127 for its CMOS values so far
	 * so we grab bytes 128 - 131 to save the receive enable values
	 */

	for (i = 0; i < 4; i++)
		cmos_write(values[i], 128 + i);
}

static void sdram_recover_receive_enable(void)
{
	int i;
	u32 reg32;
	u8 values[4];

	for (i = 0; i < 4; i++)
		values[i] = cmos_read(128 + i);

	mchbar_write8(C0WL0REOST, values[0]);
	mchbar_write8(C1WL0REOST, values[1]);

	reg32 = mchbar_read32(RCVENMT);
	reg32 &= ~((0x0f << 8) | (0x0f << 0));
	reg32 |= ((u32)(values[2] & 0xf0) << (8 - 4)) | (values[2] & 0x0f);
	mchbar_write32(RCVENMT, reg32);

	reg32 = mchbar_read32(C0DRT1) & ~(0x0f << 24);
	reg32 |= (u32)(values[3] & 0x0f) << 24;
	mchbar_write32(C0DRT1, reg32);

	reg32 = mchbar_read32(C1DRT1) & ~(0x0f << 24);
	reg32 |= (u32)(values[3] & 0xf0) << (24 - 4);
	mchbar_write32(C1DRT1, reg32);
}

static void sdram_program_receive_enable(struct sys_info *sysinfo)
{
	mchbar_setbits32(REPC, 1 << 0);

	/* Program Receive Enable Timings */
	if (sysinfo->boot_path == BOOT_PATH_RESUME) {
		sdram_recover_receive_enable();
	} else {
		receive_enable_adjust(sysinfo);
		sdram_save_receive_enable();
	}

	mchbar_setbits32(C0DRC1, 1 << 6);
	mchbar_setbits32(C1DRC1, 1 << 6);
	mchbar_clrbits32(C0DRC1, 1 << 6);
	mchbar_clrbits32(C1DRC1, 1 << 6);

	mchbar_setbits32(MIPMC3, 0x0f << 0);
}

/**
 * @brief Enable On-Die Termination for DDR2.
 *
 */

static void sdram_on_die_termination(struct sys_info *sysinfo)
{
	static const u32 odt[] = {
		0x00024911, 0xe0010000,
		0x00049211, 0xe0020000,
		0x0006db11, 0xe0030000,
	};

	u32 reg32;
	int cas;

	reg32 = mchbar_read32(ODTC);
	reg32 &= ~(3 << 16);
	reg32 |= (1 << 14) | (1 << 6) | (2 << 16);
	mchbar_write32(ODTC, reg32);

	if (sysinfo->dimm[0] == SYSINFO_DIMM_NOT_POPULATED ||
	    sysinfo->dimm[1] == SYSINFO_DIMM_NOT_POPULATED) {
		printk(BIOS_DEBUG, "one dimm per channel config..\n");

		reg32 = mchbar_read32(C0ODT);
		reg32 &= ~(7 << 28);
		mchbar_write32(C0ODT, reg32);
		reg32 = mchbar_read32(C1ODT);
		reg32 &= ~(7 << 28);
		mchbar_write32(C1ODT, reg32);
	}

	cas = sysinfo->cas;

	reg32 = mchbar_read32(C0ODT) & 0xfff00000;
	reg32 |= odt[(cas - 3) * 2];
	mchbar_write32(C0ODT, reg32);

	reg32 = mchbar_read32(C1ODT) & 0xfff00000;
	reg32 |= odt[(cas - 3) * 2];
	mchbar_write32(C1ODT, reg32);

	reg32 = mchbar_read32(C0ODT + 4) & 0x1fc8ffff;
	reg32 |= odt[((cas - 3) * 2) + 1];
	mchbar_write32(C0ODT + 4, reg32);

	reg32 = mchbar_read32(C1ODT + 4) & 0x1fc8ffff;
	reg32 |= odt[((cas - 3) * 2) + 1];
	mchbar_write32(C1ODT + 4, reg32);
}

/**
 * @brief Enable clocks to populated sockets
 */

static void sdram_enable_memory_clocks(struct sys_info *sysinfo)
{
	u8 clocks[2] = { 0, 0 };

#if CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM)
#define CLOCKS_WIDTH 2
#elif CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GC)
#define CLOCKS_WIDTH 3
#endif
	if (sysinfo->dimm[0] != SYSINFO_DIMM_NOT_POPULATED)
		clocks[0] |= (1 << CLOCKS_WIDTH) - 1;

	if (sysinfo->dimm[1] != SYSINFO_DIMM_NOT_POPULATED)
		clocks[0] |= ((1 << CLOCKS_WIDTH) - 1) << CLOCKS_WIDTH;

	if (sysinfo->dimm[2] != SYSINFO_DIMM_NOT_POPULATED)
		clocks[1] |= (1 << CLOCKS_WIDTH) - 1;

	if (sysinfo->dimm[3] != SYSINFO_DIMM_NOT_POPULATED)
		clocks[1] |= ((1 << CLOCKS_WIDTH) - 1) << CLOCKS_WIDTH;

#if CONFIG(OVERRIDE_CLOCK_DISABLE)
	/* Usually system firmware turns off system memory clock signals to unused SO-DIMM slots
	 * to reduce EMI and power consumption.
	 * However, the Kontron 986LCD-M does not like unused clock signals to be disabled.
	 */

	clocks[0] = 0xf; /* force all clock gate pairs to enable */
	clocks[1] = 0xf; /* force all clock gate pairs to enable */
#endif

	mchbar_write8(C0DCLKDIS, clocks[0]);
	mchbar_write8(C1DCLKDIS, clocks[1]);
}

#define RTT_ODT_NONE	0
#define RTT_ODT_50_OHM	((1 << 9) | (1 << 5))
#define RTT_ODT_75_OHM	(1 << 5)
#define RTT_ODT_150_OHM	(1 << 9)

#define EMRS_OCD_DEFAULT	((1 << 12) | (1 << 11) | (1 << 10))

#define MRS_CAS_3	(3 << 7)
#define MRS_CAS_4	(4 << 7)
#define MRS_CAS_5	(5 << 7)

#define MRS_TWR_3	(2 << 12)
#define MRS_TWR_4	(3 << 12)
#define MRS_TWR_5	(4 << 12)

#define MRS_BT		(1 << 6)

#define MRS_BL4		(2 << 3)
#define MRS_BL8		(3 << 3)

static void sdram_jedec_enable(struct sys_info *sysinfo)
{
	int i, nonzero;
	u32 bankaddr = 0, tmpaddr, mrsaddr = 0;

	for (i = 0, nonzero = -1; i < 8; i++) {
		if (sysinfo->banksize[i] == 0)
			continue;

		printk(BIOS_DEBUG, "jedec enable sequence: bank %d\n", i);

		if (nonzero != -1) {
			if (sysinfo->interleaved && nonzero < 4 && i >= 4) {
				bankaddr = 0x40;
			} else {
				printk(BIOS_DEBUG, "bankaddr from bank size of rank %d\n",
					nonzero);
				bankaddr += sysinfo->banksize[nonzero] <<
					(sysinfo->interleaved ? 26 : 25);
			}
		}

		/*
		 * We have a bank with a non-zero size... Remember it
		 * for the next offset we have to calculate
		 */
		nonzero = i;

		/* Get CAS latency set up */
		switch (sysinfo->cas) {
		case 5:
			mrsaddr = MRS_CAS_5;
			break;
		case 4:
			mrsaddr = MRS_CAS_4;
			break;
		case 3:
			mrsaddr = MRS_CAS_3;
			break;
		default:
			die("Jedec Error (CAS).\n");
		}

		/* Get tWR set */
		switch (sysinfo->twr) {
		case 5:
			mrsaddr |= MRS_TWR_5;
			break;
		case 4:
			mrsaddr |= MRS_TWR_4;
			break;
		case 3:
			mrsaddr |= MRS_TWR_3;
			break;
		default:
			die("Jedec Error (tWR).\n");
		}

		/* Set "Burst Type" */
		mrsaddr |= MRS_BT;

		/* Interleaved */
		if (sysinfo->interleaved)
			mrsaddr = mrsaddr << 1;

		/* Only burst length 8 supported */
		mrsaddr |= MRS_BL8;

		/* Apply NOP */
		PRINTK_DEBUG("Apply NOP\n");
		do_ram_command(RAM_COMMAND_NOP);
		ram_read32(bankaddr);

		/* Precharge all banks */
		PRINTK_DEBUG("All Banks Precharge\n");
		do_ram_command(RAM_COMMAND_PRECHARGE);
		ram_read32(bankaddr);

		/* Extended Mode Register Set (2) */
		PRINTK_DEBUG("Extended Mode Register Set(2)\n");
		do_ram_command(RAM_COMMAND_EMRS | RAM_EMRS_2);
		ram_read32(bankaddr);

		/* Extended Mode Register Set (3) */
		PRINTK_DEBUG("Extended Mode Register Set(3)\n");
		do_ram_command(RAM_COMMAND_EMRS | RAM_EMRS_3);
		ram_read32(bankaddr);

		/* Extended Mode Register Set */
		PRINTK_DEBUG("Extended Mode Register Set\n");
		do_ram_command(RAM_COMMAND_EMRS | RAM_EMRS_1);
		tmpaddr = bankaddr;
		if (!sdram_capabilities_dual_channel())
			tmpaddr |= RTT_ODT_75_OHM;
		else if (sysinfo->interleaved)
			tmpaddr |= (RTT_ODT_150_OHM << 1);
		else
			tmpaddr |= RTT_ODT_150_OHM;
		ram_read32(tmpaddr);

		/* Mode Register Set: Reset DLLs */
		PRINTK_DEBUG("MRS: Reset DLLs\n");
		do_ram_command(RAM_COMMAND_MRS);
		tmpaddr = bankaddr;
		tmpaddr |= mrsaddr;
		/* Set DLL reset bit */
		if (sysinfo->interleaved)
			tmpaddr |= (1 << 12);
		else
			tmpaddr |= (1 << 11);
		ram_read32(tmpaddr);

		/* Precharge all banks */
		PRINTK_DEBUG("All Banks Precharge\n");
		do_ram_command(RAM_COMMAND_PRECHARGE);
		ram_read32(bankaddr);

		/* CAS before RAS Refresh */
		PRINTK_DEBUG("CAS before RAS\n");
		do_ram_command(RAM_COMMAND_CBR);

		/* CBR wants two READs */
		ram_read32(bankaddr);
		ram_read32(bankaddr);

		/* Mode Register Set: Enable DLLs */
		PRINTK_DEBUG("MRS: Enable DLLs\n");
		do_ram_command(RAM_COMMAND_MRS);

		tmpaddr = bankaddr;
		tmpaddr |= mrsaddr;
		ram_read32(tmpaddr);

		/* Extended Mode Register Set */
		PRINTK_DEBUG("Extended Mode Register Set: ODT/OCD\n");
		do_ram_command(RAM_COMMAND_EMRS | RAM_EMRS_1);

		tmpaddr = bankaddr;
		if (!sdram_capabilities_dual_channel())
			tmpaddr |= RTT_ODT_75_OHM | EMRS_OCD_DEFAULT;
		else if (sysinfo->interleaved)
			tmpaddr |= ((RTT_ODT_150_OHM | EMRS_OCD_DEFAULT) << 1);
		else
			tmpaddr |= RTT_ODT_150_OHM | EMRS_OCD_DEFAULT;
		ram_read32(tmpaddr);

		/* Extended Mode Register Set */
		PRINTK_DEBUG("Extended Mode Register Set: OCD Exit\n");
		do_ram_command(RAM_COMMAND_EMRS | RAM_EMRS_1);

		tmpaddr = bankaddr;
		if (!sdram_capabilities_dual_channel())
			tmpaddr |= RTT_ODT_75_OHM;
		else if (sysinfo->interleaved)
			tmpaddr |= (RTT_ODT_150_OHM << 1);
		else
			tmpaddr |= RTT_ODT_150_OHM;
		ram_read32(tmpaddr);
	}
}

static void sdram_init_complete(void)
{
	PRINTK_DEBUG("Normal Operation\n");
	do_ram_command(RAM_COMMAND_NORMAL);
}

static void sdram_setup_processor_side(void)
{
	if (i945_silicon_revision() == 0)
		mchbar_setbits32(FSBPMC3, 1 << 2);

	mchbar_setbits8(0xb00, 1);

	if (i945_silicon_revision() == 0)
		mchbar_setbits32(SLPCTL, 1 << 8);
}

/**
 * @param boot_path: 0 = normal, 1 = reset, 2 = resume from s3
 * @param spd_addresses pointer to a list of SPD addresses
 */
void sdram_initialize(int boot_path, const u8 *spd_addresses)
{
	struct sys_info sysinfo;

	timestamp_add_now(TS_INITRAM_START);
	printk(BIOS_DEBUG, "Setting up RAM controller.\n");

	memset(&sysinfo, 0, sizeof(sysinfo));

	sysinfo.boot_path = boot_path;
	sysinfo.spd_addresses = spd_addresses;

	/* Look at the type of DIMMs and verify all DIMMs are x8 or x16 width */
	sdram_get_dram_configuration(&sysinfo);

	/* If error, do cold boot */
	sdram_detect_errors(&sysinfo);

	/* Program PLL settings */
	sdram_program_pll_settings(&sysinfo);

	/*
	 * Program Graphics Frequency
	 * Set core display and render clock on 945GC to the max
	 */
	if (CONFIG(NORTHBRIDGE_INTEL_SUBTYPE_I945GM))
		sdram_program_graphics_frequency(&sysinfo);
	else
		pci_write_config16(IGD_DEV, GCFC, 0x0534);

	/* Program System Memory Frequency */
	sdram_program_memory_frequency(&sysinfo);

	/* Determine Mode of Operation (Interleaved etc) */
	sdram_set_channel_mode(&sysinfo);

	/* Program Clock Crossing values */
	sdram_program_clock_crossing();

	/* Disable fast dispatch */
	sdram_disable_fast_dispatch();

	/* Enable WIODLL Power Down in ACPI states */
	mchbar_setbits32(C0DMC, 1 << 24);
	mchbar_setbits32(C1DMC, 1 << 24);

	/* Program DRAM Row Boundary/Attribute Registers */

	/* program row size DRB and set TOLUD */
	sdram_program_row_boundaries(&sysinfo);

	/* program page size DRA */
	sdram_set_row_attributes(&sysinfo);

	/* Program CxBNKARC */
	sdram_set_bank_architecture(&sysinfo);

	/* Program DRAM Timing and Control registers based on SPD */
	sdram_set_timing_and_control(&sysinfo);

	/* On-Die Termination Adjustment */
	sdram_on_die_termination(&sysinfo);

	/* Pre Jedec Initialization */
	sdram_pre_jedec_initialization();

	/* Perform System Memory IO Initialization */
	sdram_initialize_system_memory_io(&sysinfo);

	/* Perform System Memory IO Buffer Enable */
	sdram_enable_system_memory_io(&sysinfo);

	/* Enable System Memory Clocks */
	sdram_enable_memory_clocks(&sysinfo);

	if (boot_path == BOOT_PATH_NORMAL) {
		/* Jedec Initialization sequence */
		sdram_jedec_enable(&sysinfo);
	}

	/* Program Power Management Registers */
	sdram_power_management(&sysinfo);

	/* Post Jedec Init */
	sdram_post_jedec_initialization(&sysinfo);

	/* Program DRAM Throttling */
	sdram_thermal_management();

	/* Normal Operations */
	sdram_init_complete();

	/* Program Receive Enable Timings */
	sdram_program_receive_enable(&sysinfo);

	/* Enable Periodic RCOMP */
	sdram_enable_rcomp();

	/* Tell ICH7 that we're done */
	pci_and_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_2, (u8)~(1 << 7));

	printk(BIOS_DEBUG, "RAM initialization finished.\n");

	sdram_setup_processor_side();
	timestamp_add_now(TS_INITRAM_END);
}
