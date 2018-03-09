#ifndef __BDK_CSRS_OCX_H__
#define __BDK_CSRS_OCX_H__
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
 * Cavium OCX.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration ocx_bar_e
 *
 * OCX Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_OCX_BAR_E_OCX_PF_BAR0 (0x87e011000000ll)
#define BDK_OCX_BAR_E_OCX_PF_BAR0_SIZE 0x800000ull
#define BDK_OCX_BAR_E_OCX_PF_BAR4 (0x87e011f00000ll)
#define BDK_OCX_BAR_E_OCX_PF_BAR4_SIZE 0x100000ull

/**
 * Enumeration ocx_int_vec_e
 *
 * OCX MSI-X Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_OCX_INT_VEC_E_COM_INTS (3)
#define BDK_OCX_INT_VEC_E_LNK_INTSX(a) (0 + (a))

/**
 * Register (RSL) ocx_com_bist_status
 *
 * OCX COM Memory BIST Status Register
 * Contains status from last memory BIST for all RX FIFO memories. BIST status for TX FIFO
 * memories and REPLAY memories are organized by link and are located in
 * OCX_TLK()_BIST_STATUS.
 */
union bdk_ocx_com_bist_status
{
    uint64_t u;
    struct bdk_ocx_com_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t status                : 36; /**< [ 35:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<35:34\> = Link 2 VC12            RX FIFOs.
                                                                 \<33:32\> = Link 2 VC4/VC2         RX FIFOs.
                                                                 \<31:30\> = Link 2 VC10/VC8/VC6    RX FIFOs. (Reserved in pass 2)
                                                                 \<29:28\> = Link 1 VC12            RX FIFOs.
                                                                 \<27:26\> = Link 1 VC4/VC2         RX FIFOs.
                                                                 \<25:24\> = Link 1 VC10/VC8/VC6    RX FIFOs. (Reserved in pass 2)
                                                                 \<23:22\> = Link 0 VC12            RX FIFOs.
                                                                 \<21:20\> = Link 0 VC4/VC2         RX FIFOs.
                                                                 \<19:18\> = Link 0 VC10/VC8/VC6    RX FIFOs. (Reserved in pass 2)
                                                                 \<17:16\> = Link 2 VC1/VC0         RX FIFOs.
                                                                 \<15:14\> = Link 2 VC5/VC3         RX FIFOs.
                                                                 \<13:12\> = Link 2 VC11/VC9/VC7    RX FIFOs. (Reserved in pass 2)
                                                                 \<11:10\> = Link 1 VC1/VC0         RX FIFOs.
                                                                 \<9:8\>   = Link 1 VC5/VC3         RX FIFOs.
                                                                 \<7:6\>   = Link 1 VC11/VC9/VC7    RX FIFOs. (Reserved in pass 2)
                                                                 \<5:4\>   = Link 0 VC1/VC0         RX FIFOs.
                                                                 \<3:2\>   = Link 0 VC5/VC3         RX FIFOs.
                                                                 \<1:0\>   = Link 0 VC11/VC9/VC7    RX FIFOs. (Reserved in pass 2) */
#else /* Word 0 - Little Endian */
        uint64_t status                : 36; /**< [ 35:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<35:34\> = Link 2 VC12            RX FIFOs.
                                                                 \<33:32\> = Link 2 VC4/VC2         RX FIFOs.
                                                                 \<31:30\> = Link 2 VC10/VC8/VC6    RX FIFOs. (Reserved in pass 2)
                                                                 \<29:28\> = Link 1 VC12            RX FIFOs.
                                                                 \<27:26\> = Link 1 VC4/VC2         RX FIFOs.
                                                                 \<25:24\> = Link 1 VC10/VC8/VC6    RX FIFOs. (Reserved in pass 2)
                                                                 \<23:22\> = Link 0 VC12            RX FIFOs.
                                                                 \<21:20\> = Link 0 VC4/VC2         RX FIFOs.
                                                                 \<19:18\> = Link 0 VC10/VC8/VC6    RX FIFOs. (Reserved in pass 2)
                                                                 \<17:16\> = Link 2 VC1/VC0         RX FIFOs.
                                                                 \<15:14\> = Link 2 VC5/VC3         RX FIFOs.
                                                                 \<13:12\> = Link 2 VC11/VC9/VC7    RX FIFOs. (Reserved in pass 2)
                                                                 \<11:10\> = Link 1 VC1/VC0         RX FIFOs.
                                                                 \<9:8\>   = Link 1 VC5/VC3         RX FIFOs.
                                                                 \<7:6\>   = Link 1 VC11/VC9/VC7    RX FIFOs. (Reserved in pass 2)
                                                                 \<5:4\>   = Link 0 VC1/VC0         RX FIFOs.
                                                                 \<3:2\>   = Link 0 VC5/VC3         RX FIFOs.
                                                                 \<1:0\>   = Link 0 VC11/VC9/VC7    RX FIFOs. (Reserved in pass 2) */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_bist_status_s cn; */
};
typedef union bdk_ocx_com_bist_status bdk_ocx_com_bist_status_t;

#define BDK_OCX_COM_BIST_STATUS BDK_OCX_COM_BIST_STATUS_FUNC()
static inline uint64_t BDK_OCX_COM_BIST_STATUS_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_BIST_STATUS_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0110000f0ll;
    __bdk_csr_fatal("OCX_COM_BIST_STATUS", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_BIST_STATUS bdk_ocx_com_bist_status_t
#define bustype_BDK_OCX_COM_BIST_STATUS BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_BIST_STATUS "OCX_COM_BIST_STATUS"
#define device_bar_BDK_OCX_COM_BIST_STATUS 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_BIST_STATUS 0
#define arguments_BDK_OCX_COM_BIST_STATUS -1,-1,-1,-1

/**
 * Register (RSL) ocx_com_dual_sort
 *
 * OCX COM Dual Sort Register
 */
union bdk_ocx_com_dual_sort
{
    uint64_t u;
    struct bdk_ocx_com_dual_sort_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t sort                  : 2;  /**< [  1:  0](R/W) Sorting procedure for multiple links to same node:
                                                                 0x0 = All to lowest link number.
                                                                 0x1 = Split by top/bottom L2C buses. (top to lowest link number).
                                                                 0x2 = IOC 1st, IOR 2nd, Mem VCs to either based on most room in TX FIFOs.
                                                                 0x3 = Illegal. */
#else /* Word 0 - Little Endian */
        uint64_t sort                  : 2;  /**< [  1:  0](R/W) Sorting procedure for multiple links to same node:
                                                                 0x0 = All to lowest link number.
                                                                 0x1 = Split by top/bottom L2C buses. (top to lowest link number).
                                                                 0x2 = IOC 1st, IOR 2nd, Mem VCs to either based on most room in TX FIFOs.
                                                                 0x3 = Illegal. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_dual_sort_s cn; */
};
typedef union bdk_ocx_com_dual_sort bdk_ocx_com_dual_sort_t;

#define BDK_OCX_COM_DUAL_SORT BDK_OCX_COM_DUAL_SORT_FUNC()
static inline uint64_t BDK_OCX_COM_DUAL_SORT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_DUAL_SORT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000008ll;
    __bdk_csr_fatal("OCX_COM_DUAL_SORT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_DUAL_SORT bdk_ocx_com_dual_sort_t
#define bustype_BDK_OCX_COM_DUAL_SORT BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_DUAL_SORT "OCX_COM_DUAL_SORT"
#define device_bar_BDK_OCX_COM_DUAL_SORT 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_DUAL_SORT 0
#define arguments_BDK_OCX_COM_DUAL_SORT -1,-1,-1,-1

/**
 * Register (RSL) ocx_com_int
 *
 * OCX COM Interrupt Register
 */
union bdk_ocx_com_int
{
    uint64_t u;
    struct bdk_ocx_com_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_55_63        : 9;
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1C/H) I/O request or response cannot be sent because a link was not found with a packet node ID
                                                                 matching the OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit set.
                                                                 Transaction has been dropped.  Should not occur during normal operation. This may indicate
                                                                 a software/configuration failure and may be considered fatal. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1C/H) Memory request or response cannot be sent because a link was not found with a packet node
                                                                 ID matching the OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit set.
                                                                 Transaction has been dropped.  Should not occur during normal operation. This may indicate
                                                                 a software/configuration failure and may be considered fatal. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1C/H) Scheduler add work or buffer pool return cannot be sent because a link was not found with
                                                                 a node ID matching the OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit
                                                                 set.
                                                                 Transaction has been dropped.  Should not occur during normal operation. This may indicate
                                                                 a software/configuration failure and may be considered fatal. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1C/H) Window request specified in SLI_WIN_RD_ADDR, SLI_WIN_WR_ADDR, OCX_WIN_CMD or OCX_PP_CMD
                                                                 cannot be sent because a link was not found with a request node ID matching the
                                                                 OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit set.  Transaction has
                                                                 been
                                                                 dropped.  Should not occur during normal operation. This may indicate a
                                                                 software/configuration failure and may be considered fatal. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1C/H) Window or core request was dropped because it could not be send during the period
                                                                 specified by OCX_WIN_TIMER.  Should not occur during normal operation. This may indicate a
                                                                 software/configuration failure and may be considered fatal. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1C/H) Window request specified in SLI_WIN_RD_ADDR, SLI_WIN_WR_ADDR, OCX_WIN_CMD or OCX_PP_CMD
                                                                 has been scheduled for transmission. If the command was not expecting a response, then a
                                                                 new command may be issued. */
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1C/H) A response to a previous window request or core request has been received. A new command
                                                                 may be issued. */
        uint64_t reserved_24_47        : 24;
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1C/H) SerDes RX lane interrupt. See OCX_LNE(0..23)_INT for more information. */
#else /* Word 0 - Little Endian */
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1C/H) SerDes RX lane interrupt. See OCX_LNE(0..23)_INT for more information. */
        uint64_t reserved_24_47        : 24;
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1C/H) A response to a previous window request or core request has been received. A new command
                                                                 may be issued. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1C/H) Window request specified in SLI_WIN_RD_ADDR, SLI_WIN_WR_ADDR, OCX_WIN_CMD or OCX_PP_CMD
                                                                 has been scheduled for transmission. If the command was not expecting a response, then a
                                                                 new command may be issued. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1C/H) Window or core request was dropped because it could not be send during the period
                                                                 specified by OCX_WIN_TIMER.  Should not occur during normal operation. This may indicate a
                                                                 software/configuration failure and may be considered fatal. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1C/H) Window request specified in SLI_WIN_RD_ADDR, SLI_WIN_WR_ADDR, OCX_WIN_CMD or OCX_PP_CMD
                                                                 cannot be sent because a link was not found with a request node ID matching the
                                                                 OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit set.  Transaction has
                                                                 been
                                                                 dropped.  Should not occur during normal operation. This may indicate a
                                                                 software/configuration failure and may be considered fatal. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1C/H) Scheduler add work or buffer pool return cannot be sent because a link was not found with
                                                                 a node ID matching the OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit
                                                                 set.
                                                                 Transaction has been dropped.  Should not occur during normal operation. This may indicate
                                                                 a software/configuration failure and may be considered fatal. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1C/H) Memory request or response cannot be sent because a link was not found with a packet node
                                                                 ID matching the OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit set.
                                                                 Transaction has been dropped.  Should not occur during normal operation. This may indicate
                                                                 a software/configuration failure and may be considered fatal. */
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1C/H) I/O request or response cannot be sent because a link was not found with a packet node ID
                                                                 matching the OCX_COM_LINK(0..2)_CTL[ID] with OCX_COM_LINK(0..2)_CTL[VALID] bit set.
                                                                 Transaction has been dropped.  Should not occur during normal operation. This may indicate
                                                                 a software/configuration failure and may be considered fatal. */
        uint64_t reserved_55_63        : 9;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_int_s cn; */
};
typedef union bdk_ocx_com_int bdk_ocx_com_int_t;

#define BDK_OCX_COM_INT BDK_OCX_COM_INT_FUNC()
static inline uint64_t BDK_OCX_COM_INT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_INT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000100ll;
    __bdk_csr_fatal("OCX_COM_INT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_INT bdk_ocx_com_int_t
#define bustype_BDK_OCX_COM_INT BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_INT "OCX_COM_INT"
#define device_bar_BDK_OCX_COM_INT 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_INT 0
#define arguments_BDK_OCX_COM_INT -1,-1,-1,-1

/**
 * Register (RSL) ocx_com_int_ena_w1c
 *
 * OCX COM Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_ocx_com_int_ena_w1c
{
    uint64_t u;
    struct bdk_ocx_com_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_55_63        : 9;
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1C/H) Reads or clears enable for OCX_COM_INT[IO_BADID]. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1C/H) Reads or clears enable for OCX_COM_INT[MEM_BADID]. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1C/H) Reads or clears enable for OCX_COM_INT[COPR_BADID]. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_REQ_BADID]. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_REQ_TOUT]. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_REQ_XMIT]. */
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_RSP]. */
        uint64_t reserved_24_47        : 24;
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1C/H) Reads or clears enable for OCX_COM_INT[RX_LANE]. */
#else /* Word 0 - Little Endian */
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1C/H) Reads or clears enable for OCX_COM_INT[RX_LANE]. */
        uint64_t reserved_24_47        : 24;
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_RSP]. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_REQ_XMIT]. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_REQ_TOUT]. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1C/H) Reads or clears enable for OCX_COM_INT[WIN_REQ_BADID]. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1C/H) Reads or clears enable for OCX_COM_INT[COPR_BADID]. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1C/H) Reads or clears enable for OCX_COM_INT[MEM_BADID]. */
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1C/H) Reads or clears enable for OCX_COM_INT[IO_BADID]. */
        uint64_t reserved_55_63        : 9;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_int_ena_w1c_s cn; */
};
typedef union bdk_ocx_com_int_ena_w1c bdk_ocx_com_int_ena_w1c_t;

#define BDK_OCX_COM_INT_ENA_W1C BDK_OCX_COM_INT_ENA_W1C_FUNC()
static inline uint64_t BDK_OCX_COM_INT_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_INT_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000118ll;
    __bdk_csr_fatal("OCX_COM_INT_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_INT_ENA_W1C bdk_ocx_com_int_ena_w1c_t
#define bustype_BDK_OCX_COM_INT_ENA_W1C BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_INT_ENA_W1C "OCX_COM_INT_ENA_W1C"
#define device_bar_BDK_OCX_COM_INT_ENA_W1C 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_INT_ENA_W1C 0
#define arguments_BDK_OCX_COM_INT_ENA_W1C -1,-1,-1,-1

/**
 * Register (RSL) ocx_com_int_ena_w1s
 *
 * OCX COM Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_ocx_com_int_ena_w1s
{
    uint64_t u;
    struct bdk_ocx_com_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_55_63        : 9;
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1S/H) Reads or sets enable for OCX_COM_INT[IO_BADID]. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1S/H) Reads or sets enable for OCX_COM_INT[MEM_BADID]. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1S/H) Reads or sets enable for OCX_COM_INT[COPR_BADID]. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_REQ_BADID]. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_REQ_TOUT]. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_REQ_XMIT]. */
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_RSP]. */
        uint64_t reserved_24_47        : 24;
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1S/H) Reads or sets enable for OCX_COM_INT[RX_LANE]. */
#else /* Word 0 - Little Endian */
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1S/H) Reads or sets enable for OCX_COM_INT[RX_LANE]. */
        uint64_t reserved_24_47        : 24;
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_RSP]. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_REQ_XMIT]. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_REQ_TOUT]. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1S/H) Reads or sets enable for OCX_COM_INT[WIN_REQ_BADID]. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1S/H) Reads or sets enable for OCX_COM_INT[COPR_BADID]. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1S/H) Reads or sets enable for OCX_COM_INT[MEM_BADID]. */
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1S/H) Reads or sets enable for OCX_COM_INT[IO_BADID]. */
        uint64_t reserved_55_63        : 9;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_int_ena_w1s_s cn; */
};
typedef union bdk_ocx_com_int_ena_w1s bdk_ocx_com_int_ena_w1s_t;

#define BDK_OCX_COM_INT_ENA_W1S BDK_OCX_COM_INT_ENA_W1S_FUNC()
static inline uint64_t BDK_OCX_COM_INT_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_INT_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000110ll;
    __bdk_csr_fatal("OCX_COM_INT_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_INT_ENA_W1S bdk_ocx_com_int_ena_w1s_t
#define bustype_BDK_OCX_COM_INT_ENA_W1S BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_INT_ENA_W1S "OCX_COM_INT_ENA_W1S"
#define device_bar_BDK_OCX_COM_INT_ENA_W1S 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_INT_ENA_W1S 0
#define arguments_BDK_OCX_COM_INT_ENA_W1S -1,-1,-1,-1

/**
 * Register (RSL) ocx_com_int_w1s
 *
 * OCX COM Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_ocx_com_int_w1s
{
    uint64_t u;
    struct bdk_ocx_com_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_55_63        : 9;
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1S/H) Reads or sets OCX_COM_INT[IO_BADID]. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1S/H) Reads or sets OCX_COM_INT[MEM_BADID]. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1S/H) Reads or sets OCX_COM_INT[COPR_BADID]. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1S/H) Reads or sets OCX_COM_INT[WIN_REQ_BADID]. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1S/H) Reads or sets OCX_COM_INT[WIN_REQ_TOUT]. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1S/H) Reads or sets OCX_COM_INT[WIN_REQ_XMIT]. */
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1S/H) Reads or sets OCX_COM_INT[WIN_RSP]. */
        uint64_t reserved_24_47        : 24;
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1S/H) Reads or sets OCX_COM_INT[RX_LANE]. */
#else /* Word 0 - Little Endian */
        uint64_t rx_lane               : 24; /**< [ 23:  0](R/W1S/H) Reads or sets OCX_COM_INT[RX_LANE]. */
        uint64_t reserved_24_47        : 24;
        uint64_t win_rsp               : 1;  /**< [ 48: 48](R/W1S/H) Reads or sets OCX_COM_INT[WIN_RSP]. */
        uint64_t win_req_xmit          : 1;  /**< [ 49: 49](R/W1S/H) Reads or sets OCX_COM_INT[WIN_REQ_XMIT]. */
        uint64_t win_req_tout          : 1;  /**< [ 50: 50](R/W1S/H) Reads or sets OCX_COM_INT[WIN_REQ_TOUT]. */
        uint64_t win_req_badid         : 1;  /**< [ 51: 51](R/W1S/H) Reads or sets OCX_COM_INT[WIN_REQ_BADID]. */
        uint64_t copr_badid            : 1;  /**< [ 52: 52](R/W1S/H) Reads or sets OCX_COM_INT[COPR_BADID]. */
        uint64_t mem_badid             : 1;  /**< [ 53: 53](R/W1S/H) Reads or sets OCX_COM_INT[MEM_BADID]. */
        uint64_t io_badid              : 1;  /**< [ 54: 54](R/W1S/H) Reads or sets OCX_COM_INT[IO_BADID]. */
        uint64_t reserved_55_63        : 9;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_int_w1s_s cn; */
};
typedef union bdk_ocx_com_int_w1s bdk_ocx_com_int_w1s_t;

#define BDK_OCX_COM_INT_W1S BDK_OCX_COM_INT_W1S_FUNC()
static inline uint64_t BDK_OCX_COM_INT_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_INT_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000108ll;
    __bdk_csr_fatal("OCX_COM_INT_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_INT_W1S bdk_ocx_com_int_w1s_t
#define bustype_BDK_OCX_COM_INT_W1S BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_INT_W1S "OCX_COM_INT_W1S"
#define device_bar_BDK_OCX_COM_INT_W1S 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_INT_W1S 0
#define arguments_BDK_OCX_COM_INT_W1S -1,-1,-1,-1

/**
 * Register (RSL) ocx_com_link#_ctl
 *
 * OCX COM Link Control Registers
 * This register controls link operations.  In addition, the combination of some of
 * these conditions are used to generate the link_down status used by the L2C_OCI_CTL[SHTOEN] and
 * as a reset condition controlled by RST_OCX[RST_LINK].  This link_down status is true when one
 * of the following occurs:
 *
 * * Link is not initialized (see description of [UP]).
 * * Retry counter expired (see OCX_COM_LINK_TIMER and OCX_COM_LINK()_INT[STOP].
 * * Receive REINIT request from Link Partner (See description of [REINIT]).
 * * Detected uncorrectable ECC error while reading the transmit FIFOs (see
 * OCX_COM_LINK(0..2)_INT[TXFIFO_DBE]).
 * * Detected uncorrectable ECC error while reading the replay buffer (see
 * OCX_COM_LINK(0..2)_INT[REPLAY_DBE]).
 */
union bdk_ocx_com_linkx_ctl
{
    uint64_t u;
    struct bdk_ocx_com_linkx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t cclk_dis              : 1;  /**< [  9:  9](R/W) Reserved.
                                                                 Internal:
                                                                 Disable conditional clocking.  Set to force link clocks on
                                                                 unconditionally. */
        uint64_t loopback              : 1;  /**< [  8:  8](R/W) Reserved.
                                                                 Internal:
                                                                 Diagnostic data loopback. Set to force outgoing link to inbound port.
                                                                 All data and link credits are returned and appear to come from link partner. Typically
                                                                 SerDes should be disabled during this operation. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W) Reinitialize link. Setting this bit forces link back into init state and sets [DROP].
                                                                 Setting the bit also causes the link to transmit a REINIT request to the link partner.
                                                                 This bit must be cleared for link to operate normally. */
        uint64_t reserved_6            : 1;
        uint64_t auto_clr              : 1;  /**< [  5:  5](R/W) When set, automatically clears the local DROP bit if link partner forces
                                                                 a reinitialization.  Typically disabled once software is running.
                                                                 If clear, software must manage clearing [DROP] after it has verified
                                                                 that any pending transactions have timed out. */
        uint64_t drop                  : 1;  /**< [  4:  4](R/W/H) Drop all requests on given link. Typically set by hardware when link has failed or been
                                                                 reinitialized. Cleared by software once pending link traffic is removed. (See
                                                                 OCX_TLK(0..2)_FIFO(0..13)_CNT.) */
        uint64_t up                    : 1;  /**< [  3:  3](RO/H) Link is operating normally and exchanging control information. */
        uint64_t valid                 : 1;  /**< [  2:  2](RO/H) Link has valid lanes and is exchanging information. This bit will never be set if
                                                                 OCX_LNK(0..2)_CFG[QLM_SELECT] is zero. */
        uint64_t id                    : 2;  /**< [  1:  0](R/W) This ID is used to sort traffic by link. If more than one link has the same value, the
                                                                 OCX_COM_DUAL_SORT[SORT] field and traffic VC are used to choose a link. This field is only
                                                                 reset during a cold reset to an arbitrary value to avoid conflicts with the
                                                                 OCX_COM_NODE[ID] field and should be configured by software before memory traffic is
                                                                 generated. */
#else /* Word 0 - Little Endian */
        uint64_t id                    : 2;  /**< [  1:  0](R/W) This ID is used to sort traffic by link. If more than one link has the same value, the
                                                                 OCX_COM_DUAL_SORT[SORT] field and traffic VC are used to choose a link. This field is only
                                                                 reset during a cold reset to an arbitrary value to avoid conflicts with the
                                                                 OCX_COM_NODE[ID] field and should be configured by software before memory traffic is
                                                                 generated. */
        uint64_t valid                 : 1;  /**< [  2:  2](RO/H) Link has valid lanes and is exchanging information. This bit will never be set if
                                                                 OCX_LNK(0..2)_CFG[QLM_SELECT] is zero. */
        uint64_t up                    : 1;  /**< [  3:  3](RO/H) Link is operating normally and exchanging control information. */
        uint64_t drop                  : 1;  /**< [  4:  4](R/W/H) Drop all requests on given link. Typically set by hardware when link has failed or been
                                                                 reinitialized. Cleared by software once pending link traffic is removed. (See
                                                                 OCX_TLK(0..2)_FIFO(0..13)_CNT.) */
        uint64_t auto_clr              : 1;  /**< [  5:  5](R/W) When set, automatically clears the local DROP bit if link partner forces
                                                                 a reinitialization.  Typically disabled once software is running.
                                                                 If clear, software must manage clearing [DROP] after it has verified
                                                                 that any pending transactions have timed out. */
        uint64_t reserved_6            : 1;
        uint64_t reinit                : 1;  /**< [  7:  7](R/W) Reinitialize link. Setting this bit forces link back into init state and sets [DROP].
                                                                 Setting the bit also causes the link to transmit a REINIT request to the link partner.
                                                                 This bit must be cleared for link to operate normally. */
        uint64_t loopback              : 1;  /**< [  8:  8](R/W) Reserved.
                                                                 Internal:
                                                                 Diagnostic data loopback. Set to force outgoing link to inbound port.
                                                                 All data and link credits are returned and appear to come from link partner. Typically
                                                                 SerDes should be disabled during this operation. */
        uint64_t cclk_dis              : 1;  /**< [  9:  9](R/W) Reserved.
                                                                 Internal:
                                                                 Disable conditional clocking.  Set to force link clocks on
                                                                 unconditionally. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_linkx_ctl_s cn; */
};
typedef union bdk_ocx_com_linkx_ctl bdk_ocx_com_linkx_ctl_t;

static inline uint64_t BDK_OCX_COM_LINKX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_LINKX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011000020ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_COM_LINKX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_LINKX_CTL(a) bdk_ocx_com_linkx_ctl_t
#define bustype_BDK_OCX_COM_LINKX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_LINKX_CTL(a) "OCX_COM_LINKX_CTL"
#define device_bar_BDK_OCX_COM_LINKX_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_LINKX_CTL(a) (a)
#define arguments_BDK_OCX_COM_LINKX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_com_link#_int
 *
 * OCX COM Link Interrupt Register
 */
union bdk_ocx_com_linkx_int
{
    uint64_t u;
    struct bdk_ocx_com_linkx_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1C/H) Illegal word decoded on at least one lane of link. These receive errors may occur during
                                                                 normal operation, and may likely occur during link bringup. Hardware normally will
                                                                 automatically correct the error. Software may choose to count the number of these errors. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1C/H) Link lanes failed to align. These receive errors may occur during normal operation, and
                                                                 may likely occur during link bringup. Hardware normally will automatically correct the
                                                                 error. Software may choose to count the number of these errors. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1C/H) Link lane alignment is complete. These may occur during normal operation, and will occur
                                                                 during link bringup. Software should disable reception of these interrupts during normal
                                                                 operation. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1C/H) Link initialization is complete and is ready to pass traffic.  Note:  This state occurs
                                                                 some time after the link starts exchanging information as indicated in
                                                                 OCX_COM_LINK(0..2)_CTL[UP]. These should not occur during normal operation. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1C/H) Link has stopped operating. Link retry count has reached threshold specified in
                                                                 OCX_COM_LINK_TIMER; outgoing traffic has been dropped and an initialization request has
                                                                 been reissued. These should not occur during normal operation. This may be considered
                                                                 fatal. */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1C/H) Link block error count has reached threshold specified in OCX_RLK(0..2)_BLK_ERR[LIMIT].
                                                                 These receive errors may occur during normal operation. Hardware normally will
                                                                 automatically correct the error. Software may choose to count the number of these errors. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1C/H) Link has received a initialization request from link partner after link has been
                                                                 established. These should not occur during normal operation */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1C/H) Set by hardware when a link data block is received in OCX_RLK(0..2)_LNK_DATA. It
                                                                 software's responsibility to clear the bit after reading the data. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1C/H) Double-bit error detected in FIFO RAMs. This error may be considered fatal. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1C/H) Single-bit error detected/corrected in FIFO RAMs. Hardware automatically corrected the
                                                                 error. Software may choose to count the number of these single-bit errors. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1C/H) Double-bit error detected in TX FIFO RAMs. This error may be considered fatal. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1C/H) Single-bit error detected/corrected in TX FIFO RAMs. Hardware automatically corrected the
                                                                 error. Software may choose to count the number of these single-bit errors. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1C/H) Double-bit error detected in REPLAY BUFFER RAMs. This error may be considered fatal. */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1C/H) Single-bit error detected/corrected in REPLAY BUFFER RAMs. Hardware automatically
                                                                 corrected the error. Software may choose to count the number of these single-bit errors. */
