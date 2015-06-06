/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef __SOC_BROADCOM_CYGNUS_YDC_DDR_BIST_H__
#define __SOC_BROADCOM_CYGNUS_YDC_DDR_BIST_H__

#include <delay.h>
#include <soc/cygnus_types.h>

#ifndef YDC_DDR_BIST_REG_BASE
#define YDC_DDR_BIST_REG_BASE                       0x18010C00
#endif

#ifndef YDC_DDR_BIST_PHY_BITWITDH_IS_32
#define YDC_DDR_BIST_PHY_BITWITDH_IS_32             1
#endif

#define YDC_DDR_BIST_POLL_INTERVAL_US               10
#define YDC_DDR_BIST_POLL_COUNT_LIMIT               100000

#define SOC_E_NONE                                  0x0
#define SOC_E_FAIL                                  0x1
#define SOC_E_TIMEOUT                               0x2
#define SOC_E_MEMORY                                0x4

typedef struct ydc_ddr_bist_info_s {
	uint32 write_weight;
	uint32 read_weight;
	uint32 bist_timer_us;		/* if bist_timer_us != 0, ignore bist_num_actions and wait bist_timer_us before stoping bist */
	uint32 bist_num_actions;
	uint32 bist_start_address;
	uint32 bist_end_address;
	uint32 prbs_mode;
	uint32 mpr_mode;
} ydc_ddr_bist_info_t;

typedef struct ydc_ddr_bist_err_cnt_s {
	uint32 bist_err_occur;
	uint32 bist_full_err_cnt;
	uint32 bist_single_err_cnt;
	uint32 bist_global_err_cnt;
} ydc_ddr_bist_err_cnt_t;

extern int soc_ydc_ddr_bist_config_set(int unit, int phy_ndx,
	ydc_ddr_bist_info_t *bist_info);
extern int soc_ydc_ddr_bist_run(int unit, int phy_ndx,
	ydc_ddr_bist_err_cnt_t *error_count);

/**
 * m = memory, c = core, r = register, f = field, d = data.
 */
#if !defined(GET_FIELD) && !defined(SET_FIELD)
#define BRCM_ALIGN(c, r, f)   c##_##r##_##f##_ALIGN
#define BRCM_BITS(c, r, f)    c##_##r##_##f##_BITS
#define BRCM_MASK(c, r, f)    c##_##r##_##f##_MASK
#define BRCM_SHIFT(c, r, f)   c##_##r##_##f##_SHIFT

#define GET_FIELD(m, c, r, f) \
	((((m) & BRCM_MASK(c, r, f)) >> BRCM_SHIFT(c, r, f)) << BRCM_ALIGN(c, r, f))

#define SET_FIELD(m, c, r, f, d) \
	((m) = (((m) & ~BRCM_MASK(c, r, f)) | ((((d) >> BRCM_ALIGN(c, r, f)) << \
	 BRCM_SHIFT(c, r, f)) & BRCM_MASK(c, r, f))) \
	)

#define SET_TYPE_FIELD(m, c, r, f, d) SET_FIELD(m, c, r, f, c##_##d)
#define SET_NAME_FIELD(m, c, r, f, d) SET_FIELD(m, c, r, f, c##_##r##_##f##_##d)
#define SET_VALUE_FIELD(m, c, r, f, d) SET_FIELD(m, c, r, f, d)

#endif /* GET & SET */

#define YDC_DDR_BIST_REG_READ(_unit, _pc, flags, _reg_addr, _val) \
	(*(uint32 *)_val = REGRD((_pc) + (_reg_addr)))
#define YDC_DDR_BIST_REG_WRITE(_unit, _pc, _flags, _reg_addr, _val) \
	REGWR((_pc) + (_reg_addr), (_val))
#define YDC_DDR_BIST_REG_MODIFY(_unit, _pc, _flags, _reg_addr, _val, _mask) \
	REGWR((_pc) + (_reg_addr), (REGRD((_pc) + (_reg_addr)) & ~(_mask)) | ((_val) & (_mask)))
#define YDC_DDR_BIST_GET_FIELD(m, c, r, f) \
	GET_FIELD(m, c, r, f)
#define YDC_DDR_BIST_SET_FIELD(m, c, r, f, d) \
	SET_FIELD(m, c, r, f, d)

/****************************************************************************
 * Core Enums.
 ***************************************************************************/
#define YDC_DDR_BIST_CONFIG                                         0x00000000
#define YDC_DDR_BIST_CONFIG_2                                       0x00000004
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS                         0x00000008
#define YDC_DDR_BIST_CONFIGURATIONS                                 0x0000000C
#define YDC_DDR_BIST_NUMBER_OF_ACTIONS                              0x00000010
#define YDC_DDR_BIST_START_ADDRESS                                  0x00000014
#define YDC_DDR_BIST_END_ADDRESS                                    0x00000018
#define YDC_DDR_BIST_SINGLE_BIT_MASK                                0x0000001C
#define YDC_DDR_BIST_PATTERN_WORD_7                                 0x00000020
#define YDC_DDR_BIST_PATTERN_WORD_6                                 0x00000024
#define YDC_DDR_BIST_PATTERN_WORD_5                                 0x00000028
#define YDC_DDR_BIST_PATTERN_WORD_4                                 0x0000002C
#define YDC_DDR_BIST_PATTERN_WORD_3                                 0x00000030
#define YDC_DDR_BIST_PATTERN_WORD_2                                 0x00000034
#define YDC_DDR_BIST_PATTERN_WORD_1                                 0x00000038
#define YDC_DDR_BIST_PATTERN_WORD_0                                 0x0000003C
#define YDC_DDR_BIST_FULL_MASK_WORD_7                               0x00000040
#define YDC_DDR_BIST_FULL_MASK_WORD_6                               0x00000044
#define YDC_DDR_BIST_FULL_MASK_WORD_5                               0x00000048
#define YDC_DDR_BIST_FULL_MASK_WORD_4                               0x0000004C
#define YDC_DDR_BIST_FULL_MASK_WORD_3                               0x00000050
#define YDC_DDR_BIST_FULL_MASK_WORD_2                               0x00000054
#define YDC_DDR_BIST_FULL_MASK_WORD_1                               0x00000058
#define YDC_DDR_BIST_FULL_MASK_WORD_0                               0x0000005C
#define YDC_DDR_BIST_STATUSES                                       0x00000060
#define YDC_DDR_BIST_FULL_MASK_ERROR_COUNTER                        0x00000064
#define YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTER                  0x00000068
#define YDC_DDR_BIST_ERROR_OCCURRED                                 0x0000006C
#define YDC_DDR_BIST_GLOBAL_ERROR_COUNTER                           0x00000070
#define YDC_DDR_BIST_LAST_ADDR_ERR                                  0x00000074
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_7                           0x00000078
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_6                           0x0000007C
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_5                           0x00000080
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_4                           0x00000084
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_3                           0x00000088
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_2                           0x0000008C
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_1                           0x00000090
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_0                           0x00000094

