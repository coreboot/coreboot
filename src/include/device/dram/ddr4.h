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

/** Maximum SPD size supported */
#define SPD_SIZE_MAX_DDR4	512

#define SPD_DDR4_PART_OFF	329
#define SPD_DDR4_PART_LEN	20

/*
 * Module type (byte 3, bits 3:0) of SPD
 * This definition is specific to DDR4. DDR2/3 SPDs have a different structure.
 */
enum spd_dimm_type_ddr4 {
	SPD_DDR4_DIMM_TYPE_EXTENDED		= 0x0,
	SPD_DDR4_DIMM_TYPE_RDIMM		= 0x1,
	SPD_DDR4_DIMM_TYPE_UDIMM		= 0x2,
	SPD_DDR4_DIMM_TYPE_SO_DIMM		= 0x3,
	SPD_DDR4_DIMM_TYPE_LRDIMM		= 0x4,
	SPD_DDR4_DIMM_TYPE_MINI_RDIMM		= 0x5,
	SPD_DDR4_DIMM_TYPE_MINI_UDIMM		= 0x6,
	SPD_DDR4_DIMM_TYPE_72B_SO_RDIMM		= 0x8,
	SPD_DDR4_DIMM_TYPE_72B_SO_UDIMM		= 0x9,
	SPD_DDR4_DIMM_TYPE_16B_SO_DIMM		= 0xc,
	SPD_DDR4_DIMM_TYPE_32B_SO_DIMM		= 0xd,
	/* Masks to bits 3:0 to give the dimm type */
	SPD_DDR4_DIMM_TYPE_MASK			= 0xf
};

/**
 * \brief DIMM characteristics
 *
 * The characteristics of each DIMM, as presented by the SPD
 */
struct dimm_attr_ddr4_st {
	enum spd_memory_type dram_type;
	enum spd_dimm_type_ddr4 dimm_type;
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
};

typedef u8 spd_ddr4_raw_data[SPD_SIZE_MAX_DDR4];

int spd_decode_ddr4(struct dimm_attr_ddr4_st *dimm, spd_ddr4_raw_data spd);

enum cb_err spd_add_smbios17_ddr4(const u8 channel, const u8 slot,
				const u16 selected_freq,
				const struct dimm_attr_ddr4_st *info);

/**
 * Converts DDR4 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t ddr4_speed_mhz_to_reported_mts(uint16_t speed_mhz);

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
 *	uint32_t addr;
 *	uint32_t mrs;
 *	chipset_enable_mrs_command_mode();
 *	mrs = ddr4_get_mr0(rtt_wr, srt, cas, asr, cwl)
 *	if (rank_has_mirrorred_pins)
 *		mrs = ddr4_mrs_mirror_pins(mrs);
 *	addr = chipset_specific_get_mrs_addr(mrs);
 *	volatile_read(addr);
 * @endcode
 *
 * The MRS representation has the following structure:
 *	- cmd[17:0] = Address pins A[13:0]
 *	- cmd[21:20] = Bank address BA[1:0]
 *	- cmd[23:22] = Bank group BG[1:0]
 *
 * Address pins A[16:14] are always low for MRS commands. A17 is reserved for
 * future use, cmd[19:18] is left as a placeholder in case it is needed.
 */

/* Swap A3<->A4, A5<->A6, A7<->A8, A11<->A13, BA0<->BA1, BG0<->BG1 */
static inline uint32_t ddr4_mrs_mirror_pins(uint32_t mrs_cmd)
{
	mrs_cmd = (mrs_cmd & 0x5000A8) << 1 |
		  (mrs_cmd & 0xA00150) >> 1 |
		  (mrs_cmd & ~0xF001F8);
	mrs_cmd = (mrs_cmd & 0x000800) << 2 |
		  (mrs_cmd & 0x002000) >> 2 |
		  (mrs_cmd & ~0x002800);
	return mrs_cmd;
}

enum ddr4_mr0_mode {
	DDR4_MR0_MODE_NORMAL = 0,
	DDR4_MR0_MODE_TEST =   1,
};
enum ddr4_mr0_dll_reset {
	DDR4_MR0_DLL_RESET_NO =  0,
	DDR4_MR0_DLL_RESET_YES = 1,
};
enum ddr4_mr0_burst_type {
	DDR4_MR0_BURST_TYPE_SEQUENTIAL =  0,
	DDR4_MR0_BURST_TYPE_INTERLEAVED = 1,
};
enum ddr4_mr0_burst_length {
	DDR4_MR0_BURST_LENGTH_FIXED_8    = 0,
	DDR4_MR0_BURST_LENGTH_ON_THE_FLY = 1,
	DDR4_MR0_BURST_LENGTH_FIXED_4    = 2,
};

