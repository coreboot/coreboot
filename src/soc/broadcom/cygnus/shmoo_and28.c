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


#include <string.h>
#include <soc/config.h>
#include <soc/reg_utils.h>

#define SOC_IF_ERROR_RETURN(x)      (x)
#define sal_memset(x,y,z)           memset(x,y,z)
#define sal_usleep(x)               udelay(x)

/* BEGIN: TEMPORARY */
#ifndef BCM_AND28_SUPPORT
#define BCM_AND28_SUPPORT
#endif
/* END: TEMPORARY */

#if IS_ENABLED(CONFIG_CYGNUS_PRINT_SHMOO_DEBUG)
#define PLOT_SUPPORT
#endif

#ifdef BCM_AND28_SUPPORT
#include <soc/shmoo_and28/shmoo_and28.h>
#include <soc/shmoo_and28/phy_reg_access.h>
#include <soc/shmoo_and28/ydc_ddr_bist.h>

#ifdef PHY_AND28_E2
#include <soc/shmoo_and28/phy_and28_e2.h>
#else
#error "Phy_AND version is not defined."
#endif
#endif

and28_shmoo_dram_info_t *shmoo_dram_info_ptr;
static and28_shmoo_dram_info_t shmoo_dram_info;
static and28_shmoo_container_t shmoo_container = {0};

#if (SHMOO_AND28_DRAM_TYPE == SHMOO_AND28_DRAM_TYPE_DDR3)
const uint32 shmoo_order_and28_ddr3[SHMOO_AND28_DDR3_SEQUENCE_COUNT] =
{
    SHMOO_AND28_RD_EN,
    SHMOO_AND28_RD_EXTENDED,
    SHMOO_AND28_WR_EXTENDED,
    SHMOO_AND28_ADDR_EXTENDED,
    SHMOO_AND28_CTRL_EXTENDED
};
#endif

#if (SHMOO_AND28_DRAM_TYPE == SHMOO_AND28_DRAM_TYPE_DDR3L)
const uint32 shmoo_order_and28_ddr3l[SHMOO_AND28_DDR3L_SEQUENCE_COUNT] =
{
    SHMOO_AND28_RD_EN,
    SHMOO_AND28_RD_EXTENDED,
    SHMOO_AND28_WR_EXTENDED,
    SHMOO_AND28_ADDR_EXTENDED,
    SHMOO_AND28_CTRL_EXTENDED
};
#endif

/* Local function prototype */
uint32 _shmoo_and28_check_dram(int phy_ndx);
int _and28_calculate_step_size(int unit, int phy_ndx, and28_step_size_t *ssPtr);
int _and28_zq_calibration(int unit, int phy_ndx);
int _soc_and28_shmoo_phy_cfg_pll(int unit, int phy_ndx);

uint32
_shmoo_and28_check_dram(int phy_ndx)
{
    return ((shmoo_dram_info_ptr->dram_bitmap >> phy_ndx) & 0x1);
}

static int
_initialize_bist(int unit, int phy_ndx, int bit, and28_shmoo_container_t *scPtr)
{
    ydc_ddr_bist_info_t bist_info;

    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x00FFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            break;
        case SHMOO_AND28_RD_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x00FFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            break;
        case SHMOO_AND28_WR_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x00FFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x00FFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x00FFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }

    return soc_ydc_ddr_bist_config_set(unit, phy_ndx, &bist_info);
}

static int
_run_bist(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, and28_shmoo_error_array_t *seaPtr)
{
    ydc_ddr_bist_err_cnt_t be;

    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info_ptr->interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_AND28_RD_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info_ptr->interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_AND28_WR_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info_ptr->interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info_ptr->interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info_ptr->interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

static int
_shmoo_and28_rd_en(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 jump;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data;
    and28_shmoo_error_array_t sea;

    yCapMin = 0;
    yCapMax = (*scPtr).sizeY;
    jump = (*scPtr).yJump;
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_AND28_RD_EN;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(y = yCapMin; y < yCapMax; y++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, FORCE, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, VDL_STEP, y << jump);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN, data);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN, data);
        #endif
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN, data);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN, data);
        #endif

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMN, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCN, data);
            #endif
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMN, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCN, data);
            #endif
        }
        #endif

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, x);

            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
            }
            #endif

            sal_usleep(SHMOO_AND28_SHORT_SLEEP);

            _run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

static int
_shmoo_and28_wr_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and28_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND28_WR_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, x);

        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM, data);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC, data);
        #endif
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM, data);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC, data);
        #endif

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDC, data);
            #endif
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDC, data);
            #endif
        }
        #endif

        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
        }
        #endif

        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        _run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }

    return SOC_E_NONE;
}

static int
_shmoo_and28_rd_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data, temp;
    uint32 rd_dqs_pos0, rd_dqs_pos1, rd_en_pos0, rd_en_pos1, rd_dqs_delta0, rd_dqs_delta1;
    uint32 rd_dq_fail_count0, rd_dq_fail_count1;
    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        uint32 rd_dqs_pos2, rd_dqs_pos3, rd_en_pos2, rd_en_pos3, rd_dqs_delta2, rd_dqs_delta3;
        uint32 rd_dq_fail_count2, rd_dq_fail_count3;
    #endif
    and28_shmoo_error_array_t sea;

    yCapMin = 16;
    yCapMax = 49;
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = SHMOO_AND28_MAX_VREF_RANGE;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).yJump = 0;
    (*scPtr).shmooType = SHMOO_AND28_RD_EXTENDED;

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, &data);
    rd_dqs_pos0 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP);

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, &data);
    rd_dqs_pos1 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, VDL_STEP);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, &data);
        rd_dqs_pos2 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP);

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, &data);
        rd_dqs_pos3 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, VDL_STEP);
    }
    #endif

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, &data);
    rd_en_pos0 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, &data);
    rd_en_pos1 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, &data);
        rd_en_pos2 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, &data);
        rd_en_pos3 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
    }
    #endif

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    x = 0;
    data = 0;
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, x);

    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, data);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, data);
    }
    #endif

    rd_dqs_delta0 = x - rd_dqs_pos0;
    rd_dqs_delta1 = x - rd_dqs_pos1;

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        rd_dqs_delta2 = x - rd_dqs_pos2;
        rd_dqs_delta3 = x - rd_dqs_pos3;
    }
    #endif

    temp = rd_en_pos0 + rd_dqs_delta0;
    data = 0;
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
    if(temp & 0x80000000)
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
    }
    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
    }
    else
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
    }
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);

    temp = rd_en_pos1 + rd_dqs_delta1;
    data = 0;
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
    if(temp & 0x80000000)
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
    }
    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
    }
    else
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
    }
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        temp = rd_en_pos2 + rd_dqs_delta2;
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
        if(temp & 0x80000000)
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
        }
        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
        }
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);

        temp = rd_en_pos3 + rd_dqs_delta3;
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
        if(temp & 0x80000000)
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
        }
        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
        }
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
    }
    #endif

    rd_dq_fail_count0 = 0;
    rd_dq_fail_count1 = 0;
    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        rd_dq_fail_count2 = 0;
        rd_dq_fail_count3 = 0;
    }
    #endif

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, FORCE, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, VDL_STEP, x);
        if(rd_dq_fail_count0 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN, data);
            #endif
        }
        if(rd_dq_fail_count1 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN, data);
            #endif
        }

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            if(rd_dq_fail_count2 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMN, data);
                #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCN, data);
                #endif
            }
            if(rd_dq_fail_count3 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMN, data);
                #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCN, data);
                #endif
            }
        }
        #endif

        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
        }
        #endif

        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        _run_bist(unit, phy_ndx, scPtr, &sea);

        if((sea[0] & 0x000000FF) == 0x000000FF)
        {
            rd_dq_fail_count0++;
        }
        if((sea[0] & 0x0000FF00) == 0x0000FF00)
        {
            rd_dq_fail_count1++;
        }

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            if((sea[0] & 0x00FF0000) == 0x00FF0000)
            {
                rd_dq_fail_count2++;
            }
            if((sea[0] & 0xFF000000) == 0xFF000000)
            {
                rd_dq_fail_count3++;
            }
        }
        #endif

        if((rd_dq_fail_count0 > SHMOO_AND28_RD_DQ_FAIL_CAP) && (rd_dq_fail_count1 > SHMOO_AND28_RD_DQ_FAIL_CAP))
        {
            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                if((rd_dq_fail_count2 > SHMOO_AND28_RD_DQ_FAIL_CAP) && (rd_dq_fail_count3 > SHMOO_AND28_RD_DQ_FAIL_CAP))
                {
                    break;
                }
            }
            else
            {
                break;
            }
            #else
                break;
            #endif
        }
    }

    for(y = yCapMin; y < yCapMax; y++)
    {
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, &data);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, y);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, y);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, x);

            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, data);
            }
            #endif

            rd_dqs_delta0 = x - rd_dqs_pos0;
            rd_dqs_delta1 = x - rd_dqs_pos1;

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                rd_dqs_delta2 = x - rd_dqs_pos2;
                rd_dqs_delta3 = x - rd_dqs_pos3;
            }
            #endif

            temp = rd_en_pos0 + rd_dqs_delta0;
            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            if(temp & 0x80000000)
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
            }
            else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
            }
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);

            temp = rd_en_pos1 + rd_dqs_delta1;
            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            if(temp & 0x80000000)
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
            }
            else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
            }
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                temp = rd_en_pos2 + rd_dqs_delta2;
                data = 0;
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                if(temp & 0x80000000)
                {
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                }
                else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                {
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                }
                else
                {
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                }
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);

                temp = rd_en_pos3 + rd_dqs_delta3;
                data = 0;
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                if(temp & 0x80000000)
                {
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                }
                else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                {
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                }
                else
                {
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                }
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
            }
            #endif

            sal_usleep(SHMOO_AND28_SHORT_SLEEP);

            _run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

