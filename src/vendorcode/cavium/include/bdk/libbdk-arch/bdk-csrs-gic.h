#ifndef __BDK_CSRS_GIC_H__
#define __BDK_CSRS_GIC_H__
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
 * Cavium GIC.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration gic_bar_e
 *
 * GIC Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_GIC_BAR_E_GIC_PF_BAR0 (0x801000000000ll)
#define BDK_GIC_BAR_E_GIC_PF_BAR0_SIZE 0x20000ull
#define BDK_GIC_BAR_E_GIC_PF_BAR2 (0x801000020000ll)
#define BDK_GIC_BAR_E_GIC_PF_BAR2_SIZE 0x20000ull
#define BDK_GIC_BAR_E_GIC_PF_BAR4 (0x801080000000ll)
#define BDK_GIC_BAR_E_GIC_PF_BAR4_SIZE 0x1000000ull

/**
 * Enumeration gic_int_req_e
 *
 * GIC Performance Counter Enumeration
 * Enumerates the index of GIC_INT_REQ()_PC.
 */
#define BDK_GIC_INT_REQ_E_GICD_CLRSPI_NSR_PC (1)
#define BDK_GIC_INT_REQ_E_GICD_CLRSPI_SR_PC (3)
#define BDK_GIC_INT_REQ_E_GICD_SETSPI_NSR_PC (0)
#define BDK_GIC_INT_REQ_E_GICD_SETSPI_SR_PC (2)
#define BDK_GIC_INT_REQ_E_GICR_CLRLPIR (6)
#define BDK_GIC_INT_REQ_E_GICR_SETLPIR (5)
#define BDK_GIC_INT_REQ_E_GITS_TRANSLATER (4)

/**
 * Enumeration gits_cmd_err_e
 *
 * GIC ITS Command Error Enumeration
 * The actual 24-bit ITS command SEI is defined as {8'h01,
 * GITS_CMD_TYPE_E(8-bit), GITS_CMD_ERR_E(8-bit)}.
 */
#define BDK_GITS_CMD_ERR_E_CSEI_CMD_TO (0xe0)
#define BDK_GITS_CMD_ERR_E_CSEI_COLLECTION_OOR (3)
#define BDK_GITS_CMD_ERR_E_CSEI_DEVICE_OOR (1)
#define BDK_GITS_CMD_ERR_E_CSEI_ID_OOR (5)
#define BDK_GITS_CMD_ERR_E_CSEI_ITE_INVALID (0x10)
#define BDK_GITS_CMD_ERR_E_CSEI_ITTSIZE_OOR (2)
#define BDK_GITS_CMD_ERR_E_CSEI_PHYSICALID_OOR (6)
#define BDK_GITS_CMD_ERR_E_CSEI_SYNCACK_INVALID (0xe1)
#define BDK_GITS_CMD_ERR_E_CSEI_TA_INVALID (0xfe)
#define BDK_GITS_CMD_ERR_E_CSEI_UNMAPPED_COLLECTION (9)
#define BDK_GITS_CMD_ERR_E_CSEI_UNMAPPED_DEVICE (4)
#define BDK_GITS_CMD_ERR_E_CSEI_UNMAPPED_INTERRUPT (7)
#define BDK_GITS_CMD_ERR_E_CSEI_UNSUPPORTED_CMD (0xff)

/**
 * Enumeration gits_cmd_type_e
 *
 * GIC ITS Command Type Enumeration
 * Enumerates the ITS commands.
 */
#define BDK_GITS_CMD_TYPE_E_CMD_CLEAR (4)
#define BDK_GITS_CMD_TYPE_E_CMD_DISCARD (0xf)
#define BDK_GITS_CMD_TYPE_E_CMD_INT (3)
#define BDK_GITS_CMD_TYPE_E_CMD_INV (0xc)
#define BDK_GITS_CMD_TYPE_E_CMD_INVALL (0xd)
#define BDK_GITS_CMD_TYPE_E_CMD_MAPC (9)
#define BDK_GITS_CMD_TYPE_E_CMD_MAPD (8)
#define BDK_GITS_CMD_TYPE_E_CMD_MAPI (0xb)
#define BDK_GITS_CMD_TYPE_E_CMD_MAPVI (0xa)
#define BDK_GITS_CMD_TYPE_E_CMD_MOVALL (0xe)
#define BDK_GITS_CMD_TYPE_E_CMD_MOVI (1)
#define BDK_GITS_CMD_TYPE_E_CMD_SYNC (5)
#define BDK_GITS_CMD_TYPE_E_CMD_UDF (0)

/**
 * Structure gits_cmd_clear_s
 *
 * GIC ITS Clear Command Structure
 */
union bdk_gits_cmd_clear_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_clear_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_CLEAR. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_CLEAR. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_96_127       : 32;
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID to be translated. */
#else /* Word 1 - Little Endian */
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID to be translated. */
        uint64_t reserved_96_127       : 32;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_128_191      : 64;
#else /* Word 2 - Little Endian */
        uint64_t reserved_128_191      : 64;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_clear_s_s cn; */
};

/**
 * Structure gits_cmd_discard_s
 *
 * GIC ITS Discard Command Structure
 */
union bdk_gits_cmd_discard_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_discard_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_DISCARD. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_DISCARD. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_96_127       : 32;
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID. */
#else /* Word 1 - Little Endian */
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID. */
        uint64_t reserved_96_127       : 32;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_128_191      : 64;
#else /* Word 2 - Little Endian */
        uint64_t reserved_128_191      : 64;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_discard_s_s cn; */
};

/**
 * Structure gits_cmd_int_s
 *
 * GIC ITS INT Command Structure
 */
union bdk_gits_cmd_int_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_int_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_INT. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_INT. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_96_127       : 32;
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID to be translated. */
#else /* Word 1 - Little Endian */
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID to be translated. */
        uint64_t reserved_96_127       : 32;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_128_191      : 64;
#else /* Word 2 - Little Endian */
        uint64_t reserved_128_191      : 64;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_int_s_s cn; */
};

/**
 * Structure gits_cmd_inv_s
 *
 * GIC ITS INV Command Structure
 */
union bdk_gits_cmd_inv_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_inv_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_INV. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_INV. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_96_127       : 32;
        uint64_t int_id                : 32; /**< [ 95: 64] Reserved. */
#else /* Word 1 - Little Endian */
        uint64_t int_id                : 32; /**< [ 95: 64] Reserved. */
        uint64_t reserved_96_127       : 32;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_128_191      : 64;
#else /* Word 2 - Little Endian */
        uint64_t reserved_128_191      : 64;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_inv_s_s cn; */
};

/**
 * Structure gits_cmd_invall_s
 *
 * GIC ITS INVALL Command Structure
 */
union bdk_gits_cmd_invall_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_invall_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_INVALL. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_INVALL. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_64_127       : 64;
#else /* Word 1 - Little Endian */
        uint64_t reserved_64_127       : 64;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_144_191      : 48;
        uint64_t cid                   : 16; /**< [143:128] Interrupt collection ID. */
#else /* Word 2 - Little Endian */
        uint64_t cid                   : 16; /**< [143:128] Interrupt collection ID. */
        uint64_t reserved_144_191      : 48;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_invall_s_s cn; */
};

/**
 * Structure gits_cmd_mapc_s
 *
 * GIC ITS MAPC Command Structure
 */
union bdk_gits_cmd_mapc_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_mapc_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPC. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPC. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_64_127       : 64;
#else /* Word 1 - Little Endian */
        uint64_t reserved_64_127       : 64;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t v                     : 1;  /**< [191:191] Valid bit. Specifies whether the ITT address and size are valid. When [V] is
                                                                 zero, this command unmaps the specified device and translation request from
                                                                 that device will be discarded. */
        uint64_t reserved_176_190      : 15;
        uint64_t ta                    : 32; /**< [175:144] Target address. Specifies the physical address of the redistributor to which
                                                                 interrupts for the collection will be forwarded. */
        uint64_t cid                   : 16; /**< [143:128] Interrupt collection ID. */
#else /* Word 2 - Little Endian */
        uint64_t cid                   : 16; /**< [143:128] Interrupt collection ID. */
        uint64_t ta                    : 32; /**< [175:144] Target address. Specifies the physical address of the redistributor to which
                                                                 interrupts for the collection will be forwarded. */
        uint64_t reserved_176_190      : 15;
        uint64_t v                     : 1;  /**< [191:191] Valid bit. Specifies whether the ITT address and size are valid. When [V] is
                                                                 zero, this command unmaps the specified device and translation request from
                                                                 that device will be discarded. */
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_mapc_s_s cn; */
};

/**
 * Structure gits_cmd_mapd_s
 *
 * GIC ITS MAPD Command Structure
 */
union bdk_gits_cmd_mapd_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_mapd_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPD. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPD. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_69_127       : 59;
        uint64_t size                  : 5;  /**< [ 68: 64] Number of bits of interrupt ID supported for this device, minus one. */
#else /* Word 1 - Little Endian */
        uint64_t size                  : 5;  /**< [ 68: 64] Number of bits of interrupt ID supported for this device, minus one. */
        uint64_t reserved_69_127       : 59;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t v                     : 1;  /**< [191:191] Valid bit. Specifies whether the ITT address and size are valid. When [V] is zero,
                                                                 this command unmaps the specified device and translation request from that
                                                                 device will be discarded. */
        uint64_t reserved_176_190      : 15;
        uint64_t itta                  : 40; /**< [175:136] ITT address. Specifies bits \<47:8\> of the physical address of the interrupt
                                                                 translation table. Bits \<7:0\> of the physical address are zero. */
        uint64_t reserved_128_135      : 8;
#else /* Word 2 - Little Endian */
        uint64_t reserved_128_135      : 8;
        uint64_t itta                  : 40; /**< [175:136] ITT address. Specifies bits \<47:8\> of the physical address of the interrupt
                                                                 translation table. Bits \<7:0\> of the physical address are zero. */
        uint64_t reserved_176_190      : 15;
        uint64_t v                     : 1;  /**< [191:191] Valid bit. Specifies whether the ITT address and size are valid. When [V] is zero,
                                                                 this command unmaps the specified device and translation request from that
                                                                 device will be discarded. */
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_mapd_s_s cn; */
};

/**
 * Structure gits_cmd_mapi_s
 *
 * GIC ITS MAPI Command Structure
 */
union bdk_gits_cmd_mapi_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_mapi_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPI. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPI. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_96_127       : 32;
        uint64_t int_id                : 32; /**< [ 95: 64] Reserved. */
#else /* Word 1 - Little Endian */
        uint64_t int_id                : 32; /**< [ 95: 64] Reserved. */
        uint64_t reserved_96_127       : 32;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_144_191      : 48;
        uint64_t cid                   : 16; /**< [143:128] Collection. Specifies the interrupt collection of which the interrupt with identifier
                                                                 physical ID is a member. */
#else /* Word 2 - Little Endian */
        uint64_t cid                   : 16; /**< [143:128] Collection. Specifies the interrupt collection of which the interrupt with identifier
                                                                 physical ID is a member. */
        uint64_t reserved_144_191      : 48;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_mapi_s_s cn; */
};

/**
 * Structure gits_cmd_mapvi_s
 *
 * GIC ITS MAPVI Command Structure
 */
union bdk_gits_cmd_mapvi_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_mapvi_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPVI. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MAPVI. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t phy_id                : 32; /**< [127: 96] Reserved. */
        uint64_t int_id                : 32; /**< [ 95: 64] Reserved. */
#else /* Word 1 - Little Endian */
        uint64_t int_id                : 32; /**< [ 95: 64] Reserved. */
        uint64_t phy_id                : 32; /**< [127: 96] Reserved. */
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_144_191      : 48;
        uint64_t cid                   : 16; /**< [143:128] Collection. Specifies the interrupt collection of which the interrupt with identifier
                                                                 physical ID is a member. */
#else /* Word 2 - Little Endian */
        uint64_t cid                   : 16; /**< [143:128] Collection. Specifies the interrupt collection of which the interrupt with identifier
                                                                 physical ID is a member. */
        uint64_t reserved_144_191      : 48;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_mapvi_s_s cn; */
};

/**
 * Structure gits_cmd_movall_s
 *
 * GIC ITS MOVALL Command Structure
 */
union bdk_gits_cmd_movall_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_movall_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MOVALL. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MOVALL. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_64_127       : 64;
#else /* Word 1 - Little Endian */
        uint64_t reserved_64_127       : 64;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_176_191      : 16;
        uint64_t ta1                   : 32; /**< [175:144] Target address 1. Specifies the old redistributor. */
        uint64_t reserved_128_143      : 16;
#else /* Word 2 - Little Endian */
        uint64_t reserved_128_143      : 16;
        uint64_t ta1                   : 32; /**< [175:144] Target address 1. Specifies the old redistributor. */
        uint64_t reserved_176_191      : 16;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_240_255      : 16;
        uint64_t ta2                   : 32; /**< [239:208] Target address 2. Specifies the new redistributor. */
        uint64_t reserved_192_207      : 16;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_207      : 16;
        uint64_t ta2                   : 32; /**< [239:208] Target address 2. Specifies the new redistributor. */
        uint64_t reserved_240_255      : 16;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_movall_s_s cn; */
};

/**
 * Structure gits_cmd_movi_s
 *
 * GIC ITS MOVI Command Structure
 */
union bdk_gits_cmd_movi_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_movi_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
        uint64_t reserved_8_31         : 24;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MOVI. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_MOVI. */
        uint64_t reserved_8_31         : 24;
        uint64_t dev_id                : 32; /**< [ 63: 32] Interrupt device ID. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_96_127       : 32;
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID to be translated. */
#else /* Word 1 - Little Endian */
        uint64_t int_id                : 32; /**< [ 95: 64] Interrupt ID to be translated. */
        uint64_t reserved_96_127       : 32;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_144_191      : 48;
        uint64_t cid                   : 16; /**< [143:128] Interrupt collection ID. */
#else /* Word 2 - Little Endian */
        uint64_t cid                   : 16; /**< [143:128] Interrupt collection ID. */
        uint64_t reserved_144_191      : 48;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_movi_s_s cn; */
};

/**
 * Structure gits_cmd_sync_s
 *
 * GIC ITS SYNC Command Structure
 */
union bdk_gits_cmd_sync_s
{
    uint64_t u[4];
    struct bdk_gits_cmd_sync_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_SYNC. */
#else /* Word 0 - Little Endian */
        uint64_t cmd_type              : 8;  /**< [  7:  0] Command type. Indicates GITS_CMD_TYPE_E::CMD_SYNC. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_64_127       : 64;
#else /* Word 1 - Little Endian */
        uint64_t reserved_64_127       : 64;
#endif /* Word 1 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 2 - Big Endian */
        uint64_t reserved_176_191      : 16;
        uint64_t ta                    : 32; /**< [175:144] Target address of the redistributor 0. */
        uint64_t reserved_128_143      : 16;
#else /* Word 2 - Little Endian */
        uint64_t reserved_128_143      : 16;
        uint64_t ta                    : 32; /**< [175:144] Target address of the redistributor 0. */
        uint64_t reserved_176_191      : 16;
#endif /* Word 2 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 3 - Big Endian */
        uint64_t reserved_192_255      : 64;
#else /* Word 3 - Little Endian */
        uint64_t reserved_192_255      : 64;
#endif /* Word 3 - End */
    } s;
    /* struct bdk_gits_cmd_sync_s_s cn; */
};

/**
 * Register (NCB) gic_bist_statusr
 *
 * GIC Implementation BIST Status Register
 * This register contains the BIST status for the GIC memories (including ITS and RDB).
 */
union bdk_gic_bist_statusr
{
    uint64_t u;
    struct bdk_gic_bist_statusr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t bist                  : 9;  /**< [  8:  0](RO/H) Memory BIST status:
                                                                   0 = Pass.
                                                                   1 = Fail.

                                                                 Internal:
                                                                 [8:0]= [cic2cic_ig_buf, lpi_cfg_buf, lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem] in GIC. */
#else /* Word 0 - Little Endian */
        uint64_t bist                  : 9;  /**< [  8:  0](RO/H) Memory BIST status:
                                                                   0 = Pass.
                                                                   1 = Fail.

                                                                 Internal:
                                                                 [8:0]= [cic2cic_ig_buf, lpi_cfg_buf, lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem] in GIC. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_bist_statusr_s cn; */
};
typedef union bdk_gic_bist_statusr bdk_gic_bist_statusr_t;