/* Returns MRS command */
uint32_t ddr4_get_mr0(u8 write_recovery,
		      enum ddr4_mr0_dll_reset dll_reset,
		      u8 cas,
		      enum ddr4_mr0_burst_type burst_type,
		      enum ddr4_mr0_burst_length burst_length);

enum ddr4_mr1_qoff {
	DDR4_MR1_QOFF_ENABLE =  0,
	DDR4_MR1_QOFF_DISABLE = 1,
};
enum ddr4_mr1_tdqs {
	DDR4_MR1_TDQS_DISABLE = 0,
	DDR4_MR1_TDQS_ENABLE =  1,
};
enum ddr4_mr1_rtt_nom {
	DDR4_MR1_RTT_NOM_OFF =   0,
	DDR4_MR1_RTT_NOM_RZQ_4 = 1,
	DDR4_MR1_RTT_NOM_RZQ_2 = 2,
	DDR4_MR1_RTT_NOM_RZQ_6 = 3,
	DDR4_MR1_RTT_NOM_RZQ_1 = 4,
	DDR4_MR1_RTT_NOM_RZQ_5 = 5,
	DDR4_MR1_RTT_NOM_RZQ_3 = 6,
	DDR4_MR1_RTT_NOM_RZQ_7 = 7,
};
enum ddr4_mr1_write_leveling {
	DDR4_MR1_WRLVL_DISABLE = 0,
	DDR4_MR1_WRLVL_ENABLE =  1,
};
enum ddr4_mr1_additive_latency {
	DDR4_MR1_AL_DISABLE =    0,
	DDR4_MR1_AL_CL_MINUS_1 = 1,
	DDR4_MR1_AL_CL_MINUS_2 = 2,
};
enum ddr4_mr1_odimp {
	DDR4_MR1_ODIMP_RZQ_7 = 0,
	DDR4_MR1_ODIMP_RZQ_5 = 1,
};
enum ddr4_mr1_dll {
	DDR4_MR1_DLL_DISABLE = 0,
	DDR4_MR1_DLL_ENABLE =  1,
};

/* Returns MRS command */
uint32_t ddr4_get_mr1(enum ddr4_mr1_qoff qoff,
		      enum ddr4_mr1_tdqs tdqs,
		      enum ddr4_mr1_rtt_nom rtt_nom,
		      enum ddr4_mr1_write_leveling write_leveling,
		      enum ddr4_mr1_odimp output_drive_impedance,
		      enum ddr4_mr1_additive_latency additive_latency,
		      enum ddr4_mr1_dll dll_enable);

enum ddr4_mr2_wr_crc {
	DDR4_MR2_WR_CRC_DISABLE = 0,
	DDR4_MR2_WR_CRC_ENABLE =  1,
};
enum ddr4_mr2_rtt_wr {
	DDR4_MR2_RTT_WR_OFF =   0,
	DDR4_MR2_RTT_WR_RZQ_2 = 1,
	DDR4_MR2_RTT_WR_RZQ_1 = 2,
	DDR4_MR2_RTT_WR_HI_Z =  3,
	DDR4_MR2_RTT_WR_RZQ_3 = 4,
};
enum ddr4_mr2_lp_asr {
	DDR4_MR2_ASR_MANUAL_NORMAL_RANGE =   0,
	DDR4_MR2_ASR_MANUAL_REDUCED_RANGE =  1,
	DDR4_MR2_ASR_MANUAL_EXTENDED_RANGE = 2,
	DDR4_MR2_ASR_AUTO =                  3,
};

/* Returns MRS command */
uint32_t ddr4_get_mr2(enum ddr4_mr2_wr_crc wr_crc,
		      enum ddr4_mr2_rtt_wr rtt_wr,
		      enum ddr4_mr2_lp_asr self_refresh, u8 cwl);

