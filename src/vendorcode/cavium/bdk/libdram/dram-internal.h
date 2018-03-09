#ifndef __DRAM_INTERNAL_H__
#define __DRAM_INTERNAL_H__
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
 * This header defines all internal API for libdram. None
 * of these functions should be called by users of the library.
 * This is the only header that DRAM files should include
 * from the libdram directory
 */

/* FIXME(dhendrix): include path */
//#include "libdram.h"
#include <libdram/libdram.h>
#include "lib_octeon_shared.h"
#include "dram-print.h"
#include "dram-util.h"
#include "dram-csr.h"
#include "dram-env.h"
#include "dram-gpio.h"
#include "dram-spd.h"
#include "dram-l2c.h"
#include "dram-init-ddr3.h"

#undef DRAM_CSR_WRITE_INLINE

// define how many HW WL samples to take for majority voting
// MUST BE odd!!
// assume there should only be 2 possible values that will show up,
// so treat ties as a problem!!!
#define WLEVEL_LOOPS_DEFAULT     5 // NOTE: do not change this without checking the code!!!

// define how many HW RL samples per rank to take
// multiple samples will allow either:
// 1. looking for the best sample score
// 2. averaging the samples into a composite score
// symbol PICK_BEST_RANK_SCORE_NOT_AVG is used to choose
// (see dram-init-ddr3.c: 
#define RLEVEL_AVG_LOOPS_DEFAULT 3
#define PICK_BEST_RANK_SCORE_NOT_AVG 1

typedef struct {
    int delay;
    int loop_total;
    int loop_count;
    int best;
    uint64_t bm;
    int bmerrs;
    int sqerrs;
    int bestsq;
} rlevel_byte_data_t;

typedef struct {
    uint64_t bm;
    uint8_t mstart;
    uint8_t width;
    int errs;
} rlevel_bitmask_t;

#define SET_DDR_DLL_CTL3(field, expr)                   \
    do {                                                \
        ddr_dll_ctl3.cn81xx.field = (expr);             \
    } while (0)

#define ENCODE_DLL90_BYTE_SEL(byte_sel) ((byte_sel)+1)

#define GET_DDR_DLL_CTL3(field)                         \
    (ddr_dll_ctl3.cn81xx.field)


#define RLEVEL_NONSEQUENTIAL_DELAY_ERROR        50
#define RLEVEL_ADJACENT_DELAY_ERROR             30

#define TWO_LMC_MASK  0x03
#define FOUR_LMC_MASK 0x0f
#define ONE_DIMM_MASK 0x01
#define TWO_DIMM_MASK 0x03

extern int initialize_ddr_clock(bdk_node_t node,
    const ddr_configuration_t *ddr_configuration, uint32_t cpu_hertz,
    uint32_t ddr_hertz, uint32_t ddr_ref_hertz, int ddr_interface_num,
    uint32_t ddr_interface_mask);

extern int test_dram_byte(bdk_node_t node, int ddr_interface_num, uint64_t p,
                          uint64_t bitmask, uint64_t *xor_data);
extern int dram_tuning_mem_xor(bdk_node_t node, int ddr_interface_num, uint64_t p,
                               uint64_t bitmask, uint64_t *xor_data);

// "mode" arg
#define DBTRAIN_TEST 0
#define DBTRAIN_DBI  1 
#define DBTRAIN_LFSR 2
extern int test_dram_byte_hw(bdk_node_t node, int ddr_interface_num,
                             uint64_t p, int mode, uint64_t *xor_data);
extern int run_best_hw_patterns(bdk_node_t node, int ddr_interface_num,
                                uint64_t p, int mode, uint64_t *xor_data);

extern int get_dimm_part_number(char *buffer, bdk_node_t node,
                                const dimm_config_t *dimm_config,
				int ddr_type);
extern uint32_t get_dimm_serial_number(bdk_node_t node,
                                       const dimm_config_t *dimm_config,
				       int ddr_type);

extern int octeon_ddr_initialize(bdk_node_t node, uint32_t cpu_hertz,
    uint32_t ddr_hertz, uint32_t ddr_ref_hertz, uint32_t ddr_interface_mask,
    const ddr_configuration_t *ddr_configuration, uint32_t *measured_ddr_hertz,
    int board_type, int board_rev_maj, int board_rev_min);

extern uint64_t divide_nint(uint64_t dividend, uint64_t divisor);

typedef enum {
    DDR3_DRAM = 3,
    DDR4_DRAM = 4,
} ddr_type_t;

static inline int get_ddr_type(bdk_node_t node, const dimm_config_t *dimm_config)
{
    int spd_ddr_type;

#define DEVICE_TYPE DDR4_SPD_KEY_BYTE_DEVICE_TYPE // same for DDR3 and DDR4
    spd_ddr_type = read_spd(node, dimm_config, DEVICE_TYPE);

    debug_print("%s:%d spd_ddr_type=0x%02x\n", __FUNCTION__, __LINE__, spd_ddr_type);

    /* we return only DDR4 or DDR3 */
    return (spd_ddr_type == 0x0C) ? DDR4_DRAM : DDR3_DRAM;
}

static inline int get_dimm_ecc(bdk_node_t node, const dimm_config_t *dimm_config, int ddr_type)
{
#define BUS_WIDTH(t)   (((t) == DDR4_DRAM) ? DDR4_SPD_MODULE_MEMORY_BUS_WIDTH : DDR3_SPD_MEMORY_BUS_WIDTH)

    return !!(read_spd(node, dimm_config, BUS_WIDTH(ddr_type)) & 8);
}

static inline int get_dimm_module_type(bdk_node_t node, const dimm_config_t *dimm_config, int ddr_type)
{
#define MODULE_TYPE DDR4_SPD_KEY_BYTE_MODULE_TYPE // same for DDR3 and DDR4

    return (read_spd(node, dimm_config, MODULE_TYPE) & 0x0F);
}

extern int common_ddr4_fixups(dram_config_t *cfg, uint32_t default_udimm_speed);

#define DEFAULT_BEST_RANK_SCORE  9999999
#define MAX_RANK_SCORE_LIMIT     99 // is this OK?

unsigned short load_dll_offset(bdk_node_t node, int ddr_interface_num,
			       int dll_offset_mode, int byte_offset, int byte);
void change_dll_offset_enable(bdk_node_t node, int ddr_interface_num, int change);

extern int perform_dll_offset_tuning(bdk_node_t node, int dll_offset_mode, int do_tune);
extern int perform_HW_dll_offset_tuning(bdk_node_t node, int dll_offset_mode, int bytelane);

extern int perform_margin_write_voltage(bdk_node_t node);
extern int perform_margin_read_voltage(bdk_node_t node);

#define LMC_DDR3_RESET_ASSERT   0
#define LMC_DDR3_RESET_DEASSERT 1
extern void cn88xx_lmc_ddr3_reset(bdk_node_t node, int ddr_interface_num, int reset);
extern void perform_lmc_reset(bdk_node_t node, int ddr_interface_num);
extern void ddr4_mrw(bdk_node_t node, int ddr_interface_num, int rank,
                     int mr_wr_addr, int mr_wr_sel, int mr_wr_bg1);
#endif /* __DRAM_INTERNAL_H__ */

