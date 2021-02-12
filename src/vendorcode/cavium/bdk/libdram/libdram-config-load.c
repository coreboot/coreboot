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
#include <bdk.h>

/* FIXME(dhendrix): added */
#include <console/console.h>	/* for die() */
#include <string.h>
#include <libbdk-arch/bdk-model.h>
#include <libbdk-hal/bdk-config.h>
#include <soc/twsi.h>
#include <device/i2c_simple.h>

/**
 * Load a "odt_*rank_config" structure
 *
 * @param cfg    Config to fill
 * @param ranks  Number of ranks we're loading (1,2,4)
 * @param node   Node we're loading for
 * @param dimm   Which DIMM this is for
 * @param lmc    Which LMC this is for
 */
static void load_rank_data(dram_config_t *cfg, int ranks, int num_dimms, int lmc, bdk_node_t node)
{
    /* Get a pointer to the structure we are filling */
    dimm_odt_config_t *c;
    switch (ranks)
    {
        case 1:
            c = &cfg->config[lmc].odt_1rank_config[num_dimms - 1];
            break;
        case 2:
            c = &cfg->config[lmc].odt_2rank_config[num_dimms - 1];
            break;
        case 4:
            c = &cfg->config[lmc].odt_4rank_config[num_dimms - 1];
            break;
        default:
            bdk_fatal("Unexpected number of ranks\n");
            break;
    }

    /* Fill the global items */
    c->odt_ena      = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_DQX_CTL,      ranks, num_dimms, lmc, node);
    c->odt_mask     = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_WODT_MASK,    ranks, num_dimms, lmc, node);

    /* Fill the per rank items */
    int rank = 0;
    c->odt_mask1.s.pasr_00              = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_PASR,    ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.asr_00               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_ASR,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.srt_00               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_SRT,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_00            = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_00_ext        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node) >> 2;
    c->odt_mask1.s.dic_00               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_DIC,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_nom_00           = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_NOM, ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.db_output_impedance  = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_DB_OUTPUT_IMPEDANCE, ranks, num_dimms, lmc, node);
    rank = 1;
    c->odt_mask1.s.pasr_01              = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_PASR,    ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.asr_01               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_ASR,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.srt_01               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_SRT,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_01            = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_01_ext        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node) >> 2;
    c->odt_mask1.s.dic_01               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_DIC,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_nom_01           = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_NOM, ranks, num_dimms, rank, lmc, node);
    rank = 2;
    c->odt_mask1.s.pasr_10              = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_PASR,    ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.asr_10               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_ASR,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.srt_10               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_SRT,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_10            = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_10_ext        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node) >> 2;
    c->odt_mask1.s.dic_10               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_DIC,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_nom_10           = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_NOM, ranks, num_dimms, rank, lmc, node);
    rank = 3;
    c->odt_mask1.s.pasr_11              = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_PASR,    ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.asr_11               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_ASR,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.srt_11               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_SRT,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_11            = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_wr_11_ext        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_WR,  ranks, num_dimms, rank, lmc, node) >> 2;
    c->odt_mask1.s.dic_11               = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_DIC,     ranks, num_dimms, rank, lmc, node);
    c->odt_mask1.s.rtt_nom_11           = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE1_RTT_NOM, ranks, num_dimms, rank, lmc, node);
    rank = 0;
    c->odt_mask2.s.rtt_park_00          = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_RTT_PARK,   ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_value_00        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_VALUE, ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_range_00        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_RANGE, ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vrefdq_train_en      = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREFDQ_TRAIN_EN, ranks, num_dimms, lmc, node);
    rank = 1;
    c->odt_mask2.s.rtt_park_01          = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_RTT_PARK,   ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_value_01        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_VALUE, ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_range_01        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_RANGE, ranks, num_dimms, rank, lmc, node);
    rank = 2;
    c->odt_mask2.s.rtt_park_10          = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_RTT_PARK,   ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_value_10        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_VALUE, ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_range_10        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_RANGE, ranks, num_dimms, rank, lmc, node);
    rank = 3;
    c->odt_mask2.s.rtt_park_11          = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_RTT_PARK,   ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_value_11        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_VALUE, ranks, num_dimms, rank, lmc, node);
    c->odt_mask2.s.vref_range_11        = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_MODE2_VREF_RANGE, ranks, num_dimms, rank, lmc, node);

    /* Fill more global items */
    c->qs_dic       = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_RODT_CTL,     ranks, num_dimms, lmc, node);
    c->rodt_ctl     = bdk_config_get_int(BDK_CONFIG_DDR_RANKS_RODT_MASK,    ranks, num_dimms, lmc, node);
}

/**
 * Load a DRAM configuration based on the current bdk-config settings
 *
 * @param node   Node the DRAM config is for
 *
 * @return Pointer to __libdram_global_cfg, a global structure. Returns NULL if bdk-config
 *         lacks information about DRAM.
 */