static int
_shmoo_and28_addr_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and28_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND28_ADDR_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, x);

        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09, data);

        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
        }
        #endif

        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        _run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }

    return SOC_E_NONE;
}

static int
_shmoo_and28_ctrl_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and28_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND28_CTRL_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, x);

        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N, data);

        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
        }
        #endif

        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        _run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }

    return SOC_E_NONE;
}

static int
_shmoo_and28_do(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
            return _shmoo_and28_rd_en(unit, phy_ndx, scPtr);
        case SHMOO_AND28_RD_EXTENDED:
            return _shmoo_and28_rd_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND28_WR_EXTENDED:
            return _shmoo_and28_wr_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND28_ADDR_EXTENDED:
            return _shmoo_and28_addr_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND28_CTRL_EXTENDED:
            if(!SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _shmoo_and28_ctrl_extended(unit, phy_ndx, scPtr);
            }
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

static int
_calib_2D(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 calibMode, uint32 calibPos)
{
    uint32 x;
    uint32 y;
    uint32 xStart;
    uint32 sizeX;
    uint32 calibStart;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 i;
    uint32 iter;
    uint32 shiftAmount;
    uint32 dataMask;
    int32 passStart;
    int32 failStart;
    int32 passStartSeen;
    int32 failStartSeen;
    int32 passLength;
    int32 maxPassStart;
    int32 maxPassLength;
    int32 maxMidPointX;
    uint32 maxPassLengthArray[SHMOO_AND28_WORD];

    xStart = 0;
    sizeX = (*scPtr).sizeX;
    calibStart = (*scPtr).calibStart;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;

    switch(calibMode)
    {
        case SHMOO_AND28_BIT:
            iter = shmoo_dram_info_ptr->interface_bitwidth;
            shiftAmount = 0;
            dataMask = 0x1;
            break;
        case SHMOO_AND28_BYTE:
            iter = shmoo_dram_info_ptr->interface_bitwidth >> 3;
            shiftAmount = 3;
            dataMask = 0xFF;
            break;
        case SHMOO_AND28_HALFWORD:
            iter = shmoo_dram_info_ptr->interface_bitwidth >> 4;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            break;
        case SHMOO_AND28_WORD:
            iter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            break;
        default:
            printf("Unsupported 2D calibration mode: %02u\n", calibMode);
            return SOC_E_FAIL;
    }

    for(i = 0; i < iter; i++)
    {
        (*scPtr).resultData[i] = 0;
        maxPassLengthArray[i] = 0;
    }

    for(y = yCapMin; y < yCapMax; y++)
    {
        for(i = 0; i < iter; i++)
        {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            passStartSeen = -1;
            failStartSeen = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPointX = -2;
            for(x = calibStart; x < sizeX; x++)
            {
                if(((*scPtr).result2D[xStart + x] >> (i << shiftAmount)) & dataMask)
                {   /* FAIL */
                    if(failStart < 0) {
                        failStart = x;
                        if(maxPassLength < passLength)
                        {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                        if((failStartSeen < 0) && (maxPassLength > 0))
                        {
                            failStartSeen = x;
                        }
                    }
                }
                else
                {   /* PASS */
                    if(passStart < 0)
                    {
                        passStart = x;
                        passLength = 1;
                        failStart = -1;
                        if((passStartSeen < 0) && (passLength < x))
                        {
                            passStartSeen = x;
                        }
                    }
                    else
                    {
                        passLength++;
                    }

                    if(x == sizeX - 1)
                    {
                        if(maxPassLength < passLength)
                        {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                }
            }

            switch (calibPos) {
                case SHMOO_AND28_CALIB_FAIL_START:
                case SHMOO_AND28_CALIB_RISING_EDGE:
                    if(failStartSeen > 0)
                    {
                        maxMidPointX = failStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_AND28_CALIB_PASS_START:
                case SHMOO_AND28_CALIB_FALLING_EDGE:
                    if(passStartSeen > 0)
                    {
                        maxMidPointX = passStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_AND28_CALIB_CENTER_PASS:
                    if((maxPassLength > 0) && (maxPassLengthArray[i] < maxPassLength))
                    {
                        maxMidPointX = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                        maxPassLengthArray[i] = maxPassLength;
                    }
                    break;
                case SHMOO_AND28_CALIB_VDL_ZERO:
                    maxMidPointX = 0;
                    (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    break;
                default:
                    printf("Unsupported calibration position: %02u\n", calibPos);
                    return SOC_E_FAIL;
            }
        }
        xStart += sizeX;
    }

    (*scPtr).calibMode = calibMode;
    (*scPtr).calibPos = calibPos;

    return SOC_E_NONE;
}

static int
_shmoo_and28_calib_2D(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_RD_EXTENDED:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_WR_EXTENDED:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_ADDR_EXTENDED:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_WORD, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_CTRL_EXTENDED:
            if(!SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_WORD, SHMOO_AND28_CALIB_CENTER_PASS);
            }
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

static int
_shmoo_and28_set_new_step(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 calibMode;
/*    uint32 engageUIshift; */
    uint32 val, yVal;
    uint32 data, temp;
    uint32 rd_dqs_pos0, rd_dqs_pos1, rd_en_pos0, rd_en_pos1, rd_dqs_delta0, rd_dqs_delta1;
    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        uint32 rd_dqs_pos2, rd_dqs_pos3, rd_en_pos2, rd_en_pos3, rd_dqs_delta2, rd_dqs_delta3;
    #endif

    calibMode = (*scPtr).calibMode;
/*    engageUIshift = (*scPtr).engageUIshift; */

    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);

                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);

                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
                    }
                    #endif

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_HALFWORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
                    }
                    #endif

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
                    }
                    #endif

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_RD_EXTENDED:
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, &data);
            rd_dqs_pos0 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP);

            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, &data);
            rd_dqs_pos1 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, VDL_STEP);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, &data);
                rd_dqs_pos2 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP);

                DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, &data);
                rd_dqs_pos3 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, VDL_STEP);
            }
            #endif

            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, &data);
            rd_en_pos0 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, &data);
            rd_en_pos1 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, &data);
                rd_en_pos2 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

                DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, &data);
                rd_en_pos3 = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
            }
            #endif

            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, val);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, data);

                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);

                    val = (*scPtr).resultData[1] & 0xFFFF;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, VDL_STEP, val);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, data);

                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        val = (*scPtr).resultData[2] & 0xFFFF;
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP, val);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, data);

                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);

                        val = (*scPtr).resultData[3] & 0xFFFF;
                        rd_dqs_delta3 = val - rd_dqs_pos3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, VDL_STEP, val);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, data);

                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
                    }
                    #endif

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)
                                + (((*scPtr).resultData[2] >> 16) & 0xFFFF) + (((*scPtr).resultData[3] >> 16) & 0xFFFF)) >> 2;
                    }
                    else
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    }
                    #else
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    #endif
                    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, &data);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, yVal);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_HALFWORD:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, val);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, data);

                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);

                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        val = (*scPtr).resultData[1] & 0xFFFF;
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        rd_dqs_delta3 = val - rd_dqs_pos3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP, val);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, data);

                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);

                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
                    }
                    #endif

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    }
                    else
                    {
                        yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    }
                    #else
                        yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    #endif
                    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, &data);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, yVal);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_WORD:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, val);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, data);

                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);

                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        rd_dqs_delta3 = val - rd_dqs_pos3;

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, data);

                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);

                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
                    }
                    #endif

                    yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, &data);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, yVal);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_WR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM, data);
                    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC, data);
                    #endif

                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_1, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM, data);
                    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC, data);
                    #endif

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM, data);
                        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDC, data);
                        #endif

                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_WR_DQ0, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_3, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM, data);
                        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDC, data);
                        #endif
                    }
                    #endif

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_HALFWORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM, data);
                    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC, data);
                    #endif
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM, data);
                    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC, data);
                    #endif

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM, data);
                        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDC, data);
                        #endif
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM, data);
                        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDC, data);
                        #endif
                    }
                    #endif

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM, data);
                    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC, data);
                    #endif
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM, data);
                    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC, data);
                    #endif

                    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
                    {
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM, data);
                        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDC, data);
                        #endif
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM, data);
                        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDC, data);
                        #endif
                    }
                    #endif

                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
                case SHMOO_AND28_HALFWORD:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
                case SHMOO_AND28_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
                    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09, data);
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            if(SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00, &data);
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);

                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N, data);
                sal_usleep(SHMOO_AND28_SHORT_SLEEP);
            }
            else
            {
                switch(calibMode)
                {
                    case SHMOO_AND28_BIT:
                        printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                        return SOC_E_FAIL;
                    case SHMOO_AND28_BYTE:
                        printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                        return SOC_E_FAIL;
                    case SHMOO_AND28_HALFWORD:
                        printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                        return SOC_E_FAIL;
                    case SHMOO_AND28_WORD:
                        data = 0;
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
                        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT, data);
                        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N, data);
                        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                        break;
                    default:
                        printf("Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n", (*scPtr).shmooType, calibMode);
                        return SOC_E_FAIL;
                }
            }
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }

    data = 0;
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
    }
    #endif

    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    return SOC_E_NONE;
}

