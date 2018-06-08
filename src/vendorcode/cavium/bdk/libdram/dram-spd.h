/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/

/**
 * Functions, enumarations, and structures related to DIMM SPDs.
 * Everything in this file is internal to libdram.
 */

/* data field addresses in the DDR3 SPD eeprom */
typedef enum ddr3_spd_addrs {
    DDR3_SPD_BYTES_PROGRAMMED				=  0,
    DDR3_SPD_REVISION					=  1,
    DDR3_SPD_KEY_BYTE_DEVICE_TYPE			=  2,
    DDR3_SPD_KEY_BYTE_MODULE_TYPE			=  3,
    DDR3_SPD_DENSITY_BANKS				=  4,
    DDR3_SPD_ADDRESSING_ROW_COL_BITS			=  5,
    DDR3_SPD_NOMINAL_VOLTAGE				=  6,
    DDR3_SPD_MODULE_ORGANIZATION			=  7,
    DDR3_SPD_MEMORY_BUS_WIDTH				=  8,
    DDR3_SPD_FINE_TIMEBASE_DIVIDEND_DIVISOR		=  9,
    DDR3_SPD_MEDIUM_TIMEBASE_DIVIDEND			= 10,
    DDR3_SPD_MEDIUM_TIMEBASE_DIVISOR			= 11,
    DDR3_SPD_MINIMUM_CYCLE_TIME_TCKMIN			= 12,
    DDR3_SPD_CAS_LATENCIES_LSB				= 14,
    DDR3_SPD_CAS_LATENCIES_MSB				= 15,
    DDR3_SPD_MIN_CAS_LATENCY_TAAMIN			= 16,
    DDR3_SPD_MIN_WRITE_RECOVERY_TWRMIN			= 17,
    DDR3_SPD_MIN_RAS_CAS_DELAY_TRCDMIN			= 18,
    DDR3_SPD_MIN_ROW_ACTIVE_DELAY_TRRDMIN		= 19,
    DDR3_SPD_MIN_ROW_PRECHARGE_DELAY_TRPMIN		= 20,
    DDR3_SPD_UPPER_NIBBLES_TRAS_TRC			= 21,
    DDR3_SPD_MIN_ACTIVE_PRECHARGE_LSB_TRASMIN		= 22,
    DDR3_SPD_MIN_ACTIVE_REFRESH_LSB_TRCMIN		= 23,
    DDR3_SPD_MIN_REFRESH_RECOVERY_LSB_TRFCMIN		= 24,
    DDR3_SPD_MIN_REFRESH_RECOVERY_MSB_TRFCMIN           = 25,
    DDR3_SPD_MIN_INTERNAL_WRITE_READ_CMD_TWTRMIN        = 26,
    DDR3_SPD_MIN_INTERNAL_READ_PRECHARGE_CMD_TRTPMIN    = 27,
    DDR3_SPD_UPPER_NIBBLE_TFAW                          = 28,
    DDR3_SPD_MIN_FOUR_ACTIVE_WINDOW_TFAWMIN             = 29,
    DDR3_SPD_MINIMUM_CYCLE_TIME_FINE_TCKMIN		= 34,
    DDR3_SPD_MIN_CAS_LATENCY_FINE_TAAMIN		= 35,
    DDR3_SPD_MIN_RAS_CAS_DELAY_FINE_TRCDMIN		= 36,
    DDR3_SPD_MIN_ROW_PRECHARGE_DELAY_FINE_TRPMIN	= 37,
    DDR3_SPD_MIN_ACTIVE_REFRESH_LSB_FINE_TRCMIN		= 38,
    DDR3_SPD_ADDRESS_MAPPING                            = 63,
    DDR3_SPD_MODULE_SERIAL_NUMBER                       = 122,
    DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_LOWER_NIBBLE      = 126,
    DDR3_SPD_CYCLICAL_REDUNDANCY_CODE_UPPER_NIBBLE      = 127,
    DDR3_SPD_MODULE_PART_NUMBER                         = 128
} ddr3_spd_addr_t;

