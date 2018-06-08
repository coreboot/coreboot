#ifndef __BDK_CSRS_MIO_BOOT_H__
#define __BDK_CSRS_MIO_BOOT_H__
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
 * Cavium MIO_BOOT.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration mio_boot_bar_e
 *
 * MIO Boot Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_MIO_BOOT_BAR_E_MIO_BOOT_PF_BAR0 (0x87e000000000ll)
#define BDK_MIO_BOOT_BAR_E_MIO_BOOT_PF_BAR0_SIZE 0x800000ull

/**
 * Register (RSL) mio_boot_ap_jump
 *
 * MIO Boot AP Jump Address Register
 */
union bdk_mio_boot_ap_jump
{
    uint64_t u;
    struct bdk_mio_boot_ap_jump_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t addr                  : 64; /**< [ 63:  0](SR/W) Boot address.  This register contains the address the internal boot loader
                                                                 will jump to after reset. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 64; /**< [ 63:  0](SR/W) Boot address.  This register contains the address the internal boot loader
                                                                 will jump to after reset. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_boot_ap_jump_s cn; */
};
typedef union bdk_mio_boot_ap_jump bdk_mio_boot_ap_jump_t;

#define BDK_MIO_BOOT_AP_JUMP BDK_MIO_BOOT_AP_JUMP_FUNC()
static inline uint64_t BDK_MIO_BOOT_AP_JUMP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_BOOT_AP_JUMP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0000000d0ll;
    __bdk_csr_fatal("MIO_BOOT_AP_JUMP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_MIO_BOOT_AP_JUMP bdk_mio_boot_ap_jump_t
#define bustype_BDK_MIO_BOOT_AP_JUMP BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_BOOT_AP_JUMP "MIO_BOOT_AP_JUMP"
#define device_bar_BDK_MIO_BOOT_AP_JUMP 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_BOOT_AP_JUMP 0
#define arguments_BDK_MIO_BOOT_AP_JUMP -1,-1,-1,-1

/**
 * Register (RSL) mio_boot_bist_stat
 *
 * MIO Boot BIST Status Register
 * The boot BIST status register contains the BIST status for the MIO boot memories: 0 = pass, 1
 * = fail.
 */
union bdk_mio_boot_bist_stat
{
    uint64_t u;
    struct bdk_mio_boot_bist_stat_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t stat                  : 16; /**< [ 15:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<0\> = mio.mio_boot.mio_boot_fifo.mem.
                                                                 \<1\> = mio.mio_boot.mio_boot_rsp.mem.
                                                                 \<2\> = mio.mio_boot.mio_boot_dma.mem.
                                                                 \<3\> = mio.mio_nbt.mio_nbt_fifo.mem.
                                                                 \<4\> = mio.mio_emm.bufs.mem.mem.
                                                                 \<5\> = mio.mio_nbr.ncbo_crd_fif_mem0.
                                                                 \<6\> = mio.csr.csr_fifo.
                                                                 \<7\> = mio_boot_rom/mio_boot_rom1 (internal bootroms).
                                                                 \<8\> = mio.mio_rsl.mio_pcc.gpi_pcc.
                                                                 \<9\> = mio.mio_pbus.mio_pbus_fifo.mem.
                                                                 \<10\> = mio.mio_pbus.mio_pbus_rsp.mem.
                                                                 \<11\> = mio.mio_ndf.mio_ndf_bufs.ndf_buf.
                                                                 \<12\> = mio.mio_ndf.ndf_ncb_rcv_fif.
                                                                 \<13\> = mio.mio_pcm.mio_pcm_ctl.dmamem.
                                                                 \<14\> = mio.mio_pcm.mio_pcm_ctl.maskmem.
                                                                 \<15\> = mio.mio_nbt.mio_nbt_pfifo.mem. */
#else /* Word 0 - Little Endian */
        uint64_t stat                  : 16; /**< [ 15:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<0\> = mio.mio_boot.mio_boot_fifo.mem.
                                                                 \<1\> = mio.mio_boot.mio_boot_rsp.mem.
                                                                 \<2\> = mio.mio_boot.mio_boot_dma.mem.
                                                                 \<3\> = mio.mio_nbt.mio_nbt_fifo.mem.
                                                                 \<4\> = mio.mio_emm.bufs.mem.mem.
                                                                 \<5\> = mio.mio_nbr.ncbo_crd_fif_mem0.
                                                                 \<6\> = mio.csr.csr_fifo.
                                                                 \<7\> = mio_boot_rom/mio_boot_rom1 (internal bootroms).
                                                                 \<8\> = mio.mio_rsl.mio_pcc.gpi_pcc.
                                                                 \<9\> = mio.mio_pbus.mio_pbus_fifo.mem.
                                                                 \<10\> = mio.mio_pbus.mio_pbus_rsp.mem.
                                                                 \<11\> = mio.mio_ndf.mio_ndf_bufs.ndf_buf.
                                                                 \<12\> = mio.mio_ndf.ndf_ncb_rcv_fif.
                                                                 \<13\> = mio.mio_pcm.mio_pcm_ctl.dmamem.
                                                                 \<14\> = mio.mio_pcm.mio_pcm_ctl.maskmem.
                                                                 \<15\> = mio.mio_nbt.mio_nbt_pfifo.mem. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_boot_bist_stat_s cn81xx; */
    struct bdk_mio_boot_bist_stat_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t stat                  : 9;  /**< [  8:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<0\> = mio.mio_boot.mio_boot_fifo.mem.
                                                                   \<1\> = mio.mio_boot.mio_boot_rsp.mem.
                                                                   \<2\> = mio.mio_boot.mio_boot_dma.mem.
                                                                   \<3\> = mio.mio_nbt.mio_nbt_fifo.mem.
                                                                   \<4\> = mio.mio_emm.bufs.mem.mem.
                                                                   \<5\> = mio.mio_nbr.ncbo_crd_fif_mem0.
                                                                   \<6\> = mio.csr.csr_fifo.
                                                                   \<7\> = mio_boot_rom/mio_boot_rom1 (internal bootroms).
                                                                   \<8\> = mio.mio_rsl.mio_pcc.gpi_pcc. */
#else /* Word 0 - Little Endian */
        uint64_t stat                  : 9;  /**< [  8:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<0\> = mio.mio_boot.mio_boot_fifo.mem.
                                                                   \<1\> = mio.mio_boot.mio_boot_rsp.mem.
                                                                   \<2\> = mio.mio_boot.mio_boot_dma.mem.
                                                                   \<3\> = mio.mio_nbt.mio_nbt_fifo.mem.
                                                                   \<4\> = mio.mio_emm.bufs.mem.mem.
                                                                   \<5\> = mio.mio_nbr.ncbo_crd_fif_mem0.
                                                                   \<6\> = mio.csr.csr_fifo.
                                                                   \<7\> = mio_boot_rom/mio_boot_rom1 (internal bootroms).
                                                                   \<8\> = mio.mio_rsl.mio_pcc.gpi_pcc. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_mio_boot_bist_stat_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_13_63        : 51;
        uint64_t stat                  : 13; /**< [ 12:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<0\> = mio.mio_boot.mio_boot_fifo.mem.
                                                                   \<1\> = mio.mio_boot.mio_boot_rsp.mem.
                                                                   \<2\> = mio.mio_boot.mio_boot_dma.mem.
                                                                   \<3\> = mio.mio_nbt.mio_nbt_fifo.mem.
                                                                   \<4\> = mio.mio_emm.bufs.mem.mem.
                                                                   \<5\> = mio.mio_nbr.ncbo_crd_fif_mem0.
                                                                   \<6\> = mio.csr.csr_fifo.
                                                                   \<7\> = mio_boot_rom/mio_boot_rom1 (internal bootroms).
                                                                   \<8\> = mio.mio_rsl.mio_pcc.gpi_pcc.
                                                                   \<9\> = mio.mio_pbus.mio_pbus_fifo.mem.
                                                                  \<10\> = mio.mio_pbus.mio_pbus_rsp.mem.
                                                                  \<11\> = mio.mio_ndf.mio_ndf_bufs.ndf_buf.
                                                                  \<12\> = mio.mio_ndf.ndf_ncb_rcv_fif. */
#else /* Word 0 - Little Endian */
        uint64_t stat                  : 13; /**< [ 12:  0](RO/H) BIST status.
                                                                 Internal:
                                                                 \<0\> = mio.mio_boot.mio_boot_fifo.mem.
                                                                   \<1\> = mio.mio_boot.mio_boot_rsp.mem.
                                                                   \<2\> = mio.mio_boot.mio_boot_dma.mem.
                                                                   \<3\> = mio.mio_nbt.mio_nbt_fifo.mem.
                                                                   \<4\> = mio.mio_emm.bufs.mem.mem.
                                                                   \<5\> = mio.mio_nbr.ncbo_crd_fif_mem0.
                                                                   \<6\> = mio.csr.csr_fifo.
                                                                   \<7\> = mio_boot_rom/mio_boot_rom1 (internal bootroms).
                                                                   \<8\> = mio.mio_rsl.mio_pcc.gpi_pcc.
                                                                   \<9\> = mio.mio_pbus.mio_pbus_fifo.mem.
                                                                  \<10\> = mio.mio_pbus.mio_pbus_rsp.mem.
                                                                  \<11\> = mio.mio_ndf.mio_ndf_bufs.ndf_buf.
                                                                  \<12\> = mio.mio_ndf.ndf_ncb_rcv_fif. */
        uint64_t reserved_13_63        : 51;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_mio_boot_bist_stat bdk_mio_boot_bist_stat_t;

#define BDK_MIO_BOOT_BIST_STAT BDK_MIO_BOOT_BIST_STAT_FUNC()
static inline uint64_t BDK_MIO_BOOT_BIST_STAT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_BOOT_BIST_STAT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0000000f8ll;
    __bdk_csr_fatal("MIO_BOOT_BIST_STAT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_MIO_BOOT_BIST_STAT bdk_mio_boot_bist_stat_t
#define bustype_BDK_MIO_BOOT_BIST_STAT BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_BOOT_BIST_STAT "MIO_BOOT_BIST_STAT"
#define device_bar_BDK_MIO_BOOT_BIST_STAT 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_BOOT_BIST_STAT 0
#define arguments_BDK_MIO_BOOT_BIST_STAT -1,-1,-1,-1

/**
 * Register (RSL) mio_boot_pin_defs
 *
 * MIO Boot Pin Defaults Register
 * This register reflects the value of some of the pins sampled
 * at the rising edge of PLL_DCOK.  The GPIO pins sampled at
 * the same time are available in the GPIO_STRAP csr.
 */
union bdk_mio_boot_pin_defs
{
    uint64_t u;
    struct bdk_mio_boot_pin_defs_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t uart1_rts             : 1;  /**< [ 17: 17](RO) State of UART1_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t uart0_rts             : 1;  /**< [ 16: 16](RO) State of UART0_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t reserved_11_15        : 5;
        uint64_t rgm_supply            : 2;  /**< [ 10:  9](RO) RGMII power supply setting based on VDD_RGM_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 _ All other values reserved. */
        uint64_t emm_supply            : 3;  /**< [  8:  6](RO) EMMC power supply settings.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
#else /* Word 0 - Little Endian */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t emm_supply            : 3;  /**< [  8:  6](RO) EMMC power supply settings.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t rgm_supply            : 2;  /**< [ 10:  9](RO) RGMII power supply setting based on VDD_RGM_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 _ All other values reserved. */
        uint64_t reserved_11_15        : 5;
        uint64_t uart0_rts             : 1;  /**< [ 16: 16](RO) State of UART0_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t uart1_rts             : 1;  /**< [ 17: 17](RO) State of UART1_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_boot_pin_defs_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t uart1_rts             : 1;  /**< [ 17: 17](RO) State of UART1_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t uart0_rts             : 1;  /**< [ 16: 16](RO) Reference clock select. State of UART0_RTS_N pin sampled when DCOK asserts.
                                                                 0 = Board supplies 100 MHz to DLM_REF_CLK\<1\> (divided by 2 internally).
                                                                 1 = Board supplies  50 MHz to PLL_REFCLK. */
        uint64_t reserved_11_15        : 5;
        uint64_t rgm_supply            : 2;  /**< [ 10:  9](RO) RGMII power supply setting based on VDD_RGM_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 _ All other values reserved. */
        uint64_t emm_supply            : 3;  /**< [  8:  6](RO) EMMC power supply settings.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
#else /* Word 0 - Little Endian */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t emm_supply            : 3;  /**< [  8:  6](RO) EMMC power supply settings.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t rgm_supply            : 2;  /**< [ 10:  9](RO) RGMII power supply setting based on VDD_RGM_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 _ All other values reserved. */
        uint64_t reserved_11_15        : 5;
        uint64_t uart0_rts             : 1;  /**< [ 16: 16](RO) Reference clock select. State of UART0_RTS_N pin sampled when DCOK asserts.
                                                                 0 = Board supplies 100 MHz to DLM_REF_CLK\<1\> (divided by 2 internally).
                                                                 1 = Board supplies  50 MHz to PLL_REFCLK. */
        uint64_t uart1_rts             : 1;  /**< [ 17: 17](RO) State of UART1_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_mio_boot_pin_defs_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
#else /* Word 0 - Little Endian */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_mio_boot_pin_defs_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t uart1_rts             : 1;  /**< [ 17: 17](RO) State of UART1_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t uart0_rts             : 1;  /**< [ 16: 16](RO) State of UART0_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t reserved_9_15         : 7;
        uint64_t emm_supply            : 3;  /**< [  8:  6](RO) EMMC power supply settings.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
#else /* Word 0 - Little Endian */
        uint64_t io_supply             : 3;  /**< [  2:  0](RO) I/O power supply setting based on VDD_IO_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t smi_supply            : 3;  /**< [  5:  3](RO) SMI power supply setting based on VDD_SMI_SUPPLY_SELECT pin:
                                                                 0x2 = 2.5V.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t emm_supply            : 3;  /**< [  8:  6](RO) EMMC power supply settings.
                                                                 0x4 = 3.3V.
                                                                 _ All other values reserved. */
        uint64_t reserved_9_15         : 7;
        uint64_t uart0_rts             : 1;  /**< [ 16: 16](RO) State of UART0_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t uart1_rts             : 1;  /**< [ 17: 17](RO) State of UART1_RTS_N pin strap sampled when DCOK asserts. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_mio_boot_pin_defs bdk_mio_boot_pin_defs_t;

#define BDK_MIO_BOOT_PIN_DEFS BDK_MIO_BOOT_PIN_DEFS_FUNC()
static inline uint64_t BDK_MIO_BOOT_PIN_DEFS_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_BOOT_PIN_DEFS_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0000000c0ll;
    __bdk_csr_fatal("MIO_BOOT_PIN_DEFS", 0, 0, 0, 0, 0);
}

#define typedef_BDK_MIO_BOOT_PIN_DEFS bdk_mio_boot_pin_defs_t
#define bustype_BDK_MIO_BOOT_PIN_DEFS BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_BOOT_PIN_DEFS "MIO_BOOT_PIN_DEFS"
#define device_bar_BDK_MIO_BOOT_PIN_DEFS 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_BOOT_PIN_DEFS 0
#define arguments_BDK_MIO_BOOT_PIN_DEFS -1,-1,-1,-1

/**
 * Register (RSL) mio_boot_rom_limit
 *
 * MIO Boot ROM Limit Register
 * This register contains the largest valid address in the internal bootrom.
 */
union bdk_mio_boot_rom_limit
{
    uint64_t u;
    struct bdk_mio_boot_rom_limit_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_17_63        : 47;
        uint64_t addr                  : 17; /**< [ 16:  0](SR/W/H) End of ROM address.  This field specifies the first invalid address in ROM_MEM();
                                                                 access to a ROM_MEM() address at or above this address will return a fault and zeros.
                                                                 Writes to this register which attempt to set an [ADDR] greater than the previous [ADDR]
                                                                 setting are ignored.

                                                                 Internal:
                                                                 If trusted boot mode, resets to the size of the internal
                                                                 bootrom (0x4000), ROM boot instructions may then write to decrease the value. If
                                                                 non-trusted boot resets to a value read from ROM_MEM() at address MAX - 12, just
                                                                 before the CRC. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 17; /**< [ 16:  0](SR/W/H) End of ROM address.  This field specifies the first invalid address in ROM_MEM();
                                                                 access to a ROM_MEM() address at or above this address will return a fault and zeros.
                                                                 Writes to this register which attempt to set an [ADDR] greater than the previous [ADDR]
                                                                 setting are ignored.

                                                                 Internal:
                                                                 If trusted boot mode, resets to the size of the internal
                                                                 bootrom (0x4000), ROM boot instructions may then write to decrease the value. If
                                                                 non-trusted boot resets to a value read from ROM_MEM() at address MAX - 12, just
                                                                 before the CRC. */
        uint64_t reserved_17_63        : 47;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_boot_rom_limit_s cn; */
};
typedef union bdk_mio_boot_rom_limit bdk_mio_boot_rom_limit_t;

#define BDK_MIO_BOOT_ROM_LIMIT BDK_MIO_BOOT_ROM_LIMIT_FUNC()
static inline uint64_t BDK_MIO_BOOT_ROM_LIMIT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_BOOT_ROM_LIMIT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0000000d8ll;
    __bdk_csr_fatal("MIO_BOOT_ROM_LIMIT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_MIO_BOOT_ROM_LIMIT bdk_mio_boot_rom_limit_t
#define bustype_BDK_MIO_BOOT_ROM_LIMIT BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_BOOT_ROM_LIMIT "MIO_BOOT_ROM_LIMIT"
#define device_bar_BDK_MIO_BOOT_ROM_LIMIT 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_BOOT_ROM_LIMIT 0
#define arguments_BDK_MIO_BOOT_ROM_LIMIT -1,-1,-1,-1

/**
 * Register (RSL) mio_boot_thr
 *
 * MIO Boot Threshold Register
 * The boot-threshold register contains MIO boot-threshold values.
 */
union bdk_mio_boot_thr
{
    uint64_t u;
    struct bdk_mio_boot_thr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t fif_cnt               : 6;  /**< [ 13:  8](RO/H) Current FIFO count. */
        uint64_t reserved_6_7          : 2;
        uint64_t fif_thr               : 6;  /**< [  5:  0](R/W) NCB busy threshold. Should always read 0x19 (the only legal value). */
#else /* Word 0 - Little Endian */
        uint64_t fif_thr               : 6;  /**< [  5:  0](R/W) NCB busy threshold. Should always read 0x19 (the only legal value). */
        uint64_t reserved_6_7          : 2;
        uint64_t fif_cnt               : 6;  /**< [ 13:  8](RO/H) Current FIFO count. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_boot_thr_s cn81xx; */
    struct bdk_mio_boot_thr_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t fif_cnt               : 6;  /**< [ 13:  8](RO/H) Current NCB FIFO count. */
        uint64_t reserved_6_7          : 2;
        uint64_t fif_thr               : 6;  /**< [  5:  0](R/W) NCB busy threshold. Should always read 0x19 (the only legal value). */
#else /* Word 0 - Little Endian */
        uint64_t fif_thr               : 6;  /**< [  5:  0](R/W) NCB busy threshold. Should always read 0x19 (the only legal value). */
        uint64_t reserved_6_7          : 2;
        uint64_t fif_cnt               : 6;  /**< [ 13:  8](RO/H) Current NCB FIFO count. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_mio_boot_thr_s cn83xx; */
};
typedef union bdk_mio_boot_thr bdk_mio_boot_thr_t;

#define BDK_MIO_BOOT_THR BDK_MIO_BOOT_THR_FUNC()
static inline uint64_t BDK_MIO_BOOT_THR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_BOOT_THR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0000000b0ll;
    __bdk_csr_fatal("MIO_BOOT_THR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_MIO_BOOT_THR bdk_mio_boot_thr_t
#define bustype_BDK_MIO_BOOT_THR BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_BOOT_THR "MIO_BOOT_THR"
#define device_bar_BDK_MIO_BOOT_THR 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_BOOT_THR 0
#define arguments_BDK_MIO_BOOT_THR -1,-1,-1,-1

#endif /* __BDK_CSRS_MIO_BOOT_H__ */