#ifdef PLOT_SUPPORT
static int
_plot(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 plotMode)
{
    uint32 x;
    uint32 y;
    uint32 xStart;
    uint32 sizeX;
    uint32 sizeY;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 yJump;
    uint32 i;
    uint32 ui;
    uint32 iter;
    uint32 shiftAmount;
    uint32 dataMask;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    uint32 engageUIshift;
    uint32 step1000;
    uint32 size1000UI;
    uint32 calibShiftAmount;
    uint32 maxMidPointX;
    uint32 maxMidPointY;
    char str0[SHMOO_AND28_STRING_LENGTH];
    char str1[SHMOO_AND28_STRING_LENGTH];
    char str2[SHMOO_AND28_STRING_LENGTH];
    char pass_low[2];
    char fail_high[2];
    char outOfSearch[2];

    outOfSearch[0] = ' ';
    outOfSearch[1] = 0;

    sizeX = (*scPtr).sizeX;
    sizeY = (*scPtr).sizeY;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;
    yJump = (*scPtr).yJump;
    calibMode = (*scPtr).calibMode;
    calibPos = (*scPtr).calibPos;
    calibStart = (*scPtr).calibStart;
    engageUIshift = (*scPtr).engageUIshift;
    step1000 = (*scPtr).step1000;
    size1000UI = (*scPtr).size1000UI;

    switch (calibPos) {
        case SHMOO_AND28_CALIB_RISING_EDGE:
        case SHMOO_AND28_CALIB_FALLING_EDGE:
            pass_low[0] = '_';
            pass_low[1] = 0;
            fail_high[0] = '|';
            fail_high[1] = 0;
            break;
        case SHMOO_AND28_CALIB_CENTER_PASS:
        case SHMOO_AND28_CALIB_PASS_START:
        case SHMOO_AND28_CALIB_FAIL_START:
        case SHMOO_AND28_CALIB_VDL_ZERO:
            pass_low[0] = '+';
            pass_low[1] = 0;
            fail_high[0] = '-';
            fail_high[1] = 0;
            break;
        default:
            printf("Unsupported calibration position: %02u\n", calibPos);
            return SOC_E_FAIL;
    }

    printf("\n\n");

    switch(plotMode)
    {
        case SHMOO_AND28_BIT:
            iter = shmoo_dram_info_ptr->interface_bitwidth;
            shiftAmount = 0;
            dataMask = 0x1;
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    calibShiftAmount = 3;
                    break;
                case SHMOO_AND28_HALFWORD:
                    calibShiftAmount = 4;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 5;
                    break;
                default:
                    printf("Unsupported calibration mode during plot: %02u\n", calibMode);
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_BYTE:
            iter = shmoo_dram_info_ptr->interface_bitwidth >> 3;
            shiftAmount = 3;
            dataMask = 0xFF;
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("WARNING: Plot mode coerced from byte mode to bit mode\n");
                    iter = shmoo_dram_info_ptr->interface_bitwidth;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_HALFWORD:
                    calibShiftAmount = 1;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 2;
                    break;
                default:
                    printf("Unsupported calibration mode during plot: %02u\n", calibMode);
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_HALFWORD:
            iter = shmoo_dram_info_ptr->interface_bitwidth >> 4;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("WARNING: Plot mode coerced from halfword mode to bit mode\n");
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    printf("WARNING: Plot mode coerced from halfword mode to byte mode\n");
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_HALFWORD:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 1;
                    break;
                default:
                    printf("Unsupported calibration mode during plot: %02u\n", calibMode);
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_WORD:
            iter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("WARNING: Plot mode coerced from word mode to bit mode\n");
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    printf("WARNING: Plot mode coerced from word mode to byte mode\n");
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_HALFWORD:
                    printf("WARNING: Plot mode coerced from word mode to halfword mode\n");
                    iter = 2;
                    shiftAmount = 4;
                    dataMask = 0xFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 0;
                    break;
                default:
                    printf("Unsupported calibration mode during plot: %02u\n", calibMode);
                    return SOC_E_FAIL;
            }
            break;
        default:
            printf("Unsupported plot mode: %02u\n", plotMode);
            return SOC_E_FAIL;
    }
/*
    if(engageUIshift)
    { */
        ui = 0;

        for(x = 0; x < sizeX; x++)
        {
            if((ui < SHMOO_AND28_MAX_VISIBLE_UI_COUNT) && (x > (*scPtr).endUI[ui]))
            {
                str0[x] = ' ';
                str1[x] = ' ';
                str2[x] = ' ';
                ui++;
            }
            else
            {
                str0[x] = '0' + (x / 100);
                str1[x] = '0' + ((x % 100) / 10);
                str2[x] = '0' + (x % 10);
            }
        }
/*    }
    else
    {
        for(x = 0; x < sizeX; x++)
        {
            str0[x] = '0' + (x / 100);
            str1[x] = '0' + ((x % 100) / 10);
            str2[x] = '0' + (x % 10);
        }
    } */

    str0[x] = 0;
    str1[x] = 0;
    str2[x] = 0;

    for(i = 0; i < iter; i++)
    {
        xStart = 0;
        maxMidPointX = (*scPtr).resultData[i >> calibShiftAmount] & 0xFFFF;
        maxMidPointY = ((*scPtr).resultData[i >> calibShiftAmount] >> 16) & 0xFFFF;

        if((sizeY > 1) || (i == 0))
        {
            printf("***** Interface.......: %3d\n", phy_ndx);
            printf(" **** VDL step size...: %3u.%03u ps\n", (step1000 / 1000), (step1000 % 1000));
            printf(" **** UI size.........: %3u.%03u steps\n", (size1000UI / 1000), (size1000UI % 1000));

            switch((*scPtr).shmooType)
            {
                case SHMOO_AND28_RD_EN:
                    printf(" **** Shmoo type......: RD_EN\n");
                    break;
                case SHMOO_AND28_RD_EXTENDED:
                    printf(" **** Shmoo type......: RD_EXTENDED\n");
                    break;
                case SHMOO_AND28_WR_EXTENDED:
                    printf(" **** Shmoo type......: WR_EXTENDED\n");
                    break;
                case SHMOO_AND28_ADDR_EXTENDED:
                    printf(" **** Shmoo type......: ADDR_EXTENDED\n");
                    break;
                case SHMOO_AND28_CTRL_EXTENDED:
                    printf(" **** Shmoo type......: CTRL_EXTENDED\n");
                    printf("  *** Quick Shmoo.....: Off\n");
                    break;
                default:
                    printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
                    return SOC_E_FAIL;
            }

            if(engageUIshift)
            {
                printf("  *** UI shift........: On\n");
            }
            else
            {
                printf("  *** UI shift........: Off or N/A\n");
            }
        }

        if(sizeY > 1)
        {
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    printf("  *** Calib mode......: 2D Bit-wise\n");
                    break;
                case SHMOO_AND28_BYTE:
                    printf("  *** Calib mode......: 2D Byte-wise\n");
                    break;
                case SHMOO_AND28_HALFWORD:
                    printf("  *** Calib mode......: 2D Halfword-wise\n");
                    break;
                case SHMOO_AND28_WORD:
                    printf("  *** Calib mode......: 2D Word-wise\n");
                    break;
                default:
                    printf("Unsupported calibration mode during plot: %02u\n", calibMode);
                    return SOC_E_FAIL;
            }

            switch(plotMode)
            {
                case SHMOO_AND28_BIT:
                    printf("  *** Plot mode.......: 2D Bit-wise\n");
                    printf("   ** Bit.............: %03u\n", i);
                    break;
                case SHMOO_AND28_BYTE:
                    printf("  *** Plot mode.......: 2D Byte-wise\n");
                    printf("   ** Byte............: %03u\n", i);
                    break;
                case SHMOO_AND28_HALFWORD:
                    printf("  *** Plot mode.......: 2D Halfword-wise\n");
                    printf("   ** Halfword........: %03u\n", i);
                    break;
                case SHMOO_AND28_WORD:
                    printf("  *** Plot mode.......: 2D Word-wise\n");
                    printf("   ** Word............: %03u\n", i);
                    break;
                default:
                    printf("Unsupported plot mode: %02u\n", plotMode);
                    return SOC_E_FAIL;
            }

            printf("    * Center X........: %03u\n", maxMidPointX);
            printf("    * Center Y........: %03u\n", maxMidPointY);
            printf("      %s\n", str0);
            printf("      %s\n", str1);
            printf("      %s\n", str2);

            for(y = yCapMin; y < yCapMax; y++)
            {
                printf("  %03u ", y << yJump);

                for(x = 0; x < calibStart; x++)
                {
                    printf("%s", outOfSearch);
                }

                for(x = calibStart; x < sizeX; x++)
                {
                    if(((*scPtr).result2D[xStart + x] >> (i << shiftAmount)) & dataMask)
                    {   /* FAIL - RISING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR FAIL */
                            printf("%s", fail_high);
                        }
                        else
                        {   /* FAIL - RISING EDGE */
                            if((calibPos == SHMOO_AND28_CALIB_RISING_EDGE) || (calibPos == SHMOO_AND28_CALIB_FAIL_START))
                            {   /* RISING EDGE */
                                printf("X");
                            }
                            else
                            {   /* FAIL */
                                printf("%s", fail_high);
                            }
                        }
                    }
                    else
                    {   /* PASS - MIDPOINT - FALLING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR PASS */
                            printf("%s", pass_low);
                        }
                        else
                        {   /* POTENTIAL MIDPOINT - FALLING EDGE */
                            if(y == maxMidPointY)
                            {   /* MID POINT - FALLING EDGE */
                                printf("X");
                            }
                            else
                            {   /* PASS */
                                printf("%s", pass_low);
                            }
                        }
                    }
                }
                printf("\n");
                xStart += sizeX;
            }
            printf("\n");
        }
        else
        {
            if(i == 0)
            {
                switch(calibMode)
                {
                    case SHMOO_AND28_BIT:
                        printf("  *** Calib mode......: 1D Bit-wise\n");
                        break;
                    case SHMOO_AND28_BYTE:
                        printf("  *** Calib mode......: 1D Byte-wise\n");
                        break;
                    case SHMOO_AND28_HALFWORD:
                        printf("  *** Calib mode......: 1D Halfword-wise\n");
                        break;
                    case SHMOO_AND28_WORD:
                        printf("  *** Calib mode......: 1D Word-wise\n");
                        break;
                    default:
                        printf("Unsupported calibration mode during plot: %02u\n", calibMode);
                        return SOC_E_FAIL;
                }

                switch(plotMode)
                {
                    case SHMOO_AND28_BIT:
                        printf("  *** Plot mode.......: 1D Bit-wise\n");
                        break;
                    case SHMOO_AND28_BYTE:
                        printf("  *** Plot mode.......: 1D Byte-wise\n");
                        break;
                    case SHMOO_AND28_HALFWORD:
                        printf("  *** Plot mode.......: 1D Halfword-wise\n");
                        break;
                    case SHMOO_AND28_WORD:
                        printf("  *** Plot mode.......: 1D Word-wise\n");
                        break;
                    default:
                        printf("Unsupported plot mode: %02u\n", plotMode);
                        return SOC_E_FAIL;
                }
                printf("      %s\n", str0);
                printf("      %s\n", str1);
                printf("      %s\n", str2);
            }

            printf("  %03u ", i);

            for(x = 0; x < calibStart; x++)
            {
                printf("%s", outOfSearch);
            }

            for(x = calibStart; x < sizeX; x++)
            {
                if(((*scPtr).result2D[x] >> (i << shiftAmount)) & dataMask)
                {   /* FAIL - RISING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR FAIL */
                        printf("%s", fail_high);
                    }
                    else
                    {   /* FAIL - RISING EDGE */
                        if((calibPos == SHMOO_AND28_CALIB_RISING_EDGE) || (calibPos == SHMOO_AND28_CALIB_FAIL_START))
                        {   /* RISING EDGE */
                            printf("X");
                        }
                        else
                        {   /* FAIL */
                            printf("%s", fail_high);
                        }
                    }
                }
                else
                {   /* PASS - MIDPOINT - FALLING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR PASS */
                        printf("%s", pass_low);
                    }
                    else
                    {   /* MID POINT - FALLING EDGE */
                        printf("X");
                    }
                }
            }
            printf("\n");
        }
    }
    printf("\n");

    return SOC_E_NONE;
}

