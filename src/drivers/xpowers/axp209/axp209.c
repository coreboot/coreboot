/*
 * Driver for X-Powers AXP 209 Power Management Unit
 *
 * Despite axp209_read/write only working on a byte at a time, there is no such
 * limitation in the AXP209.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "axp209.h"
#include "chip.h"

#include <device/device.h>
#include <device/i2c.h>

/* Hide these definitions from the rest of the source, so keep them here */
enum registers {
	REG_POWER_STATUS	= 0x00,
	REG_POWER_MODE		= 0x01,
	REG_OTG_VBUS		= 0x02,
	REG_CHIP_ID		= 0x03,
	REG_CHIP_PWROUT_CTL	= 0x12,
	REG_DCDC2_VOLTAGE	= 0x23,
	REG_DCDC2_LDO3_CTL	= 0x25,
	REG_DCDC3_VOLTAGE	= 0x27,
	REG_LDO24_VOLTAGE	= 0x28,
	REG_LDO3_VOLTAGE	= 0x29,
	REG_VBUS_IPSOUT		= 0x30,
	REG_PWROFF_VOLTAGE	= 0x31,
	REG_SHTDWN_SETTING	= 0x32,
};

/* REG_LDO24_VOLTAGE definitions */
#define REG_LDO24_VOLTAGE_LDO2_MASK	(0xf << 4)
#define REG_LDO24_VOLTAGE_LDO2_VAL(x)	((x << 4) & REG_LDO24_VOLTAGE_LDO2_MASK)
#define REG_LDO24_VOLTAGE_LDO4_MASK	(0xf << 0)
#define REG_LDO24_VOLTAGE_LDO4_VAL(x)	((x << 0) & REG_LDO24_VOLTAGE_LDO4_MASK)

/*
 * Read and write accessors. We only work on one register at a time, but there
 * is no limitation on the AXP209 as to how many registers we may read or write
 * in one transaction.
 * These return the number of bytes read/written, or an error code. In this
 * case, they return 1 on success, or an error code otherwise.
 */
static int axp209_read(u8 bus, u8 reg, u8 *val)
{
	return i2c_read(bus, AXP209_I2C_ADDR, reg, 1, val, 1);
}

static int axp209_write(u8 bus, u8 reg, u8 val)
{
	return i2c_write(bus, AXP209_I2C_ADDR, reg, 1, &val, 1);
}

/**
 * \brief Identify and initialize an AXP209 on the I²C bus
 *
 * @param[in] bus I²C bus to which the AXP209 is connected
 * @return CB_SUCCES on if an AXP209 is found, or an error code otherwise.
 */
enum cb_err axp209_init(u8 bus)
{
	u8 id;

	if (axp209_read(bus, REG_CHIP_ID, &id) != 1)
		return CB_ERR;

	/* From U-Boot code : Low 4 bits is chip version */
	if ((id & 0x0f) != 0x1)
		return CB_ERR;

	return CB_SUCCESS;
}

/**
 * \brief Configure the output voltage of DC-DC2 converter
 *
 * If the requested voltage is not available, the next lowest voltage will
 * be applied.
 * Valid values are between 700mV and 2275mV
 *
 * @param[in] millivolts voltage in mV units.
 * @param[in] bus I²C bus to which the AXP209 is connected
 * @return CB_SUCCES on success,
 *	   CB_ERR_ARG if voltage is out of range, or an error code otherwise.
 */
enum cb_err axp209_set_dcdc2_voltage(u8 bus, u16 millivolts)
{
	u8 val;

	if (millivolts < 700 || millivolts > 2275)
		return CB_ERR_ARG;

	val = (millivolts - 700) / 25;

	if (axp209_write(bus, REG_DCDC2_VOLTAGE, val) != 1)
		return CB_ERR;

	return CB_SUCCESS;
}

/**
 * \brief Configure the output voltage of DC-DC3 converter
 *
 * If the requested voltage is not available, the next lowest voltage will
 * be applied.
 * Valid values are between 700mV and 3500mV
 *
 * @param[in] millivolts voltage in mV units.
 * @param[in] bus I²C bus to which the AXP209 is connected
 * @return CB_SUCCES on success,
 *	   CB_ERR_ARG if voltage is out of range, or an error code otherwise.
 */
enum cb_err axp209_set_dcdc3_voltage(u8 bus, u16 millivolts)
{
	u8 val;

