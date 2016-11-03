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

#include <arch/acpigen.h>
#include <arch/io.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <string.h>
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
	uint32_t slv_data_nak_only;
	uint32_t dma_cr;
	uint32_t dma_tdlr;
	uint32_t dma_rdlr;
	uint32_t sda_setup;
	uint32_t ack_general_call;
	uint32_t enable_status;
	uint32_t fs_spklen;
	uint32_t hs_spklen;
	uint32_t clr_restart_det;
	uint32_t comp_param1;
	uint32_t comp_version;
	uint32_t comp_type;
} __attribute__((packed));

/* Use a ~10ms timeout for various operations */
#define LPSS_I2C_TIMEOUT_US		10000

/* High and low times in different speed modes (in ns) */
enum {
	/* SDA Hold Time */
	DEFAULT_SDA_HOLD_TIME		= 300,
	/* Standard Speed */
	MIN_SS_SCL_HIGHTIME		= 4000,
	MIN_SS_SCL_LOWTIME		= 4700,
	/* Fast Speed */
	MIN_FS_SCL_HIGHTIME		= 600,
	MIN_FS_SCL_LOWTIME		= 1300,
	/* Fast Plus Speed */
	MIN_FP_SCL_HIGHTIME		= 260,
	MIN_FP_SCL_LOWTIME		= 500,
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
		while (read32(&regs->enable_status) & ENABLE_CONTROLLER)
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
	int ret = -1;

	if (count <= 0 || !segments)
		return -1;

	regs = (struct lpss_i2c_regs *)lpss_i2c_base_address(bus);
	if (!regs) {
		printk(BIOS_ERR, "I2C bus %u base address not found\n", bus);
		return -1;
	}

	lpss_i2c_enable(regs);

	if (lpss_i2c_wait_for_bus_idle(regs)) {
		printk(BIOS_ERR, "I2C timeout waiting for bus %u idle\n", bus);
		goto out;
	}

	/* Process each segment */
	while (count--) {
		if (CONFIG_SOC_INTEL_COMMON_LPSS_I2C_DEBUG)
			printk(BIOS_DEBUG, "i2c %u:%02x %s %d bytes : ",
			       bus, segments->chip, segments->read ? "R" : "W",
			       segments->len);

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
				goto out;
			}
		}

		if (CONFIG_SOC_INTEL_COMMON_LPSS_I2C_DEBUG) {
			int j;
			for (j = 0; j < segments->len; j++)
				printk(BIOS_DEBUG, "%02x ", segments->buf[j]);
			printk(BIOS_DEBUG, "\n");
		}

		segments++;
	}

	/* Wait for interrupt status to indicate transfer is complete */
	stopwatch_init_usecs_expire(&sw, LPSS_I2C_TIMEOUT_US);
	while (!(read32(&regs->raw_intr_stat) & INTR_STAT_STOP_DET)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "I2C stop bit not received\n");
			goto out;
		}
	}

	/* Read to clear INTR_STAT_STOP_DET */
	read32(&regs->clear_stop_det_intr);

	/* Wait for the bus to go idle */
	if (lpss_i2c_wait_for_bus_idle(regs)) {
		printk(BIOS_ERR, "I2C timeout waiting for bus %u idle\n", bus);
		goto out;
	}

	/* Flush the RX FIFO in case it is not empty */
	stopwatch_init_usecs_expire(&sw, 16 * LPSS_I2C_TIMEOUT_US);
	while (read32(&regs->status) & STATUS_RX_FIFO_NOT_EMPTY) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "I2C timeout flushing RX FIFO\n");
			goto out;
		}
		read32(&regs->cmd_data);
	}

	ret = 0;

out:
	read32(&regs->clear_intr);
	lpss_i2c_disable(regs);
	return ret;
}

/*
 * Write ACPI object to describe speed configuration.
 *
 * ACPI Object: Name ("xxxx", Package () { scl_lcnt, scl_hcnt, sda_hold }
 *
 * SSCN: I2C_SPEED_STANDARD
 * FMCN: I2C_SPEED_FAST
 * FPCN: I2C_SPEED_FAST_PLUS
 * HSCN: I2C_SPEED_HIGH
 */