static int
_shmoo_and28_plot(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
            return _plot(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE);
        case SHMOO_AND28_RD_EXTENDED:
            return _plot(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE);
        case SHMOO_AND28_WR_EXTENDED:
            return _plot(unit, phy_ndx, scPtr, SHMOO_AND28_BIT);
        case SHMOO_AND28_ADDR_EXTENDED:
            return _plot(unit, phy_ndx, scPtr, SHMOO_AND28_WORD);
        case SHMOO_AND28_CTRL_EXTENDED:
            if(!SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _plot(unit, phy_ndx, scPtr, SHMOO_AND28_WORD);
            }
            else
            {
                printf("\n\n");
                printf("***** Interface.......: %3d\n", phy_ndx);
                printf(" **** Shmoo type......: CTRL_EXTENDED\n");
                printf("  *** Quick Shmoo.....: On\n");
                printf("  *** Plot............: Off\n");
                printf("  *** Copying.........: VDL_STEP\n");
                printf("   ** From............: AD00 - AD09\n");
                printf("   ** To..............: AD10 - WE_N\n");
                printf("\n");
            }
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}
#endif

int
_and28_calculate_step_size(int unit, int phy_ndx, and28_step_size_t *ssPtr)
{
    uint32 data;
    uint32 timeout;

    if(shmoo_dram_info_ptr->sim_system_mode)
    {
        (*ssPtr).step1000 = 8000;
        (*ssPtr).size1000UI = 78125;

        return SOC_E_NONE;
    }

    data = 0;
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CALIBRATE, CALIB_ONCE, 1);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CALIBRATE, UPDATE_REGS, 0);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CALIBRATE, UPDATE_FAST, 0);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    timeout = 2000;
    do
    {
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIB_STATUS1, &data);

        if(DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CALIB_STATUS1, CALIB_IDLE))
        {
        /*    printf("     VDL calibration complete.\n"); */
            break;
        }

        if (timeout == 0)
        {
            printf("     VDL calibration failed!!! (Timeout)\n");
            return SOC_E_TIMEOUT;
        }

        timeout--;
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    }
    while(TRUE);

    if(DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CALIB_STATUS1, CALIB_LOCK_4B) == 0)
    {
        printf("     VDL calibration failed!!! (No lock)\n");
        return SOC_E_FAIL;
    }

    (*ssPtr).size1000UI = DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CALIB_STATUS1, CALIB_TOTAL_STEPS) * 500;
    (*ssPtr).step1000 = ((1000000000 / shmoo_dram_info_ptr->data_rate_mbps) * 1000) / ((*ssPtr).size1000UI);

    data = 0;
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CALIBRATE, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    return SOC_E_NONE;
}

int
_and28_zq_calibration(int unit, int phy_ndx)
{
    int i;
    uint32 data;
    uint32 p_drive, n_drive;
    uint32 p_term, n_term;
    uint32 p_idle, n_idle;

    p_drive = 16;
    n_drive = 16;

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, &data);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_IDDQ, 0);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, 0);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, 31);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 0);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    for(i = 0; i < SHMOO_AND28_MAX_ZQ_CAL_RANGE; i++)
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, i);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, &data);
        if(DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_STATUS))
        {
            p_drive = i;
            break;
        }
    }

    if(i == SHMOO_AND28_MAX_ZQ_CAL_RANGE)
    {
        printf("     WARNING: ZQ calibration error (P) - Manual IO programming required for correct operation\n");
        /* return SOC_E_FAIL; */
    }

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 0);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 0);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, 31);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, 0);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 0);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    for(i = 0; i < SHMOO_AND28_MAX_ZQ_CAL_RANGE; i++)
    {
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, i);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, &data);
        if(DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_STATUS))
        {
            n_drive = i;
            break;
        }
    }

    if(i == SHMOO_AND28_MAX_ZQ_CAL_RANGE)
    {
        printf("     WARNING: ZQ calibration error (N) - Manual IO programming required for correct operation\n");
        /* return SOC_E_FAIL; */
    }

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_IDDQ, 0);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, 0);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, 0);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_ZQ_CAL, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    p_term = 6;
    n_term = 6;

    p_idle = 0;
    n_idle = 0;

    data = 0;
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_IDLE_STRENGTH, p_idle);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_IDLE_STRENGTH, n_idle);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_TERM_STRENGTH, p_term);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_TERM_STRENGTH, n_term);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_STRENGTH, p_drive);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_STRENGTH, n_drive);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DRIVE_PAD_CTL, data);

    data = 0;
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_IDLE_STRENGTH, p_idle);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_IDLE_STRENGTH, n_idle);
    #endif
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);

    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_DRIVE_PAD_CTL, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_DRIVE_PAD_CTL, data);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_DRIVE_PAD_CTL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_DRIVE_PAD_CTL, data);
    }
    #endif

    #ifdef PHY_AND28_F0
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);

        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_DQSP_DRIVE_PAD_CTL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_DQSN_DRIVE_PAD_CTL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_DQSP_DRIVE_PAD_CTL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_DQSN_DRIVE_PAD_CTL, data);

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_DQSP_DRIVE_PAD_CTL, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_DQSN_DRIVE_PAD_CTL, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_DQSP_DRIVE_PAD_CTL, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_DQSN_DRIVE_PAD_CTL, data);
        }
        #endif

        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);

        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_ALERT_DRIVE_PAD_CTL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_ALERT_DRIVE_PAD_CTL, data);

        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_ALERT_DRIVE_PAD_CTL, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_ALERT_DRIVE_PAD_CTL, data);
        }
        #endif
    #endif

    data = 0;
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, EDC_RD_EN_PD_STRENGTH, p_drive);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, EDC_RD_EN_ND_STRENGTH, n_drive);
    #endif
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, BL_RD_EN_PD_STRENGTH, p_drive);
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, BL_RD_EN_ND_STRENGTH, n_drive);

    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTL, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTL, data);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_RD_EN_DRIVE_PAD_CTL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_RD_EN_DRIVE_PAD_CTL, data);
    }
    #endif

    printf("     P drive..........: 0x%02X\n", p_drive);
    printf("     N drive..........: 0x%02X\n", n_drive);
    printf("     P termination....: 0x%02X\n", p_term);
    printf("     N termination....: 0x%02X\n", n_term);
    printf("     P idle...........: 0x%02X\n", p_idle);
    printf("     N idle...........: 0x%02X\n", n_idle);

    return SOC_E_NONE;
}

