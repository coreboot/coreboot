#ifndef __BDK_CSRS_FUS_H__
#define __BDK_CSRS_FUS_H__
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
 * Cavium FUS.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration fus_bar_e
 *
 * Fuse Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_FUS_BAR_E_FUS_PF_BAR0 (0x87e003000000ll)
#define BDK_FUS_BAR_E_FUS_PF_BAR0_SIZE 0x10000ull

/**
 * Enumeration fus_fuse_num_e
 *
 * INTERNAL: Fuse Number Enumeration
 *
 * Enumerates the fuse numbers.
 */
#define BDK_FUS_FUSE_NUM_E_ALT_BANKX(a) (0xf80 + (a))
#define BDK_FUS_FUSE_NUM_E_ALT_FUSEX(a) (0x7b + (a))
#define BDK_FUS_FUSE_NUM_E_AP_CRIPPLEX(a) (0x180 + (a))
#define BDK_FUS_FUSE_NUM_E_AP_NOCRYPTOX(a) (0x200 + (a))
#define BDK_FUS_FUSE_NUM_E_AP_POWER_LIMITX(a) (0x259 + (a))
#define BDK_FUS_FUSE_NUM_E_BCH_DISABLE (0x202)
#define BDK_FUS_FUSE_NUM_E_BISR_FUSED_ONLY (0x53)
#define BDK_FUS_FUSE_NUM_E_BSR_LOBE_DISABLEX(a) (0x290 + (a))
#define BDK_FUS_FUSE_NUM_E_CGX_CRIPPLEX(a) (0x228 + (a))
#define BDK_FUS_FUSE_NUM_E_CHIP_IDX(a) (8 + (a))
#define BDK_FUS_FUSE_NUM_E_CHIP_TYPEX(a) (0 + (a))
#define BDK_FUS_FUSE_NUM_E_CORE_INIT_MULX(a) (0x43 + (a))
#define BDK_FUS_FUSE_NUM_E_CORE_INIT_PLL (0x4a)
#define BDK_FUS_FUSE_NUM_E_CORE_MAX_MULX(a) (0x4b + (a))
#define BDK_FUS_FUSE_NUM_E_CPT0_NOCRYPTOX(a) (0x255 + (a))
#define BDK_FUS_FUSE_NUM_E_CPT1_NOCRYPTOX(a) (0x257 + (a))
#define BDK_FUS_FUSE_NUM_E_CPT_EXE_DISABLEX(a) (0x580 + (a))
#define BDK_FUS_FUSE_NUM_E_CPT_INIT_MULX(a) (0x5d + (a))
#define BDK_FUS_FUSE_NUM_E_CPT_INIT_PLL (0x64)
#define BDK_FUS_FUSE_NUM_E_CPT_MAX_MULX(a) (0x65 + (a))
#define BDK_FUS_FUSE_NUM_E_DESX(a) (0x540 + (a))
#define BDK_FUS_FUSE_NUM_E_DFA_INFO_CLMX(a) (0x220 + (a))
#define BDK_FUS_FUSE_NUM_E_DFA_INFO_DTEX(a) (0x21d + (a))
#define BDK_FUS_FUSE_NUM_E_DLL0_PD_DELAYX(a) (0x88 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL0_SETTINGSX(a) (0x80 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL10_PD_DELAYX(a) (0x100 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL10_SETTINGSX(a) (0xf8 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL11_PD_DELAYX(a) (0x10c + (a))
#define BDK_FUS_FUSE_NUM_E_DLL11_SETTINGSX(a) (0x104 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL12_PD_DELAYX(a) (0x118 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL12_SETTINGSX(a) (0x110 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL13_PD_DELAYX(a) (0x124 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL13_SETTINGSX(a) (0x11c + (a))
#define BDK_FUS_FUSE_NUM_E_DLL14_PD_DELAYX(a) (0x130 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL14_SETTINGSX(a) (0x128 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL15_PD_DELAYX(a) (0x13c + (a))
#define BDK_FUS_FUSE_NUM_E_DLL15_SETTINGSX(a) (0x134 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL1_PD_DELAYX(a) (0x94 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL1_SETTINGSX(a) (0x8c + (a))
#define BDK_FUS_FUSE_NUM_E_DLL2_PD_DELAYX(a) (0xa0 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL2_SETTINGSX(a) (0x98 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL3_PD_DELAYX(a) (0xac + (a))
#define BDK_FUS_FUSE_NUM_E_DLL3_SETTINGSX(a) (0xa4 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL4_PD_DELAYX(a) (0xb8 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL4_SETTINGSX(a) (0xb0 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL5_PD_DELAYX(a) (0xc4 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL5_SETTINGSX(a) (0xbc + (a))
#define BDK_FUS_FUSE_NUM_E_DLL6_PD_DELAYX(a) (0xd0 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL6_SETTINGSX(a) (0xc8 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL7_PD_DELAYX(a) (0xdc + (a))
#define BDK_FUS_FUSE_NUM_E_DLL7_SETTINGSX(a) (0xd4 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL8_PD_DELAYX(a) (0xe8 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL8_SETTINGSX(a) (0xe0 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL9_PD_DELAYX(a) (0xf4 + (a))
#define BDK_FUS_FUSE_NUM_E_DLL9_SETTINGSX(a) (0xec + (a))
#define BDK_FUS_FUSE_NUM_E_DLL_INIT_SETTINGSX(a) (0x6c + (a))
#define BDK_FUS_FUSE_NUM_E_DLL_LOCK_FREEZE (0x54)
#define BDK_FUS_FUSE_NUM_E_DLL_REFRESH_RATEX(a) (0x55 + (a))
#define BDK_FUS_FUSE_NUM_E_DRO_CRIPPLE (0x73)
#define BDK_FUS_FUSE_NUM_E_EFUS_LCK_CFG (0x78)
#define BDK_FUS_FUSE_NUM_E_EFUS_LCK_MAN (0x79)
#define BDK_FUS_FUSE_NUM_E_EFUS_LCK_PRD (0x7a)
#define BDK_FUS_FUSE_NUM_E_EMA0X(a) (0x10 + (a))
#define BDK_FUS_FUSE_NUM_E_EMA1X(a) (0x16 + (a))
#define BDK_FUS_FUSE_NUM_E_EMA2X(a) (0x1c + (a))
#define BDK_FUS_FUSE_NUM_E_EMA3X(a) (0x23 + (a))
#define BDK_FUS_FUSE_NUM_E_EMA4X(a) (0x25 + (a))
#define BDK_FUS_FUSE_NUM_E_EMA5 (0x28)
#define BDK_FUS_FUSE_NUM_E_FDFX(a) (0xa00 + (a))
#define BDK_FUS_FUSE_NUM_E_GSER_ERCX(a) (0xf00 + (a))
#define BDK_FUS_FUSE_NUM_E_LLC_CRIPPLEX(a) (0x216 + (a))
#define BDK_FUS_FUSE_NUM_E_LMC_CRIPPLEX(a) (0x283 + (a))
#define BDK_FUS_FUSE_NUM_E_LMC_HALF (0x203)
#define BDK_FUS_FUSE_NUM_E_METAL_FIXX(a) (0x2c + (a))
#define BDK_FUS_FUSE_NUM_E_MFG_INFOX(a) (0x400 + (a))
#define BDK_FUS_FUSE_NUM_E_MLC_CRIPPLEX(a) (0x219 + (a))
#define BDK_FUS_FUSE_NUM_E_NODE1TRAPENA (0x250)
#define BDK_FUS_FUSE_NUM_E_NODFA_CP2 (0x21c)
#define BDK_FUS_FUSE_NUM_E_NPC_CRIPPLE (0x280)
#define BDK_FUS_FUSE_NUM_E_OCX_CRIPPLE (0x227)
#define BDK_FUS_FUSE_NUM_E_PDFX(a) (0x640 + (a))
#define BDK_FUS_FUSE_NUM_E_PEM_CRIPPLEX(a) (0x230 + (a))
#define BDK_FUS_FUSE_NUM_E_PNR_INIT_MULX(a) (0x34 + (a))
#define BDK_FUS_FUSE_NUM_E_PNR_INIT_PLL (0x3b)
#define BDK_FUS_FUSE_NUM_E_PNR_MAX_MULX(a) (0x3c + (a))
#define BDK_FUS_FUSE_NUM_E_POWER_LIMITX(a) (0x225 + (a))
#define BDK_FUS_FUSE_NUM_E_PVTX(a) (0x680 + (a))
#define BDK_FUS_FUSE_NUM_E_RAID_CRIPPLE (0x224)
#define BDK_FUS_FUSE_NUM_E_REFCLK_CHECK (0x52)
#define BDK_FUS_FUSE_NUM_E_REPAIRX(a) (0x1000 + (a))
#define BDK_FUS_FUSE_NUM_E_ROM_INFOX(a) (0x3f0 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD140X(a) (0x140 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD205 (0x205)
#define BDK_FUS_FUSE_NUM_E_RSVD251X(a) (0x251 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD254 (0x254)
#define BDK_FUS_FUSE_NUM_E_RSVD25BX(a) (0x25b + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD264X(a) (0x264 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD281 (0x281)
#define BDK_FUS_FUSE_NUM_E_RSVD282 (0x282)
#define BDK_FUS_FUSE_NUM_E_RSVD28B (0x28b)
#define BDK_FUS_FUSE_NUM_E_RSVD28F (0x28f)
#define BDK_FUS_FUSE_NUM_E_RSVD29X(a) (0x29 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD29DX(a) (0x29d + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD31X(a) (0x31 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD74 (0x74)
#define BDK_FUS_FUSE_NUM_E_RSVD780X(a) (0x780 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD801X(a) (0x801 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVD914X(a) (0x914 + (a))
#define BDK_FUS_FUSE_NUM_E_RSVDE00X(a) (0xe00 + (a))
#define BDK_FUS_FUSE_NUM_E_RUN_PLATFORMX(a) (0x75 + (a))
#define BDK_FUS_FUSE_NUM_E_RVU_CRIPPLE (0x28e)
#define BDK_FUS_FUSE_NUM_E_SATA_CRIPPLEX(a) (0x260 + (a))
#define BDK_FUS_FUSE_NUM_E_SERIALX(a) (0x500 + (a))
#define BDK_FUS_FUSE_NUM_E_SSO_CRIPPLE (0x253)
#define BDK_FUS_FUSE_NUM_E_TGGX(a) (0x600 + (a))
#define BDK_FUS_FUSE_NUM_E_TS0_M_CX(a) (0x810 + (a))
#define BDK_FUS_FUSE_NUM_E_TS0_N_OFFX(a) (0x81c + (a))
#define BDK_FUS_FUSE_NUM_E_TS0_STROBE_COMP_DLYX(a) (0x827 + (a))
#define BDK_FUS_FUSE_NUM_E_TS1_M_CX(a) (0x82a + (a))
#define BDK_FUS_FUSE_NUM_E_TS1_N_OFFX(a) (0x836 + (a))
#define BDK_FUS_FUSE_NUM_E_TS1_STROBE_COMP_DLYX(a) (0x841 + (a))
#define BDK_FUS_FUSE_NUM_E_TS2_M_CX(a) (0x844 + (a))
#define BDK_FUS_FUSE_NUM_E_TS2_N_OFFX(a) (0x850 + (a))
#define BDK_FUS_FUSE_NUM_E_TS2_STROBE_COMP_DLYX(a) (0x85b + (a))
#define BDK_FUS_FUSE_NUM_E_TS3_M_CX(a) (0x85e + (a))
#define BDK_FUS_FUSE_NUM_E_TS3_N_OFFX(a) (0x86a + (a))
#define BDK_FUS_FUSE_NUM_E_TS3_STROBE_COMP_DLYX(a) (0x875 + (a))
#define BDK_FUS_FUSE_NUM_E_TS4_M_CX(a) (0x878 + (a))
#define BDK_FUS_FUSE_NUM_E_TS4_N_OFFX(a) (0x884 + (a))
#define BDK_FUS_FUSE_NUM_E_TS4_STROBE_COMP_DLYX(a) (0x88f + (a))
#define BDK_FUS_FUSE_NUM_E_TS5_M_CX(a) (0x892 + (a))
#define BDK_FUS_FUSE_NUM_E_TS5_N_OFFX(a) (0x89e + (a))
#define BDK_FUS_FUSE_NUM_E_TS5_STROBE_COMP_DLYX(a) (0x8a9 + (a))
#define BDK_FUS_FUSE_NUM_E_TS6_M_CX(a) (0x8ac + (a))
#define BDK_FUS_FUSE_NUM_E_TS6_N_OFFX(a) (0x8b8 + (a))
#define BDK_FUS_FUSE_NUM_E_TS6_STROBE_COMP_DLYX(a) (0x8c3 + (a))
#define BDK_FUS_FUSE_NUM_E_TS7_M_CX(a) (0x8c6 + (a))
#define BDK_FUS_FUSE_NUM_E_TS7_N_OFFX(a) (0x8d2 + (a))
#define BDK_FUS_FUSE_NUM_E_TS7_STROBE_COMP_DLYX(a) (0x8dd + (a))
#define BDK_FUS_FUSE_NUM_E_TS8_M_CX(a) (0x8e0 + (a))
#define BDK_FUS_FUSE_NUM_E_TS8_N_OFFX(a) (0x8ec + (a))
#define BDK_FUS_FUSE_NUM_E_TS8_STROBE_COMP_DLYX(a) (0x8f7 + (a))
#define BDK_FUS_FUSE_NUM_E_TS9_M_CX(a) (0x8fa + (a))
#define BDK_FUS_FUSE_NUM_E_TS9_N_OFFX(a) (0x906 + (a))
#define BDK_FUS_FUSE_NUM_E_TS9_STROBE_COMP_DLYX(a) (0x911 + (a))
#define BDK_FUS_FUSE_NUM_E_TS_CALBRATED (0x800)
#define BDK_FUS_FUSE_NUM_E_USBDRD_CRIPPLEX(a) (0x28c + (a))
#define BDK_FUS_FUSE_NUM_E_ZIP_CRIPPLEX(a) (0x206 + (a))
#define BDK_FUS_FUSE_NUM_E_ZIP_CTL_DISABLE (0x204)

