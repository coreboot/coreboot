/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/bcd.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/i2c_bus.h>
#include <timer.h>
#include <types.h>
#include <version.h>
#include "chip.h"
#include "rv3028c7.h"

static enum cb_err rtc_eep_wait_ready(struct device *dev)
{
	struct stopwatch sw;
	uint8_t status;

	stopwatch_init_msecs_expire(&sw, EEP_SYNC_TIMEOUT_MS);
	do {
		status = (uint8_t)i2c_dev_readb_at(dev, STATUS_REG);
		mdelay(1);
	} while ((status & EE_BUSY_BIT) && !stopwatch_expired(&sw));

	if (status & EE_BUSY_BIT) {
		return CB_ERR;
	} else {
		return CB_SUCCESS;
	}
}

static enum cb_err rtc_eep_auto_refresh(struct device *dev, uint8_t state)
{
	uint8_t reg;

	reg = (uint8_t)i2c_dev_readb_at(dev, CTRL1_REG);
	reg &= ~EERD_BIT;
	if (state == EEP_REFRESH_DIS)
		reg |= EERD_BIT;
	i2c_dev_writeb_at(dev, CTRL1_REG, reg);
	/* Wait until the EEPROM has finished a possible running operation. */
	if (rtc_eep_wait_ready(dev) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s: EEPROM access timed out (%d ms)!\n",
			dev->chip_ops->name, EEP_SYNC_TIMEOUT_MS);
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static enum cb_err rtc_eep_start_update(struct device *dev)
{
	/* Disable EEPROM auto refresh before writing RAM to EEPROM
	   to avoid race conditions. */
	if (rtc_eep_auto_refresh(dev, EEP_REFRESH_DIS))
		return CB_ERR;

	/* Now start the update cycle.*/
	i2c_dev_writeb_at(dev, EEP_CMD_REG, EEP_CMD_PREFIX);
	i2c_dev_writeb_at(dev, EEP_CMD_REG, EEP_CMD_UPDATE);
	return CB_SUCCESS;
}

static void rtc_set_time_date(struct device *dev)
{
	struct drivers_i2c_rv3028c7_config *config = dev->chip_info;
	uint8_t buf[7];

	/* The buffer contains the seconds through years of the new time and date.
	   Whenever a new date is set, the time is set to 00:00:00. */
	buf[0] = 0;	/* Entry for seconds. */
	buf[1] = 0;	/* Entry for minutes. */
	buf[2] = 0;	/* Entry for hours. */
	if (config->set_user_date) {
		buf[3] = config->user_weekday;
		buf[4] = bin2bcd(config->user_day);
		buf[5] = bin2bcd(config->user_month);
		buf[6] = bin2bcd(config->user_year);
		printk(BIOS_DEBUG, "%s: Set to user date\n", dev->chip_ops->name);
	} else {
		buf[3] = coreboot_build_date.weekday;
		buf[4] = coreboot_build_date.day;
		buf[5] = coreboot_build_date.month;
		buf[6] = coreboot_build_date.year;
		printk(BIOS_DEBUG, "%s: Set to coreboot build date\n", dev->chip_ops->name);
	}
	/* According to the datasheet, date and time should be transferred in "one go"
	   in order to avoid value corruption. */
	if (i2c_dev_write_at(dev, buf, sizeof(buf), 0) != sizeof(buf)) {
		printk(BIOS_ERR, "%s: Not able to set date and time!\n", dev->chip_ops->name);
	}
}

static void rtc_final(struct device *dev)
{
	uint8_t buf[7];

	/* Read back current RTC date and time and print it to the console.
	   Date and time are read in "one go", the buffer contains seconds (byte 0)
	   through years (byte 6) after this read. */
	if (i2c_dev_read_at(dev, buf, sizeof(buf), 0) != sizeof(buf)) {
		printk(BIOS_ERR, "%s: Not able to read current date and time!\n",
			dev->chip_ops->name);
	} else {
		printk(BIOS_INFO, "%s: Current date %02d.%02d.%02d %02d:%02d:%02d\n",
				dev->chip_ops->name, bcd2bin(buf[5]), bcd2bin(buf[4]),
				bcd2bin(buf[6]), bcd2bin(buf[2]), bcd2bin(buf[1]),
				bcd2bin(buf[0]));
	}
	/* Make sure the EEPROM automatic refresh is enabled. */
	if (rtc_eep_auto_refresh(dev, EEP_REFRESH_EN) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s: Not able to enable EEPROM auto refresh!\n",
			dev->chip_ops->name);
	}
}

static void rtc_init(struct device *dev)
{
	struct drivers_i2c_rv3028c7_config *config = dev->chip_info;
	uint8_t reg, backup_reg, eep_update_needed = 0;

	/* On every startup, the RTC synchronizes the internal EEPROM with RAM.
	 * During this time no operation shall modify RAM registers. Ensure this
	 * sync is finished before starting the initialization. */
	if (rtc_eep_wait_ready(dev) != CB_SUCCESS) {
		printk(BIOS_WARNING, "%s: Timeout on EEPROM sync after power on!\n",
			dev->chip_ops->name);
		return;
	}
	reg = backup_reg = (uint8_t)i2c_dev_readb_at(dev, EEP_BACKUP_REG);
	/* Configure the switch-over setting according to devicetree. */
	if (config->bckup_sw_mode) {
		reg &= ~BSM_MASK;
		reg |= config->bckup_sw_mode << BSM_BIT;
	}
	/* Configure the VBACKUP charging mode. */
	if (config->cap_charge) {
		reg &= ~TCR_MASK;
		reg |= ((config->cap_charge - 1) << TCR_BIT);
		reg |= TCE_BIT;
	} else {
		reg &= ~TCE_BIT;
	}
	/* According to the datasheet the Fast Edge Detection Enable (FEDE) bit
	   should always be set. */
	reg |= FEDE_BIT;
	if (reg != backup_reg) {
		/* Write new register value into shadow RAM and request an EEPROM update. */
		i2c_dev_writeb_at(dev, EEP_BACKUP_REG, reg);
		eep_update_needed = 1;
	}
	/* Make sure the hour register is in 24h format.*/
	reg = (uint8_t)i2c_dev_readb_at(dev, CTRL2_REG);
	if (reg & HOUR_12_24_BIT) {
		reg &= ~HOUR_12_24_BIT;
		i2c_dev_writeb_at(dev, CTRL2_REG, reg);
	}
	/* Check for a possible voltage drop event. */
	reg = (uint8_t)i2c_dev_readb_at(dev, STATUS_REG);
	if (reg & PORF_BIT) {
		/* Voltage drop was detected, date and time needs to be set properly. */
		rtc_set_time_date(dev);
		/* Clear the PORF bit to mark that the event was handled. */
		reg &= ~PORF_BIT;
		i2c_dev_writeb_at(dev, STATUS_REG, reg);
	}
	/*
	 * Finally, trigger the EEPROM update procedure if needed.
	 * According to the datasheet, this update will consume ~63 ms.
	 * In order to not block the boot process here waiting for this update being finished,
	 * trigger the update now and check for readiness in the final hook.
	 */
	if (eep_update_needed && rtc_eep_start_update(dev) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s: Not able to trigger EEPROM update!\n",
			dev->chip_ops->name);
	}
}

static struct device_operations rv3028c7_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= rtc_init,
	.final			= rtc_final,
};

static void rtc_enable(struct device *dev)
{
	dev->ops = &rv3028c7_ops;
}

struct chip_operations drivers_i2c_rv3028c7_ops = {
	CHIP_NAME("RV-3028-C7")
	.enable_dev = rtc_enable
};