static int
_shmoo_and28_entry(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential entry
     * Mode 1: Single entry
     */

    uint32 i;
    uint32 data, temp;
    and28_step_size_t ss;

    (*scPtr).calibStart = 0;

    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
    /*A04*/ printf("R04. Configure reference voltage\n");
    /*R04*/ DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, &data);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, 32);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, 32);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);
            sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    /*A08*/ printf("R08. ZQ calibration\n");
    /*R08*/ if(shmoo_dram_info_ptr->sim_system_mode)
            {
                printf("     Skipped for emulation\n");

                goto SHMOO_AND28_RD_EN_ZQ_CALIBRATION_END;
            }

            _and28_zq_calibration(unit, phy_ndx);

            SHMOO_AND28_RD_EN_ZQ_CALIBRATION_END:

            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 3) / 1000;      /* 300% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            (*scPtr).yJump = 2;
            temp = temp >> (*scPtr).yJump;
            if(temp > SHMOO_AND28_MAX_VREF_RANGE)
            {
                (*scPtr).sizeY = SHMOO_AND28_MAX_VREF_RANGE;
            }
            else
            {
                (*scPtr).sizeY = temp;
            }

            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_CONTROL, MODE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_CONTROL, RD_DATA_DLY, SHMOO_AND28_RD_DATA_DLY_INIT);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_CONTROL, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_CONTROL, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS0_CYCLES, SHMOO_AND28_RD_EN_CYC_INIT);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS1_CYCLES, SHMOO_AND28_RD_EN_CYC_INIT);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_RD_EN_DLY_CYC, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_RD_EN_DLY_CYC, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_RD_EN_VDL_INIT);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS1, FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS1, VDL_STEP, SHMOO_AND28_RD_EN_VDL_INIT);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, VDL_STEP, SHMOO_AND28_RD_DQ_VDL_INIT);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCN, data);
            #endif
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCN, data);
            #endif

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMN, data);
                #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCN, data);
                #endif
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7P, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7N, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMN, data);
                #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCP, data);
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCN, data);
                #endif
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
            temp = (((*scPtr).size1000UI * 3) / 4000) + SHMOO_AND28_RD_DQS_VDL_OFFSET;       /* 75% + Offset */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, temp);
            }
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, WR_CHAN_DLY_CYC, FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, WR_CHAN_DLY_CYC, CYCLES, SHMOO_AND28_WR_CYC_INIT);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC, data);

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_WR_CHAN_DLY_CYC, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_WR_CHAN_DLY_CYC, data);
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
            temp = (*scPtr).size1000UI / 2000;
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, temp);
            }
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC, data);
            #endif
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM, data);
            #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC, data);
            #endif

            #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info_ptr->interface_bitwidth == 32)
            {
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM, data);
                #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDC, data);
                #endif
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7, data);
                DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM, data);
                #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
                    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDC, data);
                #endif
            }
            #endif

            sal_usleep(SHMOO_AND28_SHORT_SLEEP);
            break;
        case SHMOO_AND28_RD_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND28_WR_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

static int
_shmoo_and28_exit(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential exit
     * Mode 1: Single exit
     */

    switch ((*scPtr).shmooType) {
        case SHMOO_AND28_RD_EN:
            break;
        case SHMOO_AND28_RD_EXTENDED:
            break;
        case SHMOO_AND28_WR_EXTENDED:
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            break;
        default:
            printf("Unsupported shmoo type: %02u\n", (*scPtr).shmooType);
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

static int
_shmoo_and28_save(int unit, int phy_ndx, and28_shmoo_config_param_t *config_param)
{
    uint32 data;

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00, &data);
    (*config_param).control_regs_ad[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01, &data);
    (*config_param).control_regs_ad[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02, &data);
    (*config_param).control_regs_ad[2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03, &data);
    (*config_param).control_regs_ad[3] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04, &data);
    (*config_param).control_regs_ad[4] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05, &data);
    (*config_param).control_regs_ad[5] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06, &data);
    (*config_param).control_regs_ad[6] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07, &data);
    (*config_param).control_regs_ad[7] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08, &data);
    (*config_param).control_regs_ad[8] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09, &data);
    (*config_param).control_regs_ad[9] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10, &data);
    (*config_param).control_regs_ad[10] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11, &data);
    (*config_param).control_regs_ad[11] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12, &data);
    (*config_param).control_regs_ad[12] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13, &data);
    (*config_param).control_regs_ad[13] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14, &data);
    (*config_param).control_regs_ad[14] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15, &data);
    (*config_param).control_regs_ad[15] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0, &data);
    (*config_param).control_regs_ba[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1, &data);
    (*config_param).control_regs_ba[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2, &data);
    (*config_param).control_regs_ba[2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0, &data);
    (*config_param).control_regs_aux[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1, &data);
    (*config_param).control_regs_aux[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2, &data);
    (*config_param).control_regs_aux[2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0, &data);
    (*config_param).control_regs_cs[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1, &data);
    (*config_param).control_regs_cs[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR, &data);
    (*config_param).control_regs_par = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N, &data);
    (*config_param).control_regs_ras_n = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N, &data);
    (*config_param).control_regs_cas_n = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE, &data);
    (*config_param).control_regs_cke = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N, &data);
    (*config_param).control_regs_rst_n = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT, &data);
    (*config_param).control_regs_odt = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N, &data);
    (*config_param).control_regs_we_n = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, &data);
    (*config_param).control_regs_vref_dac_control = data;

    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P, &data);
    #elif defined(PHY_AND28_F0)
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS, &data);
    #endif
    (*config_param).wr_vdl_dqsp[0] = (uint16) data;
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N, &data);
        (*config_param).wr_vdl_dqsn[0] = (uint16) data;
    #endif
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0, &data);
    (*config_param).wr_vdl_dq[0][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1, &data);
    (*config_param).wr_vdl_dq[0][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2, &data);
    (*config_param).wr_vdl_dq[0][2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3, &data);
    (*config_param).wr_vdl_dq[0][3] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4, &data);
    (*config_param).wr_vdl_dq[0][4] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5, &data);
    (*config_param).wr_vdl_dq[0][5] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6, &data);
    (*config_param).wr_vdl_dq[0][6] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7, &data);
    (*config_param).wr_vdl_dq[0][7] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM, &data);
    (*config_param).wr_vdl_dm[0] = (uint16) data;
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC, &data);
        (*config_param).wr_vdl_edc[0] = (uint16) data;
    #endif
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC, &data);
    (*config_param).wr_chan_dly_cyc[0] = (uint8) data;

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, &data);
    (*config_param).rd_vdl_dqsp[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, &data);
    (*config_param).rd_vdl_dqsn[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P, &data);
    (*config_param).rd_vdl_dqp[0][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P, &data);
    (*config_param).rd_vdl_dqp[0][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P, &data);
    (*config_param).rd_vdl_dqp[0][2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P, &data);
    (*config_param).rd_vdl_dqp[0][3] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P, &data);
    (*config_param).rd_vdl_dqp[0][4] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P, &data);
    (*config_param).rd_vdl_dqp[0][5] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P, &data);
    (*config_param).rd_vdl_dqp[0][6] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P, &data);
    (*config_param).rd_vdl_dqp[0][7] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N, &data);
    (*config_param).rd_vdl_dqn[0][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N, &data);
    (*config_param).rd_vdl_dqn[0][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N, &data);
    (*config_param).rd_vdl_dqn[0][2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N, &data);
    (*config_param).rd_vdl_dqn[0][3] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N, &data);
    (*config_param).rd_vdl_dqn[0][4] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N, &data);
    (*config_param).rd_vdl_dqn[0][5] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N, &data);
    (*config_param).rd_vdl_dqn[0][6] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N, &data);
    (*config_param).rd_vdl_dqn[0][7] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP, &data);
    (*config_param).rd_vdl_dmp[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN, &data);
    (*config_param).rd_vdl_dmn[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, &data);
    (*config_param).rd_en_vdl_cs[0][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, &data);
    (*config_param).rd_en_vdl_cs[0][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC, &data);
    (*config_param).rd_en_dly_cyc[0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL, &data);
    (*config_param).rd_control[0] = (uint8) data;

    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P, &data);
    #elif defined(PHY_AND28_F0)
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS, &data);
    #endif
    (*config_param).wr_vdl_dqsp[1] = (uint16) data;
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N, &data);
        (*config_param).wr_vdl_dqsn[1] = (uint16) data;
    #endif
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0, &data);
    (*config_param).wr_vdl_dq[1][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1, &data);
    (*config_param).wr_vdl_dq[1][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2, &data);
    (*config_param).wr_vdl_dq[1][2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3, &data);
    (*config_param).wr_vdl_dq[1][3] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4, &data);
    (*config_param).wr_vdl_dq[1][4] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5, &data);
    (*config_param).wr_vdl_dq[1][5] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6, &data);
    (*config_param).wr_vdl_dq[1][6] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7, &data);
    (*config_param).wr_vdl_dq[1][7] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM, &data);
    (*config_param).wr_vdl_dm[1] = (uint16) data;
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC, &data);
        (*config_param).wr_vdl_edc[1] = (uint16) data;
    #endif
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC, &data);
    (*config_param).wr_chan_dly_cyc[1] = (uint8) data;

    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, &data);
    (*config_param).rd_vdl_dqsp[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, &data);
    (*config_param).rd_vdl_dqsn[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P, &data);
    (*config_param).rd_vdl_dqp[1][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P, &data);
    (*config_param).rd_vdl_dqp[1][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P, &data);
    (*config_param).rd_vdl_dqp[1][2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P, &data);
    (*config_param).rd_vdl_dqp[1][3] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P, &data);
    (*config_param).rd_vdl_dqp[1][4] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P, &data);
    (*config_param).rd_vdl_dqp[1][5] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P, &data);
    (*config_param).rd_vdl_dqp[1][6] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P, &data);
    (*config_param).rd_vdl_dqp[1][7] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N, &data);
    (*config_param).rd_vdl_dqn[1][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N, &data);
    (*config_param).rd_vdl_dqn[1][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N, &data);
    (*config_param).rd_vdl_dqn[1][2] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N, &data);
    (*config_param).rd_vdl_dqn[1][3] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N, &data);
    (*config_param).rd_vdl_dqn[1][4] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N, &data);
    (*config_param).rd_vdl_dqn[1][5] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N, &data);
    (*config_param).rd_vdl_dqn[1][6] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N, &data);
    (*config_param).rd_vdl_dqn[1][7] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP, &data);
    (*config_param).rd_vdl_dmp[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN, &data);
    (*config_param).rd_vdl_dmn[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, &data);
    (*config_param).rd_en_vdl_cs[1][0] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, &data);
    (*config_param).rd_en_vdl_cs[1][1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC, &data);
    (*config_param).rd_en_dly_cyc[1] = (uint16) data;
    DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL, &data);
    (*config_param).rd_control[1] = (uint8) data;

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_P, &data);
        #elif defined(PHY_AND28_F0)
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS, &data);
        #endif
        (*config_param).wr_vdl_dqsp[2] = (uint16) data;
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_N, &data);
            (*config_param).wr_vdl_dqsn[2] = (uint16) data;
        #endif
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0, &data);
        (*config_param).wr_vdl_dq[2][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1, &data);
        (*config_param).wr_vdl_dq[2][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2, &data);
        (*config_param).wr_vdl_dq[2][2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3, &data);
        (*config_param).wr_vdl_dq[2][3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4, &data);
        (*config_param).wr_vdl_dq[2][4] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5, &data);
        (*config_param).wr_vdl_dq[2][5] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6, &data);
        (*config_param).wr_vdl_dq[2][6] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7, &data);
        (*config_param).wr_vdl_dq[2][7] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM, &data);
        (*config_param).wr_vdl_dm[2] = (uint16) data;
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDC, &data);
            (*config_param).wr_vdl_edc[2] = (uint16) data;
        #endif
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_WR_CHAN_DLY_CYC, &data);
        (*config_param).wr_chan_dly_cyc[2] = (uint8) data;

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, &data);
        (*config_param).rd_vdl_dqsp[2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, &data);
        (*config_param).rd_vdl_dqsn[2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0P, &data);
        (*config_param).rd_vdl_dqp[2][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1P, &data);
        (*config_param).rd_vdl_dqp[2][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2P, &data);
        (*config_param).rd_vdl_dqp[2][2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3P, &data);
        (*config_param).rd_vdl_dqp[2][3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4P, &data);
        (*config_param).rd_vdl_dqp[2][4] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5P, &data);
        (*config_param).rd_vdl_dqp[2][5] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6P, &data);
        (*config_param).rd_vdl_dqp[2][6] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7P, &data);
        (*config_param).rd_vdl_dqp[2][7] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0N, &data);
        (*config_param).rd_vdl_dqn[2][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1N, &data);
        (*config_param).rd_vdl_dqn[2][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2N, &data);
        (*config_param).rd_vdl_dqn[2][2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3N, &data);
        (*config_param).rd_vdl_dqn[2][3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4N, &data);
        (*config_param).rd_vdl_dqn[2][4] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5N, &data);
        (*config_param).rd_vdl_dqn[2][5] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6N, &data);
        (*config_param).rd_vdl_dqn[2][6] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7N, &data);
        (*config_param).rd_vdl_dqn[2][7] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMP, &data);
        (*config_param).rd_vdl_dmp[2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMN, &data);
        (*config_param).rd_vdl_dmn[2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, &data);
        (*config_param).rd_en_vdl_cs[2][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, &data);
        (*config_param).rd_en_vdl_cs[2][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_RD_EN_DLY_CYC, &data);
        (*config_param).rd_en_dly_cyc[2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_CONTROL, &data);
        (*config_param).rd_control[2] = (uint8) data;

        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_P, &data);
        #elif defined(PHY_AND28_F0)
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS, &data);
        #endif
        (*config_param).wr_vdl_dqsp[3] = (uint16) data;
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_N, &data);
            (*config_param).wr_vdl_dqsn[3] = (uint16) data;
        #endif
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0, &data);
        (*config_param).wr_vdl_dq[3][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1, &data);
        (*config_param).wr_vdl_dq[3][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2, &data);
        (*config_param).wr_vdl_dq[3][2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3, &data);
        (*config_param).wr_vdl_dq[3][3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4, &data);
        (*config_param).wr_vdl_dq[3][4] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5, &data);
        (*config_param).wr_vdl_dq[3][5] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6, &data);
        (*config_param).wr_vdl_dq[3][6] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7, &data);
        (*config_param).wr_vdl_dq[3][7] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM, &data);
        (*config_param).wr_vdl_dm[3] = (uint16) data;
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDC, &data);
            (*config_param).wr_vdl_edc[3] = (uint16) data;
        #endif
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_WR_CHAN_DLY_CYC, &data);
        (*config_param).wr_chan_dly_cyc[3] = (uint8) data;

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, &data);
        (*config_param).rd_vdl_dqsp[3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, &data);
        (*config_param).rd_vdl_dqsn[3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0P, &data);
        (*config_param).rd_vdl_dqp[3][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1P, &data);
        (*config_param).rd_vdl_dqp[3][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2P, &data);
        (*config_param).rd_vdl_dqp[3][2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3P, &data);
        (*config_param).rd_vdl_dqp[3][3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4P, &data);
        (*config_param).rd_vdl_dqp[3][4] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5P, &data);
        (*config_param).rd_vdl_dqp[3][5] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6P, &data);
        (*config_param).rd_vdl_dqp[3][6] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7P, &data);
        (*config_param).rd_vdl_dqp[3][7] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0N, &data);
        (*config_param).rd_vdl_dqn[3][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1N, &data);
        (*config_param).rd_vdl_dqn[3][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2N, &data);
        (*config_param).rd_vdl_dqn[3][2] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3N, &data);
        (*config_param).rd_vdl_dqn[3][3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4N, &data);
        (*config_param).rd_vdl_dqn[3][4] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5N, &data);
        (*config_param).rd_vdl_dqn[3][5] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6N, &data);
        (*config_param).rd_vdl_dqn[3][6] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7N, &data);
        (*config_param).rd_vdl_dqn[3][7] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMP, &data);
        (*config_param).rd_vdl_dmp[3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMN, &data);
        (*config_param).rd_vdl_dmn[3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, &data);
        (*config_param).rd_en_vdl_cs[3][0] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, &data);
        (*config_param).rd_en_vdl_cs[3][1] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_RD_EN_DLY_CYC, &data);
        (*config_param).rd_en_dly_cyc[3] = (uint16) data;
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_CONTROL, &data);
        (*config_param).rd_control[3] = (uint8) data;
    }
    #endif

    return SOC_E_NONE;
}

