/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <commonlib/bsd/bcd.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/i2c_bus.h>
#include <timer.h>
#include <version.h>
#include "chip.h"
#include "rx6110sa.h"

/* Function to write a register in the RTC with the given value. */
static void rx6110sa_write(struct device *dev, uint8_t reg, uint8_t val)
{
	i2c_dev_writeb_at(dev, reg, val);
}

/* Function to read a register in the RTC. */
static uint8_t rx6110sa_read(struct device *dev, uint8_t reg)
{
	return (uint8_t)i2c_dev_readb_at(dev, reg);
}

/* Set RTC date from coreboot build date. */
static void rx6110sa_set_build_date(struct device *dev)
{
	rx6110sa_write(dev, YEAR_REG, coreboot_build_date.year);
	rx6110sa_write(dev, MONTH_REG, coreboot_build_date.month);
	rx6110sa_write(dev, DAY_REG, coreboot_build_date.day);
	rx6110sa_write(dev, WEEK_REG, (1 << coreboot_build_date.weekday));
}

/* Set RTC date from user defined date (available in e.g. device tree). */
static void rx6110sa_set_user_date(struct device *dev)
{
	struct drivers_i2c_rx6110sa_config *config = dev->chip_info;

	rx6110sa_write(dev, YEAR_REG, bin2bcd(config->user_year));
	rx6110sa_write(dev, MONTH_REG, bin2bcd(config->user_month));
	rx6110sa_write(dev, DAY_REG, bin2bcd(config->user_day));
	rx6110sa_write(dev, WEEK_REG, (1 << config->user_weekday));
}

static void rx6110sa_final(struct device *dev)
{
	uint8_t hour, minute, second, year, month, day;

	/* Read back current RTC date and time and print it to the console. */
	hour = rx6110sa_read(dev, HOUR_REG);
	minute = rx6110sa_read(dev, MINUTE_REG);
	second = rx6110sa_read(dev, SECOND_REG);
	year = rx6110sa_read(dev, YEAR_REG);
	month = rx6110sa_read(dev, MONTH_REG);
	day = rx6110sa_read(dev, DAY_REG);

	printk(BIOS_INFO, "%s: Current date %02d.%02d.%02d %02d:%02d:%02d\n",
		dev->chip_ops->name, bcd2bin(month), bcd2bin(day),
		bcd2bin(year), bcd2bin(hour), bcd2bin(minute), bcd2bin(second));
}

