/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_I2C_NCT7802Y_H
#define DRIVERS_I2C_NCT7802Y_H

#include <types.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c_bus.h>

#define BANK_SELECT			0x00

/* Bank 0 */
#define MODE_SELECTION			0x22
#define  MODE_SELECTION_LTD_EN		(1 << 6)
#define  MODE_SELECTION_RTDx(x, val)	((val) << (x) * 2)

#define PECI_ENABLE			0x23
#define  PECI_ENABLE_AGENTx(x)		(1 << (x))

#define FAN_ENABLE			0x24
#define  FANx_ENABLE(fan)		(1 << (fan))

#define FAN_CTRL(fan)			(0x60 + (fan))

#define CLOSE_LOOP_FAN_RPM_CTRL		0x63
#define  CLOSE_LOOP_FANx_EN(fan)	(1 << ((fan) + 5))
#define  CLOSE_LOOP_FANx_HIGH_RPM(fan)	(1 << ((fan) + 2))
#define  CLOSE_LOOP_FAN_PECI_ERR_MASK	(3 << 0)
#define  CLOSE_LOOP_FAN_PECI_ERR_CURR	(0 << 0)
#define  CLOSE_LOOP_FAN_PECI_ERR_VALUE	(1 << 0)
#define  CLOSE_LOOP_FAN_PECI_ERR_MAX	(2 << 0)

#define TEMP_SHIFT(temp) (((temp) % 2) * 4)
#define TEMP_TO_FAN_MAP(temp)		(0x64 + (temp) / 2)
#define  TEMPx_TO_FAN_MAP_MASK(temp)	(7 << TEMP_SHIFT(temp))
#define  TEMPx_TO_FANy_MAP(temp, fan)	(1 << (TEMP_SHIFT(temp) + (fan)))

#define FAN_CTRL_TEMP_SRC(temp)		(0x68 + (temp) / 2)
#define  FAN_CTRL_TEMPx_SRC_MASK(temp)	(7 << TEMP_SHIFT(temp))
#define  FAN_CTRL_TEMPx_SRCy(temp, src)	((src) << TEMP_SHIFT(temp))

#define FAN_DUTY_ON_PECI_ERROR		0x7a

#define TABLEx_TEMP_POINTy(fan, pt)	(0x80 + (0x10 * (fan)) + (pt))
#define TABLEx_TARGET_POINTy(fan, pt)	(0x85 + (0x10 * (fan)) + (pt))

/* Bank 1 */

#define PECI_CTRL_1			0x01
#define  PECI_CTRL_1_EN			(1 << 7)
#define  PECI_CTRL_1_MANUAL_EN		(1 << 1)
#define  PECI_CTRL_1_ROUTINE_EN		(1 << 0)

#define PECI_CTRL_3			0x03
#define  PECI_CTRL_3_EN_AGENTx(x)	(1 << ((x) + 4))
#define  PECI_CTRL_3_HAS_DOM1_AGENTx(x)	(1 << (x))

#define PECI_REPORT_TEMP_STYLE		0x04
#define  PECI_TEMP_STYLE_DOM0_AGENTx(x)	(0 << ((x) + 1))
#define  PECI_TEMP_STYLE_DOM1_AGENTx(x)	(1 << ((x) + 1))
#define  PECI_TEMP_STYLE_SINGLE		(0 << 0)
#define  PECI_TEMP_STYLE_HIGHEST	(1 << 0)

#define PECI_BASE_TEMP_AGENT(x)		(0x09 + (x))
#define  PECI_BASE_TEMP_MAX		(0x7f << 0)

static inline int nct7802y_select_bank(struct device *const dev, const u8 bank)
{
	const int ret = i2c_dev_writeb_at(dev, BANK_SELECT, bank);
	if (ret != CB_SUCCESS)
		printk(BIOS_NOTICE, "nct7802y: Select bank failed: %d\n", ret);
	return ret;
}

static inline int nct7802y_write(struct device *const dev,
				 const u8 reg, const u8 value)
{
	return i2c_dev_writeb_at(dev, reg, value);
}

static inline int nct7802y_update(struct device *const dev, const u8 reg,
				  const u8 clear_mask, const u8 set_mask)
{
	const int val = i2c_dev_readb_at(dev, reg);
	if (val < 0)
		return val;
	return i2c_dev_writeb_at(dev, reg, (val & ~clear_mask) | set_mask);
}

void nct7802y_init_fan(struct device *dev);
void nct7802y_init_peci(struct device *dev);

#endif /* DRIVERS_I2C_NCT7802Y_H */
