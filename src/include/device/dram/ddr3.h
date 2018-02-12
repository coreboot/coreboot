/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * JEDEC Standard No. 21-C
 * Annex K: Serial Presence Detect (SPD) for DDR3 SDRAM Modules 2014
 * http://www.jedec.org/sites/default/files/docs/4_01_02_11R24.pdf
 */

#ifndef DEVICE_DRAM_DDR3L_H
#define DEVICE_DRAM_DDR3L_H

/**
 * @file ddr3.h
 *
 * \brief Utilities for decoding DDR3 SPDs
 */

#include <stdint.h>
#include <spd.h>
#include <device/dram/common.h>


/**
 * Convenience definitions for SPD offsets
 *
 * @{
 */
#define SPD_DIMM_MOD_ID1	117
#define SPD_DIMM_MOD_ID2	118
#define SPD_DIMM_SERIAL_NUM	122
#define SPD_DIMM_SERIAL_LEN	4
#define SPD_DIMM_PART_NUM	128
#define SPD_DIMM_PART_LEN	18
/** @} */

/**
 * \brief Convenience macro for enabling printk with CONFIG_DEBUG_RAM_SETUP
 *
 * Use this macro instead of printk(); for verbose RAM initialization messages.
 * When CONFIG_DEBUG_RAM_SETUP is not selected, these messages are automatically
 * disabled.
 * @{
 */
#if IS_ENABLED(CONFIG_DEBUG_RAM_SETUP)
#define printram(x, ...) printk(BIOS_DEBUG, x, ##__VA_ARGS__)
#else
#define printram(x, ...)
#endif
/** @} */

/*
 * Module type (byte 3, bits 3:0) of SPD
 * This definition is specific to DDR3. DDR2 SPDs have a different structure.
 */
enum spd_dimm_type {
	SPD_DIMM_TYPE_UNDEFINED			= 0x00,
	SPD_DIMM_TYPE_RDIMM			= 0x01,
	SPD_DIMM_TYPE_UDIMM			= 0x02,
	SPD_DIMM_TYPE_SO_DIMM			= 0x03,
	SPD_DIMM_TYPE_MICRO_DIMM		= 0x04,
	SPD_DIMM_TYPE_MINI_RDIMM		= 0x05,
	SPD_DIMM_TYPE_MINI_UDIMM		= 0x06,
	SPD_DIMM_TYPE_MINI_CDIMM		= 0x07,
	SPD_DIMM_TYPE_72B_SO_UDIMM		= 0x08,
	SPD_DIMM_TYPE_72B_SO_RDIMM		= 0x09,
	SPD_DIMM_TYPE_72B_SO_CDIMM		= 0x0a,
	SPD_DIMM_TYPE_LRDIMM			= 0x0b,
	SPD_DIMM_TYPE_16B_SO_DIMM		= 0x0c,
	SPD_DIMM_TYPE_32B_SO_DIMM		= 0x0d,
	/* Masks to bits 3:0 to give the dimm type */
	SPD_DIMM_TYPE_MASK			= 0x0f,
};

/**
 * \brief DIMM flags
 *
 * Characteristic flags for the DIMM, as presented by the SPD
 */
typedef union dimm_flags_st {
	/* The whole point of the union/struct construct is to allow us to clear
	 * all the bits with one line: flags.raw = 0.
	 * We do not care how these bits are ordered */
	struct {
		/* Indicates if rank 1 of DIMM uses a mirrored pin mapping. See:
		 * Annex K: Serial Presence Detect (SPD) for DDR3 SDRAM */
		unsigned int pins_mirrored:1;
		/* Module can work at 1.50V - All DIMMS must be 1.5V operable */
		unsigned int operable_1_50V:1;
		/* Module can work at 1.35V */
		unsigned int operable_1_35V:1;
		/* Module can work at 1.20V */
		unsigned int operable_1_25V:1;
		/* Has an 8-bit bus extension, meaning the DIMM supports ECC */
		unsigned int is_ecc:1;
		/* DLL-Off Mode Support */
		unsigned int dll_off_mode:1;
		/* Indicates a drive strength of RZQ/6 (40 Ohm) is supported */
		unsigned int rzq6_supported:1;
		/* Indicates a drive strength of RZQ/7 (35 Ohm) is supported */
		unsigned int rzq7_supported:1;
		/* Partial Array Self Refresh */
		unsigned int pasr:1;
		/* On-die Thermal Sensor Readout */
		unsigned int odts:1;
		/* Auto Self Refresh */
		unsigned int asr:1;
		/* Extended temperature range supported */
		unsigned int ext_temp_range:1;
		/* Operating at extended temperature requires 2X refresh rate */
		unsigned int ext_temp_refresh:1;
		/* Thermal sensor incorporated */
		unsigned int therm_sensor:1;
	};
	unsigned int raw;
} dimm_flags_t;