static void rx6110sa_init(struct device *dev)
{
	struct drivers_i2c_rx6110sa_config *config = dev->chip_info;
	uint8_t reg, flags;
	struct stopwatch sw;

	/* Do a dummy read first as requested in the datasheet. */
	rx6110sa_read(dev, SECOND_REG);
	/* Check power loss status by reading the VLF-bit. */
	flags = rx6110sa_read(dev, FLAG_REGISTER);
	if (flags & VLF_BIT) {
		/*
		 * Voltage low detected, perform RX6110 SA reset sequence as
		 * requested in the datasheet. The meaning of the registers 0x60
		 * and above is not documented in the datasheet, they have to be
		 * used as requested according to Epson.
		 */
		rx6110sa_write(dev, BATTERY_BACKUP_REG, 0x00);
		rx6110sa_write(dev, CTRL_REG, 0x00);
		rx6110sa_write(dev, CTRL_REG, TEST_BIT);
		rx6110sa_write(dev, 0x60, 0xd3);
		rx6110sa_write(dev, 0x66, 0x03);
		rx6110sa_write(dev, 0x6b, 0x02);
		rx6110sa_write(dev, 0x6b, 0x01);
		/* According to the datasheet one have to wait for at least 2 ms
		 * before the VLF bit can be cleared in the flag register after
		 * this reset sequence. As the other registers are still
		 * accessible use the stopwatch to parallel the flow.
		 */
		stopwatch_init_msecs_expire(&sw, AFTER_RESET_DELAY_MS);
	}
	/*
	 * Set up important registers even if there was no power loss to make
	 * sure that the right mode is used as it directly influences the
	 * backup current consumption and therefore the backup time. These
	 * settings do not change current date and time and the RTC will not
	 * be stopped while the registers are set up.
	 */
	reg = (config->pmon_sampling & PMON_SAMPL_MASK) |
		(!!config->bks_off << 2) | (!!config->bks_on << 3) |
		(!!config->iocut_en << 4);
	rx6110sa_write(dev, BATTERY_BACKUP_REG, reg);

	/* Clear timer enable bit and set frequency of clock output. */
	reg = rx6110sa_read(dev, EXTENSION_REG);
	reg &= ~(FSEL_MASK);
	reg |= ((config->cof_selection << 6) & FSEL_MASK);
	if (config->timer_preset) {
		/* Timer needs to be in stop mode prior to programming it. */
		if (reg & TE_BIT) {
			reg &= ~TE_BIT;
			rx6110sa_write(dev, EXTENSION_REG, reg);
		}
		/* Program the timer preset value. */
		rx6110sa_write(dev, TMR_COUNTER_0_REG,
				config->timer_preset & 0xff);
		rx6110sa_write(dev, TMR_COUNTER_1_REG,
				(config->timer_preset >> 8) & 0xff);
		/* Set Timer Enable bit and the timer clock value. */
		reg &= ~TSEL_MASK;
		reg |= ((!!config->timer_en << 4) |
			(config->timer_clk & TSEL_MASK));
	}
	rx6110sa_write(dev, EXTENSION_REG, reg);
	rx6110sa_write(dev, CTRL_REG, 0x00);
	rx6110sa_write(dev, DIGITAL_REG, 0x00);
	rx6110sa_write(dev, RESERVED_BIT_REG, RTC_INIT_VALUE);
	reg = (!!config->enable_1hz_out << 4) |
		(!!config->irq_output_pin << 2) |
		(config->fout_output_pin & FOUT_OUTPUT_PIN_MASK);
	rx6110sa_write(dev, IRQ_CONTROL_REG, reg);
	/* If there was no power loss event no further steps are needed. */
	if (!(flags & VLF_BIT))
		return;
	/* There was a power loss event, clear voltage low detect bit.
	 * Take the needed delay after a reset sequence into account before the
	 * VLF-bit can be cleared.
	 */
	stopwatch_wait_until_expired(&sw);
	flags &= ~VLF_BIT;
	rx6110sa_write(dev, FLAG_REGISTER, flags);

	/* Before setting the clock stop oscillator. */
	rx6110sa_write(dev, CTRL_REG, STOP_BIT);

	if (config->set_user_date) {
		/* Set user date defined in device tree. */
		printk(BIOS_DEBUG, "%s: Set to user date\n",
				dev->chip_ops->name);
		rx6110sa_set_user_date(dev);
	} else {
		/* Set date from coreboot build. */
		printk(BIOS_DEBUG, "%s: Set to coreboot build date\n",
				dev->chip_ops->name);
		rx6110sa_set_build_date(dev);
	}
	rx6110sa_write(dev, HOUR_REG, 1);
	rx6110sa_write(dev, MINUTE_REG, 0);
	rx6110sa_write(dev, SECOND_REG, 0);
	/* Start oscillator again as the RTC is set up now. */
	reg = (!!config->timer_irq_en << 4) |
		(config->timer_mode & TMR_MODE_MASK);
	rx6110sa_write(dev, CTRL_REG, reg);
}

#if CONFIG(HAVE_ACPI_TABLES) && !CONFIG(RX6110SA_DISABLE_ACPI)
static void rx6110sa_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_rx6110sa_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	enum i2c_speed bus_speed;

	if (!scope)
		return;

	switch (config->bus_speed) {
	case I2C_SPEED_STANDARD:
	case I2C_SPEED_FAST:
		bus_speed = config->bus_speed;
		break;
	default:
		printk(BIOS_INFO, "%s: Bus speed unsupported, fall back to %d kHz!\n",
			dev->chip_ops->name, I2C_SPEED_STANDARD / 1000);
		bus_speed = I2C_SPEED_STANDARD;
		break;
	}

	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = bus_speed,
		.resource = scope,
	};

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", RX6110SA_HID_NAME);
	acpigen_write_name_string("_DDN", RX6110SA_HID_DESC);
	acpigen_write_STA(acpi_device_status(dev));

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);

	acpigen_write_resourcetemplate_footer();

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev),
			dev->chip_ops->name, dev_path(dev));
}

static const char *rx6110sa_acpi_name(const struct device *dev)
{
	return RX6110SA_ACPI_NAME;
}
#endif

static struct device_operations rx6110sa_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= rx6110sa_init,
	.final			= rx6110sa_final,
#if CONFIG(HAVE_ACPI_TABLES) && !CONFIG(RX6110SA_DISABLE_ACPI)
	.acpi_name		= rx6110sa_acpi_name,
	.acpi_fill_ssdt		= rx6110sa_fill_ssdt,
#endif
};

static void rx6110sa_enable(struct device *dev)
{
	dev->ops = &rx6110sa_ops;
}

struct chip_operations drivers_i2c_rx6110sa_ops = {
	CHIP_NAME("RX6110 SA")
	.enable_dev = rx6110sa_enable
};