/****************************************************************************
 * YDC_DDR_BIST_YDC_DDR_BIST
 ***************************************************************************/

/* BIST Configuration Register */
#define READ_YDC_DDR_BIST_CONFIGr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000000, (_val))
#define WRITE_YDC_DDR_BIST_CONFIGr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000000, (_val))
#define MODIFY_YDC_DDR_BIST_CONFIGr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000000, (_val), (_mask))

/* BIST Configuration Register 2 */
#define READ_YDC_DDR_BIST_CONFIG_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000004, (_val))
#define WRITE_YDC_DDR_BIST_CONFIG_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000004, (_val))
#define MODIFY_YDC_DDR_BIST_CONFIG_2r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000004, (_val), (_mask))

/* BIST General Configurations Register */
#define READ_YDC_DDR_BIST_GENERAL_CONFIGURATIONSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000008, (_val))
#define WRITE_YDC_DDR_BIST_GENERAL_CONFIGURATIONSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000008, (_val))
#define MODIFY_YDC_DDR_BIST_GENERAL_CONFIGURATIONSr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000008, (_val), (_mask))

/* BIST Configurations Register */
#define READ_YDC_DDR_BIST_CONFIGURATIONSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000000c, (_val))
#define WRITE_YDC_DDR_BIST_CONFIGURATIONSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000000c, (_val))
#define MODIFY_YDC_DDR_BIST_CONFIGURATIONSr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000000c, (_val), (_mask))

/* BIST Number of Actions Register */
#define READ_YDC_DDR_BIST_NUMBER_OF_ACTIONSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000010, (_val))
#define WRITE_YDC_DDR_BIST_NUMBER_OF_ACTIONSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000010, (_val))
#define MODIFY_YDC_DDR_BIST_NUMBER_OF_ACTIONSr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000010, (_val), (_mask))

/* BIST Start Address Register */
#define READ_YDC_DDR_BIST_START_ADDRESSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000014, (_val))
#define WRITE_YDC_DDR_BIST_START_ADDRESSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000014, (_val))
#define MODIFY_YDC_DDR_BIST_START_ADDRESSr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000014, (_val), (_mask))

/* BIST End Address Register */
#define READ_YDC_DDR_BIST_END_ADDRESSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000018, (_val))
#define WRITE_YDC_DDR_BIST_END_ADDRESSr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000018, (_val))
#define MODIFY_YDC_DDR_BIST_END_ADDRESSr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000018, (_val), (_mask))

/* BIST Single Bit Mask Register */
#define READ_YDC_DDR_BIST_SINGLE_BIT_MASKr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000001c, (_val))
#define WRITE_YDC_DDR_BIST_SINGLE_BIT_MASKr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000001c, (_val))
#define MODIFY_YDC_DDR_BIST_SINGLE_BIT_MASKr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000001c, (_val), (_mask))

/* BIST Pattern Word 7 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_7r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000020, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_7r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000020, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_7r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000020, (_val), (_mask))

/* BIST Pattern Word 6 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_6r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000024, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_6r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000024, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_6r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000024, (_val), (_mask))

/* BIST Pattern Word 5 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_5r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000028, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_5r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000028, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_5r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000028, (_val), (_mask))

/* BIST Pattern Word 4 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_4r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000002c, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_4r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000002c, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_4r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000002c, (_val), (_mask))

/* BIST Pattern Word 3 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_3r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000030, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_3r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000030, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_3r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000030, (_val), (_mask))

/* BIST Pattern Word 2 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000034, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000034, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_2r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000034, (_val), (_mask))

/* BIST Pattern Word 1 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_1r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000038, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_1r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000038, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_1r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000038, (_val), (_mask))

/* BIST Pattern Word 0 Register */
#define READ_YDC_DDR_BIST_PATTERN_WORD_0r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000003c, (_val))
#define WRITE_YDC_DDR_BIST_PATTERN_WORD_0r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000003c, (_val))
#define MODIFY_YDC_DDR_BIST_PATTERN_WORD_0r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000003c, (_val), (_mask))

/* BIST Full Mask Word 7 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_7r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000040, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_7r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000040, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_7r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000040, (_val), (_mask))

/* BIST Full Mask Word 6 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_6r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000044, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_6r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000044, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_6r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000044, (_val), (_mask))

/* BIST Full Mask Word 5 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_5r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000048, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_5r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000048, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_5r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000048, (_val), (_mask))

/* BIST Full Mask Word 4 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_4r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000004c, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_4r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000004c, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_4r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000004c, (_val), (_mask))

/* BIST Full Mask Word 3 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_3r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000050, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_3r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000050, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_3r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000050, (_val), (_mask))

/* BIST Full Mask Word 2 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000054, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000054, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_2r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000054, (_val), (_mask))

/* BIST Full Mask Word 1 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_1r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000058, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_1r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000058, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_1r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000058, (_val), (_mask))

/* BIST Full Mask Word 0 Register */
#define READ_YDC_DDR_BIST_FULL_MASK_WORD_0r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000005c, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_WORD_0r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000005c, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_WORD_0r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000005c, (_val), (_mask))

/* BIST Status Register */
#define READ_YDC_DDR_BIST_STATUSESr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000060, (_val))
#define WRITE_YDC_DDR_BIST_STATUSESr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000060, (_val))
#define MODIFY_YDC_DDR_BIST_STATUSESr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000060, (_val), (_mask))

/* BIST Full Mask Error Counter Register */
#define READ_YDC_DDR_BIST_FULL_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000064, (_val))
#define WRITE_YDC_DDR_BIST_FULL_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000064, (_val))
#define MODIFY_YDC_DDR_BIST_FULL_MASK_ERROR_COUNTERr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000064, (_val), (_mask))