static int
_shmoo_and28_restore(int unit, int phy_ndx, and28_shmoo_config_param_t *config_param)
{
    uint32 data;

    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[3]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[4]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[5]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[6]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[7]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[8]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[9]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[10]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[11]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[12]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[13]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[14]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[15]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ba[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ba[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ba[2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_aux[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_aux[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_aux[2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cs[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cs[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_par);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ras_n);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cas_n);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cke);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_rst_n);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_odt);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_we_n);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N, data);
    data = SET_VREF_DAC_CONTROL((*config_param).control_regs_vref_dac_control);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);

    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[0]);
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P, data);
    #elif defined(PHY_AND28_F0)
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS, data);
    #endif
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_N, data);
    #endif
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][3]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][4]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][5]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][6]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][7]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM, data);
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDC, data);
    #endif
    data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYC, data);

    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][3]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][4]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][5]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][6]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][7]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][3]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][4]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][5]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][6]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][7]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMP, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMN, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[0][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[0][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0, data);
    data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_RD_EN_DLY_CYC, data);
    data = SET_RD_CONTROL((uint32) (*config_param).rd_control[0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_CONTROL, data);

    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[1]);
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_P, data);
    #elif defined(PHY_AND28_F0)
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS, data);
    #endif
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_N, data);
    #endif
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][3]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][4]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][5]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][6]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][7]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM, data);
    #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDC, data);
    #endif
    data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYC, data);

    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][3]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][4]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][5]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][6]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][7]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7P, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][2]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][3]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][4]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][5]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][6]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][7]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7N, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMP, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMN, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[1][0]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[1][1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0, data);
    data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_RD_EN_DLY_CYC, data);
    data = SET_RD_CONTROL((uint32) (*config_param).rd_control[1]);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_CONTROL, data);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[2]);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_P, data);
        #elif defined(PHY_AND28_F0)
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS, data);
        #endif
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[2]);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_N, data);
        #endif
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][4]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][5]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][6]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][7]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM, data);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[2]);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDC, data);
        #endif
        data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_WR_CHAN_DLY_CYC, data);

        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][4]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][5]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][6]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][7]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][4]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][5]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][6]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][7]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMP, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMN, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[2][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[2][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0, data);
        data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_RD_EN_DLY_CYC, data);
        data = SET_RD_CONTROL((uint32) (*config_param).rd_control[2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_CONTROL, data);

        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[3]);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_P, data);
        #elif defined(PHY_AND28_F0)
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS, data);
        #endif
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[3]);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_N, data);
        #endif
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][4]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][5]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][6]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][7]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM, data);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[3]);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDC, data);
        #endif
        data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_WR_CHAN_DLY_CYC, data);

        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][4]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][5]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][6]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][7]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7P, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][2]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][4]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][5]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][6]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][7]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7N, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMP, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMN, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[3][0]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[3][1]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0, data);
        data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_RD_EN_DLY_CYC, data);
        data = SET_RD_CONTROL((uint32) (*config_param).rd_control[3]);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_CONTROL, data);
    }
    #endif

    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    data = 0;
    DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_READ_FIFO_CLEAR, data);
    DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_READ_FIFO_CLEAR, data);

    #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info_ptr->interface_bitwidth == 32)
    {
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_READ_FIFO_CLEAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_READ_FIFO_CLEAR, data);
    }
    #endif

    sal_usleep(SHMOO_AND28_SHORT_SLEEP);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_and28_shmoo_ctl
 * Purpose:
 *      Perform shmoo (PHY calibration) on specific DRC index.
 * Parameters:
 *      unit                - unit number
 *      phy_ndx             - DRC index to perform shmoo on.
 *      shmoo_type          - Selects shmoo sub-section to be performs (-1 for full shmoo)
 *      stat                - RFU
 *      plot                - Plot shmoo results when not equal to 0
 *      action              - Save/restore functionality
 *      *config_param       - PHY configuration saved/restored
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 */