	if (millivolts < 700 || millivolts > 3500)
		return CB_ERR_ARG;

	val = (millivolts - 700) / 25;

	if (axp209_write(bus, REG_DCDC3_VOLTAGE, val) != 1)
		return CB_ERR;

	return CB_SUCCESS;
}

/**
 * \brief Configure the output voltage of LDO2 regulator
 *
 * If the requested voltage is not available, the next lowest voltage will
 * be applied.
 * Valid values are between 700mV and 3300mV
 *
 * @param[in] millivolts voltage in mV units.
 * @param[in] bus I²C bus to which the AXP209 is connected
 * @return CB_SUCCES on success,
 *	   CB_ERR_ARG if voltage is out of range, or an error code otherwise.
 */
enum cb_err axp209_set_ldo2_voltage(u8 bus, u16 millivolts)
{
	u8 reg8, val;

	if (millivolts < 1800 || millivolts > 3300)
		return CB_ERR_ARG;

	/* Try to read the register first, and stop here on error */
	if (axp209_read(bus, REG_LDO24_VOLTAGE, &reg8) != 1)
		return CB_ERR;

	val = (millivolts - 1800) / 100;
	reg8 &= ~REG_LDO24_VOLTAGE_LDO2_MASK;
	reg8 |= REG_LDO24_VOLTAGE_LDO2_VAL(val);

	if (axp209_write(bus, REG_LDO24_VOLTAGE, reg8) != 1)
		return CB_ERR;

	return CB_SUCCESS;
}

/**
 * \brief Configure the output voltage of LDO4 regulator
 *
 * If the requested voltage is not available, the next lowest voltage will
 * be applied.
 * Valid values are between 700mV and 3500mV. Datasheet lists maximum voltage at
 * 2250mV, but hardware samples go as high as 3500mV.
 *
 * @param[in] millivolts voltage in mV units.
 * @param[in] bus I²C bus to which the AXP209 is connected
 * @return CB_SUCCES on success,
 *	   CB_ERR_ARG if voltage is out of range, or an error code otherwise.
 */
enum cb_err axp209_set_ldo3_voltage(u8 bus, u16 millivolts)
{
	u8 val;

	/* Datasheet lists 2250 max, but PMU will output up to 3500mV */
	if (millivolts < 700 || millivolts > 3500)
		return CB_ERR_ARG;

	val = (millivolts - 700) / 25;

	if (axp209_write(bus, REG_LDO3_VOLTAGE, val) != 1)
		return CB_ERR;

	return CB_SUCCESS;
}

/**
 * \brief Configure the output voltage of DC-DC2 converter
 *
 * If the requested voltage is not available, the next lowest voltage will
 * be applied.
 * Valid values are between 1250V and 3300mV
 *
 * @param[in] millivolts voltage in mV units.
 * @param[in] bus I²C bus to which the AXP209 is connected
 * @return CB_SUCCES on success,
 *	   CB_ERR_ARG if voltage is out of range, or an error code otherwise.
 */
enum cb_err axp209_set_ldo4_voltage(u8 bus, u16 millivolts)
{
	u8 reg8, val;

	if (millivolts < 1250 || millivolts > 3300)
		return CB_ERR_ARG;

	/* Try to read the register first, and stop here on error */
	if (axp209_read(bus, REG_LDO24_VOLTAGE, &reg8) != 1)
		return CB_ERR;

	if (millivolts <= 2000)
		val = (millivolts - 1200) / 100;
	else if (millivolts <= 2700)
		val = 9 + (millivolts - 2500) / 100;
	else if (millivolts <= 2800)
		val = 11;
	else
		val = 12 + (millivolts - 3000) / 100;

	reg8 &= ~REG_LDO24_VOLTAGE_LDO4_MASK;
	reg8 |= REG_LDO24_VOLTAGE_LDO4_VAL(val);

	if (axp209_write(bus, REG_LDO24_VOLTAGE, reg8) != 1)
			return CB_ERR;

	return CB_SUCCESS;
}

/*
 * Usually, the AXP209 is enabled and configured in romstage, so there is no
 * need for a full ramstage driver. Hence .enable_dev is NULL.
 */
#ifndef __PRE_RAM__
struct chip_operations drivers_xpowers_axp209_config = {
	CHIP_NAME("X-Powers AXP 209 Power Management Unit")
	.enable_dev = NULL,
};
#endif /* __PRE_RAM__ */