#else /* Word 0 - Little Endian */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1C/H) Single-bit error detected/corrected in REPLAY BUFFER RAMs. Hardware automatically
                                                                 corrected the error. Software may choose to count the number of these single-bit errors. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1C/H) Double-bit error detected in REPLAY BUFFER RAMs. This error may be considered fatal. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1C/H) Single-bit error detected/corrected in TX FIFO RAMs. Hardware automatically corrected the
                                                                 error. Software may choose to count the number of these single-bit errors. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1C/H) Double-bit error detected in TX FIFO RAMs. This error may be considered fatal. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1C/H) Single-bit error detected/corrected in FIFO RAMs. Hardware automatically corrected the
                                                                 error. Software may choose to count the number of these single-bit errors. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1C/H) Double-bit error detected in FIFO RAMs. This error may be considered fatal. */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1C/H) Set by hardware when a link data block is received in OCX_RLK(0..2)_LNK_DATA. It
                                                                 software's responsibility to clear the bit after reading the data. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1C/H) Link has received a initialization request from link partner after link has been
                                                                 established. These should not occur during normal operation */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1C/H) Link block error count has reached threshold specified in OCX_RLK(0..2)_BLK_ERR[LIMIT].
                                                                 These receive errors may occur during normal operation. Hardware normally will
                                                                 automatically correct the error. Software may choose to count the number of these errors. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1C/H) Link has stopped operating. Link retry count has reached threshold specified in
                                                                 OCX_COM_LINK_TIMER; outgoing traffic has been dropped and an initialization request has
                                                                 been reissued. These should not occur during normal operation. This may be considered
                                                                 fatal. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1C/H) Link initialization is complete and is ready to pass traffic.  Note:  This state occurs
                                                                 some time after the link starts exchanging information as indicated in
                                                                 OCX_COM_LINK(0..2)_CTL[UP]. These should not occur during normal operation. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1C/H) Link lane alignment is complete. These may occur during normal operation, and will occur
                                                                 during link bringup. Software should disable reception of these interrupts during normal
                                                                 operation. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1C/H) Link lanes failed to align. These receive errors may occur during normal operation, and
                                                                 may likely occur during link bringup. Hardware normally will automatically correct the
                                                                 error. Software may choose to count the number of these errors. */
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1C/H) Illegal word decoded on at least one lane of link. These receive errors may occur during
                                                                 normal operation, and may likely occur during link bringup. Hardware normally will
                                                                 automatically correct the error. Software may choose to count the number of these errors. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_linkx_int_s cn; */
};
typedef union bdk_ocx_com_linkx_int bdk_ocx_com_linkx_int_t;

static inline uint64_t BDK_OCX_COM_LINKX_INT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_LINKX_INT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011000120ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_COM_LINKX_INT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_LINKX_INT(a) bdk_ocx_com_linkx_int_t
#define bustype_BDK_OCX_COM_LINKX_INT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_LINKX_INT(a) "OCX_COM_LINKX_INT"
#define device_bar_BDK_OCX_COM_LINKX_INT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_LINKX_INT(a) (a)
#define arguments_BDK_OCX_COM_LINKX_INT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_com_link#_int_ena_w1c
 *
 * OCX COM Link Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_ocx_com_linkx_int_ena_w1c
{
    uint64_t u;
    struct bdk_ocx_com_linkx_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[BAD_WORD]. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[ALIGN_FAIL]. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[ALIGN_DONE]. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[UP]. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[STOP]. */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[BLK_ERR]. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[REINIT]. */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[LNK_DATA]. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[RXFIFO_DBE]. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[RXFIFO_SBE]. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[TXFIFO_DBE]. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[TXFIFO_SBE]. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[REPLAY_DBE]. */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[REPLAY_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[REPLAY_SBE]. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[REPLAY_DBE]. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[TXFIFO_SBE]. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[TXFIFO_DBE]. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[RXFIFO_SBE]. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[RXFIFO_DBE]. */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[LNK_DATA]. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[REINIT]. */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[BLK_ERR]. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[STOP]. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[UP]. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[ALIGN_DONE]. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[ALIGN_FAIL]. */
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for OCX_COM_LINK(0..2)_INT[BAD_WORD]. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_linkx_int_ena_w1c_s cn; */
};
typedef union bdk_ocx_com_linkx_int_ena_w1c bdk_ocx_com_linkx_int_ena_w1c_t;

static inline uint64_t BDK_OCX_COM_LINKX_INT_ENA_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_LINKX_INT_ENA_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011000180ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_COM_LINKX_INT_ENA_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_LINKX_INT_ENA_W1C(a) bdk_ocx_com_linkx_int_ena_w1c_t
#define bustype_BDK_OCX_COM_LINKX_INT_ENA_W1C(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_LINKX_INT_ENA_W1C(a) "OCX_COM_LINKX_INT_ENA_W1C"
#define device_bar_BDK_OCX_COM_LINKX_INT_ENA_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_LINKX_INT_ENA_W1C(a) (a)
#define arguments_BDK_OCX_COM_LINKX_INT_ENA_W1C(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_com_link#_int_ena_w1s
 *
 * OCX COM Link Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_ocx_com_linkx_int_ena_w1s
{
    uint64_t u;
    struct bdk_ocx_com_linkx_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[BAD_WORD]. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[ALIGN_FAIL]. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[ALIGN_DONE]. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[UP]. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[STOP]. */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[BLK_ERR]. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[REINIT]. */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[LNK_DATA]. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[RXFIFO_DBE]. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[RXFIFO_SBE]. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[TXFIFO_DBE]. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[TXFIFO_SBE]. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[REPLAY_DBE]. */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[REPLAY_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[REPLAY_SBE]. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[REPLAY_DBE]. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[TXFIFO_SBE]. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[TXFIFO_DBE]. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[RXFIFO_SBE]. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[RXFIFO_DBE]. */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[LNK_DATA]. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[REINIT]. */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[BLK_ERR]. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[STOP]. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[UP]. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[ALIGN_DONE]. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[ALIGN_FAIL]. */
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for OCX_COM_LINK(0..2)_INT[BAD_WORD]. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_linkx_int_ena_w1s_s cn; */
};
typedef union bdk_ocx_com_linkx_int_ena_w1s bdk_ocx_com_linkx_int_ena_w1s_t;

static inline uint64_t BDK_OCX_COM_LINKX_INT_ENA_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_LINKX_INT_ENA_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011000160ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_COM_LINKX_INT_ENA_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_LINKX_INT_ENA_W1S(a) bdk_ocx_com_linkx_int_ena_w1s_t
#define bustype_BDK_OCX_COM_LINKX_INT_ENA_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_LINKX_INT_ENA_W1S(a) "OCX_COM_LINKX_INT_ENA_W1S"
#define device_bar_BDK_OCX_COM_LINKX_INT_ENA_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_LINKX_INT_ENA_W1S(a) (a)
#define arguments_BDK_OCX_COM_LINKX_INT_ENA_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_com_link#_int_w1s
 *
 * OCX COM Link Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_ocx_com_linkx_int_w1s
{
    uint64_t u;
    struct bdk_ocx_com_linkx_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[BAD_WORD]. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[ALIGN_FAIL]. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[ALIGN_DONE]. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[UP]. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[STOP]. */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[BLK_ERR]. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[REINIT]. */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[LNK_DATA]. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[RXFIFO_DBE]. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[RXFIFO_SBE]. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[TXFIFO_DBE]. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[TXFIFO_SBE]. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[REPLAY_DBE]. */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[REPLAY_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t replay_sbe            : 1;  /**< [  0:  0](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[REPLAY_SBE]. */
        uint64_t replay_dbe            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[REPLAY_DBE]. */
        uint64_t txfifo_sbe            : 1;  /**< [  2:  2](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[TXFIFO_SBE]. */
        uint64_t txfifo_dbe            : 1;  /**< [  3:  3](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[TXFIFO_DBE]. */
        uint64_t rxfifo_sbe            : 1;  /**< [  4:  4](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[RXFIFO_SBE]. */
        uint64_t rxfifo_dbe            : 1;  /**< [  5:  5](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[RXFIFO_DBE]. */
        uint64_t lnk_data              : 1;  /**< [  6:  6](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[LNK_DATA]. */
        uint64_t reinit                : 1;  /**< [  7:  7](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[REINIT]. */
        uint64_t blk_err               : 1;  /**< [  8:  8](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[BLK_ERR]. */
        uint64_t stop                  : 1;  /**< [  9:  9](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[STOP]. */
        uint64_t up                    : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[UP]. */
        uint64_t align_done            : 1;  /**< [ 11: 11](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[ALIGN_DONE]. */
        uint64_t align_fail            : 1;  /**< [ 12: 12](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[ALIGN_FAIL]. */
        uint64_t bad_word              : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets OCX_COM_LINK(0..2)_INT[BAD_WORD]. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_linkx_int_w1s_s cn; */
};
typedef union bdk_ocx_com_linkx_int_w1s bdk_ocx_com_linkx_int_w1s_t;

static inline uint64_t BDK_OCX_COM_LINKX_INT_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_LINKX_INT_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011000140ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_COM_LINKX_INT_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_LINKX_INT_W1S(a) bdk_ocx_com_linkx_int_w1s_t
#define bustype_BDK_OCX_COM_LINKX_INT_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_LINKX_INT_W1S(a) "OCX_COM_LINKX_INT_W1S"
#define device_bar_BDK_OCX_COM_LINKX_INT_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_LINKX_INT_W1S(a) (a)
#define arguments_BDK_OCX_COM_LINKX_INT_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_com_link_timer
 *
 * OCX COM Link Timer Register
 */
union bdk_ocx_com_link_timer
{
    uint64_t u;
    struct bdk_ocx_com_link_timer_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t tout                  : 24; /**< [ 23:  0](R/W) Indicates the number of unacknowledged retry requests issued before link stops
                                                                 operation and OCX_COM_LINK()_INT[STOP] is asserted. */
#else /* Word 0 - Little Endian */
        uint64_t tout                  : 24; /**< [ 23:  0](R/W) Indicates the number of unacknowledged retry requests issued before link stops
                                                                 operation and OCX_COM_LINK()_INT[STOP] is asserted. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_link_timer_s cn; */
};
typedef union bdk_ocx_com_link_timer bdk_ocx_com_link_timer_t;

#define BDK_OCX_COM_LINK_TIMER BDK_OCX_COM_LINK_TIMER_FUNC()
static inline uint64_t BDK_OCX_COM_LINK_TIMER_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_LINK_TIMER_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000010ll;
    __bdk_csr_fatal("OCX_COM_LINK_TIMER", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_LINK_TIMER bdk_ocx_com_link_timer_t
#define bustype_BDK_OCX_COM_LINK_TIMER BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_LINK_TIMER "OCX_COM_LINK_TIMER"
#define device_bar_BDK_OCX_COM_LINK_TIMER 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_LINK_TIMER 0
#define arguments_BDK_OCX_COM_LINK_TIMER -1,-1,-1,-1

/**
 * Register (RSL) ocx_com_node
 *
 * OCX COM Node Register
 */
union bdk_ocx_com_node
{
    uint64_t u;
    struct bdk_ocx_com_node_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t fixed_pin             : 1;  /**< [  3:  3](RO/H) The current value of the OCI_FIXED_NODE pin. */
        uint64_t fixed                 : 1;  /**< [  2:  2](R/W) ID valid associated with the chip. This register is used by the link
                                                                 initialization software to help assign IDs and is transmitted over CCPI. The
                                                                 [FIXED] field set during a cold reset to the value of the OCI_FIXED_NODE
                                                                 pin. The value is also readable in the OCX_LNE()_STS_MSG[TX_META_DAT]\<2\> for
                                                                 each lane. The [FIXED] field of the link partner can be examined by locally
                                                                 reading the OCX_LNE()_STS_MSG[RX_META_DAT]\<2\> on each valid lane or remotely
                                                                 reading the OCX_COM_NODE[FIXED] on the link partner. */
        uint64_t id                    : 2;  /**< [  1:  0](R/W) Node ID associated with the chip. This register is used by the rest of the chip
                                                                 to determine what traffic is transmitted over CCPI. The value should not match
                                                                 the OCX_COM_LINK()_CTL[ID] of any active link. The ID field is set during a cold
                                                                 reset to the value of the OCI_NODE_ID pins. The value is also readable in the
                                                                 OCX_LNE()_STS_MSG[TX_META_DAT]\<1:0\> for each lane. The ID field of the link
                                                                 partner can be examined by locally reading the
                                                                 OCX_LNE()_STS_MSG[RX_META_DAT]\<1:0\> on each valid lane or remotely reading the
                                                                 OCX_COM_NODE[ID] on the link partner. */
#else /* Word 0 - Little Endian */
        uint64_t id                    : 2;  /**< [  1:  0](R/W) Node ID associated with the chip. This register is used by the rest of the chip
                                                                 to determine what traffic is transmitted over CCPI. The value should not match
                                                                 the OCX_COM_LINK()_CTL[ID] of any active link. The ID field is set during a cold
                                                                 reset to the value of the OCI_NODE_ID pins. The value is also readable in the
                                                                 OCX_LNE()_STS_MSG[TX_META_DAT]\<1:0\> for each lane. The ID field of the link
                                                                 partner can be examined by locally reading the
                                                                 OCX_LNE()_STS_MSG[RX_META_DAT]\<1:0\> on each valid lane or remotely reading the
                                                                 OCX_COM_NODE[ID] on the link partner. */
        uint64_t fixed                 : 1;  /**< [  2:  2](R/W) ID valid associated with the chip. This register is used by the link
                                                                 initialization software to help assign IDs and is transmitted over CCPI. The
                                                                 [FIXED] field set during a cold reset to the value of the OCI_FIXED_NODE
                                                                 pin. The value is also readable in the OCX_LNE()_STS_MSG[TX_META_DAT]\<2\> for
                                                                 each lane. The [FIXED] field of the link partner can be examined by locally
                                                                 reading the OCX_LNE()_STS_MSG[RX_META_DAT]\<2\> on each valid lane or remotely
                                                                 reading the OCX_COM_NODE[FIXED] on the link partner. */
        uint64_t fixed_pin             : 1;  /**< [  3:  3](RO/H) The current value of the OCI_FIXED_NODE pin. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_com_node_s cn; */
};
typedef union bdk_ocx_com_node bdk_ocx_com_node_t;

#define BDK_OCX_COM_NODE BDK_OCX_COM_NODE_FUNC()
static inline uint64_t BDK_OCX_COM_NODE_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_COM_NODE_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000000ll;
    __bdk_csr_fatal("OCX_COM_NODE", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_COM_NODE bdk_ocx_com_node_t
#define bustype_BDK_OCX_COM_NODE BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_COM_NODE "OCX_COM_NODE"
#define device_bar_BDK_OCX_COM_NODE 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_COM_NODE 0
#define arguments_BDK_OCX_COM_NODE -1,-1,-1,-1

/**
 * Register (RSL) ocx_dll#_status
 *
 * OCX DLL Observability Registers
 * These registers provides the parameters for DLL observability.  Index 0 is the northeast DLL,
 * index 1 the southeast DLL.
 */
union bdk_ocx_dllx_status
{
    uint64_t u;
    struct bdk_ocx_dllx_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 35: 35](RO/H) Phase detector output. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 34: 34](RO/H) Phase detector output. */
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 33: 33](RO/H) Phase detector output. */
        uint64_t reserved_32           : 1;
        uint64_t dly_elem_enable       : 16; /**< [ 31: 16](RO/H) Delay element enable. */
        uint64_t dll_setting           : 12; /**< [ 15:  4](RO/H) DLL setting. */
        uint64_t reserved_1_3          : 3;
        uint64_t dll_lock              : 1;  /**< [  0:  0](RO/H) DLL lock: 1 = locked, 0 = unlocked. */
#else /* Word 0 - Little Endian */
        uint64_t dll_lock              : 1;  /**< [  0:  0](RO/H) DLL lock: 1 = locked, 0 = unlocked. */
        uint64_t reserved_1_3          : 3;
        uint64_t dll_setting           : 12; /**< [ 15:  4](RO/H) DLL setting. */
        uint64_t dly_elem_enable       : 16; /**< [ 31: 16](RO/H) Delay element enable. */
        uint64_t reserved_32           : 1;
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 33: 33](RO/H) Phase detector output. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 34: 34](RO/H) Phase detector output. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 35: 35](RO/H) Phase detector output. */
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_dllx_status_s cn; */
};
typedef union bdk_ocx_dllx_status bdk_ocx_dllx_status_t;