/* data field addresses in the DDR4 SPD eeprom */
typedef enum ddr4_spd_addrs {
    DDR4_SPD_BYTES_PROGRAMMED				=  0,
    DDR4_SPD_REVISION					=  1,
    DDR4_SPD_KEY_BYTE_DEVICE_TYPE			=  2,
    DDR4_SPD_KEY_BYTE_MODULE_TYPE			=  3,
    DDR4_SPD_DENSITY_BANKS				=  4,
    DDR4_SPD_ADDRESSING_ROW_COL_BITS			=  5,
    DDR4_SPD_PACKAGE_TYPE				=  6,
    DDR4_SPD_OPTIONAL_FEATURES				=  7,
    DDR4_SPD_THERMAL_REFRESH_OPTIONS			=  8,
    DDR4_SPD_OTHER_OPTIONAL_FEATURES			=  9,
    DDR4_SPD_SECONDARY_PACKAGE_TYPE			= 10,
    DDR4_SPD_MODULE_NOMINAL_VOLTAGE			= 11,
    DDR4_SPD_MODULE_ORGANIZATION			= 12,
    DDR4_SPD_MODULE_MEMORY_BUS_WIDTH			= 13,
    DDR4_SPD_MODULE_THERMAL_SENSOR			= 14,
    DDR4_SPD_RESERVED_BYTE15				= 15,
    DDR4_SPD_RESERVED_BYTE16				= 16,
    DDR4_SPD_TIMEBASES					= 17,
    DDR4_SPD_MINIMUM_CYCLE_TIME_TCKAVGMIN		= 18,
    DDR4_SPD_MAXIMUM_CYCLE_TIME_TCKAVGMAX		= 19,
    DDR4_SPD_CAS_LATENCIES_BYTE0			= 20,
    DDR4_SPD_CAS_LATENCIES_BYTE1			= 21,
    DDR4_SPD_CAS_LATENCIES_BYTE2			= 22,
    DDR4_SPD_CAS_LATENCIES_BYTE3			= 23,
    DDR4_SPD_MIN_CAS_LATENCY_TAAMIN			= 24,
    DDR4_SPD_MIN_RAS_CAS_DELAY_TRCDMIN			= 25,
    DDR4_SPD_MIN_ROW_PRECHARGE_DELAY_TRPMIN		= 26,
    DDR4_SPD_UPPER_NIBBLES_TRAS_TRC			= 27,
    DDR4_SPD_MIN_ACTIVE_PRECHARGE_LSB_TRASMIN		= 28,
    DDR4_SPD_MIN_ACTIVE_REFRESH_LSB_TRCMIN		= 29,
    DDR4_SPD_MIN_REFRESH_RECOVERY_LSB_TRFC1MIN		= 30,
    DDR4_SPD_MIN_REFRESH_RECOVERY_MSB_TRFC1MIN          = 31,
    DDR4_SPD_MIN_REFRESH_RECOVERY_LSB_TRFC2MIN		= 32,
    DDR4_SPD_MIN_REFRESH_RECOVERY_MSB_TRFC2MIN          = 33,
    DDR4_SPD_MIN_REFRESH_RECOVERY_LSB_TRFC4MIN		= 34,
    DDR4_SPD_MIN_REFRESH_RECOVERY_MSB_TRFC4MIN          = 35,
    DDR4_SPD_MIN_FOUR_ACTIVE_WINDOW_MSN_TFAWMIN         = 36,
    DDR4_SPD_MIN_FOUR_ACTIVE_WINDOW_LSB_TFAWMIN         = 37,
    DDR4_SPD_MIN_ROW_ACTIVE_DELAY_SAME_TRRD_SMIN	= 38,
    DDR4_SPD_MIN_ROW_ACTIVE_DELAY_DIFF_TRRD_LMIN	= 39,
    DDR4_SPD_MIN_CAS_TO_CAS_DELAY_TCCD_LMIN		= 40,
    DDR4_SPD_MIN_CAS_TO_CAS_DELAY_FINE_TCCD_LMIN	= 117,
    DDR4_SPD_MIN_ACT_TO_ACT_DELAY_SAME_FINE_TRRD_LMIN	= 118,
    DDR4_SPD_MIN_ACT_TO_ACT_DELAY_DIFF_FINE_TRRD_SMIN	= 119,
    DDR4_SPD_MIN_ACT_TO_ACT_REFRESH_DELAY_FINE_TRCMIN	= 120,
    DDR4_SPD_MIN_ROW_PRECHARGE_DELAY_FINE_TRPMIN	= 121,
    DDR4_SPD_MIN_RAS_TO_CAS_DELAY_FINE_TRCDMIN		= 122,
    DDR4_SPD_MIN_CAS_LATENCY_FINE_TAAMIN		= 123,
    DDR4_SPD_MAX_CYCLE_TIME_FINE_TCKAVGMAX		= 124,
    DDR4_SPD_MIN_CYCLE_TIME_FINE_TCKAVGMIN		= 125,
    DDR4_SPD_CYCLICAL_REDUNDANCY_CODE_LOWER_NIBBLE      = 126,
    DDR4_SPD_CYCLICAL_REDUNDANCY_CODE_UPPER_NIBBLE      = 127,
    DDR4_SPD_REFERENCE_RAW_CARD				= 130,
    DDR4_SPD_UDIMM_ADDR_MAPPING_FROM_EDGE		= 131,
    DDR4_SPD_REGISTER_MANUFACTURER_ID_LSB		= 133,
    DDR4_SPD_REGISTER_MANUFACTURER_ID_MSB		= 134,
    DDR4_SPD_REGISTER_REVISION_NUMBER			= 135,
    DDR4_SPD_RDIMM_ADDR_MAPPING_FROM_REGISTER_TO_DRAM   = 136,
    DDR4_SPD_RDIMM_REGISTER_DRIVE_STRENGTH_CTL          = 137,
    DDR4_SPD_RDIMM_REGISTER_DRIVE_STRENGTH_CK           = 138,
    DDR4_SPD_MODULE_SERIAL_NUMBER			= 325,
    DDR4_SPD_MODULE_PART_NUMBER				= 329
} ddr4_spd_addr_t;

extern int read_entire_spd(bdk_node_t node, dram_config_t *cfg, int lmc, int dimm);
extern int read_spd(bdk_node_t node, const dimm_config_t *dimm_config, int spd_field);

extern int validate_dimm(bdk_node_t node, const dimm_config_t *dimm_config);

extern void report_dimm(bdk_node_t node, const dimm_config_t *dimm_config,
                        int dimm, int ddr_interface_num, int num_ranks,
                        int dram_width, int dimm_size_mb);

extern int dram_get_default_spd_speed(bdk_node_t node, const ddr_configuration_t *ddr_config);

extern const char *ddr3_dimm_types[];
extern const char *ddr4_dimm_types[];
