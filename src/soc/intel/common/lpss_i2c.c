/*
 * This file is part of the coreboot project.
 *
 * Copyright 2009 Vipin Kumar, ST Microelectronics
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <timer.h>
#include "lpss_i2c.h"

struct lpss_i2c_regs {
	uint32_t control;
	uint32_t target_addr;
	uint32_t slave_addr;
	uint32_t master_addr;
	uint32_t cmd_data;
	uint32_t ss_scl_hcnt;
	uint32_t ss_scl_lcnt;
	uint32_t fs_scl_hcnt;
	uint32_t fs_scl_lcnt;
	uint32_t hs_scl_hcnt;
	uint32_t hs_scl_lcnt;
	uint32_t intr_stat;
	uint32_t intr_mask;
	uint32_t raw_intr_stat;
	uint32_t rx_thresh;
	uint32_t tx_thresh;
	uint32_t clear_intr;
	uint32_t clear_rx_under_intr;
	uint32_t clear_rx_over_intr;
	uint32_t clear_tx_over_intr;
	uint32_t clear_rd_req_intr;
	uint32_t clear_tx_abrt_intr;
	uint32_t clear_rx_done_intr;
	uint32_t clear_activity_intr;
	uint32_t clear_stop_det_intr;
	uint32_t clear_start_det_intr;
	uint32_t clear_gen_call_intr;
	uint32_t enable;
	uint32_t status;
	uint32_t tx_level;
	uint32_t rx_level;
	uint32_t sda_hold;
	uint32_t tx_abort_source;
} __attribute__((packed));

/* Use a ~2ms timeout for various operations */
#define LPSS_I2C_TIMEOUT_US		2000

/* High and low times in different speed modes (in ns) */
enum {
	/* Standard Speed */
	MIN_SS_SCL_HIGHTIME		= 4000,
	MIN_SS_SCL_LOWTIME		= 4700,
	/* Fast/Fast+ Speed */
	MIN_FS_SCL_HIGHTIME		= 600,
	MIN_FS_SCL_LOWTIME		= 1300,
	/* High Speed */
	MIN_HS_SCL_HIGHTIME		= 60,
	MIN_HS_SCL_LOWTIME		= 160,
};

/* Control register definitions */
enum {
	CONTROL_MASTER_MODE		= (1 << 0),
	CONTROL_SPEED_SS		= (1 << 1),
	CONTROL_SPEED_FS		= (1 << 2),
	CONTROL_SPEED_HS		= (3 << 1),
	CONTROL_SPEED_MASK		= (3 << 1),
	CONTROL_10BIT_SLAVE		= (1 << 3),
	CONTROL_10BIT_MASTER		= (1 << 4),
	CONTROL_RESTART_ENABLE		= (1 << 5),
	CONTROL_SLAVE_DISABLE		= (1 << 6),
};

/* Command/Data register definitions */
enum {
	CMD_DATA_CMD			= (1 << 8),
	CMD_DATA_STOP			= (1 << 9),
};

/* Status register definitions */
enum {
	STATUS_ACTIVITY			= (1 << 0),
	STATUS_TX_FIFO_NOT_FULL		= (1 << 1),
	STATUS_TX_FIFO_EMPTY		= (1 << 2),
	STATUS_RX_FIFO_NOT_EMPTY	= (1 << 3),
	STATUS_RX_FIFO_FULL		= (1 << 4),
	STATUS_MASTER_ACTIVITY		= (1 << 5),
	STATUS_SLAVE_ACTIVITY		= (1 << 6),
};

/* Enable register definitions */
enum {
	ENABLE_CONTROLLER		= (1 << 0),
};

/* Interrupt status register definitions */
enum {
	INTR_STAT_RX_UNDER		= (1 << 0),
	INTR_STAT_RX_OVER		= (1 << 1),
	INTR_STAT_RX_FULL		= (1 << 2),
	INTR_STAT_TX_OVER		= (1 << 3),
	INTR_STAT_TX_EMPTY		= (1 << 4),
	INTR_STAT_RD_REQ		= (1 << 5),
	INTR_STAT_TX_ABORT		= (1 << 6),
	INTR_STAT_RX_DONE		= (1 << 7),
	INTR_STAT_ACTIVITY		= (1 << 8),
	INTR_STAT_STOP_DET		= (1 << 9),
	INTR_STAT_START_DET		= (1 << 10),
	INTR_STAT_GEN_CALL		= (1 << 11),
};