/**
 * Register (RSL) fus_bnk_dat#
 *
 * Fuse Bank Store Register
 * The initial state of FUS_BNK_DAT() is as if bank6 were just read,
 * i.e. DAT* = fus[895:768].
 */
union bdk_fus_bnk_datx
{
    uint64_t u;
    struct bdk_fus_bnk_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](R/W/H) Efuse bank store. For read operations, [DAT] gets the fus bank last read. For write
                                                                 operations, the [DAT] determines which fuses to blow. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](R/W/H) Efuse bank store. For read operations, [DAT] gets the fus bank last read. For write
                                                                 operations, the [DAT] determines which fuses to blow. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fus_bnk_datx_s cn; */
};
typedef union bdk_fus_bnk_datx bdk_fus_bnk_datx_t;

static inline uint64_t BDK_FUS_BNK_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUS_BNK_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e003001520ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("FUS_BNK_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUS_BNK_DATX(a) bdk_fus_bnk_datx_t
#define bustype_BDK_FUS_BNK_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUS_BNK_DATX(a) "FUS_BNK_DATX"
#define device_bar_BDK_FUS_BNK_DATX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUS_BNK_DATX(a) (a)
#define arguments_BDK_FUS_BNK_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) fus_cache#
 *
 * Fuse Cache Register
 * This register returns the cached state of every fuse, organized into 64-fuse
 * chunks. Each bit corresponds to a fuse enumerated by FUS_FUSE_NUM_E.
 */
union bdk_fus_cachex
{
    uint64_t u;
    struct bdk_fus_cachex_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](RO/H) Reads the cached fuse value.
                                                                 Modifications to the cache will take effect on the next
                                                                 chip domain reset and are lost on a cold domain reset. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](RO/H) Reads the cached fuse value.
                                                                 Modifications to the cache will take effect on the next
                                                                 chip domain reset and are lost on a cold domain reset. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fus_cachex_s cn; */
};
typedef union bdk_fus_cachex bdk_fus_cachex_t;