static void lpss_i2c_acpi_write_speed_config(
	const struct lpss_i2c_speed_config *config)
{
	if (!config)
		return;
	if (!config->scl_lcnt && !config->scl_hcnt && !config->sda_hold)
		return;

	if (config->speed >= I2C_SPEED_HIGH)
		acpigen_write_name("HSCN");
	else if (config->speed >= I2C_SPEED_FAST_PLUS)
		acpigen_write_name("FPCN");
	else if (config->speed >= I2C_SPEED_FAST)
		acpigen_write_name("FMCN");
	else
		acpigen_write_name("SSCN");

	/* Package () { scl_lcnt, scl_hcnt, sda_hold } */
	acpigen_write_package(3);
	acpigen_write_word(config->scl_hcnt);
	acpigen_write_word(config->scl_lcnt);
	acpigen_write_dword(config->sda_hold);
	acpigen_pop_len();
}

void lpss_i2c_acpi_fill_ssdt(const struct lpss_i2c_speed_config *override)
{
	const struct lpss_i2c_speed_config *sptr;
	struct lpss_i2c_speed_config sgen;
	enum i2c_speed speeds[LPSS_I2C_SPEED_CONFIG_COUNT] = {
		I2C_SPEED_STANDARD,
		I2C_SPEED_FAST,
		I2C_SPEED_FAST_PLUS,
		I2C_SPEED_HIGH,
	};
	int i;

	/* Report timing values for the OS driver */
	for (i = 0; i < LPSS_I2C_SPEED_CONFIG_COUNT; i++) {
		/* Generate speed config for default case */
		if (lpss_i2c_gen_speed_config(speeds[i], &sgen) < 0)
			continue;

		/* Apply board specific override for this speed if found */
		for (sptr = override; sptr && sptr->speed; sptr++) {
			if (sptr->speed == speeds[i]) {
				memcpy(&sgen, sptr, sizeof(sgen));
				break;
			}
		}

		/* Generate ACPI based on selected speed config */
		lpss_i2c_acpi_write_speed_config(&sgen);
	}
}

int lpss_i2c_set_speed_config(unsigned bus,
			      const struct lpss_i2c_speed_config *config)
{
	struct lpss_i2c_regs *regs;
	void *hcnt_reg, *lcnt_reg;

	regs = (struct lpss_i2c_regs *)lpss_i2c_base_address(bus);
	if (!regs || !config)
		return -1;

	/* Nothing to do if no values are set */
	if (!config->scl_lcnt && !config->scl_hcnt && !config->sda_hold)
		return 0;

	if (config->speed >= I2C_SPEED_FAST_PLUS) {
		/* Fast-Plus and High speed */
		hcnt_reg = &regs->hs_scl_hcnt;
		lcnt_reg = &regs->hs_scl_lcnt;
	} else if (config->speed >= I2C_SPEED_FAST) {
		/* Fast speed */
		hcnt_reg = &regs->fs_scl_hcnt;
		lcnt_reg = &regs->fs_scl_lcnt;
	} else {
		/* Standard speed */
		hcnt_reg = &regs->ss_scl_hcnt;
		lcnt_reg = &regs->ss_scl_lcnt;
	}

	/* SCL count must be set after the speed is selected */
	if (config->scl_hcnt)
		write32(hcnt_reg, config->scl_hcnt);
	if (config->scl_lcnt)
		write32(lcnt_reg, config->scl_lcnt);

	/* Set SDA Hold Time register */
	if (config->sda_hold)
		write32(&regs->sda_hold, config->sda_hold);

	return 0;
}

int lpss_i2c_gen_speed_config(enum i2c_speed speed,
			      struct lpss_i2c_speed_config *config)
{
	const int ic_clk = CONFIG_SOC_INTEL_COMMON_LPSS_I2C_CLOCK_MHZ;
	uint16_t hcnt_min, lcnt_min;

