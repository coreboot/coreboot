#ifndef __BDK_CSRS_FUSF_H__
#define __BDK_CSRS_FUSF_H__
/* This file is auto-generated. Do not edit */

/***********************license start***************
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

 *   * Neither the name of Cavium Inc. nor the names of
 *     its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.

 * This Software, including technical data, may be subject to U.S. export  control
 * laws, including the U.S. Export Administration Act and its  associated
 * regulations, and may be subject to export or import  regulations in other
 * countries.

 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND CAVIUM  NETWORKS MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY REPRESENTATION OR
 * DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT DEFECTS, AND CAVIUM
 * SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES OF TITLE,
 * MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF
 * VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK ARISING OUT OF USE OR
 * PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 ***********************license end**************************************/


/**
 * @file
 *
 * Configuration and status register (CSR) address and type definitions for
 * Cavium FUSF.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration fusf_bar_e
 *
 * Field Fuse Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_FUSF_BAR_E_FUSF_PF_BAR0 (0x87e004000000ll)
#define BDK_FUSF_BAR_E_FUSF_PF_BAR0_SIZE 0x10000ull

/**
 * Enumeration fusf_fuse_num_e
 *
 * Field Fuse Fuse Number Enumeration
 * Enumerates the fuse numbers.
 */
#define BDK_FUSF_FUSE_NUM_E_CRYPT_NO_DIS (0xe)
#define BDK_FUSF_FUSE_NUM_E_CRYPT_SSK_DIS (0xf)
#define BDK_FUSF_FUSE_NUM_E_DIS_HUK (0xd)
#define BDK_FUSF_FUSE_NUM_E_EKX(a) (0x500 + (a))
#define BDK_FUSF_FUSE_NUM_E_FJ_CORE0 (0xc)
#define BDK_FUSF_FUSE_NUM_E_FJ_DIS (9)
#define BDK_FUSF_FUSE_NUM_E_FJ_TIMEOUTX(a) (0xa + (a))
#define BDK_FUSF_FUSE_NUM_E_FUSF_LCK (0)
#define BDK_FUSF_FUSE_NUM_E_HUKX(a) (0x480 + (a))
#define BDK_FUSF_FUSE_NUM_E_MFG_LCK (6)
#define BDK_FUSF_FUSE_NUM_E_ROM_SCRIPT_DISABLE (0x1e)
#define BDK_FUSF_FUSE_NUM_E_ROM_T_CNTX(a) (0x20 + (a))
#define BDK_FUSF_FUSE_NUM_E_ROTPKX(a) (0x300 + (a))
#define BDK_FUSF_FUSE_NUM_E_ROT_LCK (2)
#define BDK_FUSF_FUSE_NUM_E_RSVD128X(a) (0x80 + (a))
#define BDK_FUSF_FUSE_NUM_E_RSVD16X(a) (0x10 + (a))
#define BDK_FUSF_FUSE_NUM_E_RSVD256X(a) (0x100 + (a))
#define BDK_FUSF_FUSE_NUM_E_RSVD4X(a) (4 + (a))
#define BDK_FUSF_FUSE_NUM_E_RSVD5 (5)
#define BDK_FUSF_FUSE_NUM_E_RSVD512X(a) (0x200 + (a))
#define BDK_FUSF_FUSE_NUM_E_RSVD64X(a) (0x40 + (a))
#define BDK_FUSF_FUSE_NUM_E_RSVD7 (7)
#define BDK_FUSF_FUSE_NUM_E_SPI_SAFEMODE (0x1f)
#define BDK_FUSF_FUSE_NUM_E_SSKX(a) (0x400 + (a))
#define BDK_FUSF_FUSE_NUM_E_SSK_LCK (1)
#define BDK_FUSF_FUSE_NUM_E_SWX(a) (0x600 + (a))
#define BDK_FUSF_FUSE_NUM_E_SW_LCK (3)
#define BDK_FUSF_FUSE_NUM_E_TRUST_DIS (4)
#define BDK_FUSF_FUSE_NUM_E_TRUST_LVL_CHK (7)
#define BDK_FUSF_FUSE_NUM_E_TZ_FORCE2 (8)

/**
 * Register (RSL) fusf_bnk_dat#
 *
 * Field Fuse Bank Store Register
 * The initial state of FUSF_BNK_DAT() is as if bank15 were just read.
 * i.e. DAT* = fus[2047:1920].
 */
union bdk_fusf_bnk_datx
{
    uint64_t u;
    struct bdk_fusf_bnk_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SR/W/H) Efuse bank store. For read operations, the DAT gets the fus bank last read. For write
                                                                 operations, the DAT determines which fuses to blow. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SR/W/H) Efuse bank store. For read operations, the DAT gets the fus bank last read. For write
                                                                 operations, the DAT determines which fuses to blow. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_bnk_datx_s cn8; */
    struct bdk_fusf_bnk_datx_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SR/W/H) Efuse bank store. For read operations, [DAT] gets the fus bank last read. For write
                                                                 operations, [DAT] determines which fuses to blow. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SR/W/H) Efuse bank store. For read operations, [DAT] gets the fus bank last read. For write
                                                                 operations, [DAT] determines which fuses to blow. */
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_fusf_bnk_datx bdk_fusf_bnk_datx_t;