#define BDK_GIC_BIST_STATUSR BDK_GIC_BIST_STATUSR_FUNC()
static inline uint64_t BDK_GIC_BIST_STATUSR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_BIST_STATUSR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x801000010020ll;
    __bdk_csr_fatal("GIC_BIST_STATUSR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_BIST_STATUSR bdk_gic_bist_statusr_t
#define bustype_BDK_GIC_BIST_STATUSR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_BIST_STATUSR "GIC_BIST_STATUSR"
#define device_bar_BDK_GIC_BIST_STATUSR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_BIST_STATUSR 0
#define arguments_BDK_GIC_BIST_STATUSR -1,-1,-1,-1

/**
 * Register (NCB) gic_bp_test0
 *
 * INTERNAL: GIC Backpressure Test Register
 */
union bdk_gic_bp_test0
{
    uint64_t u;
    struct bdk_gic_bp_test0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t enable                : 4;  /**< [ 63: 60](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.
                                                                 \<63\> = Limit RDB NCBI transactions. Never limit 100% of the time.
                                                                 \<62\> = Limit ITS NCBI transactions. Never limit 100% of the time.
                                                                 \<61\> = Limit RDB interrupt message handling via NCBO. Never limit 100% of the time.
                                                                 \<60\> = Limit ITS interrupt message handling via NCBO. Never limit 100% of the time. */
        uint64_t reserved_24_59        : 36;
        uint64_t bp_cfg                : 8;  /**< [ 23: 16](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<23:22\> = Config 3.
                                                                   \<21:20\> = Config 2.
                                                                   \<19:18\> = Config 1.
                                                                   \<17:16\> = Config 0. */
        uint64_t reserved_12_15        : 4;
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
#else /* Word 0 - Little Endian */
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
        uint64_t reserved_12_15        : 4;
        uint64_t bp_cfg                : 8;  /**< [ 23: 16](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<23:22\> = Config 3.
                                                                   \<21:20\> = Config 2.
                                                                   \<19:18\> = Config 1.
                                                                   \<17:16\> = Config 0. */
        uint64_t reserved_24_59        : 36;
        uint64_t enable                : 4;  /**< [ 63: 60](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.
                                                                 \<63\> = Limit RDB NCBI transactions. Never limit 100% of the time.
                                                                 \<62\> = Limit ITS NCBI transactions. Never limit 100% of the time.
                                                                 \<61\> = Limit RDB interrupt message handling via NCBO. Never limit 100% of the time.
                                                                 \<60\> = Limit ITS interrupt message handling via NCBO. Never limit 100% of the time. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_bp_test0_s cn; */
};
typedef union bdk_gic_bp_test0 bdk_gic_bp_test0_t;

#define BDK_GIC_BP_TEST0 BDK_GIC_BP_TEST0_FUNC()
static inline uint64_t BDK_GIC_BP_TEST0_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_BP_TEST0_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8010000100a0ll;
    __bdk_csr_fatal("GIC_BP_TEST0", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_BP_TEST0 bdk_gic_bp_test0_t
#define bustype_BDK_GIC_BP_TEST0 BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_BP_TEST0 "GIC_BP_TEST0"
#define device_bar_BDK_GIC_BP_TEST0 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_BP_TEST0 0
#define arguments_BDK_GIC_BP_TEST0 -1,-1,-1,-1

/**
 * Register (NCB) gic_bp_test1
 *
 * INTERNAL: GIC Backpressure Test Register
 */
union bdk_gic_bp_test1
{
    uint64_t u;
    struct bdk_gic_bp_test1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t enable                : 4;  /**< [ 63: 60](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.
                                                                 \<63\> = Reserved.
                                                                 \<62\> = Reserved.
                                                                 \<61\> = Reserved.
                                                                 \<60\> = Reserved. TBD?: Limit messages to AP CIMs. */
        uint64_t reserved_24_59        : 36;
        uint64_t bp_cfg                : 8;  /**< [ 23: 16](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<23:22\> = Reserved.
                                                                   \<21:20\> = Config 2.
                                                                   \<19:18\> = Config 1.
                                                                   \<17:16\> = Config 0. */
        uint64_t reserved_12_15        : 4;
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
#else /* Word 0 - Little Endian */
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
        uint64_t reserved_12_15        : 4;
        uint64_t bp_cfg                : 8;  /**< [ 23: 16](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<23:22\> = Reserved.
                                                                   \<21:20\> = Config 2.
                                                                   \<19:18\> = Config 1.
                                                                   \<17:16\> = Config 0. */
        uint64_t reserved_24_59        : 36;
        uint64_t enable                : 4;  /**< [ 63: 60](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.
                                                                 \<63\> = Reserved.
                                                                 \<62\> = Reserved.
                                                                 \<61\> = Reserved.
                                                                 \<60\> = Reserved. TBD?: Limit messages to AP CIMs. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_bp_test1_s cn; */
};
typedef union bdk_gic_bp_test1 bdk_gic_bp_test1_t;

#define BDK_GIC_BP_TEST1 BDK_GIC_BP_TEST1_FUNC()
static inline uint64_t BDK_GIC_BP_TEST1_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_BP_TEST1_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8010000100b0ll;
    __bdk_csr_fatal("GIC_BP_TEST1", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_BP_TEST1 bdk_gic_bp_test1_t
#define bustype_BDK_GIC_BP_TEST1 BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_BP_TEST1 "GIC_BP_TEST1"
#define device_bar_BDK_GIC_BP_TEST1 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_BP_TEST1 0
#define arguments_BDK_GIC_BP_TEST1 -1,-1,-1,-1

/**
 * Register (NCB) gic_cfg_ctlr
 *
 * GIC Implementation Secure Configuration Control Register
 * This register configures GIC features.
 */
union bdk_gic_cfg_ctlr
{
    uint64_t u;
    struct bdk_gic_cfg_ctlr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_34_63        : 30;
        uint64_t dis_redist_lpi_aggr_merge : 1;/**< [ 33: 33](SR/W) Disable aggressive SETLPIR merging in redistributors. */
        uint64_t dis_cpu_if_load_balancer : 1;/**< [ 32: 32](SR/W) Disable the CPU interface load balancer. */
        uint64_t reserved_10_31        : 22;
        uint64_t dis_lpi_pend_cache    : 1;  /**< [  9:  9](SR/W) Disable the LPI pending table cache. */
        uint64_t dis_lpi_cfg_cache     : 1;  /**< [  8:  8](SR/W) Disable the LPI configuration cache. */
        uint64_t dis_inv_hct           : 1;  /**< [  7:  7](SR/W) Disable HW invalidating ITS HCT during ITS disable process. */
        uint64_t dis_its_cdtc          : 1;  /**< [  6:  6](SR/W) Disable 1-entry device table cache in ITS CEU. */
        uint64_t dis_its_itlb          : 1;  /**< [  5:  5](SR/W) Disable ITS ITLB (interrupt translation entry lookup buffer). */
        uint64_t dis_its_dtlb          : 1;  /**< [  4:  4](SR/W) Disable ITS DTLB (device table entry lookup buffer). */
        uint64_t reserved_3            : 1;
        uint64_t root_dist             : 1;  /**< [  2:  2](SR/W) Specifies whether the distributor on this socket is root.
                                                                 0 = Distributor is not root.
                                                                 1 = Distributor is root.

                                                                 Out of reset, this field is set. EL3 firmware will clear this field as required for multi-
                                                                 socket operation. */
        uint64_t om                    : 2;  /**< [  1:  0](SR/W) Operation mode.
                                                                 0x0 = Single-socket single-root mode.
                                                                 0x1 = Reserved.
                                                                 0x2 = Multisocket single-root mode.
                                                                 0x3 = Multisocket multiroot mode. */
#else /* Word 0 - Little Endian */
        uint64_t om                    : 2;  /**< [  1:  0](SR/W) Operation mode.
                                                                 0x0 = Single-socket single-root mode.
                                                                 0x1 = Reserved.
                                                                 0x2 = Multisocket single-root mode.
                                                                 0x3 = Multisocket multiroot mode. */
        uint64_t root_dist             : 1;  /**< [  2:  2](SR/W) Specifies whether the distributor on this socket is root.
                                                                 0 = Distributor is not root.
                                                                 1 = Distributor is root.

                                                                 Out of reset, this field is set. EL3 firmware will clear this field as required for multi-
                                                                 socket operation. */
        uint64_t reserved_3            : 1;
        uint64_t dis_its_dtlb          : 1;  /**< [  4:  4](SR/W) Disable ITS DTLB (device table entry lookup buffer). */
        uint64_t dis_its_itlb          : 1;  /**< [  5:  5](SR/W) Disable ITS ITLB (interrupt translation entry lookup buffer). */
        uint64_t dis_its_cdtc          : 1;  /**< [  6:  6](SR/W) Disable 1-entry device table cache in ITS CEU. */
        uint64_t dis_inv_hct           : 1;  /**< [  7:  7](SR/W) Disable HW invalidating ITS HCT during ITS disable process. */
        uint64_t dis_lpi_cfg_cache     : 1;  /**< [  8:  8](SR/W) Disable the LPI configuration cache. */
        uint64_t dis_lpi_pend_cache    : 1;  /**< [  9:  9](SR/W) Disable the LPI pending table cache. */
        uint64_t reserved_10_31        : 22;
        uint64_t dis_cpu_if_load_balancer : 1;/**< [ 32: 32](SR/W) Disable the CPU interface load balancer. */
        uint64_t dis_redist_lpi_aggr_merge : 1;/**< [ 33: 33](SR/W) Disable aggressive SETLPIR merging in redistributors. */
        uint64_t reserved_34_63        : 30;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_cfg_ctlr_s cn9; */
    /* struct bdk_gic_cfg_ctlr_s cn81xx; */
    struct bdk_gic_cfg_ctlr_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t dis_cpu_if_load_balancer : 1;/**< [ 32: 32](SR/W) Disable the CPU interface load balancer. */
        uint64_t reserved_10_31        : 22;
        uint64_t dis_lpi_pend_cache    : 1;  /**< [  9:  9](SR/W) Disable the LPI pending table cache. */
        uint64_t dis_lpi_cfg_cache     : 1;  /**< [  8:  8](SR/W) Disable the LPI configuration cache. */
        uint64_t dis_inv_hct           : 1;  /**< [  7:  7](SR/W) Disable HW invalidating ITS HCT during ITS disable process. */
        uint64_t dis_its_cdtc          : 1;  /**< [  6:  6](SR/W) Disable 1-entry device table cache in ITS CEU. */
        uint64_t dis_its_itlb          : 1;  /**< [  5:  5](SR/W) Disable ITS ITLB (interrupt translation entry lookup buffer). */
        uint64_t dis_its_dtlb          : 1;  /**< [  4:  4](SR/W) Disable ITS DTLB (device table entry lookup buffer). */
        uint64_t reserved_3            : 1;
        uint64_t root_dist             : 1;  /**< [  2:  2](SR/W) Specifies whether the distributor on this socket is root.
                                                                 0 = Distributor is not root.
                                                                 1 = Distributor is root.

                                                                 Out of reset, this field is set. EL3 firmware will clear this field as required for multi-
                                                                 socket operation. */
        uint64_t om                    : 2;  /**< [  1:  0](SR/W) Operation mode.
                                                                 0x0 = Single-socket single-root mode.
                                                                 0x1 = Reserved.
                                                                 0x2 = Multisocket single-root mode.
                                                                 0x3 = Multisocket multiroot mode. */
#else /* Word 0 - Little Endian */
        uint64_t om                    : 2;  /**< [  1:  0](SR/W) Operation mode.
                                                                 0x0 = Single-socket single-root mode.
                                                                 0x1 = Reserved.
                                                                 0x2 = Multisocket single-root mode.
                                                                 0x3 = Multisocket multiroot mode. */
        uint64_t root_dist             : 1;  /**< [  2:  2](SR/W) Specifies whether the distributor on this socket is root.
                                                                 0 = Distributor is not root.
                                                                 1 = Distributor is root.

                                                                 Out of reset, this field is set. EL3 firmware will clear this field as required for multi-
                                                                 socket operation. */
        uint64_t reserved_3            : 1;
        uint64_t dis_its_dtlb          : 1;  /**< [  4:  4](SR/W) Disable ITS DTLB (device table entry lookup buffer). */
        uint64_t dis_its_itlb          : 1;  /**< [  5:  5](SR/W) Disable ITS ITLB (interrupt translation entry lookup buffer). */
        uint64_t dis_its_cdtc          : 1;  /**< [  6:  6](SR/W) Disable 1-entry device table cache in ITS CEU. */
        uint64_t dis_inv_hct           : 1;  /**< [  7:  7](SR/W) Disable HW invalidating ITS HCT during ITS disable process. */
        uint64_t dis_lpi_cfg_cache     : 1;  /**< [  8:  8](SR/W) Disable the LPI configuration cache. */
        uint64_t dis_lpi_pend_cache    : 1;  /**< [  9:  9](SR/W) Disable the LPI pending table cache. */
        uint64_t reserved_10_31        : 22;
        uint64_t dis_cpu_if_load_balancer : 1;/**< [ 32: 32](SR/W) Disable the CPU interface load balancer. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_gic_cfg_ctlr_s cn83xx; */
};
typedef union bdk_gic_cfg_ctlr bdk_gic_cfg_ctlr_t;

#define BDK_GIC_CFG_CTLR BDK_GIC_CFG_CTLR_FUNC()
static inline uint64_t BDK_GIC_CFG_CTLR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_CFG_CTLR_FUNC(void)
{
    return 0x801000010000ll;
}

#define typedef_BDK_GIC_CFG_CTLR bdk_gic_cfg_ctlr_t
#define bustype_BDK_GIC_CFG_CTLR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_CFG_CTLR "GIC_CFG_CTLR"
#define device_bar_BDK_GIC_CFG_CTLR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_CFG_CTLR 0
#define arguments_BDK_GIC_CFG_CTLR -1,-1,-1,-1

/**
 * Register (NCB) gic_const
 *
 * GIC Constants Register
 * This register contains constant for software discovery.
 */
union bdk_gic_const
{
    uint64_t u;
    struct bdk_gic_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_const_s cn; */
};
typedef union bdk_gic_const bdk_gic_const_t;

#define BDK_GIC_CONST BDK_GIC_CONST_FUNC()
static inline uint64_t BDK_GIC_CONST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_CONST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x801000010088ll;
    __bdk_csr_fatal("GIC_CONST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_CONST bdk_gic_const_t
#define bustype_BDK_GIC_CONST BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_CONST "GIC_CONST"
#define device_bar_BDK_GIC_CONST 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_CONST 0
#define arguments_BDK_GIC_CONST -1,-1,-1,-1

/**
 * Register (NCB) gic_csclk_active_pc
 *
 * GIC Conditional Sclk Clock Counter Register
 * This register counts conditional clocks for power management.
 */
union bdk_gic_csclk_active_pc
{
    uint64_t u;
    struct bdk_gic_csclk_active_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Count of conditional coprocessor-clock cycles since reset. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Count of conditional coprocessor-clock cycles since reset. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_csclk_active_pc_s cn; */
};
typedef union bdk_gic_csclk_active_pc bdk_gic_csclk_active_pc_t;

#define BDK_GIC_CSCLK_ACTIVE_PC BDK_GIC_CSCLK_ACTIVE_PC_FUNC()
static inline uint64_t BDK_GIC_CSCLK_ACTIVE_PC_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_CSCLK_ACTIVE_PC_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x801000010090ll;
    __bdk_csr_fatal("GIC_CSCLK_ACTIVE_PC", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_CSCLK_ACTIVE_PC bdk_gic_csclk_active_pc_t
#define bustype_BDK_GIC_CSCLK_ACTIVE_PC BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_CSCLK_ACTIVE_PC "GIC_CSCLK_ACTIVE_PC"
#define device_bar_BDK_GIC_CSCLK_ACTIVE_PC 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_CSCLK_ACTIVE_PC 0
#define arguments_BDK_GIC_CSCLK_ACTIVE_PC -1,-1,-1,-1

/**
 * Register (NCB) gic_del3t_ctlr
 *
 * GIC Debug EL3 Trap Secure Control Register
 * This register allows disabling the signaling of some DEL3T errors.
 */
union bdk_gic_del3t_ctlr
{
    uint64_t u;
    struct bdk_gic_del3t_ctlr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_38_63        : 26;
        uint64_t del3t_core_id         : 6;  /**< [ 37: 32](SR/W) Target CoreID for signaling of GIC DEL3T Errors. Legal range is [0,47]. */
        uint64_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_31         : 32;
        uint64_t del3t_core_id         : 6;  /**< [ 37: 32](SR/W) Target CoreID for signaling of GIC DEL3T Errors. Legal range is [0,47]. */
        uint64_t reserved_38_63        : 26;
#endif /* Word 0 - End */
    } s;
    struct bdk_gic_del3t_ctlr_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_38_63        : 26;
        uint64_t del3t_core_id         : 6;  /**< [ 37: 32](SR/W) Target CoreID for signaling of GIC DEL3T Errors. Legal range is [0,47]. */
        uint64_t reserved_11_31        : 21;
        uint64_t del3t_dis             : 11; /**< [ 10:  0](SR/W) Disable signaling of DEL3T Errors.
                                                                 Internal:
                                                                 for del3t_dis[10:0]=
                                                                 [ncbr_stdn,ncbr_fill,cic2cic_ig_buf, lpi_cfg_buf,
                                                                 lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem]  in GIC. */
#else /* Word 0 - Little Endian */
        uint64_t del3t_dis             : 11; /**< [ 10:  0](SR/W) Disable signaling of DEL3T Errors.
                                                                 Internal:
                                                                 for del3t_dis[10:0]=
                                                                 [ncbr_stdn,ncbr_fill,cic2cic_ig_buf, lpi_cfg_buf,
                                                                 lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem]  in GIC. */
        uint64_t reserved_11_31        : 21;
        uint64_t del3t_core_id         : 6;  /**< [ 37: 32](SR/W) Target CoreID for signaling of GIC DEL3T Errors. Legal range is [0,47]. */
        uint64_t reserved_38_63        : 26;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_gic_del3t_ctlr_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_38_63        : 26;
        uint64_t del3t_core_id         : 6;  /**< [ 37: 32](SR/W) Target CoreID for signaling of GIC DEL3T Errors. Legal range is [0,23]. */
        uint64_t reserved_11_31        : 21;
        uint64_t del3t_dis             : 2;  /**< [ 10:  9](SR/W) Disable signaling of DEL3T Errors.
                                                                 Internal:
                                                                 for del3t_dis[10:9]=
                                                                 [ncbr_stdn,ncbr_fill] in GIC. */
        uint64_t reserved_0_8          : 9;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_8          : 9;
        uint64_t del3t_dis             : 2;  /**< [ 10:  9](SR/W) Disable signaling of DEL3T Errors.
                                                                 Internal:
                                                                 for del3t_dis[10:9]=
                                                                 [ncbr_stdn,ncbr_fill] in GIC. */
        uint64_t reserved_11_31        : 21;
        uint64_t del3t_core_id         : 6;  /**< [ 37: 32](SR/W) Target CoreID for signaling of GIC DEL3T Errors. Legal range is [0,23]. */
        uint64_t reserved_38_63        : 26;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_gic_del3t_ctlr bdk_gic_del3t_ctlr_t;

#define BDK_GIC_DEL3T_CTLR BDK_GIC_DEL3T_CTLR_FUNC()
static inline uint64_t BDK_GIC_DEL3T_CTLR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_DEL3T_CTLR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x801000010060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x801000010060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X))
        return 0x801000010060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x801000010060ll;
    __bdk_csr_fatal("GIC_DEL3T_CTLR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_DEL3T_CTLR bdk_gic_del3t_ctlr_t
#define bustype_BDK_GIC_DEL3T_CTLR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_DEL3T_CTLR "GIC_DEL3T_CTLR"
#define device_bar_BDK_GIC_DEL3T_CTLR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_DEL3T_CTLR 0
#define arguments_BDK_GIC_DEL3T_CTLR -1,-1,-1,-1

/**
 * Register (NCB) gic_ecc_ctlr
 *
 * INTERNAL: GIC Implementation Secure ECC Control Register
 *
 * This register is reserved for backwards compatibility.
 */
union bdk_gic_ecc_ctlr
{
    uint64_t u;
    struct bdk_gic_ecc_ctlr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t ram_flip1             : 9;  /**< [ 48: 40](SR/W) Flip syndrome bits on write. Flip syndrome bits \<1\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_29_39        : 11;
        uint64_t ram_flip0             : 9;  /**< [ 28: 20](SR/W) Flip syndrome bits on write. Flip syndrome bits \<0\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_9_19         : 11;
        uint64_t cor_dis               : 9;  /**< [  8:  0](SR/W) RAM ECC correction disable.
                                                                 Internal:
                                                                 for cor_dis[8:0]= [cic2cic_ig_buf, lpi_cfg_buf,
                                                                 lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem]  in GIC. */
#else /* Word 0 - Little Endian */
        uint64_t cor_dis               : 9;  /**< [  8:  0](SR/W) RAM ECC correction disable.
                                                                 Internal:
                                                                 for cor_dis[8:0]= [cic2cic_ig_buf, lpi_cfg_buf,
                                                                 lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem]  in GIC. */
        uint64_t reserved_9_19         : 11;
        uint64_t ram_flip0             : 9;  /**< [ 28: 20](SR/W) Flip syndrome bits on write. Flip syndrome bits \<0\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_29_39        : 11;
        uint64_t ram_flip1             : 9;  /**< [ 48: 40](SR/W) Flip syndrome bits on write. Flip syndrome bits \<1\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } s;
    struct bdk_gic_ecc_ctlr_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t reserved_49_59        : 11;
        uint64_t ram_flip1             : 9;  /**< [ 48: 40](SR/W) Flip syndrome bits on write. Flip syndrome bits \<1\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_29_39        : 11;
        uint64_t ram_flip0             : 9;  /**< [ 28: 20](SR/W) Flip syndrome bits on write. Flip syndrome bits \<0\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_9_19         : 11;
        uint64_t cor_dis               : 9;  /**< [  8:  0](SR/W) RAM ECC correction disable.
                                                                 Internal:
                                                                 for cor_dis[8:0]= [cic2cic_ig_buf, lpi_cfg_buf,
                                                                 lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem]  in GIC. */
#else /* Word 0 - Little Endian */
        uint64_t cor_dis               : 9;  /**< [  8:  0](SR/W) RAM ECC correction disable.
                                                                 Internal:
                                                                 for cor_dis[8:0]= [cic2cic_ig_buf, lpi_cfg_buf,
                                                                 lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem]  in GIC. */
        uint64_t reserved_9_19         : 11;
        uint64_t ram_flip0             : 9;  /**< [ 28: 20](SR/W) Flip syndrome bits on write. Flip syndrome bits \<0\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_29_39        : 11;
        uint64_t ram_flip1             : 9;  /**< [ 48: 40](SR/W) Flip syndrome bits on write. Flip syndrome bits \<1\> on writes to the corresponding ram to
                                                                 test single-bit or double-bit error handling. See COR_DIS bit definitions. */
        uint64_t reserved_49_59        : 11;
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_gic_ecc_ctlr_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_gic_ecc_ctlr bdk_gic_ecc_ctlr_t;

#define BDK_GIC_ECC_CTLR BDK_GIC_ECC_CTLR_FUNC()
static inline uint64_t BDK_GIC_ECC_CTLR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_ECC_CTLR_FUNC(void)
{
    return 0x801000010008ll;
}

#define typedef_BDK_GIC_ECC_CTLR bdk_gic_ecc_ctlr_t
#define bustype_BDK_GIC_ECC_CTLR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_ECC_CTLR "GIC_ECC_CTLR"
#define device_bar_BDK_GIC_ECC_CTLR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_ECC_CTLR 0
#define arguments_BDK_GIC_ECC_CTLR -1,-1,-1,-1

/**
 * Register (NCB) gic_ecc_int_statusr
 *
 * GIC Implementation ECC Error Interrupt Status Register
 * This register contains the ECC error status for the GIC memories (including ITS and RDB).
 */
union bdk_gic_ecc_int_statusr
{
    uint64_t u;
    struct bdk_gic_ecc_int_statusr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_41_63        : 23;
        uint64_t dbe                   : 9;  /**< [ 40: 32](R/W1C/H) RAM ECC DBE detected.
                                                                 Internal:
                                                                 [8:0] = [cic2cic_ig_buf, lpi_cfg_buf, lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem] in GIC. */
        uint64_t reserved_9_31         : 23;
        uint64_t sbe                   : 9;  /**< [  8:  0](R/W1C/H) RAM ECC SBE detected.
                                                                 Internal:
                                                                 [8:0] = [cic2cic_ig_buf, lpi_cfg_buf, lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem] in GIC. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 9;  /**< [  8:  0](R/W1C/H) RAM ECC SBE detected.
                                                                 Internal:
                                                                 [8:0] = [cic2cic_ig_buf, lpi_cfg_buf, lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem] in GIC. */
        uint64_t reserved_9_31         : 23;
        uint64_t dbe                   : 9;  /**< [ 40: 32](R/W1C/H) RAM ECC DBE detected.
                                                                 Internal:
                                                                 [8:0] = [cic2cic_ig_buf, lpi_cfg_buf, lip_rmw_buf,
                                                                 dtlb_mem,itlb_mem,hct_mem,cqf_mem,rdb_pktf_mem,aprf_mem] in GIC. */
        uint64_t reserved_41_63        : 23;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_ecc_int_statusr_s cn; */
};
typedef union bdk_gic_ecc_int_statusr bdk_gic_ecc_int_statusr_t;

#define BDK_GIC_ECC_INT_STATUSR BDK_GIC_ECC_INT_STATUSR_FUNC()
static inline uint64_t BDK_GIC_ECC_INT_STATUSR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_ECC_INT_STATUSR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x801000010030ll;
    __bdk_csr_fatal("GIC_ECC_INT_STATUSR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_ECC_INT_STATUSR bdk_gic_ecc_int_statusr_t
#define bustype_BDK_GIC_ECC_INT_STATUSR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_ECC_INT_STATUSR "GIC_ECC_INT_STATUSR"
#define device_bar_BDK_GIC_ECC_INT_STATUSR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_ECC_INT_STATUSR 0
#define arguments_BDK_GIC_ECC_INT_STATUSR -1,-1,-1,-1

/**
 * Register (NCB) gic_int_req#_pc
 *
 * GIC Performance Counter Register
 * Index enumerated by GIC_INT_REQ_E.
 */
union bdk_gic_int_reqx_pc
{
    uint64_t u;
    struct bdk_gic_int_reqx_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Performance count for each register. Increments each time the corresponding register is written. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Performance count for each register. Increments each time the corresponding register is written. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_int_reqx_pc_s cn; */
};
typedef union bdk_gic_int_reqx_pc bdk_gic_int_reqx_pc_t;

static inline uint64_t BDK_GIC_INT_REQX_PC(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_INT_REQX_PC(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=6))
        return 0x801000010100ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("GIC_INT_REQX_PC", 1, a, 0, 0, 0);
}

#define typedef_BDK_GIC_INT_REQX_PC(a) bdk_gic_int_reqx_pc_t
#define bustype_BDK_GIC_INT_REQX_PC(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_INT_REQX_PC(a) "GIC_INT_REQX_PC"
#define device_bar_BDK_GIC_INT_REQX_PC(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_INT_REQX_PC(a) (a)
#define arguments_BDK_GIC_INT_REQX_PC(a) (a),-1,-1,-1

/**
 * Register (NCB) gic_rdb_its_if_err_statusr
 *
 * GIC Redistributor Network ITS Interface Error Status Register
 * This register holds the status of errors detected on the redistributor network interface to ITS.
 */
union bdk_gic_rdb_its_if_err_statusr
{
    uint64_t u;
    struct bdk_gic_rdb_its_if_err_statusr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_62_63        : 2;
        uint64_t v                     : 1;  /**< [ 61: 61](R/W1C/H) When set, the command error is valid. */
        uint64_t m                     : 1;  /**< [ 60: 60](RO/H) When set, it means multiple errors have happened. It is meaningful only when [V]=1. */
        uint64_t reserved_59           : 1;
        uint64_t cmd                   : 3;  /**< [ 58: 56](RO/H) ITS Command. Relevant only when [V]=1. Command encodings are
                                                                  SETLPIR = 0x1,
                                                                  CLRLPIR = 0x2,
                                                                  INVLPIR = 0x3,
                                                                  INVALLR = 0x4,
                                                                  SYNCR = 0x5,
                                                                  MOVLPIR = 0x6, and
                                                                  MOVALLR = 0x7. */
        uint64_t reserved_52_55        : 4;
        uint64_t err_multi_socket      : 1;  /**< [ 51: 51](RO/H) Invalid multi-socket message. Relevant only when [V]=1. Indicates incompatibility between
                                                                 operation mode setting (GIC_CFG_CTLR[OM]) and the ITS message. */
        uint64_t err_dest_gicr_id      : 1;  /**< [ 50: 50](RO/H) Invalid destination GICR (redistributor). Relevant only when [V]=1 and [CMD]=MOVLPIR or MOVALLR. */
        uint64_t err_src_gicr_id       : 1;  /**< [ 49: 49](RO/H) Invalid source GICR (Redistributor). Relevant only when [V]=1. */
        uint64_t err_int_id_range      : 1;  /**< [ 48: 48](RO/H) LPI interrupt ID out of range. Relevant only when [V]=1. */
        uint64_t reserved_44_47        : 4;
        uint64_t dst_id                : 8;  /**< [ 43: 36](RO/H) DestID, specified as node_id[1:0], gicr_id[5:0]. Relevant only when [V]=1 and
                                                                 [CMD]=MOVLPIR or MOVALLR. */
        uint64_t src_id                : 8;  /**< [ 35: 28](RO/H) SourceID, specified as node_id[1:0], gicr_id[5:0]. It is meaningful only when [V]=1. */
        uint64_t reserved_20_27        : 8;
        uint64_t int_id                : 20; /**< [ 19:  0](RO/H) Interrrupt ID in the ITS message (except for INVALLR, SYNCR, MOVALLR). It is meaningful
                                                                 only when [V]=1. */
#else /* Word 0 - Little Endian */
        uint64_t int_id                : 20; /**< [ 19:  0](RO/H) Interrrupt ID in the ITS message (except for INVALLR, SYNCR, MOVALLR). It is meaningful
                                                                 only when [V]=1. */
        uint64_t reserved_20_27        : 8;
        uint64_t src_id                : 8;  /**< [ 35: 28](RO/H) SourceID, specified as node_id[1:0], gicr_id[5:0]. It is meaningful only when [V]=1. */
        uint64_t dst_id                : 8;  /**< [ 43: 36](RO/H) DestID, specified as node_id[1:0], gicr_id[5:0]. Relevant only when [V]=1 and
                                                                 [CMD]=MOVLPIR or MOVALLR. */
        uint64_t reserved_44_47        : 4;
        uint64_t err_int_id_range      : 1;  /**< [ 48: 48](RO/H) LPI interrupt ID out of range. Relevant only when [V]=1. */
        uint64_t err_src_gicr_id       : 1;  /**< [ 49: 49](RO/H) Invalid source GICR (Redistributor). Relevant only when [V]=1. */
        uint64_t err_dest_gicr_id      : 1;  /**< [ 50: 50](RO/H) Invalid destination GICR (redistributor). Relevant only when [V]=1 and [CMD]=MOVLPIR or MOVALLR. */
        uint64_t err_multi_socket      : 1;  /**< [ 51: 51](RO/H) Invalid multi-socket message. Relevant only when [V]=1. Indicates incompatibility between
                                                                 operation mode setting (GIC_CFG_CTLR[OM]) and the ITS message. */
        uint64_t reserved_52_55        : 4;
        uint64_t cmd                   : 3;  /**< [ 58: 56](RO/H) ITS Command. Relevant only when [V]=1. Command encodings are
                                                                  SETLPIR = 0x1,
                                                                  CLRLPIR = 0x2,
                                                                  INVLPIR = 0x3,
                                                                  INVALLR = 0x4,
                                                                  SYNCR = 0x5,
                                                                  MOVLPIR = 0x6, and
                                                                  MOVALLR = 0x7. */
        uint64_t reserved_59           : 1;
        uint64_t m                     : 1;  /**< [ 60: 60](RO/H) When set, it means multiple errors have happened. It is meaningful only when [V]=1. */
        uint64_t v                     : 1;  /**< [ 61: 61](R/W1C/H) When set, the command error is valid. */
        uint64_t reserved_62_63        : 2;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_rdb_its_if_err_statusr_s cn; */
};
typedef union bdk_gic_rdb_its_if_err_statusr bdk_gic_rdb_its_if_err_statusr_t;

#define BDK_GIC_RDB_ITS_IF_ERR_STATUSR BDK_GIC_RDB_ITS_IF_ERR_STATUSR_FUNC()
static inline uint64_t BDK_GIC_RDB_ITS_IF_ERR_STATUSR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_RDB_ITS_IF_ERR_STATUSR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x801000010070ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x801000010070ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x801000010070ll;
    __bdk_csr_fatal("GIC_RDB_ITS_IF_ERR_STATUSR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_RDB_ITS_IF_ERR_STATUSR bdk_gic_rdb_its_if_err_statusr_t
#define bustype_BDK_GIC_RDB_ITS_IF_ERR_STATUSR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_RDB_ITS_IF_ERR_STATUSR "GIC_RDB_ITS_IF_ERR_STATUSR"
#define device_bar_BDK_GIC_RDB_ITS_IF_ERR_STATUSR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_RDB_ITS_IF_ERR_STATUSR 0
#define arguments_BDK_GIC_RDB_ITS_IF_ERR_STATUSR -1,-1,-1,-1

/**
 * Register (NCB) gic_rib_err_adrr
 *
 * GIC Implementation RIB Error Address Register
 * This register holds the address of the first RIB error message.
 */
union bdk_gic_rib_err_adrr
{
    uint64_t u;
    struct bdk_gic_rib_err_adrr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t addr                  : 34; /**< [ 35:  2](RO/H) Address of the target CSR. It is meaningful only when GIC_RIB_ERR_STATUSR[V] is set. */
        uint64_t node                  : 2;  /**< [  1:  0](RO/H) ID of the target node. It is meaningful only when GIC_RIB_ERR_STATUSR[V] is set. */
#else /* Word 0 - Little Endian */
        uint64_t node                  : 2;  /**< [  1:  0](RO/H) ID of the target node. It is meaningful only when GIC_RIB_ERR_STATUSR[V] is set. */
        uint64_t addr                  : 34; /**< [ 35:  2](RO/H) Address of the target CSR. It is meaningful only when GIC_RIB_ERR_STATUSR[V] is set. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_rib_err_adrr_s cn; */
};
typedef union bdk_gic_rib_err_adrr bdk_gic_rib_err_adrr_t;

#define BDK_GIC_RIB_ERR_ADRR BDK_GIC_RIB_ERR_ADRR_FUNC()
static inline uint64_t BDK_GIC_RIB_ERR_ADRR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_RIB_ERR_ADRR_FUNC(void)
{
    return 0x801000010048ll;
}

#define typedef_BDK_GIC_RIB_ERR_ADRR bdk_gic_rib_err_adrr_t
#define bustype_BDK_GIC_RIB_ERR_ADRR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_RIB_ERR_ADRR "GIC_RIB_ERR_ADRR"
#define device_bar_BDK_GIC_RIB_ERR_ADRR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_RIB_ERR_ADRR 0
#define arguments_BDK_GIC_RIB_ERR_ADRR -1,-1,-1,-1

/**
 * Register (NCB) gic_rib_err_statusr
 *
 * GIC Implementation RIB Error Status Register
 * This register holds the status of the first RIB error message.
 */
union bdk_gic_rib_err_statusr
{
    uint64_t u;
    struct bdk_gic_rib_err_statusr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_62_63        : 2;
        uint64_t v                     : 1;  /**< [ 61: 61](R/W1C/H) When set, the command error is valid. */
        uint64_t m                     : 1;  /**< [ 60: 60](RO/H) When set, it means multiple errors have happened. It is meaningful only when [V]=1. */
        uint64_t reserved_56_59        : 4;
        uint64_t dev_id                : 24; /**< [ 55: 32](RO/H) Device ID inside the RIB message. */
        uint64_t reserved_29_31        : 3;
        uint64_t secure                : 1;  /**< [ 28: 28](RO/H) Secure bit inside the RIB message. It is meaningful only when V=1. */
        uint64_t reserved_20_27        : 8;
        uint64_t int_id                : 20; /**< [ 19:  0](RO/H) Interrrupt ID inside the RIB message. It is meaningful only when V=1. */
#else /* Word 0 - Little Endian */
        uint64_t int_id                : 20; /**< [ 19:  0](RO/H) Interrrupt ID inside the RIB message. It is meaningful only when V=1. */
        uint64_t reserved_20_27        : 8;
        uint64_t secure                : 1;  /**< [ 28: 28](RO/H) Secure bit inside the RIB message. It is meaningful only when V=1. */
        uint64_t reserved_29_31        : 3;
        uint64_t dev_id                : 24; /**< [ 55: 32](RO/H) Device ID inside the RIB message. */
        uint64_t reserved_56_59        : 4;
        uint64_t m                     : 1;  /**< [ 60: 60](RO/H) When set, it means multiple errors have happened. It is meaningful only when [V]=1. */
        uint64_t v                     : 1;  /**< [ 61: 61](R/W1C/H) When set, the command error is valid. */
        uint64_t reserved_62_63        : 2;
#endif /* Word 0 - End */
    } s;
    struct bdk_gic_rib_err_statusr_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_62_63        : 2;
        uint64_t v                     : 1;  /**< [ 61: 61](R/W1C/H) When set, the command error is valid. */
        uint64_t m                     : 1;  /**< [ 60: 60](RO/H) When set, it means multiple errors have happened. It is meaningful only when [V]=1. */
        uint64_t reserved_53_59        : 7;
        uint64_t dev_id                : 21; /**< [ 52: 32](RO/H) Device ID inside the RIB message. */
        uint64_t reserved_29_31        : 3;
        uint64_t secure                : 1;  /**< [ 28: 28](RO/H) Secure bit inside the RIB message. It is meaningful only when V=1. */
        uint64_t reserved_20_27        : 8;
        uint64_t int_id                : 20; /**< [ 19:  0](RO/H) Interrrupt ID inside the RIB message. It is meaningful only when V=1. */
#else /* Word 0 - Little Endian */
        uint64_t int_id                : 20; /**< [ 19:  0](RO/H) Interrrupt ID inside the RIB message. It is meaningful only when V=1. */
        uint64_t reserved_20_27        : 8;
        uint64_t secure                : 1;  /**< [ 28: 28](RO/H) Secure bit inside the RIB message. It is meaningful only when V=1. */
        uint64_t reserved_29_31        : 3;
        uint64_t dev_id                : 21; /**< [ 52: 32](RO/H) Device ID inside the RIB message. */
        uint64_t reserved_53_59        : 7;
        uint64_t m                     : 1;  /**< [ 60: 60](RO/H) When set, it means multiple errors have happened. It is meaningful only when [V]=1. */
        uint64_t v                     : 1;  /**< [ 61: 61](R/W1C/H) When set, the command error is valid. */
        uint64_t reserved_62_63        : 2;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_gic_rib_err_statusr_s cn9; */
};
typedef union bdk_gic_rib_err_statusr bdk_gic_rib_err_statusr_t;

#define BDK_GIC_RIB_ERR_STATUSR BDK_GIC_RIB_ERR_STATUSR_FUNC()
static inline uint64_t BDK_GIC_RIB_ERR_STATUSR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_RIB_ERR_STATUSR_FUNC(void)
{
    return 0x801000010040ll;
}

#define typedef_BDK_GIC_RIB_ERR_STATUSR bdk_gic_rib_err_statusr_t
#define bustype_BDK_GIC_RIB_ERR_STATUSR BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_RIB_ERR_STATUSR "GIC_RIB_ERR_STATUSR"
#define device_bar_BDK_GIC_RIB_ERR_STATUSR 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_RIB_ERR_STATUSR 0
#define arguments_BDK_GIC_RIB_ERR_STATUSR -1,-1,-1,-1

/**
 * Register (NCB) gic_scratch
 *
 * GIC Scratch Register
 * This is a scratch register.
 */
union bdk_gic_scratch
{
    uint64_t u;
    struct bdk_gic_scratch_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) This is a scratch register. Reads and writes of this register have no side effects. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) This is a scratch register. Reads and writes of this register have no side effects. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_scratch_s cn; */
};
typedef union bdk_gic_scratch bdk_gic_scratch_t;

#define BDK_GIC_SCRATCH BDK_GIC_SCRATCH_FUNC()
static inline uint64_t BDK_GIC_SCRATCH_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_SCRATCH_FUNC(void)
{
    return 0x801000010080ll;
}

#define typedef_BDK_GIC_SCRATCH bdk_gic_scratch_t
#define bustype_BDK_GIC_SCRATCH BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_SCRATCH "GIC_SCRATCH"
#define device_bar_BDK_GIC_SCRATCH 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_SCRATCH 0
#define arguments_BDK_GIC_SCRATCH -1,-1,-1,-1

/**
 * Register (NCB) gic_sync_cfg
 *
 * GIC SYNC Configuration Register
 * This register configures the behavior of ITS SYNC command.
 */
union bdk_gic_sync_cfg
{
    uint64_t u;
    struct bdk_gic_sync_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t te                    : 1;  /**< [ 63: 63](R/W) Translation enable. If set, there is interrupt translation is enabled during
                                                                 sync command execution. If clear, interrupt translation is disabled during sync
                                                                 command execution. */
        uint64_t reserved_32_62        : 31;
        uint64_t tol                   : 32; /**< [ 31:  0](R/W) Time out limit. Timeout wait period for the ITS SYNC command. SYNC command will
                                                                 wait for ACK from a GICR for at most [TOL] system-clock cycles. If ACK is not
                                                                 received within [TOL] system-clock cycles, SYNC is timed out and considered
                                                                 done. [TOL] = 0x0 means SYNC timeout scheme is not used and SYNC command always
                                                                 waits for ACK. */
#else /* Word 0 - Little Endian */
        uint64_t tol                   : 32; /**< [ 31:  0](R/W) Time out limit. Timeout wait period for the ITS SYNC command. SYNC command will
                                                                 wait for ACK from a GICR for at most [TOL] system-clock cycles. If ACK is not
                                                                 received within [TOL] system-clock cycles, SYNC is timed out and considered
                                                                 done. [TOL] = 0x0 means SYNC timeout scheme is not used and SYNC command always
                                                                 waits for ACK. */
        uint64_t reserved_32_62        : 31;
        uint64_t te                    : 1;  /**< [ 63: 63](R/W) Translation enable. If set, there is interrupt translation is enabled during
                                                                 sync command execution. If clear, interrupt translation is disabled during sync
                                                                 command execution. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gic_sync_cfg_s cn; */
};
typedef union bdk_gic_sync_cfg bdk_gic_sync_cfg_t;

#define BDK_GIC_SYNC_CFG BDK_GIC_SYNC_CFG_FUNC()
static inline uint64_t BDK_GIC_SYNC_CFG_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GIC_SYNC_CFG_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x801000010050ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x801000010050ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X))
        return 0x801000010050ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x801000010050ll;
    __bdk_csr_fatal("GIC_SYNC_CFG", 0, 0, 0, 0, 0);
}

#define typedef_BDK_GIC_SYNC_CFG bdk_gic_sync_cfg_t
#define bustype_BDK_GIC_SYNC_CFG BDK_CSR_TYPE_NCB
#define basename_BDK_GIC_SYNC_CFG "GIC_SYNC_CFG"
#define device_bar_BDK_GIC_SYNC_CFG 0x0 /* PF_BAR0 */
#define busnum_BDK_GIC_SYNC_CFG 0
#define arguments_BDK_GIC_SYNC_CFG -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_cidr0
 *
 * GIC Distributor Component Identification Register 0
 */
union bdk_gicd_cidr0
{
    uint32_t u;
    struct bdk_gicd_cidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_cidr0_s cn; */
};
typedef union bdk_gicd_cidr0 bdk_gicd_cidr0_t;

#define BDK_GICD_CIDR0 BDK_GICD_CIDR0_FUNC()
static inline uint64_t BDK_GICD_CIDR0_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_CIDR0_FUNC(void)
{
    return 0x80100000fff0ll;
}

#define typedef_BDK_GICD_CIDR0 bdk_gicd_cidr0_t
#define bustype_BDK_GICD_CIDR0 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_CIDR0 "GICD_CIDR0"
#define device_bar_BDK_GICD_CIDR0 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_CIDR0 0
#define arguments_BDK_GICD_CIDR0 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_cidr1
 *
 * GIC Distributor Component Identification Register 1
 */
union bdk_gicd_cidr1
{
    uint32_t u;
    struct bdk_gicd_cidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_cidr1_s cn; */
};
typedef union bdk_gicd_cidr1 bdk_gicd_cidr1_t;

#define BDK_GICD_CIDR1 BDK_GICD_CIDR1_FUNC()
static inline uint64_t BDK_GICD_CIDR1_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_CIDR1_FUNC(void)
{
    return 0x80100000fff4ll;
}

#define typedef_BDK_GICD_CIDR1 bdk_gicd_cidr1_t
#define bustype_BDK_GICD_CIDR1 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_CIDR1 "GICD_CIDR1"
#define device_bar_BDK_GICD_CIDR1 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_CIDR1 0
#define arguments_BDK_GICD_CIDR1 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_cidr2
 *
 * GIC Distributor Component Identification Register 2
 */
union bdk_gicd_cidr2
{
    uint32_t u;
    struct bdk_gicd_cidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_cidr2_s cn; */
};
typedef union bdk_gicd_cidr2 bdk_gicd_cidr2_t;

#define BDK_GICD_CIDR2 BDK_GICD_CIDR2_FUNC()
static inline uint64_t BDK_GICD_CIDR2_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_CIDR2_FUNC(void)
{
    return 0x80100000fff8ll;
}