/* BIST Single Bit Mask Error Counter Register */
#define READ_YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000068, (_val))
#define WRITE_YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000068, (_val))
#define MODIFY_YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000068, (_val), (_mask))

/* BIST Error Occurred Register */
#define READ_YDC_DDR_BIST_ERROR_OCCURREDr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000006c, (_val))
#define WRITE_YDC_DDR_BIST_ERROR_OCCURREDr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000006c, (_val))
#define MODIFY_YDC_DDR_BIST_ERROR_OCCURREDr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000006c, (_val), (_mask))

/* BIST Global Error Counter Register */
#define READ_YDC_DDR_BIST_GLOBAL_ERROR_COUNTERr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000070, (_val))
#define WRITE_YDC_DDR_BIST_GLOBAL_ERROR_COUNTERr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000070, (_val))
#define MODIFY_YDC_DDR_BIST_GLOBAL_ERROR_COUNTERr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000070, (_val), (_mask))

/* BIST Last Error Address Register */
#define READ_YDC_DDR_BIST_LAST_ADDR_ERRr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000074, (_val))
#define WRITE_YDC_DDR_BIST_LAST_ADDR_ERRr(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000074, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_ADDR_ERRr(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000074, (_val), (_mask))

/* BIST Last Data Error Word 7 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_7r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000078, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_7r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000078, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_7r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000078, (_val), (_mask))

/* BIST Last Data Error Word 6 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_6r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000007c, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_6r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000007c, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_6r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000007c, (_val), (_mask))

/* BIST Last Data Error Word 5 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_5r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000080, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_5r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000080, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_5r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000080, (_val), (_mask))

/* BIST Last Data Error Word 4 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_4r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000084, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_4r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000084, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_4r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000084, (_val), (_mask))

/* BIST Last Data Error Word 3 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_3r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000088, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_3r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000088, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_3r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000088, (_val), (_mask))

/* BIST Last Data Error Word 2 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x0000008c, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_2r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x0000008c, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_2r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x0000008c, (_val), (_mask))

/* BIST Last Data Error Word 1 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_1r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000090, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_1r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000090, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_1r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000090, (_val), (_mask))

/* BIST Last Data Error Word 0 Register */
#define READ_YDC_DDR_BIST_LAST_DATA_ERR_WORD_0r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_READ((_unit), (_pc), 0x00, 0x00000094, (_val))
#define WRITE_YDC_DDR_BIST_LAST_DATA_ERR_WORD_0r(_unit, _pc, _val) \
	YDC_DDR_BIST_REG_WRITE((_unit), (_pc), 0x00, 0x00000094, (_val))
#define MODIFY_YDC_DDR_BIST_LAST_DATA_ERR_WORD_0r(_unit, _pc, _val, _mask) \
	YDC_DDR_BIST_REG_MODIFY((_unit), (_pc), 0x00, 0x00000094, (_val), (_mask))


/****************************************************************************
 * YDC_DDR_BIST_YDC_DDR_BIST
 ***************************************************************************/
/****************************************************************************
 * YDC_DDR_BIST :: CONFIG
 ***************************************************************************/
/* YDC_DDR_BIST :: CONFIG :: reserved0 [31:25] */
#define YDC_DDR_BIST_CONFIG_RESERVED0_MASK                         0xfe000000
#define YDC_DDR_BIST_CONFIG_RESERVED0_ALIGN                        0
#define YDC_DDR_BIST_CONFIG_RESERVED0_BITS                         7
#define YDC_DDR_BIST_CONFIG_RESERVED0_SHIFT                        25
#define YDC_DDR_BIST_CONFIG_RESERVED0_DEFAULT                      0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_FINISHED_PERIOD [24:17] */
#define YDC_DDR_BIST_CONFIG_BIST_FINISHED_PERIOD_MASK              0x01fe0000
#define YDC_DDR_BIST_CONFIG_BIST_FINISHED_PERIOD_ALIGN             0
#define YDC_DDR_BIST_CONFIG_BIST_FINISHED_PERIOD_BITS              8
#define YDC_DDR_BIST_CONFIG_BIST_FINISHED_PERIOD_SHIFT             17
#define YDC_DDR_BIST_CONFIG_BIST_FINISHED_PERIOD_DEFAULT           0x00000000

/* YDC_DDR_BIST :: CONFIG :: CLR_BIST_LAST_DATA_ERR [16:16] */
#define YDC_DDR_BIST_CONFIG_CLR_BIST_LAST_DATA_ERR_MASK            0x00010000
#define YDC_DDR_BIST_CONFIG_CLR_BIST_LAST_DATA_ERR_ALIGN           0
#define YDC_DDR_BIST_CONFIG_CLR_BIST_LAST_DATA_ERR_BITS            1
#define YDC_DDR_BIST_CONFIG_CLR_BIST_LAST_DATA_ERR_SHIFT           16
#define YDC_DDR_BIST_CONFIG_CLR_BIST_LAST_DATA_ERR_DEFAULT         0x00000000

/* YDC_DDR_BIST :: CONFIG :: BUS16_MODE [15:15] */
#define YDC_DDR_BIST_CONFIG_BUS16_MODE_MASK                        0x00008000
#define YDC_DDR_BIST_CONFIG_BUS16_MODE_ALIGN                       0
#define YDC_DDR_BIST_CONFIG_BUS16_MODE_BITS                        1
#define YDC_DDR_BIST_CONFIG_BUS16_MODE_SHIFT                       15
#define YDC_DDR_BIST_CONFIG_BUS16_MODE_DEFAULT                     0x00000000

/* YDC_DDR_BIST :: CONFIG :: ENABLE_8_BANKS_MODE [14:14] */
#define YDC_DDR_BIST_CONFIG_ENABLE_8_BANKS_MODE_MASK               0x00004000
#define YDC_DDR_BIST_CONFIG_ENABLE_8_BANKS_MODE_ALIGN              0
#define YDC_DDR_BIST_CONFIG_ENABLE_8_BANKS_MODE_BITS               1
#define YDC_DDR_BIST_CONFIG_ENABLE_8_BANKS_MODE_SHIFT              14
#define YDC_DDR_BIST_CONFIG_ENABLE_8_BANKS_MODE_DEFAULT            0x00000000