int
soc_and28_shmoo_ctl(int unit, int phy_ndx, int shmoo_type, int stat, int plot, int action, and28_shmoo_config_param_t *config_param)
{
    and28_shmoo_container_t *scPtr = NULL;
    uint32 dramType;
    uint32 ctlType;
    uint32 i;
    int ndx, ndxEnd;
    const uint32 *seqPtr;
    uint32 seqCount;

    dramType = shmoo_dram_info_ptr->dram_type;
    ctlType = shmoo_dram_info_ptr->ctl_type;

    if(!stat)
    {
        scPtr = &shmoo_container;
        if(scPtr == NULL)
        {
            return SOC_E_MEMORY;
        }
        sal_memset(scPtr, 0, sizeof(and28_shmoo_container_t));

        if(phy_ndx != SHMOO_AND28_INTERFACE_RSVP)
        {
            ndx = phy_ndx;
            ndxEnd = phy_ndx + 1;
        }
        else
        {
            ndx = 0;
            ndxEnd = SHMOO_AND28_MAX_INTERFACES;
        }

        for(; ndx < ndxEnd; ndx++)
        {
            if(!_shmoo_and28_check_dram(ndx)) {
                continue;
            }

            if(action == SHMOO_AND28_ACTION_RESTORE)
            {
                switch(ctlType)
                {
                    case SHMOO_AND28_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_AND28_CTL_TYPE_1:
                        _shmoo_and28_restore(unit, phy_ndx, config_param);
                        break;
                    default:
                        if(scPtr != NULL)
                        {
                        /*    sal_free(scPtr); */
                            scPtr = NULL;
                        }

                        printf("Unsupported controller type: %02u\n", ctlType);
                        return SOC_E_FAIL;
                }
            }
            else if((action == SHMOO_AND28_ACTION_RUN) || (action == SHMOO_AND28_ACTION_RUN_AND_SAVE))
            {
                switch(ctlType)
                {
                    case SHMOO_AND28_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_AND28_CTL_TYPE_1:
                        switch(dramType)
                        {
#if (SHMOO_AND28_DRAM_TYPE == SHMOO_AND28_DRAM_TYPE_DDR3)
                            case SHMOO_AND28_DRAM_TYPE_DDR3:
                                seqPtr = &shmoo_order_and28_ddr3[0];
                                seqCount = SHMOO_AND28_DDR3_SEQUENCE_COUNT;
                                break;
#endif
#if (SHMOO_AND28_DRAM_TYPE == SHMOO_AND28_DRAM_TYPE_DDR3L)
                            case SHMOO_AND28_DRAM_TYPE_DDR3L:
                                seqPtr = &shmoo_order_and28_ddr3l[0];
                                seqCount = SHMOO_AND28_DDR3L_SEQUENCE_COUNT;
                                break;
#endif
                            default:
                                printf("Unsupported dram type: %02u\n", dramType);
                                return SOC_E_FAIL;
                        }

                        (*scPtr).dramType = dramType;
                        (*scPtr).ctlType = ctlType;

                        if(shmoo_type != SHMOO_AND28_SHMOO_RSVP)
                        {
                            (*scPtr).shmooType = shmoo_type;
                            _shmoo_and28_entry(unit, ndx, scPtr, SHMOO_AND28_SINGLE);
                            _shmoo_and28_do(unit, ndx, scPtr);
                            _shmoo_and28_calib_2D(unit, ndx, scPtr);
                            _shmoo_and28_set_new_step(unit, ndx, scPtr);
#ifdef PLOT_SUPPORT
                            if(plot)
                            {
                                _shmoo_and28_plot(unit, ndx, scPtr);
                            }
#endif
                            _shmoo_and28_exit(unit, ndx, scPtr, SHMOO_AND28_SINGLE);
                        }
                        else
                        {
                            for(i = 0; i < seqCount; i++)
                            {
                                (*scPtr).shmooType = seqPtr[i];
                                _shmoo_and28_entry(unit, ndx, scPtr, SHMOO_AND28_SEQUENTIAL);
                                _shmoo_and28_do(unit, ndx, scPtr);
                                _shmoo_and28_calib_2D(unit, ndx, scPtr);
                                _shmoo_and28_set_new_step(unit, ndx, scPtr);
#ifdef PLOT_SUPPORT
                                if(plot)
                                {
                                    _shmoo_and28_plot(unit, ndx, scPtr);
                                }
#endif
                                _shmoo_and28_exit(unit, ndx, scPtr, SHMOO_AND28_SEQUENTIAL);
                            }
                        }

                        break;
                    default:
                        if(scPtr != NULL)
                        {
                        /*    sal_free(scPtr); */
                            scPtr = NULL;
                        }

                        printf("Unsupported controller type: %02u\n", ctlType);
                        return SOC_E_FAIL;
                }
            }

            if((action == SHMOO_AND28_ACTION_RUN_AND_SAVE) || (action == SHMOO_AND28_ACTION_SAVE))
            {
                _shmoo_and28_save(unit, phy_ndx, config_param);
            }
        }

        if(scPtr != NULL)
        {
        /*    sal_free(scPtr); */
            scPtr = NULL;
        }

        printf("DDR Tuning Complete\n");
    }
    else
    {
        /* Report only */
        switch(ctlType)
        {
            case SHMOO_AND28_CTL_TYPE_RSVP:
                break;
            case SHMOO_AND28_CTL_TYPE_1:
                break;
            default:
                printf("Unsupported controller type: %02u\n", ctlType);
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/* Set Dram Parameters/Info to Shmoo driver */
int
soc_and28_shmoo_dram_info_set(int unit, and28_shmoo_dram_info_t *sdi)
{

#if(!SHMOO_AND28_PHY_CONSTANT_CONFIG)
    shmoo_dram_info_ptr = &shmoo_dram_info;
    shmoo_dram_info_ptr->ctl_type = (*sdi).ctl_type;
    shmoo_dram_info_ptr->dram_type = (*sdi).dram_type;
    shmoo_dram_info_ptr->dram_bitmap = (*sdi).dram_bitmap;
    shmoo_dram_info_ptr->interface_bitwidth = (*sdi).interface_bitwidth;
    shmoo_dram_info_ptr->num_columns = (*sdi).num_columns;
    shmoo_dram_info_ptr->num_rows = (*sdi).num_rows;
    shmoo_dram_info_ptr->num_banks = (*sdi).num_banks;
    shmoo_dram_info_ptr->data_rate_mbps = (*sdi).data_rate_mbps;
    shmoo_dram_info_ptr->ref_clk_mhz = (*sdi).ref_clk_mhz;
    shmoo_dram_info_ptr->refi = (*sdi).refi;
    shmoo_dram_info_ptr->command_parity_latency = (*sdi).command_parity_latency;
    shmoo_dram_info_ptr->sim_system_mode = (*sdi).sim_system_mode;
#endif
    return SOC_E_NONE;
}

/* Configure PHY PLL and wait for lock */
int
_soc_and28_shmoo_phy_cfg_pll(int unit, int phy_ndx)
{
    int ndx, ndxEnd;
    uint32 data;
    uint32 timeout;
    uint32 pll_config;
    uint32 pll_control2;
    uint32 pll_dividers;
    uint32 pll_frac_divider;

    if(shmoo_dram_info_ptr->ref_clk_mhz != 50)
    {
        printf("     Unsupported reference flock frequency: %4d MHz\n", shmoo_dram_info_ptr->ref_clk_mhz);
        return SOC_E_FAIL;
    }

    switch(shmoo_dram_info_ptr->data_rate_mbps)
    {
        case 800:
            pll_config = 0x018D0012;
            pll_control2 = 0x94000000;
            pll_dividers = 0x004030C0;
            pll_frac_divider = 0x00000000;
            break;
        case 1066:
            pll_config = 0x018D0012;
            pll_control2 = 0x94000000;
            pll_dividers = 0x003030BF;
            pll_frac_divider = 0x000E147A;
            break;
        case 1333:
            pll_config = 0x018D0012;
            pll_control2 = 0x94000000;
            pll_dividers = 0x002030A0;
            pll_frac_divider = 0x000147AE;
            break;
        case 1600:
            pll_config = 0x018D0012;
            pll_control2 = 0x94000000;
            pll_dividers = 0x002030C0;
            pll_frac_divider = 0x00000000;
            break;
        default:
            printf("     Unsupported data rate: %4d Mbps\n", shmoo_dram_info_ptr->data_rate_mbps);
            return SOC_E_FAIL;
    }

    printf("     PHY PLL Configuration\n");
    printf("     Fref.............: %4d MHz\n", shmoo_dram_info_ptr->ref_clk_mhz);
    printf("     Data rate........: %4d Mbps\n", shmoo_dram_info_ptr->data_rate_mbps);

    if(phy_ndx != SHMOO_AND28_INTERFACE_RSVP)
    {
        ndx = phy_ndx;
        ndxEnd = phy_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_AND28_MAX_INTERFACES;
    }

    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_and28_check_dram(ndx))
        {
            continue;
        }

        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG, pll_config);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_CONTROL2, pll_control2);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_DIVIDERS, pll_dividers);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_FRAC_DIVIDER, pll_frac_divider);

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG, &data);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, PLL_CONFIG, RESET, 0);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG, data);

        timeout = 2000;
        do
        {
            DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_STATUS, &data);

            if(DDR_PHY_GET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, PLL_STATUS, LOCK))
            {
                printf("     PLL locked.\n");
                break;
            }

            if (timeout == 0)
            {
                printf("     PLL not locked!!! (Timeout)\n");
                return SOC_E_TIMEOUT;
            }

            timeout--;
            sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        }
        while(TRUE);

        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG, &data);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, PLL_CONFIG, RESET_POST_DIV, 0);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_PLL_CONFIG, data);
    }

    return SOC_E_NONE;
}

