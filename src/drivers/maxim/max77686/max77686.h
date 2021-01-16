/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __MAX77686_H_
#define __MAX77686_H_

enum max77686_regnum {
	PMIC_BUCK1 = 0,
	PMIC_BUCK2,
	PMIC_BUCK3,
	PMIC_BUCK4,
	PMIC_BUCK5,
	PMIC_BUCK6,
	PMIC_BUCK7,
	PMIC_BUCK8,
	PMIC_BUCK9,
	PMIC_LDO1,
	PMIC_LDO2,
	PMIC_LDO3,
	PMIC_LDO4,
	PMIC_LDO5,
	PMIC_LDO6,
	PMIC_LDO7,
	PMIC_LDO8,
	PMIC_LDO9,
	PMIC_LDO10,
	PMIC_LDO11,
	PMIC_LDO12,
	PMIC_LDO13,
	PMIC_LDO14,
	PMIC_LDO15,
	PMIC_LDO16,
	PMIC_LDO17,
	PMIC_LDO18,
	PMIC_LDO19,
	PMIC_LDO20,
	PMIC_LDO21,
	PMIC_LDO22,
	PMIC_LDO23,
	PMIC_LDO24,
	PMIC_LDO25,
	PMIC_LDO26,
	PMIC_EN32KHZ_CP,
};

/**
 * struct max77686_para - max77686 register parameters
 * @param vol_addr	i2c address of the given buck/ldo register
 * @param vol_bitpos	bit position to be set or clear within register
 * @param vol_bitmask	bit mask value
 * @param reg_enaddr	control register address, which enable the given
 *			buck/ldo.
 * @param reg_enbitpos	bit position to be enabled
 * @param reg_enbiton	value to be written to buck/ldo to make it ON
 * @param reg_enbitoff	value to be written to buck/ldo to make it OFF
 * @param vol_min	minimum voltage level supported by given buck/ldo
 * @param vol_div	voltage division value of given buck/ldo
 */
struct max77686_para {
	u8	vol_addr;
	u8	vol_bitpos;
	u8	vol_bitmask;
	u8	reg_enaddr;
	u8	reg_enbitpos;
	u8	reg_enbitmask;
	u8	reg_enbiton;
	u8	reg_enbitoff;
	u16	vol_min;
	u16	vol_div;
};

/* I2C device address for pmic max77686 */
#define MAX77686_I2C_ADDR (0x12 >> 1)

enum {
	REG_DISABLE = 0,
	REG_ENABLE
};

enum {
	MAX77686_MV = 0,	/* mili volt */
	MAX77686_UV		/* micro volt */
};

/**
 * This function enables the 32KHz coprocessor clock.
 *
 * @param bus		i2c bus
 *
 * Return 0 if ok, else -1
 */
int max77686_enable_32khz_cp(unsigned int bus);

/**
 * Set the required voltage level of pmic
 *
 * @param bus		i2c bus
 * @param reg		register number of buck/ldo to be set
 * @param volt		voltage level to be set
 * @param enable	enable or disable bit
 * @param volt_units	MAX77686_MV or MAX77686_UV, unit of the
 *			voltage parameters
 *
 * @return		Return 0 if ok, else -1
 */
int max77686_volsetting(unsigned int bus, enum max77686_regnum reg,
			unsigned int volt, int enable, int volt_units);

/**
 * Disable charging of the RTC backup battery
 *
 * @param bus		i2c bus
 *
 * @return		Return 0 if ok, else -1
 */
int max77686_disable_backup_batt(unsigned int bus);

#endif /* __MAX77686_H_ */
