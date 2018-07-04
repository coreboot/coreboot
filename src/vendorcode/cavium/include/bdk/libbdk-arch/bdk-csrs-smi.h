#ifndef __BDK_CSRS_SMI_H__
#define __BDK_CSRS_SMI_H__
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
 * Cavium SMI.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration smi_bar_e
 *
 * SMI Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_SMI_BAR_E_SMI_PF_BAR0_CN8 (0x87e005000000ll)
#define BDK_SMI_BAR_E_SMI_PF_BAR0_CN8_SIZE 0x800000ull
#define BDK_SMI_BAR_E_SMI_PF_BAR0_CN9 (0x87e005000000ll)
#define BDK_SMI_BAR_E_SMI_PF_BAR0_CN9_SIZE 0x100000ull

/**
 * Register (RSL) smi_#_clk
 *
 * SMI Clock Control Register
 * This register determines the SMI timing characteristics.
 * If software wants to change SMI CLK timing parameters ([SAMPLE]/[SAMPLE_HI]), software
 * must delay the SMI_()_CLK CSR write by at least 512 coprocessor-clock cycles after the
 * previous SMI operation is finished.
 */
union bdk_smi_x_clk
{
    uint64_t u;
    struct bdk_smi_x_clk_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_25_63        : 39;
        uint64_t mode                  : 1;  /**< [ 24: 24](R/W) IEEE operating mode; 0 = Clause 22 compliant, 1 = Clause 45 compliant. */
        uint64_t reserved_21_23        : 3;
        uint64_t sample_hi             : 5;  /**< [ 20: 16](R/W) Sample (extended bits). Specifies in coprocessor clock cycles when to sample read data. */
        uint64_t sample_mode           : 1;  /**< [ 15: 15](R/W) Read data sampling mode.
                                                                 According to the 802.3 specification, on read operations, the STA transitions SMIn_MDC and
                                                                 the PHY drives SMIn_MDIO with some delay relative to that edge. This is Edge1.
                                                                 The STA then samples SMIn_MDIO on the next rising edge of SMIn_MDC. This is Edge2. The
                                                                 read data can be sampled relative to either edge.
                                                                 0 = Sample time is relative to Edge2.
                                                                 1 = Sample time is relative to Edge1. */
        uint64_t reserved_14           : 1;
        uint64_t clk_idle              : 1;  /**< [ 13: 13](R/W) SMIn_MDC toggle. When set, this bit causes SMIn_MDC not to toggle on idle cycles. */
        uint64_t preamble              : 1;  /**< [ 12: 12](R/W) Preamble. When this bit is set, the 32-bit preamble is sent first on SMI transactions.
                                                                 This field must be set to 1 when [MODE] = 1 in order for the receiving PHY to correctly
                                                                 frame the transaction. */
        uint64_t sample                : 4;  /**< [ 11:  8](R/W) Sample read data. Specifies the number of coprocessor clock cycles after the rising edge
                                                                 of SMIn_MDC to wait before sampling read data.

                                                                 _ ([SAMPLE_HI],[SAMPLE]) \> 1

                                                                 _ ([SAMPLE_HI],[SAMPLE]) + 3 \<= 2 * [PHASE] */
        uint64_t phase                 : 8;  /**< [  7:  0](R/W) MDC clock phase. Specifies the number of coprocessor clock cycles that make up an SMIn_MDC
                                                                 phase.

                                                                 _ [PHASE] \> 2 */
#else /* Word 0 - Little Endian */
        uint64_t phase                 : 8;  /**< [  7:  0](R/W) MDC clock phase. Specifies the number of coprocessor clock cycles that make up an SMIn_MDC
                                                                 phase.

                                                                 _ [PHASE] \> 2 */
        uint64_t sample                : 4;  /**< [ 11:  8](R/W) Sample read data. Specifies the number of coprocessor clock cycles after the rising edge
                                                                 of SMIn_MDC to wait before sampling read data.

                                                                 _ ([SAMPLE_HI],[SAMPLE]) \> 1

                                                                 _ ([SAMPLE_HI],[SAMPLE]) + 3 \<= 2 * [PHASE] */
        uint64_t preamble              : 1;  /**< [ 12: 12](R/W) Preamble. When this bit is set, the 32-bit preamble is sent first on SMI transactions.
                                                                 This field must be set to 1 when [MODE] = 1 in order for the receiving PHY to correctly
                                                                 frame the transaction. */
        uint64_t clk_idle              : 1;  /**< [ 13: 13](R/W) SMIn_MDC toggle. When set, this bit causes SMIn_MDC not to toggle on idle cycles. */
        uint64_t reserved_14           : 1;
        uint64_t sample_mode           : 1;  /**< [ 15: 15](R/W) Read data sampling mode.
                                                                 According to the 802.3 specification, on read operations, the STA transitions SMIn_MDC and
                                                                 the PHY drives SMIn_MDIO with some delay relative to that edge. This is Edge1.
                                                                 The STA then samples SMIn_MDIO on the next rising edge of SMIn_MDC. This is Edge2. The
                                                                 read data can be sampled relative to either edge.
                                                                 0 = Sample time is relative to Edge2.
                                                                 1 = Sample time is relative to Edge1. */
        uint64_t sample_hi             : 5;  /**< [ 20: 16](R/W) Sample (extended bits). Specifies in coprocessor clock cycles when to sample read data. */
        uint64_t reserved_21_23        : 3;
        uint64_t mode                  : 1;  /**< [ 24: 24](R/W) IEEE operating mode; 0 = Clause 22 compliant, 1 = Clause 45 compliant. */
        uint64_t reserved_25_63        : 39;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_x_clk_s cn8; */
    struct bdk_smi_x_clk_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_25_63        : 39;
        uint64_t mode                  : 1;  /**< [ 24: 24](R/W) IEEE operating mode; 0 = Clause 22 compliant, 1 = Clause 45 compliant. */
        uint64_t reserved_21_23        : 3;
        uint64_t sample_hi             : 5;  /**< [ 20: 16](R/W) Sample (extended bits). Specifies in coprocessor clock cycles when to sample read data. */
        uint64_t sample_mode           : 1;  /**< [ 15: 15](R/W) Read data sampling mode.
                                                                 According to the 802.3 specification, on read operations, the STA transitions SMIn_MDC and
                                                                 the PHY drives SMIn_MDIO with some delay relative to that edge. This is Edge1.
                                                                 The STA then samples SMIn_MDIO on the next rising edge of SMIn_MDC. This is Edge2. The
                                                                 read data can be sampled relative to either edge.
                                                                 0 = Sample time is relative to Edge2.
                                                                 1 = Sample time is relative to Edge1. */
        uint64_t reserved_14           : 1;
        uint64_t clk_idle              : 1;  /**< [ 13: 13](R/W) SMIn_MDC toggle. When set, this bit causes SMIn_MDC not to toggle on idle cycles. */
        uint64_t preamble              : 1;  /**< [ 12: 12](R/W) Preamble. When this bit is set, the 32-bit preamble is sent first on SMI transactions.
                                                                 This field must be set to 1 when [MODE] = 1 in order for the receiving PHY to correctly
                                                                 frame the transaction. */
        uint64_t sample                : 4;  /**< [ 11:  8](R/W) Sample read data. Specifies the number of coprocessor clock cycles after the rising edge
                                                                 of SMIn_MDC to wait before sampling read data.

                                                                 _ ([SAMPLE_HI],[SAMPLE]) \> 1

                                                                 _ ([SAMPLE_HI],[SAMPLE]) + 3 \<= 2 * [PHASE] */
        uint64_t phase                 : 8;  /**< [  7:  0](R/W) MDC clock phase. Specifies the number of coprocessor clock cycles that make up an SMIn_MDC
                                                                 phase.

                                                                 _ [PHASE] \> 2

                                                                 Internal:
                                                                 FIXME number of 100MHz clocks or coproc clocks based on CSR that defaults to 100MHz. */
#else /* Word 0 - Little Endian */
        uint64_t phase                 : 8;  /**< [  7:  0](R/W) MDC clock phase. Specifies the number of coprocessor clock cycles that make up an SMIn_MDC
                                                                 phase.

                                                                 _ [PHASE] \> 2

                                                                 Internal:
                                                                 FIXME number of 100MHz clocks or coproc clocks based on CSR that defaults to 100MHz. */
        uint64_t sample                : 4;  /**< [ 11:  8](R/W) Sample read data. Specifies the number of coprocessor clock cycles after the rising edge
                                                                 of SMIn_MDC to wait before sampling read data.

                                                                 _ ([SAMPLE_HI],[SAMPLE]) \> 1

                                                                 _ ([SAMPLE_HI],[SAMPLE]) + 3 \<= 2 * [PHASE] */
        uint64_t preamble              : 1;  /**< [ 12: 12](R/W) Preamble. When this bit is set, the 32-bit preamble is sent first on SMI transactions.
                                                                 This field must be set to 1 when [MODE] = 1 in order for the receiving PHY to correctly
                                                                 frame the transaction. */
        uint64_t clk_idle              : 1;  /**< [ 13: 13](R/W) SMIn_MDC toggle. When set, this bit causes SMIn_MDC not to toggle on idle cycles. */
        uint64_t reserved_14           : 1;
        uint64_t sample_mode           : 1;  /**< [ 15: 15](R/W) Read data sampling mode.
                                                                 According to the 802.3 specification, on read operations, the STA transitions SMIn_MDC and
                                                                 the PHY drives SMIn_MDIO with some delay relative to that edge. This is Edge1.
                                                                 The STA then samples SMIn_MDIO on the next rising edge of SMIn_MDC. This is Edge2. The
                                                                 read data can be sampled relative to either edge.
                                                                 0 = Sample time is relative to Edge2.
                                                                 1 = Sample time is relative to Edge1. */
        uint64_t sample_hi             : 5;  /**< [ 20: 16](R/W) Sample (extended bits). Specifies in coprocessor clock cycles when to sample read data. */
        uint64_t reserved_21_23        : 3;
        uint64_t mode                  : 1;  /**< [ 24: 24](R/W) IEEE operating mode; 0 = Clause 22 compliant, 1 = Clause 45 compliant. */
        uint64_t reserved_25_63        : 39;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_smi_x_clk bdk_smi_x_clk_t;

static inline uint64_t BDK_SMI_X_CLK(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_X_CLK(unsigned long a)
{
    if (a<=1)
        return 0x87e005003818ll + 0x80ll * ((a) & 0x1);
    __bdk_csr_fatal("SMI_X_CLK", 1, a, 0, 0, 0);
}

#define typedef_BDK_SMI_X_CLK(a) bdk_smi_x_clk_t
#define bustype_BDK_SMI_X_CLK(a) BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_X_CLK(a) "SMI_X_CLK"
#define device_bar_BDK_SMI_X_CLK(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_X_CLK(a) (a)
#define arguments_BDK_SMI_X_CLK(a) (a),-1,-1,-1

/**
 * Register (RSL) smi_#_clken
 *
 * SMI Clock Enable Register
 * This register is to force conditional clock enable.
 */
union bdk_smi_x_clken
{
    uint64_t u;
    struct bdk_smi_x_clken_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t clken                 : 1;  /**< [  0:  0](R/W) Force the conditional clocking within SMI to be always on. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t clken                 : 1;  /**< [  0:  0](R/W) Force the conditional clocking within SMI to be always on. For diagnostic use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_x_clken_s cn; */
};
typedef union bdk_smi_x_clken bdk_smi_x_clken_t;

static inline uint64_t BDK_SMI_X_CLKEN(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_X_CLKEN(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e005003830ll + 0x80ll * ((a) & 0x1);
    __bdk_csr_fatal("SMI_X_CLKEN", 1, a, 0, 0, 0);
}

#define typedef_BDK_SMI_X_CLKEN(a) bdk_smi_x_clken_t
#define bustype_BDK_SMI_X_CLKEN(a) BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_X_CLKEN(a) "SMI_X_CLKEN"
#define device_bar_BDK_SMI_X_CLKEN(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_X_CLKEN(a) (a)
#define arguments_BDK_SMI_X_CLKEN(a) (a),-1,-1,-1

/**
 * Register (RSL) smi_#_cmd
 *
 * SMI Command Control Register
 * This register forces a read or write command to the PHY. Write operations to this register
 * create SMI transactions. Software will poll (depending on the transaction type).
 */
union bdk_smi_x_cmd
{
    uint64_t u;
    struct bdk_smi_x_cmd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t phy_op                : 2;  /**< [ 17: 16](R/W) PHY opcode, depending on SMI_()_CLK[MODE] setting.
                                                                 * If SMI_()_CLK[MODE] = 0 (\<=1Gbs / Clause 22):
                                                                 0 = Write operation, encoded in the frame as 01.
                                                                 1 = Read operation, encoded in the frame as 10.

                                                                 * If SMI_()_CLK[MODE] = 1 (\>1Gbs / Clause 45):
                                                                 0x0 = Address.
                                                                 0x1 = Write.
                                                                 0x2 = Post-read-increment-address.
                                                                 0x3 = Read. */
        uint64_t reserved_13_15        : 3;
        uint64_t phy_adr               : 5;  /**< [ 12:  8](R/W) PHY address. */
        uint64_t reserved_5_7          : 3;
        uint64_t reg_adr               : 5;  /**< [  4:  0](R/W) PHY register offset. */
#else /* Word 0 - Little Endian */
        uint64_t reg_adr               : 5;  /**< [  4:  0](R/W) PHY register offset. */
        uint64_t reserved_5_7          : 3;
        uint64_t phy_adr               : 5;  /**< [ 12:  8](R/W) PHY address. */
        uint64_t reserved_13_15        : 3;
        uint64_t phy_op                : 2;  /**< [ 17: 16](R/W) PHY opcode, depending on SMI_()_CLK[MODE] setting.
                                                                 * If SMI_()_CLK[MODE] = 0 (\<=1Gbs / Clause 22):
                                                                 0 = Write operation, encoded in the frame as 01.
                                                                 1 = Read operation, encoded in the frame as 10.

                                                                 * If SMI_()_CLK[MODE] = 1 (\>1Gbs / Clause 45):
                                                                 0x0 = Address.
                                                                 0x1 = Write.
                                                                 0x2 = Post-read-increment-address.
                                                                 0x3 = Read. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_x_cmd_s cn; */
};
typedef union bdk_smi_x_cmd bdk_smi_x_cmd_t;

static inline uint64_t BDK_SMI_X_CMD(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_X_CMD(unsigned long a)
{
    if (a<=1)
        return 0x87e005003800ll + 0x80ll * ((a) & 0x1);
    __bdk_csr_fatal("SMI_X_CMD", 1, a, 0, 0, 0);
}

#define typedef_BDK_SMI_X_CMD(a) bdk_smi_x_cmd_t
#define bustype_BDK_SMI_X_CMD(a) BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_X_CMD(a) "SMI_X_CMD"
#define device_bar_BDK_SMI_X_CMD(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_X_CMD(a) (a)
#define arguments_BDK_SMI_X_CMD(a) (a),-1,-1,-1

/**
 * Register (RSL) smi_#_en
 *
 * SMI Enable Register
 * Enables the SMI interface.
 */
union bdk_smi_x_en
{
    uint64_t u;
    struct bdk_smi_x_en_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t en                    : 1;  /**< [  0:  0](R/W) SMI/MDIO interface enable:
                                                                 1 = Enable interface.
                                                                 0 = Disable interface: no transactions, no SMIn_MDC transitions. */
#else /* Word 0 - Little Endian */
        uint64_t en                    : 1;  /**< [  0:  0](R/W) SMI/MDIO interface enable:
                                                                 1 = Enable interface.
                                                                 0 = Disable interface: no transactions, no SMIn_MDC transitions. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_x_en_s cn; */
};
typedef union bdk_smi_x_en bdk_smi_x_en_t;

static inline uint64_t BDK_SMI_X_EN(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_X_EN(unsigned long a)
{
    if (a<=1)
        return 0x87e005003820ll + 0x80ll * ((a) & 0x1);
    __bdk_csr_fatal("SMI_X_EN", 1, a, 0, 0, 0);
}

#define typedef_BDK_SMI_X_EN(a) bdk_smi_x_en_t
#define bustype_BDK_SMI_X_EN(a) BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_X_EN(a) "SMI_X_EN"
#define device_bar_BDK_SMI_X_EN(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_X_EN(a) (a)
#define arguments_BDK_SMI_X_EN(a) (a),-1,-1,-1

/**
 * Register (RSL) smi_#_rd_dat
 *
 * SMI Read Data Register
 * This register contains the data in a read operation.
 */
union bdk_smi_x_rd_dat
{
    uint64_t u;
    struct bdk_smi_x_rd_dat_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t pending               : 1;  /**< [ 17: 17](RO/H) Read transaction pending. Indicates that an SMI read transaction is in flight. */
        uint64_t val                   : 1;  /**< [ 16: 16](RO/H) Read data valid. Asserts when the read transaction completes. A read to this register clears [VAL]. */
        uint64_t dat                   : 16; /**< [ 15:  0](RO/H) Read data. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 16; /**< [ 15:  0](RO/H) Read data. */
        uint64_t val                   : 1;  /**< [ 16: 16](RO/H) Read data valid. Asserts when the read transaction completes. A read to this register clears [VAL]. */
        uint64_t pending               : 1;  /**< [ 17: 17](RO/H) Read transaction pending. Indicates that an SMI read transaction is in flight. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_x_rd_dat_s cn; */
};
typedef union bdk_smi_x_rd_dat bdk_smi_x_rd_dat_t;

static inline uint64_t BDK_SMI_X_RD_DAT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_X_RD_DAT(unsigned long a)
{
    if (a<=1)
        return 0x87e005003810ll + 0x80ll * ((a) & 0x1);
    __bdk_csr_fatal("SMI_X_RD_DAT", 1, a, 0, 0, 0);
}

#define typedef_BDK_SMI_X_RD_DAT(a) bdk_smi_x_rd_dat_t
#define bustype_BDK_SMI_X_RD_DAT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_X_RD_DAT(a) "SMI_X_RD_DAT"
#define device_bar_BDK_SMI_X_RD_DAT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_X_RD_DAT(a) (a)
#define arguments_BDK_SMI_X_RD_DAT(a) (a),-1,-1,-1

/**
 * Register (RSL) smi_#_wr_dat
 *
 * SMI Write Data Register
 * This register provides the data for a write operation.
 */
union bdk_smi_x_wr_dat
{
    uint64_t u;
    struct bdk_smi_x_wr_dat_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_18_63        : 46;
        uint64_t pending               : 1;  /**< [ 17: 17](RO/H) Write transaction pending. Indicates that an SMI write transaction is in flight. */
        uint64_t val                   : 1;  /**< [ 16: 16](RO/H) Write data valid. Asserts when the write transaction completes. A read to this
                                                                 register clears [VAL]. */
        uint64_t dat                   : 16; /**< [ 15:  0](R/W/H) Write data. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 16; /**< [ 15:  0](R/W/H) Write data. */
        uint64_t val                   : 1;  /**< [ 16: 16](RO/H) Write data valid. Asserts when the write transaction completes. A read to this
                                                                 register clears [VAL]. */
        uint64_t pending               : 1;  /**< [ 17: 17](RO/H) Write transaction pending. Indicates that an SMI write transaction is in flight. */
        uint64_t reserved_18_63        : 46;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_x_wr_dat_s cn; */
};
typedef union bdk_smi_x_wr_dat bdk_smi_x_wr_dat_t;

static inline uint64_t BDK_SMI_X_WR_DAT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_X_WR_DAT(unsigned long a)
{
    if (a<=1)
        return 0x87e005003808ll + 0x80ll * ((a) & 0x1);
    __bdk_csr_fatal("SMI_X_WR_DAT", 1, a, 0, 0, 0);
}

#define typedef_BDK_SMI_X_WR_DAT(a) bdk_smi_x_wr_dat_t
#define bustype_BDK_SMI_X_WR_DAT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_X_WR_DAT(a) "SMI_X_WR_DAT"
#define device_bar_BDK_SMI_X_WR_DAT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_X_WR_DAT(a) (a)
#define arguments_BDK_SMI_X_WR_DAT(a) (a),-1,-1,-1

/**
 * Register (RSL) smi_drv_ctl
 *
 * SMI Drive Strength Control Register
 * Enables the SMI interface.
 */
union bdk_smi_drv_ctl
{
    uint64_t u;
    struct bdk_smi_drv_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_11_63        : 53;
        uint64_t pctl                  : 3;  /**< [ 10:  8](R/W) PCTL drive strength control bits. Suggested values:
                                                                 0x4 = 60 ohm.
                                                                 0x6 = 40 ohm.
                                                                 0x7 = 30 ohm. */
        uint64_t reserved_3_7          : 5;
        uint64_t nctl                  : 3;  /**< [  2:  0](R/W) NCTL drive strength control bits. Suggested values:
                                                                 0x4 = 60 ohm.
                                                                 0x6 = 40 ohm.
                                                                 0x7 = 30 ohm. */
#else /* Word 0 - Little Endian */
        uint64_t nctl                  : 3;  /**< [  2:  0](R/W) NCTL drive strength control bits. Suggested values:
                                                                 0x4 = 60 ohm.
                                                                 0x6 = 40 ohm.
                                                                 0x7 = 30 ohm. */
        uint64_t reserved_3_7          : 5;
        uint64_t pctl                  : 3;  /**< [ 10:  8](R/W) PCTL drive strength control bits. Suggested values:
                                                                 0x4 = 60 ohm.
                                                                 0x6 = 40 ohm.
                                                                 0x7 = 30 ohm. */
        uint64_t reserved_11_63        : 53;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_drv_ctl_s cn; */
};
typedef union bdk_smi_drv_ctl bdk_smi_drv_ctl_t;

#define BDK_SMI_DRV_CTL BDK_SMI_DRV_CTL_FUNC()
static inline uint64_t BDK_SMI_DRV_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_DRV_CTL_FUNC(void)
{
    return 0x87e005003828ll;
}

#define typedef_BDK_SMI_DRV_CTL bdk_smi_drv_ctl_t
#define bustype_BDK_SMI_DRV_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_DRV_CTL "SMI_DRV_CTL"
#define device_bar_BDK_SMI_DRV_CTL 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_DRV_CTL 0
#define arguments_BDK_SMI_DRV_CTL -1,-1,-1,-1

/**
 * Register (RSL) smi_drv_rsvd
 *
 * INTERNAL: SMI Drive Reserve Register
 *
 * Enables the SMI1 interface.
 */
union bdk_smi_drv_rsvd
{
    uint64_t u;
    struct bdk_smi_drv_rsvd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_11_63        : 53;
        uint64_t pctl                  : 3;  /**< [ 10:  8](R/W) Reserved. */
        uint64_t reserved_3_7          : 5;
        uint64_t nctl                  : 3;  /**< [  2:  0](R/W) Reserved. */
#else /* Word 0 - Little Endian */
        uint64_t nctl                  : 3;  /**< [  2:  0](R/W) Reserved. */
        uint64_t reserved_3_7          : 5;
        uint64_t pctl                  : 3;  /**< [ 10:  8](R/W) Reserved. */
        uint64_t reserved_11_63        : 53;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_smi_drv_rsvd_s cn; */
};
typedef union bdk_smi_drv_rsvd bdk_smi_drv_rsvd_t;

#define BDK_SMI_DRV_RSVD BDK_SMI_DRV_RSVD_FUNC()
static inline uint64_t BDK_SMI_DRV_RSVD_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SMI_DRV_RSVD_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0050038a8ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0050038a8ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0050038a8ll;
    __bdk_csr_fatal("SMI_DRV_RSVD", 0, 0, 0, 0, 0);
}

#define typedef_BDK_SMI_DRV_RSVD bdk_smi_drv_rsvd_t
#define bustype_BDK_SMI_DRV_RSVD BDK_CSR_TYPE_RSL
#define basename_BDK_SMI_DRV_RSVD "SMI_DRV_RSVD"
#define device_bar_BDK_SMI_DRV_RSVD 0x0 /* PF_BAR0 */
#define busnum_BDK_SMI_DRV_RSVD 0
#define arguments_BDK_SMI_DRV_RSVD -1,-1,-1,-1

#endif /* __BDK_CSRS_SMI_H__ */