/* Enable this I2C controller */
static void lpss_i2c_enable(struct lpss_i2c_regs *regs)
{
	uint32_t enable = read32(&regs->enable);

	if (!(enable & ENABLE_CONTROLLER))
		write32(&regs->enable, enable | ENABLE_CONTROLLER);
}

/* Disable this I2C controller */
static int lpss_i2c_disable(struct lpss_i2c_regs *regs)
{
	uint32_t enable = read32(&regs->enable);

	if (enable & ENABLE_CONTROLLER) {
		struct stopwatch sw;

		write32(&regs->enable, enable & ~ENABLE_CONTROLLER);

		/* Wait for enable bit to clear */
		stopwatch_init_usecs_expire(&sw, LPSS_I2C_TIMEOUT_US);
		while (read32(&regs->enable) & ENABLE_CONTROLLER)
			if (stopwatch_expired(&sw))
				return -1;
	}

	return 0;
}

/* Wait for this I2C controller to go idle for transmit */
static int lpss_i2c_wait_for_bus_idle(struct lpss_i2c_regs *regs)
{
	struct stopwatch sw;

	/* Start timeout for up to 16 bytes in FIFO */
	stopwatch_init_usecs_expire(&sw, 16 * LPSS_I2C_TIMEOUT_US);

	while (!stopwatch_expired(&sw)) {
		uint32_t status = read32(&regs->status);

		/* Check for master activity and keep waiting */
		if (status & STATUS_MASTER_ACTIVITY)
			continue;

		/* Check for TX FIFO empty to indicate TX idle */
		if (status & STATUS_TX_FIFO_EMPTY)
			return 0;
	}

	/* Timed out while waiting for bus to go idle */
	return -1;
}

/* Transfer one byte of one segment, sending stop bit if requested */
static int lpss_i2c_transfer_byte(struct lpss_i2c_regs *regs,
				  struct i2c_seg *segment,
				  size_t byte, int send_stop)
{
	struct stopwatch sw;
	uint32_t cmd = CMD_DATA_CMD; /* Read op */

	stopwatch_init_usecs_expire(&sw, LPSS_I2C_TIMEOUT_US);

	if (!segment->read) {
		/* Write op only: Wait for FIFO not full */
		while (!(read32(&regs->status) & STATUS_TX_FIFO_NOT_FULL)) {
			if (stopwatch_expired(&sw)) {
				printk(BIOS_ERR, "I2C transmit timeout\n");
				return -1;
			}
		}
		cmd = segment->buf[byte];
	}

	/* Send stop on last byte, if desired */
	if (send_stop && byte == segment->len - 1)
		cmd |= CMD_DATA_STOP;

	write32(&regs->cmd_data, cmd);

	if (segment->read) {
		/* Read op only: Wait for FIFO data and store it */
		while (!(read32(&regs->status) & STATUS_RX_FIFO_NOT_EMPTY)) {
			if (stopwatch_expired(&sw)) {
				printk(BIOS_ERR, "I2C receive timeout\n");
				return -1;
			}
		}
		segment->buf[byte] = read32(&regs->cmd_data);
	}

	return 0;
}

/* Global I2C bus handler, defined in include/i2c.h */
int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int count)
{
	struct stopwatch sw;
	struct lpss_i2c_regs *regs;
	size_t byte;

	if (count <= 0 || !segments)
		return -1;

	regs = (struct lpss_i2c_regs *)lpss_i2c_base_address(bus);
	if (!regs) {
		printk(BIOS_ERR, "I2C bus %u base address not found\n", bus);
		return -1;
	}

	if (!(read32(&regs->enable) & ENABLE_CONTROLLER)) {
		printk(BIOS_ERR, "I2C bus %u not initialized\n", bus);
		return -1;
	}

	if (lpss_i2c_wait_for_bus_idle(regs)) {
		printk(BIOS_ERR, "I2C timeout waiting for bus %u idle\n", bus);
		return -1;
	}

	/* Process each segment */
	while (count--) {
		/* Set target slave address */
		write32(&regs->target_addr, segments->chip);

		/* Read or write each byte in segment */
		for (byte = 0; byte < segments->len; byte++) {
			/*
			 * Set stop condition on final segment only.
			 * Repeated start will be automatically generated
			 * by the controller on R->W or W->R switch.
			 */
			if (lpss_i2c_transfer_byte(regs, segments, byte,
						   count == 0) < 0) {
				printk(BIOS_ERR, "I2C %s failed: bus %u "
				       "addr 0x%02x\n", segments->read ?
				       "read" : "write", bus, segments->chip);
				return -1;
			}
		}
		segments++;
	}

	/* Wait for interrupt status to indicate transfer is complete */
	stopwatch_init_usecs_expire(&sw, LPSS_I2C_TIMEOUT_US);
	while (!(read32(&regs->raw_intr_stat) & INTR_STAT_STOP_DET)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "I2C stop bit not received\n");
			return -1;
		}
	}

	/* Read to clear INTR_STAT_STOP_DET */
	read32(&regs->clear_stop_det_intr);

	/* Wait for the bus to go idle */
	if (lpss_i2c_wait_for_bus_idle(regs)) {
		printk(BIOS_ERR, "I2C timeout waiting for bus %u idle\n", bus);
		return -1;
	}

	/* Flush the RX FIFO in case it is not empty */
	stopwatch_init_usecs_expire(&sw, 16 * LPSS_I2C_TIMEOUT_US);
	while (read32(&regs->status) & STATUS_RX_FIFO_NOT_EMPTY) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "I2C timeout flushing RX FIFO\n");
			return -1;
		}
		read32(&regs->cmd_data);
	}

	return 0;
}

