/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <gpio.h>
#include <assert.h>
#include <soc/spi.h>

enum {
	EN_OFFSET = 0x60,
	SEL_OFFSET = 0x80,
	EH_RSEL_OFFSET = 0xF0,
	GPIO_DRV0_OFFSET = 0xA0,
	GPIO_DRV1_OFFSET = 0xB0,
};

static void gpio_set_pull_pupd(gpio_t gpio, enum pull_enable enable,
			       enum pull_select select)
{
	void *reg = GPIO_TO_IOCFG_BASE(gpio.base) + gpio.offset;
	int bit = gpio.bit;

	if (enable == GPIO_PULL_ENABLE) {
		if (select == GPIO_PULL_DOWN)
			setbits32(reg, 1 << (bit + 2));
		else
			clrbits32(reg, 1 << (bit + 2));
	}

	if (enable == GPIO_PULL_ENABLE)
		clrsetbits32(reg, 3 << bit, 1 << bit);
	else
		clrbits32(reg, 3 << bit);
}

static void gpio_set_pull_en_sel(gpio_t gpio, enum pull_enable enable,
				 enum pull_select select)
{
	void *reg = GPIO_TO_IOCFG_BASE(gpio.base) + gpio.offset;
	int bit = gpio.bit;

	if (enable == GPIO_PULL_ENABLE) {
		if (select == GPIO_PULL_DOWN)
			clrbits32(reg + SEL_OFFSET, 1 << bit);
		else
			setbits32(reg + SEL_OFFSET, 1 << bit);
	}

	if (enable == GPIO_PULL_ENABLE)
		setbits32(reg + EN_OFFSET, 1 << bit);
	else
		clrbits32(reg + EN_OFFSET, 1 << bit);
}

void gpio_set_pull(gpio_t gpio, enum pull_enable enable,
		   enum pull_select select)
{
	if (gpio.flag)
		gpio_set_pull_pupd(gpio, enable, select);
	else
		gpio_set_pull_en_sel(gpio, enable, select);
}

enum {
	EH_VAL = 0x0,
	RSEL_VAL = 0x3,
	EH_MASK = 0x7,
	RSEL_MASK = 0x3,
	SCL0_EH = 19,
	SCL0_RSEL = 15,
	SDA0_EH = 9,
	SDA0_RSEL = 5,
	SCL1_EH = 22,
	SCL1_RSEL = 17,
	SDA1_EH = 12,
	SDA1_RSEL = 7,
	SCL2_EH = 24,
	SCL2_RSEL = 20,
	SDA2_EH = 14,
	SDA2_RSEL = 10,
	SCL3_EH = 12,
	SCL3_RSEL = 10,
	SDA3_EH = 7,
	SDA3_RSEL = 5,
	SCL4_EH = 27,
	SCL4_RSEL = 22,
	SDA4_EH = 17,
	SDA4_RSEL = 12,
	SCL5_EH = 20,
	SCL5_RSEL = 18,
	SDA5_EH = 15,
	SDA5_RSEL = 13,
};

#define I2C_EH_RSL_MASK(name) \
	(EH_MASK << name##_EH | RSEL_MASK << name##_RSEL)

#define I2C_EH_RSL_VAL(name) \
	(EH_VAL << name##_EH | RSEL_VAL << name##_RSEL)

void gpio_set_i2c_eh_rsel(void)
{
	clrsetbits32((void *)IOCFG_RB_BASE + EH_RSEL_OFFSET,
		I2C_EH_RSL_MASK(SCL0) | I2C_EH_RSL_MASK(SDA0) |
		I2C_EH_RSL_MASK(SCL1) | I2C_EH_RSL_MASK(SDA1),
		I2C_EH_RSL_VAL(SCL0) | I2C_EH_RSL_VAL(SDA0) |
		I2C_EH_RSL_VAL(SCL1) | I2C_EH_RSL_VAL(SDA1));

	clrsetbits32((void *)IOCFG_RM_BASE + EH_RSEL_OFFSET,
		I2C_EH_RSL_MASK(SCL2) | I2C_EH_RSL_MASK(SDA2) |
		I2C_EH_RSL_MASK(SCL4) | I2C_EH_RSL_MASK(SDA4),
		I2C_EH_RSL_VAL(SCL2) | I2C_EH_RSL_VAL(SDA2) |
		I2C_EH_RSL_VAL(SCL4) | I2C_EH_RSL_VAL(SDA4));

	clrsetbits32((void *)IOCFG_BL_BASE + EH_RSEL_OFFSET,
		I2C_EH_RSL_MASK(SCL3) | I2C_EH_RSL_MASK(SDA3),
		I2C_EH_RSL_VAL(SCL3) | I2C_EH_RSL_VAL(SDA3));

	clrsetbits32((void *)IOCFG_LB_BASE + EH_RSEL_OFFSET,
		I2C_EH_RSL_MASK(SCL5) | I2C_EH_RSL_MASK(SDA5),
		I2C_EH_RSL_VAL(SCL5) | I2C_EH_RSL_VAL(SDA5));
}

void gpio_set_spi_driving(unsigned int bus, enum spi_pad_mask pad_select,
			  unsigned int milliamps)
{
	void *reg = NULL;
	unsigned int reg_val = milliamps / 2 - 1, offset = 0;

	assert(bus < SPI_BUS_NUMBER);
	assert(milliamps >= 2 && milliamps <= 16);
	assert(pad_select <= SPI_PAD1_MASK);

	switch (bus) {
	case 0:
		reg = (void *)(IOCFG_RB_BASE + GPIO_DRV1_OFFSET);
		offset = 0;
		break;
	case 1:
		if (pad_select == SPI_PAD0_MASK) {
			reg = (void *)(IOCFG_LM_BASE + GPIO_DRV0_OFFSET);
			offset = 0;
		} else if (pad_select == SPI_PAD1_MASK) {
			clrsetbits32((void *)IOCFG_RM_BASE +
					GPIO_DRV0_OFFSET, 0xf | 0xf << 20,
					reg_val | reg_val << 20);
			clrsetbits32((void *)IOCFG_RM_BASE +
					GPIO_DRV1_OFFSET, 0xf << 16,
					reg_val << 16);
			return;
		}
		break;
	case 2:
		clrsetbits32((void *)IOCFG_RM_BASE + GPIO_DRV0_OFFSET,
				0xf << 8 | 0xf << 12,
				reg_val << 8 | reg_val << 12);
		return;
	case 3:
		reg = (void *)(IOCFG_LM_BASE + GPIO_DRV0_OFFSET);
		offset = 16;
		break;
	case 4:
		reg = (void *)(IOCFG_LM_BASE + GPIO_DRV0_OFFSET);
		offset = 12;
		break;
	case 5:
		reg = (void *)(IOCFG_LM_BASE + GPIO_DRV0_OFFSET);
		offset = 8;
		break;
	}

	clrsetbits32(reg, 0xf << offset, reg_val << offset);
}