const dram_config_t *libdram_config_load(bdk_node_t node)
{
    dram_config_t *cfg = &__libdram_global_cfg;
    const int MAX_LMCS = ARRAY_SIZE(cfg->config);

    /* Make all fields for the node default to zero */
    memset(cfg, 0, sizeof(*cfg));

    /* Fill the SPD data first as some parameters need to know the DRAM type
       to lookup the correct values */
    for (int lmc = 0; lmc < MAX_LMCS; lmc++)
    {
        for (int dimm = 0; dimm < DDR_CFG_T_MAX_DIMMS; dimm++)
        {
            int spd_addr = bdk_config_get_int(BDK_CONFIG_DDR_SPD_ADDR, dimm, lmc, node);
            if (spd_addr)
            {
                cfg->config[lmc].dimm_config_table[dimm].spd_addr = spd_addr;
            }
            else
            {
#if 0
                int spd_size;
                const void *spd_data = bdk_config_get_blob(&spd_size, BDK_CONFIG_DDR_SPD_DATA, dimm, lmc, node);
                if (spd_data && spd_size)
                    cfg->config[lmc].dimm_config_table[dimm].spd_ptr = spd_data;
#endif
            }
        }
    }

    /* Check that we know how to get DIMM inofmration. If not, return failure */
    if (!cfg->config[0].dimm_config_table[0].spd_addr && !cfg->config[0].dimm_config_table[0].spd_ptr)
        return NULL;

    cfg->name = "Loaded from bdk-config";
    for (int lmc = 0; lmc < MAX_LMCS; lmc++)
    {
        for (int num_dimms = 1; num_dimms <= DDR_CFG_T_MAX_DIMMS; num_dimms++)
        {
            load_rank_data(cfg, 1, num_dimms, lmc, node);
            load_rank_data(cfg, 2, num_dimms, lmc, node);
            load_rank_data(cfg, 4, num_dimms, lmc, node);
        }

        ddr_configuration_t *c = &cfg->config[lmc];
        ddr3_custom_config_t *custom = &c->custom_lmc_config;
        custom->min_rtt_nom_idx                 = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MIN_RTT_NOM_IDX,     lmc, node);
        custom->max_rtt_nom_idx                 = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MAX_RTT_NOM_IDX,     lmc, node);
        custom->min_rodt_ctl                    = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MIN_RODT_CTL,        lmc, node);
        custom->max_rodt_ctl                    = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MAX_RODT_CTL,        lmc, node);
        custom->ck_ctl                          = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_CK_CTL,              lmc, node);
        custom->cmd_ctl                         = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_CMD_CTL,             lmc, node);
        custom->ctl_ctl                         = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_CTL_CTL,             lmc, node);
        custom->min_cas_latency                 = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MIN_CAS_LATENCY,     lmc, node);
        custom->offset_en                       = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_OFFSET_EN,           lmc, node);
        custom->offset_udimm                    = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_OFFSET,              "UDIMM", lmc, node);
        custom->offset_rdimm                    = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_OFFSET,              "RDIMM", lmc, node);
        custom->rlevel_compute                  = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_RLEVEL_COMPUTE,      lmc, node);
        custom->rlevel_comp_offset_udimm        = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_RLEVEL_COMP_OFFSET,  "UDIMM", lmc, node);
        custom->rlevel_comp_offset_rdimm        = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_RLEVEL_COMP_OFFSET,  "RDIMM", lmc, node);
        custom->ddr2t_udimm                     = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_DDR2T,               "UDIMM", lmc, node);
        custom->ddr2t_rdimm                     = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_DDR2T,               "RDIMM", lmc, node);
        custom->disable_sequential_delay_check  = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_DISABLE_SEQUENTIAL_DELAY_CHECK, lmc, node);
        custom->maximum_adjacent_rlevel_delay_increment
                                                = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MAXIMUM_ADJACENT_RLEVEL_DELAY_INCREMENT, lmc, node);
        custom->parity                          = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_PARITY,          lmc, node);
        custom->fprch2                          = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_FPRCH2,          lmc, node);
        custom->mode32b                         = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MODE32B,         lmc, node);
        custom->measured_vref                   = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_MEASURED_VREF,   lmc, node);

        /* CN80XX only supports 32bit mode */
        if (cavium_is_altpkg(CAVIUM_CN81XX))
            custom->mode32b = 1;

        /* Loop through 8 bytes, plus ecc byte */
        #define NUM_BYTES 9 /* Max bytes on LMC (8 plus ECC) */
        static int8_t dll_write_offset[NUM_BYTES];
        static int8_t dll_read_offset[NUM_BYTES];
        for (int b = 0; b < NUM_BYTES; b++)
        {
            dll_write_offset[b] = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_DLL_WRITE_OFFSET, b, lmc, node);
            dll_read_offset[b] = bdk_config_get_int(BDK_CONFIG_DDR_CUSTOM_DLL_READ_OFFSET,   b, lmc, node);
        }
        custom->dll_write_offset = dll_write_offset;
        custom->dll_read_offset = dll_read_offset;
    }

    int is_ddr4 = (cfg->config[0].odt_1rank_config[0].odt_mask2.u != 0);
    int speed = bdk_config_get_int(BDK_CONFIG_DDR_SPEED, node);
    switch (speed)
    {
    	case 0: // AUTO
            cfg->ddr_clock_hertz = 0;
            break;
        case 800:
        case 1600:
        case 2400:
            cfg->ddr_clock_hertz = (uint64_t)speed * 1000000 / 2;
            break;
        case 666:
            cfg->ddr_clock_hertz = 333333333;
            break;
        case 1066:
            cfg->ddr_clock_hertz = 533333333;
            break;
        case 1333:
            cfg->ddr_clock_hertz = 666666666;
            break;
        case 1866:
            if (is_ddr4)
                cfg->ddr_clock_hertz = 940000000;
            else
                cfg->ddr_clock_hertz = 933333333;
            break;
        case 2133:
            cfg->ddr_clock_hertz = 1050000000;
            break;
        default:
            bdk_warn("Unsupported DRAM speed of %d MT/s\n", speed);
            cfg->ddr_clock_hertz = speed * 1000000 / 2;
            break;
    }

    return cfg;
};
