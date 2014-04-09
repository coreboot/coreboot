/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.*
 *
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __UART_DM_H__
#define __UART_DM_H__

#include <asm/io.h>
#include "common.h"
#define MSM_BOOT_UART_DM_EXTR_BITS(value, start_pos, end_pos) \
                                             ((value << (32 - end_pos))\
                                              >> (32 - (end_pos - start_pos)))


extern void dsb(void);
#define PACK_CHARS_INTO_WORDS(a, cnt, word)  {                                 \
                                               word = 0;                       \
                                               int j;                          \
                                               for(j=0; j < (int)cnt; j++) { \
                                                   word |= (a[j] & 0xff)<< (j * 8);\
                                               }                               \
                                              }

extern void __udelay(unsigned long usec);


enum MSM_BOOT_UART_DM_PARITY_MODE {
        MSM_BOOT_UART_DM_NO_PARITY,
        MSM_BOOT_UART_DM_ODD_PARITY,
        MSM_BOOT_UART_DM_EVEN_PARITY,
        MSM_BOOT_UART_DM_SPACE_PARITY
};

/* UART Stop Bit Length */
enum MSM_BOOT_UART_DM_STOP_BIT_LEN {
        MSM_BOOT_UART_DM_SBL_9_16,
        MSM_BOOT_UART_DM_SBL_1,
        MSM_BOOT_UART_DM_SBL_1_9_16,
        MSM_BOOT_UART_DM_SBL_2
};

/* UART Bits per Char */
enum MSM_BOOT_UART_DM_BITS_PER_CHAR {
        MSM_BOOT_UART_DM_5_BPS,
        MSM_BOOT_UART_DM_6_BPS,
        MSM_BOOT_UART_DM_7_BPS,
        MSM_BOOT_UART_DM_8_BPS
};

/* 8-N-1 Configuration */
#define MSM_BOOT_UART_DM_8_N_1_MODE          (MSM_BOOT_UART_DM_NO_PARITY | \
                                             (MSM_BOOT_UART_DM_SBL_1 << 2) | \
                                             (MSM_BOOT_UART_DM_8_BPS << 4))

/* UART_DM Registers */

/* UART Operational Mode Register */
#define MSM_BOOT_UART_DM_MR1(base)             ((base) + 0x00)
#define MSM_BOOT_UART_DM_MR2(base)             ((base) + 0x04)
#define MSM_BOOT_UART_DM_RXBRK_ZERO_CHAR_OFF (1 << 8)
#define MSM_BOOT_UART_DM_LOOPBACK            (1 << 7)

/* UART Clock Selection Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_CSR(base)             ((base) + 0xA0)
#else
#define MSM_BOOT_UART_DM_CSR(base)             ((base) + 0x08)
#endif

/* UART DM TX FIFO Registers - 4 */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_TF(base, x)         ((base) + 0x100+(4*(x)))
#else
#define MSM_BOOT_UART_DM_TF(base, x)         ((base) + 0x70+(4*(x)))
#endif

/* UART Command Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_CR(base)              ((base) + 0xA8)
#else
#define MSM_BOOT_UART_DM_CR(base)              ((base) + 0x10)
#endif
#define MSM_BOOT_UART_DM_CR_RX_ENABLE        (1 << 0)
#define MSM_BOOT_UART_DM_CR_RX_DISABLE       (1 << 1)
#define MSM_BOOT_UART_DM_CR_TX_ENABLE        (1 << 2)
#define MSM_BOOT_UART_DM_CR_TX_DISABLE       (1 << 3)

/* UART Channel Command */
#define MSM_BOOT_UART_DM_CR_CH_CMD_LSB(x)    ((x & 0x0f) << 4)
#define MSM_BOOT_UART_DM_CR_CH_CMD_MSB(x)    ((x >> 4 ) << 11 )
#define MSM_BOOT_UART_DM_CR_CH_CMD(x)        (MSM_BOOT_UART_DM_CR_CH_CMD_LSB(x)\
                                             | MSM_BOOT_UART_DM_CR_CH_CMD_MSB(x))