/**
 * \brief DIMM characteristics
 *
 * The characteristics of each DIMM, as presented by the SPD
 */
typedef struct dimm_attr_st {
	enum spd_memory_type dram_type;
	enum spd_dimm_type dimm_type;
	u16 cas_supported;
	/* Flags extracted from SPD */
	dimm_flags_t flags;
	/* SDRAM width */
	u8 width;
	/* Number of ranks */
	u8 ranks;
	/* Number or row address bits */
	u8 row_bits;
	/* Number or column address bits */
	u8 col_bits;
	/* Size of module in MiB */
	u32 size_mb;
	/* Latencies are in units of 1/256 ns */
	u32 tCK;
	u32 tAA;
	u32 tWR;
	u32 tRCD;
	u32 tRRD;
	u32 tRP;
	u32 tRAS;
	u32 tRC;
	u32 tRFC;
	u32 tWTR;
	u32 tRTP;
	u32 tFAW;

	u8 reference_card;
	/* XMP: Module voltage in mV */
	u16 voltage;
	/* XMP: max DIMMs per channel supported (1-4) */
	u8 dimms_per_channel;
	/* Manufacturer ID */
	u16 manufacturer_id;
	/* ASCII part number - NULL terminated */
	u8 part_number[17];
} dimm_attr;

enum ddr3_xmp_profile {
	DDR3_XMP_PROFILE_1 = 0,
	DDR3_XMP_PROFILE_2 = 1,
};

typedef u8 spd_raw_data[256];

u16 spd_ddr3_calc_crc(u8 *spd, int len);
u16 spd_ddr3_calc_unique_crc(u8 *spd, int len);
int spd_decode_ddr3(dimm_attr *dimm, spd_raw_data spd_data);
int spd_dimm_is_registered_ddr3(enum spd_dimm_type type);
void dram_print_spd_ddr3(const dimm_attr *dimm);
int spd_xmp_decode_ddr3(dimm_attr *dimm,
			spd_raw_data spd,
			enum ddr3_xmp_profile profile);

/**
 * \brief Read double word from specified address
 *
 * Should be useful when doing an MRS to the DIMM
 */
static inline u32 volatile_read(volatile uintptr_t addr)
{
	volatile u32 result;
	result = *(volatile u32 *)addr;
	return result;
}

/**
 * \brief Representation of an MRS command
 *
 * This represents an MRS command as seen by the DIMM. This is not a memory
 * address that can be read to generate an MRS command. The mapping of CPU
 * to memory pins is hardware-dependent.
 * \n
 * The idea is to generalize the MRS code, and only need a hardware-specific
 * function to map the MRS bits to CPU address bits. An MRS command can be
 * sent like:
 * @code{.c}
 *	u32 addr;
 *	mrs_cmd_t mrs;
 *	chipset_enable_mrs_command_mode();
 *	mrs = ddr3_get_mr2(rtt_wr, srt, asr, cwl)
 *	if (rank_has_mirrorred_pins)
 *		mrs = ddr3_mrs_mirror_pins(mrs);
 *	addr = chipset_specific_get_mrs_addr(mrs);
 *	volatile_read(addr);
 * @endcode
 *
 * The MRS representation has the following structure:
 *	- cmd[15:0] = Address pins MA[15:0]
 *	- cmd[18:16] = Bank address BA[2:0]
 */
typedef u32 mrs_cmd_t;