static inline uint64_t BDK_FUS_CACHEX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUS_CACHEX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=63))
        return 0x87e003001000ll + 8ll * ((a) & 0x3f);
    __bdk_csr_fatal("FUS_CACHEX", 1, a, 0, 0, 0);
}

#define typedef_BDK_FUS_CACHEX(a) bdk_fus_cachex_t
#define bustype_BDK_FUS_CACHEX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_FUS_CACHEX(a) "FUS_CACHEX"
#define device_bar_BDK_FUS_CACHEX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_FUS_CACHEX(a) (a)
#define arguments_BDK_FUS_CACHEX(a) (a),-1,-1,-1

/**
 * Register (RSL) fus_const
 *
 * Fuse Constants Register
 */
union bdk_fus_const
{
    uint64_t u;
    struct bdk_fus_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t repair_banks          : 8;  /**< [ 15:  8](RO) Number of 128-bit memory repair banks present. */
        uint64_t fuse_banks            : 8;  /**< [  7:  0](RO) Number of 128-bit general purpose fuse banks present. */
#else /* Word 0 - Little Endian */
        uint64_t fuse_banks            : 8;  /**< [  7:  0](RO) Number of 128-bit general purpose fuse banks present. */
        uint64_t repair_banks          : 8;  /**< [ 15:  8](RO) Number of 128-bit memory repair banks present. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fus_const_s cn; */
};
typedef union bdk_fus_const bdk_fus_const_t;

