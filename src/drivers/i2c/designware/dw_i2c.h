/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_DESIGNWARE_I2C_H__
#define __DRIVERS_I2C_DESIGNWARE_I2C_H__

#include <device/device.h>
#include <device/i2c.h>
#include <stdint.h>

#if CONFIG(DRIVERS_I2C_DESIGNWARE_DEBUG)
#define DW_I2C_DEBUG BIOS_DEBUG

#else
#define DW_I2C_DEBUG BIOS_NEVER

#endif // CONFIG_DRIVERS_I2C_DESIGNWARE_DEBUG

/*
 * Timing values are in units of clock period, with the clock speed
 * provided by the SOC in CONFIG_DRIVERS_I2C_DESIGNWARE_CLOCK_MHZ
 * Automatic configuration is done based on requested speed, but the
 * values may need tuned depending on the board and the number of
 * devices present on the bus.
 */
struct dw_i2c_speed_config {
	enum i2c_speed speed;
	/* SCL high and low period count */
	uint16_t scl_lcnt;
	uint16_t scl_hcnt;
	/*
	 * SDA hold time should be 300ns in standard and fast modes
	 * and long enough for deterministic logic level change in
	 * fast-plus and high speed modes.
	 *
	 *  [15:0] SDA TX Hold Time
	 * [23:16] SDA RX Hold Time
	 */
	uint32_t sda_hold;
};

/*
 * This I2C controller has support for 3 independent speed configs but can
 * support both FAST_PLUS and HIGH speeds through the same set of speed
 * config registers.  These are treated separately so the speed config values
 * can be provided via ACPI to the OS.
 */
#define DW_I2C_SPEED_CONFIG_COUNT	4

struct dw_i2c_bus_config {
	/* Bus should be enabled prior to ramstage with temporary base */
	int early_init;
	/* Bus speed in Hz, default is I2C_SPEED_FAST (400 KHz) */
	enum i2c_speed speed;
	/* If rise_time_ns is non-zero the calculations for lcnt and hcnt
	 * registers take into account the times of the bus. However, if
	 * there is a match in speed_config those register values take
	 * precedence. */
	int rise_time_ns;
	int fall_time_ns;
	int data_hold_time_ns;
	/* Specific bus speed configuration */
	struct dw_i2c_speed_config speed_config[DW_I2C_SPEED_CONFIG_COUNT];
};

/* Functions to be implemented by SoC code */

/* Get base address for early init of I2C controllers. */
uintptr_t dw_i2c_get_soc_early_base(unsigned int bus);

/*
 * Map given I2C bus number to devfn.
 * Return value:
 * -1 = error
 * otherwise, devfn(>=0) corresponding to I2C bus number.
 */
int dw_i2c_soc_devfn_to_bus(unsigned int devfn);

/*
 * Map given bus number to a I2C Controller.
 * Return value:
 * -1 = error
 * otherwise, devfn(>=0) corresponding to I2C bus number.
 */
int dw_i2c_soc_bus_to_devfn(unsigned int bus);

/*
 * SoC implemented callback for getting I2C bus configuration.
 *
 * Returns NULL if i2c config is not found
 */
const struct dw_i2c_bus_config *dw_i2c_get_soc_cfg(unsigned int bus);

/* Get I2C controller base address */
uintptr_t dw_i2c_base_address(unsigned int bus);

/*
 * Initialize this bus controller and set the speed
 * Return value:
 * -1 = failure
 *  0 = success
*/
int dw_i2c_init(unsigned int bus, const struct dw_i2c_bus_config *bcfg);

/*
 * Generate speed config based on clock
 * Return value:
 * -1 = failure
 *  0 = success
*/
int dw_i2c_gen_speed_config(uintptr_t dw_i2c_addr,
					enum i2c_speed speed,
					const struct dw_i2c_bus_config *bcfg,
					struct dw_i2c_speed_config *config);

/*
 * Process given I2C segments in a single transfer
 * Return value:
 * -1 = failure
 *  0 = success
 */
int dw_i2c_transfer(unsigned int bus,
			const struct i2c_msg *segments,
			size_t count);

/*
 * Map an i2c host controller device to a logical bus number.
 * Return value:
 * -1 = failure
 * >=0 = logical bus number
 */
int dw_i2c_soc_dev_to_bus(const struct device *dev);

/*
 * Common device_operations implementation to initialize the i2c host
 * controller.
 */
void dw_i2c_dev_init(struct device *dev);

/*
 * Common device_operations implementation to fill ACPI SSDT table for i2c
 * host controller.
 */
void dw_i2c_acpi_fill_ssdt(const struct device *dev);

/*
 * Common device_operations implementation for i2c host controller ops.
 */
extern const struct i2c_bus_operations dw_i2c_bus_ops;

#endif /* __DRIVERS_I2C_DESIGNWARE_I2C_H__ */