#define typedef_BDK_GICD_CIDR2 bdk_gicd_cidr2_t
#define bustype_BDK_GICD_CIDR2 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_CIDR2 "GICD_CIDR2"
#define device_bar_BDK_GICD_CIDR2 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_CIDR2 0
#define arguments_BDK_GICD_CIDR2 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_cidr3
 *
 * GIC Distributor Component Identification Register 3
 */
union bdk_gicd_cidr3
{
    uint32_t u;
    struct bdk_gicd_cidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_cidr3_s cn; */
};
typedef union bdk_gicd_cidr3 bdk_gicd_cidr3_t;

#define BDK_GICD_CIDR3 BDK_GICD_CIDR3_FUNC()
static inline uint64_t BDK_GICD_CIDR3_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_CIDR3_FUNC(void)
{
    return 0x80100000fffcll;
}

#define typedef_BDK_GICD_CIDR3 bdk_gicd_cidr3_t
#define bustype_BDK_GICD_CIDR3 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_CIDR3 "GICD_CIDR3"
#define device_bar_BDK_GICD_CIDR3 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_CIDR3 0
#define arguments_BDK_GICD_CIDR3 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_clrspi_nsr
 *
 * GIC Distributor Clear SPI Pending Register
 */
union bdk_gicd_clrspi_nsr
{
    uint32_t u;
    struct bdk_gicd_clrspi_nsr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_10_31        : 22;
        uint32_t spi_id                : 10; /**< [  9:  0](WO) Clear an SPI pending state (write-only). If the SPI is not pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access and the value specifies a secure SPI
                                                                 and the value of the corresponding GICD_NSACR() register is less than 0x2 (i.e. does not
                                                                 permit nonsecure accesses to clear the interrupt pending state), the write has no effect. */
#else /* Word 0 - Little Endian */
        uint32_t spi_id                : 10; /**< [  9:  0](WO) Clear an SPI pending state (write-only). If the SPI is not pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access and the value specifies a secure SPI
                                                                 and the value of the corresponding GICD_NSACR() register is less than 0x2 (i.e. does not
                                                                 permit nonsecure accesses to clear the interrupt pending state), the write has no effect. */
        uint32_t reserved_10_31        : 22;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_clrspi_nsr_s cn; */
};
typedef union bdk_gicd_clrspi_nsr bdk_gicd_clrspi_nsr_t;

#define BDK_GICD_CLRSPI_NSR BDK_GICD_CLRSPI_NSR_FUNC()
static inline uint64_t BDK_GICD_CLRSPI_NSR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_CLRSPI_NSR_FUNC(void)
{
    return 0x801000000048ll;
}

#define typedef_BDK_GICD_CLRSPI_NSR bdk_gicd_clrspi_nsr_t
#define bustype_BDK_GICD_CLRSPI_NSR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_CLRSPI_NSR "GICD_CLRSPI_NSR"
#define device_bar_BDK_GICD_CLRSPI_NSR 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_CLRSPI_NSR 0
#define arguments_BDK_GICD_CLRSPI_NSR -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_clrspi_sr
 *
 * GIC Distributor Clear Secure SPI Pending Register
 */
union bdk_gicd_clrspi_sr
{
    uint32_t u;
    struct bdk_gicd_clrspi_sr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_10_31        : 22;
        uint32_t spi_id                : 10; /**< [  9:  0](SWO) Clear an SPI pending state (write-only). If the SPI is not pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access, the write has no effect. */
#else /* Word 0 - Little Endian */
        uint32_t spi_id                : 10; /**< [  9:  0](SWO) Clear an SPI pending state (write-only). If the SPI is not pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access, the write has no effect. */
        uint32_t reserved_10_31        : 22;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_clrspi_sr_s cn; */
};
typedef union bdk_gicd_clrspi_sr bdk_gicd_clrspi_sr_t;

#define BDK_GICD_CLRSPI_SR BDK_GICD_CLRSPI_SR_FUNC()
static inline uint64_t BDK_GICD_CLRSPI_SR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_CLRSPI_SR_FUNC(void)
{
    return 0x801000000058ll;
}

#define typedef_BDK_GICD_CLRSPI_SR bdk_gicd_clrspi_sr_t
#define bustype_BDK_GICD_CLRSPI_SR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_CLRSPI_SR "GICD_CLRSPI_SR"
#define device_bar_BDK_GICD_CLRSPI_SR 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_CLRSPI_SR 0
#define arguments_BDK_GICD_CLRSPI_SR -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_icactiver#
 *
 * GIC Distributor Interrupt Clear-Active Registers
 * Each bit in this register provides a clear-active bit for each SPI supported by the GIC.
 * Writing one to a clear-active bit clears the active status of the corresponding SPI.
 */
union bdk_gicd_icactiverx
{
    uint32_t u;
    struct bdk_gicd_icactiverx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicd_icactiverx_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicd_icactiverx_cn9 cn81xx; */
    /* struct bdk_gicd_icactiverx_s cn88xx; */
    /* struct bdk_gicd_icactiverx_cn9 cn83xx; */
};
typedef union bdk_gicd_icactiverx bdk_gicd_icactiverx_t;

static inline uint64_t BDK_GICD_ICACTIVERX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_ICACTIVERX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000380ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_ICACTIVERX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_ICACTIVERX(a) bdk_gicd_icactiverx_t
#define bustype_BDK_GICD_ICACTIVERX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_ICACTIVERX(a) "GICD_ICACTIVERX"
#define device_bar_BDK_GICD_ICACTIVERX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_ICACTIVERX(a) (a)
#define arguments_BDK_GICD_ICACTIVERX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_icenabler#
 *
 * GIC Distributor Interrupt Clear-Enable Registers
 * Each bit in GICD_ICENABLER() provides a clear-enable bit for each SPI supported by the GIC.
 * Writing one to a clear-enable bit disables forwarding of the corresponding SPI from the
 * distributor to the CPU interfaces. Reading a bit identifies whether the SPI is enabled.
 */
union bdk_gicd_icenablerx
{
    uint32_t u;
    struct bdk_gicd_icenablerx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SPI for SPI IDs in the range 159..32. Upon reading, if a bit is
                                                                 zero, then the SPI is not enabled to be forwarded to the CPU interface. Upon reading, if a
                                                                 bit is one, the SPI is enabled to be forwarded to the CPU interface. Clear-enable bits
                                                                 corresponding to secure interrupts (either group 0 or group 1) may only be set by secure
                                                                 accesses.

                                                                 Writes to the register cannot be considered complete until the effects of the write are
                                                                 visible throughout the affinity hierarchy. To ensure that an enable has been cleared,
                                                                 software must write to this register with bits set to clear the required enables. Software
                                                                 must then poll GICD_(S)CTLR[RWP] (register writes pending) until it has the value zero. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SPI for SPI IDs in the range 159..32. Upon reading, if a bit is
                                                                 zero, then the SPI is not enabled to be forwarded to the CPU interface. Upon reading, if a
                                                                 bit is one, the SPI is enabled to be forwarded to the CPU interface. Clear-enable bits
                                                                 corresponding to secure interrupts (either group 0 or group 1) may only be set by secure
                                                                 accesses.

                                                                 Writes to the register cannot be considered complete until the effects of the write are
                                                                 visible throughout the affinity hierarchy. To ensure that an enable has been cleared,
                                                                 software must write to this register with bits set to clear the required enables. Software
                                                                 must then poll GICD_(S)CTLR[RWP] (register writes pending) until it has the value zero. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_icenablerx_s cn; */
};
typedef union bdk_gicd_icenablerx bdk_gicd_icenablerx_t;

static inline uint64_t BDK_GICD_ICENABLERX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_ICENABLERX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000180ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_ICENABLERX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_ICENABLERX(a) bdk_gicd_icenablerx_t
#define bustype_BDK_GICD_ICENABLERX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_ICENABLERX(a) "GICD_ICENABLERX"
#define device_bar_BDK_GICD_ICENABLERX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_ICENABLERX(a) (a)
#define arguments_BDK_GICD_ICENABLERX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_icfgr#
 *
 * GIC Distributor SPI Configuration Registers
 */
union bdk_gicd_icfgrx
{
    uint32_t u;
    struct bdk_gicd_icfgrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W) Two bits per SPI. Defines whether an SPI is level-sensitive or edge-triggered.
                                                                 Bit[1] is zero, the SPI is level-sensitive.
                                                                 Bit[1] is one, the SPI is edge-triggered.
                                                                 Bit[0] Reserved.

                                                                 If SPI is a secure interrupt, then its corresponding field is RAZ/WI to nonsecure
                                                                 accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W) Two bits per SPI. Defines whether an SPI is level-sensitive or edge-triggered.
                                                                 Bit[1] is zero, the SPI is level-sensitive.
                                                                 Bit[1] is one, the SPI is edge-triggered.
                                                                 Bit[0] Reserved.

                                                                 If SPI is a secure interrupt, then its corresponding field is RAZ/WI to nonsecure
                                                                 accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_icfgrx_s cn; */
};
typedef union bdk_gicd_icfgrx bdk_gicd_icfgrx_t;

static inline uint64_t BDK_GICD_ICFGRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_ICFGRX(unsigned long a)
{
    if ((a>=2)&&(a<=9))
        return 0x801000000c00ll + 4ll * ((a) & 0xf);
    __bdk_csr_fatal("GICD_ICFGRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_ICFGRX(a) bdk_gicd_icfgrx_t
#define bustype_BDK_GICD_ICFGRX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_ICFGRX(a) "GICD_ICFGRX"
#define device_bar_BDK_GICD_ICFGRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_ICFGRX(a) (a)
#define arguments_BDK_GICD_ICFGRX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_icpendr#
 *
 * GIC Distributor Interrupt Clear-Pending Registers
 * Each bit in GICD_ICPENDR() provides a clear-pending bit for each SPI supported by the GIC.
 * Writing one to a clear-pending bit clears the pending status of the corresponding SPI.
 */
union bdk_gicd_icpendrx
{
    uint32_t u;
    struct bdk_gicd_icpendrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicd_icpendrx_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicd_icpendrx_cn9 cn81xx; */
    /* struct bdk_gicd_icpendrx_s cn88xx; */
    /* struct bdk_gicd_icpendrx_cn9 cn83xx; */
};
typedef union bdk_gicd_icpendrx bdk_gicd_icpendrx_t;

static inline uint64_t BDK_GICD_ICPENDRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_ICPENDRX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000280ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_ICPENDRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_ICPENDRX(a) bdk_gicd_icpendrx_t
#define bustype_BDK_GICD_ICPENDRX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_ICPENDRX(a) "GICD_ICPENDRX"
#define device_bar_BDK_GICD_ICPENDRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_ICPENDRX(a) (a)
#define arguments_BDK_GICD_ICPENDRX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_igroupr#
 *
 * GIC Distributor Secure Interrupt Group Registers
 * The bit in this register for a particular SPI is concatenated with the corresponding
 * bit for that SPI in GICD_IGRPMODR() to form a two-bit field that defines the
 * interrupt group (G0S, G1S, G1NS) for that SPI.
 */
union bdk_gicd_igrouprx
{
    uint32_t u;
    struct bdk_gicd_igrouprx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If zero, then the SPI is
                                                                 secure. If one, the SPI is nonsecure. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If zero, then the SPI is
                                                                 secure. If one, the SPI is nonsecure. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_igrouprx_s cn; */
};
typedef union bdk_gicd_igrouprx bdk_gicd_igrouprx_t;

static inline uint64_t BDK_GICD_IGROUPRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_IGROUPRX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000080ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_IGROUPRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_IGROUPRX(a) bdk_gicd_igrouprx_t
#define bustype_BDK_GICD_IGROUPRX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_IGROUPRX(a) "GICD_IGROUPRX"
#define device_bar_BDK_GICD_IGROUPRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_IGROUPRX(a) (a)
#define arguments_BDK_GICD_IGROUPRX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_igrpmodr#
 *
 * GIC Distributor Interrupt Group Modifier Secure Registers
 * The bit in this register for a particular SPI is concatenated with the
 * corresponding bit for that SPI in GICD_IGROUPR() to form a two-bit field that defines
 * the interrupt group (G0S, G1S, G1NS) for that SPI.
 */
union bdk_gicd_igrpmodrx
{
    uint32_t u;
    struct bdk_gicd_igrpmodrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If zero, then
                                                                 the SPI group is not modified. If one, then the SPI group is modified from group
                                                                 0 to secure group 1. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If zero, then
                                                                 the SPI group is not modified. If one, then the SPI group is modified from group
                                                                 0 to secure group 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_igrpmodrx_s cn; */
};
typedef union bdk_gicd_igrpmodrx bdk_gicd_igrpmodrx_t;

static inline uint64_t BDK_GICD_IGRPMODRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_IGRPMODRX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000d00ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_IGRPMODRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_IGRPMODRX(a) bdk_gicd_igrpmodrx_t
#define bustype_BDK_GICD_IGRPMODRX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_IGRPMODRX(a) "GICD_IGRPMODRX"
#define device_bar_BDK_GICD_IGRPMODRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_IGRPMODRX(a) (a)
#define arguments_BDK_GICD_IGRPMODRX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_iidr
 *
 * GIC Distributor Implementation Identification Register
 * This 32-bit register is read-only and specifies the version and features supported by the
 * distributor.
 */
union bdk_gicd_iidr
{
    uint32_t u;
    struct bdk_gicd_iidr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t productid             : 8;  /**< [ 31: 24](RO) An implementation defined product number for the device.
                                                                 In CNXXXX, enumerated by PCC_PROD_E. */
        uint32_t reserved_20_23        : 4;
        uint32_t variant               : 4;  /**< [ 19: 16](RO) Indicates the major revision or variant of the product.
                                                                 On CNXXXX, this is the major revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t revision              : 4;  /**< [ 15: 12](RO) Indicates the minor revision of the product.
                                                                 On CNXXXX, this is the minor revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t implementer           : 12; /**< [ 11:  0](RO) Indicates the implementer:
                                                                    0x34C = Cavium. */
#else /* Word 0 - Little Endian */
        uint32_t implementer           : 12; /**< [ 11:  0](RO) Indicates the implementer:
                                                                    0x34C = Cavium. */
        uint32_t revision              : 4;  /**< [ 15: 12](RO) Indicates the minor revision of the product.
                                                                 On CNXXXX, this is the minor revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t variant               : 4;  /**< [ 19: 16](RO) Indicates the major revision or variant of the product.
                                                                 On CNXXXX, this is the major revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t reserved_20_23        : 4;
        uint32_t productid             : 8;  /**< [ 31: 24](RO) An implementation defined product number for the device.
                                                                 In CNXXXX, enumerated by PCC_PROD_E. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_iidr_s cn; */
};
typedef union bdk_gicd_iidr bdk_gicd_iidr_t;

#define BDK_GICD_IIDR BDK_GICD_IIDR_FUNC()
static inline uint64_t BDK_GICD_IIDR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_IIDR_FUNC(void)
{
    return 0x801000000008ll;
}

#define typedef_BDK_GICD_IIDR bdk_gicd_iidr_t
#define bustype_BDK_GICD_IIDR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_IIDR "GICD_IIDR"
#define device_bar_BDK_GICD_IIDR 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_IIDR 0
#define arguments_BDK_GICD_IIDR -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_ipriorityr#
 *
 * GIC Distributor Interrupt Priority Registers
 * Each byte in this register provides a priority field for each SPI supported by the GIC.
 */
union bdk_gicd_ipriorityrx
{
    uint32_t u;
    struct bdk_gicd_ipriorityrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W) Each byte corresponds to an SPI for SPI IDs in the range 159..32.

                                                                 Priority fields corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses, or when GICD_(S)CTLR[DS] is one.

                                                                 Byte accesses are permitted to these registers.

                                                                 A priority field for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W) Each byte corresponds to an SPI for SPI IDs in the range 159..32.

                                                                 Priority fields corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses, or when GICD_(S)CTLR[DS] is one.

                                                                 Byte accesses are permitted to these registers.

                                                                 A priority field for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_ipriorityrx_s cn; */
};
typedef union bdk_gicd_ipriorityrx bdk_gicd_ipriorityrx_t;