	/* Clock must be provided by Kconfig */
	if (!ic_clk || !config)
		return -1;

	if (speed >= I2C_SPEED_HIGH) {
		/* High speed */
		hcnt_min = MIN_HS_SCL_HIGHTIME;
		lcnt_min = MIN_HS_SCL_LOWTIME;
	} else if (speed >= I2C_SPEED_FAST_PLUS) {
		/* Fast-Plus speed */
		hcnt_min = MIN_FP_SCL_HIGHTIME;
		lcnt_min = MIN_FP_SCL_LOWTIME;
	} else if (speed >= I2C_SPEED_FAST) {
		/* Fast speed */
		hcnt_min = MIN_FS_SCL_HIGHTIME;
		lcnt_min = MIN_FS_SCL_LOWTIME;
	} else {
		/* Standard speed */
		hcnt_min = MIN_SS_SCL_HIGHTIME;
		lcnt_min = MIN_SS_SCL_LOWTIME;
	}

	config->speed = speed;
	config->scl_hcnt = ic_clk * hcnt_min / KHz;
	config->scl_lcnt = ic_clk * lcnt_min / KHz;
	config->sda_hold = ic_clk * DEFAULT_SDA_HOLD_TIME / KHz;

	return 0;
}

int lpss_i2c_set_speed(unsigned bus, enum i2c_speed speed)
{
	struct lpss_i2c_regs *regs;
	struct lpss_i2c_speed_config config;
	uint32_t control;

	/* Clock must be provided by Kconfig */
	regs = (struct lpss_i2c_regs *)lpss_i2c_base_address(bus);
	if (!regs || !speed)
		return -1;

	control = read32(&regs->control);
	control &= ~CONTROL_SPEED_MASK;

	if (speed >= I2C_SPEED_FAST_PLUS) {
		/* High and Fast-Plus speed share config registers */
		control |= CONTROL_SPEED_HS;
	} else if (speed >= I2C_SPEED_FAST) {
		/* Fast speed */
		control |= CONTROL_SPEED_FS;
	} else {
		/* Standard speed */
		control |= CONTROL_SPEED_SS;
	}

	/* Generate speed config based on clock */
	if (lpss_i2c_gen_speed_config(speed, &config) < 0)
		return -1;

	/* Select this speed in the control register */
	write32(&regs->control, control);

	/* Write the speed config that was generated earlier */
	lpss_i2c_set_speed_config(bus, &config);

	return 0;
}

int lpss_i2c_init(unsigned bus, enum i2c_speed speed)
{
	struct lpss_i2c_regs *regs;

	regs = (struct lpss_i2c_regs *)lpss_i2c_base_address(bus);
	if (!regs) {
		printk(BIOS_ERR, "I2C bus %u base address not found\n", bus);
		return -1;
	}

	if (lpss_i2c_disable(regs) < 0) {
		printk(BIOS_ERR, "I2C timeout disabling bus %u\n", bus);
		return -1;
	}

	/* Put controller in master mode with restart enabled */
	write32(&regs->control, CONTROL_MASTER_MODE | CONTROL_SLAVE_DISABLE |
		CONTROL_RESTART_ENABLE);

	/* Set bus speed to FAST by default */
	if (lpss_i2c_set_speed(bus, speed ? : I2C_SPEED_FAST) < 0) {
		printk(BIOS_ERR, "I2C failed to set speed for bus %u\n", bus);
		return -1;
	}

	/* Set RX/TX thresholds to smallest values */
	write32(&regs->rx_thresh, 0);
	write32(&regs->tx_thresh, 0);

	/* Enable stop detection interrupt */
	write32(&regs->intr_mask, INTR_STAT_STOP_DET);

	printk(BIOS_INFO, "LPSS I2C bus %u at 0x%p (%u KHz)\n",
	       bus, regs, (speed ? : I2C_SPEED_FAST) / KHz);

	return 0;
}
