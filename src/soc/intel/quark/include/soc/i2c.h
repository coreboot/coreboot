/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _QUARK_I2C_H_
#define _QUARK_I2C_H_

#include <stdint.h>

typedef volatile struct _I2C_REGS {
	volatile uint32_t ic_con;  /* 00: Control Register */
	volatile uint32_t ic_tar;  /* 04: Master Target Address */
	uint32_t reserved_08[2];
	volatile uint32_t ic_data_cmd;    /* 10: Data Buffer & Command */
	uint32_t ic_ss_scl_hcnt;   /* 14: Standard Speed Clock SCL High Count */
	uint32_t ic_ss_scl_lcnt;   /* 18: Standard Speed Clock SCL Low Count */
	uint32_t ic_fs_scl_hcnt;   /* 1c: Fast Speed Clock SCL High Count */
	uint32_t ic_fs_scl_lcnt;   /* 20: Fast Speed Clock SCL Low Count */
	uint32_t reserved_24[2];
	volatile uint32_t ic_intr_stat;   /* 2c: Interrupt Status */
	uint32_t ic_intr_mask;     /* 30: Interrupt Mask */
	uint32_t ic_raw_intr_stat; /* 34: Raw Interrupt Status */
	uint32_t ic_rx_tl;         /* 38: Receive FIFO Threshold Level */
	uint32_t ic_tx_tl;         /* 3c: Transmit FIFO Threshold Level */
	uint32_t ic_clr_intr;      /* 40: Clear Combined/Individual Interrupt */
	uint32_t ic_clr_rx_under;  /* 44: Clear RX_UNDER Interrupt */
	uint32_t ic_clr_rx_over;   /* 48: Clear RX_OVER Interrupt */
	uint32_t ic_clr_tx_over;   /* 4c: Clear TX_OVER Interrupt */
	uint32_t ic_clr_rd_req;    /* 50: Clear RD_REQ Interrupt */
	uint32_t ic_clr_tx_abrt;   /* 54: Clear TX_ABRT Interrupt */
	uint32_t reserved_58;
	uint32_t ic_clr_activity;  /* 5c: Clear ACTIVITY Interrupt */
	uint32_t ic_clr_stop_det;  /* 60: Clear STOP_DET Interrupt */
	uint32_t ic_clr_start_det; /* 64: Clear START_DET Interrupt */
	uint32_t reserved_68;
	volatile uint32_t ic_enable;            /* 6c: Enable */
	volatile uint32_t ic_status;            /* 70: Status */
	uint32_t ic_txflr;                      /* 74: Transmit FIFO Level */
	uint32_t ic_rxflr;                      /* 78: Receive FIFO Level */
	uint32_t ic_sda_hold;                   /* 7c: SDA Hold */
	volatile uint32_t ic_tx_abrt_source;    /* 80: Transmit Abort Source */
	uint32_t reserved_84[6];
	volatile uint32_t ic_enable_status;     /* 9c: Enable Status */
	uint32_t ic_fs_spklen;     /* a0: SS and FS Spike Suppression Limit */
} I2C_REGS;

