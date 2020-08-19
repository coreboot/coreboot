/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * JEDEC Standard No. 21-C
 * Annex L: Serial Presence Detect (SPD) for DDR4 SDRAM Modules
 */

#ifndef DEVICE_DRAM_DDR4L_H
#define DEVICE_DRAM_DDR4L_H

/**
 * @file ddr4.h
 *
 * \brief Utilities for decoding DDR4 SPDs
 */

#include <spd.h>
#include <device/dram/common.h>
#include <types.h>

#define SPD_DDR4_PART_OFF	329
#define SPD_DDR4_PART_LEN	20

/*
 * Module type (byte 3, bits 3:0) of SPD
 * This definition is specific to DDR4. DDR2/3 SPDs have a different structure.
 */
enum spd_dimm_type {
	SPD_DIMM_TYPE_EXTENDED			= 0x0,
	SPD_DIMM_TYPE_RDIMM			= 0x1,
	SPD_DIMM_TYPE_UDIMM			= 0x2,
	SPD_DIMM_TYPE_SO_DIMM			= 0x3,
	SPD_DIMM_TYPE_LRDIMM			= 0x4,
	SPD_DIMM_TYPE_MINI_RDIMM		= 0x5,
	SPD_DIMM_TYPE_MINI_UDIMM		= 0x6,
	SPD_DIMM_TYPE_72B_SO_RDIMM		= 0x8,
	SPD_DIMM_TYPE_72B_SO_UDIMM		= 0x9,
	SPD_DIMM_TYPE_16B_SO_DIMM		= 0xc,
	SPD_DIMM_TYPE_32B_SO_DIMM		= 0xd,
	/* Masks to bits 3:0 to give the dimm type */
	SPD_DIMM_TYPE_MASK			= 0xf
};

/**
 * \brief DIMM characteristics
 *
 * The characteristics of each DIMM, as presented by the SPD
 */
typedef struct dimm_attr_st {
	enum spd_memory_type dram_type;
	enum spd_dimm_type dimm_type;
	char part_number[SPD_DDR4_PART_LEN + 1];
	u8 serial_number[4];
	u8 bus_width;
	u8 ranks;
	u8 sdram_width;
	u16 cap_per_die_mbit;
	u16 size_mb;
	u16 manufacturer_id;
	u16 vdd_voltage;
	bool ecc_extension;
} dimm_attr;

typedef u8 spd_raw_data[512];

int spd_decode_ddr4(dimm_attr *dimm, spd_raw_data spd);

enum cb_err spd_add_smbios17_ddr4(const u8 channel, const u8 slot,
				const u16 selected_freq,
				const dimm_attr *info);

#endif /* DEVICE_DRAM_DDR4L_H */