static inline uint64_t BDK_GICD_IPRIORITYRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_IPRIORITYRX(unsigned long a)
{
    if ((a>=8)&&(a<=39))
        return 0x801000000400ll + 4ll * ((a) & 0x3f);
    __bdk_csr_fatal("GICD_IPRIORITYRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_IPRIORITYRX(a) bdk_gicd_ipriorityrx_t
#define bustype_BDK_GICD_IPRIORITYRX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_IPRIORITYRX(a) "GICD_IPRIORITYRX"
#define device_bar_BDK_GICD_IPRIORITYRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_IPRIORITYRX(a) (a)
#define arguments_BDK_GICD_IPRIORITYRX(a) (a),-1,-1,-1

/**
 * Register (NCB) gicd_irouter#
 *
 * GIC Distributor SPI Routing Registers
 * These registers provide the routing information for the security state of the associated SPIs.
 * Up to 64 bits of state to control the routing.
 */
union bdk_gicd_irouterx
{
    uint64_t u;
    struct bdk_gicd_irouterx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_40_63        : 24;
        uint64_t a3                    : 8;  /**< [ 39: 32](RO) Specifies the affinity 3 level for the SPI. In CNXXXX implementation, 0x0. */
        uint64_t irm                   : 1;  /**< [ 31: 31](R/W) Specifies the interrupt routing mode for the SPI.
                                                                 0 = Route to the processor specified by the affinity levels A3.A2.A1.A0.
                                                                 1 = Route to any one processor in the system (one-of-N). */
        uint64_t reserved_24_30        : 7;
        uint64_t a2                    : 8;  /**< [ 23: 16](R/W) Specifies the affinity 2 level for the SPI. */
        uint64_t a1                    : 8;  /**< [ 15:  8](R/W) Specifies the affinity 1 level for the SPI. */
        uint64_t a0                    : 8;  /**< [  7:  0](R/W) Specifies the affinity 0 level for the SPI. */
#else /* Word 0 - Little Endian */
        uint64_t a0                    : 8;  /**< [  7:  0](R/W) Specifies the affinity 0 level for the SPI. */
        uint64_t a1                    : 8;  /**< [ 15:  8](R/W) Specifies the affinity 1 level for the SPI. */
        uint64_t a2                    : 8;  /**< [ 23: 16](R/W) Specifies the affinity 2 level for the SPI. */
        uint64_t reserved_24_30        : 7;
        uint64_t irm                   : 1;  /**< [ 31: 31](R/W) Specifies the interrupt routing mode for the SPI.
                                                                 0 = Route to the processor specified by the affinity levels A3.A2.A1.A0.
                                                                 1 = Route to any one processor in the system (one-of-N). */
        uint64_t a3                    : 8;  /**< [ 39: 32](RO) Specifies the affinity 3 level for the SPI. In CNXXXX implementation, 0x0. */
        uint64_t reserved_40_63        : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_irouterx_s cn; */
};
typedef union bdk_gicd_irouterx bdk_gicd_irouterx_t;

static inline uint64_t BDK_GICD_IROUTERX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_IROUTERX(unsigned long a)
{
    if ((a>=32)&&(a<=159))
        return 0x801000006000ll + 8ll * ((a) & 0xff);
    __bdk_csr_fatal("GICD_IROUTERX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_IROUTERX(a) bdk_gicd_irouterx_t
#define bustype_BDK_GICD_IROUTERX(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICD_IROUTERX(a) "GICD_IROUTERX"
#define device_bar_BDK_GICD_IROUTERX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_IROUTERX(a) (a)
#define arguments_BDK_GICD_IROUTERX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_isactiver#
 *
 * GIC Distributor Interrupt Set-Active Registers
 * Each bit in this register provides a set-active bit for each SPI supported by the GIC.
 * Writing one to a set-active bit sets the status of the corresponding SPI to active.
 */
union bdk_gicd_isactiverx
{
    uint32_t u;
    struct bdk_gicd_isactiverx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicd_isactiverx_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not active. If read as one, the SPI is in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-active bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicd_isactiverx_cn9 cn81xx; */
    /* struct bdk_gicd_isactiverx_s cn88xx; */
    /* struct bdk_gicd_isactiverx_cn9 cn83xx; */
};
typedef union bdk_gicd_isactiverx bdk_gicd_isactiverx_t;

static inline uint64_t BDK_GICD_ISACTIVERX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_ISACTIVERX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000300ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_ISACTIVERX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_ISACTIVERX(a) bdk_gicd_isactiverx_t
#define bustype_BDK_GICD_ISACTIVERX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_ISACTIVERX(a) "GICD_ISACTIVERX"
#define device_bar_BDK_GICD_ISACTIVERX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_ISACTIVERX(a) (a)
#define arguments_BDK_GICD_ISACTIVERX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_isenabler#
 *
 * GIC Distributor Interrupt Set-Enable Registers
 * Each bit in GICD_ISENABLER() provides a set-enable bit for each SPI supported by the GIC.
 * Writing one to a set-enable bit enables forwarding of the corresponding SPI from the
 * distributor to the CPU interfaces.
 */
union bdk_gicd_isenablerx
{
    uint32_t u;
    struct bdk_gicd_isenablerx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If zero, then
                                                                 the SPI is not enabled to be forwarded to the CPU interface. If one, the SPI is
                                                                 enabled to be forwarded to the CPU interface. Set-enable bits corresponding to
                                                                 secure interrupts (either group 0 or group 1) may only be set by secure
                                                                 accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If zero, then
                                                                 the SPI is not enabled to be forwarded to the CPU interface. If one, the SPI is
                                                                 enabled to be forwarded to the CPU interface. Set-enable bits corresponding to
                                                                 secure interrupts (either group 0 or group 1) may only be set by secure
                                                                 accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_isenablerx_s cn; */
};
typedef union bdk_gicd_isenablerx bdk_gicd_isenablerx_t;

static inline uint64_t BDK_GICD_ISENABLERX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_ISENABLERX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000100ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_ISENABLERX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_ISENABLERX(a) bdk_gicd_isenablerx_t
#define bustype_BDK_GICD_ISENABLERX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_ISENABLERX(a) "GICD_ISENABLERX"
#define device_bar_BDK_GICD_ISENABLERX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_ISENABLERX(a) (a)
#define arguments_BDK_GICD_ISENABLERX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_ispendr#
 *
 * GIC Distributor Interrupt Set-Pending Registers
 * Each bit in GICD_ISPENDR() provides a set-pending bit for each SPI supported by the GIC.
 * Writing one to a set-pending bit sets the status of the corresponding SPI to pending.
 */
union bdk_gicd_ispendrx
{
    uint32_t u;
    struct bdk_gicd_ispendrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicd_ispendrx_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SPI for SPI IDs in the range 159..32. If read as zero, then the
                                                                 SPI
                                                                 is not pending. If read as one, the SPI is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A set-pending bit for a secure SPI is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicd_ispendrx_cn9 cn81xx; */
    /* struct bdk_gicd_ispendrx_s cn88xx; */
    /* struct bdk_gicd_ispendrx_cn9 cn83xx; */
};
typedef union bdk_gicd_ispendrx bdk_gicd_ispendrx_t;

static inline uint64_t BDK_GICD_ISPENDRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_ISPENDRX(unsigned long a)
{
    if ((a>=1)&&(a<=4))
        return 0x801000000200ll + 4ll * ((a) & 0x7);
    __bdk_csr_fatal("GICD_ISPENDRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_ISPENDRX(a) bdk_gicd_ispendrx_t
#define bustype_BDK_GICD_ISPENDRX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_ISPENDRX(a) "GICD_ISPENDRX"
#define device_bar_BDK_GICD_ISPENDRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_ISPENDRX(a) (a)
#define arguments_BDK_GICD_ISPENDRX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_nsacr#
 *
 * GIC Distributor Nonsecure Access Control Secure Registers
 */
union bdk_gicd_nsacrx
{
    uint32_t u;
    struct bdk_gicd_nsacrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Two bits per SPI. Defines whether nonsecure access is permitted to secure SPI resources.
                                                                 0x0 = No nonsecure access is permitted to fields associated with the corresponding SPI.
                                                                 0x1 = Nonsecure read and write access is permitted to fields associated with the SPI in
                                                                 GICD_ISPENDR(). A nonsecure write access to GICD_SETSPI_NSR is permitted to
                                                                 set the pending state of the corresponding SPI.
                                                                 0x2 = Adds nonsecure read and write access permissions to fields associated with the
                                                                 corresponding SPI in GICD_ICPENDR(). A nonsecure write access to
                                                                 GICD_CLRSPI_NSR is permitted to clear the pending state of the corresponding SPI. Also
                                                                 adds nonsecure read access permission to fields associated with the corresponding SPI in
                                                                 the GICD_ISACTIVER() and GICD_ICACTIVER().
                                                                 0x3 = Adds nonsecure read and write access permission to fields associated with the
                                                                 corresponding SPI in GICD_IROUTER().

                                                                 This register is RAZ/WI for nonsecure accesses.

                                                                 When GICD_(S)CTLR[DS] is one, this register is RAZ/WI. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Two bits per SPI. Defines whether nonsecure access is permitted to secure SPI resources.
                                                                 0x0 = No nonsecure access is permitted to fields associated with the corresponding SPI.
                                                                 0x1 = Nonsecure read and write access is permitted to fields associated with the SPI in
                                                                 GICD_ISPENDR(). A nonsecure write access to GICD_SETSPI_NSR is permitted to
                                                                 set the pending state of the corresponding SPI.
                                                                 0x2 = Adds nonsecure read and write access permissions to fields associated with the
                                                                 corresponding SPI in GICD_ICPENDR(). A nonsecure write access to
                                                                 GICD_CLRSPI_NSR is permitted to clear the pending state of the corresponding SPI. Also
                                                                 adds nonsecure read access permission to fields associated with the corresponding SPI in
                                                                 the GICD_ISACTIVER() and GICD_ICACTIVER().
                                                                 0x3 = Adds nonsecure read and write access permission to fields associated with the
                                                                 corresponding SPI in GICD_IROUTER().

                                                                 This register is RAZ/WI for nonsecure accesses.

                                                                 When GICD_(S)CTLR[DS] is one, this register is RAZ/WI. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_nsacrx_s cn; */
};
typedef union bdk_gicd_nsacrx bdk_gicd_nsacrx_t;

static inline uint64_t BDK_GICD_NSACRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_NSACRX(unsigned long a)
{
    if ((a>=2)&&(a<=9))
        return 0x801000000e00ll + 4ll * ((a) & 0xf);
    __bdk_csr_fatal("GICD_NSACRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICD_NSACRX(a) bdk_gicd_nsacrx_t
#define bustype_BDK_GICD_NSACRX(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_NSACRX(a) "GICD_NSACRX"
#define device_bar_BDK_GICD_NSACRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_NSACRX(a) (a)
#define arguments_BDK_GICD_NSACRX(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr0
 *
 * GIC Distributor Peripheral Identification Register 0
 */
union bdk_gicd_pidr0
{
    uint32_t u;
    struct bdk_gicd_pidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  Indicates PCC_PIDR_PARTNUM0_E::GICD. */
#else /* Word 0 - Little Endian */
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  Indicates PCC_PIDR_PARTNUM0_E::GICD. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr0_s cn; */
};
typedef union bdk_gicd_pidr0 bdk_gicd_pidr0_t;

#define BDK_GICD_PIDR0 BDK_GICD_PIDR0_FUNC()
static inline uint64_t BDK_GICD_PIDR0_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR0_FUNC(void)
{
    return 0x80100000ffe0ll;
}

#define typedef_BDK_GICD_PIDR0 bdk_gicd_pidr0_t
#define bustype_BDK_GICD_PIDR0 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR0 "GICD_PIDR0"
#define device_bar_BDK_GICD_PIDR0 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR0 0
#define arguments_BDK_GICD_PIDR0 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr1
 *
 * GIC Distributor Peripheral Identification Register 1
 */
union bdk_gicd_pidr1
{
    uint32_t u;
    struct bdk_gicd_pidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t idcode                : 4;  /**< [  7:  4](RO) JEP106 identification code \<3:0\>. Cavium code is 0x4C. */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  Indicates PCC_PIDR_PARTNUM1_E::COMP. */
#else /* Word 0 - Little Endian */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  Indicates PCC_PIDR_PARTNUM1_E::COMP. */
        uint32_t idcode                : 4;  /**< [  7:  4](RO) JEP106 identification code \<3:0\>. Cavium code is 0x4C. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr1_s cn; */
};
typedef union bdk_gicd_pidr1 bdk_gicd_pidr1_t;

#define BDK_GICD_PIDR1 BDK_GICD_PIDR1_FUNC()
static inline uint64_t BDK_GICD_PIDR1_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR1_FUNC(void)
{
    return 0x80100000ffe4ll;
}

#define typedef_BDK_GICD_PIDR1 bdk_gicd_pidr1_t
#define bustype_BDK_GICD_PIDR1 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR1 "GICD_PIDR1"
#define device_bar_BDK_GICD_PIDR1 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR1 0
#define arguments_BDK_GICD_PIDR1 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr2
 *
 * GIC Distributor Peripheral Identification Register 2
 */
union bdk_gicd_pidr2
{
    uint32_t u;
    struct bdk_gicd_pidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t archrev               : 4;  /**< [  7:  4](RO) Architectural revision:
                                                                   0x1 = GICv1.
                                                                   0x2 = GICV2.
                                                                   0x3 = GICv3.
                                                                   0x4 = GICv4.
                                                                   0x5-0xF = Reserved. */
        uint32_t usesjepcode           : 1;  /**< [  3:  3](RO) JEDEC assigned. */
        uint32_t jepid                 : 3;  /**< [  2:  0](RO) JEP106 identification code \<6:4\>. Cavium code is 0x4C. */
#else /* Word 0 - Little Endian */
        uint32_t jepid                 : 3;  /**< [  2:  0](RO) JEP106 identification code \<6:4\>. Cavium code is 0x4C. */
        uint32_t usesjepcode           : 1;  /**< [  3:  3](RO) JEDEC assigned. */
        uint32_t archrev               : 4;  /**< [  7:  4](RO) Architectural revision:
                                                                   0x1 = GICv1.
                                                                   0x2 = GICV2.
                                                                   0x3 = GICv3.
                                                                   0x4 = GICv4.
                                                                   0x5-0xF = Reserved. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr2_s cn; */
};
typedef union bdk_gicd_pidr2 bdk_gicd_pidr2_t;

#define BDK_GICD_PIDR2 BDK_GICD_PIDR2_FUNC()
static inline uint64_t BDK_GICD_PIDR2_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR2_FUNC(void)
{
    return 0x80100000ffe8ll;
}

#define typedef_BDK_GICD_PIDR2 bdk_gicd_pidr2_t
#define bustype_BDK_GICD_PIDR2 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR2 "GICD_PIDR2"
#define device_bar_BDK_GICD_PIDR2 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR2 0
#define arguments_BDK_GICD_PIDR2 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr3
 *
 * GIC Distributor Peripheral Identification Register 3
 */
union bdk_gicd_pidr3
{
    uint32_t u;
    struct bdk_gicd_pidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t cmod                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
#else /* Word 0 - Little Endian */
        uint32_t cmod                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr3_s cn; */
};
typedef union bdk_gicd_pidr3 bdk_gicd_pidr3_t;

#define BDK_GICD_PIDR3 BDK_GICD_PIDR3_FUNC()
static inline uint64_t BDK_GICD_PIDR3_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR3_FUNC(void)
{
    return 0x80100000ffecll;
}

#define typedef_BDK_GICD_PIDR3 bdk_gicd_pidr3_t
#define bustype_BDK_GICD_PIDR3 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR3 "GICD_PIDR3"
#define device_bar_BDK_GICD_PIDR3 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR3 0
#define arguments_BDK_GICD_PIDR3 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr4
 *
 * GIC Distributor Peripheral Identification Register 4
 */
union bdk_gicd_pidr4
{
    uint32_t u;
    struct bdk_gicd_pidr4_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t cnt_4k                : 4;  /**< [  7:  4](RO) This field is 0x4, indicating a 64 KB software-visible page. */
        uint32_t continuation_code     : 4;  /**< [  3:  0](RO) JEP106 continuation code, least significant nibble. Indicates Cavium. */
#else /* Word 0 - Little Endian */
        uint32_t continuation_code     : 4;  /**< [  3:  0](RO) JEP106 continuation code, least significant nibble. Indicates Cavium. */
        uint32_t cnt_4k                : 4;  /**< [  7:  4](RO) This field is 0x4, indicating a 64 KB software-visible page. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr4_s cn; */
};
typedef union bdk_gicd_pidr4 bdk_gicd_pidr4_t;

#define BDK_GICD_PIDR4 BDK_GICD_PIDR4_FUNC()
static inline uint64_t BDK_GICD_PIDR4_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR4_FUNC(void)
{
    return 0x80100000ffd0ll;
}

#define typedef_BDK_GICD_PIDR4 bdk_gicd_pidr4_t
#define bustype_BDK_GICD_PIDR4 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR4 "GICD_PIDR4"
#define device_bar_BDK_GICD_PIDR4 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR4 0
#define arguments_BDK_GICD_PIDR4 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr5
 *
 * GIC Distributor Peripheral Identification Register 5
 */
union bdk_gicd_pidr5
{
    uint32_t u;
    struct bdk_gicd_pidr5_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr5_s cn; */
};
typedef union bdk_gicd_pidr5 bdk_gicd_pidr5_t;

#define BDK_GICD_PIDR5 BDK_GICD_PIDR5_FUNC()
static inline uint64_t BDK_GICD_PIDR5_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR5_FUNC(void)
{
    return 0x80100000ffd4ll;
}

#define typedef_BDK_GICD_PIDR5 bdk_gicd_pidr5_t
#define bustype_BDK_GICD_PIDR5 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR5 "GICD_PIDR5"
#define device_bar_BDK_GICD_PIDR5 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR5 0
#define arguments_BDK_GICD_PIDR5 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr6
 *
 * GIC Distributor Peripheral Identification Register 6
 */
union bdk_gicd_pidr6
{
    uint32_t u;
    struct bdk_gicd_pidr6_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr6_s cn; */
};
typedef union bdk_gicd_pidr6 bdk_gicd_pidr6_t;

#define BDK_GICD_PIDR6 BDK_GICD_PIDR6_FUNC()
static inline uint64_t BDK_GICD_PIDR6_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR6_FUNC(void)
{
    return 0x80100000ffd8ll;
}

#define typedef_BDK_GICD_PIDR6 bdk_gicd_pidr6_t
#define bustype_BDK_GICD_PIDR6 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR6 "GICD_PIDR6"
#define device_bar_BDK_GICD_PIDR6 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR6 0
#define arguments_BDK_GICD_PIDR6 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_pidr7
 *
 * GIC Distributor Peripheral Identification Register 7
 */
union bdk_gicd_pidr7
{
    uint32_t u;
    struct bdk_gicd_pidr7_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_pidr7_s cn; */
};
typedef union bdk_gicd_pidr7 bdk_gicd_pidr7_t;

#define BDK_GICD_PIDR7 BDK_GICD_PIDR7_FUNC()
static inline uint64_t BDK_GICD_PIDR7_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_PIDR7_FUNC(void)
{
    return 0x80100000ffdcll;
}

#define typedef_BDK_GICD_PIDR7 bdk_gicd_pidr7_t
#define bustype_BDK_GICD_PIDR7 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_PIDR7 "GICD_PIDR7"
#define device_bar_BDK_GICD_PIDR7 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_PIDR7 0
#define arguments_BDK_GICD_PIDR7 -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_sctlr
 *
 * GIC Distributor (Secure) Control Register
 * Controls the behavior of the distributor.
 */
union bdk_gicd_sctlr
{
    uint32_t u;
    struct bdk_gicd_sctlr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t rwp                   : 1;  /**< [ 31: 31](RO/H) Register write pending.
                                                                 Indicates whether a register write is in progress.
                                                                 0 = The effects of all register writes are visible to all descendants of the top-level
                                                                 redistributor, including processors.
                                                                 1 = The effects of all register writes are not visible to all descendants of the top-level
                                                                 redistributor.

                                                                 Note: this field tracks completion of writes to GICD_(S)CTLR that change the state of an
                                                                 interrupt group enable or an affinity routing setting and writes to GICD_ICENABLER() that
                                                                 clear the enable of one or more SPIs. */
        uint32_t reserved_7_30         : 24;
        uint32_t ds                    : 1;  /**< [  6:  6](SR/W) Disable security.
                                                                 When set, nonsecure accesses are permitted to access and modify registers that control
                                                                 group 0 interrupts.
                                                                 If [DS] becomes one when [ARE_SNS] is one, then ARE for the single security state is
                                                                 RAO/WI.

                                                                 When [DS] is set, all accesses to GICD_(S)CTLR access the single security state view
                                                                 (below) and all bits are accessible.

                                                                 This bit is RAO/WI if the distributor only supports a single security state (see
                                                                 below).

                                                                 Once set, [DS] may only be clear by a hardware reset. */
        uint32_t are_ns                : 1;  /**< [  5:  5](SRO) Enable affinity routing for the nonsecure state when set.
                                                                 In CNXXXX this bit is always one as only affinity routing is supported.

                                                                 Note: this bit is RAO/WI when ARE is one for the secure state. */
        uint32_t are_sns               : 1;  /**< [  4:  4](RO) Enables affinity routing for the nonsecure state.
                                                                 This field is fixed as RAO/WI for CNXXXX for both secure and non secure state. */
        uint32_t reserved_3            : 1;
        uint32_t enable_g1s            : 1;  /**< [  2:  2](SR/W) Enables secure group 1 interrupts.
                                                                 0 = Disable G1S interrupts.
                                                                 1 = Enable G1S interrupts. */
        uint32_t enable_g1ns           : 1;  /**< [  1:  1](R/W) S - Enables nonsecure group 1 interrupts. Behaves as defined for GICv2. This
                                                                 enable also controls whether LPIs are forwarded to processors. When written
                                                                 to zero, [RWP] indicates whether the effects of this enable on LPIs
                                                                 have been made visible.

                                                                 NS - This field is called ENABLE_G1A. It enables nonsecure group 1 interrupts. */
        uint32_t enable_g0             : 1;  /**< [  0:  0](SR/W) Secure view or [DS] is set -- Enable/disable group 0 interrupts.
                                                                 0 = Disable G0 interrupts.
                                                                 1 = Enable G0 interrupts.

                                                                 Nonsecure view -- RES0 for CNXXXX since [ARE_NS] is RAO. */
#else /* Word 0 - Little Endian */
        uint32_t enable_g0             : 1;  /**< [  0:  0](SR/W) Secure view or [DS] is set -- Enable/disable group 0 interrupts.
                                                                 0 = Disable G0 interrupts.
                                                                 1 = Enable G0 interrupts.

                                                                 Nonsecure view -- RES0 for CNXXXX since [ARE_NS] is RAO. */
        uint32_t enable_g1ns           : 1;  /**< [  1:  1](R/W) S - Enables nonsecure group 1 interrupts. Behaves as defined for GICv2. This
                                                                 enable also controls whether LPIs are forwarded to processors. When written
                                                                 to zero, [RWP] indicates whether the effects of this enable on LPIs
                                                                 have been made visible.

                                                                 NS - This field is called ENABLE_G1A. It enables nonsecure group 1 interrupts. */
        uint32_t enable_g1s            : 1;  /**< [  2:  2](SR/W) Enables secure group 1 interrupts.
                                                                 0 = Disable G1S interrupts.
                                                                 1 = Enable G1S interrupts. */
        uint32_t reserved_3            : 1;
        uint32_t are_sns               : 1;  /**< [  4:  4](RO) Enables affinity routing for the nonsecure state.
                                                                 This field is fixed as RAO/WI for CNXXXX for both secure and non secure state. */
        uint32_t are_ns                : 1;  /**< [  5:  5](SRO) Enable affinity routing for the nonsecure state when set.
                                                                 In CNXXXX this bit is always one as only affinity routing is supported.

                                                                 Note: this bit is RAO/WI when ARE is one for the secure state. */
        uint32_t ds                    : 1;  /**< [  6:  6](SR/W) Disable security.
                                                                 When set, nonsecure accesses are permitted to access and modify registers that control
                                                                 group 0 interrupts.
                                                                 If [DS] becomes one when [ARE_SNS] is one, then ARE for the single security state is
                                                                 RAO/WI.

                                                                 When [DS] is set, all accesses to GICD_(S)CTLR access the single security state view
                                                                 (below) and all bits are accessible.

                                                                 This bit is RAO/WI if the distributor only supports a single security state (see
                                                                 below).

                                                                 Once set, [DS] may only be clear by a hardware reset. */
        uint32_t reserved_7_30         : 24;
        uint32_t rwp                   : 1;  /**< [ 31: 31](RO/H) Register write pending.
                                                                 Indicates whether a register write is in progress.
                                                                 0 = The effects of all register writes are visible to all descendants of the top-level
                                                                 redistributor, including processors.
                                                                 1 = The effects of all register writes are not visible to all descendants of the top-level
                                                                 redistributor.

                                                                 Note: this field tracks completion of writes to GICD_(S)CTLR that change the state of an
                                                                 interrupt group enable or an affinity routing setting and writes to GICD_ICENABLER() that
                                                                 clear the enable of one or more SPIs. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_sctlr_s cn; */
};
typedef union bdk_gicd_sctlr bdk_gicd_sctlr_t;

#define BDK_GICD_SCTLR BDK_GICD_SCTLR_FUNC()
static inline uint64_t BDK_GICD_SCTLR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_SCTLR_FUNC(void)
{
    return 0x801000000000ll;
}

#define typedef_BDK_GICD_SCTLR bdk_gicd_sctlr_t
#define bustype_BDK_GICD_SCTLR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_SCTLR "GICD_SCTLR"
#define device_bar_BDK_GICD_SCTLR 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_SCTLR 0
#define arguments_BDK_GICD_SCTLR -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_setspi_nsr
 *
 * GIC Distributor Set SPI Pending Register
 */
union bdk_gicd_setspi_nsr
{
    uint32_t u;
    struct bdk_gicd_setspi_nsr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_10_31        : 22;
        uint32_t spi_id                : 10; /**< [  9:  0](WO) Set an SPI to pending (write-only). If the SPI is already pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access and the value specifies a secure SPI
                                                                 and the value of the corresponding GICD_NSACR() register is zero (i.e. does not permit
                                                                 nonsecure accesses to set the interrupt as pending), the write has no effect. */
#else /* Word 0 - Little Endian */
        uint32_t spi_id                : 10; /**< [  9:  0](WO) Set an SPI to pending (write-only). If the SPI is already pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access and the value specifies a secure SPI
                                                                 and the value of the corresponding GICD_NSACR() register is zero (i.e. does not permit
                                                                 nonsecure accesses to set the interrupt as pending), the write has no effect. */
        uint32_t reserved_10_31        : 22;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_setspi_nsr_s cn; */
};
typedef union bdk_gicd_setspi_nsr bdk_gicd_setspi_nsr_t;

#define BDK_GICD_SETSPI_NSR BDK_GICD_SETSPI_NSR_FUNC()
static inline uint64_t BDK_GICD_SETSPI_NSR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_SETSPI_NSR_FUNC(void)
{
    return 0x801000000040ll;
}

#define typedef_BDK_GICD_SETSPI_NSR bdk_gicd_setspi_nsr_t
#define bustype_BDK_GICD_SETSPI_NSR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_SETSPI_NSR "GICD_SETSPI_NSR"
#define device_bar_BDK_GICD_SETSPI_NSR 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_SETSPI_NSR 0
#define arguments_BDK_GICD_SETSPI_NSR -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_setspi_sr
 *
 * GIC Distributor Set Secure SPI Pending Register
 */
union bdk_gicd_setspi_sr
{
    uint32_t u;
    struct bdk_gicd_setspi_sr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_10_31        : 22;
        uint32_t spi_id                : 10; /**< [  9:  0](SWO) Set an SPI to pending (write-only). If the SPI is already pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access, the write has no effect. */
#else /* Word 0 - Little Endian */
        uint32_t spi_id                : 10; /**< [  9:  0](SWO) Set an SPI to pending (write-only). If the SPI is already pending, then the write has no
                                                                 effect.

                                                                 If the SPI ID is invalid, then the write has no effect.

                                                                 If the register is written using a nonsecure access, the write has no effect. */
        uint32_t reserved_10_31        : 22;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_setspi_sr_s cn; */
};
typedef union bdk_gicd_setspi_sr bdk_gicd_setspi_sr_t;

#define BDK_GICD_SETSPI_SR BDK_GICD_SETSPI_SR_FUNC()
static inline uint64_t BDK_GICD_SETSPI_SR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_SETSPI_SR_FUNC(void)
{
    return 0x801000000050ll;
}

#define typedef_BDK_GICD_SETSPI_SR bdk_gicd_setspi_sr_t
#define bustype_BDK_GICD_SETSPI_SR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_SETSPI_SR "GICD_SETSPI_SR"
#define device_bar_BDK_GICD_SETSPI_SR 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_SETSPI_SR 0
#define arguments_BDK_GICD_SETSPI_SR -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_sstatusr
 *
 * GIC Distributor (Secure) Status Register
 */
union bdk_gicd_sstatusr
{
    uint32_t u;
    struct bdk_gicd_sstatusr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_4_31         : 28;
        uint32_t wrod                  : 1;  /**< [  3:  3](R/W) This bit is set if a write to a read-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rwod                  : 1;  /**< [  2:  2](R/W) This bit is set if a read to a write-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t wrd                   : 1;  /**< [  1:  1](R/W) This bit is set if a write to a reserved location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rrd                   : 1;  /**< [  0:  0](R/W) This bit is set if a read to a reserved location is detected. Software must write a one to
                                                                 this bit to clear it. */
#else /* Word 0 - Little Endian */
        uint32_t rrd                   : 1;  /**< [  0:  0](R/W) This bit is set if a read to a reserved location is detected. Software must write a one to
                                                                 this bit to clear it. */
        uint32_t wrd                   : 1;  /**< [  1:  1](R/W) This bit is set if a write to a reserved location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rwod                  : 1;  /**< [  2:  2](R/W) This bit is set if a read to a write-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t wrod                  : 1;  /**< [  3:  3](R/W) This bit is set if a write to a read-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t reserved_4_31         : 28;
#endif /* Word 0 - End */
    } s;
    struct bdk_gicd_sstatusr_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_4_31         : 28;
        uint32_t wrod                  : 1;  /**< [  3:  3](R/W/H) This bit is set if a write to a read-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rwod                  : 1;  /**< [  2:  2](R/W/H) This bit is set if a read to a write-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t wrd                   : 1;  /**< [  1:  1](R/W/H) This bit is set if a write to a reserved location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rrd                   : 1;  /**< [  0:  0](R/W/H) This bit is set if a read to a reserved location is detected. Software must write a one to
                                                                 this bit to clear it. */
#else /* Word 0 - Little Endian */
        uint32_t rrd                   : 1;  /**< [  0:  0](R/W/H) This bit is set if a read to a reserved location is detected. Software must write a one to
                                                                 this bit to clear it. */
        uint32_t wrd                   : 1;  /**< [  1:  1](R/W/H) This bit is set if a write to a reserved location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rwod                  : 1;  /**< [  2:  2](R/W/H) This bit is set if a read to a write-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t wrod                  : 1;  /**< [  3:  3](R/W/H) This bit is set if a write to a read-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t reserved_4_31         : 28;
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicd_sstatusr_cn9 cn81xx; */
    /* struct bdk_gicd_sstatusr_s cn88xx; */
    /* struct bdk_gicd_sstatusr_cn9 cn83xx; */
};
typedef union bdk_gicd_sstatusr bdk_gicd_sstatusr_t;

#define BDK_GICD_SSTATUSR BDK_GICD_SSTATUSR_FUNC()
static inline uint64_t BDK_GICD_SSTATUSR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_SSTATUSR_FUNC(void)
{
    return 0x801000000010ll;
}

#define typedef_BDK_GICD_SSTATUSR bdk_gicd_sstatusr_t
#define bustype_BDK_GICD_SSTATUSR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_SSTATUSR "GICD_SSTATUSR"
#define device_bar_BDK_GICD_SSTATUSR 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_SSTATUSR 0
#define arguments_BDK_GICD_SSTATUSR -1,-1,-1,-1

/**
 * Register (NCB32b) gicd_typer
 *
 * GIC Distributor Type Register
 * Describes features supported by the distributor.
 */
union bdk_gicd_typer
{
    uint32_t u;
    struct bdk_gicd_typer_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_25_31        : 7;
        uint32_t a3v                   : 1;  /**< [ 24: 24](RO) Indicates whether the distributor supports nonzero values of affinity 3. */
        uint32_t idbits                : 5;  /**< [ 23: 19](RO) The number of interrupt identifier bits supported by the GIC stream protocol interface minus one. */
        uint32_t dvis                  : 1;  /**< [ 18: 18](RO) Direct virtual LPI injection supported. */
        uint32_t lpis                  : 1;  /**< [ 17: 17](RO) Locality-specific peripheral interrupt supported. */
        uint32_t mbis                  : 1;  /**< [ 16: 16](RO) Message based interrupt supported. */
        uint32_t lspi                  : 5;  /**< [ 15: 11](RO) The number of lockable SPI interrupts. This is not supported in GICv3 and is RES0. */
        uint32_t securityextn          : 1;  /**< [ 10: 10](RO) Security extension supported. When GICD_(S)CTLR[DS] is
                                                                 set, this field is clear. */
        uint32_t reserved_8_9          : 2;
        uint32_t cpunumber             : 3;  /**< [  7:  5](RO) Reserved. In CNXXXX implementation, not used. */
        uint32_t itlinesnumber         : 5;  /**< [  4:  0](RO) The value derived from this specifies the maximum number of SPIs. */
#else /* Word 0 - Little Endian */
        uint32_t itlinesnumber         : 5;  /**< [  4:  0](RO) The value derived from this specifies the maximum number of SPIs. */
        uint32_t cpunumber             : 3;  /**< [  7:  5](RO) Reserved. In CNXXXX implementation, not used. */
        uint32_t reserved_8_9          : 2;
        uint32_t securityextn          : 1;  /**< [ 10: 10](RO) Security extension supported. When GICD_(S)CTLR[DS] is
                                                                 set, this field is clear. */
        uint32_t lspi                  : 5;  /**< [ 15: 11](RO) The number of lockable SPI interrupts. This is not supported in GICv3 and is RES0. */
        uint32_t mbis                  : 1;  /**< [ 16: 16](RO) Message based interrupt supported. */
        uint32_t lpis                  : 1;  /**< [ 17: 17](RO) Locality-specific peripheral interrupt supported. */
        uint32_t dvis                  : 1;  /**< [ 18: 18](RO) Direct virtual LPI injection supported. */
        uint32_t idbits                : 5;  /**< [ 23: 19](RO) The number of interrupt identifier bits supported by the GIC stream protocol interface minus one. */
        uint32_t a3v                   : 1;  /**< [ 24: 24](RO) Indicates whether the distributor supports nonzero values of affinity 3. */
        uint32_t reserved_25_31        : 7;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicd_typer_s cn; */
};
typedef union bdk_gicd_typer bdk_gicd_typer_t;

#define BDK_GICD_TYPER BDK_GICD_TYPER_FUNC()
static inline uint64_t BDK_GICD_TYPER_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICD_TYPER_FUNC(void)
{
    return 0x801000000004ll;
}

#define typedef_BDK_GICD_TYPER bdk_gicd_typer_t
#define bustype_BDK_GICD_TYPER BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICD_TYPER "GICD_TYPER"
#define device_bar_BDK_GICD_TYPER 0x0 /* PF_BAR0 */
#define busnum_BDK_GICD_TYPER 0
#define arguments_BDK_GICD_TYPER -1,-1,-1,-1

/**
 * Register (NCB32b) gicr#_cidr0
 *
 * GIC Redistributor Component Identification Register 0
 */
union bdk_gicrx_cidr0
{
    uint32_t u;
    struct bdk_gicrx_cidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_cidr0_s cn; */
};
typedef union bdk_gicrx_cidr0 bdk_gicrx_cidr0_t;

static inline uint64_t BDK_GICRX_CIDR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_CIDR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000fff0ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000fff0ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000fff0ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000fff0ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_CIDR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_CIDR0(a) bdk_gicrx_cidr0_t
#define bustype_BDK_GICRX_CIDR0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_CIDR0(a) "GICRX_CIDR0"
#define device_bar_BDK_GICRX_CIDR0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_CIDR0(a) (a)
#define arguments_BDK_GICRX_CIDR0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_cidr1
 *
 * GIC Redistributor Component Identification Register 1
 */
union bdk_gicrx_cidr1
{
    uint32_t u;
    struct bdk_gicrx_cidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_cidr1_s cn; */
};
typedef union bdk_gicrx_cidr1 bdk_gicrx_cidr1_t;

static inline uint64_t BDK_GICRX_CIDR1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_CIDR1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000fff4ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000fff4ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000fff4ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000fff4ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_CIDR1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_CIDR1(a) bdk_gicrx_cidr1_t
#define bustype_BDK_GICRX_CIDR1(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_CIDR1(a) "GICRX_CIDR1"
#define device_bar_BDK_GICRX_CIDR1(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_CIDR1(a) (a)
#define arguments_BDK_GICRX_CIDR1(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_cidr2
 *
 * GIC Redistributor Component Identification Register 2
 */
union bdk_gicrx_cidr2
{
    uint32_t u;
    struct bdk_gicrx_cidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_cidr2_s cn; */
};
typedef union bdk_gicrx_cidr2 bdk_gicrx_cidr2_t;

static inline uint64_t BDK_GICRX_CIDR2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_CIDR2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000fff8ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000fff8ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000fff8ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000fff8ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_CIDR2", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_CIDR2(a) bdk_gicrx_cidr2_t
#define bustype_BDK_GICRX_CIDR2(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_CIDR2(a) "GICRX_CIDR2"
#define device_bar_BDK_GICRX_CIDR2(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_CIDR2(a) (a)
#define arguments_BDK_GICRX_CIDR2(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_cidr3
 *
 * GIC Redistributor Component Identification Register 3
 */
union bdk_gicrx_cidr3
{
    uint32_t u;
    struct bdk_gicrx_cidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_cidr3_s cn; */
};
typedef union bdk_gicrx_cidr3 bdk_gicrx_cidr3_t;

static inline uint64_t BDK_GICRX_CIDR3(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_CIDR3(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000fffcll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000fffcll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000fffcll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000fffcll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_CIDR3", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_CIDR3(a) bdk_gicrx_cidr3_t
#define bustype_BDK_GICRX_CIDR3(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_CIDR3(a) "GICRX_CIDR3"
#define device_bar_BDK_GICRX_CIDR3(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_CIDR3(a) (a)
#define arguments_BDK_GICRX_CIDR3(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_clrlpir
 *
 * GIC Redistributor Clear LPI Register
 */
union bdk_gicrx_clrlpir
{
    uint64_t u;
    struct bdk_gicrx_clrlpir_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical ID of the LPI to be set as not pending. If the LPI is already not pending, the
                                                                 write has no effect.
                                                                 If the LPI with the physical ID is not implemented, the write has no effect.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
#else /* Word 0 - Little Endian */
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical ID of the LPI to be set as not pending. If the LPI is already not pending, the
                                                                 write has no effect.
                                                                 If the LPI with the physical ID is not implemented, the write has no effect.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_clrlpir_s cn; */
};
typedef union bdk_gicrx_clrlpir bdk_gicrx_clrlpir_t;

static inline uint64_t BDK_GICRX_CLRLPIR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_CLRLPIR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000048ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000048ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000048ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000048ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_CLRLPIR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_CLRLPIR(a) bdk_gicrx_clrlpir_t
#define bustype_BDK_GICRX_CLRLPIR(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_CLRLPIR(a) "GICRX_CLRLPIR"
#define device_bar_BDK_GICRX_CLRLPIR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_CLRLPIR(a) (a)
#define arguments_BDK_GICRX_CLRLPIR(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_icactiver0
 *
 * GIC Redistributor Interrupt Clear-Active Register 0
 * Each bit in GICR()_ICACTIVER0 provides a clear-active bit for an SGI or a
 * PPI. Writing one to a clear-active bit clears the active status of the corresponding
 * interrupt.
 */
union bdk_gicrx_icactiver0
{
    uint32_t u;
    struct bdk_gicrx_icactiver0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not active. If read as one, the interrupt is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not active. If read as one, the interrupt is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_icactiver0_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not active. If read as one, the interrupt is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not active. If read as one, the interrupt is in active state.

                                                                 Clear-active bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 A clear-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicrx_icactiver0_cn9 cn81xx; */
    /* struct bdk_gicrx_icactiver0_s cn88xx; */
    /* struct bdk_gicrx_icactiver0_cn9 cn83xx; */
};
typedef union bdk_gicrx_icactiver0 bdk_gicrx_icactiver0_t;

static inline uint64_t BDK_GICRX_ICACTIVER0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ICACTIVER0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010380ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010380ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010380ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010380ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ICACTIVER0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ICACTIVER0(a) bdk_gicrx_icactiver0_t
#define bustype_BDK_GICRX_ICACTIVER0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ICACTIVER0(a) "GICRX_ICACTIVER0"
#define device_bar_BDK_GICRX_ICACTIVER0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ICACTIVER0(a) (a)
#define arguments_BDK_GICRX_ICACTIVER0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_icenabler0
 *
 * GIC Redistributor Interrupt Clear-Enable Register 0
 * Each bit in GICR()_ICENABLER0 provides a clear-enable bit for an SGI or a PPI. Writing one to
 * a
 * clear-enable bit disables forwarding of the corresponding SGI or PPI from the redistributor
 * to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled.
 */
union bdk_gicrx_icenabler0
{
    uint32_t u;
    struct bdk_gicrx_icenabler0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. Upon reading,
                                                                 if a bit is zero, then the interrupt is not enabled to be forwarded to the CPU interface.
                                                                 Upon reading, if a bit is one, the SPI is enabled to be forwarded to the CPU interface.

                                                                 Clear-enable bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 Writes to the register cannot be considered complete until the effects of the write are
                                                                 visible throughout the affinity hierarchy. To ensure that an enable has been cleared,
                                                                 software must write to this register with bits set to clear the required enables. Software
                                                                 must then poll GICR()_(S)CTLR[RWP] (register writes pending) until it has the value zero. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. Upon reading,
                                                                 if a bit is zero, then the interrupt is not enabled to be forwarded to the CPU interface.
                                                                 Upon reading, if a bit is one, the SPI is enabled to be forwarded to the CPU interface.

                                                                 Clear-enable bits corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses.

                                                                 Writes to the register cannot be considered complete until the effects of the write are
                                                                 visible throughout the affinity hierarchy. To ensure that an enable has been cleared,
                                                                 software must write to this register with bits set to clear the required enables. Software
                                                                 must then poll GICR()_(S)CTLR[RWP] (register writes pending) until it has the value zero. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_icenabler0_s cn; */
};
typedef union bdk_gicrx_icenabler0 bdk_gicrx_icenabler0_t;

static inline uint64_t BDK_GICRX_ICENABLER0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ICENABLER0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010180ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010180ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010180ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010180ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ICENABLER0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ICENABLER0(a) bdk_gicrx_icenabler0_t
#define bustype_BDK_GICRX_ICENABLER0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ICENABLER0(a) "GICRX_ICENABLER0"
#define device_bar_BDK_GICRX_ICENABLER0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ICENABLER0(a) (a)
#define arguments_BDK_GICRX_ICENABLER0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_icfgr0
 *
 * GIC Redistributor Interrupt Configuration Register 0
 */
union bdk_gicrx_icfgr0
{
    uint32_t u;
    struct bdk_gicrx_icfgr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](RO) Two bits per SGI. Defines whether an SGI is level-sensitive or edge-triggered.
                                                                 Note SGIs are always edge-triggered, so Bit[1] for an SGI is RAO and read-only.

                                                                 Bit[1] is zero, the interrupt is level-sensitive.

                                                                 Bit[1] is one, the interrupt is edge-triggered.

                                                                 Bit[0] Reserved.

                                                                 If a secure interrupt, then its corresponding field is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](RO) Two bits per SGI. Defines whether an SGI is level-sensitive or edge-triggered.
                                                                 Note SGIs are always edge-triggered, so Bit[1] for an SGI is RAO and read-only.

                                                                 Bit[1] is zero, the interrupt is level-sensitive.

                                                                 Bit[1] is one, the interrupt is edge-triggered.

                                                                 Bit[0] Reserved.

                                                                 If a secure interrupt, then its corresponding field is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_icfgr0_s cn; */
};
typedef union bdk_gicrx_icfgr0 bdk_gicrx_icfgr0_t;

static inline uint64_t BDK_GICRX_ICFGR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ICFGR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010c00ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010c00ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010c00ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010c00ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ICFGR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ICFGR0(a) bdk_gicrx_icfgr0_t
#define bustype_BDK_GICRX_ICFGR0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ICFGR0(a) "GICRX_ICFGR0"
#define device_bar_BDK_GICRX_ICFGR0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ICFGR0(a) (a)
#define arguments_BDK_GICRX_ICFGR0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_icfgr1
 *
 * GIC Redistributor Interrupt Configuration Register 1
 * Redistributor interrupt configuration register 1.
 */
union bdk_gicrx_icfgr1
{
    uint32_t u;
    struct bdk_gicrx_icfgr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](RO) Two bits per PPI. Defines whether an PPI is level-sensitive or edge-triggered.

                                                                 Bit[1] is zero, the interrupt is level-sensitive.

                                                                 Bit[1] is one, the interrupt is edge-triggered.

                                                                 Bit[0] Reserved.

                                                                 If a secure interrupt, then its corresponding field is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](RO) Two bits per PPI. Defines whether an PPI is level-sensitive or edge-triggered.

                                                                 Bit[1] is zero, the interrupt is level-sensitive.

                                                                 Bit[1] is one, the interrupt is edge-triggered.

                                                                 Bit[0] Reserved.

                                                                 If a secure interrupt, then its corresponding field is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_icfgr1_s cn; */
};
typedef union bdk_gicrx_icfgr1 bdk_gicrx_icfgr1_t;

static inline uint64_t BDK_GICRX_ICFGR1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ICFGR1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010c04ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010c04ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010c04ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010c04ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ICFGR1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ICFGR1(a) bdk_gicrx_icfgr1_t
#define bustype_BDK_GICRX_ICFGR1(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ICFGR1(a) "GICRX_ICFGR1"
#define device_bar_BDK_GICRX_ICFGR1(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ICFGR1(a) (a)
#define arguments_BDK_GICRX_ICFGR1(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_icpendr0
 *
 * GIC Redistributor Interrupt Clear-Pending Register 0
 * Each bit in GICR()_ICPENDR0 provides a clear-pending bit for an SGI or a PPI. Writing one to a
 * clear-pending bit clears the pending status of the corresponding interrupt.
 */
union bdk_gicrx_icpendr0
{
    uint32_t u;
    struct bdk_gicrx_icpendr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not pending. If read as one, the interrupt is in pending
                                                                 state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not pending. If read as one, the interrupt is in pending
                                                                 state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_icpendr0_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not pending. If read as one, the interrupt is in pending
                                                                 state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1C/H) Each bit corresponds to an SGI or a PPI for interrupt IDs in the range 31..0. If read as
                                                                 zero, then the interrupt is not pending. If read as one, the interrupt is in pending
                                                                 state.

                                                                 Clear-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A clear-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicrx_icpendr0_cn9 cn81xx; */
    /* struct bdk_gicrx_icpendr0_s cn88xx; */
    /* struct bdk_gicrx_icpendr0_cn9 cn83xx; */
};
typedef union bdk_gicrx_icpendr0 bdk_gicrx_icpendr0_t;

static inline uint64_t BDK_GICRX_ICPENDR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ICPENDR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010280ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010280ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010280ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010280ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ICPENDR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ICPENDR0(a) bdk_gicrx_icpendr0_t
#define bustype_BDK_GICRX_ICPENDR0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ICPENDR0(a) "GICRX_ICPENDR0"
#define device_bar_BDK_GICRX_ICPENDR0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ICPENDR0(a) (a)
#define arguments_BDK_GICRX_ICPENDR0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_igroupr0
 *
 * GIC Redistributor Interrupt Group Secure Register
 */
union bdk_gicrx_igroupr0
{
    uint32_t u;
    struct bdk_gicrx_igroupr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t ppi                   : 16; /**< [ 31: 16](SR/W) Groups for PPIs.
                                                                 0 = Group 0.
                                                                 1 = Group 1. */
        uint32_t sgi                   : 16; /**< [ 15:  0](SR/W) Groups for SGIs.
                                                                 0 = Group 0.
                                                                 1 = Group 1. */
#else /* Word 0 - Little Endian */
        uint32_t sgi                   : 16; /**< [ 15:  0](SR/W) Groups for SGIs.
                                                                 0 = Group 0.
                                                                 1 = Group 1. */
        uint32_t ppi                   : 16; /**< [ 31: 16](SR/W) Groups for PPIs.
                                                                 0 = Group 0.
                                                                 1 = Group 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_igroupr0_s cn; */
};
typedef union bdk_gicrx_igroupr0 bdk_gicrx_igroupr0_t;

static inline uint64_t BDK_GICRX_IGROUPR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_IGROUPR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010080ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010080ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010080ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010080ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_IGROUPR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_IGROUPR0(a) bdk_gicrx_igroupr0_t
#define bustype_BDK_GICRX_IGROUPR0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_IGROUPR0(a) "GICRX_IGROUPR0"
#define device_bar_BDK_GICRX_IGROUPR0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_IGROUPR0(a) (a)
#define arguments_BDK_GICRX_IGROUPR0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_igrpmodr0
 *
 * GIC Redistributor Interrupt Group Secure Register
 * Control the group modifier for PPIs and SGIs, similar to GICD_IGRPMODR() for SPIs.
 */
union bdk_gicrx_igrpmodr0
{
    uint32_t u;
    struct bdk_gicrx_igrpmodr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t ppi                   : 16; /**< [ 31: 16](SR/W) Group modifiers for PPIs.
                                                                 0 = No group modification.
                                                                 1 = Modify to group 1. */
        uint32_t sgi                   : 16; /**< [ 15:  0](SR/W) Group modifiers for SGIs.
                                                                 0 = No group modification.
                                                                 1 = Modify to group 1. */
#else /* Word 0 - Little Endian */
        uint32_t sgi                   : 16; /**< [ 15:  0](SR/W) Group modifiers for SGIs.
                                                                 0 = No group modification.
                                                                 1 = Modify to group 1. */
        uint32_t ppi                   : 16; /**< [ 31: 16](SR/W) Group modifiers for PPIs.
                                                                 0 = No group modification.
                                                                 1 = Modify to group 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_igrpmodr0_s cn; */
};
typedef union bdk_gicrx_igrpmodr0 bdk_gicrx_igrpmodr0_t;

static inline uint64_t BDK_GICRX_IGRPMODR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_IGRPMODR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010d00ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010d00ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010d00ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010d00ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_IGRPMODR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_IGRPMODR0(a) bdk_gicrx_igrpmodr0_t
#define bustype_BDK_GICRX_IGRPMODR0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_IGRPMODR0(a) "GICRX_IGRPMODR0"
#define device_bar_BDK_GICRX_IGRPMODR0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_IGRPMODR0(a) (a)
#define arguments_BDK_GICRX_IGRPMODR0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_iidr
 *
 * GIC Redistributor Implementation Identification Register
 * This 32-bit register is read-only and specifies the version and features supported by the
 * redistributor.
 */
union bdk_gicrx_iidr
{
    uint32_t u;
    struct bdk_gicrx_iidr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t productid             : 8;  /**< [ 31: 24](RO) An implementation defined product number for the device.
                                                                 In CNXXXX, enumerated by PCC_PROD_E. */
        uint32_t reserved_20_23        : 4;
        uint32_t variant               : 4;  /**< [ 19: 16](RO) Indicates the major revision or variant of the product.
                                                                 On CNXXXX, this is the major revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t revision              : 4;  /**< [ 15: 12](RO) Indicates the minor revision of the product.
                                                                 On CNXXXX, this is the minor revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t implementer           : 12; /**< [ 11:  0](RO) Indicates the implementer:
                                                                    0x34C = Cavium. */
#else /* Word 0 - Little Endian */
        uint32_t implementer           : 12; /**< [ 11:  0](RO) Indicates the implementer:
                                                                    0x34C = Cavium. */
        uint32_t revision              : 4;  /**< [ 15: 12](RO) Indicates the minor revision of the product.
                                                                 On CNXXXX, this is the minor revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t variant               : 4;  /**< [ 19: 16](RO) Indicates the major revision or variant of the product.
                                                                 On CNXXXX, this is the major revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t reserved_20_23        : 4;
        uint32_t productid             : 8;  /**< [ 31: 24](RO) An implementation defined product number for the device.
                                                                 In CNXXXX, enumerated by PCC_PROD_E. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_iidr_s cn; */
};
typedef union bdk_gicrx_iidr bdk_gicrx_iidr_t;

static inline uint64_t BDK_GICRX_IIDR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_IIDR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000004ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000004ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000004ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000004ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_IIDR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_IIDR(a) bdk_gicrx_iidr_t
#define bustype_BDK_GICRX_IIDR(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_IIDR(a) "GICRX_IIDR"
#define device_bar_BDK_GICRX_IIDR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_IIDR(a) (a)
#define arguments_BDK_GICRX_IIDR(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_invallr
 *
 * GIC Redistributor LPI Invalidate All Register
 * This register is write-only and causes the LPI configuration to be reloaded from the table in
 * memory.
 */
union bdk_gicrx_invallr
{
    uint64_t u;
    struct bdk_gicrx_invallr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_invallr_s cn; */
};
typedef union bdk_gicrx_invallr bdk_gicrx_invallr_t;

static inline uint64_t BDK_GICRX_INVALLR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_INVALLR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x8010800000b0ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x8010800000b0ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x8010800000b0ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x8010800000b0ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_INVALLR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_INVALLR(a) bdk_gicrx_invallr_t
#define bustype_BDK_GICRX_INVALLR(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_INVALLR(a) "GICRX_INVALLR"
#define device_bar_BDK_GICRX_INVALLR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_INVALLR(a) (a)
#define arguments_BDK_GICRX_INVALLR(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_invlpir
 *
 * GIC Redistributor Invalidate LPI Register
 */
union bdk_gicrx_invlpir
{
    uint64_t u;
    struct bdk_gicrx_invlpir_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical LPI ID to be cleaned. The invalidate in the register name and the ITS command is
                                                                 a misnomer. This actually results in a clean operation wherein the cached (in the
                                                                 redistributor) pending state of the LPI is updated to the pending table held in memory and
                                                                 its cached configuration is invalidated in the cache. */
#else /* Word 0 - Little Endian */
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical LPI ID to be cleaned. The invalidate in the register name and the ITS command is
                                                                 a misnomer. This actually results in a clean operation wherein the cached (in the
                                                                 redistributor) pending state of the LPI is updated to the pending table held in memory and
                                                                 its cached configuration is invalidated in the cache. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_invlpir_s cn; */
};
typedef union bdk_gicrx_invlpir bdk_gicrx_invlpir_t;

static inline uint64_t BDK_GICRX_INVLPIR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_INVLPIR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x8010800000a0ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x8010800000a0ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x8010800000a0ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x8010800000a0ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_INVLPIR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_INVLPIR(a) bdk_gicrx_invlpir_t
#define bustype_BDK_GICRX_INVLPIR(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_INVLPIR(a) "GICRX_INVLPIR"
#define device_bar_BDK_GICRX_INVLPIR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_INVLPIR(a) (a)
#define arguments_BDK_GICRX_INVLPIR(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_ipriorityr#
 *
 * GIC Redistributor Interrupt Priority Registers
 * Each byte in this register provides a priority field for each SGI or PPI supported by the
 * GIC.
 */
union bdk_gicrx_ipriorityrx
{
    uint32_t u;
    struct bdk_gicrx_ipriorityrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W) Each byte corresponds to an SGI or PPI for interrupt IDs in the range 31..0.

                                                                 Priority fields corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses, or when GICD_(S)CTLR[DS] is one.

                                                                 Byte accesses are permitted to these registers.

                                                                 A priority field for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W) Each byte corresponds to an SGI or PPI for interrupt IDs in the range 31..0.

                                                                 Priority fields corresponding to secure interrupts (either group 0 or group 1)
                                                                 may only be set by secure accesses, or when GICD_(S)CTLR[DS] is one.

                                                                 Byte accesses are permitted to these registers.

                                                                 A priority field for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_ipriorityrx_s cn; */
};
typedef union bdk_gicrx_ipriorityrx bdk_gicrx_ipriorityrx_t;

static inline uint64_t BDK_GICRX_IPRIORITYRX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_IPRIORITYRX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=7)))
        return 0x801080010400ll + 0x20000ll * ((a) & 0x3) + 4ll * ((b) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=23) && (b<=7)))
        return 0x801080010400ll + 0x20000ll * ((a) & 0x1f) + 4ll * ((b) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=47) && (b<=7)))
        return 0x801080010400ll + 0x20000ll * ((a) & 0x3f) + 4ll * ((b) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=23) && (b<=7)))
        return 0x801080010400ll + 0x20000ll * ((a) & 0x1f) + 4ll * ((b) & 0x7);
    __bdk_csr_fatal("GICRX_IPRIORITYRX", 2, a, b, 0, 0);
}