#define IC_CON                  offsetof(I2C_REGS, ic_con)
#define IC_TAR                  offsetof(I2C_REGS, ic_tar)
#define IC_DATA_CMD             offsetof(I2C_REGS, ic_data_cmd)
#define IC_SS_SCL_HCNT          offsetof(I2C_REGS, ic_ss_scl_hcnt)
#define IC_SS_SCL_LCNT          offsetof(I2C_REGS, ic_ss_scl_lcnt)
#define IC_FS_SCL_HCNT          offsetof(I2C_REGS, ic_fs_scl_hcnt)
#define IC_FS_SCL_LCNT          offsetof(I2C_REGS, ic_fs_scl_lcnt)
#define IC_INTR_STAT            offsetof(I2C_REGS, ic_intr_stat)
#define IC_INTR_MASK            offsetof(I2C_REGS, ic_intr_mask)
#define IC_RAW_INTR_STAT        offsetof(I2C_REGS, ic_raw_intr_stat)
#define IC_RX_TL                offsetof(I2C_REGS, ic_rx_tl)
#define IC_TX_TL                offsetof(I2C_REGS, ic_tx_tl)
#define IC_CLR_INTR             offsetof(I2C_REGS, ic_clr_intr)
#define IC_CLR_RX_UNDER         offsetof(I2C_REGS, ic_clr_rx_under)
#define IC_CLR_RX_OVER          offsetof(I2C_REGS, ic_clr_rx_over)
#define IC_CLR_TX_OVER          offsetof(I2C_REGS, ic_clr_tx_over)
#define IC_CLR_RD_REQ           offsetof(I2C_REGS, ic_clr_rd_req)
#define IC_CLR_TX_ABRT          offsetof(I2C_REGS, ic_clr_tx_abrt)
#define IC_CLR_ACTIVITY         offsetof(I2C_REGS, ic_clr_activity)
#define IC_CLR_STOP_DET         offsetof(I2C_REGS, ic_clr_stop_det)
#define IC_CLR_START_DET        offsetof(I2C_REGS, ic_clr_start_det)
#define IC_ENABLE               offsetof(I2C_REGS, ic_enable)
#define IC_STATUS               offsetof(I2C_REGS, ic_status)
#define IC_TXFLR                offsetof(I2C_REGS, ic_txflr)
#define IC_RXFLR                offsetof(I2C_REGS, ic_rxflr)
#define IC_SDA_HOLD             offsetof(I2C_REGS, ic_sda_hold)
#define IC_TX_ABRT_SOURCE       offsetof(I2C_REGS, ic_tx_abrt_source)
#define IC_ENABLE_STATUS        offsetof(I2C_REGS, ic_enable_status)
#define IC_FS_SPKLEN            offsetof(I2C_REGS, ic_fs_spklen)

/* 0x00: ic_con */
#define IC_CON_RESTART_EN       0x00000020 /* Enable start/restart */
#define IC_CON_10B              0x00000010 /* 10-bit addressing */
#define IC_CON_7B               0          /* 7-bit addressing */
#define IC_CON_SPEED            0x00000006 /* I2C bus speed */
#define IC_CON_SPEED_400_KHz    0x00000004 /* Fast mode */
#define IC_CON_SPEED_100_KHz    0x00000002 /* Standard mode */
#define IC_CON_MASTER_MODE      0x00000001 /* Enable master mode */

/* 0x10: ic_data_cmd */
#define IC_DATA_CMD_RESTART     0x00000400 /* Send restart before byte */
#define IC_DATA_CMD_STOP        0x00000200 /* Send stop after byte */
#define IC_DATA_CMD_CMD         0x00000100 /* Type of transaction */
#define IC_DATA_CMD_READ        IC_DATA_CMD_CMD
#define IC_DATA_CMD_WRITE       0
#define IC_DATA_CMD_DATA        0x000000ff /* Data byte */

/* 0x2c: ic_intr_stat
 * 0x30: ic_intr_mask
 * 0x34: ic_raw_intr_stat
 */
#define IC_INTR_START_DET       0x00000400 /* Start bit detected */
#define IC_INTR_STOP_DET        0x00000200 /* Stop bit detected */
#define IC_INTR_ACTIVITY        0x00000100 /* Activity detected */
#define IC_INTR_TX_ABRT         0x00000040 /* Transmit abort */
#define IC_INTR_RD_REQ          0x00000020 /* Read request */
#define IC_INTR_TX_EMPTY        0x00000010 /* TX FIFO is empty */
#define IC_INTR_TX_OVER         0x00000008 /* TX FIFO overflow */
#define IC_INTR_RX_FULL         0x00000004 /* Receive FIFO is full */
#define IC_INTR_RX_OVER         0x00000002 /* Receive FIFO overflow */
#define IC_INTR_RX_UNDER        0x00000001 /* Receive FIFO underflow */

/* 0x6c: ic_enable
 * 0x9c: ic_enable_status
 */
#define IC_ENABLE_CONTROLLER    0x00000001 /* Enable the I2C controller */

/* 0x70: ic_status */
#define IC_STATUS_MST_ACTIVITY  0x00000020 /* Master FSM activity */
#define IC_STATUS_RFF           0x00000010 /* Receive FIFO completely full */
#define IC_STATUS_RFNE          0x00000008 /* Receive FIFO not empty */
#define IC_STATUS_TFE           0x00000004 /* Transmit FIFO completely empty */
#define IC_STATUS_TFNF          0x00000002 /* Transmit FIFO not full */
#define IC_STATUS_ACTIVITY      0x00000001 /* Activity */

#endif /* _QUARK_I2C_H_ */