int
soc_and28_shmoo_phy_init(int unit, int phy_ndx)
{
    int ndx, ndxEnd;
    uint32 data;
    uint32 dfi_ctrl;
    uint32 dram_config;
    uint32 dram_timing1;
    uint32 dram_timing2;
    uint32 dram_timing3;
    uint32 dram_timing4;
    uint32 size1000UI, sizeUI;
    and28_step_size_t ss;

    if(phy_ndx != SHMOO_AND28_INTERFACE_RSVP)
    {
        ndx = phy_ndx;
        ndxEnd = phy_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_AND28_MAX_INTERFACES;
    }

    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_and28_check_dram(ndx))
        {
            continue;
        }

        printf("A Series - PHY Initialization (PHY index: %02d)\n", ndx);

/*A01*/ printf("A01. Turn off CKE\n");
        dfi_ctrl = 0;
        DDR_PHY_SET_FIELD(dfi_ctrl, DDR34_CORE_PHY_CONTROL_REGS, DFI_CNTRL, ASSERT_REQ, 1);
        DDR_PHY_SET_FIELD(dfi_ctrl, DDR34_CORE_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CS0 , 1);
        DDR_PHY_SET_FIELD(dfi_ctrl, DDR34_CORE_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CS1 , 1);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_F0))
            DDR_PHY_SET_FIELD(dfi_ctrl, DDR34_CORE_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CKE , 0);
        #elif defined(PHY_AND28_E2)
            DDR_PHY_SET_FIELD(dfi_ctrl, DDR34_CORE_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CKE0 , 0);
            DDR_PHY_SET_FIELD(dfi_ctrl, DDR34_CORE_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CKE1 , 0);
        #endif
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL, dfi_ctrl);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A02*/ printf("A02. Configure timing parameters\n");
        if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        {
            switch(shmoo_dram_info_ptr->data_rate_mbps)
            {
                case 800:
                    dram_timing1 = 0x0F040606;
                    dram_timing2 = 0x04060506;
                    dram_timing3 = 0x00044068;
                    dram_timing4 = 0x00000000;
                    break;
                case 1066:
                    dram_timing1 = 0x14040707;
                    dram_timing2 = 0x04080607;
                    dram_timing3 = 0x0004408C;
                    dram_timing4 = 0x00000000;
                    break;
                case 1333:
                    dram_timing1 = 0x18050909;
                    dram_timing2 = 0x050A0709;
                    dram_timing3 = 0x000450B0;
                    dram_timing4 = 0x00000000;
                    break;
                case 1600:
                    dram_timing1 = 0x1C060B0B;
                    dram_timing2 = 0x060C080B;
                    dram_timing3 = 0x000460D0;
                    dram_timing4 = 0x00000000;
                    break;
                default:
                    printf("Unsupported data rate: %4d Mbps\n", shmoo_dram_info_ptr->data_rate_mbps);
                    return SOC_E_FAIL;
            }
        }
        else
        {
            switch(shmoo_dram_info_ptr->data_rate_mbps)
            {
                case 800:
                    dram_timing1 = 0x0F040606;
                    dram_timing2 = 0x04060506;
                    dram_timing3 = 0x00044068;
                    dram_timing4 = 0x00000000;
                    break;
                case 1066:
                    dram_timing1 = 0x14040707;
                    dram_timing2 = 0x04080607;
                    dram_timing3 = 0x0004408C;
                    dram_timing4 = 0x00000000;
                    break;
                case 1333:
                    dram_timing1 = 0x18040909;
                    dram_timing2 = 0x050A0709;
                    dram_timing3 = 0x000450B0;
                    dram_timing4 = 0x00000000;
                    break;
                case 1600:
                    dram_timing1 = 0x1C050B0B;
                    dram_timing2 = 0x060C080B;
                    dram_timing3 = 0x000460D0;
                    dram_timing4 = 0x00000000;
                    break;
                default:
                    printf("Unsupported data rate: %4d Mbps\n", shmoo_dram_info_ptr->data_rate_mbps);
                    return SOC_E_FAIL;
            }
        }

        dram_config = 0x00001000;
        if (!SHMOO_AND28_PHY_BITWIDTH_IS_32)
            dram_config |= 0x02000000;
        switch(shmoo_dram_info_ptr->num_rows)
        {
            case 4096:  dram_config |= 0x00000000; break;
            case 8192:  dram_config |= 0x00000010; break;
            case 16384: dram_config |= 0x00000020; break;
            case 32768: dram_config |= 0x00000030; break;
            case 65536: dram_config |= 0x00000040; break;
            default:
                printf("Unsupported number of rows: %d\n", shmoo_dram_info_ptr->num_rows);
                return SOC_E_FAIL;
        }
        switch(shmoo_dram_info_ptr->num_columns)
        {
            case 512:   dram_config |= 0x00000000; break;
            case 1024:  dram_config |= 0x00000100; break;
            case 2048:  dram_config |= 0x00000200; break;
            default:
                printf("Unsupported number of columns: %d\n", shmoo_dram_info_ptr->num_columns);
                return SOC_E_FAIL;
        }
        switch(shmoo_dram_info_ptr->num_banks)
        {
            case 4:     dram_config |= 0x00000000; break;
            case 8:     dram_config |= 0x00000400; break;
            default:
                printf("Unsupported number of banks: %d\n", shmoo_dram_info_ptr->num_banks);
                return SOC_E_FAIL;
        }
        printf("DRAM config: 0x%X\n", dram_config);

        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DRAM_CONFIG, dram_config);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING1, dram_timing1);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING2, dram_timing2);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING3, dram_timing3);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DRAM_TIMING4, dram_timing4);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A03*/ printf("A03. Configure PHY PLL\n");
        _soc_and28_shmoo_phy_cfg_pll(unit, ndx);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A04*/ printf("A04. Configure reference voltage\n");
/*R04*/ DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, &data);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, 32);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, 32);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN0, 0);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN1, 0);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN2, 0);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN3, 0);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VREF_DAC_CONTROL, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A05*/ printf("A05. Compute VDL step size\n");
        _and28_calculate_step_size(unit, ndx, &ss);

        size1000UI = ss.size1000UI;
        sizeUI = size1000UI / 1000;
        printf("     VDL calibration complete.\n");
        printf("     VDL step size....: %3u.%03u ps\n", (ss.step1000 / 1000), (ss.step1000 % 1000));
        printf("     UI size..........: %3u.%03u steps\n", sizeUI, (size1000UI % 1000));

/*A06*/ printf("A06. Configure ADDR/CTRL VDLs\n");
        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
        if(sizeUI > SHMOO_AND28_MAX_VDL_LENGTH)
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, sizeUI);
        }
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD00, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD01, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD02, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD03, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD04, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD05, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD06, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD07, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD08, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD09, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD10, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD11, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD12, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD13, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD14, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AD15, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_BA2, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_AUX2, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS0, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CS1, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_PAR, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RAS_N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CAS_N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_CKE, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_RST_N, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_ODT, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VDL_CONTROL_WE_N, data);

/*A07*/ printf("A07. Disable Virtual VTT\n");
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL, &data);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CTL_IDLE, 0);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CS_IDLE, 0);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CKE_IDLE, 0);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROL, data);

/*A08*/ printf("A08. ZQ calibration\n");
/*R08*/ if(shmoo_dram_info_ptr->sim_system_mode)
        {
            printf("     Skipped for emulation\n");

            goto SHMOO_AND28_ZQ_CALIBRATION_END;
        }

        _and28_zq_calibration(unit, ndx);

        SHMOO_AND28_ZQ_CALIBRATION_END:

/*A09*/ printf("A09. Configure Static Pad Control\n");
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL, &data);
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_CLK1, 1);
        #endif
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, STATIC_PAD_CTL, AUTO_OEB, 1);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_STATIC_PAD_CTL, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A10*/ printf("A10. Configure ODT\n");
        data = 0;
        #if (defined(PHY_AND28_E0) || defined(PHY_AND28_E2))
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT_FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT_FORCE_VALUE, 1);
        #elif defined(PHY_AND28_F0)
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT0_FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT0_FORCE_VALUE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT1_FORCE, 1);
            DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT1_FORCE_VALUE, 1);
        #endif
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_WRITE_ODT_CNTRL, data);

        data = 0;
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_ENABLE, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_DELAY, 0);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_POST_LENGTH, 2);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_PRE_LENGTH, 4);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_ODT_CONTROL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_ODT_CONTROL, data);
        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_ODT_CONTROL, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_ODT_CONTROL, data);
        }
        #endif
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A11*/ printf("A11. Configure Write Pre-/Post-amble\n");
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE, &data);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQ_POSTAM_BITS, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQ_PREAM_BITS, 1);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS, 0xE);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS_POSTAM_BITS, 0);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS_PREAM_BITS, 2);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_WR_PREAMBLE_MODE, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_WR_PREAMBLE_MODE, data);
        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_WR_PREAMBLE_MODE, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_WR_PREAMBLE_MODE, data);
        }
        #endif
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A12*/ printf("A12. Configure Auto Idle\n");
        DDR_PHY_REG_READ(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL, &data);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, AUTO_DQ_RXENB_MODE, 3);
        DDR_PHY_SET_FIELD(data, DDR34_CORE_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, AUTO_DQ_IDDQ_MODE, 3);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL, data);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL, data);
        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info_ptr->interface_bitwidth == 32)
        {
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_2_IDLE_PAD_CONTROL, data);
            DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_BYTE_LANE_3_IDLE_PAD_CONTROL, data);
        }
        #endif
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

/*A13*/ printf("A13. Release PHY control\n");
        DDR_PHY_SET_FIELD(dfi_ctrl, DDR34_CORE_PHY_CONTROL_REGS, DFI_CNTRL, ASSERT_REQ, 0);
        DDR_PHY_REG_WRITE(unit, SHMOO_AND28_PHY_REG_BASE, 0, DDR34_CORE_PHY_CONTROL_REGS_DFI_CNTRL, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);

        printf("A Series - PHY Initialization complete (PHY index: %02d)\n", ndx);
    }

    return SOC_E_NONE;
}