enum ddr4_mr3_mpr_read_format {
	DDR4_MR3_MPR_SERIAL =    0,
	DDR4_MR3_MPR_PARALLEL =  1,
	DDR4_MR3_MPR_STAGGERED = 2,
};
enum ddr4_mr3_wr_cmd_lat_crc_dm {
	DDR4_MR3_CRC_DM_4 = 0,
	DDR4_MR3_CRC_DM_5 = 1,
	DDR4_MR3_CRC_DM_6 = 2,
};
enum ddr4_mr3_fine_gran_ref {
	DDR4_MR3_FINE_GRAN_REF_NORMAL       = 0,
	DDR4_MR3_FINE_GRAN_REF_FIXED_2      = 1,
	DDR4_MR3_FINE_GRAN_REF_FIXED_4      = 2,
	/* Two reserved values */
	DDR4_MR3_FINE_GRAN_REF_ON_THE_FLY_2 = 5,
	DDR4_MR3_FINE_GRAN_REF_ON_THE_FLY_4 = 6,
};
enum ddr4_mr3_temp_sensor_readout {
	DDR4_MR3_TEMP_SENSOR_DISABLE = 0,
	DDR4_MR3_TEMP_SENSOR_ENABLE =  1,
};
enum ddr4_mr3_pda {
	DDR4_MR3_PDA_DISABLE = 0,
	DDR4_MR3_PDA_ENABLE =  1,
};
enum ddr4_mr3_geardown_mode {
	DDR4_MR3_GEARDOWN_1_2_RATE = 0,
	DDR4_MR3_GEARDOWN_1_4_RATE = 1,
};
enum ddr4_mr3_mpr_operation {
	DDR4_MR3_MPR_NORMAL = 0,
	DDR4_MR3_MPR_MPR =    1,
};

/* Returns MRS command */
uint32_t ddr4_get_mr3(enum ddr4_mr3_mpr_read_format mpr_read_format,
		      enum ddr4_mr3_wr_cmd_lat_crc_dm command_latency_crc_dm,
		      enum ddr4_mr3_fine_gran_ref fine_refresh,
		      enum ddr4_mr3_temp_sensor_readout temp_sensor,
		      enum ddr4_mr3_pda pda,
		      enum ddr4_mr3_geardown_mode geardown,
		      enum ddr4_mr3_mpr_operation mpr_operation,
		      u8 mpr_page);

enum ddr4_mr4_hppr {
	DDR4_MR4_HPPR_DISABLE = 0,
	DDR4_MR4_HPPR_ENABLE =  1,
};
enum ddr4_mr4_wr_preamble {
	DDR4_MR4_WR_PREAMBLE_1 = 0,
	DDR4_MR4_WR_PREAMBLE_2 = 1,
};
enum ddr4_mr4_rd_preamble {
	DDR4_MR4_RD_PREAMBLE_1 = 0,
	DDR4_MR4_RD_PREAMBLE_2 = 1,
};
enum ddr4_mr4_rd_preamble_training {
	DDR4_MR4_RD_PREAMBLE_TRAINING_DISABLE = 0,
	DDR4_MR4_RD_PREAMBLE_TRAINING_ENABLE =  1,
};
enum ddr4_mr4_self_refr_abort {
	DDR4_MR4_SELF_REFRESH_ABORT_DISABLE = 0,
	DDR4_MR4_SELF_REFRESH_ABORT_ENABLE =  1,
};
enum ddr4_mr4_cs_to_cmd_latency {
	DDR4_MR4_CS_TO_CMD_LAT_DISABLE = 0,
	DDR4_MR4_CS_TO_CMD_LAT_3 =       1,
	DDR4_MR4_CS_TO_CMD_LAT_4 =       2,
	DDR4_MR4_CS_TO_CMD_LAT_5 =       3,
	DDR4_MR4_CS_TO_CMD_LAT_6 =       4,
	DDR4_MR4_CS_TO_CMD_LAT_8 =       5,
};
enum ddr4_mr4_sppr {
	DDR4_MR4_SPPR_DISABLE = 0,
	DDR4_MR4_SPPR_ENABLE =  1,
};
enum ddr4_mr4_internal_vref_mon {
	DDR4_MR4_INTERNAL_VREF_MON_DISABLE = 0,
	DDR4_MR4_INTERNAL_VREF_MON_ENABLE =  1,
};
enum ddr4_mr4_temp_controlled_refr {
	DDR4_MR4_TEMP_CONTROLLED_REFR_DISABLE =  0,
	DDR4_MR4_TEMP_CONTROLLED_REFR_NORMAL =   2,
	DDR4_MR4_TEMP_CONTROLLED_REFR_EXTENDED = 3,
};
enum ddr4_mr4_max_pd_mode {
	DDR4_MR4_MAX_PD_MODE_DISABLE = 0,
	DDR4_MR4_MAX_PD_MODE_ENABLE =  1,
};