/* YDC_DDR_BIST :: CONFIG :: DISABLE_COL_BANK_SWAPPING [13:13] */
#define YDC_DDR_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_MASK         0x00002000
#define YDC_DDR_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_ALIGN        0
#define YDC_DDR_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_BITS         1
#define YDC_DDR_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_SHIFT        13
#define YDC_DDR_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_DEFAULT      0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_ARPRIORITY [12:10] */
#define YDC_DDR_BIST_CONFIG_BIST_ARPRIORITY_MASK                   0x00001c00
#define YDC_DDR_BIST_CONFIG_BIST_ARPRIORITY_ALIGN                  0
#define YDC_DDR_BIST_CONFIG_BIST_ARPRIORITY_BITS                   3
#define YDC_DDR_BIST_CONFIG_BIST_ARPRIORITY_SHIFT                  10
#define YDC_DDR_BIST_CONFIG_BIST_ARPRIORITY_DEFAULT                0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_ARAPCMD [09:09] */
#define YDC_DDR_BIST_CONFIG_BIST_ARAPCMD_MASK                      0x00000200
#define YDC_DDR_BIST_CONFIG_BIST_ARAPCMD_ALIGN                     0
#define YDC_DDR_BIST_CONFIG_BIST_ARAPCMD_BITS                      1
#define YDC_DDR_BIST_CONFIG_BIST_ARAPCMD_SHIFT                     9
#define YDC_DDR_BIST_CONFIG_BIST_ARAPCMD_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_AWUSER [08:08] */
#define YDC_DDR_BIST_CONFIG_BIST_AWUSER_MASK                       0x00000100
#define YDC_DDR_BIST_CONFIG_BIST_AWUSER_ALIGN                      0
#define YDC_DDR_BIST_CONFIG_BIST_AWUSER_BITS                       1
#define YDC_DDR_BIST_CONFIG_BIST_AWUSER_SHIFT                      8
#define YDC_DDR_BIST_CONFIG_BIST_AWUSER_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_AWPRIORITY [07:05] */
#define YDC_DDR_BIST_CONFIG_BIST_AWPRIORITY_MASK                   0x000000e0
#define YDC_DDR_BIST_CONFIG_BIST_AWPRIORITY_ALIGN                  0
#define YDC_DDR_BIST_CONFIG_BIST_AWPRIORITY_BITS                   3
#define YDC_DDR_BIST_CONFIG_BIST_AWPRIORITY_SHIFT                  5
#define YDC_DDR_BIST_CONFIG_BIST_AWPRIORITY_DEFAULT                0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_AWCOBUF [04:04] */
#define YDC_DDR_BIST_CONFIG_BIST_AWCOBUF_MASK                      0x00000010
#define YDC_DDR_BIST_CONFIG_BIST_AWCOBUF_ALIGN                     0
#define YDC_DDR_BIST_CONFIG_BIST_AWCOBUF_BITS                      1
#define YDC_DDR_BIST_CONFIG_BIST_AWCOBUF_SHIFT                     4
#define YDC_DDR_BIST_CONFIG_BIST_AWCOBUF_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_AWAPCMD [03:03] */
#define YDC_DDR_BIST_CONFIG_BIST_AWAPCMD_MASK                      0x00000008
#define YDC_DDR_BIST_CONFIG_BIST_AWAPCMD_ALIGN                     0
#define YDC_DDR_BIST_CONFIG_BIST_AWAPCMD_BITS                      1
#define YDC_DDR_BIST_CONFIG_BIST_AWAPCMD_SHIFT                     3
#define YDC_DDR_BIST_CONFIG_BIST_AWAPCMD_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_AWCACHE_0 [02:02] */
#define YDC_DDR_BIST_CONFIG_BIST_AWCACHE_0_MASK                    0x00000004
#define YDC_DDR_BIST_CONFIG_BIST_AWCACHE_0_ALIGN                   0
#define YDC_DDR_BIST_CONFIG_BIST_AWCACHE_0_BITS                    1
#define YDC_DDR_BIST_CONFIG_BIST_AWCACHE_0_SHIFT                   2
#define YDC_DDR_BIST_CONFIG_BIST_AWCACHE_0_DEFAULT                 0x00000000

/* YDC_DDR_BIST :: CONFIG :: AXI_PORT_SEL [01:01] */
#define YDC_DDR_BIST_CONFIG_AXI_PORT_SEL_MASK                      0x00000002
#define YDC_DDR_BIST_CONFIG_AXI_PORT_SEL_ALIGN                     0
#define YDC_DDR_BIST_CONFIG_AXI_PORT_SEL_BITS                      1
#define YDC_DDR_BIST_CONFIG_AXI_PORT_SEL_SHIFT                     1
#define YDC_DDR_BIST_CONFIG_AXI_PORT_SEL_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: CONFIG :: BIST_RESETB [00:00] */
#define YDC_DDR_BIST_CONFIG_BIST_RESETB_MASK                       0x00000001
#define YDC_DDR_BIST_CONFIG_BIST_RESETB_ALIGN                      0
#define YDC_DDR_BIST_CONFIG_BIST_RESETB_BITS                       1
#define YDC_DDR_BIST_CONFIG_BIST_RESETB_SHIFT                      0
#define YDC_DDR_BIST_CONFIG_BIST_RESETB_DEFAULT                    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: CONFIG_2
 ***************************************************************************/
/* YDC_DDR_BIST :: CONFIG_2 :: reserved0 [31:19] */
#define YDC_DDR_BIST_CONFIG_2_RESERVED0_MASK                       0xfff80000
#define YDC_DDR_BIST_CONFIG_2_RESERVED0_ALIGN                      0
#define YDC_DDR_BIST_CONFIG_2_RESERVED0_BITS                       13
#define YDC_DDR_BIST_CONFIG_2_RESERVED0_SHIFT                      19
#define YDC_DDR_BIST_CONFIG_2_RESERVED0_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: CONFIG_2 :: FIFO_RADDR_ALMOST_FULL_LEVEL [18:16] */
#define YDC_DDR_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_MASK    0x00070000
#define YDC_DDR_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_ALIGN   0
#define YDC_DDR_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_BITS    3
#define YDC_DDR_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_SHIFT   16
#define YDC_DDR_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_DEFAULT 0x00000004

