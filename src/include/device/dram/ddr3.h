/* SPDX-License-Identifier: GPL-2.0-or-later */

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

#include <spd.h>
#include <device/dram/common.h>
#include <types.h>

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

/* Byte 3 [3:0]: DDR3 Module type information */
enum spd_dimm_type_ddr3 {
	SPD_DDR3_DIMM_TYPE_UNDEFINED		= 0x00,
	SPD_DDR3_DIMM_TYPE_RDIMM		= 0x01,
	SPD_DDR3_DIMM_TYPE_UDIMM		= 0x02,
	SPD_DDR3_DIMM_TYPE_SO_DIMM		= 0x03,
	SPD_DDR3_DIMM_TYPE_MICRO_DIMM		= 0x04,
	SPD_DDR3_DIMM_TYPE_MINI_RDIMM		= 0x05,
	SPD_DDR3_DIMM_TYPE_MINI_UDIMM		= 0x06,
	SPD_DDR3_DIMM_TYPE_MINI_CDIMM		= 0x07,
	SPD_DDR3_DIMM_TYPE_72B_SO_UDIMM		= 0x08,
	SPD_DDR3_DIMM_TYPE_72B_SO_RDIMM		= 0x09,
	SPD_DDR3_DIMM_TYPE_72B_SO_CDIMM		= 0x0a,
	SPD_DDR3_DIMM_TYPE_LRDIMM		= 0x0b,
	SPD_DDR3_DIMM_TYPE_16B_SO_DIMM		= 0x0c,
	SPD_DDR3_DIMM_TYPE_32B_SO_DIMM		= 0x0d,
	/* Masks to bits 3:0 to give the dimm type */
	SPD_DDR3_DIMM_TYPE_MASK			= 0x0f,
};

/**
 * \brief DIMM flags
 *
 * Characteristic flags for the DIMM, as presented by the SPD
 */
union dimm_flags_ddr3_st {
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
};

/**
 * \brief DIMM characteristics
 *
 * The characteristics of each DIMM, as presented by the SPD
 */
struct dimm_attr_ddr3_st {
	enum spd_memory_type dram_type;
	enum spd_dimm_type_ddr3 dimm_type;
	u16 cas_supported;
	/* Flags extracted from SPD */
	union dimm_flags_ddr3_st flags;
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
	u32 tCWL;
	u16 tCMD;

	u8 reference_card;
	/* XMP: Module voltage in mV */
	u16 voltage;
	/* XMP: max DIMMs per channel supported (1-4) */
	u8 dimms_per_channel;
	/* Manufacturer ID */
	u16 manufacturer_id;
	/* ASCII part number - NULL terminated */
	u8 part_number[17];
	/* Serial number */
	u8 serial[SPD_DIMM_SERIAL_LEN];
};

enum ddr3_xmp_profile {
	DDR3_XMP_PROFILE_1 = 0,
	DDR3_XMP_PROFILE_2 = 1,
};

typedef u8 spd_raw_data[256];

u16 spd_ddr3_calc_crc(u8 *spd, int len);
u16 spd_ddr3_calc_unique_crc(u8 *spd, int len);
int spd_decode_ddr3(struct dimm_attr_ddr3_st *dimm, spd_raw_data spd_data);
int spd_dimm_is_registered_ddr3(enum spd_dimm_type_ddr3 type);
void dram_print_spd_ddr3(const struct dimm_attr_ddr3_st *dimm);
int spd_xmp_decode_ddr3(struct dimm_attr_ddr3_st *dimm,
			spd_raw_data spd,
			enum ddr3_xmp_profile profile);
enum cb_err spd_add_smbios17(const u8 channel, const u8 slot,
			     const u16 selected_freq,
			     const struct dimm_attr_ddr3_st *info);

#endif /* DEVICE_DRAM_DDR3L_H */