#define BDK_FUS_CONST BDK_FUS_CONST_FUNC()
static inline uint64_t BDK_FUS_CONST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUS_CONST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e003001578ll;
    __bdk_csr_fatal("FUS_CONST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUS_CONST bdk_fus_const_t
#define bustype_BDK_FUS_CONST BDK_CSR_TYPE_RSL
#define basename_BDK_FUS_CONST "FUS_CONST"
#define device_bar_BDK_FUS_CONST 0x0 /* PF_BAR0 */
#define busnum_BDK_FUS_CONST 0
#define arguments_BDK_FUS_CONST -1,-1,-1,-1

/**
 * Register (RSL) fus_prog
 *
 * INTERNAL: Fuse Programming Register
 */
union bdk_fus_prog
{
    uint64_t u;
    struct bdk_fus_prog_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t efuse                 : 1;  /**< [ 15: 15](R/W) Efuse storage. When set, the data is written directly to the efuse
                                                                 bank.  When cleared, data is soft blown to local storage.
                                                                 A soft blown fuse is subject to lockdown fuses.
                                                                 Soft blown fuses will become active after a chip domain reset
                                                                 but will not persist through a cold domain reset. */
        uint64_t voltage               : 1;  /**< [ 14: 14](RO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be set when
                                                                 programming fuses (ie. [EFUSE] and [PROG] set). */
        uint64_t reserved_13           : 1;
        uint64_t prog                  : 1;  /**< [ 12: 12](R/W/H) Internal:
                                                                 When written to one by software, blow the fuse bank. Hardware will
                                                                 clear the field when the program operation is complete.
                                                                 To write a bank of fuses, software must write the fuse data into
                                                                 FUS_BNK_DAT().  Then it writes the ADDR and EFUSE fields of this register
                                                                 and sets the PROG bit.  Hardware will clear the [PROG] when the write is
                                                                 completed.  New fuses will become active after a chip domain reset. */
        uint64_t reserved_11           : 1;
        uint64_t addr                  : 7;  /**< [ 10:  4](R/W) Indicates which of the banks of 128 fuses to blow. Software
                                                                 should not change this field while the FUS_PROG[PROG] bit is set. */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t addr                  : 7;  /**< [ 10:  4](R/W) Indicates which of the banks of 128 fuses to blow. Software
                                                                 should not change this field while the FUS_PROG[PROG] bit is set. */
        uint64_t reserved_11           : 1;
        uint64_t prog                  : 1;  /**< [ 12: 12](R/W/H) Internal:
                                                                 When written to one by software, blow the fuse bank. Hardware will
                                                                 clear the field when the program operation is complete.
                                                                 To write a bank of fuses, software must write the fuse data into
                                                                 FUS_BNK_DAT().  Then it writes the ADDR and EFUSE fields of this register
                                                                 and sets the PROG bit.  Hardware will clear the [PROG] when the write is
                                                                 completed.  New fuses will become active after a chip domain reset. */
        uint64_t reserved_13           : 1;
        uint64_t voltage               : 1;  /**< [ 14: 14](RO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be set when
                                                                 programming fuses (ie. [EFUSE] and [PROG] set). */
        uint64_t efuse                 : 1;  /**< [ 15: 15](R/W) Efuse storage. When set, the data is written directly to the efuse
                                                                 bank.  When cleared, data is soft blown to local storage.
                                                                 A soft blown fuse is subject to lockdown fuses.
                                                                 Soft blown fuses will become active after a chip domain reset
                                                                 but will not persist through a cold domain reset. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fus_prog_s cn; */
};
typedef union bdk_fus_prog bdk_fus_prog_t;

#define BDK_FUS_PROG BDK_FUS_PROG_FUNC()
static inline uint64_t BDK_FUS_PROG_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUS_PROG_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e003001510ll;
    __bdk_csr_fatal("FUS_PROG", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUS_PROG bdk_fus_prog_t
#define bustype_BDK_FUS_PROG BDK_CSR_TYPE_RSL
#define basename_BDK_FUS_PROG "FUS_PROG"
#define device_bar_BDK_FUS_PROG 0x0 /* PF_BAR0 */
#define busnum_BDK_FUS_PROG 0
#define arguments_BDK_FUS_PROG -1,-1,-1,-1

/**
 * Register (RSL) fus_rcmd
 *
 * Fuse Read Command Register
 * Read Fuse Banks.
 */
union bdk_fus_rcmd
{
    uint64_t u;
    struct bdk_fus_rcmd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t efuse                 : 1;  /**< [ 15: 15](R/W) Efuse storage. When set, the return data is from the efuse
                                                                 bank directly.  When cleared data is read from the local storage. */
        uint64_t voltage               : 1;  /**< [ 14: 14](RO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be cleared when
                                                                 reading fuses directly (ie. [EFUSE] and [PEND] set). */
        uint64_t reserved_13           : 1;
        uint64_t pend                  : 1;  /**< [ 12: 12](R/W/H) Software sets this bit to one on a write operation that starts
                                                                 the fuse read operation. Hardware clears this bit when the read
                                                                 operation is complete and FUS_BNK_DAT() is valid.
                                                                 FUS_READ_TIMES[RDSTB_WH] determines the time for the operation
                                                                 to complete. */
        uint64_t reserved_11           : 1;
        uint64_t addr                  : 7;  /**< [ 10:  4](R/W) Address. Specifies the bank address of 128 fuses to read.
                                                                 Software should not change this field while [PEND]
                                                                 is set. It must wait for the hardware to clear it. */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t addr                  : 7;  /**< [ 10:  4](R/W) Address. Specifies the bank address of 128 fuses to read.
                                                                 Software should not change this field while [PEND]
                                                                 is set. It must wait for the hardware to clear it. */
        uint64_t reserved_11           : 1;
        uint64_t pend                  : 1;  /**< [ 12: 12](R/W/H) Software sets this bit to one on a write operation that starts
                                                                 the fuse read operation. Hardware clears this bit when the read
                                                                 operation is complete and FUS_BNK_DAT() is valid.
                                                                 FUS_READ_TIMES[RDSTB_WH] determines the time for the operation
                                                                 to complete. */
        uint64_t reserved_13           : 1;
        uint64_t voltage               : 1;  /**< [ 14: 14](RO) Efuse programming voltage status.  When set, EFUSE banks have
                                                                 programming voltage applied.  Required to be cleared when
                                                                 reading fuses directly (ie. [EFUSE] and [PEND] set). */
        uint64_t efuse                 : 1;  /**< [ 15: 15](R/W) Efuse storage. When set, the return data is from the efuse
                                                                 bank directly.  When cleared data is read from the local storage. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fus_rcmd_s cn; */
};
typedef union bdk_fus_rcmd bdk_fus_rcmd_t;

#define BDK_FUS_RCMD BDK_FUS_RCMD_FUNC()
static inline uint64_t BDK_FUS_RCMD_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUS_RCMD_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e003001500ll;
    __bdk_csr_fatal("FUS_RCMD", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUS_RCMD bdk_fus_rcmd_t
#define bustype_BDK_FUS_RCMD BDK_CSR_TYPE_RSL
#define basename_BDK_FUS_RCMD "FUS_RCMD"
#define device_bar_BDK_FUS_RCMD 0x0 /* PF_BAR0 */
#define busnum_BDK_FUS_RCMD 0
#define arguments_BDK_FUS_RCMD -1,-1,-1,-1

/**
 * Register (RSL) fus_read_times
 *
 * Fuse Read Times Register
 * The reset values correspond to accesses of internal fuses with PLL reference clock
 * up to 115 MHz.  If any of the formulas below result in a value less than 0x0, the
 * corresponding timing parameter should be set to zero.
 *
 * Prior to issuing a read operation to the fuse banks (via FUS_RCMD),
 * this register should be written with the timing parameters that will be read.
 * This register should not be written while FUS_RCMD[PEND] = 1.
 */
union bdk_fus_read_times
{
    uint64_t u;
    struct bdk_fus_read_times_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t done                  : 4;  /**< [ 31: 28](R/W) Hold time of CSB, PGENB, and LOAD with respect to falling edge
                                                                 of STROBE for read and write mode in PLL_REF_CLK + 1 cycles.
                                                                 Timing specs are th_CS = 6 ns, th_PG = 10 ns, th_LD_p = 7 ns.
                                                                 Default of 0x0 yields 8.7 ns at 115 MHz. */
        uint64_t ahd                   : 4;  /**< [ 27: 24](R/W) Hold time of A with respect to falling edge of STROBE
                                                                 for read and write modes in PLL_REF_CLK + 1 cycles.
                                                                 Timing spec of tsu_A_r and tsu_A_p is 3 ns min.
                                                                 Default of 0x0 yields 8.7 ns at 115MHz. */
        uint64_t wrstb_wh              : 12; /**< [ 23: 12](R/W) Pulse width high of STROBE in write mode in PLL_REF_CLK + 1 cycles.
                                                                 Timing spec of twh_SB_p is 9.8 us max.
                                                                 Default of 0x3E8 yields 8.7 us at 115MHz. */
        uint64_t rdstb_wh              : 4;  /**< [ 11:  8](R/W) Pulse width high of STROBE in read mode in PLL_REF_CLK + 1 cycles.
                                                                 Timing spec of twh_SB_p is 20 ns min.
                                                                 Default of 0x2 yields 26.1 ns at 115 MHz. */
        uint64_t asu                   : 4;  /**< [  7:  4](R/W) Setup time of A to rising edge of STROBE for read and write
                                                                 modes in PLL_REF_CLK cycles.
                                                                 Timing spec of tsu_A_r and tsu_A_p is 12 ns min.
                                                                 Default of 0x1 yields 17.4 ns at 115 MHz. */
        uint64_t setup                 : 4;  /**< [  3:  0](R/W) Setup time of CSB, PGENB, LOAD to rising edge of STROBE
                                                                 in read and write modes in PLL_REF_CLK + 1 cycles.
                                                                 tsu_CS = 16 ns, tsu_PG = 14 ns, tsu_LD_r = 10 ns.
                                                                 Default of 0x0 yields 8.7 ns plus ASU cycles at 115 MHz. */
#else /* Word 0 - Little Endian */
        uint64_t setup                 : 4;  /**< [  3:  0](R/W) Setup time of CSB, PGENB, LOAD to rising edge of STROBE
                                                                 in read and write modes in PLL_REF_CLK + 1 cycles.
                                                                 tsu_CS = 16 ns, tsu_PG = 14 ns, tsu_LD_r = 10 ns.
                                                                 Default of 0x0 yields 8.7 ns plus ASU cycles at 115 MHz. */
        uint64_t asu                   : 4;  /**< [  7:  4](R/W) Setup time of A to rising edge of STROBE for read and write
                                                                 modes in PLL_REF_CLK cycles.
                                                                 Timing spec of tsu_A_r and tsu_A_p is 12 ns min.
                                                                 Default of 0x1 yields 17.4 ns at 115 MHz. */
        uint64_t rdstb_wh              : 4;  /**< [ 11:  8](R/W) Pulse width high of STROBE in read mode in PLL_REF_CLK + 1 cycles.
                                                                 Timing spec of twh_SB_p is 20 ns min.
                                                                 Default of 0x2 yields 26.1 ns at 115 MHz. */
        uint64_t wrstb_wh              : 12; /**< [ 23: 12](R/W) Pulse width high of STROBE in write mode in PLL_REF_CLK + 1 cycles.
                                                                 Timing spec of twh_SB_p is 9.8 us max.
                                                                 Default of 0x3E8 yields 8.7 us at 115MHz. */
        uint64_t ahd                   : 4;  /**< [ 27: 24](R/W) Hold time of A with respect to falling edge of STROBE
                                                                 for read and write modes in PLL_REF_CLK + 1 cycles.
                                                                 Timing spec of tsu_A_r and tsu_A_p is 3 ns min.
                                                                 Default of 0x0 yields 8.7 ns at 115MHz. */
        uint64_t done                  : 4;  /**< [ 31: 28](R/W) Hold time of CSB, PGENB, and LOAD with respect to falling edge
                                                                 of STROBE for read and write mode in PLL_REF_CLK + 1 cycles.
                                                                 Timing specs are th_CS = 6 ns, th_PG = 10 ns, th_LD_p = 7 ns.
                                                                 Default of 0x0 yields 8.7 ns at 115 MHz. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fus_read_times_s cn; */
};
typedef union bdk_fus_read_times bdk_fus_read_times_t;

#define BDK_FUS_READ_TIMES BDK_FUS_READ_TIMES_FUNC()
static inline uint64_t BDK_FUS_READ_TIMES_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUS_READ_TIMES_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e003001570ll;
    __bdk_csr_fatal("FUS_READ_TIMES", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUS_READ_TIMES bdk_fus_read_times_t
#define bustype_BDK_FUS_READ_TIMES BDK_CSR_TYPE_RSL
#define basename_BDK_FUS_READ_TIMES "FUS_READ_TIMES"
#define device_bar_BDK_FUS_READ_TIMES 0x0 /* PF_BAR0 */
#define busnum_BDK_FUS_READ_TIMES 0
#define arguments_BDK_FUS_READ_TIMES -1,-1,-1,-1

/**
 * Register (RSL) fus_soft_repair
 *
 * INTERNAL: Fuse Soft Repair Register
 *
 * Internal:
 * Aka `soft blow'. Upon reset fuse repairs are loaded into FUS_FUSE_NUM_E::REPAIR()
 * fuses as they are loaded into the memories.
 */
union bdk_fus_soft_repair
{
    uint64_t u;
    struct bdk_fus_soft_repair_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t auto_dly              : 16; /**< [ 47: 32](R/W/H) Internal:
                                                                 Autoblow Delay.  Power supply ramp delay in 1uS increments from enabling
                                                                 [AUTOBLOW] to programming first bit. */
        uint64_t reserved_18_31        : 14;
        uint64_t autoblow              : 1;  /**< [ 17: 17](R/W/H) Internal:
                                                                 Set to initiate burning of defect fuses to fuse macro. Clears when fuses are
                                                                 blown. */
        uint64_t reserved_16           : 1;
        uint64_t numdefects            : 8;  /**< [ 15:  8](RO/H) Internal:
                                                                 After reset/BIST indicates the number of memory defects reported. Defects are
                                                                 stored in fuses FUS_FUSE_NUM_E::REPAIR() from index [NUMREPAIRS]*32 to
                                                                 ([NUMREPAIRS]*32 + [NUMDEFECTS]*32 - 1). */
        uint64_t numrepairs            : 8;  /**< [  7:  0](R/W) Internal:
                                                                 Indicates the number of repairs loaded from repair mem to the memories on
                                                                 the last chip/core/mcp/scp reset. */
#else /* Word 0 - Little Endian */
        uint64_t numrepairs            : 8;  /**< [  7:  0](R/W) Internal:
                                                                 Indicates the number of repairs loaded from repair mem to the memories on
                                                                 the last chip/core/mcp/scp reset. */
        uint64_t numdefects            : 8;  /**< [ 15:  8](RO/H) Internal:
                                                                 After reset/BIST indicates the number of memory defects reported. Defects are
                                                                 stored in fuses FUS_FUSE_NUM_E::REPAIR() from index [NUMREPAIRS]*32 to
                                                                 ([NUMREPAIRS]*32 + [NUMDEFECTS]*32 - 1). */
        uint64_t reserved_16           : 1;
        uint64_t autoblow              : 1;  /**< [ 17: 17](R/W/H) Internal:
                                                                 Set to initiate burning of defect fuses to fuse macro. Clears when fuses are
                                                                 blown. */
        uint64_t reserved_18_31        : 14;
        uint64_t auto_dly              : 16; /**< [ 47: 32](R/W/H) Internal:
                                                                 Autoblow Delay.  Power supply ramp delay in 1uS increments from enabling
                                                                 [AUTOBLOW] to programming first bit. */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_fus_soft_repair_s cn; */
};
typedef union bdk_fus_soft_repair bdk_fus_soft_repair_t;

#define BDK_FUS_SOFT_REPAIR BDK_FUS_SOFT_REPAIR_FUNC()
static inline uint64_t BDK_FUS_SOFT_REPAIR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_FUS_SOFT_REPAIR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e003001540ll;
    __bdk_csr_fatal("FUS_SOFT_REPAIR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_FUS_SOFT_REPAIR bdk_fus_soft_repair_t
#define bustype_BDK_FUS_SOFT_REPAIR BDK_CSR_TYPE_RSL
#define basename_BDK_FUS_SOFT_REPAIR "FUS_SOFT_REPAIR"
#define device_bar_BDK_FUS_SOFT_REPAIR 0x0 /* PF_BAR0 */
#define busnum_BDK_FUS_SOFT_REPAIR 0
#define arguments_BDK_FUS_SOFT_REPAIR -1,-1,-1,-1

#endif /* __BDK_CSRS_FUS_H__ */