/* YDC_DDR_BIST :: CONFIG_2 :: reserved1 [15:11] */
#define YDC_DDR_BIST_CONFIG_2_RESERVED1_MASK                       0x0000f800
#define YDC_DDR_BIST_CONFIG_2_RESERVED1_ALIGN                      0
#define YDC_DDR_BIST_CONFIG_2_RESERVED1_BITS                       5
#define YDC_DDR_BIST_CONFIG_2_RESERVED1_SHIFT                      11
#define YDC_DDR_BIST_CONFIG_2_RESERVED1_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: CONFIG_2 :: FIFO_CMD_ALMOST_FULL_LEVEL [10:08] */
#define YDC_DDR_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_MASK      0x00000700
#define YDC_DDR_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_ALIGN     0
#define YDC_DDR_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_BITS      3
#define YDC_DDR_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_SHIFT     8
#define YDC_DDR_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_DEFAULT   0x00000002

/* YDC_DDR_BIST :: CONFIG_2 :: reserved2 [07:03] */
#define YDC_DDR_BIST_CONFIG_2_RESERVED2_MASK                       0x000000f8
#define YDC_DDR_BIST_CONFIG_2_RESERVED2_ALIGN                      0
#define YDC_DDR_BIST_CONFIG_2_RESERVED2_BITS                       5
#define YDC_DDR_BIST_CONFIG_2_RESERVED2_SHIFT                      3
#define YDC_DDR_BIST_CONFIG_2_RESERVED2_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: CONFIG_2 :: FIFO_WDATA_ALMOST_FULL_LEVEL [02:00] */
#define YDC_DDR_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_MASK    0x00000007
#define YDC_DDR_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_ALIGN   0
#define YDC_DDR_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_BITS    3
#define YDC_DDR_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_SHIFT   0
#define YDC_DDR_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_DEFAULT 0x00000002


/****************************************************************************
 * YDC_DDR_BIST :: GENERAL_CONFIGURATIONS
 ***************************************************************************/
/* YDC_DDR_BIST :: GENERAL_CONFIGURATIONS :: reserved0 [31:07] */
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED0_MASK         0xffffff80
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED0_ALIGN        0
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED0_BITS         25
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED0_SHIFT        7
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED0_DEFAULT      0x00000000

/* YDC_DDR_BIST :: GENERAL_CONFIGURATIONS :: NUM_COLS [06:04] */
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_MASK          0x00000070
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_ALIGN         0
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_BITS          3
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_SHIFT         4
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_DEFAULT       0x00000002

/* YDC_DDR_BIST :: GENERAL_CONFIGURATIONS :: reserved1 [03:00] */
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED1_MASK         0x0000000f
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED1_ALIGN        0
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED1_BITS         4
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED1_SHIFT        0
#define YDC_DDR_BIST_GENERAL_CONFIGURATIONS_RESERVED1_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: CONFIGURATIONS
 ***************************************************************************/
/* YDC_DDR_BIST :: CONFIGURATIONS :: reserved0 [31:26] */
#define YDC_DDR_BIST_CONFIGURATIONS_RESERVED0_MASK                 0xfc000000
#define YDC_DDR_BIST_CONFIGURATIONS_RESERVED0_ALIGN                0
#define YDC_DDR_BIST_CONFIGURATIONS_RESERVED0_BITS                 6
#define YDC_DDR_BIST_CONFIGURATIONS_RESERVED0_SHIFT                26
#define YDC_DDR_BIST_CONFIGURATIONS_RESERVED0_DEFAULT              0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: BIST_EN [25:25] */
#define YDC_DDR_BIST_CONFIGURATIONS_BIST_EN_MASK                   0x02000000
#define YDC_DDR_BIST_CONFIGURATIONS_BIST_EN_ALIGN                  0
#define YDC_DDR_BIST_CONFIGURATIONS_BIST_EN_BITS                   1
#define YDC_DDR_BIST_CONFIGURATIONS_BIST_EN_SHIFT                  25
#define YDC_DDR_BIST_CONFIGURATIONS_BIST_EN_DEFAULT                0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: DATA_ADDR_MODE [24:24] */
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_ADDR_MODE_MASK            0x01000000
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_ADDR_MODE_ALIGN           0
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_ADDR_MODE_BITS            1
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_ADDR_MODE_SHIFT           24
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_ADDR_MODE_DEFAULT         0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: DATA_SHIFT_MODE [23:23] */
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_SHIFT_MODE_MASK           0x00800000
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_SHIFT_MODE_ALIGN          0
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_SHIFT_MODE_BITS           1
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_SHIFT_MODE_SHIFT          23
#define YDC_DDR_BIST_CONFIGURATIONS_DATA_SHIFT_MODE_DEFAULT        0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: ADDRESS_SHIFT_MODE [22:22] */
#define YDC_DDR_BIST_CONFIGURATIONS_ADDRESS_SHIFT_MODE_MASK        0x00400000
#define YDC_DDR_BIST_CONFIGURATIONS_ADDRESS_SHIFT_MODE_ALIGN       0
#define YDC_DDR_BIST_CONFIGURATIONS_ADDRESS_SHIFT_MODE_BITS        1
#define YDC_DDR_BIST_CONFIGURATIONS_ADDRESS_SHIFT_MODE_SHIFT       22
#define YDC_DDR_BIST_CONFIGURATIONS_ADDRESS_SHIFT_MODE_DEFAULT     0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: CONS_ADDR_8_BANKS [21:21] */
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_8_BANKS_MASK         0x00200000
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_8_BANKS_ALIGN        0
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_8_BANKS_BITS         1
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_8_BANKS_SHIFT        21
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_8_BANKS_DEFAULT      0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: CONS_ADDR_4_BANKS [20:20] */
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_4_BANKS_MASK         0x00100000
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_4_BANKS_ALIGN        0
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_4_BANKS_BITS         1
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_4_BANKS_SHIFT        20
#define YDC_DDR_BIST_CONFIGURATIONS_CONS_ADDR_4_BANKS_DEFAULT      0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: IND_WR_RD_ADDR_MODE [19:19] */
#define YDC_DDR_BIST_CONFIGURATIONS_IND_WR_RD_ADDR_MODE_MASK       0x00080000
#define YDC_DDR_BIST_CONFIGURATIONS_IND_WR_RD_ADDR_MODE_ALIGN      0
#define YDC_DDR_BIST_CONFIGURATIONS_IND_WR_RD_ADDR_MODE_BITS       1
#define YDC_DDR_BIST_CONFIGURATIONS_IND_WR_RD_ADDR_MODE_SHIFT      19
#define YDC_DDR_BIST_CONFIGURATIONS_IND_WR_RD_ADDR_MODE_DEFAULT    0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: PRBS_MODE [18:18] */
#define YDC_DDR_BIST_CONFIGURATIONS_PRBS_MODE_MASK                 0x00040000
#define YDC_DDR_BIST_CONFIGURATIONS_PRBS_MODE_ALIGN                0
#define YDC_DDR_BIST_CONFIGURATIONS_PRBS_MODE_BITS                 1
#define YDC_DDR_BIST_CONFIGURATIONS_PRBS_MODE_SHIFT                18
#define YDC_DDR_BIST_CONFIGURATIONS_PRBS_MODE_DEFAULT              0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: TWO_ADDR_MODE [17:17] */
#define YDC_DDR_BIST_CONFIGURATIONS_TWO_ADDR_MODE_MASK             0x00020000
#define YDC_DDR_BIST_CONFIGURATIONS_TWO_ADDR_MODE_ALIGN            0
#define YDC_DDR_BIST_CONFIGURATIONS_TWO_ADDR_MODE_BITS             1
#define YDC_DDR_BIST_CONFIGURATIONS_TWO_ADDR_MODE_SHIFT            17
#define YDC_DDR_BIST_CONFIGURATIONS_TWO_ADDR_MODE_DEFAULT          0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: PATTERN_BIT_MODE [16:16] */
#define YDC_DDR_BIST_CONFIGURATIONS_PATTERN_BIT_MODE_MASK          0x00010000
#define YDC_DDR_BIST_CONFIGURATIONS_PATTERN_BIT_MODE_ALIGN         0
#define YDC_DDR_BIST_CONFIGURATIONS_PATTERN_BIT_MODE_BITS          1
#define YDC_DDR_BIST_CONFIGURATIONS_PATTERN_BIT_MODE_SHIFT         16
#define YDC_DDR_BIST_CONFIGURATIONS_PATTERN_BIT_MODE_DEFAULT       0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: READ_WEIGHT [15:08] */
#define YDC_DDR_BIST_CONFIGURATIONS_READ_WEIGHT_MASK               0x0000ff00
#define YDC_DDR_BIST_CONFIGURATIONS_READ_WEIGHT_ALIGN              0
#define YDC_DDR_BIST_CONFIGURATIONS_READ_WEIGHT_BITS               8
#define YDC_DDR_BIST_CONFIGURATIONS_READ_WEIGHT_SHIFT              8
#define YDC_DDR_BIST_CONFIGURATIONS_READ_WEIGHT_DEFAULT            0x00000000