static inline uint64_t BDK_OCX_DLLX_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_DLLX_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e011000080ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("OCX_DLLX_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_DLLX_STATUS(a) bdk_ocx_dllx_status_t
#define bustype_BDK_OCX_DLLX_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_DLLX_STATUS(a) "OCX_DLLX_STATUS"
#define device_bar_BDK_OCX_DLLX_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_DLLX_STATUS(a) (a)
#define arguments_BDK_OCX_DLLX_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_frc#_stat0
 *
 * OCX FRC 0-5 Statistics Registers 0
 */
union bdk_ocx_frcx_stat0
{
    uint64_t u;
    struct bdk_ocx_frcx_stat0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_21_63        : 43;
        uint64_t align_cnt             : 21; /**< [ 20:  0](R/W/H) Indicates the number of alignment sequences received (i.e. those that do not violate the
                                                                 current alignment). */
#else /* Word 0 - Little Endian */
        uint64_t align_cnt             : 21; /**< [ 20:  0](R/W/H) Indicates the number of alignment sequences received (i.e. those that do not violate the
                                                                 current alignment). */
        uint64_t reserved_21_63        : 43;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_frcx_stat0_s cn; */
};
typedef union bdk_ocx_frcx_stat0 bdk_ocx_frcx_stat0_t;

static inline uint64_t BDK_OCX_FRCX_STAT0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_FRCX_STAT0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e01100fa00ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("OCX_FRCX_STAT0", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_FRCX_STAT0(a) bdk_ocx_frcx_stat0_t
#define bustype_BDK_OCX_FRCX_STAT0(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_FRCX_STAT0(a) "OCX_FRCX_STAT0"
#define device_bar_BDK_OCX_FRCX_STAT0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_FRCX_STAT0(a) (a)
#define arguments_BDK_OCX_FRCX_STAT0(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_frc#_stat1
 *
 * OCX FRC 0-5 Statistics Registers 1
 */
union bdk_ocx_frcx_stat1
{
    uint64_t u;
    struct bdk_ocx_frcx_stat1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_21_63        : 43;
        uint64_t align_err_cnt         : 21; /**< [ 20:  0](R/W/H) Indicates the number of alignment sequences received in error (i.e. those that violate the
                                                                 current alignment). */
#else /* Word 0 - Little Endian */
        uint64_t align_err_cnt         : 21; /**< [ 20:  0](R/W/H) Indicates the number of alignment sequences received in error (i.e. those that violate the
                                                                 current alignment). */
        uint64_t reserved_21_63        : 43;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_frcx_stat1_s cn; */
};
typedef union bdk_ocx_frcx_stat1 bdk_ocx_frcx_stat1_t;

static inline uint64_t BDK_OCX_FRCX_STAT1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_FRCX_STAT1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e01100fa80ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("OCX_FRCX_STAT1", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_FRCX_STAT1(a) bdk_ocx_frcx_stat1_t
#define bustype_BDK_OCX_FRCX_STAT1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_FRCX_STAT1(a) "OCX_FRCX_STAT1"
#define device_bar_BDK_OCX_FRCX_STAT1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_FRCX_STAT1(a) (a)
#define arguments_BDK_OCX_FRCX_STAT1(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_frc#_stat2
 *
 * OCX FRC 0-5 Statistics Registers 2
 */
union bdk_ocx_frcx_stat2
{
    uint64_t u;
    struct bdk_ocx_frcx_stat2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_21_63        : 43;
        uint64_t align_done            : 21; /**< [ 20:  0](R/W/H) Indicates the number of attempts at alignment that succeeded. */
#else /* Word 0 - Little Endian */
        uint64_t align_done            : 21; /**< [ 20:  0](R/W/H) Indicates the number of attempts at alignment that succeeded. */
        uint64_t reserved_21_63        : 43;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_frcx_stat2_s cn; */
};
typedef union bdk_ocx_frcx_stat2 bdk_ocx_frcx_stat2_t;

static inline uint64_t BDK_OCX_FRCX_STAT2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_FRCX_STAT2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e01100fb00ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("OCX_FRCX_STAT2", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_FRCX_STAT2(a) bdk_ocx_frcx_stat2_t
#define bustype_BDK_OCX_FRCX_STAT2(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_FRCX_STAT2(a) "OCX_FRCX_STAT2"
#define device_bar_BDK_OCX_FRCX_STAT2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_FRCX_STAT2(a) (a)
#define arguments_BDK_OCX_FRCX_STAT2(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_frc#_stat3
 *
 * OCX FRC 0-5 Statistics Registers 3
 */
union bdk_ocx_frcx_stat3
{
    uint64_t u;
    struct bdk_ocx_frcx_stat3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_21_63        : 43;
        uint64_t align_fail            : 21; /**< [ 20:  0](R/W/H) Indicates the number of attempts at alignment that failed. */
#else /* Word 0 - Little Endian */
        uint64_t align_fail            : 21; /**< [ 20:  0](R/W/H) Indicates the number of attempts at alignment that failed. */
        uint64_t reserved_21_63        : 43;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_frcx_stat3_s cn; */
};
typedef union bdk_ocx_frcx_stat3 bdk_ocx_frcx_stat3_t;

static inline uint64_t BDK_OCX_FRCX_STAT3(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_FRCX_STAT3(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e01100fb80ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("OCX_FRCX_STAT3", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_FRCX_STAT3(a) bdk_ocx_frcx_stat3_t
#define bustype_BDK_OCX_FRCX_STAT3(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_FRCX_STAT3(a) "OCX_FRCX_STAT3"
#define device_bar_BDK_OCX_FRCX_STAT3(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_FRCX_STAT3(a) (a)
#define arguments_BDK_OCX_FRCX_STAT3(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_bad_cnt
 *
 * OCX Lane Bad Count Register
 */
union bdk_ocx_lnex_bad_cnt
{
    uint64_t u;
    struct bdk_ocx_lnex_bad_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t tx_bad_crc32          : 1;  /**< [ 11: 11](R/W/H) Send one diagnostic word with bad CRC32 to the selected lane.
                                                                 Injects just once. */
        uint64_t tx_bad_6467_cnt       : 5;  /**< [ 10:  6](R/W/H) Send N bad 64B/67B code words on selected lane. */
        uint64_t tx_bad_sync_cnt       : 3;  /**< [  5:  3](R/W/H) Send N bad sync words on selected lane. */
        uint64_t tx_bad_scram_cnt      : 3;  /**< [  2:  0](R/W/H) Send N bad scram state on selected lane. */
#else /* Word 0 - Little Endian */
        uint64_t tx_bad_scram_cnt      : 3;  /**< [  2:  0](R/W/H) Send N bad scram state on selected lane. */
        uint64_t tx_bad_sync_cnt       : 3;  /**< [  5:  3](R/W/H) Send N bad sync words on selected lane. */
        uint64_t tx_bad_6467_cnt       : 5;  /**< [ 10:  6](R/W/H) Send N bad 64B/67B code words on selected lane. */
        uint64_t tx_bad_crc32          : 1;  /**< [ 11: 11](R/W/H) Send one diagnostic word with bad CRC32 to the selected lane.
                                                                 Injects just once. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_bad_cnt_s cn; */
};
typedef union bdk_ocx_lnex_bad_cnt bdk_ocx_lnex_bad_cnt_t;

static inline uint64_t BDK_OCX_LNEX_BAD_CNT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_BAD_CNT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008028ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_BAD_CNT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_BAD_CNT(a) bdk_ocx_lnex_bad_cnt_t
#define bustype_BDK_OCX_LNEX_BAD_CNT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_BAD_CNT(a) "OCX_LNEX_BAD_CNT"
#define device_bar_BDK_OCX_LNEX_BAD_CNT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_BAD_CNT(a) (a)
#define arguments_BDK_OCX_LNEX_BAD_CNT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_cfg
 *
 * OCX Lane Config Register
 */
union bdk_ocx_lnex_cfg
{
    uint64_t u;
    struct bdk_ocx_lnex_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t rx_bdry_lock_dis      : 1;  /**< [  8:  8](R/W) Disable word boundary lock. While disabled, received data is tossed. Once enabled,
                                                                 received data is searched for legal two-bit patterns. */
        uint64_t reserved_3_7          : 5;
        uint64_t rx_stat_wrap_dis      : 1;  /**< [  2:  2](R/W) Upon overflow, a statistics counter should saturate instead of wrapping. */
        uint64_t rx_stat_rdclr         : 1;  /**< [  1:  1](R/W) CSR read to OCX_LNEx_STAT* clears the selected counter after returning its current value. */
        uint64_t rx_stat_ena           : 1;  /**< [  0:  0](R/W) Enable RX lane statistics counters. */
#else /* Word 0 - Little Endian */
        uint64_t rx_stat_ena           : 1;  /**< [  0:  0](R/W) Enable RX lane statistics counters. */
        uint64_t rx_stat_rdclr         : 1;  /**< [  1:  1](R/W) CSR read to OCX_LNEx_STAT* clears the selected counter after returning its current value. */
        uint64_t rx_stat_wrap_dis      : 1;  /**< [  2:  2](R/W) Upon overflow, a statistics counter should saturate instead of wrapping. */
        uint64_t reserved_3_7          : 5;
        uint64_t rx_bdry_lock_dis      : 1;  /**< [  8:  8](R/W) Disable word boundary lock. While disabled, received data is tossed. Once enabled,
                                                                 received data is searched for legal two-bit patterns. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_cfg_s cn; */
};
typedef union bdk_ocx_lnex_cfg bdk_ocx_lnex_cfg_t;

static inline uint64_t BDK_OCX_LNEX_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008000ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_CFG(a) bdk_ocx_lnex_cfg_t
#define bustype_BDK_OCX_LNEX_CFG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_CFG(a) "OCX_LNEX_CFG"
#define device_bar_BDK_OCX_LNEX_CFG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_CFG(a) (a)
#define arguments_BDK_OCX_LNEX_CFG(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_int
 *
 * OCX Lane Interrupt Register
 */
union bdk_ocx_lnex_int
{
    uint64_t u;
    struct bdk_ocx_lnex_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t disp_err              : 1;  /**< [  9:  9](R/W1C/H) RX disparity error encountered. These receive errors may occur during normal
                                                                 operation, and may likely occur during link bring up. Hardware normally will
                                                                 automatically correct the error. Software may choose to count the number of
                                                                 these errors. */
        uint64_t bad_64b67b            : 1;  /**< [  8:  8](R/W1C/H) Bad 64B/67B codeword encountered. Once the bad word reaches the link, as denoted by
                                                                 OCX_COM_LINK(0..2)_INT[BAD_WORD], a retry handshake is initiated. These receive errors may
                                                                 occur during normal operation, and may likely occur during link bringup. Hardware normally
                                                                 automatically corrects the error. Software may choose to count the number of these
                                                                 errors. */
        uint64_t stat_cnt_ovfl         : 1;  /**< [  7:  7](R/W1C/H) RX lane statistic counter overflow. */
        uint64_t stat_msg              : 1;  /**< [  6:  6](R/W1C/H) Status bits for the link or a lane transitioned from a 1 (healthy) to a 0 (problem). These
                                                                 should not occur during normal operation. This may be considered fatal, depending on the
                                                                 software protocol. */
        uint64_t dskew_fifo_ovfl       : 1;  /**< [  5:  5](R/W1C/H) RX deskew FIFO overflow occurred. These receive errors may occur during normal operation,
                                                                 and may likely occur during link bring up. Hardware normally automatically corrects the
                                                                 error. Software may choose to count the number of these errors. */
        uint64_t scrm_sync_loss        : 1;  /**< [  4:  4](R/W1C/H) Four consecutive bad sync words or three consecutive scramble state
                                                                 mismatches. These receive errors should not occur during normal operation, but
                                                                 may likely occur during link bring up.
                                                                 Hardware normally will automatically correct the error. Software may choose to count the
                                                                 number of these errors. */
        uint64_t ukwn_cntl_word        : 1;  /**< [  3:  3](R/W1C/H) Unknown framing-control word. The block type does not match any of (SYNC, SCRAM, SKIP,
                                                                 DIAG).
                                                                 These receive errors may occur during normal operation. Hardware normally
                                                                 automatically corrects the error. Software may choose to count the number of these errors. */
        uint64_t crc32_err             : 1;  /**< [  2:  2](R/W1C/H) Diagnostic CRC32 errors. These receive errors may occur during normal operation, typically
                                                                 in the presence of other errors, and may likely occur during link bring up. Hardware
                                                                 normally automatically corrects the error. Software may choose to count the number of
                                                                 these errors. */
        uint64_t bdry_sync_loss        : 1;  /**< [  1:  1](R/W1C/H) RX logic lost word boundary sync after 16 tries. Hardware automatically attempts to regain
                                                                 word boundary sync. These receive errors should not occur during normal operation, but may
                                                                 likely occur during link bring up. Hardware normally automatically corrects the error.
                                                                 Software may choose to count the number of these errors. */
        uint64_t serdes_lock_loss      : 1;  /**< [  0:  0](R/W1C/H) RX SerDes loses lock. These receive errors should not occur during normal operation. This
                                                                 may be considered fatal. */
#else /* Word 0 - Little Endian */
        uint64_t serdes_lock_loss      : 1;  /**< [  0:  0](R/W1C/H) RX SerDes loses lock. These receive errors should not occur during normal operation. This
                                                                 may be considered fatal. */
        uint64_t bdry_sync_loss        : 1;  /**< [  1:  1](R/W1C/H) RX logic lost word boundary sync after 16 tries. Hardware automatically attempts to regain
                                                                 word boundary sync. These receive errors should not occur during normal operation, but may
                                                                 likely occur during link bring up. Hardware normally automatically corrects the error.
                                                                 Software may choose to count the number of these errors. */
        uint64_t crc32_err             : 1;  /**< [  2:  2](R/W1C/H) Diagnostic CRC32 errors. These receive errors may occur during normal operation, typically
                                                                 in the presence of other errors, and may likely occur during link bring up. Hardware
                                                                 normally automatically corrects the error. Software may choose to count the number of
                                                                 these errors. */
        uint64_t ukwn_cntl_word        : 1;  /**< [  3:  3](R/W1C/H) Unknown framing-control word. The block type does not match any of (SYNC, SCRAM, SKIP,
                                                                 DIAG).
                                                                 These receive errors may occur during normal operation. Hardware normally
                                                                 automatically corrects the error. Software may choose to count the number of these errors. */
        uint64_t scrm_sync_loss        : 1;  /**< [  4:  4](R/W1C/H) Four consecutive bad sync words or three consecutive scramble state
                                                                 mismatches. These receive errors should not occur during normal operation, but
                                                                 may likely occur during link bring up.
                                                                 Hardware normally will automatically correct the error. Software may choose to count the
                                                                 number of these errors. */
        uint64_t dskew_fifo_ovfl       : 1;  /**< [  5:  5](R/W1C/H) RX deskew FIFO overflow occurred. These receive errors may occur during normal operation,
                                                                 and may likely occur during link bring up. Hardware normally automatically corrects the
                                                                 error. Software may choose to count the number of these errors. */
        uint64_t stat_msg              : 1;  /**< [  6:  6](R/W1C/H) Status bits for the link or a lane transitioned from a 1 (healthy) to a 0 (problem). These
                                                                 should not occur during normal operation. This may be considered fatal, depending on the
                                                                 software protocol. */
        uint64_t stat_cnt_ovfl         : 1;  /**< [  7:  7](R/W1C/H) RX lane statistic counter overflow. */
        uint64_t bad_64b67b            : 1;  /**< [  8:  8](R/W1C/H) Bad 64B/67B codeword encountered. Once the bad word reaches the link, as denoted by
                                                                 OCX_COM_LINK(0..2)_INT[BAD_WORD], a retry handshake is initiated. These receive errors may
                                                                 occur during normal operation, and may likely occur during link bringup. Hardware normally
                                                                 automatically corrects the error. Software may choose to count the number of these
                                                                 errors. */
        uint64_t disp_err              : 1;  /**< [  9:  9](R/W1C/H) RX disparity error encountered. These receive errors may occur during normal
                                                                 operation, and may likely occur during link bring up. Hardware normally will
                                                                 automatically correct the error. Software may choose to count the number of
                                                                 these errors. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_int_s cn; */
};
typedef union bdk_ocx_lnex_int bdk_ocx_lnex_int_t;

static inline uint64_t BDK_OCX_LNEX_INT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_INT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008018ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_INT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_INT(a) bdk_ocx_lnex_int_t
#define bustype_BDK_OCX_LNEX_INT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_INT(a) "OCX_LNEX_INT"
#define device_bar_BDK_OCX_LNEX_INT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_INT(a) (a)
#define arguments_BDK_OCX_LNEX_INT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_int_en
 *
 * OCX Lane Interrupt Enable Register
 */
union bdk_ocx_lnex_int_en
{
    uint64_t u;
    struct bdk_ocx_lnex_int_en_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t disp_err              : 1;  /**< [  9:  9](RAZ) Reserved. */
        uint64_t bad_64b67b            : 1;  /**< [  8:  8](R/W) Enable bit for bad 64B/67B codeword encountered. */
        uint64_t stat_cnt_ovfl         : 1;  /**< [  7:  7](R/W) Enable bit for RX lane statistic counter overflow. */
        uint64_t stat_msg              : 1;  /**< [  6:  6](R/W) Enable bit for status bits for the link or a lane transitioned from a 1 (healthy) to a 0 (problem). */
        uint64_t dskew_fifo_ovfl       : 1;  /**< [  5:  5](R/W) Enable bit for RX deskew FIFO overflow occurred. */
        uint64_t scrm_sync_loss        : 1;  /**< [  4:  4](R/W) Enable bit for 4 consecutive bad sync words or 3 consecutive scramble state mismatches. */
        uint64_t ukwn_cntl_word        : 1;  /**< [  3:  3](R/W) Enable bit for unknown framing control word. Block type does not match any of (SYNC,
                                                                 SCRAM, SKIP, DIAG). */
        uint64_t crc32_err             : 1;  /**< [  2:  2](R/W) Enable bit for diagnostic CRC32 errors. */
        uint64_t bdry_sync_loss        : 1;  /**< [  1:  1](R/W) Enable bit for RX logic lost word boundary sync after 16 tries. */
        uint64_t serdes_lock_loss      : 1;  /**< [  0:  0](R/W) Enable bit for RX SerDes loses lock. */
#else /* Word 0 - Little Endian */
        uint64_t serdes_lock_loss      : 1;  /**< [  0:  0](R/W) Enable bit for RX SerDes loses lock. */
        uint64_t bdry_sync_loss        : 1;  /**< [  1:  1](R/W) Enable bit for RX logic lost word boundary sync after 16 tries. */
        uint64_t crc32_err             : 1;  /**< [  2:  2](R/W) Enable bit for diagnostic CRC32 errors. */
        uint64_t ukwn_cntl_word        : 1;  /**< [  3:  3](R/W) Enable bit for unknown framing control word. Block type does not match any of (SYNC,
                                                                 SCRAM, SKIP, DIAG). */
        uint64_t scrm_sync_loss        : 1;  /**< [  4:  4](R/W) Enable bit for 4 consecutive bad sync words or 3 consecutive scramble state mismatches. */
        uint64_t dskew_fifo_ovfl       : 1;  /**< [  5:  5](R/W) Enable bit for RX deskew FIFO overflow occurred. */
        uint64_t stat_msg              : 1;  /**< [  6:  6](R/W) Enable bit for status bits for the link or a lane transitioned from a 1 (healthy) to a 0 (problem). */
        uint64_t stat_cnt_ovfl         : 1;  /**< [  7:  7](R/W) Enable bit for RX lane statistic counter overflow. */
        uint64_t bad_64b67b            : 1;  /**< [  8:  8](R/W) Enable bit for bad 64B/67B codeword encountered. */
        uint64_t disp_err              : 1;  /**< [  9:  9](RAZ) Reserved. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_int_en_s cn88xxp1; */
    struct bdk_ocx_lnex_int_en_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t disp_err              : 1;  /**< [  9:  9](R/W) Enable bit for RX disparity error encountered. */
        uint64_t bad_64b67b            : 1;  /**< [  8:  8](R/W) Enable bit for bad 64B/67B codeword encountered. */
        uint64_t stat_cnt_ovfl         : 1;  /**< [  7:  7](R/W) Enable bit for RX lane statistic counter overflow. */
        uint64_t stat_msg              : 1;  /**< [  6:  6](R/W) Enable bit for status bits for the link or a lane transitioned from a 1 (healthy) to a 0 (problem). */
        uint64_t dskew_fifo_ovfl       : 1;  /**< [  5:  5](R/W) Enable bit for RX deskew FIFO overflow occurred. */
        uint64_t scrm_sync_loss        : 1;  /**< [  4:  4](R/W) Enable bit for 4 consecutive bad sync words or 3 consecutive scramble state mismatches. */
        uint64_t ukwn_cntl_word        : 1;  /**< [  3:  3](R/W) Enable bit for unknown framing control word. Block type does not match any of (SYNC,
                                                                 SCRAM, SKIP, DIAG). */
        uint64_t crc32_err             : 1;  /**< [  2:  2](R/W) Enable bit for diagnostic CRC32 errors. */
        uint64_t bdry_sync_loss        : 1;  /**< [  1:  1](R/W) Enable bit for RX logic lost word boundary sync after 16 tries. */
        uint64_t serdes_lock_loss      : 1;  /**< [  0:  0](R/W) Enable bit for RX SerDes loses lock. */
#else /* Word 0 - Little Endian */
        uint64_t serdes_lock_loss      : 1;  /**< [  0:  0](R/W) Enable bit for RX SerDes loses lock. */
        uint64_t bdry_sync_loss        : 1;  /**< [  1:  1](R/W) Enable bit for RX logic lost word boundary sync after 16 tries. */
        uint64_t crc32_err             : 1;  /**< [  2:  2](R/W) Enable bit for diagnostic CRC32 errors. */
        uint64_t ukwn_cntl_word        : 1;  /**< [  3:  3](R/W) Enable bit for unknown framing control word. Block type does not match any of (SYNC,
                                                                 SCRAM, SKIP, DIAG). */
        uint64_t scrm_sync_loss        : 1;  /**< [  4:  4](R/W) Enable bit for 4 consecutive bad sync words or 3 consecutive scramble state mismatches. */
        uint64_t dskew_fifo_ovfl       : 1;  /**< [  5:  5](R/W) Enable bit for RX deskew FIFO overflow occurred. */
        uint64_t stat_msg              : 1;  /**< [  6:  6](R/W) Enable bit for status bits for the link or a lane transitioned from a 1 (healthy) to a 0 (problem). */
        uint64_t stat_cnt_ovfl         : 1;  /**< [  7:  7](R/W) Enable bit for RX lane statistic counter overflow. */
        uint64_t bad_64b67b            : 1;  /**< [  8:  8](R/W) Enable bit for bad 64B/67B codeword encountered. */
        uint64_t disp_err              : 1;  /**< [  9:  9](R/W) Enable bit for RX disparity error encountered. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_ocx_lnex_int_en bdk_ocx_lnex_int_en_t;

static inline uint64_t BDK_OCX_LNEX_INT_EN(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_INT_EN(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008020ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_INT_EN", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_INT_EN(a) bdk_ocx_lnex_int_en_t
#define bustype_BDK_OCX_LNEX_INT_EN(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_INT_EN(a) "OCX_LNEX_INT_EN"
#define device_bar_BDK_OCX_LNEX_INT_EN(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_INT_EN(a) (a)
#define arguments_BDK_OCX_LNEX_INT_EN(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat00
 *
 * OCX Lane Statistic 0 Register
 */
union bdk_ocx_lnex_stat00
{
    uint64_t u;
    struct bdk_ocx_lnex_stat00_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t ser_lock_loss_cnt     : 18; /**< [ 17:  0](RO/H) Number of times the lane lost clock-data-recovery. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t ser_lock_loss_cnt     : 18; /**< [ 17:  0](RO/H) Number of times the lane lost clock-data-recovery. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat00_s cn; */
};
typedef union bdk_ocx_lnex_stat00 bdk_ocx_lnex_stat00_t;

static inline uint64_t BDK_OCX_LNEX_STAT00(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT00(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008040ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT00", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT00(a) bdk_ocx_lnex_stat00_t
#define bustype_BDK_OCX_LNEX_STAT00(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT00(a) "OCX_LNEX_STAT00"
#define device_bar_BDK_OCX_LNEX_STAT00(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT00(a) (a)
#define arguments_BDK_OCX_LNEX_STAT00(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat01
 *
 * OCX Lane Statistic 1 Register
 */
union bdk_ocx_lnex_stat01
{
    uint64_t u;
    struct bdk_ocx_lnex_stat01_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t bdry_sync_loss_cnt    : 18; /**< [ 17:  0](RO/H) Number of times a lane lost word boundary synchronization. Saturates. Interrupt on
                                                                 saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t bdry_sync_loss_cnt    : 18; /**< [ 17:  0](RO/H) Number of times a lane lost word boundary synchronization. Saturates. Interrupt on
                                                                 saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat01_s cn; */
};
typedef union bdk_ocx_lnex_stat01 bdk_ocx_lnex_stat01_t;

static inline uint64_t BDK_OCX_LNEX_STAT01(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT01(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008048ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT01", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT01(a) bdk_ocx_lnex_stat01_t
#define bustype_BDK_OCX_LNEX_STAT01(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT01(a) "OCX_LNEX_STAT01"
#define device_bar_BDK_OCX_LNEX_STAT01(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT01(a) (a)
#define arguments_BDK_OCX_LNEX_STAT01(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat02
 *
 * OCX Lane Statistic 2 Register
 */
union bdk_ocx_lnex_stat02
{
    uint64_t u;
    struct bdk_ocx_lnex_stat02_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t syncw_bad_cnt         : 18; /**< [ 17:  0](RO/H) Number of bad synchronization words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t syncw_bad_cnt         : 18; /**< [ 17:  0](RO/H) Number of bad synchronization words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat02_s cn; */
};
typedef union bdk_ocx_lnex_stat02 bdk_ocx_lnex_stat02_t;

static inline uint64_t BDK_OCX_LNEX_STAT02(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT02(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008050ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT02", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT02(a) bdk_ocx_lnex_stat02_t
#define bustype_BDK_OCX_LNEX_STAT02(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT02(a) "OCX_LNEX_STAT02"
#define device_bar_BDK_OCX_LNEX_STAT02(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT02(a) (a)
#define arguments_BDK_OCX_LNEX_STAT02(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat03
 *
 * OCX Lane Statistic 3 Register
 */
union bdk_ocx_lnex_stat03
{
    uint64_t u;
    struct bdk_ocx_lnex_stat03_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t syncw_good_cnt        : 18; /**< [ 17:  0](RO/H) Number of good synchronization words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t syncw_good_cnt        : 18; /**< [ 17:  0](RO/H) Number of good synchronization words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat03_s cn; */
};
typedef union bdk_ocx_lnex_stat03 bdk_ocx_lnex_stat03_t;

static inline uint64_t BDK_OCX_LNEX_STAT03(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT03(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008058ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT03", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT03(a) bdk_ocx_lnex_stat03_t
#define bustype_BDK_OCX_LNEX_STAT03(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT03(a) "OCX_LNEX_STAT03"
#define device_bar_BDK_OCX_LNEX_STAT03(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT03(a) (a)
#define arguments_BDK_OCX_LNEX_STAT03(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat04
 *
 * OCX Lane Statistic 4 Register
 */
union bdk_ocx_lnex_stat04
{
    uint64_t u;
    struct bdk_ocx_lnex_stat04_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t bad_64b67b_cnt        : 18; /**< [ 17:  0](RO/H) Number of bad 64B/67B words, meaning bit 65 or 64 has been corrupted. Saturates. Interrupt
                                                                 on saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t bad_64b67b_cnt        : 18; /**< [ 17:  0](RO/H) Number of bad 64B/67B words, meaning bit 65 or 64 has been corrupted. Saturates. Interrupt
                                                                 on saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat04_s cn; */
};
typedef union bdk_ocx_lnex_stat04 bdk_ocx_lnex_stat04_t;

static inline uint64_t BDK_OCX_LNEX_STAT04(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT04(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008060ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT04", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT04(a) bdk_ocx_lnex_stat04_t
#define bustype_BDK_OCX_LNEX_STAT04(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT04(a) "OCX_LNEX_STAT04"
#define device_bar_BDK_OCX_LNEX_STAT04(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT04(a) (a)
#define arguments_BDK_OCX_LNEX_STAT04(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat05
 *
 * OCX Lane Statistic 5 Register
 */
union bdk_ocx_lnex_stat05
{
    uint64_t u;
    struct bdk_ocx_lnex_stat05_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_27_63        : 37;
        uint64_t data_word_cnt         : 27; /**< [ 26:  0](RO/H) Number of data words received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data_word_cnt         : 27; /**< [ 26:  0](RO/H) Number of data words received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_27_63        : 37;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat05_s cn; */
};
typedef union bdk_ocx_lnex_stat05 bdk_ocx_lnex_stat05_t;

static inline uint64_t BDK_OCX_LNEX_STAT05(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT05(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008068ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT05", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT05(a) bdk_ocx_lnex_stat05_t
#define bustype_BDK_OCX_LNEX_STAT05(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT05(a) "OCX_LNEX_STAT05"
#define device_bar_BDK_OCX_LNEX_STAT05(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT05(a) (a)
#define arguments_BDK_OCX_LNEX_STAT05(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat06
 *
 * OCX Lane Statistic 6 Register
 */
union bdk_ocx_lnex_stat06
{
    uint64_t u;
    struct bdk_ocx_lnex_stat06_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_27_63        : 37;
        uint64_t cntl_word_cnt         : 27; /**< [ 26:  0](RO/H) Number of control words received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t cntl_word_cnt         : 27; /**< [ 26:  0](RO/H) Number of control words received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_27_63        : 37;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat06_s cn; */
};
typedef union bdk_ocx_lnex_stat06 bdk_ocx_lnex_stat06_t;

static inline uint64_t BDK_OCX_LNEX_STAT06(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT06(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008070ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT06", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT06(a) bdk_ocx_lnex_stat06_t
#define bustype_BDK_OCX_LNEX_STAT06(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT06(a) "OCX_LNEX_STAT06"
#define device_bar_BDK_OCX_LNEX_STAT06(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT06(a) (a)
#define arguments_BDK_OCX_LNEX_STAT06(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat07
 *
 * OCX Lane Statistic 7 Register
 */
union bdk_ocx_lnex_stat07
{
    uint64_t u;
    struct bdk_ocx_lnex_stat07_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t unkwn_word_cnt        : 18; /**< [ 17:  0](RO/H) Number of unknown control words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t unkwn_word_cnt        : 18; /**< [ 17:  0](RO/H) Number of unknown control words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat07_s cn; */
};
typedef union bdk_ocx_lnex_stat07 bdk_ocx_lnex_stat07_t;

static inline uint64_t BDK_OCX_LNEX_STAT07(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT07(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008078ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT07", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT07(a) bdk_ocx_lnex_stat07_t
#define bustype_BDK_OCX_LNEX_STAT07(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT07(a) "OCX_LNEX_STAT07"
#define device_bar_BDK_OCX_LNEX_STAT07(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT07(a) (a)
#define arguments_BDK_OCX_LNEX_STAT07(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat08
 *
 * OCX Lane Statistic 8 Register
 */
union bdk_ocx_lnex_stat08
{
    uint64_t u;
    struct bdk_ocx_lnex_stat08_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t scrm_sync_loss_cnt    : 18; /**< [ 17:  0](RO/H) Number of times scrambler synchronization was lost (due to either four
                                                                 consecutive bad sync words or three consecutive scrambler state
                                                                 mismatches). Saturates. Interrupt on saturation if
                                                                 OCX_LNE()_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t scrm_sync_loss_cnt    : 18; /**< [ 17:  0](RO/H) Number of times scrambler synchronization was lost (due to either four
                                                                 consecutive bad sync words or three consecutive scrambler state
                                                                 mismatches). Saturates. Interrupt on saturation if
                                                                 OCX_LNE()_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat08_s cn; */
};
typedef union bdk_ocx_lnex_stat08 bdk_ocx_lnex_stat08_t;

static inline uint64_t BDK_OCX_LNEX_STAT08(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT08(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008080ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT08", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT08(a) bdk_ocx_lnex_stat08_t
#define bustype_BDK_OCX_LNEX_STAT08(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT08(a) "OCX_LNEX_STAT08"
#define device_bar_BDK_OCX_LNEX_STAT08(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT08(a) (a)
#define arguments_BDK_OCX_LNEX_STAT08(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat09
 *
 * OCX Lane Statistic 9 Register
 */
union bdk_ocx_lnex_stat09
{
    uint64_t u;
    struct bdk_ocx_lnex_stat09_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t scrm_match_cnt        : 18; /**< [ 17:  0](RO/H) Number of scrambler state matches received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t scrm_match_cnt        : 18; /**< [ 17:  0](RO/H) Number of scrambler state matches received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat09_s cn; */
};
typedef union bdk_ocx_lnex_stat09 bdk_ocx_lnex_stat09_t;

static inline uint64_t BDK_OCX_LNEX_STAT09(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT09(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008088ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT09", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT09(a) bdk_ocx_lnex_stat09_t
#define bustype_BDK_OCX_LNEX_STAT09(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT09(a) "OCX_LNEX_STAT09"
#define device_bar_BDK_OCX_LNEX_STAT09(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT09(a) (a)
#define arguments_BDK_OCX_LNEX_STAT09(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat10
 *
 * OCX Lane Statistic 10 Register
 */
union bdk_ocx_lnex_stat10
{
    uint64_t u;
    struct bdk_ocx_lnex_stat10_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t skipw_good_cnt        : 18; /**< [ 17:  0](RO/H) Number of good skip words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t skipw_good_cnt        : 18; /**< [ 17:  0](RO/H) Number of good skip words. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat10_s cn; */
};
typedef union bdk_ocx_lnex_stat10 bdk_ocx_lnex_stat10_t;

static inline uint64_t BDK_OCX_LNEX_STAT10(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT10(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008090ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT10", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT10(a) bdk_ocx_lnex_stat10_t
#define bustype_BDK_OCX_LNEX_STAT10(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT10(a) "OCX_LNEX_STAT10"
#define device_bar_BDK_OCX_LNEX_STAT10(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT10(a) (a)
#define arguments_BDK_OCX_LNEX_STAT10(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat11
 *
 * OCX Lane Statistic 11 Register
 */
union bdk_ocx_lnex_stat11
{
    uint64_t u;
    struct bdk_ocx_lnex_stat11_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_27_63        : 37;
        uint64_t crc32_err_cnt         : 27; /**< [ 26:  0](RO/H) Number of errors in the lane CRC. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t crc32_err_cnt         : 27; /**< [ 26:  0](RO/H) Number of errors in the lane CRC. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_27_63        : 37;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat11_s cn; */
};
typedef union bdk_ocx_lnex_stat11 bdk_ocx_lnex_stat11_t;

static inline uint64_t BDK_OCX_LNEX_STAT11(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT11(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008098ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT11", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT11(a) bdk_ocx_lnex_stat11_t
#define bustype_BDK_OCX_LNEX_STAT11(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT11(a) "OCX_LNEX_STAT11"
#define device_bar_BDK_OCX_LNEX_STAT11(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT11(a) (a)
#define arguments_BDK_OCX_LNEX_STAT11(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat12
 *
 * OCX Lane Statistic 12 Register
 */
union bdk_ocx_lnex_stat12
{
    uint64_t u;
    struct bdk_ocx_lnex_stat12_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_27_63        : 37;
        uint64_t crc32_match_cnt       : 27; /**< [ 26:  0](RO/H) Number of CRC32 matches received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t crc32_match_cnt       : 27; /**< [ 26:  0](RO/H) Number of CRC32 matches received. Saturates. Interrupt on saturation if
                                                                 OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_27_63        : 37;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat12_s cn; */
};
typedef union bdk_ocx_lnex_stat12 bdk_ocx_lnex_stat12_t;

static inline uint64_t BDK_OCX_LNEX_STAT12(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT12(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e0110080a0ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT12", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT12(a) bdk_ocx_lnex_stat12_t
#define bustype_BDK_OCX_LNEX_STAT12(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT12(a) "OCX_LNEX_STAT12"
#define device_bar_BDK_OCX_LNEX_STAT12(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT12(a) (a)
#define arguments_BDK_OCX_LNEX_STAT12(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat13
 *
 * OCX Lane Statistic 13 Register
 */
union bdk_ocx_lnex_stat13
{
    uint64_t u;
    struct bdk_ocx_lnex_stat13_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t trn_bad_cnt           : 16; /**< [ 15:  0](RO/H) Number of training frames received with an invalid control channel. Saturates. Interrupt
                                                                 on saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t trn_bad_cnt           : 16; /**< [ 15:  0](RO/H) Number of training frames received with an invalid control channel. Saturates. Interrupt
                                                                 on saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat13_s cn; */
};
typedef union bdk_ocx_lnex_stat13 bdk_ocx_lnex_stat13_t;

static inline uint64_t BDK_OCX_LNEX_STAT13(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT13(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e0110080a8ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT13", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT13(a) bdk_ocx_lnex_stat13_t
#define bustype_BDK_OCX_LNEX_STAT13(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT13(a) "OCX_LNEX_STAT13"
#define device_bar_BDK_OCX_LNEX_STAT13(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT13(a) (a)
#define arguments_BDK_OCX_LNEX_STAT13(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_stat14
 *
 * OCX Lane Statistic 14 Register
 */
union bdk_ocx_lnex_stat14
{
    uint64_t u;
    struct bdk_ocx_lnex_stat14_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t trn_prbs_bad_cnt      : 16; /**< [ 15:  0](RO/H) Number of training frames received with a bad PRBS pattern. Saturates. Interrupt on
                                                                 saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t trn_prbs_bad_cnt      : 16; /**< [ 15:  0](RO/H) Number of training frames received with a bad PRBS pattern. Saturates. Interrupt on
                                                                 saturation if OCX_LNE(0..23)_INT_EN[STAT_CNT_OVFL] = 1. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_stat14_s cn; */
};
typedef union bdk_ocx_lnex_stat14 bdk_ocx_lnex_stat14_t;

static inline uint64_t BDK_OCX_LNEX_STAT14(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STAT14(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e0110080b0ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STAT14", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STAT14(a) bdk_ocx_lnex_stat14_t
#define bustype_BDK_OCX_LNEX_STAT14(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STAT14(a) "OCX_LNEX_STAT14"
#define device_bar_BDK_OCX_LNEX_STAT14(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STAT14(a) (a)
#define arguments_BDK_OCX_LNEX_STAT14(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_status
 *
 * OCX Lane Status Register
 */
union bdk_ocx_lnex_status
{
    uint64_t u;
    struct bdk_ocx_lnex_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t rx_trn_val            : 1;  /**< [  2:  2](R/W/H) The control channel of a link training was received without any errors. */
        uint64_t rx_scrm_sync          : 1;  /**< [  1:  1](RO/H) RX scrambler synchronization status. Set to 1 when synchronization achieved. */
        uint64_t rx_bdry_sync          : 1;  /**< [  0:  0](RO/H) RX word boundary sync status. Set to 1 when synchronization achieved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_bdry_sync          : 1;  /**< [  0:  0](RO/H) RX word boundary sync status. Set to 1 when synchronization achieved. */
        uint64_t rx_scrm_sync          : 1;  /**< [  1:  1](RO/H) RX scrambler synchronization status. Set to 1 when synchronization achieved. */
        uint64_t rx_trn_val            : 1;  /**< [  2:  2](R/W/H) The control channel of a link training was received without any errors. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_status_s cn; */
};
typedef union bdk_ocx_lnex_status bdk_ocx_lnex_status_t;

static inline uint64_t BDK_OCX_LNEX_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008008ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STATUS(a) bdk_ocx_lnex_status_t
#define bustype_BDK_OCX_LNEX_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STATUS(a) "OCX_LNEX_STATUS"
#define device_bar_BDK_OCX_LNEX_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STATUS(a) (a)
#define arguments_BDK_OCX_LNEX_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_sts_msg
 *
 * OCX Lane Status Message Register
 */
union bdk_ocx_lnex_sts_msg
{
    uint64_t u;
    struct bdk_ocx_lnex_sts_msg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rx_meta_val           : 1;  /**< [ 63: 63](RO/H) Meta-data received in the diagnostic word (per-lane) is valid. */
        uint64_t reserved_37_62        : 26;
        uint64_t rx_meta_dat           : 3;  /**< [ 36: 34](RO/H) Meta-data received in the diagnostic word (per-lane). */
        uint64_t rx_lne_stat           : 1;  /**< [ 33: 33](RO/H) Lane status received in the diagnostic word (per-lane). Set to 1 when healthy
                                                                 (according to the Interlaken spec). */
        uint64_t rx_lnk_stat           : 1;  /**< [ 32: 32](RO/H) Link status received in the diagnostic word (per-lane). Set to 1 when healthy
                                                                 (according to the Interlaken spec). */
        uint64_t reserved_5_31         : 27;
        uint64_t tx_meta_dat           : 3;  /**< [  4:  2](RO/H) Meta-data transmitted in the diagnostic word (per-lane). */
        uint64_t tx_lne_stat           : 1;  /**< [  1:  1](R/W/H) Lane status transmitted in the diagnostic word (per-lane). Set to 1 means
                                                                 healthy (according to the Interlaken spec). */
        uint64_t tx_lnk_stat           : 1;  /**< [  0:  0](R/W/H) Link status transmitted in the diagnostic word (per-lane). Set to 1 means
                                                                 healthy (according to the Interlaken spec). */
#else /* Word 0 - Little Endian */
        uint64_t tx_lnk_stat           : 1;  /**< [  0:  0](R/W/H) Link status transmitted in the diagnostic word (per-lane). Set to 1 means
                                                                 healthy (according to the Interlaken spec). */
        uint64_t tx_lne_stat           : 1;  /**< [  1:  1](R/W/H) Lane status transmitted in the diagnostic word (per-lane). Set to 1 means
                                                                 healthy (according to the Interlaken spec). */
        uint64_t tx_meta_dat           : 3;  /**< [  4:  2](RO/H) Meta-data transmitted in the diagnostic word (per-lane). */
        uint64_t reserved_5_31         : 27;
        uint64_t rx_lnk_stat           : 1;  /**< [ 32: 32](RO/H) Link status received in the diagnostic word (per-lane). Set to 1 when healthy
                                                                 (according to the Interlaken spec). */
        uint64_t rx_lne_stat           : 1;  /**< [ 33: 33](RO/H) Lane status received in the diagnostic word (per-lane). Set to 1 when healthy
                                                                 (according to the Interlaken spec). */
        uint64_t rx_meta_dat           : 3;  /**< [ 36: 34](RO/H) Meta-data received in the diagnostic word (per-lane). */
        uint64_t reserved_37_62        : 26;
        uint64_t rx_meta_val           : 1;  /**< [ 63: 63](RO/H) Meta-data received in the diagnostic word (per-lane) is valid. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_sts_msg_s cn; */
};
typedef union bdk_ocx_lnex_sts_msg bdk_ocx_lnex_sts_msg_t;

static inline uint64_t BDK_OCX_LNEX_STS_MSG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_STS_MSG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e011008010ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_STS_MSG", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_STS_MSG(a) bdk_ocx_lnex_sts_msg_t
#define bustype_BDK_OCX_LNEX_STS_MSG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_STS_MSG(a) "OCX_LNEX_STS_MSG"
#define device_bar_BDK_OCX_LNEX_STS_MSG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_STS_MSG(a) (a)
#define arguments_BDK_OCX_LNEX_STS_MSG(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_trn_ctl
 *
 * OCX Lane Training Link Partner Register
 */
union bdk_ocx_lnex_trn_ctl
{
    uint64_t u;
    struct bdk_ocx_lnex_trn_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lock                  : 1;  /**< [  3:  3](RO/H) Training frame boundary locked. */
        uint64_t done                  : 1;  /**< [  2:  2](R/W/H) Training done. For diagnostic use only may be written to 1 to force training done. */
        uint64_t ena                   : 1;  /**< [  1:  1](RO/H) OCX_LNEX_TRN_CTL[TRN_ENA]=1 indicates that the lane is currently training.  It is a status
                                                                 bit used for debug.  It will read as zero when training has completed or when the QLM
                                                                 isn't ready for training. */
        uint64_t eie_detect            : 1;  /**< [  0:  0](RO/H) Electrical idle exit (EIE) detected. */
#else /* Word 0 - Little Endian */
        uint64_t eie_detect            : 1;  /**< [  0:  0](RO/H) Electrical idle exit (EIE) detected. */
        uint64_t ena                   : 1;  /**< [  1:  1](RO/H) OCX_LNEX_TRN_CTL[TRN_ENA]=1 indicates that the lane is currently training.  It is a status
                                                                 bit used for debug.  It will read as zero when training has completed or when the QLM
                                                                 isn't ready for training. */
        uint64_t done                  : 1;  /**< [  2:  2](R/W/H) Training done. For diagnostic use only may be written to 1 to force training done. */
        uint64_t lock                  : 1;  /**< [  3:  3](RO/H) Training frame boundary locked. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_trn_ctl_s cn; */
};
typedef union bdk_ocx_lnex_trn_ctl bdk_ocx_lnex_trn_ctl_t;

static inline uint64_t BDK_OCX_LNEX_TRN_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_TRN_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e0110080d0ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_TRN_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_TRN_CTL(a) bdk_ocx_lnex_trn_ctl_t
#define bustype_BDK_OCX_LNEX_TRN_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_TRN_CTL(a) "OCX_LNEX_TRN_CTL"
#define device_bar_BDK_OCX_LNEX_TRN_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_TRN_CTL(a) (a)
#define arguments_BDK_OCX_LNEX_TRN_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_trn_ld
 *
 * OCX Lane Training Local Device Register
 */
union bdk_ocx_lnex_trn_ld
{
    uint64_t u;
    struct bdk_ocx_lnex_trn_ld_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t lp_manual             : 1;  /**< [ 63: 63](R/W) Allow software to manually manipulate local device CU/SR by ignoring hardware update. */
        uint64_t reserved_49_62        : 14;
        uint64_t ld_cu_val             : 1;  /**< [ 48: 48](RO/H) Local device coefficient update field valid. */
        uint64_t ld_cu_dat             : 16; /**< [ 47: 32](R/W/H) Local device coefficient update field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_CUP_S. */
        uint64_t reserved_17_31        : 15;
        uint64_t ld_sr_val             : 1;  /**< [ 16: 16](RO/H) Local device status report field valid. */
        uint64_t ld_sr_dat             : 16; /**< [ 15:  0](R/W/H) Local device status report field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_REP_S. */
#else /* Word 0 - Little Endian */
        uint64_t ld_sr_dat             : 16; /**< [ 15:  0](R/W/H) Local device status report field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_REP_S. */
        uint64_t ld_sr_val             : 1;  /**< [ 16: 16](RO/H) Local device status report field valid. */
        uint64_t reserved_17_31        : 15;
        uint64_t ld_cu_dat             : 16; /**< [ 47: 32](R/W/H) Local device coefficient update field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_CUP_S. */
        uint64_t ld_cu_val             : 1;  /**< [ 48: 48](RO/H) Local device coefficient update field valid. */
        uint64_t reserved_49_62        : 14;
        uint64_t lp_manual             : 1;  /**< [ 63: 63](R/W) Allow software to manually manipulate local device CU/SR by ignoring hardware update. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_trn_ld_s cn; */
};
typedef union bdk_ocx_lnex_trn_ld bdk_ocx_lnex_trn_ld_t;

static inline uint64_t BDK_OCX_LNEX_TRN_LD(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_TRN_LD(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e0110080c0ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_TRN_LD", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_TRN_LD(a) bdk_ocx_lnex_trn_ld_t
#define bustype_BDK_OCX_LNEX_TRN_LD(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_TRN_LD(a) "OCX_LNEX_TRN_LD"
#define device_bar_BDK_OCX_LNEX_TRN_LD(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_TRN_LD(a) (a)
#define arguments_BDK_OCX_LNEX_TRN_LD(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne#_trn_lp
 *
 * OCX Lane Training Link Partner Register
 */
union bdk_ocx_lnex_trn_lp
{
    uint64_t u;
    struct bdk_ocx_lnex_trn_lp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t lp_cu_val             : 1;  /**< [ 48: 48](RO/H) Link partner coefficient update field valid. */
        uint64_t lp_cu_dat             : 16; /**< [ 47: 32](RO/H) Link partner coefficient update field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_CUP_S. */
        uint64_t reserved_17_31        : 15;
        uint64_t lp_sr_val             : 1;  /**< [ 16: 16](RO/H) Link partner status report field valid. */
        uint64_t lp_sr_dat             : 16; /**< [ 15:  0](RO/H) Link partner status report field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_REP_S. */
#else /* Word 0 - Little Endian */
        uint64_t lp_sr_dat             : 16; /**< [ 15:  0](RO/H) Link partner status report field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_REP_S. */
        uint64_t lp_sr_val             : 1;  /**< [ 16: 16](RO/H) Link partner status report field valid. */
        uint64_t reserved_17_31        : 15;
        uint64_t lp_cu_dat             : 16; /**< [ 47: 32](RO/H) Link partner coefficient update field data.
                                                                 The format of this field is BGX_SPU_BR_TRAIN_CUP_S. */
        uint64_t lp_cu_val             : 1;  /**< [ 48: 48](RO/H) Link partner coefficient update field valid. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnex_trn_lp_s cn; */
};
typedef union bdk_ocx_lnex_trn_lp bdk_ocx_lnex_trn_lp_t;

static inline uint64_t BDK_OCX_LNEX_TRN_LP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNEX_TRN_LP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=23))
        return 0x87e0110080c8ll + 0x100ll * ((a) & 0x1f);
    __bdk_csr_fatal("OCX_LNEX_TRN_LP", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNEX_TRN_LP(a) bdk_ocx_lnex_trn_lp_t
#define bustype_BDK_OCX_LNEX_TRN_LP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNEX_TRN_LP(a) "OCX_LNEX_TRN_LP"
#define device_bar_BDK_OCX_LNEX_TRN_LP(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNEX_TRN_LP(a) (a)
#define arguments_BDK_OCX_LNEX_TRN_LP(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_lne_dbg
 *
 * OCX Lane Debug Register
 */
union bdk_ocx_lne_dbg
{
    uint64_t u;
    struct bdk_ocx_lne_dbg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t timeout               : 24; /**< [ 63: 40](R/W/H) Number of core-clock cycles (RCLKs) used by the bad lane timer. If this timer
                                                                 expires before all enabled lanes can be made ready, then any lane that is not
                                                                 ready is disabled via OCX_QLM()_CFG[SER_LANE_BAD]. For diagnostic use only. */
        uint64_t reserved_38_39        : 2;
        uint64_t frc_stats_ena         : 1;  /**< [ 37: 37](R/W) Enable FRC statistic counters. */
        uint64_t rx_dis_psh_skip       : 1;  /**< [ 36: 36](R/W/H) When [RX_DIS_PSH_SKIP] = 0, skip words are destriped. When [RX_DIS_PSH_SKIP] =
                                                                 1, skip words are discarded in the lane logic. If the lane is in internal
                                                                 loopback mode, [RX_DIS_PSH_SKIP] is ignored and skip words are always discarded
                                                                 in the lane logic. */
        uint64_t rx_mfrm_len           : 2;  /**< [ 35: 34](R/W/H) The quantity of data received on each lane including one sync word, scrambler state,
                                                                 diagnostic word, zero or more skip words, and the data payload.
                                                                 0x0 = 2048 words.
                                                                 0x1 = 1024 words.
                                                                 0x2 = 512 words.
                                                                 0x3 = 128 words. */
        uint64_t rx_dis_ukwn           : 1;  /**< [ 33: 33](R/W) Disable normal response to unknown words. They are still logged but do not cause an error
                                                                 to all open channels. */
        uint64_t rx_dis_scram          : 1;  /**< [ 32: 32](R/W) Disable lane scrambler. */
        uint64_t reserved_5_31         : 27;
        uint64_t tx_lane_rev           : 1;  /**< [  4:  4](R/W) TX lane reversal. When enabled, lane destriping is performed from the most significant
                                                                 lane enabled to least significant lane enabled [QLM_SELECT] must be 0x0 before changing
                                                                 [LANE_REV]. */
        uint64_t tx_mfrm_len           : 2;  /**< [  3:  2](R/W/H) The quantity of data sent on each lane including one sync word, scrambler state,
                                                                 diagnostic word, zero or more skip words, and the data payload.
                                                                 0x0 = 2048 words.
                                                                 0x1 = 1024 words.
                                                                 0x2 = 512 words.
                                                                 0x3 = 128 words. */
        uint64_t tx_dis_dispr          : 1;  /**< [  1:  1](R/W) Disparity disable. */
        uint64_t tx_dis_scram          : 1;  /**< [  0:  0](R/W) Scrambler disable. */
#else /* Word 0 - Little Endian */
        uint64_t tx_dis_scram          : 1;  /**< [  0:  0](R/W) Scrambler disable. */
        uint64_t tx_dis_dispr          : 1;  /**< [  1:  1](R/W) Disparity disable. */
        uint64_t tx_mfrm_len           : 2;  /**< [  3:  2](R/W/H) The quantity of data sent on each lane including one sync word, scrambler state,
                                                                 diagnostic word, zero or more skip words, and the data payload.
                                                                 0x0 = 2048 words.
                                                                 0x1 = 1024 words.
                                                                 0x2 = 512 words.
                                                                 0x3 = 128 words. */
        uint64_t tx_lane_rev           : 1;  /**< [  4:  4](R/W) TX lane reversal. When enabled, lane destriping is performed from the most significant
                                                                 lane enabled to least significant lane enabled [QLM_SELECT] must be 0x0 before changing
                                                                 [LANE_REV]. */
        uint64_t reserved_5_31         : 27;
        uint64_t rx_dis_scram          : 1;  /**< [ 32: 32](R/W) Disable lane scrambler. */
        uint64_t rx_dis_ukwn           : 1;  /**< [ 33: 33](R/W) Disable normal response to unknown words. They are still logged but do not cause an error
                                                                 to all open channels. */
        uint64_t rx_mfrm_len           : 2;  /**< [ 35: 34](R/W/H) The quantity of data received on each lane including one sync word, scrambler state,
                                                                 diagnostic word, zero or more skip words, and the data payload.
                                                                 0x0 = 2048 words.
                                                                 0x1 = 1024 words.
                                                                 0x2 = 512 words.
                                                                 0x3 = 128 words. */
        uint64_t rx_dis_psh_skip       : 1;  /**< [ 36: 36](R/W/H) When [RX_DIS_PSH_SKIP] = 0, skip words are destriped. When [RX_DIS_PSH_SKIP] =
                                                                 1, skip words are discarded in the lane logic. If the lane is in internal
                                                                 loopback mode, [RX_DIS_PSH_SKIP] is ignored and skip words are always discarded
                                                                 in the lane logic. */
        uint64_t frc_stats_ena         : 1;  /**< [ 37: 37](R/W) Enable FRC statistic counters. */
        uint64_t reserved_38_39        : 2;
        uint64_t timeout               : 24; /**< [ 63: 40](R/W/H) Number of core-clock cycles (RCLKs) used by the bad lane timer. If this timer
                                                                 expires before all enabled lanes can be made ready, then any lane that is not
                                                                 ready is disabled via OCX_QLM()_CFG[SER_LANE_BAD]. For diagnostic use only. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lne_dbg_s cn; */
};
typedef union bdk_ocx_lne_dbg bdk_ocx_lne_dbg_t;

#define BDK_OCX_LNE_DBG BDK_OCX_LNE_DBG_FUNC()
static inline uint64_t BDK_OCX_LNE_DBG_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNE_DBG_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e01100ff00ll;
    __bdk_csr_fatal("OCX_LNE_DBG", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_LNE_DBG bdk_ocx_lne_dbg_t
#define bustype_BDK_OCX_LNE_DBG BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNE_DBG "OCX_LNE_DBG"
#define device_bar_BDK_OCX_LNE_DBG 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNE_DBG 0
#define arguments_BDK_OCX_LNE_DBG -1,-1,-1,-1

/**
 * Register (RSL) ocx_lnk#_cfg
 *
 * OCX Link 0-2 Configuration Registers
 */
union bdk_ocx_lnkx_cfg
{
    uint64_t u;
    struct bdk_ocx_lnkx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t qlm_manual            : 6;  /**< [ 53: 48](R/W/H) QLM manual mask, where each bit corresponds to a QLM. A link automatically selects a QLM
                                                                 unless either QLM_MANUAL[QLM] = 1 or a QLM is not eligible for the link.

                                                                 _ QLM_MANUAL\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_MANUAL\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_MANUAL\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_MANUAL\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_MANUAL\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_MANUAL\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.

                                                                 During a cold reset, this field is initialized to 0x3F when OCI_SPD\<3:0\> == 0xF.

                                                                 During a cold reset, this field is initialized to 0x0 when OCI_SPD\<3:0\> != 0xF.

                                                                 This field is not modified by hardware at any other time.

                                                                 This field is not affected by soft or warm reset. */
        uint64_t reserved_38_47        : 10;
        uint64_t qlm_select            : 6;  /**< [ 37: 32](R/W/H) QLM select mask, where each bit corresponds to a QLM. A link will transmit/receive data
                                                                 using only the selected QLMs. A link is enabled if any QLM is selected. The same QLM
                                                                 should not be selected for multiple links.
                                                                 [LANE_REV] has no effect on this mapping.

                                                                 _ QLM_SELECT\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_SELECT\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_SELECT\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_SELECT\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_SELECT\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_SELECT\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.
                                                                 _ LINK 2 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 automatically selects QLM0 when [QLM_MANUAL]\<0\>=0.
                                                                 _ LINK 0 automatically selects QLM1 when [QLM_MANUAL]\<1\>=0.
                                                                 _ LINK 0 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=0.
                                                                 _ LINK 1 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=1.
                                                                 _ LINK 1 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=1.
                                                                 _ LINK 2 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=0.
                                                                 _ LINK 3 automatically selects QLM4 when [QLM_MANUAL]\<4\>=0.
                                                                 _ LINK 3 automatically selects QLM5 when [QLM_MANUAL]\<5\>=0.

                                                                 A link with [QLM_SELECT] = 0x0 is invalid and will never exchange traffic with the
                                                                 link partner. */
        uint64_t reserved_29_31        : 3;
        uint64_t data_rate             : 13; /**< [ 28: 16](R/W/H) The number of core-clock cycles (RCLKs) to transmit 32 words, where each word is
                                                                 67 bits. Hardware automatically calculates a conservative value for this
                                                                 field. Software can override the calculation by writing
                                                                 TX_DAT_RATE=roundup((67*RCLK / GBAUD)*32). */
        uint64_t low_delay             : 6;  /**< [ 15: 10](R/W) The delay before reacting to a lane low data indication, as a multiple of 64 core-clock
                                                                 cycles (RCLKs). */
        uint64_t lane_align_dis        : 1;  /**< [  9:  9](R/W/H) Disable the RX lane alignment. */
        uint64_t lane_rev              : 1;  /**< [  8:  8](R/W/H) RX lane reversal.   When enabled, lane destriping is performed from the most significant
                                                                 lane enabled to least significant lane enabled [QLM_SELECT] must be 0x0 before changing
                                                                 [LANE_REV]. */
        uint64_t lane_rev_auto         : 1;  /**< [  7:  7](RAZ) Reserved. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t lane_rev_auto         : 1;  /**< [  7:  7](RAZ) Reserved. */
        uint64_t lane_rev              : 1;  /**< [  8:  8](R/W/H) RX lane reversal.   When enabled, lane destriping is performed from the most significant
                                                                 lane enabled to least significant lane enabled [QLM_SELECT] must be 0x0 before changing
                                                                 [LANE_REV]. */
        uint64_t lane_align_dis        : 1;  /**< [  9:  9](R/W/H) Disable the RX lane alignment. */
        uint64_t low_delay             : 6;  /**< [ 15: 10](R/W) The delay before reacting to a lane low data indication, as a multiple of 64 core-clock
                                                                 cycles (RCLKs). */
        uint64_t data_rate             : 13; /**< [ 28: 16](R/W/H) The number of core-clock cycles (RCLKs) to transmit 32 words, where each word is
                                                                 67 bits. Hardware automatically calculates a conservative value for this
                                                                 field. Software can override the calculation by writing
                                                                 TX_DAT_RATE=roundup((67*RCLK / GBAUD)*32). */
        uint64_t reserved_29_31        : 3;
        uint64_t qlm_select            : 6;  /**< [ 37: 32](R/W/H) QLM select mask, where each bit corresponds to a QLM. A link will transmit/receive data
                                                                 using only the selected QLMs. A link is enabled if any QLM is selected. The same QLM
                                                                 should not be selected for multiple links.
                                                                 [LANE_REV] has no effect on this mapping.

                                                                 _ QLM_SELECT\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_SELECT\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_SELECT\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_SELECT\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_SELECT\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_SELECT\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.
                                                                 _ LINK 2 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 automatically selects QLM0 when [QLM_MANUAL]\<0\>=0.
                                                                 _ LINK 0 automatically selects QLM1 when [QLM_MANUAL]\<1\>=0.
                                                                 _ LINK 0 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=0.
                                                                 _ LINK 1 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=1.
                                                                 _ LINK 1 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=1.
                                                                 _ LINK 2 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=0.
                                                                 _ LINK 3 automatically selects QLM4 when [QLM_MANUAL]\<4\>=0.
                                                                 _ LINK 3 automatically selects QLM5 when [QLM_MANUAL]\<5\>=0.

                                                                 A link with [QLM_SELECT] = 0x0 is invalid and will never exchange traffic with the
                                                                 link partner. */
        uint64_t reserved_38_47        : 10;
        uint64_t qlm_manual            : 6;  /**< [ 53: 48](R/W/H) QLM manual mask, where each bit corresponds to a QLM. A link automatically selects a QLM
                                                                 unless either QLM_MANUAL[QLM] = 1 or a QLM is not eligible for the link.

                                                                 _ QLM_MANUAL\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_MANUAL\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_MANUAL\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_MANUAL\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_MANUAL\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_MANUAL\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.

                                                                 During a cold reset, this field is initialized to 0x3F when OCI_SPD\<3:0\> == 0xF.

                                                                 During a cold reset, this field is initialized to 0x0 when OCI_SPD\<3:0\> != 0xF.

                                                                 This field is not modified by hardware at any other time.

                                                                 This field is not affected by soft or warm reset. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_lnkx_cfg_s cn88xxp1; */
    struct bdk_ocx_lnkx_cfg_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t qlm_manual            : 6;  /**< [ 53: 48](R/W/H) QLM manual mask, where each bit corresponds to a QLM. A link automatically selects a QLM
                                                                 unless either QLM_MANUAL[QLM] = 1 or a QLM is not eligible for the link.

                                                                 _ QLM_MANUAL\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_MANUAL\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_MANUAL\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_MANUAL\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_MANUAL\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_MANUAL\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.

                                                                 During a cold reset, this field is initialized to 0x3F when OCI_SPD\<3:0\> == 0xF.

                                                                 During a cold reset, this field is initialized to 0x0 when OCI_SPD\<3:0\> != 0xF.

                                                                 This field is not modified by hardware at any other time.

                                                                 This field is not affected by soft or warm reset. */
        uint64_t reserved_38_47        : 10;
        uint64_t qlm_select            : 6;  /**< [ 37: 32](R/W/H) QLM select mask, where each bit corresponds to a QLM. A link will transmit/receive data
                                                                 using only the selected QLMs. A link is enabled if any QLM is selected. The same QLM
                                                                 should not be selected for multiple links.
                                                                 [LANE_REV] has no effect on this mapping.

                                                                 _ QLM_SELECT\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_SELECT\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_SELECT\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_SELECT\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_SELECT\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_SELECT\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.
                                                                 _ LINK 2 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 automatically selects QLM0 when [QLM_MANUAL]\<0\>=0.
                                                                 _ LINK 0 automatically selects QLM1 when [QLM_MANUAL]\<1\>=0.
                                                                 _ LINK 0 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=0.
                                                                 _ LINK 1 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=1.
                                                                 _ LINK 1 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=1.
                                                                 _ LINK 2 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=0.
                                                                 _ LINK 3 automatically selects QLM4 when [QLM_MANUAL]\<4\>=0.
                                                                 _ LINK 3 automatically selects QLM5 when [QLM_MANUAL]\<5\>=0.

                                                                 A link with [QLM_SELECT] = 0x0 is invalid and will never exchange traffic with the
                                                                 link partner. */
        uint64_t reserved_29_31        : 3;
        uint64_t data_rate             : 13; /**< [ 28: 16](R/W/H) The number of core-clock cycles (RCLKs) to transmit 32 words, where each word is
                                                                 67 bits. Hardware automatically calculates a conservative value for this
                                                                 field. Software can override the calculation by writing
                                                                 TX_DAT_RATE=roundup((67*RCLK / GBAUD)*32). */
        uint64_t low_delay             : 6;  /**< [ 15: 10](R/W) The delay before reacting to a lane low data indication, as a multiple of 64 core-clock
                                                                 cycles (RCLKs). */
        uint64_t lane_align_dis        : 1;  /**< [  9:  9](R/W/H) Disable the RX lane alignment. */
        uint64_t lane_rev              : 1;  /**< [  8:  8](R/W/H) RX lane reversal.   When enabled, lane destriping is performed from the most significant
                                                                 lane enabled to least significant lane enabled [QLM_SELECT] must be 0x0 before changing
                                                                 [LANE_REV]. */
        uint64_t lane_rev_auto         : 1;  /**< [  7:  7](R/W) Automatically detect RX lane reversal.  When enabled, [LANE_REV] is updated by
                                                                 hardware. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t lane_rev_auto         : 1;  /**< [  7:  7](R/W) Automatically detect RX lane reversal.  When enabled, [LANE_REV] is updated by
                                                                 hardware. */
        uint64_t lane_rev              : 1;  /**< [  8:  8](R/W/H) RX lane reversal.   When enabled, lane destriping is performed from the most significant
                                                                 lane enabled to least significant lane enabled [QLM_SELECT] must be 0x0 before changing
                                                                 [LANE_REV]. */
        uint64_t lane_align_dis        : 1;  /**< [  9:  9](R/W/H) Disable the RX lane alignment. */
        uint64_t low_delay             : 6;  /**< [ 15: 10](R/W) The delay before reacting to a lane low data indication, as a multiple of 64 core-clock
                                                                 cycles (RCLKs). */
        uint64_t data_rate             : 13; /**< [ 28: 16](R/W/H) The number of core-clock cycles (RCLKs) to transmit 32 words, where each word is
                                                                 67 bits. Hardware automatically calculates a conservative value for this
                                                                 field. Software can override the calculation by writing
                                                                 TX_DAT_RATE=roundup((67*RCLK / GBAUD)*32). */
        uint64_t reserved_29_31        : 3;
        uint64_t qlm_select            : 6;  /**< [ 37: 32](R/W/H) QLM select mask, where each bit corresponds to a QLM. A link will transmit/receive data
                                                                 using only the selected QLMs. A link is enabled if any QLM is selected. The same QLM
                                                                 should not be selected for multiple links.
                                                                 [LANE_REV] has no effect on this mapping.

                                                                 _ QLM_SELECT\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_SELECT\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_SELECT\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_SELECT\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_SELECT\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_SELECT\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.
                                                                 _ LINK 2 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 may not select QLM2 or QLM3 when LINK1 selects any QLM.
                                                                 _ LINK 0 automatically selects QLM0 when [QLM_MANUAL]\<0\>=0.
                                                                 _ LINK 0 automatically selects QLM1 when [QLM_MANUAL]\<1\>=0.
                                                                 _ LINK 0 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=0.
                                                                 _ LINK 1 automatically selects QLM2 when [QLM_MANUAL]\<2\>=0 and OCX_QLM2_CFG[SER_LOCAL]=1.
                                                                 _ LINK 1 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=1.
                                                                 _ LINK 2 automatically selects QLM3 when [QLM_MANUAL]\<3\>=0 and OCX_QLM3_CFG[SER_LOCAL]=0.
                                                                 _ LINK 3 automatically selects QLM4 when [QLM_MANUAL]\<4\>=0.
                                                                 _ LINK 3 automatically selects QLM5 when [QLM_MANUAL]\<5\>=0.

                                                                 A link with [QLM_SELECT] = 0x0 is invalid and will never exchange traffic with the
                                                                 link partner. */
        uint64_t reserved_38_47        : 10;
        uint64_t qlm_manual            : 6;  /**< [ 53: 48](R/W/H) QLM manual mask, where each bit corresponds to a QLM. A link automatically selects a QLM
                                                                 unless either QLM_MANUAL[QLM] = 1 or a QLM is not eligible for the link.

                                                                 _ QLM_MANUAL\<0\> = LNE(0..3) = QLM0.
                                                                 _ QLM_MANUAL\<1\> = LNE(7..4) = QLM1.
                                                                 _ QLM_MANUAL\<2\> = LNE(11..8) = QLM2.
                                                                 _ QLM_MANUAL\<3\> = LNE(15..12) = QLM3.
                                                                 _ QLM_MANUAL\<4\> = LNE(19..16) = QLM4.
                                                                 _ QLM_MANUAL\<5\> = LNE(23..23) = QLM5.
                                                                 _ LINK 0 may not select QLM4, QLM5.
                                                                 _ LINK 1 may not select QLM0, QLM1, QLM4, QLM5.
                                                                 _ LINK 2 may not select QLM0, QLM1.

                                                                 During a cold reset, this field is initialized to 0x3F when OCI_SPD\<3:0\> == 0xF.

                                                                 During a cold reset, this field is initialized to 0x0 when OCI_SPD\<3:0\> != 0xF.

                                                                 This field is not modified by hardware at any other time.

                                                                 This field is not affected by soft or warm reset. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_ocx_lnkx_cfg bdk_ocx_lnkx_cfg_t;

static inline uint64_t BDK_OCX_LNKX_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_LNKX_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e01100f900ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_LNKX_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_LNKX_CFG(a) bdk_ocx_lnkx_cfg_t
#define bustype_BDK_OCX_LNKX_CFG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_LNKX_CFG(a) "OCX_LNKX_CFG"
#define device_bar_BDK_OCX_LNKX_CFG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_LNKX_CFG(a) (a)
#define arguments_BDK_OCX_LNKX_CFG(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_msix_pba#
 *
 * OCX MSI-X Pending Bit Array Register
 * This register is the MSI-X PBA table; the bit number is indexed by the OCX_INT_VEC_E
 * enumeration.
 */
union bdk_ocx_msix_pbax
{
    uint64_t u;
    struct bdk_ocx_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated OCX_MSIX_VEC()_CTL, enumerated by
                                                                 OCX_INT_VEC_E. Bits that have no associated OCX_INT_VEC_E are 0. */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated OCX_MSIX_VEC()_CTL, enumerated by
                                                                 OCX_INT_VEC_E. Bits that have no associated OCX_INT_VEC_E are 0. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_msix_pbax_s cn; */
};
typedef union bdk_ocx_msix_pbax bdk_ocx_msix_pbax_t;

static inline uint64_t BDK_OCX_MSIX_PBAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_MSIX_PBAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a==0))
        return 0x87e011ff0000ll + 8ll * ((a) & 0x0);
    __bdk_csr_fatal("OCX_MSIX_PBAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_MSIX_PBAX(a) bdk_ocx_msix_pbax_t
#define bustype_BDK_OCX_MSIX_PBAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_MSIX_PBAX(a) "OCX_MSIX_PBAX"
#define device_bar_BDK_OCX_MSIX_PBAX(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_OCX_MSIX_PBAX(a) (a)
#define arguments_BDK_OCX_MSIX_PBAX(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_msix_vec#_addr
 *
 * OCX MSI-X Vector-Table Address Registers
 * This register is the MSI-X vector table, indexed by the OCX_INT_VEC_E enumeration.
 */
union bdk_ocx_msix_vecx_addr
{
    uint64_t u;
    struct bdk_ocx_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's OCX_MSIX_VEC()_ADDR, OCX_MSIX_VEC()_CTL, and
                                                                 corresponding bit of OCX_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_OCX_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) =
                                                                 1, all vectors are secure and function as if [SECVEC] was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's OCX_MSIX_VEC()_ADDR, OCX_MSIX_VEC()_CTL, and
                                                                 corresponding bit of OCX_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_OCX_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) =
                                                                 1, all vectors are secure and function as if [SECVEC] was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_msix_vecx_addr_s cn; */
};
typedef union bdk_ocx_msix_vecx_addr bdk_ocx_msix_vecx_addr_t;

static inline uint64_t BDK_OCX_MSIX_VECX_ADDR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_MSIX_VECX_ADDR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e011f00000ll + 0x10ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_MSIX_VECX_ADDR", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_MSIX_VECX_ADDR(a) bdk_ocx_msix_vecx_addr_t
#define bustype_BDK_OCX_MSIX_VECX_ADDR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_MSIX_VECX_ADDR(a) "OCX_MSIX_VECX_ADDR"
#define device_bar_BDK_OCX_MSIX_VECX_ADDR(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_OCX_MSIX_VECX_ADDR(a) (a)
#define arguments_BDK_OCX_MSIX_VECX_ADDR(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_msix_vec#_ctl
 *
 * OCX MSI-X Vector-Table Control and Data Registers
 * This register is the MSI-X vector table, indexed by the OCX_INT_VEC_E enumeration.
 */
union bdk_ocx_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_ocx_msix_vecx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts are sent to this vector. */
        uint64_t reserved_20_31        : 12;
        uint64_t data                  : 20; /**< [ 19:  0](R/W) Data to use for MSI-X delivery of this vector. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 20; /**< [ 19:  0](R/W) Data to use for MSI-X delivery of this vector. */
        uint64_t reserved_20_31        : 12;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts are sent to this vector. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_msix_vecx_ctl_s cn; */
};
typedef union bdk_ocx_msix_vecx_ctl bdk_ocx_msix_vecx_ctl_t;

static inline uint64_t BDK_OCX_MSIX_VECX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_MSIX_VECX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e011f00008ll + 0x10ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_MSIX_VECX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_MSIX_VECX_CTL(a) bdk_ocx_msix_vecx_ctl_t
#define bustype_BDK_OCX_MSIX_VECX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_MSIX_VECX_CTL(a) "OCX_MSIX_VECX_CTL"
#define device_bar_BDK_OCX_MSIX_VECX_CTL(a) 0x4 /* PF_BAR4 */
#define busnum_BDK_OCX_MSIX_VECX_CTL(a) (a)
#define arguments_BDK_OCX_MSIX_VECX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_pp_cmd
 *
 * OCX Core Address Register
 * Contains the address, read size and write mask to used for the core operation. Write data
 * should be written first and placed in the OCX_PP_WR_DATA register. Writing this register
 * starts the operation. A second write to this register while an operation is in progress will
 * stall. Data is placed in the OCX_PP_RD_DATA register.
 * This register has the same bit fields as OCX_WIN_CMD.
 */
union bdk_ocx_pp_cmd
{
    uint64_t u;
    struct bdk_ocx_pp_cmd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wr_mask               : 8;  /**< [ 63: 56](R/W) Mask for the data to be written. When a bit is 1, the corresponding byte will be written.
                                                                 The values of this field must be contiguous and for 1, 2, 4, or 8 byte operations and
                                                                 aligned to operation size. A value of 0 will produce unpredictable results. Field is
                                                                 ignored during a read (LD_OP=1). */
        uint64_t reserved_54_55        : 2;
        uint64_t el                    : 2;  /**< [ 53: 52](R/W) Execution level.  This field is used to supply the execution level of the generated load
                                                                 or store command. */
        uint64_t nsecure               : 1;  /**< [ 51: 51](R/W) Nonsecure mode.  Setting this bit causes the generated load or store command to be
                                                                 considered nonsecure. */
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command sent with the read:
                                                                 0x0 = Load 1-bytes.
                                                                 0x1 = Load 2-bytes.
                                                                 0x2 = Load 4-bytes.
                                                                 0x3 = Load 8-bytes. */
        uint64_t ld_op                 : 1;  /**< [ 48: 48](R/W) Operation type:
                                                                 0 = Store.
                                                                 1 = Load operation. */
        uint64_t addr                  : 48; /**< [ 47:  0](R/W) The address used in both the load and store operations:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = NCB_ID.
                                                                 \<35:0\>  = Address.

                                                                 When \<43:36\> NCB_ID is RSL (0x7E) address field is defined as:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = 0x7E.
                                                                 \<35:32\> = Reserved.
                                                                 \<31:24\> = RSL_ID.
                                                                 \<23:0\>  = RSL register offset.

                                                                 \<2:0\> are ignored in a store operation. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 48; /**< [ 47:  0](R/W) The address used in both the load and store operations:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = NCB_ID.
                                                                 \<35:0\>  = Address.

                                                                 When \<43:36\> NCB_ID is RSL (0x7E) address field is defined as:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = 0x7E.
                                                                 \<35:32\> = Reserved.
                                                                 \<31:24\> = RSL_ID.
                                                                 \<23:0\>  = RSL register offset.

                                                                 \<2:0\> are ignored in a store operation. */
        uint64_t ld_op                 : 1;  /**< [ 48: 48](R/W) Operation type:
                                                                 0 = Store.
                                                                 1 = Load operation. */
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command sent with the read:
                                                                 0x0 = Load 1-bytes.
                                                                 0x1 = Load 2-bytes.
                                                                 0x2 = Load 4-bytes.
                                                                 0x3 = Load 8-bytes. */
        uint64_t nsecure               : 1;  /**< [ 51: 51](R/W) Nonsecure mode.  Setting this bit causes the generated load or store command to be
                                                                 considered nonsecure. */
        uint64_t el                    : 2;  /**< [ 53: 52](R/W) Execution level.  This field is used to supply the execution level of the generated load
                                                                 or store command. */
        uint64_t reserved_54_55        : 2;
        uint64_t wr_mask               : 8;  /**< [ 63: 56](R/W) Mask for the data to be written. When a bit is 1, the corresponding byte will be written.
                                                                 The values of this field must be contiguous and for 1, 2, 4, or 8 byte operations and
                                                                 aligned to operation size. A value of 0 will produce unpredictable results. Field is
                                                                 ignored during a read (LD_OP=1). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_pp_cmd_s cn; */
};
typedef union bdk_ocx_pp_cmd bdk_ocx_pp_cmd_t;

#define BDK_OCX_PP_CMD BDK_OCX_PP_CMD_FUNC()
static inline uint64_t BDK_OCX_PP_CMD_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_PP_CMD_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0110000c8ll;
    __bdk_csr_fatal("OCX_PP_CMD", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_PP_CMD bdk_ocx_pp_cmd_t
#define bustype_BDK_OCX_PP_CMD BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_PP_CMD "OCX_PP_CMD"
#define device_bar_BDK_OCX_PP_CMD 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_PP_CMD 0
#define arguments_BDK_OCX_PP_CMD -1,-1,-1,-1

/**
 * Register (RSL) ocx_pp_rd_data
 *
 * OCX Core Read Data Register
 * This register is the read response data associated with core command. Reads all-ones until
 * response is received.
 * This register has the same bit fields as OCX_WIN_RD_DATA.
 */
union bdk_ocx_pp_rd_data
{
    uint64_t u;
    struct bdk_ocx_pp_rd_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](RO/H) Read response data. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](RO/H) Read response data. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_pp_rd_data_s cn; */
};
typedef union bdk_ocx_pp_rd_data bdk_ocx_pp_rd_data_t;

#define BDK_OCX_PP_RD_DATA BDK_OCX_PP_RD_DATA_FUNC()
static inline uint64_t BDK_OCX_PP_RD_DATA_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_PP_RD_DATA_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0110000d0ll;
    __bdk_csr_fatal("OCX_PP_RD_DATA", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_PP_RD_DATA bdk_ocx_pp_rd_data_t
#define bustype_BDK_OCX_PP_RD_DATA BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_PP_RD_DATA "OCX_PP_RD_DATA"
#define device_bar_BDK_OCX_PP_RD_DATA 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_PP_RD_DATA 0
#define arguments_BDK_OCX_PP_RD_DATA -1,-1,-1,-1

/**
 * Register (RSL) ocx_pp_wr_data
 *
 * OCX Core Data Register
 * Contains the data to write to the address located in OCX_PP_CMD. Writing this register will
 * cause a write operation to take place.
 * This register has the same bit fields as OCX_WIN_WR_DATA.
 */
union bdk_ocx_pp_wr_data
{
    uint64_t u;
    struct bdk_ocx_pp_wr_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#else /* Word 0 - Little Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_pp_wr_data_s cn; */
};
typedef union bdk_ocx_pp_wr_data bdk_ocx_pp_wr_data_t;

#define BDK_OCX_PP_WR_DATA BDK_OCX_PP_WR_DATA_FUNC()
static inline uint64_t BDK_OCX_PP_WR_DATA_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_PP_WR_DATA_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0110000c0ll;
    __bdk_csr_fatal("OCX_PP_WR_DATA", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_PP_WR_DATA bdk_ocx_pp_wr_data_t
#define bustype_BDK_OCX_PP_WR_DATA BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_PP_WR_DATA "OCX_PP_WR_DATA"
#define device_bar_BDK_OCX_PP_WR_DATA 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_PP_WR_DATA 0
#define arguments_BDK_OCX_PP_WR_DATA -1,-1,-1,-1

/**
 * Register (RSL) ocx_qlm#_cfg
 *
 * OCX QLM 0-5 Configuration Registers
 */
union bdk_ocx_qlmx_cfg
{
    uint64_t u;
    struct bdk_ocx_qlmx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ser_low               : 4;  /**< [ 63: 60](R/W/H) Reduce latency by limiting the amount of data in flight for each SerDes.  Writing to 0
                                                                 causes hardware to determine a typically optimal value. */
        uint64_t reserved_42_59        : 18;
        uint64_t ser_limit             : 10; /**< [ 41: 32](RAZ) Reserved. */
        uint64_t crd_dis               : 1;  /**< [ 31: 31](R/W) For diagnostic use only. */
        uint64_t reserved_27_30        : 4;
        uint64_t trn_rxeq_only         : 1;  /**< [ 26: 26](R/W/H) Shortened training sequence.  Initialized to 1 during cold reset when OCI_SPD\<3:0\> pins
                                                                 indicate 5 GBAUD \<=speed \< 8 GBAUD. Otherwise, initialized to 0 during a cold reset. This
                                                                 field is not affected by soft or warm reset.  For diagnostic use only. */
        uint64_t timer_dis             : 1;  /**< [ 25: 25](R/W/H) Disable bad lane timer. A timer counts core clocks (RCLKs) when any enabled lane is not
                                                                 ready, i.e. not in the scrambler sync state. If this timer expires before all enabled
                                                                 lanes can be made ready, then any lane which is not ready is disabled via
                                                                 OCX_QLM(0..5)_CFG[SER_LANE_BAD]. This field is not affected by soft or warm reset. */
        uint64_t trn_ena               : 1;  /**< [ 24: 24](R/W/H) Link training enable. Link training is performed during auto link bring up. Initialized to
                                                                 1 during cold reset when OCI_SPD\<3:0\> pins indicate speed \>= 5 GBAUD. Otherwise,
                                                                 initialized to 0 during a cold reset. This field is not affected by soft or warm reset. */
        uint64_t ser_lane_ready        : 4;  /**< [ 23: 20](R/W/H) SerDes lanes that are ready for bundling into the link. */
        uint64_t ser_lane_bad          : 4;  /**< [ 19: 16](R/W/H) SerDes lanes excluded from use. */
        uint64_t reserved_7_15         : 9;
        uint64_t ser_lane_rev          : 1;  /**< [  6:  6](RO/H) SerDes lane reversal has been detected. */
        uint64_t ser_rxpol_auto        : 1;  /**< [  5:  5](R/W) SerDes lane receive polarity auto detection mode. */
        uint64_t ser_rxpol             : 1;  /**< [  4:  4](R/W) SerDes lane receive polarity:
                                                                 0 = RX without inversion.
                                                                 1 = RX with inversion. */
        uint64_t ser_txpol             : 1;  /**< [  3:  3](R/W) SerDes lane transmit polarity:
                                                                 0 = TX without inversion.
                                                                 1 = TX with inversion. */
        uint64_t reserved_1_2          : 2;
        uint64_t ser_local             : 1;  /**< [  0:  0](R/W/H) Auto initialization may set OCX_LNK0_CFG[QLM_SELECT\<2\>] = 1 only if
                                                                 OCX_QLM2_CFG[SER_LOCAL] = 0.
                                                                 Auto initialization may set OCX_LNK1_CFG[QLM_SELECT\<2\>] = 1 only if
                                                                 OCX_QLM2_CFG[SER_LOCAL] = 1.
                                                                 Auto initialization may set OCX_LNK1_CFG[QLM_SELECT\<3\>] = 1 only if
                                                                 OCX_QLM3_CFG[SER_LOCAL] = 1.
                                                                 Auto initialization may set OCX_LNK2_CFG[QLM_SELECT\<3\>] = 1 only if
                                                                 OCX_QLM3_CFG[SER_LOCAL] = 0.

                                                                 QLM0/1 can only participate in LNK0; therefore
                                                                 OCX_QLM0/1_CFG[SER_LOCAL] has no effect.
                                                                 QLM4/5 can only participate in LNK2; therefore
                                                                 OCX_QLM4/5_CFG[SER_LOCAL] has no effect.

                                                                 During a cold reset, initialized as follows:
                                                                 _ OCX_QLM2_CFG[SER_LOCAL] = pi_oci2_link1.
                                                                 _ OCX_QLM3_CFG[SER_LOCAL] = pi_oci3_link1.

                                                                 The combo of pi_oci2_link1=1 and pi_oci3_link1=0 is illegal.

                                                                 The combo of OCX_QLM2_CFG[SER_LOCAL]=1 and OCX_QLM3_CFG[SER_LOCAL] = 0 is illegal. */
#else /* Word 0 - Little Endian */
        uint64_t ser_local             : 1;  /**< [  0:  0](R/W/H) Auto initialization may set OCX_LNK0_CFG[QLM_SELECT\<2\>] = 1 only if
                                                                 OCX_QLM2_CFG[SER_LOCAL] = 0.
                                                                 Auto initialization may set OCX_LNK1_CFG[QLM_SELECT\<2\>] = 1 only if
                                                                 OCX_QLM2_CFG[SER_LOCAL] = 1.
                                                                 Auto initialization may set OCX_LNK1_CFG[QLM_SELECT\<3\>] = 1 only if
                                                                 OCX_QLM3_CFG[SER_LOCAL] = 1.
                                                                 Auto initialization may set OCX_LNK2_CFG[QLM_SELECT\<3\>] = 1 only if
                                                                 OCX_QLM3_CFG[SER_LOCAL] = 0.

                                                                 QLM0/1 can only participate in LNK0; therefore
                                                                 OCX_QLM0/1_CFG[SER_LOCAL] has no effect.
                                                                 QLM4/5 can only participate in LNK2; therefore
                                                                 OCX_QLM4/5_CFG[SER_LOCAL] has no effect.

                                                                 During a cold reset, initialized as follows:
                                                                 _ OCX_QLM2_CFG[SER_LOCAL] = pi_oci2_link1.
                                                                 _ OCX_QLM3_CFG[SER_LOCAL] = pi_oci3_link1.

                                                                 The combo of pi_oci2_link1=1 and pi_oci3_link1=0 is illegal.

                                                                 The combo of OCX_QLM2_CFG[SER_LOCAL]=1 and OCX_QLM3_CFG[SER_LOCAL] = 0 is illegal. */
        uint64_t reserved_1_2          : 2;
        uint64_t ser_txpol             : 1;  /**< [  3:  3](R/W) SerDes lane transmit polarity:
                                                                 0 = TX without inversion.
                                                                 1 = TX with inversion. */
        uint64_t ser_rxpol             : 1;  /**< [  4:  4](R/W) SerDes lane receive polarity:
                                                                 0 = RX without inversion.
                                                                 1 = RX with inversion. */
        uint64_t ser_rxpol_auto        : 1;  /**< [  5:  5](R/W) SerDes lane receive polarity auto detection mode. */
        uint64_t ser_lane_rev          : 1;  /**< [  6:  6](RO/H) SerDes lane reversal has been detected. */
        uint64_t reserved_7_15         : 9;
        uint64_t ser_lane_bad          : 4;  /**< [ 19: 16](R/W/H) SerDes lanes excluded from use. */
        uint64_t ser_lane_ready        : 4;  /**< [ 23: 20](R/W/H) SerDes lanes that are ready for bundling into the link. */
        uint64_t trn_ena               : 1;  /**< [ 24: 24](R/W/H) Link training enable. Link training is performed during auto link bring up. Initialized to
                                                                 1 during cold reset when OCI_SPD\<3:0\> pins indicate speed \>= 5 GBAUD. Otherwise,
                                                                 initialized to 0 during a cold reset. This field is not affected by soft or warm reset. */
        uint64_t timer_dis             : 1;  /**< [ 25: 25](R/W/H) Disable bad lane timer. A timer counts core clocks (RCLKs) when any enabled lane is not
                                                                 ready, i.e. not in the scrambler sync state. If this timer expires before all enabled
                                                                 lanes can be made ready, then any lane which is not ready is disabled via
                                                                 OCX_QLM(0..5)_CFG[SER_LANE_BAD]. This field is not affected by soft or warm reset. */
        uint64_t trn_rxeq_only         : 1;  /**< [ 26: 26](R/W/H) Shortened training sequence.  Initialized to 1 during cold reset when OCI_SPD\<3:0\> pins
                                                                 indicate 5 GBAUD \<=speed \< 8 GBAUD. Otherwise, initialized to 0 during a cold reset. This
                                                                 field is not affected by soft or warm reset.  For diagnostic use only. */
        uint64_t reserved_27_30        : 4;
        uint64_t crd_dis               : 1;  /**< [ 31: 31](R/W) For diagnostic use only. */
        uint64_t ser_limit             : 10; /**< [ 41: 32](RAZ) Reserved. */
        uint64_t reserved_42_59        : 18;
        uint64_t ser_low               : 4;  /**< [ 63: 60](R/W/H) Reduce latency by limiting the amount of data in flight for each SerDes.  Writing to 0
                                                                 causes hardware to determine a typically optimal value. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_qlmx_cfg_s cn; */
};
typedef union bdk_ocx_qlmx_cfg bdk_ocx_qlmx_cfg_t;

static inline uint64_t BDK_OCX_QLMX_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_QLMX_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e01100f800ll + 8ll * ((a) & 0x7);
    __bdk_csr_fatal("OCX_QLMX_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_QLMX_CFG(a) bdk_ocx_qlmx_cfg_t
#define bustype_BDK_OCX_QLMX_CFG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_QLMX_CFG(a) "OCX_QLMX_CFG"
#define device_bar_BDK_OCX_QLMX_CFG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_QLMX_CFG(a) (a)
#define arguments_BDK_OCX_QLMX_CFG(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_align
 *
 * OCX Receive Link Align Registers
 */
union bdk_ocx_rlkx_align
{
    uint64_t u;
    struct bdk_ocx_rlkx_align_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t bad_cnt               : 32; /**< [ 63: 32](R/W/H) Number of alignment sequences received in error (i.e. those that violate the current
                                                                 alignment). Count saturates at max value. */
        uint64_t good_cnt              : 32; /**< [ 31:  0](R/W/H) Number of alignment sequences received (i.e. those that do not violate the current
                                                                 alignment). Count saturates at max value. */
#else /* Word 0 - Little Endian */
        uint64_t good_cnt              : 32; /**< [ 31:  0](R/W/H) Number of alignment sequences received (i.e. those that do not violate the current
                                                                 alignment). Count saturates at max value. */
        uint64_t bad_cnt               : 32; /**< [ 63: 32](R/W/H) Number of alignment sequences received in error (i.e. those that violate the current
                                                                 alignment). Count saturates at max value. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_align_s cn; */
};
typedef union bdk_ocx_rlkx_align bdk_ocx_rlkx_align_t;

static inline uint64_t BDK_OCX_RLKX_ALIGN(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_ALIGN(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018060ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_ALIGN", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_ALIGN(a) bdk_ocx_rlkx_align_t
#define bustype_BDK_OCX_RLKX_ALIGN(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_ALIGN(a) "OCX_RLKX_ALIGN"
#define device_bar_BDK_OCX_RLKX_ALIGN(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_ALIGN(a) (a)
#define arguments_BDK_OCX_RLKX_ALIGN(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_blk_err
 *
 * OCX Receive Link Block Error Registers
 */
union bdk_ocx_rlkx_blk_err
{
    uint64_t u;
    struct bdk_ocx_rlkx_blk_err_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t limit                 : 16; /**< [ 31: 16](R/W) Contains the number of blocks received with errors before the
                                                                 OCX_COM_LINK()_INT[BLK_ERR] interrupt is generated. */
        uint64_t count                 : 16; /**< [ 15:  0](R/W) Shows the number of blocks received with one or more errors detected. Multiple
                                                                 errors may be detected as the link starts up. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 16; /**< [ 15:  0](R/W) Shows the number of blocks received with one or more errors detected. Multiple
                                                                 errors may be detected as the link starts up. */
        uint64_t limit                 : 16; /**< [ 31: 16](R/W) Contains the number of blocks received with errors before the
                                                                 OCX_COM_LINK()_INT[BLK_ERR] interrupt is generated. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_blk_err_s cn; */
};
typedef union bdk_ocx_rlkx_blk_err bdk_ocx_rlkx_blk_err_t;

static inline uint64_t BDK_OCX_RLKX_BLK_ERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_BLK_ERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018050ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_BLK_ERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_BLK_ERR(a) bdk_ocx_rlkx_blk_err_t
#define bustype_BDK_OCX_RLKX_BLK_ERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_BLK_ERR(a) "OCX_RLKX_BLK_ERR"
#define device_bar_BDK_OCX_RLKX_BLK_ERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_BLK_ERR(a) (a)
#define arguments_BDK_OCX_RLKX_BLK_ERR(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_ecc_ctl
 *
 * OCX Receive ECC Control Registers
 */
union bdk_ocx_rlkx_ecc_ctl
{
    uint64_t u;
    struct bdk_ocx_rlkx_ecc_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t fifo1_flip            : 2;  /**< [ 35: 34](R/W) Test pattern to cause ECC errors in top RX FIFO syndromes. */
        uint64_t fifo0_flip            : 2;  /**< [ 33: 32](R/W) Test pattern to cause ECC errors in bottom RX FIFO syndromes. */
        uint64_t reserved_2_31         : 30;
        uint64_t fifo1_cdis            : 1;  /**< [  1:  1](R/W) ECC correction disable for top RX FIFO RAM. */
        uint64_t fifo0_cdis            : 1;  /**< [  0:  0](R/W) ECC correction disable for bottom RX FIFO RAM. */
#else /* Word 0 - Little Endian */
        uint64_t fifo0_cdis            : 1;  /**< [  0:  0](R/W) ECC correction disable for bottom RX FIFO RAM. */
        uint64_t fifo1_cdis            : 1;  /**< [  1:  1](R/W) ECC correction disable for top RX FIFO RAM. */
        uint64_t reserved_2_31         : 30;
        uint64_t fifo0_flip            : 2;  /**< [ 33: 32](R/W) Test pattern to cause ECC errors in bottom RX FIFO syndromes. */
        uint64_t fifo1_flip            : 2;  /**< [ 35: 34](R/W) Test pattern to cause ECC errors in top RX FIFO syndromes. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_ecc_ctl_s cn; */
};
typedef union bdk_ocx_rlkx_ecc_ctl bdk_ocx_rlkx_ecc_ctl_t;

static inline uint64_t BDK_OCX_RLKX_ECC_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_ECC_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018018ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_ECC_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_ECC_CTL(a) bdk_ocx_rlkx_ecc_ctl_t
#define bustype_BDK_OCX_RLKX_ECC_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_ECC_CTL(a) "OCX_RLKX_ECC_CTL"
#define device_bar_BDK_OCX_RLKX_ECC_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_ECC_CTL(a) (a)
#define arguments_BDK_OCX_RLKX_ECC_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_enables
 *
 * OCX Receive Link Enable Registers
 */
union bdk_ocx_rlkx_enables
{
    uint64_t u;
    struct bdk_ocx_rlkx_enables_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t mcd                   : 1;  /**< [  4:  4](R/W) Master enable for all inbound MCD bits. This bit must be enabled by software. once any
                                                                 trusted-mode validation has occurred and before any [MCD] traffic is generated. [MCD]
                                                                 traffic
                                                                 is typically controlled by the OCX_TLK(0..2)_MCD_CTL register. */
        uint64_t m_req                 : 1;  /**< [  3:  3](R/W/H) Master enable for all inbound memory requests. This bit is typically set at reset but is
                                                                 cleared when operating in trusted-mode and must be enabled by software. */
        uint64_t io_req                : 1;  /**< [  2:  2](R/W/H) Master enable for all inbound I/O requests. This bit is typically set at reset but is
                                                                 cleared when operating in trusted-mode and must be enabled by software. */
        uint64_t fwd                   : 1;  /**< [  1:  1](R/W/H) Master enable for all inbound forward commands. This bit is typically set at reset but is
                                                                 cleared when operating in trusted-mode and must be enabled by software. */
        uint64_t co_proc               : 1;  /**< [  0:  0](R/W/H) Master enable for all inbound coprocessor commands. This bit is typically set at reset but
                                                                 is cleared when operating in trusted-mode and must be enabled by software. */
#else /* Word 0 - Little Endian */
        uint64_t co_proc               : 1;  /**< [  0:  0](R/W/H) Master enable for all inbound coprocessor commands. This bit is typically set at reset but
                                                                 is cleared when operating in trusted-mode and must be enabled by software. */
        uint64_t fwd                   : 1;  /**< [  1:  1](R/W/H) Master enable for all inbound forward commands. This bit is typically set at reset but is
                                                                 cleared when operating in trusted-mode and must be enabled by software. */
        uint64_t io_req                : 1;  /**< [  2:  2](R/W/H) Master enable for all inbound I/O requests. This bit is typically set at reset but is
                                                                 cleared when operating in trusted-mode and must be enabled by software. */
        uint64_t m_req                 : 1;  /**< [  3:  3](R/W/H) Master enable for all inbound memory requests. This bit is typically set at reset but is
                                                                 cleared when operating in trusted-mode and must be enabled by software. */
        uint64_t mcd                   : 1;  /**< [  4:  4](R/W) Master enable for all inbound MCD bits. This bit must be enabled by software. once any
                                                                 trusted-mode validation has occurred and before any [MCD] traffic is generated. [MCD]
                                                                 traffic
                                                                 is typically controlled by the OCX_TLK(0..2)_MCD_CTL register. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_enables_s cn; */
};
typedef union bdk_ocx_rlkx_enables bdk_ocx_rlkx_enables_t;

static inline uint64_t BDK_OCX_RLKX_ENABLES(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_ENABLES(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018000ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_ENABLES", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_ENABLES(a) bdk_ocx_rlkx_enables_t
#define bustype_BDK_OCX_RLKX_ENABLES(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_ENABLES(a) "OCX_RLKX_ENABLES"
#define device_bar_BDK_OCX_RLKX_ENABLES(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_ENABLES(a) (a)
#define arguments_BDK_OCX_RLKX_ENABLES(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_fifo#_cnt
 *
 * OCX Receive Link FIFO Count Registers
 */
union bdk_ocx_rlkx_fifox_cnt
{
    uint64_t u;
    struct bdk_ocx_rlkx_fifox_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) RX FIFO count of 64-bit words to send to core. VC13 traffic is used immediately so the
                                                                 FIFO count is always 0. See OCX_RLK(0..2)_LNK_DATA. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) RX FIFO count of 64-bit words to send to core. VC13 traffic is used immediately so the
                                                                 FIFO count is always 0. See OCX_RLK(0..2)_LNK_DATA. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_fifox_cnt_s cn; */
};
typedef union bdk_ocx_rlkx_fifox_cnt bdk_ocx_rlkx_fifox_cnt_t;

static inline uint64_t BDK_OCX_RLKX_FIFOX_CNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_FIFOX_CNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=13)))
        return 0x87e011018100ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0xf);
    __bdk_csr_fatal("OCX_RLKX_FIFOX_CNT", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_RLKX_FIFOX_CNT(a,b) bdk_ocx_rlkx_fifox_cnt_t
#define bustype_BDK_OCX_RLKX_FIFOX_CNT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_FIFOX_CNT(a,b) "OCX_RLKX_FIFOX_CNT"
#define device_bar_BDK_OCX_RLKX_FIFOX_CNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_FIFOX_CNT(a,b) (a)
#define arguments_BDK_OCX_RLKX_FIFOX_CNT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_rlk#_key_high#
 *
 * OCX Receive Encryption Key Registers
 */
union bdk_ocx_rlkx_key_highx
{
    uint64_t u;
    struct bdk_ocx_rlkx_key_highx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit key data \<127:64\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit key data \<127:64\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_key_highx_s cn; */
};
typedef union bdk_ocx_rlkx_key_highx bdk_ocx_rlkx_key_highx_t;

static inline uint64_t BDK_OCX_RLKX_KEY_HIGHX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_KEY_HIGHX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=2)))
        return 0x87e011018208ll + 0x2000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_KEY_HIGHX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_RLKX_KEY_HIGHX(a,b) bdk_ocx_rlkx_key_highx_t
#define bustype_BDK_OCX_RLKX_KEY_HIGHX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_KEY_HIGHX(a,b) "OCX_RLKX_KEY_HIGHX"
#define device_bar_BDK_OCX_RLKX_KEY_HIGHX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_KEY_HIGHX(a,b) (a)
#define arguments_BDK_OCX_RLKX_KEY_HIGHX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_rlk#_key_low#
 *
 * OCX Receive Encryption Key Registers
 */
union bdk_ocx_rlkx_key_lowx
{
    uint64_t u;
    struct bdk_ocx_rlkx_key_lowx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Receive key data \<63:0\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Receive key data \<63:0\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_key_lowx_s cn; */
};
typedef union bdk_ocx_rlkx_key_lowx bdk_ocx_rlkx_key_lowx_t;

static inline uint64_t BDK_OCX_RLKX_KEY_LOWX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_KEY_LOWX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=2)))
        return 0x87e011018200ll + 0x2000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_KEY_LOWX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_RLKX_KEY_LOWX(a,b) bdk_ocx_rlkx_key_lowx_t
#define bustype_BDK_OCX_RLKX_KEY_LOWX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_KEY_LOWX(a,b) "OCX_RLKX_KEY_LOWX"
#define device_bar_BDK_OCX_RLKX_KEY_LOWX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_KEY_LOWX(a,b) (a)
#define arguments_BDK_OCX_RLKX_KEY_LOWX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_rlk#_lnk_data
 *
 * OCX Receive Link Data Registers
 */
union bdk_ocx_rlkx_lnk_data
{
    uint64_t u;
    struct bdk_ocx_rlkx_lnk_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rcvd                  : 1;  /**< [ 63: 63](RO/H) Reads state of OCX_COM_LINK(0..2)_INT[LNK_DATA]; set by hardware when a link data block is
                                                                 received. */
        uint64_t reserved_56_62        : 7;
        uint64_t data                  : 56; /**< [ 55:  0](RO/H) Contents of this register are received from the OCX_TLK(0..2)_LNK_DATA register on the
                                                                 link partner. Each time a new value is received the RX_LDAT interrupt is generated. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 56; /**< [ 55:  0](RO/H) Contents of this register are received from the OCX_TLK(0..2)_LNK_DATA register on the
                                                                 link partner. Each time a new value is received the RX_LDAT interrupt is generated. */
        uint64_t reserved_56_62        : 7;
        uint64_t rcvd                  : 1;  /**< [ 63: 63](RO/H) Reads state of OCX_COM_LINK(0..2)_INT[LNK_DATA]; set by hardware when a link data block is
                                                                 received. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_lnk_data_s cn; */
};
typedef union bdk_ocx_rlkx_lnk_data bdk_ocx_rlkx_lnk_data_t;

static inline uint64_t BDK_OCX_RLKX_LNK_DATA(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_LNK_DATA(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018028ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_LNK_DATA", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_LNK_DATA(a) bdk_ocx_rlkx_lnk_data_t
#define bustype_BDK_OCX_RLKX_LNK_DATA(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_LNK_DATA(a) "OCX_RLKX_LNK_DATA"
#define device_bar_BDK_OCX_RLKX_LNK_DATA(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_LNK_DATA(a) (a)
#define arguments_BDK_OCX_RLKX_LNK_DATA(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_mcd_ctl
 *
 * OCX Receive MCD Control Registers
 * This debug register captures which new MCD bits have been received from the link partner. The
 * MCD bits are received when the both the OCX_RLK(0..2)_ENABLES[MCD] bit is set and the MCD was
 * not previously transmitted.
 */
union bdk_ocx_rlkx_mcd_ctl
{
    uint64_t u;
    struct bdk_ocx_rlkx_mcd_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t clr                   : 3;  /**< [  2:  0](R/W1C/H) Shows the inbound MCD value being driven by link(0..2). Set by hardware
                                                                 receiving an MCD packet and cleared by this register. */
#else /* Word 0 - Little Endian */
        uint64_t clr                   : 3;  /**< [  2:  0](R/W1C/H) Shows the inbound MCD value being driven by link(0..2). Set by hardware
                                                                 receiving an MCD packet and cleared by this register. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_mcd_ctl_s cn; */
};
typedef union bdk_ocx_rlkx_mcd_ctl bdk_ocx_rlkx_mcd_ctl_t;

static inline uint64_t BDK_OCX_RLKX_MCD_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_MCD_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018020ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_MCD_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_MCD_CTL(a) bdk_ocx_rlkx_mcd_ctl_t
#define bustype_BDK_OCX_RLKX_MCD_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_MCD_CTL(a) "OCX_RLKX_MCD_CTL"
#define device_bar_BDK_OCX_RLKX_MCD_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_MCD_CTL(a) (a)
#define arguments_BDK_OCX_RLKX_MCD_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_protect
 *
 * OCX Receive Data Protection Control Registers
 */
union bdk_ocx_rlkx_protect
{
    uint64_t u;
    struct bdk_ocx_rlkx_protect_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t wo_key                : 1;  /**< [  7:  7](R/W1S) Reserved. Setting this bit blocks read access to OCX_RLK()_KEY_LOW,
                                                                 OCX_RLK()_KEY_HIGH, OCX_RLK()_SALT_LOW and OCX_RLK()_SALT_HIGH registers. */
        uint64_t reserved_1_6          : 6;
        uint64_t enable                : 1;  /**< [  0:  0](RO/H) Data encryption enabled. This bit is set and cleared by the transmitting link
                                                                 partner. */
#else /* Word 0 - Little Endian */
        uint64_t enable                : 1;  /**< [  0:  0](RO/H) Data encryption enabled. This bit is set and cleared by the transmitting link
                                                                 partner. */
        uint64_t reserved_1_6          : 6;
        uint64_t wo_key                : 1;  /**< [  7:  7](R/W1S) Reserved. Setting this bit blocks read access to OCX_RLK()_KEY_LOW,
                                                                 OCX_RLK()_KEY_HIGH, OCX_RLK()_SALT_LOW and OCX_RLK()_SALT_HIGH registers. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_protect_s cn; */
};
typedef union bdk_ocx_rlkx_protect bdk_ocx_rlkx_protect_t;

static inline uint64_t BDK_OCX_RLKX_PROTECT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_PROTECT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e0110182c0ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_PROTECT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_PROTECT(a) bdk_ocx_rlkx_protect_t
#define bustype_BDK_OCX_RLKX_PROTECT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_PROTECT(a) "OCX_RLKX_PROTECT"
#define device_bar_BDK_OCX_RLKX_PROTECT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_PROTECT(a) (a)
#define arguments_BDK_OCX_RLKX_PROTECT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_salt_high
 *
 * OCX Receive Encryption Salt Registers
 */
union bdk_ocx_rlkx_salt_high
{
    uint64_t u;
    struct bdk_ocx_rlkx_salt_high_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Receive salt data \<127:64\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Receive salt data \<127:64\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_salt_high_s cn; */
};
typedef union bdk_ocx_rlkx_salt_high bdk_ocx_rlkx_salt_high_t;

static inline uint64_t BDK_OCX_RLKX_SALT_HIGH(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_SALT_HIGH(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018288ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_SALT_HIGH", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_SALT_HIGH(a) bdk_ocx_rlkx_salt_high_t
#define bustype_BDK_OCX_RLKX_SALT_HIGH(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_SALT_HIGH(a) "OCX_RLKX_SALT_HIGH"
#define device_bar_BDK_OCX_RLKX_SALT_HIGH(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_SALT_HIGH(a) (a)
#define arguments_BDK_OCX_RLKX_SALT_HIGH(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_rlk#_salt_low
 *
 * OCX Receive Encryption Salt Registers
 */
union bdk_ocx_rlkx_salt_low
{
    uint64_t u;
    struct bdk_ocx_rlkx_salt_low_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Receive salt data \<63:0\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Receive salt data \<63:0\>.
                                                                 Reads as zero if OCX_RLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_rlkx_salt_low_s cn; */
};
typedef union bdk_ocx_rlkx_salt_low bdk_ocx_rlkx_salt_low_t;

static inline uint64_t BDK_OCX_RLKX_SALT_LOW(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_RLKX_SALT_LOW(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011018280ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_RLKX_SALT_LOW", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_RLKX_SALT_LOW(a) bdk_ocx_rlkx_salt_low_t
#define bustype_BDK_OCX_RLKX_SALT_LOW(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_RLKX_SALT_LOW(a) "OCX_RLKX_SALT_LOW"
#define device_bar_BDK_OCX_RLKX_SALT_LOW(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_RLKX_SALT_LOW(a) (a)
#define arguments_BDK_OCX_RLKX_SALT_LOW(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_strap
 *
 * OCX Strap Register
 * This register provide read-only access to OCI straps.
 */
union bdk_ocx_strap
{
    uint64_t u;
    struct bdk_ocx_strap_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_26_63        : 38;
        uint64_t oci3_lnk1             : 1;  /**< [ 25: 25](RO) OCI3_LNK1 strap. */
        uint64_t oci2_lnk1             : 1;  /**< [ 24: 24](RO) OCI2_LNK1 strap. */
        uint64_t reserved_17_23        : 7;
        uint64_t oci_fixed_node        : 1;  /**< [ 16: 16](RO) OCI_FIXED_NODE strap. */
        uint64_t reserved_10_15        : 6;
        uint64_t oci_node_id           : 2;  /**< [  9:  8](RO) OCI_NODE_ID\<1:0\> straps. */
        uint64_t reserved_4_7          : 4;
        uint64_t oci_spd               : 4;  /**< [  3:  0](RO) OCI_SPD\<3:0\> straps. */
#else /* Word 0 - Little Endian */
        uint64_t oci_spd               : 4;  /**< [  3:  0](RO) OCI_SPD\<3:0\> straps. */
        uint64_t reserved_4_7          : 4;
        uint64_t oci_node_id           : 2;  /**< [  9:  8](RO) OCI_NODE_ID\<1:0\> straps. */
        uint64_t reserved_10_15        : 6;
        uint64_t oci_fixed_node        : 1;  /**< [ 16: 16](RO) OCI_FIXED_NODE strap. */
        uint64_t reserved_17_23        : 7;
        uint64_t oci2_lnk1             : 1;  /**< [ 24: 24](RO) OCI2_LNK1 strap. */
        uint64_t oci3_lnk1             : 1;  /**< [ 25: 25](RO) OCI3_LNK1 strap. */
        uint64_t reserved_26_63        : 38;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_strap_s cn; */
};
typedef union bdk_ocx_strap bdk_ocx_strap_t;

#define BDK_OCX_STRAP BDK_OCX_STRAP_FUNC()
static inline uint64_t BDK_OCX_STRAP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_STRAP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X))
        return 0x87e01100ff08ll;
    __bdk_csr_fatal("OCX_STRAP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_STRAP bdk_ocx_strap_t
#define bustype_BDK_OCX_STRAP BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_STRAP "OCX_STRAP"
#define device_bar_BDK_OCX_STRAP 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_STRAP 0
#define arguments_BDK_OCX_STRAP -1,-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_bist_status
 *
 * OCX Link REPLAY Memories and TX FIFOs BIST Status Register
 * Contains status from last memory BIST for all TX FIFO memories and REPLAY memories in this
 * link. RX FIFO status can be found in OCX_COM_BIST_STATUS.
 */
union bdk_ocx_tlkx_bist_status
{
    uint64_t u;
    struct bdk_ocx_tlkx_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t status                : 16; /**< [ 15:  0](RO/H) \<15:14\> = REPLAY Memories BIST Status \<1:0\>.
                                                                 \<13:12\> = MOC TX_FIFO BIST Status \<1:0\>.
                                                                 \<11:0\>  = TX_FIFO\<11:0\> by Link VC number. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 16; /**< [ 15:  0](RO/H) \<15:14\> = REPLAY Memories BIST Status \<1:0\>.
                                                                 \<13:12\> = MOC TX_FIFO BIST Status \<1:0\>.
                                                                 \<11:0\>  = TX_FIFO\<11:0\> by Link VC number. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_bist_status_s cn; */
};
typedef union bdk_ocx_tlkx_bist_status bdk_ocx_tlkx_bist_status_t;

static inline uint64_t BDK_OCX_TLKX_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010008ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_BIST_STATUS(a) bdk_ocx_tlkx_bist_status_t
#define bustype_BDK_OCX_TLKX_BIST_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_BIST_STATUS(a) "OCX_TLKX_BIST_STATUS"
#define device_bar_BDK_OCX_TLKX_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_BIST_STATUS(a) (a)
#define arguments_BDK_OCX_TLKX_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_byp_ctl
 *
 * OCX Transmit FIFO Bypass Control Registers
 * This register is for diagnostic use.
 */
union bdk_ocx_tlkx_byp_ctl
{
    uint64_t u;
    struct bdk_ocx_tlkx_byp_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t vc_dis                : 11; /**< [ 11:  1](R/W) VC bypass disable.  When set, the corresponding VC is restricted from using
                                                                 the low latency TX FIFO bypass logic.  This logic is typically disabled for
                                                                 VC0 only.  For diagnostic use only. */
        uint64_t reserved_0            : 1;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0            : 1;
        uint64_t vc_dis                : 11; /**< [ 11:  1](R/W) VC bypass disable.  When set, the corresponding VC is restricted from using
                                                                 the low latency TX FIFO bypass logic.  This logic is typically disabled for
                                                                 VC0 only.  For diagnostic use only. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_byp_ctl_s cn; */
};
typedef union bdk_ocx_tlkx_byp_ctl bdk_ocx_tlkx_byp_ctl_t;

static inline uint64_t BDK_OCX_TLKX_BYP_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_BYP_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X) && (a<=2))
        return 0x87e011010030ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_BYP_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_BYP_CTL(a) bdk_ocx_tlkx_byp_ctl_t
#define bustype_BDK_OCX_TLKX_BYP_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_BYP_CTL(a) "OCX_TLKX_BYP_CTL"
#define device_bar_BDK_OCX_TLKX_BYP_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_BYP_CTL(a) (a)
#define arguments_BDK_OCX_TLKX_BYP_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_ecc_ctl
 *
 * OCX Transmit Link ECC Control Registers
 */
union bdk_ocx_tlkx_ecc_ctl
{
    uint64_t u;
    struct bdk_ocx_tlkx_ecc_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_38_63        : 26;
        uint64_t rply1_flip            : 2;  /**< [ 37: 36](R/W) Test pattern to cause ECC errors in RPLY1 RAM. */
        uint64_t rply0_flip            : 2;  /**< [ 35: 34](R/W) Test pattern to cause ECC errors in RPLY0 RAM. */
        uint64_t fifo_flip             : 2;  /**< [ 33: 32](R/W) Test pattern to cause ECC errors in TX FIFO RAM. */
        uint64_t reserved_3_31         : 29;
        uint64_t rply1_cdis            : 1;  /**< [  2:  2](R/W) ECC correction disable for replay top memories. */
        uint64_t rply0_cdis            : 1;  /**< [  1:  1](R/W) ECC correction disable for replay bottom memories. */
        uint64_t fifo_cdis             : 1;  /**< [  0:  0](R/W) ECC correction disable for TX FIFO memories. */
#else /* Word 0 - Little Endian */
        uint64_t fifo_cdis             : 1;  /**< [  0:  0](R/W) ECC correction disable for TX FIFO memories. */
        uint64_t rply0_cdis            : 1;  /**< [  1:  1](R/W) ECC correction disable for replay bottom memories. */
        uint64_t rply1_cdis            : 1;  /**< [  2:  2](R/W) ECC correction disable for replay top memories. */
        uint64_t reserved_3_31         : 29;
        uint64_t fifo_flip             : 2;  /**< [ 33: 32](R/W) Test pattern to cause ECC errors in TX FIFO RAM. */
        uint64_t rply0_flip            : 2;  /**< [ 35: 34](R/W) Test pattern to cause ECC errors in RPLY0 RAM. */
        uint64_t rply1_flip            : 2;  /**< [ 37: 36](R/W) Test pattern to cause ECC errors in RPLY1 RAM. */
        uint64_t reserved_38_63        : 26;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_ecc_ctl_s cn; */
};
typedef union bdk_ocx_tlkx_ecc_ctl bdk_ocx_tlkx_ecc_ctl_t;

static inline uint64_t BDK_OCX_TLKX_ECC_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_ECC_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010018ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_ECC_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_ECC_CTL(a) bdk_ocx_tlkx_ecc_ctl_t
#define bustype_BDK_OCX_TLKX_ECC_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_ECC_CTL(a) "OCX_TLKX_ECC_CTL"
#define device_bar_BDK_OCX_TLKX_ECC_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_ECC_CTL(a) (a)
#define arguments_BDK_OCX_TLKX_ECC_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_fifo#_cnt
 *
 * OCX Transmit Link FIFO Count Registers
 */
union bdk_ocx_tlkx_fifox_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_fifox_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) TX FIFO count of bus cycles to send. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) TX FIFO count of bus cycles to send. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_fifox_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_fifox_cnt bdk_ocx_tlkx_fifox_cnt_t;

static inline uint64_t BDK_OCX_TLKX_FIFOX_CNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_FIFOX_CNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=13)))
        return 0x87e011010100ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0xf);
    __bdk_csr_fatal("OCX_TLKX_FIFOX_CNT", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_FIFOX_CNT(a,b) bdk_ocx_tlkx_fifox_cnt_t
#define bustype_BDK_OCX_TLKX_FIFOX_CNT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_FIFOX_CNT(a,b) "OCX_TLKX_FIFOX_CNT"
#define device_bar_BDK_OCX_TLKX_FIFOX_CNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_FIFOX_CNT(a,b) (a)
#define arguments_BDK_OCX_TLKX_FIFOX_CNT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_key_high#
 *
 * OCX Transmit Encryption Key Registers
 */
union bdk_ocx_tlkx_key_highx
{
    uint64_t u;
    struct bdk_ocx_tlkx_key_highx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit encryption key \<127:64\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit encryption key \<127:64\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_key_highx_s cn; */
};
typedef union bdk_ocx_tlkx_key_highx bdk_ocx_tlkx_key_highx_t;

static inline uint64_t BDK_OCX_TLKX_KEY_HIGHX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_KEY_HIGHX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=2)))
        return 0x87e011010708ll + 0x2000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_KEY_HIGHX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_KEY_HIGHX(a,b) bdk_ocx_tlkx_key_highx_t
#define bustype_BDK_OCX_TLKX_KEY_HIGHX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_KEY_HIGHX(a,b) "OCX_TLKX_KEY_HIGHX"
#define device_bar_BDK_OCX_TLKX_KEY_HIGHX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_KEY_HIGHX(a,b) (a)
#define arguments_BDK_OCX_TLKX_KEY_HIGHX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_key_low#
 *
 * OCX Transmit Encryption Key Registers
 */
union bdk_ocx_tlkx_key_lowx
{
    uint64_t u;
    struct bdk_ocx_tlkx_key_lowx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit encryption key \<63:0\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit encryption key \<63:0\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_key_lowx_s cn; */
};
typedef union bdk_ocx_tlkx_key_lowx bdk_ocx_tlkx_key_lowx_t;

static inline uint64_t BDK_OCX_TLKX_KEY_LOWX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_KEY_LOWX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=2)))
        return 0x87e011010700ll + 0x2000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_KEY_LOWX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_KEY_LOWX(a,b) bdk_ocx_tlkx_key_lowx_t
#define bustype_BDK_OCX_TLKX_KEY_LOWX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_KEY_LOWX(a,b) "OCX_TLKX_KEY_LOWX"
#define device_bar_BDK_OCX_TLKX_KEY_LOWX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_KEY_LOWX(a,b) (a)
#define arguments_BDK_OCX_TLKX_KEY_LOWX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_lnk_data
 *
 * OCX Transmit Link Data Registers
 */
union bdk_ocx_tlkx_lnk_data
{
    uint64_t u;
    struct bdk_ocx_tlkx_lnk_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_56_63        : 8;
        uint64_t data                  : 56; /**< [ 55:  0](R/W) Writes to this register transfer the contents to the OCX_RLK(0..2)_LNK_DATA register on
                                                                 the receiving link. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 56; /**< [ 55:  0](R/W) Writes to this register transfer the contents to the OCX_RLK(0..2)_LNK_DATA register on
                                                                 the receiving link. */
        uint64_t reserved_56_63        : 8;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_lnk_data_s cn; */
};
typedef union bdk_ocx_tlkx_lnk_data bdk_ocx_tlkx_lnk_data_t;

static inline uint64_t BDK_OCX_TLKX_LNK_DATA(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_LNK_DATA(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010028ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_LNK_DATA", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_LNK_DATA(a) bdk_ocx_tlkx_lnk_data_t
#define bustype_BDK_OCX_TLKX_LNK_DATA(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_LNK_DATA(a) "OCX_TLKX_LNK_DATA"
#define device_bar_BDK_OCX_TLKX_LNK_DATA(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_LNK_DATA(a) (a)
#define arguments_BDK_OCX_TLKX_LNK_DATA(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_lnk_vc#_cnt
 *
 * OCX Transmit Link VC Credits Registers
 */
union bdk_ocx_tlkx_lnk_vcx_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_lnk_vcx_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) Link VC credits available for use. VC13 always reads 1 since credits are not required. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) Link VC credits available for use. VC13 always reads 1 since credits are not required. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_lnk_vcx_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_lnk_vcx_cnt bdk_ocx_tlkx_lnk_vcx_cnt_t;

static inline uint64_t BDK_OCX_TLKX_LNK_VCX_CNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_LNK_VCX_CNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=13)))
        return 0x87e011010200ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0xf);
    __bdk_csr_fatal("OCX_TLKX_LNK_VCX_CNT", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_LNK_VCX_CNT(a,b) bdk_ocx_tlkx_lnk_vcx_cnt_t
#define bustype_BDK_OCX_TLKX_LNK_VCX_CNT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_LNK_VCX_CNT(a,b) "OCX_TLKX_LNK_VCX_CNT"
#define device_bar_BDK_OCX_TLKX_LNK_VCX_CNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_LNK_VCX_CNT(a,b) (a)
#define arguments_BDK_OCX_TLKX_LNK_VCX_CNT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_mcd_ctl
 *
 * OCX Transmit Link MCD Control Registers
 * This register controls which MCD bits are transported via the link. For proper operation
 * only one link must be enabled in both directions between each pair of link partners.
 *
 * Internal:
 * If N chips are connected over OCX, N-1 links should have MCD enabled.
 * A single "central" chip should connect all MCD buses and have a single MCD enabled link
 * to each of the other chips.  No MCD enabled links should connect between chips that don't
 * include the "central" chip.
 */
union bdk_ocx_tlkx_mcd_ctl
{
    uint64_t u;
    struct bdk_ocx_tlkx_mcd_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t tx_enb                : 3;  /**< [  2:  0](R/W) Transmission enable signals for MCD bits \<2:0\>. */
#else /* Word 0 - Little Endian */
        uint64_t tx_enb                : 3;  /**< [  2:  0](R/W) Transmission enable signals for MCD bits \<2:0\>. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_mcd_ctl_s cn; */
};
typedef union bdk_ocx_tlkx_mcd_ctl bdk_ocx_tlkx_mcd_ctl_t;

static inline uint64_t BDK_OCX_TLKX_MCD_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_MCD_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010020ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_MCD_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_MCD_CTL(a) bdk_ocx_tlkx_mcd_ctl_t
#define bustype_BDK_OCX_TLKX_MCD_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_MCD_CTL(a) "OCX_TLKX_MCD_CTL"
#define device_bar_BDK_OCX_TLKX_MCD_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_MCD_CTL(a) (a)
#define arguments_BDK_OCX_TLKX_MCD_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_protect
 *
 * OCX Transmit Data Protection Control Registers
 */
union bdk_ocx_tlkx_protect
{
    uint64_t u;
    struct bdk_ocx_tlkx_protect_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t wo_key                : 1;  /**< [  7:  7](R/W1S) Setting this bit blocks read access to the OCX_TLK(0..2)_KEY and
                                                                 OCX_TLK(0..2)_SALT registers. Once set this bit cannot be cleared until reset. */
        uint64_t reserved_3_6          : 4;
        uint64_t busy                  : 1;  /**< [  2:  2](RO/H) When set, LOAD and/or BUSY signals are being transmitted to the link
                                                                 partner. Hold off any updates to the OCX_TLK()_KEY_LOW, OCX_TLK()_KEY_HIGH,
                                                                 OCX_TLK()_SALT_LOW, OCX_TLK()_SALT_HIGH and OCX_TLK()_PROTECT registers while
                                                                 this bit is set. */
        uint64_t load                  : 1;  /**< [  1:  1](WO) Seting this bit loads the current set of keys written to the
                                                                 OCX_TLK()_KEY_LOW, OCX_TLK()_KEY_HIGH, OCX_TLK()_SALT_LOW, OCX_TLK()_SALT_HIGH
                                                                 and forces the receive side of the link parter to do likewise. */
        uint64_t enable                : 1;  /**< [  0:  0](R/W) Enable data encryption.  When set this bit enables encryption on the
                                                                 transmitter and the receiving link partner.

                                                                 Internal:
                                                                 Encryption is non-functional on pass 1. */
#else /* Word 0 - Little Endian */
        uint64_t enable                : 1;  /**< [  0:  0](R/W) Enable data encryption.  When set this bit enables encryption on the
                                                                 transmitter and the receiving link partner.

                                                                 Internal:
                                                                 Encryption is non-functional on pass 1. */
        uint64_t load                  : 1;  /**< [  1:  1](WO) Seting this bit loads the current set of keys written to the
                                                                 OCX_TLK()_KEY_LOW, OCX_TLK()_KEY_HIGH, OCX_TLK()_SALT_LOW, OCX_TLK()_SALT_HIGH
                                                                 and forces the receive side of the link parter to do likewise. */
        uint64_t busy                  : 1;  /**< [  2:  2](RO/H) When set, LOAD and/or BUSY signals are being transmitted to the link
                                                                 partner. Hold off any updates to the OCX_TLK()_KEY_LOW, OCX_TLK()_KEY_HIGH,
                                                                 OCX_TLK()_SALT_LOW, OCX_TLK()_SALT_HIGH and OCX_TLK()_PROTECT registers while
                                                                 this bit is set. */
        uint64_t reserved_3_6          : 4;
        uint64_t wo_key                : 1;  /**< [  7:  7](R/W1S) Setting this bit blocks read access to the OCX_TLK(0..2)_KEY and
                                                                 OCX_TLK(0..2)_SALT registers. Once set this bit cannot be cleared until reset. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_protect_s cn; */
};
typedef union bdk_ocx_tlkx_protect bdk_ocx_tlkx_protect_t;

static inline uint64_t BDK_OCX_TLKX_PROTECT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_PROTECT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e0110107c0ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_PROTECT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_PROTECT(a) bdk_ocx_tlkx_protect_t
#define bustype_BDK_OCX_TLKX_PROTECT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_PROTECT(a) "OCX_TLKX_PROTECT"
#define device_bar_BDK_OCX_TLKX_PROTECT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_PROTECT(a) (a)
#define arguments_BDK_OCX_TLKX_PROTECT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_rtn_vc#_cnt
 *
 * OCX Transmit Link Return VC Credits Registers
 */
union bdk_ocx_tlkx_rtn_vcx_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_rtn_vcx_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) Link VC credits to return. VC13 always reads 0 since credits are never returned. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 16; /**< [ 15:  0](RO/H) Link VC credits to return. VC13 always reads 0 since credits are never returned. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_rtn_vcx_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_rtn_vcx_cnt bdk_ocx_tlkx_rtn_vcx_cnt_t;

static inline uint64_t BDK_OCX_TLKX_RTN_VCX_CNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_RTN_VCX_CNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=13)))
        return 0x87e011010300ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0xf);
    __bdk_csr_fatal("OCX_TLKX_RTN_VCX_CNT", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_RTN_VCX_CNT(a,b) bdk_ocx_tlkx_rtn_vcx_cnt_t
#define bustype_BDK_OCX_TLKX_RTN_VCX_CNT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_RTN_VCX_CNT(a,b) "OCX_TLKX_RTN_VCX_CNT"
#define device_bar_BDK_OCX_TLKX_RTN_VCX_CNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_RTN_VCX_CNT(a,b) (a)
#define arguments_BDK_OCX_TLKX_RTN_VCX_CNT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_salt_high
 *
 * OCX Transmit Encryption Salt Registers
 */
union bdk_ocx_tlkx_salt_high
{
    uint64_t u;
    struct bdk_ocx_tlkx_salt_high_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit salt data \<127:64\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit salt data \<127:64\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_salt_high_s cn; */
};
typedef union bdk_ocx_tlkx_salt_high bdk_ocx_tlkx_salt_high_t;

static inline uint64_t BDK_OCX_TLKX_SALT_HIGH(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_SALT_HIGH(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010788ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_SALT_HIGH", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_SALT_HIGH(a) bdk_ocx_tlkx_salt_high_t
#define bustype_BDK_OCX_TLKX_SALT_HIGH(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_SALT_HIGH(a) "OCX_TLKX_SALT_HIGH"
#define device_bar_BDK_OCX_TLKX_SALT_HIGH(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_SALT_HIGH(a) (a)
#define arguments_BDK_OCX_TLKX_SALT_HIGH(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_salt_low
 *
 * OCX Transmit Encryption Salt Registers
 */
union bdk_ocx_tlkx_salt_low
{
    uint64_t u;
    struct bdk_ocx_tlkx_salt_low_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit salt data \<63:0\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](WO) Transmit salt data \<63:0\>.
                                                                 Reads as zero if OCX_TLK(0..2)_PROTECT[WO_KEY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_salt_low_s cn; */
};
typedef union bdk_ocx_tlkx_salt_low bdk_ocx_tlkx_salt_low_t;

static inline uint64_t BDK_OCX_TLKX_SALT_LOW(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_SALT_LOW(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010780ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_SALT_LOW", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_SALT_LOW(a) bdk_ocx_tlkx_salt_low_t
#define bustype_BDK_OCX_TLKX_SALT_LOW(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_SALT_LOW(a) "OCX_TLKX_SALT_LOW"
#define device_bar_BDK_OCX_TLKX_SALT_LOW(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_SALT_LOW(a) (a)
#define arguments_BDK_OCX_TLKX_SALT_LOW(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_ctl
 *
 * OCX Transmit Link Statistics Control Registers
 */
union bdk_ocx_tlkx_stat_ctl
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t clear                 : 1;  /**< [  1:  1](WO) Setting this bit clears all OCX_TLK(a)_STAT_*CNT, OCX_TLK(a)_STAT_*CMD,
                                                                 OCX_TLK(a)_STAT_*PKT and OCX_TLK(0..2)_STAT_*CON registers. */
        uint64_t enable                : 1;  /**< [  0:  0](R/W) This bit controls the capture of statistics to the OCX_TLK(a)_STAT_*CNT,
                                                                 OCX_TLK(a)_STAT_*CMD, OCX_TLK(a)_STAT_*PKT and OCX_TLK(a)_STAT_*CON registers. When set,
                                                                 traffic increments the corresponding registers. When cleared, traffic is ignored. */
#else /* Word 0 - Little Endian */
        uint64_t enable                : 1;  /**< [  0:  0](R/W) This bit controls the capture of statistics to the OCX_TLK(a)_STAT_*CNT,
                                                                 OCX_TLK(a)_STAT_*CMD, OCX_TLK(a)_STAT_*PKT and OCX_TLK(a)_STAT_*CON registers. When set,
                                                                 traffic increments the corresponding registers. When cleared, traffic is ignored. */
        uint64_t clear                 : 1;  /**< [  1:  1](WO) Setting this bit clears all OCX_TLK(a)_STAT_*CNT, OCX_TLK(a)_STAT_*CMD,
                                                                 OCX_TLK(a)_STAT_*PKT and OCX_TLK(0..2)_STAT_*CON registers. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_ctl_s cn; */
};
typedef union bdk_ocx_tlkx_stat_ctl bdk_ocx_tlkx_stat_ctl_t;

static inline uint64_t BDK_OCX_TLKX_STAT_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010040ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_CTL(a) bdk_ocx_tlkx_stat_ctl_t
#define bustype_BDK_OCX_TLKX_STAT_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_CTL(a) "OCX_TLKX_STAT_CTL"
#define device_bar_BDK_OCX_TLKX_STAT_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_CTL(a) (a)
#define arguments_BDK_OCX_TLKX_STAT_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_data_cnt
 *
 * OCX Transmit Link Statistics Data Count Registers
 */
union bdk_ocx_tlkx_stat_data_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_data_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Indicates the number of data blocks transferred over the CCPI link while
                                                                 OCX_TLK()_STAT_CTL[ENABLE] has been set. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Indicates the number of data blocks transferred over the CCPI link while
                                                                 OCX_TLK()_STAT_CTL[ENABLE] has been set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_data_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_stat_data_cnt bdk_ocx_tlkx_stat_data_cnt_t;

static inline uint64_t BDK_OCX_TLKX_STAT_DATA_CNT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_DATA_CNT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010408ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_DATA_CNT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_DATA_CNT(a) bdk_ocx_tlkx_stat_data_cnt_t
#define bustype_BDK_OCX_TLKX_STAT_DATA_CNT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_DATA_CNT(a) "OCX_TLKX_STAT_DATA_CNT"
#define device_bar_BDK_OCX_TLKX_STAT_DATA_CNT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_DATA_CNT(a) (a)
#define arguments_BDK_OCX_TLKX_STAT_DATA_CNT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_err_cnt
 *
 * OCX Transmit Link Statistics Error Count Registers
 */
union bdk_ocx_tlkx_stat_err_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_err_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of blocks received with an error over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of blocks received with an error over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_err_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_stat_err_cnt bdk_ocx_tlkx_stat_err_cnt_t;

static inline uint64_t BDK_OCX_TLKX_STAT_ERR_CNT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_ERR_CNT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010420ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_ERR_CNT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_ERR_CNT(a) bdk_ocx_tlkx_stat_err_cnt_t
#define bustype_BDK_OCX_TLKX_STAT_ERR_CNT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_ERR_CNT(a) "OCX_TLKX_STAT_ERR_CNT"
#define device_bar_BDK_OCX_TLKX_STAT_ERR_CNT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_ERR_CNT(a) (a)
#define arguments_BDK_OCX_TLKX_STAT_ERR_CNT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_idle_cnt
 *
 * OCX Transmit Link Statistics Idle Count Registers
 */
union bdk_ocx_tlkx_stat_idle_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_idle_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of idle blocks transferred over the CCPI link while OCX_TLK(0..2)_STAT_CTL[ENABLE]
                                                                 has been set. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of idle blocks transferred over the CCPI link while OCX_TLK(0..2)_STAT_CTL[ENABLE]
                                                                 has been set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_idle_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_stat_idle_cnt bdk_ocx_tlkx_stat_idle_cnt_t;

static inline uint64_t BDK_OCX_TLKX_STAT_IDLE_CNT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_IDLE_CNT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010400ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_IDLE_CNT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_IDLE_CNT(a) bdk_ocx_tlkx_stat_idle_cnt_t
#define bustype_BDK_OCX_TLKX_STAT_IDLE_CNT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_IDLE_CNT(a) "OCX_TLKX_STAT_IDLE_CNT"
#define device_bar_BDK_OCX_TLKX_STAT_IDLE_CNT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_IDLE_CNT(a) (a)
#define arguments_BDK_OCX_TLKX_STAT_IDLE_CNT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_mat#_cnt
 *
 * OCX Transmit Link Statistics Match Count Registers
 */
union bdk_ocx_tlkx_stat_matx_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_matx_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of packets that have matched OCX_TLK(a)_STAT_MATCH0 and have been transferred over
                                                                 the CCPI link while OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of packets that have matched OCX_TLK(a)_STAT_MATCH0 and have been transferred over
                                                                 the CCPI link while OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_matx_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_stat_matx_cnt bdk_ocx_tlkx_stat_matx_cnt_t;

static inline uint64_t BDK_OCX_TLKX_STAT_MATX_CNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_MATX_CNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=3)))
        return 0x87e011010440ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_MATX_CNT", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_MATX_CNT(a,b) bdk_ocx_tlkx_stat_matx_cnt_t
#define bustype_BDK_OCX_TLKX_STAT_MATX_CNT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_MATX_CNT(a,b) "OCX_TLKX_STAT_MATX_CNT"
#define device_bar_BDK_OCX_TLKX_STAT_MATX_CNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_MATX_CNT(a,b) (a)
#define arguments_BDK_OCX_TLKX_STAT_MATX_CNT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_match#
 *
 * OCX Transmit Link Statistics Match Registers
 */
union bdk_ocx_tlkx_stat_matchx
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_matchx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_25_63        : 39;
        uint64_t mask                  : 9;  /**< [ 24: 16](R/W) Setting these bits mask (really matches) the corresponding bit comparison for each packet. */
        uint64_t reserved_9_15         : 7;
        uint64_t cmd                   : 5;  /**< [  8:  4](R/W) These bits are compared against the command for each packet sent over the link. If both
                                                                 the unmasked [VC] and [CMD] bits match then OCX_TLK(0..2)_STAT_MAT(0..3)_CNT is
                                                                 incremented. */
        uint64_t vc                    : 4;  /**< [  3:  0](R/W) These bits are compared against the link VC number for each packet sent over the link.
                                                                 If both the unmasked [VC] and [CMD] bits match, then OCX_TLK(0..2)_STAT_MAT(0..3)_CNT is
                                                                 incremented.  Only memory and I/O traffic are monitored.  Matches are limited to
                                                                 VC0 through VC11. */
#else /* Word 0 - Little Endian */
        uint64_t vc                    : 4;  /**< [  3:  0](R/W) These bits are compared against the link VC number for each packet sent over the link.
                                                                 If both the unmasked [VC] and [CMD] bits match, then OCX_TLK(0..2)_STAT_MAT(0..3)_CNT is
                                                                 incremented.  Only memory and I/O traffic are monitored.  Matches are limited to
                                                                 VC0 through VC11. */
        uint64_t cmd                   : 5;  /**< [  8:  4](R/W) These bits are compared against the command for each packet sent over the link. If both
                                                                 the unmasked [VC] and [CMD] bits match then OCX_TLK(0..2)_STAT_MAT(0..3)_CNT is
                                                                 incremented. */
        uint64_t reserved_9_15         : 7;
        uint64_t mask                  : 9;  /**< [ 24: 16](R/W) Setting these bits mask (really matches) the corresponding bit comparison for each packet. */
        uint64_t reserved_25_63        : 39;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_matchx_s cn; */
};
typedef union bdk_ocx_tlkx_stat_matchx bdk_ocx_tlkx_stat_matchx_t;

static inline uint64_t BDK_OCX_TLKX_STAT_MATCHX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_MATCHX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=3)))
        return 0x87e011010080ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_MATCHX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_MATCHX(a,b) bdk_ocx_tlkx_stat_matchx_t
#define bustype_BDK_OCX_TLKX_STAT_MATCHX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_MATCHX(a,b) "OCX_TLKX_STAT_MATCHX"
#define device_bar_BDK_OCX_TLKX_STAT_MATCHX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_MATCHX(a,b) (a)
#define arguments_BDK_OCX_TLKX_STAT_MATCHX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_retry_cnt
 *
 * OCX Transmit Link Statistics Retry Count Registers
 */
union bdk_ocx_tlkx_stat_retry_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_retry_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Specifies the number of data blocks repeated over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has
                                                                 been set. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Specifies the number of data blocks repeated over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has
                                                                 been set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_retry_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_stat_retry_cnt bdk_ocx_tlkx_stat_retry_cnt_t;

static inline uint64_t BDK_OCX_TLKX_STAT_RETRY_CNT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_RETRY_CNT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010418ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_RETRY_CNT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_RETRY_CNT(a) bdk_ocx_tlkx_stat_retry_cnt_t
#define bustype_BDK_OCX_TLKX_STAT_RETRY_CNT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_RETRY_CNT(a) "OCX_TLKX_STAT_RETRY_CNT"
#define device_bar_BDK_OCX_TLKX_STAT_RETRY_CNT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_RETRY_CNT(a) (a)
#define arguments_BDK_OCX_TLKX_STAT_RETRY_CNT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_sync_cnt
 *
 * OCX Transmit Link Statistics Sync Count Registers
 */
union bdk_ocx_tlkx_stat_sync_cnt
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_sync_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Indicates the number of sync (control) blocks transferred over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Indicates the number of sync (control) blocks transferred over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_sync_cnt_s cn; */
};
typedef union bdk_ocx_tlkx_stat_sync_cnt bdk_ocx_tlkx_stat_sync_cnt_t;

static inline uint64_t BDK_OCX_TLKX_STAT_SYNC_CNT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_SYNC_CNT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010410ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STAT_SYNC_CNT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_SYNC_CNT(a) bdk_ocx_tlkx_stat_sync_cnt_t
#define bustype_BDK_OCX_TLKX_STAT_SYNC_CNT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_SYNC_CNT(a) "OCX_TLKX_STAT_SYNC_CNT"
#define device_bar_BDK_OCX_TLKX_STAT_SYNC_CNT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_SYNC_CNT(a) (a)
#define arguments_BDK_OCX_TLKX_STAT_SYNC_CNT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_vc#_cmd
 *
 * OCX Transmit Link Statistics VC Commands Count Registers
 */
union bdk_ocx_tlkx_stat_vcx_cmd
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_vcx_cmd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of commands on this VC that have been transfered over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. For VCs 6 through 13 the number of commands
                                                                 is equal to the number of packets. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of commands on this VC that have been transfered over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. For VCs 6 through 13 the number of commands
                                                                 is equal to the number of packets. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_vcx_cmd_s cn; */
};
typedef union bdk_ocx_tlkx_stat_vcx_cmd bdk_ocx_tlkx_stat_vcx_cmd_t;

static inline uint64_t BDK_OCX_TLKX_STAT_VCX_CMD(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_VCX_CMD(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=5)))
        return 0x87e011010480ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0x7);
    __bdk_csr_fatal("OCX_TLKX_STAT_VCX_CMD", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_VCX_CMD(a,b) bdk_ocx_tlkx_stat_vcx_cmd_t
#define bustype_BDK_OCX_TLKX_STAT_VCX_CMD(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_VCX_CMD(a,b) "OCX_TLKX_STAT_VCX_CMD"
#define device_bar_BDK_OCX_TLKX_STAT_VCX_CMD(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_VCX_CMD(a,b) (a)
#define arguments_BDK_OCX_TLKX_STAT_VCX_CMD(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_vc#_con
 *
 * OCX Transmit Link Statistics VC Conflict Count Registers
 */
union bdk_ocx_tlkx_stat_vcx_con
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_vcx_con_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of conflicts on this VC while OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. A
                                                                 conflict is indicated when a VC has one or more packets to send and no link credits are
                                                                 available. VC13 does not require credits so no conflicts are ever indicated (i.e. reads
                                                                 0). */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of conflicts on this VC while OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. A
                                                                 conflict is indicated when a VC has one or more packets to send and no link credits are
                                                                 available. VC13 does not require credits so no conflicts are ever indicated (i.e. reads
                                                                 0). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_vcx_con_s cn; */
};
typedef union bdk_ocx_tlkx_stat_vcx_con bdk_ocx_tlkx_stat_vcx_con_t;

static inline uint64_t BDK_OCX_TLKX_STAT_VCX_CON(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_VCX_CON(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=13)))
        return 0x87e011010580ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0xf);
    __bdk_csr_fatal("OCX_TLKX_STAT_VCX_CON", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_VCX_CON(a,b) bdk_ocx_tlkx_stat_vcx_con_t
#define bustype_BDK_OCX_TLKX_STAT_VCX_CON(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_VCX_CON(a,b) "OCX_TLKX_STAT_VCX_CON"
#define device_bar_BDK_OCX_TLKX_STAT_VCX_CON(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_VCX_CON(a,b) (a)
#define arguments_BDK_OCX_TLKX_STAT_VCX_CON(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_stat_vc#_pkt
 *
 * OCX Transmit Link Statistics VC Packet Count Registers
 */
union bdk_ocx_tlkx_stat_vcx_pkt
{
    uint64_t u;
    struct bdk_ocx_tlkx_stat_vcx_pkt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of packets on this VC that have been transferred over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W) Number of packets on this VC that have been transferred over the CCPI link while
                                                                 OCX_TLK(0..2)_STAT_CTL[ENABLE] has been set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_stat_vcx_pkt_s cn; */
};
typedef union bdk_ocx_tlkx_stat_vcx_pkt bdk_ocx_tlkx_stat_vcx_pkt_t;

static inline uint64_t BDK_OCX_TLKX_STAT_VCX_PKT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STAT_VCX_PKT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=13)))
        return 0x87e011010500ll + 0x2000ll * ((a) & 0x3) + 8ll * ((b) & 0xf);
    __bdk_csr_fatal("OCX_TLKX_STAT_VCX_PKT", 2, a, b, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STAT_VCX_PKT(a,b) bdk_ocx_tlkx_stat_vcx_pkt_t
#define bustype_BDK_OCX_TLKX_STAT_VCX_PKT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STAT_VCX_PKT(a,b) "OCX_TLKX_STAT_VCX_PKT"
#define device_bar_BDK_OCX_TLKX_STAT_VCX_PKT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STAT_VCX_PKT(a,b) (a)
#define arguments_BDK_OCX_TLKX_STAT_VCX_PKT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocx_tlk#_status
 *
 * OCX Transmit Link Status Registers
 */
union bdk_ocx_tlkx_status
{
    uint64_t u;
    struct bdk_ocx_tlkx_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_56_63        : 8;
        uint64_t rply_fptr             : 8;  /**< [ 55: 48](RO/H) Replay buffer last free pointer. */
        uint64_t tx_seq                : 8;  /**< [ 47: 40](RO/H) Last block transmitted. */
        uint64_t rx_seq                : 8;  /**< [ 39: 32](RO/H) Last block received. */
        uint64_t reserved_23_31        : 9;
        uint64_t ackcnt                : 7;  /**< [ 22: 16](RO/H) Indicates the number of ACKs waiting to be transmitted. */
        uint64_t reserved_9_15         : 7;
        uint64_t drop                  : 1;  /**< [  8:  8](RO/H) Link is dropping all requests. */
        uint64_t sm                    : 6;  /**< [  7:  2](RO/H) Block state machine:
                                                                 Bit\<2\>: Req / Ack (Init or retry only).
                                                                 Bit\<3\>: Init.
                                                                 Bit\<4\>: Run.
                                                                 Bit\<5\>: Retry.
                                                                 Bit\<6\>: Replay.
                                                                 Bit\<7\>: Replay Pending. */
        uint64_t cnt                   : 2;  /**< [  1:  0](RO/H) Block subcount. Should always increment 0,1,2,3,0.. except during TX PHY stall. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 2;  /**< [  1:  0](RO/H) Block subcount. Should always increment 0,1,2,3,0.. except during TX PHY stall. */
        uint64_t sm                    : 6;  /**< [  7:  2](RO/H) Block state machine:
                                                                 Bit\<2\>: Req / Ack (Init or retry only).
                                                                 Bit\<3\>: Init.
                                                                 Bit\<4\>: Run.
                                                                 Bit\<5\>: Retry.
                                                                 Bit\<6\>: Replay.
                                                                 Bit\<7\>: Replay Pending. */
        uint64_t drop                  : 1;  /**< [  8:  8](RO/H) Link is dropping all requests. */
        uint64_t reserved_9_15         : 7;
        uint64_t ackcnt                : 7;  /**< [ 22: 16](RO/H) Indicates the number of ACKs waiting to be transmitted. */
        uint64_t reserved_23_31        : 9;
        uint64_t rx_seq                : 8;  /**< [ 39: 32](RO/H) Last block received. */
        uint64_t tx_seq                : 8;  /**< [ 47: 40](RO/H) Last block transmitted. */
        uint64_t rply_fptr             : 8;  /**< [ 55: 48](RO/H) Replay buffer last free pointer. */
        uint64_t reserved_56_63        : 8;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_tlkx_status_s cn; */
};
typedef union bdk_ocx_tlkx_status bdk_ocx_tlkx_status_t;

static inline uint64_t BDK_OCX_TLKX_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_TLKX_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e011010000ll + 0x2000ll * ((a) & 0x3);
    __bdk_csr_fatal("OCX_TLKX_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCX_TLKX_STATUS(a) bdk_ocx_tlkx_status_t
#define bustype_BDK_OCX_TLKX_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_TLKX_STATUS(a) "OCX_TLKX_STATUS"
#define device_bar_BDK_OCX_TLKX_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_TLKX_STATUS(a) (a)
#define arguments_BDK_OCX_TLKX_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) ocx_win_cmd
 *
 * OCX Window Address Register
 * For diagnostic use only. This register is typically written by hardware after accesses to the
 * SLI_WIN_* registers. Contains the address, read size and write mask to used for the window
 * operation. Write data should be written first and placed in the OCX_WIN_WR_DATA register.
 * Writing this register starts the operation. A second write operation to this register while an
 * operation
 * is in progress will stall.
 */
union bdk_ocx_win_cmd
{
    uint64_t u;
    struct bdk_ocx_win_cmd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wr_mask               : 8;  /**< [ 63: 56](R/W) Mask for the data to be written. When a bit is 1, the corresponding byte will be written.
                                                                 The values of this field must be contiguous and for 1, 2, 4, or 8 byte operations and
                                                                 aligned to operation size. A value of 0 will produce unpredictable results. Field is
                                                                 ignored during a read (LD_OP=1). */
        uint64_t reserved_54_55        : 2;
        uint64_t el                    : 2;  /**< [ 53: 52](R/W) Execution level.  This field is used to supply the execution level of the generated load
                                                                 or store command. */
        uint64_t nsecure               : 1;  /**< [ 51: 51](R/W) Nonsecure mode.  Setting this bit causes the generated load or store command to be
                                                                 considered nonsecure. */
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command sent with the read:
                                                                 0x0 = Load 1-bytes.
                                                                 0x1 = Load 2-bytes.
                                                                 0x2 = Load 4-bytes.
                                                                 0x3 = Load 8-bytes. */
        uint64_t ld_op                 : 1;  /**< [ 48: 48](R/W) Operation type:
                                                                 0 = Store.
                                                                 1 = Load operation. */
        uint64_t addr                  : 48; /**< [ 47:  0](R/W) The address used in both the load and store operations:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = NCB_ID.
                                                                 \<35:0\>  = Address.

                                                                 When \<43:36\> NCB_ID is RSL (0x7E) address field is defined as:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = 0x7E.
                                                                 \<35:32\> = Reserved.
                                                                 \<31:24\> = RSL_ID.
                                                                 \<23:0\>  = RSL register offset.

                                                                 \<2:0\> are ignored in a store operation. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 48; /**< [ 47:  0](R/W) The address used in both the load and store operations:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = NCB_ID.
                                                                 \<35:0\>  = Address.

                                                                 When \<43:36\> NCB_ID is RSL (0x7E) address field is defined as:
                                                                 \<47:46\> = Reserved.
                                                                 \<45:44\> = CCPI_ID.
                                                                 \<43:36\> = 0x7E.
                                                                 \<35:32\> = Reserved.
                                                                 \<31:24\> = RSL_ID.
                                                                 \<23:0\>  = RSL register offset.

                                                                 \<2:0\> are ignored in a store operation. */
        uint64_t ld_op                 : 1;  /**< [ 48: 48](R/W) Operation type:
                                                                 0 = Store.
                                                                 1 = Load operation. */
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command sent with the read:
                                                                 0x0 = Load 1-bytes.
                                                                 0x1 = Load 2-bytes.
                                                                 0x2 = Load 4-bytes.
                                                                 0x3 = Load 8-bytes. */
        uint64_t nsecure               : 1;  /**< [ 51: 51](R/W) Nonsecure mode.  Setting this bit causes the generated load or store command to be
                                                                 considered nonsecure. */
        uint64_t el                    : 2;  /**< [ 53: 52](R/W) Execution level.  This field is used to supply the execution level of the generated load
                                                                 or store command. */
        uint64_t reserved_54_55        : 2;
        uint64_t wr_mask               : 8;  /**< [ 63: 56](R/W) Mask for the data to be written. When a bit is 1, the corresponding byte will be written.
                                                                 The values of this field must be contiguous and for 1, 2, 4, or 8 byte operations and
                                                                 aligned to operation size. A value of 0 will produce unpredictable results. Field is
                                                                 ignored during a read (LD_OP=1). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_win_cmd_s cn; */
};
typedef union bdk_ocx_win_cmd bdk_ocx_win_cmd_t;

#define BDK_OCX_WIN_CMD BDK_OCX_WIN_CMD_FUNC()
static inline uint64_t BDK_OCX_WIN_CMD_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_WIN_CMD_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000048ll;
    __bdk_csr_fatal("OCX_WIN_CMD", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_WIN_CMD bdk_ocx_win_cmd_t
#define bustype_BDK_OCX_WIN_CMD BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_WIN_CMD "OCX_WIN_CMD"
#define device_bar_BDK_OCX_WIN_CMD 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_WIN_CMD 0
#define arguments_BDK_OCX_WIN_CMD -1,-1,-1,-1

/**
 * Register (RSL) ocx_win_rd_data
 *
 * OCX Window Read Data Register
 * For diagnostic use only. This register is the read response data associated with window
 * command. Reads all-ones until response is received.
 */
union bdk_ocx_win_rd_data
{
    uint64_t u;
    struct bdk_ocx_win_rd_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](RO/H) Read response data. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](RO/H) Read response data. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_win_rd_data_s cn; */
};
typedef union bdk_ocx_win_rd_data bdk_ocx_win_rd_data_t;

#define BDK_OCX_WIN_RD_DATA BDK_OCX_WIN_RD_DATA_FUNC()
static inline uint64_t BDK_OCX_WIN_RD_DATA_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_WIN_RD_DATA_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000050ll;
    __bdk_csr_fatal("OCX_WIN_RD_DATA", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_WIN_RD_DATA bdk_ocx_win_rd_data_t
#define bustype_BDK_OCX_WIN_RD_DATA BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_WIN_RD_DATA "OCX_WIN_RD_DATA"
#define device_bar_BDK_OCX_WIN_RD_DATA 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_WIN_RD_DATA 0
#define arguments_BDK_OCX_WIN_RD_DATA -1,-1,-1,-1

/**
 * Register (RSL) ocx_win_timer
 *
 * OCX Window Timer Register
 * Number of core clocks before untransmitted WIN request is dropped and interrupt is issued.
 */
union bdk_ocx_win_timer
{
    uint64_t u;
    struct bdk_ocx_win_timer_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t tout                  : 14; /**< [ 15:  2](R/W) Number of core clocks times four. */
        uint64_t tout1                 : 2;  /**< [  1:  0](RO) Reserved as all-ones. */
#else /* Word 0 - Little Endian */
        uint64_t tout1                 : 2;  /**< [  1:  0](RO) Reserved as all-ones. */
        uint64_t tout                  : 14; /**< [ 15:  2](R/W) Number of core clocks times four. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_win_timer_s cn; */
};
typedef union bdk_ocx_win_timer bdk_ocx_win_timer_t;

#define BDK_OCX_WIN_TIMER BDK_OCX_WIN_TIMER_FUNC()
static inline uint64_t BDK_OCX_WIN_TIMER_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_WIN_TIMER_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000058ll;
    __bdk_csr_fatal("OCX_WIN_TIMER", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_WIN_TIMER bdk_ocx_win_timer_t
#define bustype_BDK_OCX_WIN_TIMER BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_WIN_TIMER "OCX_WIN_TIMER"
#define device_bar_BDK_OCX_WIN_TIMER 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_WIN_TIMER 0
#define arguments_BDK_OCX_WIN_TIMER -1,-1,-1,-1

/**
 * Register (RSL) ocx_win_wr_data
 *
 * OCX Window Write Data Register
 * For diagnostic use only. This register is typically written by hardware after accesses to the
 * SLI_WIN_WR_DATA register. Contains the data to write to the address located in OCX_WIN_CMD.
 */
union bdk_ocx_win_wr_data
{
    uint64_t u;
    struct bdk_ocx_win_wr_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#else /* Word 0 - Little Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocx_win_wr_data_s cn; */
};
typedef union bdk_ocx_win_wr_data bdk_ocx_win_wr_data_t;

#define BDK_OCX_WIN_WR_DATA BDK_OCX_WIN_WR_DATA_FUNC()
static inline uint64_t BDK_OCX_WIN_WR_DATA_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCX_WIN_WR_DATA_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e011000040ll;
    __bdk_csr_fatal("OCX_WIN_WR_DATA", 0, 0, 0, 0, 0);
}

#define typedef_BDK_OCX_WIN_WR_DATA bdk_ocx_win_wr_data_t
#define bustype_BDK_OCX_WIN_WR_DATA BDK_CSR_TYPE_RSL
#define basename_BDK_OCX_WIN_WR_DATA "OCX_WIN_WR_DATA"
#define device_bar_BDK_OCX_WIN_WR_DATA 0x0 /* PF_BAR0 */
#define busnum_BDK_OCX_WIN_WR_DATA 0
#define arguments_BDK_OCX_WIN_WR_DATA -1,-1,-1,-1

#endif /* __BDK_CSRS_OCX_H__ */