#define MSM_BOOT_UART_DM_CMD_NULL            MSM_BOOT_UART_DM_CR_CH_CMD(0)
#define MSM_BOOT_UART_DM_CMD_RESET_RX        MSM_BOOT_UART_DM_CR_CH_CMD(1)
#define MSM_BOOT_UART_DM_CMD_RESET_TX        MSM_BOOT_UART_DM_CR_CH_CMD(2)
#define MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT  MSM_BOOT_UART_DM_CR_CH_CMD(3)
#define MSM_BOOT_UART_DM_CMD_RES_BRK_CHG_INT MSM_BOOT_UART_DM_CR_CH_CMD(4)
#define MSM_BOOT_UART_DM_CMD_START_BRK       MSM_BOOT_UART_DM_CR_CH_CMD(5)
#define MSM_BOOT_UART_DM_CMD_STOP_BRK        MSM_BOOT_UART_DM_CR_CH_CMD(6)
#define MSM_BOOT_UART_DM_CMD_RES_CTS_N       MSM_BOOT_UART_DM_CR_CH_CMD(7)
#define MSM_BOOT_UART_DM_CMD_RES_STALE_INT   MSM_BOOT_UART_DM_CR_CH_CMD(8)
#define MSM_BOOT_UART_DM_CMD_PACKET_MODE     MSM_BOOT_UART_DM_CR_CH_CMD(9)
#define MSM_BOOT_UART_DM_CMD_MODE_RESET      MSM_BOOT_UART_DM_CR_CH_CMD(C)
#define MSM_BOOT_UART_DM_CMD_SET_RFR_N       MSM_BOOT_UART_DM_CR_CH_CMD(D)
#define MSM_BOOT_UART_DM_CMD_RES_RFR_N       MSM_BOOT_UART_DM_CR_CH_CMD(E)
#define MSM_BOOT_UART_DM_CMD_RES_TX_ERR      MSM_BOOT_UART_DM_CR_CH_CMD(10)
#define MSM_BOOT_UART_DM_CMD_CLR_TX_DONE     MSM_BOOT_UART_DM_CR_CH_CMD(11)
#define MSM_BOOT_UART_DM_CMD_RES_BRKSTRT_INT MSM_BOOT_UART_DM_CR_CH_CMD(12)
#define MSM_BOOT_UART_DM_CMD_RES_BRKEND_INT  MSM_BOOT_UART_DM_CR_CH_CMD(13)
#define MSM_BOOT_UART_DM_CMD_RES_PER_FRM_INT MSM_BOOT_UART_DM_CR_CH_CMD(14)

/*UART General Command */
#define MSM_BOOT_UART_DM_CR_GENERAL_CMD(x)   ((x) << 8)

#define MSM_BOOT_UART_DM_GCMD_NULL            MSM_BOOT_UART_DM_CR_GENERAL_CMD(0)
#define MSM_BOOT_UART_DM_GCMD_CR_PROT_EN      MSM_BOOT_UART_DM_CR_GENERAL_CMD(1)
#define MSM_BOOT_UART_DM_GCMD_CR_PROT_DIS     MSM_BOOT_UART_DM_CR_GENERAL_CMD(2)
#define MSM_BOOT_UART_DM_GCMD_RES_TX_RDY_INT  MSM_BOOT_UART_DM_CR_GENERAL_CMD(3)
#define MSM_BOOT_UART_DM_GCMD_SW_FORCE_STALE  MSM_BOOT_UART_DM_CR_GENERAL_CMD(4)
#define MSM_BOOT_UART_DM_GCMD_ENA_STALE_EVT   MSM_BOOT_UART_DM_CR_GENERAL_CMD(5)
#define MSM_BOOT_UART_DM_GCMD_DIS_STALE_EVT   MSM_BOOT_UART_DM_CR_GENERAL_CMD(6)

/* UART Interrupt Mask Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_IMR(base)             ((base) + 0xB0)
#else
#define MSM_BOOT_UART_DM_IMR(base)             ((base) + 0x14)
#endif

#define MSM_BOOT_UART_DM_TXLEV               (1 << 0)
#define MSM_BOOT_UART_DM_RXHUNT              (1 << 1)
#define MSM_BOOT_UART_DM_RXBRK_CHNG          (1 << 2)
#define MSM_BOOT_UART_DM_RXSTALE             (1 << 3)
#define MSM_BOOT_UART_DM_RXLEV               (1 << 4)
#define MSM_BOOT_UART_DM_DELTA_CTS           (1 << 5)
#define MSM_BOOT_UART_DM_CURRENT_CTS         (1 << 6)
#define MSM_BOOT_UART_DM_TX_READY            (1 << 7)
#define MSM_BOOT_UART_DM_TX_ERROR            (1 << 8)
#define MSM_BOOT_UART_DM_TX_DONE             (1 << 9)
#define MSM_BOOT_UART_DM_RXBREAK_START       (1 << 10)
#define MSM_BOOT_UART_DM_RXBREAK_END         (1 << 11)
#define MSM_BOOT_UART_DM_PAR_FRAME_ERR_IRQ   (1 << 12)

#define MSM_BOOT_UART_DM_IMR_ENABLED         (MSM_BOOT_UART_DM_TX_READY | \
                                              MSM_BOOT_UART_DM_TXLEV    | \
                                              MSM_BOOT_UART_DM_RXSTALE)

/* UART Interrupt Programming Register */
#define MSM_BOOT_UART_DM_IPR(base)             ((base) + 0x18)
#define MSM_BOOT_UART_DM_STALE_TIMEOUT_LSB   0x0f
#define MSM_BOOT_UART_DM_STALE_TIMEOUT_MSB   0	/* Not used currently */

/* UART Transmit/Receive FIFO Watermark Register */
#define MSM_BOOT_UART_DM_TFWR(base)            ((base) + 0x1C)
/* Interrupt is generated when FIFO level is less than or equal to this value */
#define MSM_BOOT_UART_DM_TFW_VALUE           0