/* YDC_DDR_BIST :: CONFIGURATIONS :: WRITE_WEIGHT [07:00] */
#define YDC_DDR_BIST_CONFIGURATIONS_WRITE_WEIGHT_MASK              0x000000ff
#define YDC_DDR_BIST_CONFIGURATIONS_WRITE_WEIGHT_ALIGN             0
#define YDC_DDR_BIST_CONFIGURATIONS_WRITE_WEIGHT_BITS              8
#define YDC_DDR_BIST_CONFIGURATIONS_WRITE_WEIGHT_SHIFT             0
#define YDC_DDR_BIST_CONFIGURATIONS_WRITE_WEIGHT_DEFAULT           0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: NUMBER_OF_ACTIONS
 ***************************************************************************/
/* YDC_DDR_BIST :: NUMBER_OF_ACTIONS :: BIST_NUM_ACTIONS [31:00] */
#define YDC_DDR_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_MASK       0xffffffff
#define YDC_DDR_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_ALIGN      0
#define YDC_DDR_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_BITS       32
#define YDC_DDR_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_SHIFT      0
#define YDC_DDR_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: START_ADDRESS
 ***************************************************************************/
/* YDC_DDR_BIST :: START_ADDRESS :: reserved0 [31:26] */
#define YDC_DDR_BIST_START_ADDRESS_RESERVED0_MASK                  0xfc000000
#define YDC_DDR_BIST_START_ADDRESS_RESERVED0_ALIGN                 0
#define YDC_DDR_BIST_START_ADDRESS_RESERVED0_BITS                  6
#define YDC_DDR_BIST_START_ADDRESS_RESERVED0_SHIFT                 26
#define YDC_DDR_BIST_START_ADDRESS_RESERVED0_DEFAULT               0x00000000

/* YDC_DDR_BIST :: START_ADDRESS :: BIST_START_ADDRESS [25:00] */
#define YDC_DDR_BIST_START_ADDRESS_BIST_START_ADDRESS_MASK         0x03ffffff
#define YDC_DDR_BIST_START_ADDRESS_BIST_START_ADDRESS_ALIGN        0
#define YDC_DDR_BIST_START_ADDRESS_BIST_START_ADDRESS_BITS         26
#define YDC_DDR_BIST_START_ADDRESS_BIST_START_ADDRESS_SHIFT        0
#define YDC_DDR_BIST_START_ADDRESS_BIST_START_ADDRESS_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: END_ADDRESS
 ***************************************************************************/
/* YDC_DDR_BIST :: END_ADDRESS :: reserved0 [31:26] */
#define YDC_DDR_BIST_END_ADDRESS_RESERVED0_MASK                    0xfc000000
#define YDC_DDR_BIST_END_ADDRESS_RESERVED0_ALIGN                   0
#define YDC_DDR_BIST_END_ADDRESS_RESERVED0_BITS                    6
#define YDC_DDR_BIST_END_ADDRESS_RESERVED0_SHIFT                   26
#define YDC_DDR_BIST_END_ADDRESS_RESERVED0_DEFAULT                 0x00000000

/* YDC_DDR_BIST :: END_ADDRESS :: BIST_END_ADDRESS [25:00] */
#define YDC_DDR_BIST_END_ADDRESS_BIST_END_ADDRESS_MASK             0x03ffffff
#define YDC_DDR_BIST_END_ADDRESS_BIST_END_ADDRESS_ALIGN            0
#define YDC_DDR_BIST_END_ADDRESS_BIST_END_ADDRESS_BITS             26
#define YDC_DDR_BIST_END_ADDRESS_BIST_END_ADDRESS_SHIFT            0
#define YDC_DDR_BIST_END_ADDRESS_BIST_END_ADDRESS_DEFAULT          0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: SINGLE_BIT_MASK
 ***************************************************************************/