static inline uint64_t BDK_FUSF_BNK_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_BNK_DATX(unsigned long a)
{
    if (a<=1)
        return 0x87e004000120ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("FUSF_BNK_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUSF_BNK_DATX(a) bdk_fusf_bnk_datx_t
#define bustype_BDK_FUSF_BNK_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_BNK_DATX(a) "FUSF_BNK_DATX"
#define device_bar_BDK_FUSF_BNK_DATX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_BNK_DATX(a) (a)
#define arguments_BDK_FUSF_BNK_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) fusf_const
 *
 * Field Fuse Constants Register
 */
union bdk_fusf_const
{
    uint64_t u;
    struct bdk_fusf_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t fuse_banks            : 8;  /**< [  7:  0](SRO) Number of 128-bit field fuse banks present. */
#else /* Word 0 - Little Endian */
        uint64_t fuse_banks            : 8;  /**< [  7:  0](SRO) Number of 128-bit field fuse banks present. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_const_s cn; */
};
typedef union bdk_fusf_const bdk_fusf_const_t;

#define BDK_FUSF_CONST BDK_FUSF_CONST_FUNC()
static inline uint64_t BDK_FUSF_CONST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_CONST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e004000130ll;
    __bdk_csr_fatal("FUSF_CONST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUSF_CONST bdk_fusf_const_t
#define bustype_BDK_FUSF_CONST BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_CONST "FUSF_CONST"
#define device_bar_BDK_FUSF_CONST 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_CONST 0
#define arguments_BDK_FUSF_CONST -1,-1,-1,-1

/**
 * Register (RSL) fusf_ctl
 *
 * Field Fuse Control Register
 */
union bdk_fusf_ctl
{
    uint64_t u;
    struct bdk_fusf_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rom_t_cnt             : 32; /**< [ 63: 32](SRO) ROM trusted counter. Reads field fuses FUSF_FUSE_NUM_E::ROM_T_CNT(). */
        uint64_t spi_safemode          : 1;  /**< [ 31: 31](SRO) Reserved.
                                                                 Internal:
                                                                 SPI safemode. Reads field fuses FUSF_FUSE_NUM_E::SPI_SAFEMODE. */
        uint64_t rom_script_disable    : 1;  /**< [ 30: 30](SRO) ROM script disable. Reads field fuses FUSF_FUSE_NUM_E::ROM_SCRIPT_DISABLE. */
        uint64_t fuse16                : 14; /**< [ 29: 16](SRO) Reserved. */
        uint64_t crypt_ssk_dis         : 1;  /**< [ 15: 15](SRO) SSK crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_SSK_DIS. */
        uint64_t crypt_no_dis          : 1;  /**< [ 14: 14](SRO) No-crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_NO_DIS. */
        uint64_t fj_dis_huk            : 1;  /**< [ 13: 13](SRO) Flash-jump HUK secret hiding. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS_HUK. */
        uint64_t fj_core0              : 1;  /**< [ 12: 12](SRO) Flash-jump core 0 only. Reads field fuse FUSF_FUSE_NUM_E::FJ_CORE0. */
        uint64_t fj_timeout            : 2;  /**< [ 11: 10](SRO) Flash-jump timeout. Reads field fuse FUSF_FUSE_NUM_E::FJ_TIMEOUT(). */
        uint64_t fj_dis                : 1;  /**< [  9:  9](SRO) Flash-jump disable. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS. */
        uint64_t tz_force2             : 1;  /**< [  8:  8](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t trust_lvl_chk         : 1;  /**< [  7:  7](SRO) Trust level check. Reads field fuse FUSF_FUSE_NUM_E::TRUST_LVL_CHK. */
        uint64_t mfg_lck               : 1;  /**< [  6:  6](SRO) Manufacturing lock. Reads field fuse FUSF_FUSE_NUM_E::MFG_LCK. */
        uint64_t fuse5                 : 1;  /**< [  5:  5](SRO) Unallocated fuse. */
        uint64_t trust_dis             : 1;  /**< [  4:  4](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t sw_lck                : 1;  /**< [  3:  3](SRO) Software fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SW_LCK. */
        uint64_t rot_lck               : 1;  /**< [  2:  2](SRO) Root-of-trust fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::ROT_LCK. */
        uint64_t ssk_lck               : 1;  /**< [  1:  1](SRO) Secret symmetric key fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SSK_LCK. */
        uint64_t fusf_lck              : 1;  /**< [  0:  0](SRO) Total field fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::FUSF_LCK. */
#else /* Word 0 - Little Endian */
        uint64_t fusf_lck              : 1;  /**< [  0:  0](SRO) Total field fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::FUSF_LCK. */
        uint64_t ssk_lck               : 1;  /**< [  1:  1](SRO) Secret symmetric key fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SSK_LCK. */
        uint64_t rot_lck               : 1;  /**< [  2:  2](SRO) Root-of-trust fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::ROT_LCK. */
        uint64_t sw_lck                : 1;  /**< [  3:  3](SRO) Software fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SW_LCK. */
        uint64_t trust_dis             : 1;  /**< [  4:  4](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t fuse5                 : 1;  /**< [  5:  5](SRO) Unallocated fuse. */
        uint64_t mfg_lck               : 1;  /**< [  6:  6](SRO) Manufacturing lock. Reads field fuse FUSF_FUSE_NUM_E::MFG_LCK. */
        uint64_t trust_lvl_chk         : 1;  /**< [  7:  7](SRO) Trust level check. Reads field fuse FUSF_FUSE_NUM_E::TRUST_LVL_CHK. */
        uint64_t tz_force2             : 1;  /**< [  8:  8](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t fj_dis                : 1;  /**< [  9:  9](SRO) Flash-jump disable. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS. */
        uint64_t fj_timeout            : 2;  /**< [ 11: 10](SRO) Flash-jump timeout. Reads field fuse FUSF_FUSE_NUM_E::FJ_TIMEOUT(). */
        uint64_t fj_core0              : 1;  /**< [ 12: 12](SRO) Flash-jump core 0 only. Reads field fuse FUSF_FUSE_NUM_E::FJ_CORE0. */
        uint64_t fj_dis_huk            : 1;  /**< [ 13: 13](SRO) Flash-jump HUK secret hiding. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS_HUK. */
        uint64_t crypt_no_dis          : 1;  /**< [ 14: 14](SRO) No-crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_NO_DIS. */
        uint64_t crypt_ssk_dis         : 1;  /**< [ 15: 15](SRO) SSK crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_SSK_DIS. */
        uint64_t fuse16                : 14; /**< [ 29: 16](SRO) Reserved. */
        uint64_t rom_script_disable    : 1;  /**< [ 30: 30](SRO) ROM script disable. Reads field fuses FUSF_FUSE_NUM_E::ROM_SCRIPT_DISABLE. */
        uint64_t spi_safemode          : 1;  /**< [ 31: 31](SRO) Reserved.
                                                                 Internal:
                                                                 SPI safemode. Reads field fuses FUSF_FUSE_NUM_E::SPI_SAFEMODE. */
        uint64_t rom_t_cnt             : 32; /**< [ 63: 32](SRO) ROM trusted counter. Reads field fuses FUSF_FUSE_NUM_E::ROM_T_CNT(). */
#endif /* Word 0 - End */
    } s;
    struct bdk_fusf_ctl_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rom_t_cnt             : 32; /**< [ 63: 32](SRO) ROM trusted counter. Reads field fuses FUSF_FUSE_NUM_E::ROM_T_CNT(). */
        uint64_t reserved_16_31        : 16;
        uint64_t crypt_ssk_dis         : 1;  /**< [ 15: 15](SRO) SSK crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_SSK_DIS. */
        uint64_t crypt_no_dis          : 1;  /**< [ 14: 14](SRO) No-crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_NO_DIS. */
        uint64_t fj_dis_huk            : 1;  /**< [ 13: 13](SRO) Flash-jump HUK secret hiding. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS_HUK. */
        uint64_t fj_core0              : 1;  /**< [ 12: 12](SRO) Flash-jump core 0 only. Reads field fuse FUSF_FUSE_NUM_E::FJ_CORE0. */
        uint64_t fj_timeout            : 2;  /**< [ 11: 10](SRO) Flash-jump timeout. Reads field fuse FUSF_FUSE_NUM_E::FJ_TIMEOUT(). */
        uint64_t fj_dis                : 1;  /**< [  9:  9](SRO) Flash-jump disable. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS. */
        uint64_t tz_force2             : 1;  /**< [  8:  8](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t reserved_4_7          : 4;
        uint64_t sw_lck                : 1;  /**< [  3:  3](SRO) Software fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SW_LCK. */
        uint64_t rot_lck               : 1;  /**< [  2:  2](SRO) Root-of-trust fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::ROT_LCK. */
        uint64_t ssk_lck               : 1;  /**< [  1:  1](SRO) Secret symmetric key fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SSK_LCK. */
        uint64_t fusf_lck              : 1;  /**< [  0:  0](SRO) Total field fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::FUSF_LCK. */
#else /* Word 0 - Little Endian */
        uint64_t fusf_lck              : 1;  /**< [  0:  0](SRO) Total field fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::FUSF_LCK. */
        uint64_t ssk_lck               : 1;  /**< [  1:  1](SRO) Secret symmetric key fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SSK_LCK. */
        uint64_t rot_lck               : 1;  /**< [  2:  2](SRO) Root-of-trust fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::ROT_LCK. */
        uint64_t sw_lck                : 1;  /**< [  3:  3](SRO) Software fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SW_LCK. */
        uint64_t reserved_4_7          : 4;
        uint64_t tz_force2             : 1;  /**< [  8:  8](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t fj_dis                : 1;  /**< [  9:  9](SRO) Flash-jump disable. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS. */
        uint64_t fj_timeout            : 2;  /**< [ 11: 10](SRO) Flash-jump timeout. Reads field fuse FUSF_FUSE_NUM_E::FJ_TIMEOUT(). */
        uint64_t fj_core0              : 1;  /**< [ 12: 12](SRO) Flash-jump core 0 only. Reads field fuse FUSF_FUSE_NUM_E::FJ_CORE0. */
        uint64_t fj_dis_huk            : 1;  /**< [ 13: 13](SRO) Flash-jump HUK secret hiding. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS_HUK. */
        uint64_t crypt_no_dis          : 1;  /**< [ 14: 14](SRO) No-crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_NO_DIS. */
        uint64_t crypt_ssk_dis         : 1;  /**< [ 15: 15](SRO) SSK crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_SSK_DIS. */
        uint64_t reserved_16_31        : 16;
        uint64_t rom_t_cnt             : 32; /**< [ 63: 32](SRO) ROM trusted counter. Reads field fuses FUSF_FUSE_NUM_E::ROM_T_CNT(). */
#endif /* Word 0 - End */
    } cn8;
    struct bdk_fusf_ctl_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rom_t_cnt             : 32; /**< [ 63: 32](SRO) ROM trusted counter. Reads field fuses FUSF_FUSE_NUM_E::ROM_T_CNT(). */
        uint64_t spi_safemode          : 1;  /**< [ 31: 31](SRO) Reserved.
                                                                 Internal:
                                                                 SPI safemode. Reads field fuses FUSF_FUSE_NUM_E::SPI_SAFEMODE. */
        uint64_t rom_script_disable    : 1;  /**< [ 30: 30](SRO) ROM script disable. Reads field fuses FUSF_FUSE_NUM_E::ROM_SCRIPT_DISABLE. */
        uint64_t fuse16                : 14; /**< [ 29: 16](SRO) Reserved. */
        uint64_t crypt_ssk_dis         : 1;  /**< [ 15: 15](SRO) SSK crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_SSK_DIS. */
        uint64_t crypt_no_dis          : 1;  /**< [ 14: 14](SRO) No-crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_NO_DIS. */
        uint64_t fj_dis_huk            : 1;  /**< [ 13: 13](SRO) Flash-jump HUK secret hiding. Reads field fuse FUSF_FUSE_NUM_E::DIS_HUK. */
        uint64_t fj_core0              : 1;  /**< [ 12: 12](SRO) Flash-jump core 0 only. Reads field fuse FUSF_FUSE_NUM_E::FJ_CORE0. */
        uint64_t fj_timeout            : 2;  /**< [ 11: 10](SRO) Flash-jump timeout. Reads field fuse FUSF_FUSE_NUM_E::FJ_TIMEOUT(). */
        uint64_t fj_dis                : 1;  /**< [  9:  9](SRO) Flash-jump disable. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS. */
        uint64_t tz_force2             : 1;  /**< [  8:  8](SRO) Trusted mode force override. Reads field fuse FUSF_FUSE_NUM_E::TZ_FORCE2. */
        uint64_t trust_lvl_chk         : 1;  /**< [  7:  7](SRO) Trust level check. Reads field fuse FUSF_FUSE_NUM_E::TRUST_LVL_CHK. */
        uint64_t mfg_lck               : 1;  /**< [  6:  6](SRO) Manufacturing lock. Reads field fuse FUSF_FUSE_NUM_E::MFG_LCK. */
        uint64_t fuse5                 : 1;  /**< [  5:  5](SRO) Unallocated fuse. */
        uint64_t trust_dis             : 1;  /**< [  4:  4](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t sw_lck                : 1;  /**< [  3:  3](SRO) Software fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SW_LCK. */
        uint64_t rot_lck               : 1;  /**< [  2:  2](SRO) Root-of-trust fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::ROT_LCK. */
        uint64_t ssk_lck               : 1;  /**< [  1:  1](SRO) Secret symmetric key fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SSK_LCK. */
        uint64_t fusf_lck              : 1;  /**< [  0:  0](SRO) Total field fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::FUSF_LCK. */
#else /* Word 0 - Little Endian */
        uint64_t fusf_lck              : 1;  /**< [  0:  0](SRO) Total field fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::FUSF_LCK. */
        uint64_t ssk_lck               : 1;  /**< [  1:  1](SRO) Secret symmetric key fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SSK_LCK. */
        uint64_t rot_lck               : 1;  /**< [  2:  2](SRO) Root-of-trust fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::ROT_LCK. */
        uint64_t sw_lck                : 1;  /**< [  3:  3](SRO) Software fuse lockdown. Reads field fuse FUSF_FUSE_NUM_E::SW_LCK. */
        uint64_t trust_dis             : 1;  /**< [  4:  4](SRO) Disable Trustzone. Reads field fuse FUSF_FUSE_NUM_E::TRUST_DIS. */
        uint64_t fuse5                 : 1;  /**< [  5:  5](SRO) Unallocated fuse. */
        uint64_t mfg_lck               : 1;  /**< [  6:  6](SRO) Manufacturing lock. Reads field fuse FUSF_FUSE_NUM_E::MFG_LCK. */
        uint64_t trust_lvl_chk         : 1;  /**< [  7:  7](SRO) Trust level check. Reads field fuse FUSF_FUSE_NUM_E::TRUST_LVL_CHK. */
        uint64_t tz_force2             : 1;  /**< [  8:  8](SRO) Trusted mode force override. Reads field fuse FUSF_FUSE_NUM_E::TZ_FORCE2. */
        uint64_t fj_dis                : 1;  /**< [  9:  9](SRO) Flash-jump disable. Reads field fuse FUSF_FUSE_NUM_E::FJ_DIS. */
        uint64_t fj_timeout            : 2;  /**< [ 11: 10](SRO) Flash-jump timeout. Reads field fuse FUSF_FUSE_NUM_E::FJ_TIMEOUT(). */
        uint64_t fj_core0              : 1;  /**< [ 12: 12](SRO) Flash-jump core 0 only. Reads field fuse FUSF_FUSE_NUM_E::FJ_CORE0. */
        uint64_t fj_dis_huk            : 1;  /**< [ 13: 13](SRO) Flash-jump HUK secret hiding. Reads field fuse FUSF_FUSE_NUM_E::DIS_HUK. */
        uint64_t crypt_no_dis          : 1;  /**< [ 14: 14](SRO) No-crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_NO_DIS. */
        uint64_t crypt_ssk_dis         : 1;  /**< [ 15: 15](SRO) SSK crypt disable. Reads field fuse FUSF_FUSE_NUM_E::CRYPT_SSK_DIS. */
        uint64_t fuse16                : 14; /**< [ 29: 16](SRO) Reserved. */
        uint64_t rom_script_disable    : 1;  /**< [ 30: 30](SRO) ROM script disable. Reads field fuses FUSF_FUSE_NUM_E::ROM_SCRIPT_DISABLE. */
        uint64_t spi_safemode          : 1;  /**< [ 31: 31](SRO) Reserved.
                                                                 Internal:
                                                                 SPI safemode. Reads field fuses FUSF_FUSE_NUM_E::SPI_SAFEMODE. */
        uint64_t rom_t_cnt             : 32; /**< [ 63: 32](SRO) ROM trusted counter. Reads field fuses FUSF_FUSE_NUM_E::ROM_T_CNT(). */
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_fusf_ctl bdk_fusf_ctl_t;

#define BDK_FUSF_CTL BDK_FUSF_CTL_FUNC()
static inline uint64_t BDK_FUSF_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_CTL_FUNC(void)
{
    return 0x87e004000000ll;
}

#define typedef_BDK_FUSF_CTL bdk_fusf_ctl_t
#define bustype_BDK_FUSF_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_CTL "FUSF_CTL"
#define device_bar_BDK_FUSF_CTL 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_CTL 0
#define arguments_BDK_FUSF_CTL -1,-1,-1,-1

/**
 * Register (RSL) fusf_eco
 *
 * INTERNAL: FUSF ECO Register
 */
union bdk_fusf_eco
{
    uint64_t u;
    struct bdk_fusf_eco_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t eco_rw                : 32; /**< [ 31:  0](R/W) ECO flops.
                                                                 This field is always reinitialized on a chip domain reset. */
#else /* Word 0 - Little Endian */
        uint64_t eco_rw                : 32; /**< [ 31:  0](R/W) ECO flops.
                                                                 This field is always reinitialized on a chip domain reset. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_eco_s cn; */
};
typedef union bdk_fusf_eco bdk_fusf_eco_t;

#define BDK_FUSF_ECO BDK_FUSF_ECO_FUNC()
static inline uint64_t BDK_FUSF_ECO_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_ECO_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e004000118ll;
    __bdk_csr_fatal("FUSF_ECO", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUSF_ECO bdk_fusf_eco_t
#define bustype_BDK_FUSF_ECO BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_ECO "FUSF_ECO"
#define device_bar_BDK_FUSF_ECO 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_ECO 0
#define arguments_BDK_FUSF_ECO -1,-1,-1,-1

/**
 * Register (RSL) fusf_ek#
 *
 * Field Fuse ECC Private Endorsement Key Registers
 */
union bdk_fusf_ekx
{
    uint64_t u;
    struct bdk_fusf_ekx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) ECC private endorsement key. Reads field fuses FUSF_FUSE_NUM_E::EK(). */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) ECC private endorsement key. Reads field fuses FUSF_FUSE_NUM_E::EK(). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_ekx_s cn; */
};
typedef union bdk_fusf_ekx bdk_fusf_ekx_t;

static inline uint64_t BDK_FUSF_EKX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_EKX(unsigned long a)
{
    if (a<=3)
        return 0x87e0040000a0ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("FUSF_EKX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUSF_EKX(a) bdk_fusf_ekx_t
#define bustype_BDK_FUSF_EKX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_EKX(a) "FUSF_EKX"
#define device_bar_BDK_FUSF_EKX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_EKX(a) (a)
#define arguments_BDK_FUSF_EKX(a) (a),-1,-1,-1

/**
 * Register (RSL) fusf_huk#
 *
 * Field Fuse Hardware Unique Key Registers
 */
union bdk_fusf_hukx
{
    uint64_t u;
    struct bdk_fusf_hukx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Hardware unique key (HUK). Reads field fuses FUSF_FUSE_NUM_E::HUK(). */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Hardware unique key (HUK). Reads field fuses FUSF_FUSE_NUM_E::HUK(). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_hukx_s cn; */
};
typedef union bdk_fusf_hukx bdk_fusf_hukx_t;

static inline uint64_t BDK_FUSF_HUKX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_HUKX(unsigned long a)
{
    if (a<=1)
        return 0x87e004000090ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("FUSF_HUKX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUSF_HUKX(a) bdk_fusf_hukx_t
#define bustype_BDK_FUSF_HUKX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_HUKX(a) "FUSF_HUKX"
#define device_bar_BDK_FUSF_HUKX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_HUKX(a) (a)
#define arguments_BDK_FUSF_HUKX(a) (a),-1,-1,-1

/**
 * Register (RSL) fusf_prog
 *
 * Field Fuse Programming Register
 */
union bdk_fusf_prog
{
    uint64_t u;
    struct bdk_fusf_prog_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t efuse                 : 1;  /**< [ 15: 15](SR/W) Efuse storage. When set, the data is written directly to the efuse
                                                                 bank.  When cleared, data is soft blown to local storage.
                                                                 A soft blown fuse is subject to lockdown fuses.
                                                                 Soft blown fuses will become active after a chip domain reset
                                                                 but will not persist through a cold domain reset. */
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be set when
                                                                 programming fuses (ie. [EFUSE] and [PROG] set). */
        uint64_t reserved_8_13         : 6;
        uint64_t addr                  : 4;  /**< [  7:  4](SR/W) Indicates which of bank of 128 fuses to blow. Software
                                                                 should not change this field while the FUSF_PROG[PROG] bit is set. */
        uint64_t reserved_3            : 1;
        uint64_t prog_pin              : 1;  /**< [  2:  2](SRO) Efuse program voltage (EFUS_PROG) is applied.
                                                                 Internal:
                                                                 Indicates state of pi_efuse_pgm_ext not pi_efuse_pgm_int. */
        uint64_t sft                   : 1;  /**< [  1:  1](SR/W/H) When set with [PROG], causes only the local storage to change and will not blow
                                                                 any fuses. Hardware will clear when the program operation is complete. */
        uint64_t reserved_0            : 1;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0            : 1;
        uint64_t sft                   : 1;  /**< [  1:  1](SR/W/H) When set with [PROG], causes only the local storage to change and will not blow
                                                                 any fuses. Hardware will clear when the program operation is complete. */
        uint64_t prog_pin              : 1;  /**< [  2:  2](SRO) Efuse program voltage (EFUS_PROG) is applied.
                                                                 Internal:
                                                                 Indicates state of pi_efuse_pgm_ext not pi_efuse_pgm_int. */
        uint64_t reserved_3            : 1;
        uint64_t addr                  : 4;  /**< [  7:  4](SR/W) Indicates which of bank of 128 fuses to blow. Software
                                                                 should not change this field while the FUSF_PROG[PROG] bit is set. */
        uint64_t reserved_8_13         : 6;
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be set when
                                                                 programming fuses (ie. [EFUSE] and [PROG] set). */
        uint64_t efuse                 : 1;  /**< [ 15: 15](SR/W) Efuse storage. When set, the data is written directly to the efuse
                                                                 bank.  When cleared, data is soft blown to local storage.
                                                                 A soft blown fuse is subject to lockdown fuses.
                                                                 Soft blown fuses will become active after a chip domain reset
                                                                 but will not persist through a cold domain reset. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    struct bdk_fusf_prog_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t volt_en               : 1;  /**< [  3:  3](SWO) Enable programming voltage.  Asserts EFUSE_ENABLE_L opep-drain output pin. */
        uint64_t prog_pin              : 1;  /**< [  2:  2](SRO) Efuse program voltage (EFUS_PROG) is applied.
                                                                 Internal:
                                                                 Indicates state of pi_efuse_pgm_ext not pi_efuse_pgm_int. */
        uint64_t sft                   : 1;  /**< [  1:  1](SR/W/H) When set with [PROG], causes only the local storage to change and will not blow
                                                                 any fuses. Hardware will clear when the program operation is complete. */
        uint64_t prog                  : 1;  /**< [  0:  0](SR/W/H) When written to 1 by software, blow the fuse bank. Hardware clears this bit when
                                                                 the program operation is complete.

                                                                 To write a bank of fuses, software must set FUSF_WADR[ADDR] to the bank to be
                                                                 programmed and then set each bit within FUSF_BNK_DATX to indicate which fuses to blow.

                                                                 Once FUSF_WADR[ADDR], and DAT are setup, software can write to FUSF_PROG[PROG]
                                                                 to start the bank write and poll on [PROG]. Once PROG is clear, the bank write
                                                                 is complete.  MIO_FUS_READ_TIMES[WRSTB_WH] set the time for the hardware to
                                                                 clear this bit. A soft blow is still subject to lockdown fuses. After a
                                                                 soft/warm reset, the chip behaves as though the fuses were actually blown. A
                                                                 cold reset restores the actual fuse values. */
#else /* Word 0 - Little Endian */
        uint64_t prog                  : 1;  /**< [  0:  0](SR/W/H) When written to 1 by software, blow the fuse bank. Hardware clears this bit when
                                                                 the program operation is complete.

                                                                 To write a bank of fuses, software must set FUSF_WADR[ADDR] to the bank to be
                                                                 programmed and then set each bit within FUSF_BNK_DATX to indicate which fuses to blow.

                                                                 Once FUSF_WADR[ADDR], and DAT are setup, software can write to FUSF_PROG[PROG]
                                                                 to start the bank write and poll on [PROG]. Once PROG is clear, the bank write
                                                                 is complete.  MIO_FUS_READ_TIMES[WRSTB_WH] set the time for the hardware to
                                                                 clear this bit. A soft blow is still subject to lockdown fuses. After a
                                                                 soft/warm reset, the chip behaves as though the fuses were actually blown. A
                                                                 cold reset restores the actual fuse values. */
        uint64_t sft                   : 1;  /**< [  1:  1](SR/W/H) When set with [PROG], causes only the local storage to change and will not blow
                                                                 any fuses. Hardware will clear when the program operation is complete. */
        uint64_t prog_pin              : 1;  /**< [  2:  2](SRO) Efuse program voltage (EFUS_PROG) is applied.
                                                                 Internal:
                                                                 Indicates state of pi_efuse_pgm_ext not pi_efuse_pgm_int. */
        uint64_t volt_en               : 1;  /**< [  3:  3](SWO) Enable programming voltage.  Asserts EFUSE_ENABLE_L opep-drain output pin. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_fusf_prog_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t efuse                 : 1;  /**< [ 15: 15](SR/W) Efuse storage. When set, the data is written directly to the efuse
                                                                 bank.  When cleared, data is soft blown to local storage.
                                                                 A soft blown fuse is subject to lockdown fuses.
                                                                 Soft blown fuses will become active after a chip domain reset
                                                                 but will not persist through a cold domain reset. */
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be set when
                                                                 programming fuses (ie. [EFUSE] and [PROG] set). */
        uint64_t volt_en               : 1;  /**< [ 13: 13](SR/W) Enable programming voltage.  Asserts EFUSE_ENABLE_L open-drain output pin. */
        uint64_t prog                  : 1;  /**< [ 12: 12](SR/W/H) Internal:
                                                                 When written to one by software, blow the fuse bank. Hardware will
                                                                 clear the field when the program operation is complete.
                                                                 To write a bank of fuses, software must write the fuse data into
                                                                 FUSF_BNK_DAT().  Then it writes the ADDR and EFUSE fields of this register
                                                                 and sets the PROG bit.  Hardware will clear the [PROG] when the write is
                                                                 completed.  New fuses will become active after a chip domain reset. */
        uint64_t reserved_8_11         : 4;
        uint64_t addr                  : 4;  /**< [  7:  4](SR/W) Indicates which of bank of 128 fuses to blow. Software
                                                                 should not change this field while the FUSF_PROG[PROG] bit is set. */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t addr                  : 4;  /**< [  7:  4](SR/W) Indicates which of bank of 128 fuses to blow. Software
                                                                 should not change this field while the FUSF_PROG[PROG] bit is set. */
        uint64_t reserved_8_11         : 4;
        uint64_t prog                  : 1;  /**< [ 12: 12](SR/W/H) Internal:
                                                                 When written to one by software, blow the fuse bank. Hardware will
                                                                 clear the field when the program operation is complete.
                                                                 To write a bank of fuses, software must write the fuse data into
                                                                 FUSF_BNK_DAT().  Then it writes the ADDR and EFUSE fields of this register
                                                                 and sets the PROG bit.  Hardware will clear the [PROG] when the write is
                                                                 completed.  New fuses will become active after a chip domain reset. */
        uint64_t volt_en               : 1;  /**< [ 13: 13](SR/W) Enable programming voltage.  Asserts EFUSE_ENABLE_L open-drain output pin. */
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be set when
                                                                 programming fuses (ie. [EFUSE] and [PROG] set). */
        uint64_t efuse                 : 1;  /**< [ 15: 15](SR/W) Efuse storage. When set, the data is written directly to the efuse
                                                                 bank.  When cleared, data is soft blown to local storage.
                                                                 A soft blown fuse is subject to lockdown fuses.
                                                                 Soft blown fuses will become active after a chip domain reset
                                                                 but will not persist through a cold domain reset. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_fusf_prog_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t volt_en               : 1;  /**< [  3:  3](SR/W) Enable programming voltage.  Asserts EFUSE_ENABLE_L open-drain output pin. */
        uint64_t prog_pin              : 1;  /**< [  2:  2](SRO) Efuse program voltage (EFUS_PROG) is applied.
                                                                 Internal:
                                                                 Indicates state of pi_efuse_pgm_ext not pi_efuse_pgm_int. */
        uint64_t sft                   : 1;  /**< [  1:  1](SR/W/H) When set with [PROG], causes only the local storage to change and will not blow
                                                                 any fuses. Hardware will clear when the program operation is complete. */
        uint64_t prog                  : 1;  /**< [  0:  0](SR/W/H) When written to 1 by software, blow the fuse bank. Hardware clears this bit when
                                                                 the program operation is complete.

                                                                 To write a bank of fuses, software must set FUSF_WADR[ADDR] to the bank to be
                                                                 programmed and then set each bit within FUSF_BNK_DATX to indicate which fuses to blow.

                                                                 Once FUSF_WADR[ADDR], and DAT are setup, software can write to FUSF_PROG[PROG]
                                                                 to start the bank write and poll on [PROG]. Once PROG is clear, the bank write
                                                                 is complete.  MIO_FUS_READ_TIMES[WRSTB_WH] set the time for the hardware to
                                                                 clear this bit. A soft blow is still subject to lockdown fuses. After a
                                                                 soft/warm reset, the chip behaves as though the fuses were actually blown. A
                                                                 cold reset restores the actual fuse values. */
#else /* Word 0 - Little Endian */
        uint64_t prog                  : 1;  /**< [  0:  0](SR/W/H) When written to 1 by software, blow the fuse bank. Hardware clears this bit when
                                                                 the program operation is complete.

                                                                 To write a bank of fuses, software must set FUSF_WADR[ADDR] to the bank to be
                                                                 programmed and then set each bit within FUSF_BNK_DATX to indicate which fuses to blow.

                                                                 Once FUSF_WADR[ADDR], and DAT are setup, software can write to FUSF_PROG[PROG]
                                                                 to start the bank write and poll on [PROG]. Once PROG is clear, the bank write
                                                                 is complete.  MIO_FUS_READ_TIMES[WRSTB_WH] set the time for the hardware to
                                                                 clear this bit. A soft blow is still subject to lockdown fuses. After a
                                                                 soft/warm reset, the chip behaves as though the fuses were actually blown. A
                                                                 cold reset restores the actual fuse values. */
        uint64_t sft                   : 1;  /**< [  1:  1](SR/W/H) When set with [PROG], causes only the local storage to change and will not blow
                                                                 any fuses. Hardware will clear when the program operation is complete. */
        uint64_t prog_pin              : 1;  /**< [  2:  2](SRO) Efuse program voltage (EFUS_PROG) is applied.
                                                                 Internal:
                                                                 Indicates state of pi_efuse_pgm_ext not pi_efuse_pgm_int. */
        uint64_t volt_en               : 1;  /**< [  3:  3](SR/W) Enable programming voltage.  Asserts EFUSE_ENABLE_L open-drain output pin. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_fusf_prog_cn81xx cn83xx; */
    /* struct bdk_fusf_prog_cn81xx cn88xxp2; */
};
typedef union bdk_fusf_prog bdk_fusf_prog_t;

#define BDK_FUSF_PROG BDK_FUSF_PROG_FUNC()
static inline uint64_t BDK_FUSF_PROG_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_PROG_FUNC(void)
{
    return 0x87e004000110ll;
}

#define typedef_BDK_FUSF_PROG bdk_fusf_prog_t
#define bustype_BDK_FUSF_PROG BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_PROG "FUSF_PROG"
#define device_bar_BDK_FUSF_PROG 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_PROG 0
#define arguments_BDK_FUSF_PROG -1,-1,-1,-1

/**
 * Register (RSL) fusf_rcmd
 *
 * Field Fuse Read Command Register
 * To read an efuse, software writes FUSF_RCMD[ADDR, PEND] with the byte address of
 * the fuse in
 * question, then software can poll FUSF_RCMD[PEND]. When [PEND] is clear and if the efuse read
 * went to the efuse banks (e.g. EFUSE was set on the read), software can read FUSF_BNK_DATx
 * which contains all 128 fuses in the bank associated in ADDR.
 */
union bdk_fusf_rcmd
{
    uint64_t u;
    struct bdk_fusf_rcmd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be cleared when
                                                                 reading fuses directly (ie. [EFUSE] and [PEND] set). */
        uint64_t reserved_13           : 1;
        uint64_t pend                  : 1;  /**< [ 12: 12](SR/W/H) Software sets this bit on a write to start FUSE read operation.
                                                                 Hardware clears the bit when the read is complete and the DAT is
                                                                 valid.  MIO_FUS_READ_TIMES[RDSTB_WH] determines the time for this
                                                                 operation. */
        uint64_t reserved_11           : 1;
        uint64_t addr_hi               : 2;  /**< [ 10:  9](SR/W) Upper fuse address bits to extend space beyond 2k fuses. Valid range is
                                                                 0x0. Enumerated by FUSF_FUSE_NUM_E\<9:8\>. */
        uint64_t reserved_0_8          : 9;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_8          : 9;
        uint64_t addr_hi               : 2;  /**< [ 10:  9](SR/W) Upper fuse address bits to extend space beyond 2k fuses. Valid range is
                                                                 0x0. Enumerated by FUSF_FUSE_NUM_E\<9:8\>. */
        uint64_t reserved_11           : 1;
        uint64_t pend                  : 1;  /**< [ 12: 12](SR/W/H) Software sets this bit on a write to start FUSE read operation.
                                                                 Hardware clears the bit when the read is complete and the DAT is
                                                                 valid.  MIO_FUS_READ_TIMES[RDSTB_WH] determines the time for this
                                                                 operation. */
        uint64_t reserved_13           : 1;
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be cleared when
                                                                 reading fuses directly (ie. [EFUSE] and [PEND] set). */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    struct bdk_fusf_rcmd_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t reserved_16_23        : 8;
        uint64_t reserved_13_15        : 3;
        uint64_t pend                  : 1;  /**< [ 12: 12](SR/W/H) Software sets this bit on a write to start FUSE read operation.
                                                                 Hardware clears the bit when the read is complete and the DAT is
                                                                 valid.  MIO_FUS_READ_TIMES[RDSTB_WH] determines the time for this
                                                                 operation. */
        uint64_t reserved_11           : 1;
        uint64_t addr_hi               : 2;  /**< [ 10:  9](SR/W) Upper fuse address bits to extend space beyond 2k fuses. Valid range is
                                                                 0x0. Enumerated by FUSF_FUSE_NUM_E\<9:8\>. */
        uint64_t efuse                 : 1;  /**< [  8:  8](SR/W) When set, return data from the efuse storage rather than the local storage.
                                                                 Software should not change this field while the FUSF_RCMD[PEND] is set.
                                                                 It should wait for the hardware to clear the bit first. */
        uint64_t addr                  : 8;  /**< [  7:  0](SR/W) The byte address of the fuse to read.  Enumerated by FUSF_FUSE_NUM_E\<7:0\>. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 8;  /**< [  7:  0](SR/W) The byte address of the fuse to read.  Enumerated by FUSF_FUSE_NUM_E\<7:0\>. */
        uint64_t efuse                 : 1;  /**< [  8:  8](SR/W) When set, return data from the efuse storage rather than the local storage.
                                                                 Software should not change this field while the FUSF_RCMD[PEND] is set.
                                                                 It should wait for the hardware to clear the bit first. */
        uint64_t addr_hi               : 2;  /**< [ 10:  9](SR/W) Upper fuse address bits to extend space beyond 2k fuses. Valid range is
                                                                 0x0. Enumerated by FUSF_FUSE_NUM_E\<9:8\>. */
        uint64_t reserved_11           : 1;
        uint64_t pend                  : 1;  /**< [ 12: 12](SR/W/H) Software sets this bit on a write to start FUSE read operation.
                                                                 Hardware clears the bit when the read is complete and the DAT is
                                                                 valid.  MIO_FUS_READ_TIMES[RDSTB_WH] determines the time for this
                                                                 operation. */
        uint64_t reserved_13_15        : 3;
        uint64_t reserved_16_23        : 8;
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_fusf_rcmd_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t efuse                 : 1;  /**< [ 15: 15](SR/W) Efuse storage. When set, the return data is from the efuse
                                                                 bank directly.  When cleared data is read from the local storage. */
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be cleared when
                                                                 reading fuses directly (ie. [EFUSE] and [PEND] set). */
        uint64_t reserved_13           : 1;
        uint64_t pend                  : 1;  /**< [ 12: 12](SR/W/H) Software sets this bit to one on a write operation that starts
                                                                 the fuse read operation. Hardware clears this bit when the read
                                                                 operation is complete and FUS_BNK_DAT() is valid.
                                                                 FUS_READ_TIMES[RDSTB_WH] determines the time for the operation
                                                                 to complete. */
        uint64_t reserved_8_11         : 4;
        uint64_t addr                  : 4;  /**< [  7:  4](SR/W) Address. Specifies the bank address of 128 fuses to read.
                                                                 Software should not change this field while [PEND]
                                                                 is set. It must wait for the hardware to clear it. */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t addr                  : 4;  /**< [  7:  4](SR/W) Address. Specifies the bank address of 128 fuses to read.
                                                                 Software should not change this field while [PEND]
                                                                 is set. It must wait for the hardware to clear it. */
        uint64_t reserved_8_11         : 4;
        uint64_t pend                  : 1;  /**< [ 12: 12](SR/W/H) Software sets this bit to one on a write operation that starts
                                                                 the fuse read operation. Hardware clears this bit when the read
                                                                 operation is complete and FUS_BNK_DAT() is valid.
                                                                 FUS_READ_TIMES[RDSTB_WH] determines the time for the operation
                                                                 to complete. */
        uint64_t reserved_13           : 1;
        uint64_t voltage               : 1;  /**< [ 14: 14](SRO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be cleared when
                                                                 reading fuses directly (ie. [EFUSE] and [PEND] set). */
        uint64_t efuse                 : 1;  /**< [ 15: 15](SR/W) Efuse storage. When set, the return data is from the efuse
                                                                 bank directly.  When cleared data is read from the local storage. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_fusf_rcmd bdk_fusf_rcmd_t;

#define BDK_FUSF_RCMD BDK_FUSF_RCMD_FUNC()
static inline uint64_t BDK_FUSF_RCMD_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_RCMD_FUNC(void)
{
    return 0x87e004000100ll;
}

#define typedef_BDK_FUSF_RCMD bdk_fusf_rcmd_t
#define bustype_BDK_FUSF_RCMD BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_RCMD "FUSF_RCMD"
#define device_bar_BDK_FUSF_RCMD 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_RCMD 0
#define arguments_BDK_FUSF_RCMD -1,-1,-1,-1

/**
 * Register (RSL) fusf_rotpk#
 *
 * Field Fuse Root-of-Trust Public Key Registers
 */
union bdk_fusf_rotpkx
{
    uint64_t u;
    struct bdk_fusf_rotpkx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Hash of the root-of-trust public key (ROTPK). Reads field fuses FUSF_FUSE_NUM_E::ROTPK(). */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Hash of the root-of-trust public key (ROTPK). Reads field fuses FUSF_FUSE_NUM_E::ROTPK(). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_rotpkx_s cn; */
};
typedef union bdk_fusf_rotpkx bdk_fusf_rotpkx_t;

static inline uint64_t BDK_FUSF_ROTPKX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_ROTPKX(unsigned long a)
{
    if (a<=3)
        return 0x87e004000060ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("FUSF_ROTPKX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUSF_ROTPKX(a) bdk_fusf_rotpkx_t
#define bustype_BDK_FUSF_ROTPKX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_ROTPKX(a) "FUSF_ROTPKX"
#define device_bar_BDK_FUSF_ROTPKX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_ROTPKX(a) (a)
#define arguments_BDK_FUSF_ROTPKX(a) (a),-1,-1,-1

/**
 * Register (RSL) fusf_ssk#
 *
 * Field Fuse Secret Symmetric Key Registers
 */
union bdk_fusf_sskx
{
    uint64_t u;
    struct bdk_fusf_sskx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Secret symmetric key (SSK). Reads field fuses FUSF_FUSE_NUM_E::SSK(). */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Secret symmetric key (SSK). Reads field fuses FUSF_FUSE_NUM_E::SSK(). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_sskx_s cn; */
};
typedef union bdk_fusf_sskx bdk_fusf_sskx_t;

static inline uint64_t BDK_FUSF_SSKX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_SSKX(unsigned long a)
{
    if (a<=1)
        return 0x87e004000080ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("FUSF_SSKX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUSF_SSKX(a) bdk_fusf_sskx_t
#define bustype_BDK_FUSF_SSKX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_SSKX(a) "FUSF_SSKX"
#define device_bar_BDK_FUSF_SSKX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_SSKX(a) (a)
#define arguments_BDK_FUSF_SSKX(a) (a),-1,-1,-1

/**
 * Register (RSL) fusf_sw#
 *
 * Field Fuse Software Fuses Registers
 */
union bdk_fusf_swx
{
    uint64_t u;
    struct bdk_fusf_swx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Software assigned fuse data. Reads field fuses FUSF_FUSE_NUM_E::SW(). */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO) Software assigned fuse data. Reads field fuses FUSF_FUSE_NUM_E::SW(). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_swx_s cn; */
};
typedef union bdk_fusf_swx bdk_fusf_swx_t;

static inline uint64_t BDK_FUSF_SWX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_SWX(unsigned long a)
{
    if (a<=7)
        return 0x87e0040000c0ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("FUSF_SWX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUSF_SWX(a) bdk_fusf_swx_t
#define bustype_BDK_FUSF_SWX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_SWX(a) "FUSF_SWX"
#define device_bar_BDK_FUSF_SWX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_SWX(a) (a)
#define arguments_BDK_FUSF_SWX(a) (a),-1,-1,-1

/**
 * Register (RSL) fusf_wadr
 *
 * Field Fuse Write Address Register
 */
union bdk_fusf_wadr
{
    uint64_t u;
    struct bdk_fusf_wadr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t addr                  : 4;  /**< [  3:  0](SR/W) Indicates which of the banks of 128 fuses to blow.  Enumerated by FUSF_FUSE_NUM_E\<10:7\>. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 4;  /**< [  3:  0](SR/W) Indicates which of the banks of 128 fuses to blow.  Enumerated by FUSF_FUSE_NUM_E\<10:7\>. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fusf_wadr_s cn; */
};
typedef union bdk_fusf_wadr bdk_fusf_wadr_t;

#define BDK_FUSF_WADR BDK_FUSF_WADR_FUNC()
static inline uint64_t BDK_FUSF_WADR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUSF_WADR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e004000108ll;
    __bdk_csr_fatal("FUSF_WADR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUSF_WADR bdk_fusf_wadr_t
#define bustype_BDK_FUSF_WADR BDK_CSR_TYPE_RSL
#define basename_BDK_FUSF_WADR "FUSF_WADR"
#define device_bar_BDK_FUSF_WADR 0x0 /* PF_BAR0 */
#define busnum_BDK_FUSF_WADR 0
#define arguments_BDK_FUSF_WADR -1,-1,-1,-1

#endif /* __BDK_CSRS_FUSF_H__ */