#define MSM_BOOT_UART_DM_RFWR(base)            ((base) + 0x20)
/*Interrupt generated when no of words in RX FIFO is greater than this value */
#define MSM_BOOT_UART_DM_RFW_VALUE           0

/* UART Hunt Character Register */
#define MSM_BOOT_UART_DM_HCR(base)             ((base) + 0x24)

/* Used for RX transfer initialization */
#define MSM_BOOT_UART_DM_DMRX(base)            ((base) + 0x34)

/* Default DMRX value - any value bigger than FIFO size would be fine */
#define MSM_BOOT_UART_DM_DMRX_DEF_VALUE    0x220

/* Register to enable IRDA function */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_IRDA(base)            ((base) + 0xB8)
#else
#define MSM_BOOT_UART_DM_IRDA(base)            ((base) + 0x38)
#endif

/* UART Data Mover Enable Register */
#define MSM_BOOT_UART_DM_DMEN(base)            ((base) + 0x3C)

/* Number of characters for Transmission */
#define MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base) ((base) + 0x040)

/* UART RX FIFO Base Address */
#define MSM_BOOT_UART_DM_BADR(base)            ((base) + 0x44)

/* UART Status Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_SR(base)              ((base) + 0x0A4)
#else
#define MSM_BOOT_UART_DM_SR(base)              ((base) + 0x008)
#endif
#define MSM_BOOT_UART_DM_SR_RXRDY            (1 << 0)
#define MSM_BOOT_UART_DM_SR_RXFULL           (1 << 1)
#define MSM_BOOT_UART_DM_SR_TXRDY            (1 << 2)
#define MSM_BOOT_UART_DM_SR_TXEMT            (1 << 3)
#define MSM_BOOT_UART_DM_SR_UART_OVERRUN     (1 << 4)
#define MSM_BOOT_UART_DM_SR_PAR_FRAME_ERR    (1 << 5)
#define MSM_BOOT_UART_DM_RX_BREAK            (1 << 6)
#define MSM_BOOT_UART_DM_HUNT_CHAR           (1 << 7)
#define MSM_BOOT_UART_DM_RX_BRK_START_LAST   (1 << 8)

/* UART Receive FIFO Registers - 4 in numbers */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_RF(base, x)      ((base) + 0x140 + (4*(x)))
#else
#define MSM_BOOT_UART_DM_RF(base, x)      ((base) + 0x70 + (4*(x)))
#endif

/* UART Masked Interrupt Status Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_MISR(base)         ((base) + 0xAC)
#else
#define MSM_BOOT_UART_DM_MISR(base)         ((base) + 0x10)
#endif

/* UART Interrupt Status Register */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_ISR(base)          ((base) + 0xB4)
#else
#define MSM_BOOT_UART_DM_ISR(base)          ((base) + 0x14)
#endif

/* Number of characters received since the end of last RX transfer */
#if PERIPH_BLK_BLSP
#define MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base)  ((base) + 0xBC)
#else
#define MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base)  ((base) + 0x38)
#endif

/* UART TX FIFO Status Register */
#define MSM_BOOT_UART_DM_TXFS(base)           ((base) + 0x4C)
#define MSM_BOOT_UART_DM_TXFS_STATE_LSB(x)   MSM_BOOT_UART_DM_EXTR_BITS(x,0,6)
#define MSM_BOOT_UART_DM_TXFS_STATE_MSB(x)   MSM_BOOT_UART_DM_EXTR_BITS(x,14,31)
#define MSM_BOOT_UART_DM_TXFS_BUF_STATE(x)   MSM_BOOT_UART_DM_EXTR_BITS(x,7,9)
#define MSM_BOOT_UART_DM_TXFS_ASYNC_STATE(x) MSM_BOOT_UART_DM_EXTR_BITS(x,10,13)

/* UART RX FIFO Status Register */
#define MSM_BOOT_UART_DM_RXFS(base)           ((base) + 0x50)
#define MSM_BOOT_UART_DM_RXFS_STATE_LSB(x)   MSM_BOOT_UART_DM_EXTR_BITS(x,0,6)
#define MSM_BOOT_UART_DM_RXFS_STATE_MSB(x)   MSM_BOOT_UART_DM_EXTR_BITS(x,14,31)
#define MSM_BOOT_UART_DM_RXFS_BUF_STATE(x)   MSM_BOOT_UART_DM_EXTR_BITS(x,7,9)
#define MSM_BOOT_UART_DM_RXFS_ASYNC_STATE(x) MSM_BOOT_UART_DM_EXTR_BITS(x,10,13)

/* Macros for Common Errors */
#define MSM_BOOT_UART_DM_E_SUCCESS           0
#define MSM_BOOT_UART_DM_E_FAILURE           1
#define MSM_BOOT_UART_DM_E_TIMEOUT           2
#define MSM_BOOT_UART_DM_E_INVAL             3
#define MSM_BOOT_UART_DM_E_MALLOC_FAIL       4
#define MSM_BOOT_UART_DM_E_RX_NOT_READY      5

#endif				/* __UART_DM_H__ */