/* YDC_DDR_BIST :: SINGLE_BIT_MASK :: BIST_SINGLE_MASK [31:00] */
#define YDC_DDR_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_MASK         0xffffffff
#define YDC_DDR_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_ALIGN        0
#define YDC_DDR_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_BITS         32
#define YDC_DDR_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_SHIFT        0
#define YDC_DDR_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_DEFAULT      0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_7
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_7 :: BIST_PATTERN_7 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_7_BIST_PATTERN_7_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_7_BIST_PATTERN_7_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_7_BIST_PATTERN_7_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_7_BIST_PATTERN_7_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_7_BIST_PATTERN_7_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_6
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_6 :: BIST_PATTERN_6 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_6_BIST_PATTERN_6_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_6_BIST_PATTERN_6_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_6_BIST_PATTERN_6_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_6_BIST_PATTERN_6_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_6_BIST_PATTERN_6_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_5
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_5 :: BIST_PATTERN_5 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_5_BIST_PATTERN_5_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_5_BIST_PATTERN_5_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_5_BIST_PATTERN_5_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_5_BIST_PATTERN_5_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_5_BIST_PATTERN_5_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_4
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_4 :: BIST_PATTERN_4 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_4_BIST_PATTERN_4_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_4_BIST_PATTERN_4_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_4_BIST_PATTERN_4_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_4_BIST_PATTERN_4_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_4_BIST_PATTERN_4_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_3
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_3 :: BIST_PATTERN_3 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_3_BIST_PATTERN_3_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_3_BIST_PATTERN_3_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_3_BIST_PATTERN_3_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_3_BIST_PATTERN_3_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_3_BIST_PATTERN_3_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_2
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_2 :: BIST_PATTERN_2 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_2_BIST_PATTERN_2_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_2_BIST_PATTERN_2_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_2_BIST_PATTERN_2_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_2_BIST_PATTERN_2_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_2_BIST_PATTERN_2_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_1
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_1 :: BIST_PATTERN_1 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_1_BIST_PATTERN_1_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_1_BIST_PATTERN_1_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_1_BIST_PATTERN_1_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_1_BIST_PATTERN_1_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_1_BIST_PATTERN_1_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: PATTERN_WORD_0
 ***************************************************************************/
/* YDC_DDR_BIST :: PATTERN_WORD_0 :: BIST_PATTERN_0 [31:00] */
#define YDC_DDR_BIST_PATTERN_WORD_0_BIST_PATTERN_0_MASK            0xffffffff
#define YDC_DDR_BIST_PATTERN_WORD_0_BIST_PATTERN_0_ALIGN           0
#define YDC_DDR_BIST_PATTERN_WORD_0_BIST_PATTERN_0_BITS            32
#define YDC_DDR_BIST_PATTERN_WORD_0_BIST_PATTERN_0_SHIFT           0
#define YDC_DDR_BIST_PATTERN_WORD_0_BIST_PATTERN_0_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_7
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_7 :: BIST_FULL_MASK_7 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_6
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_6 :: BIST_FULL_MASK_6 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_5
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_5 :: BIST_FULL_MASK_5 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_4
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_4 :: BIST_FULL_MASK_4 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_3
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_3 :: BIST_FULL_MASK_3 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_2
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_2 :: BIST_FULL_MASK_2 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_1
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_1 :: BIST_FULL_MASK_1 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_WORD_0
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_WORD_0 :: BIST_FULL_MASK_0 [31:00] */
#define YDC_DDR_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_MASK        0xffffffff
#define YDC_DDR_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_ALIGN       0
#define YDC_DDR_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_BITS        32
#define YDC_DDR_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_SHIFT       0
#define YDC_DDR_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: STATUSES
 ***************************************************************************/
/* YDC_DDR_BIST :: STATUSES :: reserved0 [31:04] */
#define YDC_DDR_BIST_STATUSES_RESERVED0_MASK                       0xfffffff0
#define YDC_DDR_BIST_STATUSES_RESERVED0_ALIGN                      0
#define YDC_DDR_BIST_STATUSES_RESERVED0_BITS                       28
#define YDC_DDR_BIST_STATUSES_RESERVED0_SHIFT                      4
#define YDC_DDR_BIST_STATUSES_RESERVED0_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: STATUSES :: OVERFLOW_FIFO_RADDR [03:03] */
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_RADDR_MASK             0x00000008
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_RADDR_ALIGN            0
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_RADDR_BITS             1
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_RADDR_SHIFT            3
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_RADDR_DEFAULT          0x00000000

/* YDC_DDR_BIST :: STATUSES :: OVERFLOW_FIFO_CMD [02:02] */
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_CMD_MASK               0x00000004
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_CMD_ALIGN              0
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_CMD_BITS               1
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_CMD_SHIFT              2
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_CMD_DEFAULT            0x00000000

/* YDC_DDR_BIST :: STATUSES :: OVERFLOW_FIFO_WDATA [01:01] */
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_WDATA_MASK             0x00000002
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_WDATA_ALIGN            0
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_WDATA_BITS             1
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_WDATA_SHIFT            1
#define YDC_DDR_BIST_STATUSES_OVERFLOW_FIFO_WDATA_DEFAULT          0x00000000

/* YDC_DDR_BIST :: STATUSES :: BIST_FINISHED [00:00] */
#define YDC_DDR_BIST_STATUSES_BIST_FINISHED_MASK                   0x00000001
#define YDC_DDR_BIST_STATUSES_BIST_FINISHED_ALIGN                  0
#define YDC_DDR_BIST_STATUSES_BIST_FINISHED_BITS                   1
#define YDC_DDR_BIST_STATUSES_BIST_FINISHED_SHIFT                  0
#define YDC_DDR_BIST_STATUSES_BIST_FINISHED_DEFAULT                0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: FULL_MASK_ERROR_COUNTER
 ***************************************************************************/
/* YDC_DDR_BIST :: FULL_MASK_ERROR_COUNTER :: FULL_ERR_CNT [31:00] */
#define YDC_DDR_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_MASK     0xffffffff
#define YDC_DDR_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_ALIGN    0
#define YDC_DDR_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_BITS     32
#define YDC_DDR_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_SHIFT    0
#define YDC_DDR_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: SINGLE_BIT_MASK_ERROR_COUNTER
 ***************************************************************************/