enum ddr3_mr0_precharge {
	DDR3_MR0_PRECHARGE_SLOW = 0,
	DDR3_MR0_PRECHARGE_FAST = 1,
};
enum ddr3_mr0_mode {
	DDR3_MR0_MODE_NORMAL = 0,
	DDR3_MR0_MODE_TEST = 1,
};
enum ddr3_mr0_dll_reset {
	DDR3_MR0_DLL_RESET_NO = 0,
	DDR3_MR0_DLL_RESET_YES = 1,
};
enum ddr3_mr0_burst_type {
	DDR3_MR0_BURST_TYPE_SEQUENTIAL = 0,
	DDR3_MR0_BURST_TYPE_INTERLEAVED = 1,
};
enum ddr3_mr0_burst_length {
	DDR3_MR0_BURST_LENGTH_8 = 0,
	DDR3_MR0_BURST_LENGTH_CHOP = 1,
	DDR3_MR0_BURST_LENGTH_4 = 2,
};
mrs_cmd_t ddr3_get_mr0(enum ddr3_mr0_precharge precharge_pd,
		       u8 write_recovery,
		       enum ddr3_mr0_dll_reset dll_reset,
		       enum ddr3_mr0_mode mode,
		       u8 cas,
		       enum ddr3_mr0_burst_type interleaved_burst,
		       enum ddr3_mr0_burst_length burst_length);

enum ddr3_mr1_qoff {
	DDR3_MR1_QOFF_ENABLE = 0,
	DDR3_MR1_QOFF_DISABLE = 1,
};
enum ddr3_mr1_tqds {
	DDR3_MR1_TQDS_DISABLE = 0,
	DDR3_MR1_TQDS_ENABLE = 1,
};
enum ddr3_mr1_write_leveling {
	DDR3_MR1_WRLVL_DISABLE = 0,
	DDR3_MR1_WRLVL_ENABLE = 1,
};
enum ddr3_mr1_rtt_nom {
	DDR3_MR1_RTT_NOM_OFF = 0,
	DDR3_MR1_RTT_NOM_RZQ4 = 1,
	DDR3_MR1_RTT_NOM_RZQ2 = 2,
	DDR3_MR1_RTT_NOM_RZQ6 = 3,
	DDR3_MR1_RTT_NOM_RZQ12 = 4,
	DDR3_MR1_RTT_NOM_RZQ8 = 5,
};
enum ddr3_mr1_additive_latency {
	DDR3_MR1_AL_DISABLE = 0,
	DDR3_MR1_AL_CL_MINUS_1 = 1,
	DDR3_MR1_AL_CL_MINUS_2 = 2,
};
enum ddr3_mr1_ods {
	DDR3_MR1_ODS_RZQ6 = 0,
	DDR3_MR1_ODS_RZQ7 = 1,
};
enum ddr3_mr1_dll {
	DDR3_MR1_DLL_ENABLE = 0,
	DDR3_MR1_DLL_DISABLE = 1,
};

mrs_cmd_t ddr3_get_mr1(enum ddr3_mr1_qoff qoff,
		       enum ddr3_mr1_tqds tqds,
		       enum ddr3_mr1_rtt_nom rtt_nom,
		       enum ddr3_mr1_write_leveling write_leveling,
		       enum ddr3_mr1_ods output_drive_strenght,
		       enum ddr3_mr1_additive_latency additive_latency,
		       enum ddr3_mr1_dll dll_disable);

enum ddr3_mr2_rttwr {
	DDR3_MR2_RTTWR_OFF = 0,
	DDR3_MR2_RTTWR_RZQ4 = 1,
	DDR3_MR2_RTTWR_RZQ2 = 2,
};
enum ddr3_mr2_srt_range {
	DDR3_MR2_SRT_NORMAL = 0,
	DDR3_MR2_SRT_EXTENDED = 1,
};
enum ddr3_mr2_asr {
	DDR3_MR2_ASR_MANUAL = 0,
	DDR3_MR2_ASR_AUTO = 1,
};

mrs_cmd_t ddr3_get_mr2(enum ddr3_mr2_rttwr rtt_wr,
		       enum ddr3_mr2_srt_range extended_temp,
		       enum ddr3_mr2_asr self_refresh, u8 cas_cwl);

mrs_cmd_t ddr3_get_mr3(char dataflow_from_mpr);
mrs_cmd_t ddr3_mrs_mirror_pins(mrs_cmd_t cmd);

#endif /* DEVICE_DRAM_DDR3L_H */