static void lpss_i2c_set_speed(struct lpss_i2c_regs *regs, enum i2c_speed speed)
{
	const int ic_clk = CONFIG_SOC_INTEL_COMMON_LPSS_I2C_CLOCK_MHZ;
	uint32_t control, hcnt_min, lcnt_min;
	void *hcnt_reg, *lcnt_reg;

	/* Clock must be provided by Kconfig */
	if (!ic_clk || !speed)
		return;

	control = read32(&regs->control);
	control &= ~CONTROL_SPEED_MASK;

	if (speed >= I2C_SPEED_HIGH) {
		/* High Speed */
		control |= CONTROL_SPEED_HS;
		hcnt_reg = &regs->hs_scl_hcnt;
		lcnt_reg = &regs->hs_scl_lcnt;
		hcnt_min = MIN_HS_SCL_HIGHTIME;
		lcnt_min = MIN_HS_SCL_LOWTIME;
	} else if (speed >= I2C_SPEED_FAST) {
		/* Fast Speed */
		control |= CONTROL_SPEED_FS;
		hcnt_reg = &regs->fs_scl_hcnt;
		lcnt_reg = &regs->fs_scl_lcnt;
		hcnt_min = MIN_FS_SCL_HIGHTIME;
		lcnt_min = MIN_FS_SCL_LOWTIME;
	} else {
		/* Standard Speed */
		control |= CONTROL_SPEED_SS;
		hcnt_reg = &regs->ss_scl_hcnt;
		lcnt_reg = &regs->ss_scl_lcnt;
		hcnt_min = MIN_SS_SCL_HIGHTIME;
		lcnt_min = MIN_SS_SCL_LOWTIME;
	}

	/* Select this speed in the control register */
	write32(&regs->control, control);

	/* SCL count must be set after the speed is selected */
	write32(hcnt_reg, ic_clk * hcnt_min / KHz);
	write32(lcnt_reg, ic_clk * lcnt_min / KHz);
}

void lpss_i2c_init(unsigned bus, enum i2c_speed speed)
{
	struct lpss_i2c_regs *regs;

	regs = (struct lpss_i2c_regs *)lpss_i2c_base_address(bus);
	if (!regs) {
		printk(BIOS_ERR, "I2C bus %u base address not found\n", bus);
		return;
	}

	if (lpss_i2c_disable(regs) < 0) {
		printk(BIOS_ERR, "I2C timeout disabling bus %u\n", bus);
		return;
	}

	/* Put controller in master mode with restart enabled */
	write32(&regs->control, CONTROL_MASTER_MODE | CONTROL_SLAVE_DISABLE |
		CONTROL_RESTART_ENABLE);

	/* Set bus speed to FAST by default */
	lpss_i2c_set_speed(regs, speed ? : I2C_SPEED_FAST);

	/* Set RX/TX thresholds to smallest values */
	write32(&regs->rx_thresh, 0);
	write32(&regs->tx_thresh, 0);

	/* Enable stop detection interrupt */
	write32(&regs->intr_mask, INTR_STAT_STOP_DET);

	lpss_i2c_enable(regs);

	printk(BIOS_INFO, "LPSS I2C bus %u at 0x%p (%u KHz)\n",
	       bus, regs, (speed ? : I2C_SPEED_FAST) / KHz);
}