/* YDC_DDR_BIST :: SINGLE_BIT_MASK_ERROR_COUNTER :: SINGLE_ERR_CNT [31:00] */
#define YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_MASK      0xffffffff
#define YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_ALIGN     0
#define YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_BITS      32
#define YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_SHIFT     0
#define YDC_DDR_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: ERROR_OCCURRED
 ***************************************************************************/
/* YDC_DDR_BIST :: ERROR_OCCURRED :: ERR_OCCURRED [31:00] */
#define YDC_DDR_BIST_ERROR_OCCURRED_ERR_OCCURRED_MASK              0xffffffff
#define YDC_DDR_BIST_ERROR_OCCURRED_ERR_OCCURRED_ALIGN             0
#define YDC_DDR_BIST_ERROR_OCCURRED_ERR_OCCURRED_BITS              32
#define YDC_DDR_BIST_ERROR_OCCURRED_ERR_OCCURRED_SHIFT             0
#define YDC_DDR_BIST_ERROR_OCCURRED_ERR_OCCURRED_DEFAULT           0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: GLOBAL_ERROR_COUNTER
 ***************************************************************************/
/* YDC_DDR_BIST :: GLOBAL_ERROR_COUNTER :: GLOBAL_ERR_CNT [31:00] */
#define YDC_DDR_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_MASK      0xffffffff
#define YDC_DDR_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_ALIGN     0
#define YDC_DDR_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_BITS      32
#define YDC_DDR_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_SHIFT     0
#define YDC_DDR_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_ADDR_ERR
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_ADDR_ERR :: reserved0 [31:26] */
#define YDC_DDR_BIST_LAST_ADDR_ERR_RESERVED0_MASK                  0xfc000000
#define YDC_DDR_BIST_LAST_ADDR_ERR_RESERVED0_ALIGN                 0
#define YDC_DDR_BIST_LAST_ADDR_ERR_RESERVED0_BITS                  6
#define YDC_DDR_BIST_LAST_ADDR_ERR_RESERVED0_SHIFT                 26
#define YDC_DDR_BIST_LAST_ADDR_ERR_RESERVED0_DEFAULT               0x00000000

/* YDC_DDR_BIST :: LAST_ADDR_ERR :: BIST_LAST_ADDR_ERR [25:00] */
#define YDC_DDR_BIST_LAST_ADDR_ERR_BIST_LAST_ADDR_ERR_MASK         0x03ffffff
#define YDC_DDR_BIST_LAST_ADDR_ERR_BIST_LAST_ADDR_ERR_ALIGN        0
#define YDC_DDR_BIST_LAST_ADDR_ERR_BIST_LAST_ADDR_ERR_BITS         26
#define YDC_DDR_BIST_LAST_ADDR_ERR_BIST_LAST_ADDR_ERR_SHIFT        0
#define YDC_DDR_BIST_LAST_ADDR_ERR_BIST_LAST_ADDR_ERR_DEFAULT      0x03ffffff


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_7
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_7 :: BIST_LAST_DATA_ERR_7 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_7_BIST_LAST_DATA_ERR_7_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_7_BIST_LAST_DATA_ERR_7_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_7_BIST_LAST_DATA_ERR_7_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_7_BIST_LAST_DATA_ERR_7_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_7_BIST_LAST_DATA_ERR_7_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_6
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_6 :: BIST_LAST_DATA_ERR_6 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_6_BIST_LAST_DATA_ERR_6_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_6_BIST_LAST_DATA_ERR_6_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_6_BIST_LAST_DATA_ERR_6_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_6_BIST_LAST_DATA_ERR_6_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_6_BIST_LAST_DATA_ERR_6_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_5
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_5 :: BIST_LAST_DATA_ERR_5 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_5_BIST_LAST_DATA_ERR_5_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_5_BIST_LAST_DATA_ERR_5_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_5_BIST_LAST_DATA_ERR_5_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_5_BIST_LAST_DATA_ERR_5_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_5_BIST_LAST_DATA_ERR_5_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_4
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_4 :: BIST_LAST_DATA_ERR_4 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_4_BIST_LAST_DATA_ERR_4_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_4_BIST_LAST_DATA_ERR_4_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_4_BIST_LAST_DATA_ERR_4_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_4_BIST_LAST_DATA_ERR_4_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_4_BIST_LAST_DATA_ERR_4_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_3
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_3 :: BIST_LAST_DATA_ERR_3 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_3_BIST_LAST_DATA_ERR_3_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_3_BIST_LAST_DATA_ERR_3_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_3_BIST_LAST_DATA_ERR_3_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_3_BIST_LAST_DATA_ERR_3_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_3_BIST_LAST_DATA_ERR_3_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_2
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_2 :: BIST_LAST_DATA_ERR_2 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_2_BIST_LAST_DATA_ERR_2_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_2_BIST_LAST_DATA_ERR_2_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_2_BIST_LAST_DATA_ERR_2_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_2_BIST_LAST_DATA_ERR_2_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_2_BIST_LAST_DATA_ERR_2_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_1
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_1 :: BIST_LAST_DATA_ERR_1 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_1_BIST_LAST_DATA_ERR_1_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_1_BIST_LAST_DATA_ERR_1_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_1_BIST_LAST_DATA_ERR_1_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_1_BIST_LAST_DATA_ERR_1_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_1_BIST_LAST_DATA_ERR_1_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: LAST_DATA_ERR_WORD_0
 ***************************************************************************/
/* YDC_DDR_BIST :: LAST_DATA_ERR_WORD_0 :: BIST_LAST_DATA_ERR_0 [31:00] */
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_0_BIST_LAST_DATA_ERR_0_MASK         0xffffffff
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_0_BIST_LAST_DATA_ERR_0_ALIGN        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_0_BIST_LAST_DATA_ERR_0_BITS         32
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_0_BIST_LAST_DATA_ERR_0_SHIFT        0
#define YDC_DDR_BIST_LAST_DATA_ERR_WORD_0_BIST_LAST_DATA_ERR_0_DEFAULT      0x00000000


/****************************************************************************
 * Datatype Definitions.
 ***************************************************************************/
#endif /* #ifndef __SOC_BROADCOM_CYGNUS_YDC_DDR_BIST_H__ */

/* End of File */