#define typedef_BDK_GICRX_IPRIORITYRX(a,b) bdk_gicrx_ipriorityrx_t
#define bustype_BDK_GICRX_IPRIORITYRX(a,b) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_IPRIORITYRX(a,b) "GICRX_IPRIORITYRX"
#define device_bar_BDK_GICRX_IPRIORITYRX(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_IPRIORITYRX(a,b) (a)
#define arguments_BDK_GICRX_IPRIORITYRX(a,b) (a),(b),-1,-1

/**
 * Register (NCB32b) gicr#_isactiver0
 *
 * GIC Redistributor Interrupt Set-Active Register 0
 * Each bit in GICR()_ISACTIVER0 provides a set-active bit for an SGI or a PPI. Writing one to a
 * set-active bit sets the status of the corresponding interrupt to active.
 */
union bdk_gicrx_isactiver0
{
    uint32_t u;
    struct bdk_gicrx_isactiver0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not active. If read as one, the interrupt is
                                                                 in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1) may only be
                                                                 set by secure accesses.

                                                                 A set-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not active. If read as one, the interrupt is
                                                                 in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1) may only be
                                                                 set by secure accesses.

                                                                 A set-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_isactiver0_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not active. If read as one, the interrupt is
                                                                 in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1) may only be
                                                                 set by secure accesses.

                                                                 A set-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not active. If read as one, the interrupt is
                                                                 in active state.

                                                                 Set-active bits corresponding to secure interrupts (either group 0 or group 1) may only be
                                                                 set by secure accesses.

                                                                 A set-active bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicrx_isactiver0_cn9 cn81xx; */
    /* struct bdk_gicrx_isactiver0_s cn88xx; */
    /* struct bdk_gicrx_isactiver0_cn9 cn83xx; */
};
typedef union bdk_gicrx_isactiver0 bdk_gicrx_isactiver0_t;

static inline uint64_t BDK_GICRX_ISACTIVER0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ISACTIVER0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010300ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010300ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010300ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010300ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ISACTIVER0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ISACTIVER0(a) bdk_gicrx_isactiver0_t
#define bustype_BDK_GICRX_ISACTIVER0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ISACTIVER0(a) "GICRX_ISACTIVER0"
#define device_bar_BDK_GICRX_ISACTIVER0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ISACTIVER0(a) (a)
#define arguments_BDK_GICRX_ISACTIVER0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_isenabler0
 *
 * GIC Redistributor Interrupt Set-Enable Register 0
 * Each bit in GICR()_ISENABLER0 provides a set-enable bit for an SGI or a PPI. Writing one
 * to a set-enable bit enables forwarding of the corresponding SGI or PPI from the
 * redistributor to the CPU interfaces.
 */
union bdk_gicrx_isenabler0
{
    uint32_t u;
    struct bdk_gicrx_isenabler0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 zero, then the interrupt is not enabled to be forwarded to the CPU interface. If
                                                                 one, the interrupt is enabled to be forwarded to the CPU interface. Set-enable
                                                                 bits corresponding to secure interrupts (either group0 or group1) may only be
                                                                 set by secure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 zero, then the interrupt is not enabled to be forwarded to the CPU interface. If
                                                                 one, the interrupt is enabled to be forwarded to the CPU interface. Set-enable
                                                                 bits corresponding to secure interrupts (either group0 or group1) may only be
                                                                 set by secure accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_isenabler0_s cn; */
};
typedef union bdk_gicrx_isenabler0 bdk_gicrx_isenabler0_t;

static inline uint64_t BDK_GICRX_ISENABLER0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ISENABLER0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010100ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010100ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010100ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010100ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ISENABLER0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ISENABLER0(a) bdk_gicrx_isenabler0_t
#define bustype_BDK_GICRX_ISENABLER0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ISENABLER0(a) "GICRX_ISENABLER0"
#define device_bar_BDK_GICRX_ISENABLER0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ISENABLER0(a) (a)
#define arguments_BDK_GICRX_ISENABLER0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_ispendr0
 *
 * GIC Redistributor Interrupt Set-Pending Register 0
 * Each bit in GICR()_ISPENDR0 provides a set-pending bit for an SGI or a PPI. Writing one
 * to a set-pending bit sets the status of the corresponding interrupt to pending.
 */
union bdk_gicrx_ispendr0
{
    uint32_t u;
    struct bdk_gicrx_ispendr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not pending. If read as one, the interrupt
                                                                 is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A set-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not pending. If read as one, the interrupt
                                                                 is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A set-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_ispendr0_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not pending. If read as one, the interrupt
                                                                 is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A set-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](R/W1S/H) Each bit corresponds to an SGI or PPI for interrupt IDs in the range 31..0. If
                                                                 read as zero, then the interrupt is not pending. If read as one, the interrupt
                                                                 is in pending state.

                                                                 Set-pending bits corresponding to secure interrupts (either group 0 or group 1) may only
                                                                 be set by secure accesses.

                                                                 A set-pending bit for a secure interrupt is RAZ/WI to nonsecure accesses. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicrx_ispendr0_cn9 cn81xx; */
    /* struct bdk_gicrx_ispendr0_s cn88xx; */
    /* struct bdk_gicrx_ispendr0_cn9 cn83xx; */
};
typedef union bdk_gicrx_ispendr0 bdk_gicrx_ispendr0_t;

static inline uint64_t BDK_GICRX_ISPENDR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_ISPENDR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010200ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010200ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010200ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010200ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_ISPENDR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_ISPENDR0(a) bdk_gicrx_ispendr0_t
#define bustype_BDK_GICRX_ISPENDR0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_ISPENDR0(a) "GICRX_ISPENDR0"
#define device_bar_BDK_GICRX_ISPENDR0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_ISPENDR0(a) (a)
#define arguments_BDK_GICRX_ISPENDR0(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_movallr
 *
 * GIC Redistributor LPI Move All Register
 * This register is write-only and causes the LPI configuration to be reloaded from the table in
 * memory.
 */
union bdk_gicrx_movallr
{
    uint64_t u;
    struct bdk_gicrx_movallr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pa                    : 32; /**< [ 63: 32](WO) Target address \<47:16\>. Base address of the redistributor to which pending LPIs are to be
                                                                 moved.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
        uint64_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_31         : 32;
        uint64_t pa                    : 32; /**< [ 63: 32](WO) Target address \<47:16\>. Base address of the redistributor to which pending LPIs are to be
                                                                 moved.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_movallr_s cn; */
};
typedef union bdk_gicrx_movallr bdk_gicrx_movallr_t;

static inline uint64_t BDK_GICRX_MOVALLR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_MOVALLR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000110ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000110ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000110ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000110ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_MOVALLR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_MOVALLR(a) bdk_gicrx_movallr_t
#define bustype_BDK_GICRX_MOVALLR(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_MOVALLR(a) "GICRX_MOVALLR"
#define device_bar_BDK_GICRX_MOVALLR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_MOVALLR(a) (a)
#define arguments_BDK_GICRX_MOVALLR(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_movlpir
 *
 * GIC Redistributor Move LPI Register
 */
union bdk_gicrx_movlpir
{
    uint64_t u;
    struct bdk_gicrx_movlpir_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pa                    : 32; /**< [ 63: 32](WO) Target address \<47:16\>. Base address of the redistributor to which the physical LPI is to
                                                                 be moved. */
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical LPI ID to be moved to the redistributor at [PA]. If the LPI with this
                                                                 PID is unimplemented, the write has no effect.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
#else /* Word 0 - Little Endian */
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical LPI ID to be moved to the redistributor at [PA]. If the LPI with this
                                                                 PID is unimplemented, the write has no effect.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
        uint64_t pa                    : 32; /**< [ 63: 32](WO) Target address \<47:16\>. Base address of the redistributor to which the physical LPI is to
                                                                 be moved. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_movlpir_s cn; */
};
typedef union bdk_gicrx_movlpir bdk_gicrx_movlpir_t;

static inline uint64_t BDK_GICRX_MOVLPIR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_MOVLPIR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000100ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000100ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000100ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000100ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_MOVLPIR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_MOVLPIR(a) bdk_gicrx_movlpir_t
#define bustype_BDK_GICRX_MOVLPIR(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_MOVLPIR(a) "GICRX_MOVLPIR"
#define device_bar_BDK_GICRX_MOVLPIR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_MOVLPIR(a) (a)
#define arguments_BDK_GICRX_MOVLPIR(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_nsacr
 *
 * GIC Redistributor Non-Secure Access Control Secure Registers
 */
union bdk_gicrx_nsacr
{
    uint32_t u;
    struct bdk_gicrx_nsacr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Two bits per SGI or PPI. Defines whether nonsecure access is permitted to secure
                                                                 interrupt resources.
                                                                 0x0 = No nonsecure access is permitted to fields associated with the corresponding
                                                                 interrupt.
                                                                 0x1 = Nonsecure write access is permitted to generate secure group0 interrupts.
                                                                 0x2 = Adds nonsecure write access permissions to generate secure group1 interrupts.
                                                                 0x3 = Reserved. Treated as 0x1.

                                                                 This register is RAZ/WI for nonsecure accesses.

                                                                 When GICD_(S)CTLR[DS] is one, this register is RAZ/WI. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SR/W) Two bits per SGI or PPI. Defines whether nonsecure access is permitted to secure
                                                                 interrupt resources.
                                                                 0x0 = No nonsecure access is permitted to fields associated with the corresponding
                                                                 interrupt.
                                                                 0x1 = Nonsecure write access is permitted to generate secure group0 interrupts.
                                                                 0x2 = Adds nonsecure write access permissions to generate secure group1 interrupts.
                                                                 0x3 = Reserved. Treated as 0x1.

                                                                 This register is RAZ/WI for nonsecure accesses.

                                                                 When GICD_(S)CTLR[DS] is one, this register is RAZ/WI. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_nsacr_s cn; */
};
typedef union bdk_gicrx_nsacr bdk_gicrx_nsacr_t;

static inline uint64_t BDK_GICRX_NSACR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_NSACR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080010e00ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080010e00ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080010e00ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080010e00ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_NSACR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_NSACR(a) bdk_gicrx_nsacr_t
#define bustype_BDK_GICRX_NSACR(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_NSACR(a) "GICRX_NSACR"
#define device_bar_BDK_GICRX_NSACR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_NSACR(a) (a)
#define arguments_BDK_GICRX_NSACR(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_pendbaser
 *
 * GIC Redistributor LPI Pending Table Address Register
 */
union bdk_gicrx_pendbaser
{
    uint64_t u;
    struct bdk_gicrx_pendbaser_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_63           : 1;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_59_61        : 3;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_52_55        : 4;
        uint64_t pa                    : 36; /**< [ 51: 16](R/W) Physical address bits \<46:16\> for the LPI pending table. */
        uint64_t reserved_12_15        : 4;
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t reserved_12_15        : 4;
        uint64_t pa                    : 36; /**< [ 51: 16](R/W) Physical address bits \<46:16\> for the LPI pending table. */
        uint64_t reserved_52_55        : 4;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_59_61        : 3;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_63           : 1;
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_pendbaser_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_63           : 1;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_48_61        : 14;
        uint64_t pa                    : 32; /**< [ 47: 16](R/W) Physical address bits \<46:16\> for the LPI pending table. */
        uint64_t reserved_0_15         : 16;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_15         : 16;
        uint64_t pa                    : 32; /**< [ 47: 16](R/W) Physical address bits \<46:16\> for the LPI pending table. */
        uint64_t reserved_48_61        : 14;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_63           : 1;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_gicrx_pendbaser_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_63           : 1;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_59_61        : 3;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_52_55        : 4;
        uint64_t pa                    : 36; /**< [ 51: 16](R/W) Physical address bits \<51:16\> for the LPI pending table.
                                                                 Software must set bits \<51:46\> and \<43\> to zero. */
        uint64_t reserved_12_15        : 4;
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t reserved_12_15        : 4;
        uint64_t pa                    : 36; /**< [ 51: 16](R/W) Physical address bits \<51:16\> for the LPI pending table.
                                                                 Software must set bits \<51:46\> and \<43\> to zero. */
        uint64_t reserved_52_55        : 4;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_59_61        : 3;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_63           : 1;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_gicrx_pendbaser_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_63           : 1;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_59_61        : 3;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_48_55        : 8;
        uint64_t pa                    : 32; /**< [ 47: 16](R/W) Physical address bits \<46:16\> for the LPI pending table. */
        uint64_t reserved_12_15        : 4;
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t reserved_12_15        : 4;
        uint64_t pa                    : 32; /**< [ 47: 16](R/W) Physical address bits \<46:16\> for the LPI pending table. */
        uint64_t reserved_48_55        : 8;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_59_61        : 3;
        uint64_t pending_table_zero    : 1;  /**< [ 62: 62](WO) Pending zero:
                                                                 0 = The coarse-grained map for the LPI pending table is valid.
                                                                 1 = The pending table has been zeroed out. */
        uint64_t reserved_63           : 1;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gicrx_pendbaser_cn81xx cn83xx; */
    /* struct bdk_gicrx_pendbaser_cn81xx cn88xxp2; */
};
typedef union bdk_gicrx_pendbaser bdk_gicrx_pendbaser_t;