/* Returns MRS command */
uint32_t ddr4_get_mr4(enum ddr4_mr4_hppr hppr,
		      enum ddr4_mr4_wr_preamble wr_preamble,
		      enum ddr4_mr4_rd_preamble rd_preamble,
		      enum ddr4_mr4_rd_preamble_training rd_preamble_train,
		      enum ddr4_mr4_self_refr_abort self_ref_abrt,
		      enum ddr4_mr4_cs_to_cmd_latency cs2cmd_lat,
		      enum ddr4_mr4_sppr sppr,
		      enum ddr4_mr4_internal_vref_mon int_vref_mon,
		      enum ddr4_mr4_temp_controlled_refr temp_ctrl_ref,
		      enum ddr4_mr4_max_pd_mode max_pd);

enum ddr4_mr5_rd_dbi {
	DDR4_MR5_RD_DBI_DISABLE = 0,
	DDR4_MR5_RD_DBI_ENABLE =  1,
};
enum ddr4_mr5_wr_dbi {
	DDR4_MR5_WR_DBI_DISABLE = 0,
	DDR4_MR5_WR_DBI_ENABLE =  1,
};
enum ddr4_mr5_data_mask {
	DDR4_MR5_DATA_MASK_DISABLE = 0,
	DDR4_MR5_DATA_MASK_ENABLE =  1,
};
enum ddr4_mr5_rtt_park {
	DDR4_MR5_RTT_PARK_OFF =   0,
	DDR4_MR5_RTT_PARK_RZQ_4 = 1,
	DDR4_MR5_RTT_PARK_RZQ_2 = 2,
	DDR4_MR5_RTT_PARK_RZQ_6 = 3,
	DDR4_MR5_RTT_PARK_RZQ_1 = 4,
	DDR4_MR5_RTT_PARK_RZQ_5 = 5,
	DDR4_MR5_RTT_PARK_RZQ_3 = 6,
	DDR4_MR5_RTT_PARK_RZQ_7 = 7,
};
enum ddr4_mr5_odt_pd {
	DDR4_MR5_ODT_PD_ACTIVADED =   0,
	DDR4_MR5_ODT_PD_DEACTIVADED = 1,
};
enum ddr4_mr5_ca_parity_lat {
	DDR4_MR5_CA_PARITY_LAT_DISABLE = 0,
	DDR4_MR5_CA_PARITY_LAT_4 =       1, /* 1600-2133 MT/s */
	DDR4_MR5_CA_PARITY_LAT_5 =       2, /* 2400-2666 MT/s */
	DDR4_MR5_CA_PARITY_LAT_6 =       3, /* 2933-3200 MT/s */
	DDR4_MR5_CA_PARITY_LAT_8 =       4, /* RFU */
};

/* Returns MRS command */
uint32_t ddr4_get_mr5(enum ddr4_mr5_rd_dbi rd_dbi,
		      enum ddr4_mr5_wr_dbi wr_dbi,
		      enum ddr4_mr5_data_mask dm,
		      enum ddr4_mr5_rtt_park rtt_park,
		      enum ddr4_mr5_odt_pd odt_pd,
		      enum ddr4_mr5_ca_parity_lat pl);

enum ddr4_mr6_vrefdq_training {
	DDR4_MR6_VREFDQ_TRAINING_DISABLE = 0,
	DDR4_MR6_VREFDQ_TRAINING_ENABLE =  1,
};
enum ddr4_mr6_vrefdq_training_range {
	DDR4_MR6_VREFDQ_TRAINING_RANGE_1 = 0, /* 60% to 92.50% in 0.65% steps */
	DDR4_MR6_VREFDQ_TRAINING_RANGE_2 = 1, /* 40% to 77.50% in 0.65% steps */
};

/* Returns MRS command */
uint32_t ddr4_get_mr6(u8 tccd_l,
		      enum ddr4_mr6_vrefdq_training vrefdq_training,
		      enum ddr4_mr6_vrefdq_training_range range,
		      u8 vrefdq_value);

enum ddr4_zqcal_ls {
	DDR4_ZQCAL_SHORT = 0,
	DDR4_ZQCAL_LONG =  1,
};

/* Returns MRS command */
uint32_t ddr4_get_zqcal_cmd(enum ddr4_zqcal_ls long_short);

#endif /* DEVICE_DRAM_DDR4L_H */