static inline uint64_t BDK_GICRX_PENDBASER(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PENDBASER(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000078ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000078ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000078ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000078ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PENDBASER", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PENDBASER(a) bdk_gicrx_pendbaser_t
#define bustype_BDK_GICRX_PENDBASER(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_PENDBASER(a) "GICRX_PENDBASER"
#define device_bar_BDK_GICRX_PENDBASER(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PENDBASER(a) (a)
#define arguments_BDK_GICRX_PENDBASER(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr0
 *
 * GIC Redistributor Peripheral Identification Register 0
 */
union bdk_gicrx_pidr0
{
    uint32_t u;
    struct bdk_gicrx_pidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  Indicates PCC_PIDR_PARTNUM0_E::GICR. */
#else /* Word 0 - Little Endian */
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  Indicates PCC_PIDR_PARTNUM0_E::GICR. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr0_s cn; */
};
typedef union bdk_gicrx_pidr0 bdk_gicrx_pidr0_t;

static inline uint64_t BDK_GICRX_PIDR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffe0ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffe0ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffe0ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffe0ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR0(a) bdk_gicrx_pidr0_t
#define bustype_BDK_GICRX_PIDR0(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR0(a) "GICRX_PIDR0"
#define device_bar_BDK_GICRX_PIDR0(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR0(a) (a)
#define arguments_BDK_GICRX_PIDR0(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr1
 *
 * GIC Redistributor Peripheral Identification Register 1
 */
union bdk_gicrx_pidr1
{
    uint32_t u;
    struct bdk_gicrx_pidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t idcode                : 4;  /**< [  7:  4](RO) JEP106 identification code \<3:0\>. Cavium code is 0x4C. */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  Indicates PCC_PIDR_PARTNUM1_E::COMP. */
#else /* Word 0 - Little Endian */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  Indicates PCC_PIDR_PARTNUM1_E::COMP. */
        uint32_t idcode                : 4;  /**< [  7:  4](RO) JEP106 identification code \<3:0\>. Cavium code is 0x4C. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr1_s cn; */
};
typedef union bdk_gicrx_pidr1 bdk_gicrx_pidr1_t;

static inline uint64_t BDK_GICRX_PIDR1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffe4ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffe4ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffe4ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffe4ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR1(a) bdk_gicrx_pidr1_t
#define bustype_BDK_GICRX_PIDR1(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR1(a) "GICRX_PIDR1"
#define device_bar_BDK_GICRX_PIDR1(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR1(a) (a)
#define arguments_BDK_GICRX_PIDR1(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr2
 *
 * GIC Redistributor Peripheral Identification Register 2
 */
union bdk_gicrx_pidr2
{
    uint32_t u;
    struct bdk_gicrx_pidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t archrev               : 4;  /**< [  7:  4](RO) Architectural revision:
                                                                   0x1 = GICv1.
                                                                   0x2 = GICV2.
                                                                   0x3 = GICv3.
                                                                   0x4 = GICv4.
                                                                   0x5-0xF = Reserved. */
        uint32_t usesjepcode           : 1;  /**< [  3:  3](RO) JEDEC assigned. */
        uint32_t jepid                 : 3;  /**< [  2:  0](RO) JEP106 identification code \<6:4\>. Cavium code is 0x4C. */
#else /* Word 0 - Little Endian */
        uint32_t jepid                 : 3;  /**< [  2:  0](RO) JEP106 identification code \<6:4\>. Cavium code is 0x4C. */
        uint32_t usesjepcode           : 1;  /**< [  3:  3](RO) JEDEC assigned. */
        uint32_t archrev               : 4;  /**< [  7:  4](RO) Architectural revision:
                                                                   0x1 = GICv1.
                                                                   0x2 = GICV2.
                                                                   0x3 = GICv3.
                                                                   0x4 = GICv4.
                                                                   0x5-0xF = Reserved. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr2_s cn; */
};
typedef union bdk_gicrx_pidr2 bdk_gicrx_pidr2_t;

static inline uint64_t BDK_GICRX_PIDR2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffe8ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffe8ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffe8ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffe8ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR2", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR2(a) bdk_gicrx_pidr2_t
#define bustype_BDK_GICRX_PIDR2(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR2(a) "GICRX_PIDR2"
#define device_bar_BDK_GICRX_PIDR2(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR2(a) (a)
#define arguments_BDK_GICRX_PIDR2(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr3
 *
 * GIC Redistributor Peripheral Identification Register 3
 */
union bdk_gicrx_pidr3
{
    uint32_t u;
    struct bdk_gicrx_pidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t cmod                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
#else /* Word 0 - Little Endian */
        uint32_t cmod                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr3_s cn; */
};
typedef union bdk_gicrx_pidr3 bdk_gicrx_pidr3_t;

static inline uint64_t BDK_GICRX_PIDR3(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR3(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffecll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffecll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffecll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffecll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR3", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR3(a) bdk_gicrx_pidr3_t
#define bustype_BDK_GICRX_PIDR3(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR3(a) "GICRX_PIDR3"
#define device_bar_BDK_GICRX_PIDR3(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR3(a) (a)
#define arguments_BDK_GICRX_PIDR3(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr4
 *
 * GIC Redistributor Peripheral Identification Register 4
 */
union bdk_gicrx_pidr4
{
    uint32_t u;
    struct bdk_gicrx_pidr4_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t cnt_4k                : 4;  /**< [  7:  4](RO) This field is 0x4, indicating a 64 KB software-visible page. */
        uint32_t continuation_code     : 4;  /**< [  3:  0](RO) 0x3 = Cavium JEP106 continuation code. */
#else /* Word 0 - Little Endian */
        uint32_t continuation_code     : 4;  /**< [  3:  0](RO) 0x3 = Cavium JEP106 continuation code. */
        uint32_t cnt_4k                : 4;  /**< [  7:  4](RO) This field is 0x4, indicating a 64 KB software-visible page. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr4_s cn; */
};
typedef union bdk_gicrx_pidr4 bdk_gicrx_pidr4_t;

static inline uint64_t BDK_GICRX_PIDR4(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR4(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffd0ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffd0ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffd0ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffd0ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR4", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR4(a) bdk_gicrx_pidr4_t
#define bustype_BDK_GICRX_PIDR4(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR4(a) "GICRX_PIDR4"
#define device_bar_BDK_GICRX_PIDR4(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR4(a) (a)
#define arguments_BDK_GICRX_PIDR4(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr5
 *
 * GIC Redistributor Peripheral Identification Register 5
 */
union bdk_gicrx_pidr5
{
    uint32_t u;
    struct bdk_gicrx_pidr5_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr5_s cn; */
};
typedef union bdk_gicrx_pidr5 bdk_gicrx_pidr5_t;

static inline uint64_t BDK_GICRX_PIDR5(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR5(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffd4ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffd4ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffd4ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffd4ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR5", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR5(a) bdk_gicrx_pidr5_t
#define bustype_BDK_GICRX_PIDR5(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR5(a) "GICRX_PIDR5"
#define device_bar_BDK_GICRX_PIDR5(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR5(a) (a)
#define arguments_BDK_GICRX_PIDR5(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr6
 *
 * GIC Redistributor Peripheral Identification Register 6
 */
union bdk_gicrx_pidr6
{
    uint32_t u;
    struct bdk_gicrx_pidr6_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr6_s cn; */
};
typedef union bdk_gicrx_pidr6 bdk_gicrx_pidr6_t;

static inline uint64_t BDK_GICRX_PIDR6(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR6(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffd8ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffd8ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffd8ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffd8ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR6", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR6(a) bdk_gicrx_pidr6_t
#define bustype_BDK_GICRX_PIDR6(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR6(a) "GICRX_PIDR6"
#define device_bar_BDK_GICRX_PIDR6(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR6(a) (a)
#define arguments_BDK_GICRX_PIDR6(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_pidr7
 *
 * GIC Redistributor Peripheral Identification Register 7
 */
union bdk_gicrx_pidr7
{
    uint32_t u;
    struct bdk_gicrx_pidr7_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_pidr7_s cn; */
};
typedef union bdk_gicrx_pidr7 bdk_gicrx_pidr7_t;

static inline uint64_t BDK_GICRX_PIDR7(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PIDR7(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000ffdcll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000ffdcll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000ffdcll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000ffdcll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PIDR7", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PIDR7(a) bdk_gicrx_pidr7_t
#define bustype_BDK_GICRX_PIDR7(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_PIDR7(a) "GICRX_PIDR7"
#define device_bar_BDK_GICRX_PIDR7(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PIDR7(a) (a)
#define arguments_BDK_GICRX_PIDR7(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_propbaser
 *
 * GIC Redistributor LPI Configuration Table Address Register
 */
union bdk_gicrx_propbaser
{
    uint64_t u;
    struct bdk_gicrx_propbaser_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_59_63        : 5;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_52_55        : 4;
        uint64_t pa                    : 40; /**< [ 51: 12](R/W) Physical address bits \<46:12\> for the LPI configuration table. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_5_6          : 2;
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
#else /* Word 0 - Little Endian */
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
        uint64_t reserved_5_6          : 2;
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t pa                    : 40; /**< [ 51: 12](R/W) Physical address bits \<46:12\> for the LPI configuration table. */
        uint64_t reserved_52_55        : 4;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_59_63        : 5;
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_propbaser_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t pa                    : 36; /**< [ 47: 12](R/W) Physical address bits \<46:12\> for the LPI configuration table. */
        uint64_t reserved_5_11         : 7;
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
#else /* Word 0 - Little Endian */
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
        uint64_t reserved_5_11         : 7;
        uint64_t pa                    : 36; /**< [ 47: 12](R/W) Physical address bits \<46:12\> for the LPI configuration table. */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_gicrx_propbaser_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_59_63        : 5;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_52_55        : 4;
        uint64_t pa                    : 40; /**< [ 51: 12](R/W) Physical address bits \<51:12\> for the LPI configuration table.
                                                                 Software must set bits \<51:46\> and \<43\> to zero. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_5_6          : 2;
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
#else /* Word 0 - Little Endian */
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
        uint64_t reserved_5_6          : 2;
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t pa                    : 40; /**< [ 51: 12](R/W) Physical address bits \<51:12\> for the LPI configuration table.
                                                                 Software must set bits \<51:46\> and \<43\> to zero. */
        uint64_t reserved_52_55        : 4;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_59_63        : 5;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_gicrx_propbaser_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_59_63        : 5;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_48_55        : 8;
        uint64_t pa                    : 36; /**< [ 47: 12](R/W) Physical address bits \<46:12\> for the LPI configuration table. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_5_6          : 2;
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
#else /* Word 0 - Little Endian */
        uint64_t num_bits              : 5;  /**< [  4:  0](R/W) The number of bits of LPI ID supported, minus one. If this value exceeds the value of
                                                                 GICD_TYPER[IDBITS], then the number of bits must be treated as the value defined by
                                                                 GICD_TYPER[IDBITS]. */
        uint64_t reserved_5_6          : 2;
        uint64_t cacheability          : 3;  /**< [  9:  7](R/W) Cacheability attributes. Ignored in CNXXXX. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attributes. Ignored in CNXXXX. */
        uint64_t pa                    : 36; /**< [ 47: 12](R/W) Physical address bits \<46:12\> for the LPI configuration table. */
        uint64_t reserved_48_55        : 8;
        uint64_t outer_cacheability    : 3;  /**< [ 58: 56](R/W) Outer cacheability attributes. Ignored in CNXXXX. */
        uint64_t reserved_59_63        : 5;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gicrx_propbaser_cn81xx cn83xx; */
    /* struct bdk_gicrx_propbaser_cn81xx cn88xxp2; */
};
typedef union bdk_gicrx_propbaser bdk_gicrx_propbaser_t;

static inline uint64_t BDK_GICRX_PROPBASER(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_PROPBASER(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000070ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000070ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000070ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000070ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_PROPBASER", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_PROPBASER(a) bdk_gicrx_propbaser_t
#define bustype_BDK_GICRX_PROPBASER(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_PROPBASER(a) "GICRX_PROPBASER"
#define device_bar_BDK_GICRX_PROPBASER(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_PROPBASER(a) (a)
#define arguments_BDK_GICRX_PROPBASER(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_sctlr
 *
 * GIC Redistributor (Secure) Control Register
 * This register controls the behavior of the nonsecure redistributor.
 */
union bdk_gicrx_sctlr
{
    uint32_t u;
    struct bdk_gicrx_sctlr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t uwp                   : 1;  /**< [ 31: 31](RO) Upstream write pending. Common to both security states. Read-only.
                                                                    0 = The effects of all upstream writes have been communicated to the parent
                                                                 redistributor, including any generate SGI packets.
                                                                    1 = The effects of all upstream writes have not been communicated to the parent
                                                                 redistributor, including any generate SGI packets. */
        uint32_t reserved_4_30         : 27;
        uint32_t rwp                   : 1;  /**< [  3:  3](RO) Register write pending. This bit indicates whether a register write for the current
                                                                 security state (banked) is in progress or not.
                                                                  0 = The effect of all register writes are visible to all descendants of the
                                                                 redistributor, including processors.
                                                                  1 = The effects of all register writes are not visible to all descendants of the
                                                                 redistributor.

                                                                 Note: this field tracks completion of writes to GICR()_ICENABLER0 that clear
                                                                 the enable of one or more interrupts. */
        uint32_t reserved_1_2          : 2;
        uint32_t enable_lpis           : 1;  /**< [  0:  0](R/W) Enable LPIs. Common to both security states. When this bit is clear,
                                                                 writes to generate physical LPIs to GICR()_SETLPIR will be ignored.
                                                                 When a write changes this bit from zero to one, this bit becomes RAO/WI and the
                                                                 redistributor must load the pending table from memory to check for any pending interrupts. */
#else /* Word 0 - Little Endian */
        uint32_t enable_lpis           : 1;  /**< [  0:  0](R/W) Enable LPIs. Common to both security states. When this bit is clear,
                                                                 writes to generate physical LPIs to GICR()_SETLPIR will be ignored.
                                                                 When a write changes this bit from zero to one, this bit becomes RAO/WI and the
                                                                 redistributor must load the pending table from memory to check for any pending interrupts. */
        uint32_t reserved_1_2          : 2;
        uint32_t rwp                   : 1;  /**< [  3:  3](RO) Register write pending. This bit indicates whether a register write for the current
                                                                 security state (banked) is in progress or not.
                                                                  0 = The effect of all register writes are visible to all descendants of the
                                                                 redistributor, including processors.
                                                                  1 = The effects of all register writes are not visible to all descendants of the
                                                                 redistributor.

                                                                 Note: this field tracks completion of writes to GICR()_ICENABLER0 that clear
                                                                 the enable of one or more interrupts. */
        uint32_t reserved_4_30         : 27;
        uint32_t uwp                   : 1;  /**< [ 31: 31](RO) Upstream write pending. Common to both security states. Read-only.
                                                                    0 = The effects of all upstream writes have been communicated to the parent
                                                                 redistributor, including any generate SGI packets.
                                                                    1 = The effects of all upstream writes have not been communicated to the parent
                                                                 redistributor, including any generate SGI packets. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_sctlr_s cn; */
};
typedef union bdk_gicrx_sctlr bdk_gicrx_sctlr_t;

static inline uint64_t BDK_GICRX_SCTLR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_SCTLR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000000ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000000ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000000ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000000ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_SCTLR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_SCTLR(a) bdk_gicrx_sctlr_t
#define bustype_BDK_GICRX_SCTLR(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_SCTLR(a) "GICRX_SCTLR"
#define device_bar_BDK_GICRX_SCTLR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_SCTLR(a) (a)
#define arguments_BDK_GICRX_SCTLR(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_seir
 *
 * GIC Redistributor Generate SEI Register
 */
union bdk_gicrx_seir
{
    uint32_t u;
    struct bdk_gicrx_seir_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_16_31        : 16;
        uint32_t syndrome              : 16; /**< [ 15:  0](WO) Syndrome value for the SEI to be generated. If another write to this register occurs
                                                                 before the previous has been forwarded to its recipients, the new value is ORed with the
                                                                 existing value. [SYNDROME] is sticky and indicates that at least one error of a
                                                                 class has occurred. */
#else /* Word 0 - Little Endian */
        uint32_t syndrome              : 16; /**< [ 15:  0](WO) Syndrome value for the SEI to be generated. If another write to this register occurs
                                                                 before the previous has been forwarded to its recipients, the new value is ORed with the
                                                                 existing value. [SYNDROME] is sticky and indicates that at least one error of a
                                                                 class has occurred. */
        uint32_t reserved_16_31        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_seir_s cn; */
};
typedef union bdk_gicrx_seir bdk_gicrx_seir_t;

static inline uint64_t BDK_GICRX_SEIR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_SEIR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000068ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000068ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000068ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000068ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_SEIR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_SEIR(a) bdk_gicrx_seir_t
#define bustype_BDK_GICRX_SEIR(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_SEIR(a) "GICRX_SEIR"
#define device_bar_BDK_GICRX_SEIR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_SEIR(a) (a)
#define arguments_BDK_GICRX_SEIR(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_setdel3tr_el1s
 *
 * GIC Redistributor Set Non-Maskable Interrupt Secure Registers
 */
union bdk_gicrx_setdel3tr_el1s
{
    uint32_t u;
    struct bdk_gicrx_setdel3tr_el1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SWO) These write-only secure registers are used to generate DEL3T interrupts to the APs.
                                                                 The value written into these registers is not used. There is no interrupt ID for DEL3Ts.
                                                                 Whenever a register in this set is written, the DEL3T signal of the AP being
                                                                 managed by that register is asserted.

                                                                 Each register in this set is RAZ/WI for nonsecure accesses. */
#else /* Word 0 - Little Endian */
        uint32_t vec                   : 32; /**< [ 31:  0](SWO) These write-only secure registers are used to generate DEL3T interrupts to the APs.
                                                                 The value written into these registers is not used. There is no interrupt ID for DEL3Ts.
                                                                 Whenever a register in this set is written, the DEL3T signal of the AP being
                                                                 managed by that register is asserted.

                                                                 Each register in this set is RAZ/WI for nonsecure accesses. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_setdel3tr_el1s_s cn; */
};
typedef union bdk_gicrx_setdel3tr_el1s bdk_gicrx_setdel3tr_el1s_t;

static inline uint64_t BDK_GICRX_SETDEL3TR_EL1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_SETDEL3TR_EL1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x80108000c000ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x80108000c000ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x80108000c000ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x80108000c000ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_SETDEL3TR_EL1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_SETDEL3TR_EL1S(a) bdk_gicrx_setdel3tr_el1s_t
#define bustype_BDK_GICRX_SETDEL3TR_EL1S(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_SETDEL3TR_EL1S(a) "GICRX_SETDEL3TR_EL1S"
#define device_bar_BDK_GICRX_SETDEL3TR_EL1S(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_SETDEL3TR_EL1S(a) (a)
#define arguments_BDK_GICRX_SETDEL3TR_EL1S(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_setlpir
 *
 * GIC Redistributor Set LPI Register
 */
union bdk_gicrx_setlpir
{
    uint64_t u;
    struct bdk_gicrx_setlpir_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical ID of the LPI to be generated. If the LPI is already pending, the write has no
                                                                 effect.
                                                                 If the LPI with the physical ID is not implemented, the write has no effect.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
#else /* Word 0 - Little Endian */
        uint64_t pid                   : 32; /**< [ 31:  0](WO) Physical ID of the LPI to be generated. If the LPI is already pending, the write has no
                                                                 effect.
                                                                 If the LPI with the physical ID is not implemented, the write has no effect.
                                                                 If GICR()_(S)CTLR[ENABLE_LPIS] is zero, the write has no effect. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_setlpir_s cn; */
};
typedef union bdk_gicrx_setlpir bdk_gicrx_setlpir_t;

static inline uint64_t BDK_GICRX_SETLPIR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_SETLPIR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000040ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000040ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000040ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000040ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_SETLPIR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_SETLPIR(a) bdk_gicrx_setlpir_t
#define bustype_BDK_GICRX_SETLPIR(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_SETLPIR(a) "GICRX_SETLPIR"
#define device_bar_BDK_GICRX_SETLPIR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_SETLPIR(a) (a)
#define arguments_BDK_GICRX_SETLPIR(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_sstatusr
 *
 * GIC Redistributor (Secure) Status Register
 */
union bdk_gicrx_sstatusr
{
    uint32_t u;
    struct bdk_gicrx_sstatusr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_4_31         : 28;
        uint32_t wrod                  : 1;  /**< [  3:  3](R/W1C/H) This bit is set if a write to a read-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rwod                  : 1;  /**< [  2:  2](R/W1C/H) This bit is set if a read to a write-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t wrd                   : 1;  /**< [  1:  1](R/W1C/H) This bit is set if a write to a reserved location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rrd                   : 1;  /**< [  0:  0](R/W1C/H) This bit is set if a read to a reserved location is detected. Software must write a one to
                                                                 this bit to clear it. */
#else /* Word 0 - Little Endian */
        uint32_t rrd                   : 1;  /**< [  0:  0](R/W1C/H) This bit is set if a read to a reserved location is detected. Software must write a one to
                                                                 this bit to clear it. */
        uint32_t wrd                   : 1;  /**< [  1:  1](R/W1C/H) This bit is set if a write to a reserved location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t rwod                  : 1;  /**< [  2:  2](R/W1C/H) This bit is set if a read to a write-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t wrod                  : 1;  /**< [  3:  3](R/W1C/H) This bit is set if a write to a read-only location is detected. Software must write a one
                                                                 to this bit to clear it. */
        uint32_t reserved_4_31         : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_sstatusr_s cn; */
};
typedef union bdk_gicrx_sstatusr bdk_gicrx_sstatusr_t;

static inline uint64_t BDK_GICRX_SSTATUSR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_SSTATUSR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000010ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000010ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000010ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000010ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_SSTATUSR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_SSTATUSR(a) bdk_gicrx_sstatusr_t
#define bustype_BDK_GICRX_SSTATUSR(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_SSTATUSR(a) "GICRX_SSTATUSR"
#define device_bar_BDK_GICRX_SSTATUSR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_SSTATUSR(a) (a)
#define arguments_BDK_GICRX_SSTATUSR(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_syncr
 *
 * GIC Redistributor Sync Register
 */
union bdk_gicrx_syncr
{
    uint32_t u;
    struct bdk_gicrx_syncr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_1_31         : 31;
        uint32_t busy                  : 1;  /**< [  0:  0](RO) Reserved. When this register is read, it will only return read-data with [BUSY] as zero when
                                                                 none of the following operations are in progress:
                                                                 * Any writes to GICR()_CLRLPIR within the redistributor.
                                                                 * Any writes to GICR()_MOVLPIR within the redistributor.
                                                                 * Any writes to GICR()_MOVALLR within the redistributor.
                                                                 * Any writes to GICR()_INVLPIR within the redistributor.
                                                                 * Any writes to GICR()_INVALLR within the redistributor.
                                                                 * Any writes to another redistributor performed as a result of a previous write to
                                                                 GICR()_MOVLPIR or GICR()_MOVALLR have completed and arrived at the target redistributor.
                                                                 Including operations initiated by writing to GICR()_PENDBASER or GICR()_PROPBASER. */
#else /* Word 0 - Little Endian */
        uint32_t busy                  : 1;  /**< [  0:  0](RO) Reserved. When this register is read, it will only return read-data with [BUSY] as zero when
                                                                 none of the following operations are in progress:
                                                                 * Any writes to GICR()_CLRLPIR within the redistributor.
                                                                 * Any writes to GICR()_MOVLPIR within the redistributor.
                                                                 * Any writes to GICR()_MOVALLR within the redistributor.
                                                                 * Any writes to GICR()_INVLPIR within the redistributor.
                                                                 * Any writes to GICR()_INVALLR within the redistributor.
                                                                 * Any writes to another redistributor performed as a result of a previous write to
                                                                 GICR()_MOVLPIR or GICR()_MOVALLR have completed and arrived at the target redistributor.
                                                                 Including operations initiated by writing to GICR()_PENDBASER or GICR()_PROPBASER. */
        uint32_t reserved_1_31         : 31;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gicrx_syncr_s cn; */
};
typedef union bdk_gicrx_syncr bdk_gicrx_syncr_t;

static inline uint64_t BDK_GICRX_SYNCR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_SYNCR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x8010800000c0ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x8010800000c0ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x8010800000c0ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x8010800000c0ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_SYNCR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_SYNCR(a) bdk_gicrx_syncr_t
#define bustype_BDK_GICRX_SYNCR(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_SYNCR(a) "GICRX_SYNCR"
#define device_bar_BDK_GICRX_SYNCR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_SYNCR(a) (a)
#define arguments_BDK_GICRX_SYNCR(a) (a),-1,-1,-1

/**
 * Register (NCB) gicr#_typer
 *
 * GIC Redistributor Type Register
 * This 64-bit read-only register is used to discover the properties of the redistributor and is
 * always accessible regardless of the ARE setting for a security state.
 */
union bdk_gicrx_typer
{
    uint64_t u;
    struct bdk_gicrx_typer_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t a3                    : 8;  /**< [ 63: 56](RO) The affinity level 3 value for the redistributor. */
        uint64_t a2                    : 8;  /**< [ 55: 48](RO/H) The affinity level 2 value for the redistributor. */
        uint64_t a1                    : 8;  /**< [ 47: 40](RO/H) The affinity level 1 value for the redistributor. */
        uint64_t a0                    : 8;  /**< [ 39: 32](RO/H) The affinity level 0 value for the redistributor. */
        uint64_t reserved_26_31        : 6;
        uint64_t commonlpiaff          : 2;  /**< [ 25: 24](RAZ) The affinity level at which re-distributors share a LPI configuration table.
                                                                 0x0 = All re-distributors must share a config table.
                                                                 0x1 = All re-distributors with the same Aff3 value must share a LPI configuration table.
                                                                 0x2 = All re-distributors with the same Aff3.Aff2 value must share a LPI configuration.
                                                                 table.
                                                                 0x3 = All re-distributors with the same Aff3.Aff2.Aff1 value must share an LPI
                                                                 configuration table. */
        uint64_t pn                    : 16; /**< [ 23:  8](RO/H) The processor number, a unique identifier for the processor understood by the ITS. Should
                                                                 be the logical processor number supported by the redistributor, which is the redistributor
                                                                 ID, ie. the variable a. */
        uint64_t reserved_6_7          : 2;
        uint64_t dpgs                  : 1;  /**< [  5:  5](RAZ) GICR()_(S)CTLR[DPG*] bits are NOT supported. */
        uint64_t last                  : 1;  /**< [  4:  4](RO/H) Last. This bit is only set for the last redistributor in a set of contiguous redistributor
                                                                 register pages. Needs to be determined from fuse signals or SKU. */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed implementation:
                                                                 0 = Monolithic implementation.
                                                                 1 = Distributed implementation registers supported. */
        uint64_t reserved_1_2          : 2;
        uint64_t plpis                 : 1;  /**< [  0:  0](RO) Physical LPIs supported:
                                                                 0 = Physical LPIs not supported.
                                                                 1 = Physical LPIs supported. */
#else /* Word 0 - Little Endian */
        uint64_t plpis                 : 1;  /**< [  0:  0](RO) Physical LPIs supported:
                                                                 0 = Physical LPIs not supported.
                                                                 1 = Physical LPIs supported. */
        uint64_t reserved_1_2          : 2;
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed implementation:
                                                                 0 = Monolithic implementation.
                                                                 1 = Distributed implementation registers supported. */
        uint64_t last                  : 1;  /**< [  4:  4](RO/H) Last. This bit is only set for the last redistributor in a set of contiguous redistributor
                                                                 register pages. Needs to be determined from fuse signals or SKU. */
        uint64_t dpgs                  : 1;  /**< [  5:  5](RAZ) GICR()_(S)CTLR[DPG*] bits are NOT supported. */
        uint64_t reserved_6_7          : 2;
        uint64_t pn                    : 16; /**< [ 23:  8](RO/H) The processor number, a unique identifier for the processor understood by the ITS. Should
                                                                 be the logical processor number supported by the redistributor, which is the redistributor
                                                                 ID, ie. the variable a. */
        uint64_t commonlpiaff          : 2;  /**< [ 25: 24](RAZ) The affinity level at which re-distributors share a LPI configuration table.
                                                                 0x0 = All re-distributors must share a config table.
                                                                 0x1 = All re-distributors with the same Aff3 value must share a LPI configuration table.
                                                                 0x2 = All re-distributors with the same Aff3.Aff2 value must share a LPI configuration.
                                                                 table.
                                                                 0x3 = All re-distributors with the same Aff3.Aff2.Aff1 value must share an LPI
                                                                 configuration table. */
        uint64_t reserved_26_31        : 6;
        uint64_t a0                    : 8;  /**< [ 39: 32](RO/H) The affinity level 0 value for the redistributor. */
        uint64_t a1                    : 8;  /**< [ 47: 40](RO/H) The affinity level 1 value for the redistributor. */
        uint64_t a2                    : 8;  /**< [ 55: 48](RO/H) The affinity level 2 value for the redistributor. */
        uint64_t a3                    : 8;  /**< [ 63: 56](RO) The affinity level 3 value for the redistributor. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_typer_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t a3                    : 8;  /**< [ 63: 56](RO) The affinity level 3 value for the redistributor. */
        uint64_t a2                    : 8;  /**< [ 55: 48](RO/H) The affinity level 2 value for the redistributor. */
        uint64_t a1                    : 8;  /**< [ 47: 40](RO/H) The affinity level 1 value for the redistributor. */
        uint64_t a0                    : 8;  /**< [ 39: 32](RO/H) The affinity level 0 value for the redistributor. */
        uint64_t reserved_24_31        : 8;
        uint64_t pn                    : 16; /**< [ 23:  8](RO/H) The processor number, a unique identifier for the processor understood by the ITS. Should
                                                                 be the logical processor number supported by the redistributor, which is the redistributor
                                                                 ID, ie. the variable a. */
        uint64_t reserved_6_7          : 2;
        uint64_t dpgs                  : 1;  /**< [  5:  5](RAZ) GICR()_(S)CTLR[DPG*] bits are NOT supported. */
        uint64_t last                  : 1;  /**< [  4:  4](RO/H) Last. This bit is only set for the last redistributor in a set of contiguous redistributor
                                                                 register pages. Needs to be determined from fuse signals or SKU. */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed implementation:
                                                                 0 = Monolithic implementation.
                                                                 1 = Distributed implementation registers supported. */
        uint64_t reserved_1_2          : 2;
        uint64_t plpis                 : 1;  /**< [  0:  0](RO) Physical LPIs supported:
                                                                 0 = Physical LPIs not supported.
                                                                 1 = Physical LPIs supported. */
#else /* Word 0 - Little Endian */
        uint64_t plpis                 : 1;  /**< [  0:  0](RO) Physical LPIs supported:
                                                                 0 = Physical LPIs not supported.
                                                                 1 = Physical LPIs supported. */
        uint64_t reserved_1_2          : 2;
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed implementation:
                                                                 0 = Monolithic implementation.
                                                                 1 = Distributed implementation registers supported. */
        uint64_t last                  : 1;  /**< [  4:  4](RO/H) Last. This bit is only set for the last redistributor in a set of contiguous redistributor
                                                                 register pages. Needs to be determined from fuse signals or SKU. */
        uint64_t dpgs                  : 1;  /**< [  5:  5](RAZ) GICR()_(S)CTLR[DPG*] bits are NOT supported. */
        uint64_t reserved_6_7          : 2;
        uint64_t pn                    : 16; /**< [ 23:  8](RO/H) The processor number, a unique identifier for the processor understood by the ITS. Should
                                                                 be the logical processor number supported by the redistributor, which is the redistributor
                                                                 ID, ie. the variable a. */
        uint64_t reserved_24_31        : 8;
        uint64_t a0                    : 8;  /**< [ 39: 32](RO/H) The affinity level 0 value for the redistributor. */
        uint64_t a1                    : 8;  /**< [ 47: 40](RO/H) The affinity level 1 value for the redistributor. */
        uint64_t a2                    : 8;  /**< [ 55: 48](RO/H) The affinity level 2 value for the redistributor. */
        uint64_t a3                    : 8;  /**< [ 63: 56](RO) The affinity level 3 value for the redistributor. */
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_gicrx_typer_s cn9; */
};
typedef union bdk_gicrx_typer bdk_gicrx_typer_t;

static inline uint64_t BDK_GICRX_TYPER(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_TYPER(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000008ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000008ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000008ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000008ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_TYPER", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_TYPER(a) bdk_gicrx_typer_t
#define bustype_BDK_GICRX_TYPER(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GICRX_TYPER(a) "GICRX_TYPER"
#define device_bar_BDK_GICRX_TYPER(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_TYPER(a) (a)
#define arguments_BDK_GICRX_TYPER(a) (a),-1,-1,-1

/**
 * Register (NCB32b) gicr#_waker
 *
 * GIC Redistributor Wake Request Control Secure Register
 */
union bdk_gicrx_waker
{
    uint32_t u;
    struct bdk_gicrx_waker_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t quiescent             : 1;  /**< [ 31: 31](SRO) Indicates that redistributor is quiescent and can be powered off. */
        uint32_t reserved_3_30         : 28;
        uint32_t ca                    : 1;  /**< [  2:  2](SRO) Children asleep.
                                                                 When [PS] is one, the redistributor treats the interrupt group enables as zero
                                                                 until a subsequent update to the enables is received. */
        uint32_t ps                    : 1;  /**< [  1:  1](SR/W) Processor sleep.
                                                                 0 = The redistributor never asserts WakeRequest.
                                                                 1 = The redistributor must assert WakeRequest and hold interrupts as pending if an enable
                                                                 bit is zero for an interrupt group and there is a pending interrupt for that group. */
        uint32_t sleep                 : 1;  /**< [  0:  0](SR/W) Sleep.
                                                                 0 = The parent never asserts WakeRequest.
                                                                 1 = The parent must assert WakeRequest and hold interrupts as pending. */
#else /* Word 0 - Little Endian */
        uint32_t sleep                 : 1;  /**< [  0:  0](SR/W) Sleep.
                                                                 0 = The parent never asserts WakeRequest.
                                                                 1 = The parent must assert WakeRequest and hold interrupts as pending. */
        uint32_t ps                    : 1;  /**< [  1:  1](SR/W) Processor sleep.
                                                                 0 = The redistributor never asserts WakeRequest.
                                                                 1 = The redistributor must assert WakeRequest and hold interrupts as pending if an enable
                                                                 bit is zero for an interrupt group and there is a pending interrupt for that group. */
        uint32_t ca                    : 1;  /**< [  2:  2](SRO) Children asleep.
                                                                 When [PS] is one, the redistributor treats the interrupt group enables as zero
                                                                 until a subsequent update to the enables is received. */
        uint32_t reserved_3_30         : 28;
        uint32_t quiescent             : 1;  /**< [ 31: 31](SRO) Indicates that redistributor is quiescent and can be powered off. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gicrx_waker_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t quiescent             : 1;  /**< [ 31: 31](SRO/H) Indicates that redistributor is quiescent and can be powered off. */
        uint32_t reserved_3_30         : 28;
        uint32_t ca                    : 1;  /**< [  2:  2](SRO/H) Children asleep.
                                                                 When [PS] is one, the redistributor treats the interrupt group enables as zero
                                                                 until a subsequent update to the enables is received. */
        uint32_t ps                    : 1;  /**< [  1:  1](SR/W) Processor sleep.
                                                                 0 = The redistributor never asserts WakeRequest.
                                                                 1 = The redistributor must assert WakeRequest and hold interrupts as pending if an enable
                                                                 bit is zero for an interrupt group and there is a pending interrupt for that group. */
        uint32_t sleep                 : 1;  /**< [  0:  0](SR/W) Sleep.
                                                                 0 = The parent never asserts WakeRequest.
                                                                 1 = The parent must assert WakeRequest and hold interrupts as pending. */
#else /* Word 0 - Little Endian */
        uint32_t sleep                 : 1;  /**< [  0:  0](SR/W) Sleep.
                                                                 0 = The parent never asserts WakeRequest.
                                                                 1 = The parent must assert WakeRequest and hold interrupts as pending. */
        uint32_t ps                    : 1;  /**< [  1:  1](SR/W) Processor sleep.
                                                                 0 = The redistributor never asserts WakeRequest.
                                                                 1 = The redistributor must assert WakeRequest and hold interrupts as pending if an enable
                                                                 bit is zero for an interrupt group and there is a pending interrupt for that group. */
        uint32_t ca                    : 1;  /**< [  2:  2](SRO/H) Children asleep.
                                                                 When [PS] is one, the redistributor treats the interrupt group enables as zero
                                                                 until a subsequent update to the enables is received. */
        uint32_t reserved_3_30         : 28;
        uint32_t quiescent             : 1;  /**< [ 31: 31](SRO/H) Indicates that redistributor is quiescent and can be powered off. */
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_gicrx_waker_cn9 cn81xx; */
    /* struct bdk_gicrx_waker_s cn88xx; */
    /* struct bdk_gicrx_waker_cn9 cn83xx; */
};
typedef union bdk_gicrx_waker bdk_gicrx_waker_t;

static inline uint64_t BDK_GICRX_WAKER(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GICRX_WAKER(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x801080000014ll + 0x20000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x801080000014ll + 0x20000ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x801080000014ll + 0x20000ll * ((a) & 0x3f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=23))
        return 0x801080000014ll + 0x20000ll * ((a) & 0x1f);
    __bdk_csr_fatal("GICRX_WAKER", 1, a, 0, 0, 0);
}

#define typedef_BDK_GICRX_WAKER(a) bdk_gicrx_waker_t
#define bustype_BDK_GICRX_WAKER(a) BDK_CSR_TYPE_NCB32b
#define basename_BDK_GICRX_WAKER(a) "GICRX_WAKER"
#define device_bar_BDK_GICRX_WAKER(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_GICRX_WAKER(a) (a)
#define arguments_BDK_GICRX_WAKER(a) (a),-1,-1,-1

/**
 * Register (NCB) gits_baser#
 *
 * GIC ITS Device Table Registers
 * This set of 64-bit registers specify the base address and size of a number of implementation
 * defined tables required by the ITS.
 * An implementation can provide up to eight such registers.
 * Where a register is not implemented, it is RES0.
 * Bits [63:32] and bits [31:0] may be accessed independently.
 */
union bdk_gits_baserx
{
    uint64_t u;
    struct bdk_gits_baserx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:48\> = Reserved.
                                                                       * Bits\<47:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t reserved_12_55        : 44;
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t reserved_12_55        : 44;
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:48\> = Reserved.
                                                                       * Bits\<47:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gits_baserx_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:48\> = Reserved.
                                                                       * Bits\<47:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t entry_size            : 8;  /**< [ 55: 48](RO) This field is read-only and specifies the number of bytes per entry, minus one. */
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. This field provides bits [41:12] of the base physical address of the
                                                                 table.
                                                                 Bits [11:0] of the base physical address are zero. The address must be aligned to the size
                                                                 specified in the page size field. Otherwise the effect is CONSTRAINED UNPREDICTABLE, and
                                                                 can
                                                                 be one of the following:
                                                                   * Bits X:12 (where X is derived from the page size) are treated as zero.
                                                                   * The value of bits X:12 are used when calculating the address of a table access.

                                                                 In CNXXXX where the address must be in DRAM this contains fewer than 48 bits of
                                                                 physical address bits. */
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. This field provides bits [41:12] of the base physical address of the
                                                                 table.
                                                                 Bits [11:0] of the base physical address are zero. The address must be aligned to the size
                                                                 specified in the page size field. Otherwise the effect is CONSTRAINED UNPREDICTABLE, and
                                                                 can
                                                                 be one of the following:
                                                                   * Bits X:12 (where X is derived from the page size) are treated as zero.
                                                                   * The value of bits X:12 are used when calculating the address of a table access.

                                                                 In CNXXXX where the address must be in DRAM this contains fewer than 48 bits of
                                                                 physical address bits. */
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t entry_size            : 8;  /**< [ 55: 48](RO) This field is read-only and specifies the number of bytes per entry, minus one. */
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:48\> = Reserved.
                                                                       * Bits\<47:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_gits_baserx_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:52\> = Reserved.
                                                                       * Bits\<51:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table. If the Type field is
                                                                 zero this field is RAZ/WI.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate,write-through.
                                                                 0x5 = Normal inner cacheable write-allocate,write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t entry_size            : 5;  /**< [ 52: 48](RO) This field is read-only and specifies the number of bytes per entry, minus one. */
        uint64_t physical_address      : 36; /**< [ 47: 12](R/W) Physical address.
                                                                 Software must configure this field to point to a valid DRAM base address.
                                                                 When page size is 4 KB or 16 KB:
                                                                   * This field provides bits \<47:12\> of the base physical address of the table.
                                                                   * Bits \<51:48\> and \<11:0\> of the base physical address are zero.
                                                                   * The address must be aligned to the size specified in the page size field.
                                                                     Otherwise the effect is CONSTRAINED UNPREDICTABLE, and
                                                                     can be one of the following:
                                                                       * Bits X:12 (where X is derived from the page size) are treated as zero.
                                                                       * The value of bits X:12 are used when calculating the address of a table access.

                                                                 When page size is 64 KB:
                                                                   * This field provides bits \<51:16\> of the base physical address of the table.
                                                                   * Bits \<15:12\> of this field provide bits \<51:48\> of the base physical address.
                                                                   * Bits \<15:0\> of the base physical address are zero.

                                                                 In CNXXXX where the address must be in DRAM this contains fewer than 52 bits of
                                                                 physical address bits. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t physical_address      : 36; /**< [ 47: 12](R/W) Physical address.
                                                                 Software must configure this field to point to a valid DRAM base address.
                                                                 When page size is 4 KB or 16 KB:
                                                                   * This field provides bits \<47:12\> of the base physical address of the table.
                                                                   * Bits \<51:48\> and \<11:0\> of the base physical address are zero.
                                                                   * The address must be aligned to the size specified in the page size field.
                                                                     Otherwise the effect is CONSTRAINED UNPREDICTABLE, and
                                                                     can be one of the following:
                                                                       * Bits X:12 (where X is derived from the page size) are treated as zero.
                                                                       * The value of bits X:12 are used when calculating the address of a table access.

                                                                 When page size is 64 KB:
                                                                   * This field provides bits \<51:16\> of the base physical address of the table.
                                                                   * Bits \<15:12\> of this field provide bits \<51:48\> of the base physical address.
                                                                   * Bits \<15:0\> of the base physical address are zero.

                                                                 In CNXXXX where the address must be in DRAM this contains fewer than 52 bits of
                                                                 physical address bits. */
        uint64_t entry_size            : 5;  /**< [ 52: 48](RO) This field is read-only and specifies the number of bytes per entry, minus one. */
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table. If the Type field is
                                                                 zero this field is RAZ/WI.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate,write-through.
                                                                 0x5 = Normal inner cacheable write-allocate,write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:52\> = Reserved.
                                                                       * Bits\<51:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
#endif /* Word 0 - End */
    } cn9;
    struct bdk_gits_baserx_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:48\> = Reserved.
                                                                       * Bits\<47:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table. If the Type field is
                                                                 zero this field is RAZ/WI.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate,write-through.
                                                                 0x5 = Normal inner cacheable write-allocate,write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t entry_size            : 5;  /**< [ 52: 48](RO) This field is read-only and specifies the number of bytes per entry, minus one. */
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. This field provides bits [41:12] of the base physical address of the
                                                                 table.
                                                                 Bits [11:0] of the base physical address are zero. The address must be aligned to the size
                                                                 specified in the page size field. Otherwise the effect is CONSTRAINED UNPREDICTABLE, and
                                                                 can
                                                                 be one of the following:
                                                                   * Bits X:12 (where X is derived from the page size) are treated as zero.
                                                                   * The value of bits X:12 are used when calculating the address of a table access.

                                                                 In CNXXXX where the address must be in DRAM this contains fewer than 48 bits of
                                                                 physical address bits. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) Size. The number of pages of memory allocated to the table, minus one. */
        uint64_t pagesize              : 2;  /**< [  9:  8](R/W) Page size:
                                                                 0x0 = 4 KB pages.
                                                                 0x1 = 16 KB pages (not supported, reserved).
                                                                 0x2 = 64 KB pages.
                                                                 0x3 = Reserved. Treated as 64 KB pages. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. This field provides bits [41:12] of the base physical address of the
                                                                 table.
                                                                 Bits [11:0] of the base physical address are zero. The address must be aligned to the size
                                                                 specified in the page size field. Otherwise the effect is CONSTRAINED UNPREDICTABLE, and
                                                                 can
                                                                 be one of the following:
                                                                   * Bits X:12 (where X is derived from the page size) are treated as zero.
                                                                   * The value of bits X:12 are used when calculating the address of a table access.

                                                                 In CNXXXX where the address must be in DRAM this contains fewer than 48 bits of
                                                                 physical address bits. */
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t entry_size            : 5;  /**< [ 52: 48](RO) This field is read-only and specifies the number of bytes per entry, minus one. */
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t tbl_type              : 3;  /**< [ 58: 56](RO) This field is read-only and specifies the type of entity that requires entries in the
                                                                 associated table. The field may have the following values:
                                                                 0x0 = Unimplemented. This register does not correspond to an ITS table and requires no
                                                                 memory.
                                                                 0x1 = Devices. This register corresponds to a table that scales according to the number of
                                                                 devices serviced by the ITS and requires
                                                                       (Entry-size * number-of-devices) bytes of memory.
                                                                 0x2 = Virtual processors. This register corresponds to a table that scales according to
                                                                 the number of virtual processors in the system and
                                                                       requires (Entry-size * number-of-processors) bytes ofmemory.
                                                                 0x3 = Physical processors.
                                                                 0x4 = Interrupt collections.
                                                                 0x5 = Reserved.
                                                                 0x6 = Reserved.
                                                                 0x7 = Reserved.

                                                                 Software must always provision memory for GITS_BASER() registers where this field
                                                                 indicate "devices","interrupt collections" or "physical processors". */
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table. If the Type field is
                                                                 zero this field is RAZ/WI.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate,write-through.
                                                                 0x5 = Normal inner cacheable write-allocate,write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t indirect              : 1;  /**< [ 62: 62](RO) Indirect.This field indicates whether an implemented register specifies a single, flat
                                                                 table or a two-level table where the first level
                                                                 contains a list of descriptors. Note: this field is RAZ/WI for implementations that only
                                                                 support flat tables.
                                                                 0 = Single level. [SIZE] indicates a number of pages used by the ITS to store data
                                                                 associated with each table entry.
                                                                 1 = Two level. [SIZE] indicates a number of pages which contain an array of 64-bit
                                                                 descriptors to pages that are used
                                                                     to store the data associated with each table entry. Each 64-bit descriptor has the
                                                                 following format:
                                                                       * Bits\<63\>    = Valid.
                                                                       * Bits\<62:48\> = Reserved.
                                                                       * Bits\<47:N\>  = Physical address.
                                                                       * Bits\<N-1:0\> = Reserved.
                                                                       * Where N is the number of bits required to specify the page size.
                                                                 Note:  software must ensure that each pointer in the first level table specifies a unique
                                                                 physical address otherwise the effects are unpredictable.
                                                                 For a two level table, if an entry is invalid:
                                                                   * If the type field specifies a valid table type other than interrupt collections, the
                                                                 ITS
                                                                     discards any writes to the interrupt translation page.
                                                                   * If the type field specifies the interrupt collections table and GITS_TYPER.HCC is
                                                                 zero,
                                                                     the ITS discards any writes to the interrupt translation page. */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid:
                                                                 0 = No memory has been allocated to the table and if the type field is nonzero, the ITS
                                                                 discards any writes to the interrupt translation page.
                                                                 1 = Memory has been allocated to the table  by software. */
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gits_baserx_cn81xx cn83xx; */
    /* struct bdk_gits_baserx_cn81xx cn88xxp2; */
};
typedef union bdk_gits_baserx bdk_gits_baserx_t;

static inline uint64_t BDK_GITS_BASERX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_BASERX(unsigned long a)
{
    if (a==0)
        return 0x801000020100ll + 8ll * ((a) & 0x0);
    __bdk_csr_fatal("GITS_BASERX", 1, a, 0, 0, 0);
}

#define typedef_BDK_GITS_BASERX(a) bdk_gits_baserx_t
#define bustype_BDK_GITS_BASERX(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_BASERX(a) "GITS_BASERX"
#define device_bar_BDK_GITS_BASERX(a) 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_BASERX(a) (a)
#define arguments_BDK_GITS_BASERX(a) (a),-1,-1,-1

/**
 * Register (NCB) gits_baser#_rowi
 *
 * GIC ITS Table Registers
 * This set of 64-bit registers specify the base address and size of a number of implementation
 * defined tables required by the ITS:
 * An implementation can provide up to eight such registers.
 * Where a register is not implemented, it is RES0.
 */
union bdk_gits_baserx_rowi
{
    uint64_t u;
    struct bdk_gits_baserx_rowi_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_baserx_rowi_s cn; */
};
typedef union bdk_gits_baserx_rowi bdk_gits_baserx_rowi_t;

static inline uint64_t BDK_GITS_BASERX_ROWI(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_BASERX_ROWI(unsigned long a)
{
    if ((a>=1)&&(a<=7))
        return 0x801000020100ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("GITS_BASERX_ROWI", 1, a, 0, 0, 0);
}

#define typedef_BDK_GITS_BASERX_ROWI(a) bdk_gits_baserx_rowi_t
#define bustype_BDK_GITS_BASERX_ROWI(a) BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_BASERX_ROWI(a) "GITS_BASERX_ROWI"
#define device_bar_BDK_GITS_BASERX_ROWI(a) 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_BASERX_ROWI(a) (a)
#define arguments_BDK_GITS_BASERX_ROWI(a) (a),-1,-1,-1

/**
 * Register (NCB) gits_cbaser
 *
 * GIC ITS Command Queue Base Register
 * This register holds the physical memory address of the ITS command queue.
 * Note: when GITS_CBASER is  successfully written, the value of GITS_CREADR is set to zero. See
 * GIC
 * spec for details on the ITS initialization sequence. Bits [63:32] and bits [31:0] may be
 * accessed
 * independently. When GITS_CTLR[ENABLED] is one or GITS_CTLR[QUIESCENT] is zero, this register is
 * read-only.
 */
union bdk_gits_cbaser
{
    uint64_t u;
    struct bdk_gits_cbaser_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
        uint64_t reserved_62           : 1;
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_56_58        : 3;
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_12_52        : 41;
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t reserved_8_9          : 2;
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
        uint64_t reserved_8_9          : 2;
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t reserved_12_52        : 41;
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_56_58        : 3;
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_62           : 1;
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gits_cbaser_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
        uint64_t reserved_62           : 1;
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_48_58        : 11;
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. Provides bits \<47:12\> of the physical address of the memory
                                                                 containing the command queue. Bits \<11:0\> of the base address of the queue are
                                                                 zero. */
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t reserved_8_9          : 2;
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
        uint64_t reserved_8_9          : 2;
        uint64_t shareability          : 2;  /**< [ 11: 10](RO) Shareability attribute:
                                                                 0x0 = Accesses are nonshareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 Ignored in CNXXXX. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. Provides bits \<47:12\> of the physical address of the memory
                                                                 containing the command queue. Bits \<11:0\> of the base address of the queue are
                                                                 zero. */
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t reserved_48_58        : 11;
        uint64_t cacheability          : 3;  /**< [ 61: 59](RO) Cacheability attribute:
                                                                 0x0 = Noncacheable, nonbufferable.
                                                                 0x1 = Noncacheable.
                                                                 0x2 = Read-allocate, writethrough.
                                                                 0x3 = Read-allocate, writeback.
                                                                 0x4 = Write-allocate, writethrough.
                                                                 0x5 = Write-allocate, writeback.
                                                                 0x6 = Read-allocate, write-allocate, writethrough.
                                                                 0x7 = Read-allocate, write-allocate, writeback.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_62           : 1;
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_gits_cbaser_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
        uint64_t reserved_62           : 1;
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate, write-through.
                                                                 0x5 = Normal inner cacheable write-allocate, write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.
                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_56_58        : 3;
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_52           : 1;
        uint64_t physical_address      : 40; /**< [ 51: 12](R/W) Physical address. Provides bits \<51:12\> of the physical address of the memory
                                                                 containing the command queue. Bits \<11:0\> of the base address of the queue are
                                                                 zero.
                                                                 Software must configure this field to point to a valid DRAM base address.
                                                                 If bits \<15:12\> are not all zeros, behavior is CONSTRAINED UNPREDICTABLE
                                                                 and the result of the calculation of an address for a command queue read
                                                                 can be corrupted. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute. The shareability attributes of accesses to the table.
                                                                 0x0 = Accesses are non-shareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_8_9          : 2;
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
        uint64_t reserved_8_9          : 2;
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute. The shareability attributes of accesses to the table.
                                                                 0x0 = Accesses are non-shareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t physical_address      : 40; /**< [ 51: 12](R/W) Physical address. Provides bits \<51:12\> of the physical address of the memory
                                                                 containing the command queue. Bits \<11:0\> of the base address of the queue are
                                                                 zero.
                                                                 Software must configure this field to point to a valid DRAM base address.
                                                                 If bits \<15:12\> are not all zeros, behavior is CONSTRAINED UNPREDICTABLE
                                                                 and the result of the calculation of an address for a command queue read
                                                                 can be corrupted. */
        uint64_t reserved_52           : 1;
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_56_58        : 3;
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate, write-through.
                                                                 0x5 = Normal inner cacheable write-allocate, write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.
                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_62           : 1;
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
#endif /* Word 0 - End */
    } cn9;
    struct bdk_gits_cbaser_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
        uint64_t reserved_62           : 1;
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate, write-through.
                                                                 0x5 = Normal inner cacheable write-allocate, write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.
                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_56_58        : 3;
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_48_52        : 5;
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. Provides bits \<47:12\> of the physical address of the memory
                                                                 containing the command queue. Bits \<11:0\> of the base address of the queue are
                                                                 zero. */
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute. The shareability attributes of accesses to the table.
                                                                 0x0 = Accesses are non-shareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_8_9          : 2;
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
#else /* Word 0 - Little Endian */
        uint64_t size                  : 8;  /**< [  7:  0](R/W) The number of 4 KB pages of physical memory provided for the command queue, minus one.
                                                                 The command queue is a circular buffer and wraps at physical address \<47:0\> + (4096 *
                                                                 (SIZE+1)). */
        uint64_t reserved_8_9          : 2;
        uint64_t shareability          : 2;  /**< [ 11: 10](R/W) Shareability attribute. The shareability attributes of accesses to the table.
                                                                 0x0 = Accesses are non-shareable.
                                                                 0x1 = Accesses are inner-shareable.
                                                                 0x2 = Accesses are outer-shareable.
                                                                 0x3 = Reserved.  Treated as 0x0.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t physical_address      : 30; /**< [ 41: 12](R/W) Physical address. Provides bits \<47:12\> of the physical address of the memory
                                                                 containing the command queue. Bits \<11:0\> of the base address of the queue are
                                                                 zero. */
        uint64_t arsvd                 : 6;  /**< [ 47: 42](R/W) Reserved; must be zero. This field will be ignored if not zero. */
        uint64_t reserved_48_52        : 5;
        uint64_t outer_cacheability    : 3;  /**< [ 55: 53](R/W) Outer cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Memory type defined in bits[61:59]; for normal memory outer cacheability is the same
                                                                 as the inner cacheable.
                                                                 0x1 = Normal outer noncacheable.
                                                                 0x2 = Normal outer cacheable read-allocate, write-through.
                                                                 0x3 = Normal outer cacheable read-allocate, write-back.
                                                                 0x4 = Normal outer cacheable write-allocate, write-through.
                                                                 0x5 = Normal outer cacheable write-allocate, write-back.
                                                                 0x6 = Normal outer cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal outer cacheable read-allocate, write-allocate, write-back.

                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_56_58        : 3;
        uint64_t cacheability          : 3;  /**< [ 61: 59](R/W) Cacheability. The cacheability attributes of accesses to the table.
                                                                 0x0 = Device-nGnRnE.
                                                                 0x1 = Normal inner noncacheable.
                                                                 0x2 = Normal inner cacheable read-allocate, write-through.
                                                                 0x3 = Normal inner cacheable read-allocate, write-back.
                                                                 0x4 = Normal inner cacheable write-allocate, write-through.
                                                                 0x5 = Normal inner cacheable write-allocate, write-back.
                                                                 0x6 = Normal inner cacheable read-allocate, write-allocate, write-through.
                                                                 0x7 = Normal inner cacheable read-allocate, write-allocate, write-back.
                                                                 In CNXXXX not implemented, ignored. */
        uint64_t reserved_62           : 1;
        uint64_t valid                 : 1;  /**< [ 63: 63](R/W) Valid.
                                                                 When set to one, indicates that memory has been allocated by software for the command
                                                                 queue
                                                                 When set to zero, no memory has been allocated to the command queue and the ITS discards
                                                                 any writes to the interrupt translation page. */
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gits_cbaser_cn81xx cn83xx; */
    /* struct bdk_gits_cbaser_cn81xx cn88xxp2; */
};
typedef union bdk_gits_cbaser bdk_gits_cbaser_t;

#define BDK_GITS_CBASER BDK_GITS_CBASER_FUNC()
static inline uint64_t BDK_GITS_CBASER_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CBASER_FUNC(void)
{
    return 0x801000020080ll;
}

#define typedef_BDK_GITS_CBASER bdk_gits_cbaser_t
#define bustype_BDK_GITS_CBASER BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_CBASER "GITS_CBASER"
#define device_bar_BDK_GITS_CBASER 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CBASER 0
#define arguments_BDK_GITS_CBASER -1,-1,-1,-1

/**
 * Register (NCB32b) gits_cidr0
 *
 * GIC ITS Component Identification Register 0
 */
union bdk_gits_cidr0
{
    uint32_t u;
    struct bdk_gits_cidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_cidr0_s cn; */
};
typedef union bdk_gits_cidr0 bdk_gits_cidr0_t;

#define BDK_GITS_CIDR0 BDK_GITS_CIDR0_FUNC()
static inline uint64_t BDK_GITS_CIDR0_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CIDR0_FUNC(void)
{
    return 0x80100002fff0ll;
}

#define typedef_BDK_GITS_CIDR0 bdk_gits_cidr0_t
#define bustype_BDK_GITS_CIDR0 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_CIDR0 "GITS_CIDR0"
#define device_bar_BDK_GITS_CIDR0 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CIDR0 0
#define arguments_BDK_GITS_CIDR0 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_cidr1
 *
 * GIC ITS Component Identification Register 1
 */
union bdk_gits_cidr1
{
    uint32_t u;
    struct bdk_gits_cidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_cidr1_s cn; */
};
typedef union bdk_gits_cidr1 bdk_gits_cidr1_t;

#define BDK_GITS_CIDR1 BDK_GITS_CIDR1_FUNC()
static inline uint64_t BDK_GITS_CIDR1_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CIDR1_FUNC(void)
{
    return 0x80100002fff4ll;
}

#define typedef_BDK_GITS_CIDR1 bdk_gits_cidr1_t
#define bustype_BDK_GITS_CIDR1 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_CIDR1 "GITS_CIDR1"
#define device_bar_BDK_GITS_CIDR1 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CIDR1 0
#define arguments_BDK_GITS_CIDR1 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_cidr2
 *
 * GIC ITS Component Identification Register 2
 */
union bdk_gits_cidr2
{
    uint32_t u;
    struct bdk_gits_cidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_cidr2_s cn; */
};
typedef union bdk_gits_cidr2 bdk_gits_cidr2_t;

#define BDK_GITS_CIDR2 BDK_GITS_CIDR2_FUNC()
static inline uint64_t BDK_GITS_CIDR2_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CIDR2_FUNC(void)
{
    return 0x80100002fff8ll;
}

#define typedef_BDK_GITS_CIDR2 bdk_gits_cidr2_t
#define bustype_BDK_GITS_CIDR2 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_CIDR2 "GITS_CIDR2"
#define device_bar_BDK_GITS_CIDR2 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CIDR2 0
#define arguments_BDK_GITS_CIDR2 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_cidr3
 *
 * GIC ITS Component Identification Register 3
 */
union bdk_gits_cidr3
{
    uint32_t u;
    struct bdk_gits_cidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_cidr3_s cn; */
};
typedef union bdk_gits_cidr3 bdk_gits_cidr3_t;

#define BDK_GITS_CIDR3 BDK_GITS_CIDR3_FUNC()
static inline uint64_t BDK_GITS_CIDR3_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CIDR3_FUNC(void)
{
    return 0x80100002fffcll;
}

#define typedef_BDK_GITS_CIDR3 bdk_gits_cidr3_t
#define bustype_BDK_GITS_CIDR3 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_CIDR3 "GITS_CIDR3"
#define device_bar_BDK_GITS_CIDR3 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CIDR3 0
#define arguments_BDK_GITS_CIDR3 -1,-1,-1,-1

/**
 * Register (NCB) gits_creadr
 *
 * GIC ITS Command Queue Read Register
 * Offset in the ITS command queue from GITS_CBASER where the next command will be read by the
 * ITS.
 *
 * The command queue is considered to be empty when GITS_CWRITER is equal to GITS_CREADR.
 *
 * The command queue is considered to be full when GITS_CWRITER is equal to (GITS_CREADR minus
 * 32), taking wrapping into account.
 *
 * Note: when GITS_CBASER is written, the value of GITS_CREADR is set to zero.
 */
union bdk_gits_creadr
{
    uint64_t u;
    struct bdk_gits_creadr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t offset                : 15; /**< [ 19:  5](RO/H) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where the ITS will
                                                                 read the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_1_4          : 4;
        uint64_t stalled               : 1;  /**< [  0:  0](RAZ) ITS commands are not stalled due to an error. */
#else /* Word 0 - Little Endian */
        uint64_t stalled               : 1;  /**< [  0:  0](RAZ) ITS commands are not stalled due to an error. */
        uint64_t reserved_1_4          : 4;
        uint64_t offset                : 15; /**< [ 19:  5](RO/H) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where the ITS will
                                                                 read the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } s;
    struct bdk_gits_creadr_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t offset                : 15; /**< [ 19:  5](RO/H) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where the ITS will
                                                                 read the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_0_4          : 5;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_4          : 5;
        uint64_t offset                : 15; /**< [ 19:  5](RO/H) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where the ITS will
                                                                 read the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_gits_creadr_s cn9; */
};
typedef union bdk_gits_creadr bdk_gits_creadr_t;

#define BDK_GITS_CREADR BDK_GITS_CREADR_FUNC()
static inline uint64_t BDK_GITS_CREADR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CREADR_FUNC(void)
{
    return 0x801000020090ll;
}

#define typedef_BDK_GITS_CREADR bdk_gits_creadr_t
#define bustype_BDK_GITS_CREADR BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_CREADR "GITS_CREADR"
#define device_bar_BDK_GITS_CREADR 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CREADR 0
#define arguments_BDK_GITS_CREADR -1,-1,-1,-1

/**
 * Register (NCB32b) gits_ctlr
 *
 * GIC ITS Control Register
 * This register controls the behavior of the interrupt translation service.
 */
union bdk_gits_ctlr
{
    uint32_t u;
    struct bdk_gits_ctlr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t quiescent             : 1;  /**< [ 31: 31](RO/H) This bit indicates whether the ITS has completed all operations following a write of
                                                                 enable to zero.
                                                                 0 = The ITS is not quiescent.
                                                                 1 = The ITS is quiescent, has completed all operations required to make any mapping data
                                                                     consistent with external memory and may be powered off. Note: in CCPI
                                                                     implementations,
                                                                     the ITS must also have forwarded any required operations to the redistributors and
                                                                     received confirmation that they have reached the appropriate redistributor. */
        uint32_t reserved_1_30         : 30;
        uint32_t enabled               : 1;  /**< [  0:  0](R/W) Enabled:
                                                                 0 = ITS is disabled. Writes to the interrupt translation space will be ignored and no
                                                                 further command queue entries will be processed.
                                                                 1 = ITS is enabled. Writes to the interrupt translation space will result in interrupt
                                                                 translations and the command queue will be processed.

                                                                 If a write to this register changes enabled from one to zero, the ITS must ensure that any
                                                                 caches containing mapping data must be made
                                                                 consistent with external memory and [QUIESCENT] must read as one until this has been
                                                                 completed. */
#else /* Word 0 - Little Endian */
        uint32_t enabled               : 1;  /**< [  0:  0](R/W) Enabled:
                                                                 0 = ITS is disabled. Writes to the interrupt translation space will be ignored and no
                                                                 further command queue entries will be processed.
                                                                 1 = ITS is enabled. Writes to the interrupt translation space will result in interrupt
                                                                 translations and the command queue will be processed.

                                                                 If a write to this register changes enabled from one to zero, the ITS must ensure that any
                                                                 caches containing mapping data must be made
                                                                 consistent with external memory and [QUIESCENT] must read as one until this has been
                                                                 completed. */
        uint32_t reserved_1_30         : 30;
        uint32_t quiescent             : 1;  /**< [ 31: 31](RO/H) This bit indicates whether the ITS has completed all operations following a write of
                                                                 enable to zero.
                                                                 0 = The ITS is not quiescent.
                                                                 1 = The ITS is quiescent, has completed all operations required to make any mapping data
                                                                     consistent with external memory and may be powered off. Note: in CCPI
                                                                     implementations,
                                                                     the ITS must also have forwarded any required operations to the redistributors and
                                                                     received confirmation that they have reached the appropriate redistributor. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_ctlr_s cn; */
};
typedef union bdk_gits_ctlr bdk_gits_ctlr_t;

#define BDK_GITS_CTLR BDK_GITS_CTLR_FUNC()
static inline uint64_t BDK_GITS_CTLR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CTLR_FUNC(void)
{
    return 0x801000020000ll;
}

#define typedef_BDK_GITS_CTLR bdk_gits_ctlr_t
#define bustype_BDK_GITS_CTLR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_CTLR "GITS_CTLR"
#define device_bar_BDK_GITS_CTLR 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CTLR 0
#define arguments_BDK_GITS_CTLR -1,-1,-1,-1

/**
 * Register (NCB) gits_cwriter
 *
 * GIC ITS Command Queue Write Register
 * Offset in the ITS command queue from GITS_CBASER where the next command will be written by
 * software.
 *
 * The command queue is considered to be empty when GITS_CWRITER is equal to GITS_CREADR.
 *
 * The command queue is considered to be full when GITS_CWRITER is equal to (GITS_CREADR minus
 * 32), taking wrapping into account.
 *
 * Each command in the queue comprises 32 bytes. See section 5.13 for details of the commands
 * supported and the format of each command.
 */
union bdk_gits_cwriter
{
    uint64_t u;
    struct bdk_gits_cwriter_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t offset                : 15; /**< [ 19:  5](R/W) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where software will
                                                                 write the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_1_4          : 4;
        uint64_t retry                 : 1;  /**< [  0:  0](RAZ) Retry of processing of ITS commands not supported. */
#else /* Word 0 - Little Endian */
        uint64_t retry                 : 1;  /**< [  0:  0](RAZ) Retry of processing of ITS commands not supported. */
        uint64_t reserved_1_4          : 4;
        uint64_t offset                : 15; /**< [ 19:  5](R/W) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where software will
                                                                 write the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } s;
    struct bdk_gits_cwriter_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t offset                : 15; /**< [ 19:  5](R/W) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where software will
                                                                 write the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_0_4          : 5;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_4          : 5;
        uint64_t offset                : 15; /**< [ 19:  5](R/W) Offset. Provides bits \<19:5\> of the offset from GITS_CBASER where software will
                                                                 write the next command. Bits \<4:0\> of the offset are zero. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_gits_cwriter_s cn9; */
};
typedef union bdk_gits_cwriter bdk_gits_cwriter_t;

#define BDK_GITS_CWRITER BDK_GITS_CWRITER_FUNC()
static inline uint64_t BDK_GITS_CWRITER_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_CWRITER_FUNC(void)
{
    return 0x801000020088ll;
}

#define typedef_BDK_GITS_CWRITER bdk_gits_cwriter_t
#define bustype_BDK_GITS_CWRITER BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_CWRITER "GITS_CWRITER"
#define device_bar_BDK_GITS_CWRITER 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_CWRITER 0
#define arguments_BDK_GITS_CWRITER -1,-1,-1,-1

/**
 * Register (NCB32b) gits_iidr
 *
 * GIC ITS Implementation Identification Register
 * This 32-bit register is read-only and specifies the version and features supported by the ITS.
 */
union bdk_gits_iidr
{
    uint32_t u;
    struct bdk_gits_iidr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t productid             : 8;  /**< [ 31: 24](RO) An implementation defined product number for the device.
                                                                 In CNXXXX, enumerated by PCC_PROD_E. */
        uint32_t reserved_20_23        : 4;
        uint32_t variant               : 4;  /**< [ 19: 16](RO) Indicates the major revision or variant of the product.
                                                                 On CNXXXX, this is the major revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t revision              : 4;  /**< [ 15: 12](RO) Indicates the minor revision of the product.
                                                                 On CNXXXX, this is the minor revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t implementer           : 12; /**< [ 11:  0](RO) Indicates the implementer:
                                                                    0x34C = Cavium. */
#else /* Word 0 - Little Endian */
        uint32_t implementer           : 12; /**< [ 11:  0](RO) Indicates the implementer:
                                                                    0x34C = Cavium. */
        uint32_t revision              : 4;  /**< [ 15: 12](RO) Indicates the minor revision of the product.
                                                                 On CNXXXX, this is the minor revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t variant               : 4;  /**< [ 19: 16](RO) Indicates the major revision or variant of the product.
                                                                 On CNXXXX, this is the major revision. See FUS_FUSE_NUM_E::CHIP_ID(). */
        uint32_t reserved_20_23        : 4;
        uint32_t productid             : 8;  /**< [ 31: 24](RO) An implementation defined product number for the device.
                                                                 In CNXXXX, enumerated by PCC_PROD_E. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_iidr_s cn; */
};
typedef union bdk_gits_iidr bdk_gits_iidr_t;

#define BDK_GITS_IIDR BDK_GITS_IIDR_FUNC()
static inline uint64_t BDK_GITS_IIDR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_IIDR_FUNC(void)
{
    return 0x801000020004ll;
}

#define typedef_BDK_GITS_IIDR bdk_gits_iidr_t
#define bustype_BDK_GITS_IIDR BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_IIDR "GITS_IIDR"
#define device_bar_BDK_GITS_IIDR 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_IIDR 0
#define arguments_BDK_GITS_IIDR -1,-1,-1,-1

/**
 * Register (NCB) gits_imp_cseir
 *
 * GIC ITS Implementation Defined Command SEI Register
 * This register holds the SEI status of the ITS command error.
 */
union bdk_gits_imp_cseir
{
    uint64_t u;
    struct bdk_gits_imp_cseir_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_52_63        : 12;
        uint64_t creadr                : 15; /**< [ 51: 37](RO/H) The read pointer of the first command with error. */
        uint64_t reserved_26_36        : 11;
        uint64_t cwriter_oor           : 1;  /**< [ 25: 25](RO/H) When set, it means command write pointer is out of range. */
        uint64_t m                     : 1;  /**< [ 24: 24](RO/H) When set, it means multiple command errors have happened. */
        uint64_t reserved_17_23        : 7;
        uint64_t v                     : 1;  /**< [ 16: 16](R/W1C/H) When set, the command error is valid. For meaning/encoding of {7'b0, V, CMD,
                                                                 ERROR}, please see ITS Command error encodings in the GIC specfication. Writing
                                                                 one to this field, will clear the whole register. */
        uint64_t cmd                   : 8;  /**< [ 15:  8](RO/H) Type field of first ITS command that has the error. */
        uint64_t error                 : 8;  /**< [  7:  0](RO/H) Error code for the first error. */
#else /* Word 0 - Little Endian */
        uint64_t error                 : 8;  /**< [  7:  0](RO/H) Error code for the first error. */
        uint64_t cmd                   : 8;  /**< [ 15:  8](RO/H) Type field of first ITS command that has the error. */
        uint64_t v                     : 1;  /**< [ 16: 16](R/W1C/H) When set, the command error is valid. For meaning/encoding of {7'b0, V, CMD,
                                                                 ERROR}, please see ITS Command error encodings in the GIC specfication. Writing
                                                                 one to this field, will clear the whole register. */
        uint64_t reserved_17_23        : 7;
        uint64_t m                     : 1;  /**< [ 24: 24](RO/H) When set, it means multiple command errors have happened. */
        uint64_t cwriter_oor           : 1;  /**< [ 25: 25](RO/H) When set, it means command write pointer is out of range. */
        uint64_t reserved_26_36        : 11;
        uint64_t creadr                : 15; /**< [ 51: 37](RO/H) The read pointer of the first command with error. */
        uint64_t reserved_52_63        : 12;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_imp_cseir_s cn; */
};
typedef union bdk_gits_imp_cseir bdk_gits_imp_cseir_t;

#define BDK_GITS_IMP_CSEIR BDK_GITS_IMP_CSEIR_FUNC()
static inline uint64_t BDK_GITS_IMP_CSEIR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_IMP_CSEIR_FUNC(void)
{
    return 0x801000020020ll;
}

#define typedef_BDK_GITS_IMP_CSEIR bdk_gits_imp_cseir_t
#define bustype_BDK_GITS_IMP_CSEIR BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_IMP_CSEIR "GITS_IMP_CSEIR"
#define device_bar_BDK_GITS_IMP_CSEIR 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_IMP_CSEIR 0
#define arguments_BDK_GITS_IMP_CSEIR -1,-1,-1,-1

/**
 * Register (NCB) gits_imp_tseir
 *
 * GIC ITS Implementation Defined Translator SEI Register
 * This register holds the SEI status of the ITS translator error.
 */
union bdk_gits_imp_tseir
{
    uint64_t u;
    struct bdk_gits_imp_tseir_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t v                     : 1;  /**< [ 63: 63](R/W1C/H) When set, the translator error is valid. Write one to this field will clear [V], [M],
                                                                 [DEV_ID], [INT_ID], and [ERROR]. */
        uint64_t m                     : 1;  /**< [ 62: 62](RO/H) When set, it means multiple errors have happened. */
        uint64_t reserved_56_61        : 6;
        uint64_t dev_id                : 24; /**< [ 55: 32](RO/H) Input device ID to the interrupt translator. */
        uint64_t reserved_28_31        : 4;
        uint64_t int_id                : 20; /**< [ 27:  8](RO/H) Input interrupt ID to the interrupt translator. */
        uint64_t error                 : 8;  /**< [  7:  0](RO/H) Error code for the first error. Valid encoding is enumerated by GITS_CMD_ERR_E
                                                                 and one of GITS_CMD_ERR_E::CSEI_UNMAPPED_DEVICE,
                                                                 GITS_CMD_ERR_E::CSEI_DEVICE_OOR, GITS_CMD_ERR_E::CSEI_ID_OOR,
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_INTERRUPT, or
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_COLLECTION. */
#else /* Word 0 - Little Endian */
        uint64_t error                 : 8;  /**< [  7:  0](RO/H) Error code for the first error. Valid encoding is enumerated by GITS_CMD_ERR_E
                                                                 and one of GITS_CMD_ERR_E::CSEI_UNMAPPED_DEVICE,
                                                                 GITS_CMD_ERR_E::CSEI_DEVICE_OOR, GITS_CMD_ERR_E::CSEI_ID_OOR,
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_INTERRUPT, or
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_COLLECTION. */
        uint64_t int_id                : 20; /**< [ 27:  8](RO/H) Input interrupt ID to the interrupt translator. */
        uint64_t reserved_28_31        : 4;
        uint64_t dev_id                : 24; /**< [ 55: 32](RO/H) Input device ID to the interrupt translator. */
        uint64_t reserved_56_61        : 6;
        uint64_t m                     : 1;  /**< [ 62: 62](RO/H) When set, it means multiple errors have happened. */
        uint64_t v                     : 1;  /**< [ 63: 63](R/W1C/H) When set, the translator error is valid. Write one to this field will clear [V], [M],
                                                                 [DEV_ID], [INT_ID], and [ERROR]. */
#endif /* Word 0 - End */
    } s;
    struct bdk_gits_imp_tseir_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t v                     : 1;  /**< [ 63: 63](R/W1C/H) When set, the translator error is valid. Write one to this field will clear [V], [M],
                                                                 [DEV_ID], [INT_ID], and [ERROR]. */
        uint64_t m                     : 1;  /**< [ 62: 62](RO/H) When set, it means multiple errors have happened. */
        uint64_t reserved_53_61        : 9;
        uint64_t dev_id                : 21; /**< [ 52: 32](RO/H) Input device ID to the interrupt translator. */
        uint64_t reserved_28_31        : 4;
        uint64_t int_id                : 20; /**< [ 27:  8](RO/H) Input interrupt ID to the interrupt translator. */
        uint64_t error                 : 8;  /**< [  7:  0](RO/H) Error code for the first error. Valid encoding is enumerated by GITS_CMD_ERR_E
                                                                 and one of GITS_CMD_ERR_E::CSEI_UNMAPPED_DEVICE,
                                                                 GITS_CMD_ERR_E::CSEI_DEVICE_OOR, GITS_CMD_ERR_E::CSEI_ID_OOR,
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_INTERRUPT, or
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_COLLECTION. */
#else /* Word 0 - Little Endian */
        uint64_t error                 : 8;  /**< [  7:  0](RO/H) Error code for the first error. Valid encoding is enumerated by GITS_CMD_ERR_E
                                                                 and one of GITS_CMD_ERR_E::CSEI_UNMAPPED_DEVICE,
                                                                 GITS_CMD_ERR_E::CSEI_DEVICE_OOR, GITS_CMD_ERR_E::CSEI_ID_OOR,
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_INTERRUPT, or
                                                                 GITS_CMD_ERR_E::CSEI_UNMAPPED_COLLECTION. */
        uint64_t int_id                : 20; /**< [ 27:  8](RO/H) Input interrupt ID to the interrupt translator. */
        uint64_t reserved_28_31        : 4;
        uint64_t dev_id                : 21; /**< [ 52: 32](RO/H) Input device ID to the interrupt translator. */
        uint64_t reserved_53_61        : 9;
        uint64_t m                     : 1;  /**< [ 62: 62](RO/H) When set, it means multiple errors have happened. */
        uint64_t v                     : 1;  /**< [ 63: 63](R/W1C/H) When set, the translator error is valid. Write one to this field will clear [V], [M],
                                                                 [DEV_ID], [INT_ID], and [ERROR]. */
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_gits_imp_tseir_s cn9; */
};
typedef union bdk_gits_imp_tseir bdk_gits_imp_tseir_t;

#define BDK_GITS_IMP_TSEIR BDK_GITS_IMP_TSEIR_FUNC()
static inline uint64_t BDK_GITS_IMP_TSEIR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_IMP_TSEIR_FUNC(void)
{
    return 0x801000020028ll;
}

#define typedef_BDK_GITS_IMP_TSEIR bdk_gits_imp_tseir_t
#define bustype_BDK_GITS_IMP_TSEIR BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_IMP_TSEIR "GITS_IMP_TSEIR"
#define device_bar_BDK_GITS_IMP_TSEIR 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_IMP_TSEIR 0
#define arguments_BDK_GITS_IMP_TSEIR -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr0
 *
 * GIC ITS Peripheral Identification Register 0
 */
union bdk_gits_pidr0
{
    uint32_t u;
    struct bdk_gits_pidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  Indicates PCC_PIDR_PARTNUM0_E::GITS. */
#else /* Word 0 - Little Endian */
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  Indicates PCC_PIDR_PARTNUM0_E::GITS. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr0_s cn; */
};
typedef union bdk_gits_pidr0 bdk_gits_pidr0_t;

#define BDK_GITS_PIDR0 BDK_GITS_PIDR0_FUNC()
static inline uint64_t BDK_GITS_PIDR0_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR0_FUNC(void)
{
    return 0x80100002ffe0ll;
}

#define typedef_BDK_GITS_PIDR0 bdk_gits_pidr0_t
#define bustype_BDK_GITS_PIDR0 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR0 "GITS_PIDR0"
#define device_bar_BDK_GITS_PIDR0 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR0 0
#define arguments_BDK_GITS_PIDR0 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr1
 *
 * GIC ITS Peripheral Identification Register 1
 */
union bdk_gits_pidr1
{
    uint32_t u;
    struct bdk_gits_pidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t idcode                : 4;  /**< [  7:  4](RO) JEP106 identification code \<3:0\>. Cavium code is 0x4C. */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  Indicates PCC_PIDR_PARTNUM1_E::COMP. */
#else /* Word 0 - Little Endian */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  Indicates PCC_PIDR_PARTNUM1_E::COMP. */
        uint32_t idcode                : 4;  /**< [  7:  4](RO) JEP106 identification code \<3:0\>. Cavium code is 0x4C. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr1_s cn; */
};
typedef union bdk_gits_pidr1 bdk_gits_pidr1_t;

#define BDK_GITS_PIDR1 BDK_GITS_PIDR1_FUNC()
static inline uint64_t BDK_GITS_PIDR1_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR1_FUNC(void)
{
    return 0x80100002ffe4ll;
}

#define typedef_BDK_GITS_PIDR1 bdk_gits_pidr1_t
#define bustype_BDK_GITS_PIDR1 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR1 "GITS_PIDR1"
#define device_bar_BDK_GITS_PIDR1 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR1 0
#define arguments_BDK_GITS_PIDR1 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr2
 *
 * GIC ITS Peripheral Identification Register 2
 */
union bdk_gits_pidr2
{
    uint32_t u;
    struct bdk_gits_pidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t archrev               : 4;  /**< [  7:  4](RO) Architectural revision:
                                                                   0x1 = GICv1.
                                                                   0x2 = GICV2.
                                                                   0x3 = GICv3.
                                                                   0x4 = GICv4.
                                                                   0x5-0xF = Reserved. */
        uint32_t usesjepcode           : 1;  /**< [  3:  3](RO) JEDEC assigned. */
        uint32_t jepid                 : 3;  /**< [  2:  0](RO) JEP106 identification code \<6:4\>. Cavium code is 0x4C. */
#else /* Word 0 - Little Endian */
        uint32_t jepid                 : 3;  /**< [  2:  0](RO) JEP106 identification code \<6:4\>. Cavium code is 0x4C. */
        uint32_t usesjepcode           : 1;  /**< [  3:  3](RO) JEDEC assigned. */
        uint32_t archrev               : 4;  /**< [  7:  4](RO) Architectural revision:
                                                                   0x1 = GICv1.
                                                                   0x2 = GICV2.
                                                                   0x3 = GICv3.
                                                                   0x4 = GICv4.
                                                                   0x5-0xF = Reserved. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr2_s cn; */
};
typedef union bdk_gits_pidr2 bdk_gits_pidr2_t;

#define BDK_GITS_PIDR2 BDK_GITS_PIDR2_FUNC()
static inline uint64_t BDK_GITS_PIDR2_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR2_FUNC(void)
{
    return 0x80100002ffe8ll;
}

#define typedef_BDK_GITS_PIDR2 bdk_gits_pidr2_t
#define bustype_BDK_GITS_PIDR2 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR2 "GITS_PIDR2"
#define device_bar_BDK_GITS_PIDR2 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR2 0
#define arguments_BDK_GITS_PIDR2 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr3
 *
 * GIC ITS Peripheral Identification Register 3
 */
union bdk_gits_pidr3
{
    uint32_t u;
    struct bdk_gits_pidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t cmod                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
#else /* Word 0 - Little Endian */
        uint32_t cmod                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr3_s cn; */
};
typedef union bdk_gits_pidr3 bdk_gits_pidr3_t;

#define BDK_GITS_PIDR3 BDK_GITS_PIDR3_FUNC()
static inline uint64_t BDK_GITS_PIDR3_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR3_FUNC(void)
{
    return 0x80100002ffecll;
}

#define typedef_BDK_GITS_PIDR3 bdk_gits_pidr3_t
#define bustype_BDK_GITS_PIDR3 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR3 "GITS_PIDR3"
#define device_bar_BDK_GITS_PIDR3 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR3 0
#define arguments_BDK_GITS_PIDR3 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr4
 *
 * GIC ITS Peripheral Identification Register 1
 */
union bdk_gits_pidr4
{
    uint32_t u;
    struct bdk_gits_pidr4_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t cnt_4k                : 4;  /**< [  7:  4](RO) 4 KB Count. This field is 0x4, indicating this is a 64 KB software-visible page. */
        uint32_t continuation_code     : 4;  /**< [  3:  0](RO) JEP106 continuation code, least significant nibble. Indicates Cavium. */
#else /* Word 0 - Little Endian */
        uint32_t continuation_code     : 4;  /**< [  3:  0](RO) JEP106 continuation code, least significant nibble. Indicates Cavium. */
        uint32_t cnt_4k                : 4;  /**< [  7:  4](RO) 4 KB Count. This field is 0x4, indicating this is a 64 KB software-visible page. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr4_s cn; */
};
typedef union bdk_gits_pidr4 bdk_gits_pidr4_t;

#define BDK_GITS_PIDR4 BDK_GITS_PIDR4_FUNC()
static inline uint64_t BDK_GITS_PIDR4_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR4_FUNC(void)
{
    return 0x80100002ffd0ll;
}

#define typedef_BDK_GITS_PIDR4 bdk_gits_pidr4_t
#define bustype_BDK_GITS_PIDR4 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR4 "GITS_PIDR4"
#define device_bar_BDK_GITS_PIDR4 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR4 0
#define arguments_BDK_GITS_PIDR4 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr5
 *
 * GIC ITS Peripheral Identification Register 5
 */
union bdk_gits_pidr5
{
    uint32_t u;
    struct bdk_gits_pidr5_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr5_s cn; */
};
typedef union bdk_gits_pidr5 bdk_gits_pidr5_t;

#define BDK_GITS_PIDR5 BDK_GITS_PIDR5_FUNC()
static inline uint64_t BDK_GITS_PIDR5_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR5_FUNC(void)
{
    return 0x80100002ffd4ll;
}

#define typedef_BDK_GITS_PIDR5 bdk_gits_pidr5_t
#define bustype_BDK_GITS_PIDR5 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR5 "GITS_PIDR5"
#define device_bar_BDK_GITS_PIDR5 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR5 0
#define arguments_BDK_GITS_PIDR5 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr6
 *
 * GIC ITS Peripheral Identification Register 6
 */
union bdk_gits_pidr6
{
    uint32_t u;
    struct bdk_gits_pidr6_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr6_s cn; */
};
typedef union bdk_gits_pidr6 bdk_gits_pidr6_t;

#define BDK_GITS_PIDR6 BDK_GITS_PIDR6_FUNC()
static inline uint64_t BDK_GITS_PIDR6_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR6_FUNC(void)
{
    return 0x80100002ffd8ll;
}

#define typedef_BDK_GITS_PIDR6 bdk_gits_pidr6_t
#define bustype_BDK_GITS_PIDR6 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR6 "GITS_PIDR6"
#define device_bar_BDK_GITS_PIDR6 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR6 0
#define arguments_BDK_GITS_PIDR6 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_pidr7
 *
 * GIC ITS Peripheral Identification Register 7
 */
union bdk_gits_pidr7
{
    uint32_t u;
    struct bdk_gits_pidr7_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_pidr7_s cn; */
};
typedef union bdk_gits_pidr7 bdk_gits_pidr7_t;

#define BDK_GITS_PIDR7 BDK_GITS_PIDR7_FUNC()
static inline uint64_t BDK_GITS_PIDR7_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_PIDR7_FUNC(void)
{
    return 0x80100002ffdcll;
}

#define typedef_BDK_GITS_PIDR7 bdk_gits_pidr7_t
#define bustype_BDK_GITS_PIDR7 BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_PIDR7 "GITS_PIDR7"
#define device_bar_BDK_GITS_PIDR7 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_PIDR7 0
#define arguments_BDK_GITS_PIDR7 -1,-1,-1,-1

/**
 * Register (NCB32b) gits_translater
 *
 * GIC ITS Translate Register
 * This 32-bit register is write-only. The value written to this register specifies an interrupt
 * identifier to be translated for the requesting device.
 * A unique device identifier is provided for each requesting device and this is presented to the
 * ITS on writes to this register. This device identifier
 * is used to index a device table that maps the incoming device identifier to an interrupt
 * translation table for that device.
 *
 * Note that writes to this register with a device identifier that has not been mapped will be
 * ignored.
 *
 * Note that writes to this register with a device identifier that exceed the supported device
 * identifier size will be ignored.
 *
 * Note that this register is provided to enable the generation (and translation) of message
 * based interrupts from devices (e.g. MSI or MSI-X writes from PCIe devices).
 *
 * The register is at the same offset as GICD_SETSPI_NSR in the distributor and GICR()_SETLPIR in
 * the redistributor to allow virtualization of guest operating systems
 * that directly program devices simply by ensuring the address programmed by the guest can be
 * translated by an SMMU to target GITS_TRANSLATER.
 */
union bdk_gits_translater
{
    uint32_t u;
    struct bdk_gits_translater_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t int_id                : 32; /**< [ 31:  0](WO/H) Interrupt ID. The ID of interrupt to be translated for the requesting device.

                                                                 Note: the number of interrupt identifier bits is defined by
                                                                 GITS_TYPER[IDBITS]. Nonzero identifier bits outside this range are ignored.

                                                                 Note: 16-bit access to bits \<15:0\> of this register must be supported. When written by a
                                                                 16-bit transaction, bits \<31:16\> are written as zero. This register can not be accessed by
                                                                 CPU. */
#else /* Word 0 - Little Endian */
        uint32_t int_id                : 32; /**< [ 31:  0](WO/H) Interrupt ID. The ID of interrupt to be translated for the requesting device.

                                                                 Note: the number of interrupt identifier bits is defined by
                                                                 GITS_TYPER[IDBITS]. Nonzero identifier bits outside this range are ignored.

                                                                 Note: 16-bit access to bits \<15:0\> of this register must be supported. When written by a
                                                                 16-bit transaction, bits \<31:16\> are written as zero. This register can not be accessed by
                                                                 CPU. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gits_translater_s cn; */
};
typedef union bdk_gits_translater bdk_gits_translater_t;

#define BDK_GITS_TRANSLATER BDK_GITS_TRANSLATER_FUNC()
static inline uint64_t BDK_GITS_TRANSLATER_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_TRANSLATER_FUNC(void)
{
    return 0x801000030040ll;
}

#define typedef_BDK_GITS_TRANSLATER bdk_gits_translater_t
#define bustype_BDK_GITS_TRANSLATER BDK_CSR_TYPE_NCB32b
#define basename_BDK_GITS_TRANSLATER "GITS_TRANSLATER"
#define device_bar_BDK_GITS_TRANSLATER 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_TRANSLATER 0
#define arguments_BDK_GITS_TRANSLATER -1,-1,-1,-1

/**
 * Register (NCB) gits_typer
 *
 * GIC ITS Type Register
 * This register describes features supported by the ITS.
 */
union bdk_gits_typer
{
    uint64_t u;
    struct bdk_gits_typer_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_37_63        : 27;
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit collection ID, GITS_TYPER[CID_BITS] is RES0.
                                                                 1 = GITS_TYPER[CID_BITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit
                                                                 reads as zero and number of collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.

                                                                 Internal:
                                                                 Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on,
                                                                 software must ensure that any hardware collections
                                                                 are re-mapped following power-on. */
        uint64_t reserved_20_23        : 4;
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported. See section 4.9.16.
                                                                   0 = Target addresses correspond to linear processor numbers. See section 5.4.6.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-its device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t cct                   : 1;  /**< [  2:  2](RAZ) Memory backed collection is not supported. */
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and direct injection of Virtual LPIs supported.
                                                                 This field is zero in GICv3 implementations. */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
#else /* Word 0 - Little Endian */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and direct injection of Virtual LPIs supported.
                                                                 This field is zero in GICv3 implementations. */
        uint64_t cct                   : 1;  /**< [  2:  2](RAZ) Memory backed collection is not supported. */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-its device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported. See section 4.9.16.
                                                                   0 = Target addresses correspond to linear processor numbers. See section 5.4.6.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t reserved_20_23        : 4;
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.

                                                                 Internal:
                                                                 Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on,
                                                                 software must ensure that any hardware collections
                                                                 are re-mapped following power-on. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit collection ID, GITS_TYPER[CID_BITS] is RES0.
                                                                 1 = GITS_TYPER[CID_BITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit
                                                                 reads as zero and number of collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t reserved_37_63        : 27;
#endif /* Word 0 - End */
    } s;
    struct bdk_gits_typer_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.

                                                                 Internal:
                                                                 Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on,
                                                                 software must ensure that any hardware collections
                                                                 are re-mapped following power-on. */
        uint64_t reserved_20_23        : 4;
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported. See section 4.9.16.
                                                                   0 = Target addresses correspond to linear processor numbers. See section 5.4.6.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-its device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t reserved_1_2          : 2;
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
#else /* Word 0 - Little Endian */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
        uint64_t reserved_1_2          : 2;
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-its device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported. See section 4.9.16.
                                                                   0 = Target addresses correspond to linear processor numbers. See section 5.4.6.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t reserved_20_23        : 4;
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.

                                                                 Internal:
                                                                 Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on,
                                                                 software must ensure that any hardware collections
                                                                 are re-mapped following power-on. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_gits_typer_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_37_63        : 27;
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit collection ID, GITS_TYPER[CID_BITS] is RES0.
                                                                 1 = GITS_TYPER[CID_BITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit
                                                                 reads as zero and number of collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.
                                                                 NOTE: Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on, software must ensure that any hardware collections are remapped following power-on.
                                                                 A powered back on event is defined as cold reset is asserted and the deasserted from ITS
                                                                 point of view. */
        uint64_t reserved_20_23        : 4;
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported.
                                                                   0 = Target addresses correspond to linear processor numbers.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 24-bit device ID is defined
                                                                 as {node_id[1:0], iob_id[1:0], ecam_id[3:0], stream_id[15:0]}. */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t cct                   : 1;  /**< [  2:  2](RAZ) Memory backed collection is not supported. */
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and direct injection of Virtual LPIs supported.
                                                                 This field is zero in GICv3 implementations. */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
#else /* Word 0 - Little Endian */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and direct injection of Virtual LPIs supported.
                                                                 This field is zero in GICv3 implementations. */
        uint64_t cct                   : 1;  /**< [  2:  2](RAZ) Memory backed collection is not supported. */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 24-bit device ID is defined
                                                                 as {node_id[1:0], iob_id[1:0], ecam_id[3:0], stream_id[15:0]}. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported.
                                                                   0 = Target addresses correspond to linear processor numbers.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t reserved_20_23        : 4;
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.
                                                                 NOTE: Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on, software must ensure that any hardware collections are remapped following power-on.
                                                                 A powered back on event is defined as cold reset is asserted and the deasserted from ITS
                                                                 point of view. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit collection ID, GITS_TYPER[CID_BITS] is RES0.
                                                                 1 = GITS_TYPER[CID_BITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit
                                                                 reads as zero and number of collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t reserved_37_63        : 27;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_gits_typer_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_37_63        : 27;
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit collection ID, GITS_TYPER[CID_BITS] is RES0.
                                                                 1 = GITS_TYPER[CID_BITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit
                                                                 reads as zero and number of collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.
                                                                 NOTE: Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on, software must ensure that any hardware collections are remapped following power-on.
                                                                 A powered back on event is defined as cold reset is asserted and the deasserted from ITS
                                                                 point of view. */
        uint64_t reserved_20_23        : 4;
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported.
                                                                   0 = Target addresses correspond to linear processor numbers.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-bit device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t reserved_2            : 1;
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and direct injection of Virtual LPIs supported.
                                                                 This field is zero in GICv3 implementations. */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
#else /* Word 0 - Little Endian */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and direct injection of Virtual LPIs supported.
                                                                 This field is zero in GICv3 implementations. */
        uint64_t reserved_2            : 1;
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-bit device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported.
                                                                   0 = Target addresses correspond to linear processor numbers.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t reserved_20_23        : 4;
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.
                                                                 NOTE: Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on, software must ensure that any hardware collections are remapped following power-on.
                                                                 A powered back on event is defined as cold reset is asserted and the deasserted from ITS
                                                                 point of view. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit collection ID, GITS_TYPER[CID_BITS] is RES0.
                                                                 1 = GITS_TYPER[CID_BITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit
                                                                 reads as zero and number of collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t reserved_37_63        : 27;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gits_typer_cn81xx cn83xx; */
    struct bdk_gits_typer_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_37_63        : 27;
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit Collection ID, GITS_TYPER[CIDbits] is RES0.
                                                                 1 = GITS_TYPER[CIDBITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit is
                                                                 RAZ and number of Collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.

                                                                 NOTE: Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on, software must ensure that any hardware collections are remapped following power-on.
                                                                 A powered back on event is defined as cold reset is asserted and the deasserted from ITS
                                                                 point of view. */
        uint64_t reserved_20_23        : 4;
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported.
                                                                   0 = Target addresses correspond to linear processor numbers.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-bit device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t reserved_2            : 1;
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and Direct injection of Virtual LPIs supported. This field is
                                                                 clear in GICv3 implementations. */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
#else /* Word 0 - Little Endian */
        uint64_t physical              : 1;  /**< [  0:  0](RO) Reserved, one. */
        uint64_t vlpi                  : 1;  /**< [  1:  1](RAZ) Reserved. Virtual LPIs and Direct injection of Virtual LPIs supported. This field is
                                                                 clear in GICv3 implementations. */
        uint64_t reserved_2            : 1;
        uint64_t distributed           : 1;  /**< [  3:  3](RO) Distributed ITS implementation supported. */
        uint64_t itte_size             : 4;  /**< [  7:  4](RO) ITT entry size. Number of bytes per entry, minus one. The ITT entry size
                                                                 implemented is four bytes (32-bit). */
        uint64_t idbits                : 5;  /**< [ 12:  8](RO) The number of interrupt identifier bits supported, minus one. */
        uint64_t devbits               : 5;  /**< [ 17: 13](RO) The number of device identifier bits supported, minus one. The 21-bit device ID is defined
                                                                 as {node_id[1:0], iob_id[2:0], stream_id[15:0]}. */
        uint64_t seis                  : 1;  /**< [ 18: 18](RO) Locally generated system error interrupts supported. */
        uint64_t pta                   : 1;  /**< [ 19: 19](RO) Physical target addresses supported.
                                                                   0 = Target addresses correspond to linear processor numbers.
                                                                   1 = Target addresses correspond to the base physical address of re-distributors. */
        uint64_t reserved_20_23        : 4;
        uint64_t hcc                   : 8;  /**< [ 31: 24](RO) Hardware collection count. The number of collections supported by the ITS without
                                                                 provisioning of external memory. If this field is nonzero,
                                                                 collections in the range zero to (HCC minus one) are solely maintained in storage within
                                                                 the ITS.

                                                                 NOTE: Note when this field is nonzero and an ITS is dynamically powered-off and back
                                                                 on, software must ensure that any hardware collections are remapped following power-on.
                                                                 A powered back on event is defined as cold reset is asserted and the deasserted from ITS
                                                                 point of view. */
        uint64_t cid_bits              : 4;  /**< [ 35: 32](RAZ) Number of collection ID bits. The number of bits of collection ID - 1.
                                                                 When GITS_TYPER.CIL==0, this field is RES0. */
        uint64_t cil                   : 1;  /**< [ 36: 36](RAZ) 0 = ITS supports 16-bit Collection ID, GITS_TYPER[CIDbits] is RES0.
                                                                 1 = GITS_TYPER[CIDBITS] indicates supported collection ID size
                                                                 CNXXXX implementations do not support collections in external memory, this bit is
                                                                 RAZ and number of Collections supported is reported by GITS_TYPER[HCC]. */
        uint64_t reserved_37_63        : 27;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_gits_typer bdk_gits_typer_t;

#define BDK_GITS_TYPER BDK_GITS_TYPER_FUNC()
static inline uint64_t BDK_GITS_TYPER_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GITS_TYPER_FUNC(void)
{
    return 0x801000020008ll;
}

#define typedef_BDK_GITS_TYPER bdk_gits_typer_t
#define bustype_BDK_GITS_TYPER BDK_CSR_TYPE_NCB
#define basename_BDK_GITS_TYPER "GITS_TYPER"
#define device_bar_BDK_GITS_TYPER 0x2 /* PF_BAR2 */
#define busnum_BDK_GITS_TYPER 0
#define arguments_BDK_GITS_TYPER -1,-1,-1,-1

#endif /* __BDK_CSRS_GIC_H__ */
