/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _GALILEO_REG_ACCESS_H_
#define _GALILEO_REG_ACCESS_H_

#include <fsp/util.h>
#include <reg_script.h>
#include <soc/IntelQNCConfig.h>
#include <soc/QuarkNcSocId.h>
#include <soc/reg_access.h>

enum {
	MAINBOARD_TYPE = REG_SCRIPT_TYPE_MAINBOARD_BASE,
	/* Add additional mainboard access types here*/
};

enum {
	GEN1_I2C_GPIO_EXP_0x20 = 0x20,	/* Cypress CY8C9540A */
	GEN1_I2C_GPIO_EXP_0x21 = 0x21,	/* Cypress CY8C9540A */

	GEN2_I2C_GPIO_EXP0 = 0x25,	/* NXP PCAL9535A */
	GEN2_I2C_GPIO_EXP1 = 0x26,	/* NXP PCAL9535A */
	GEN2_I2C_GPIO_EXP2 = 0x27,	/* NXP PCAL9535A */
	GEN2_I2C_LED_PWM = 0x47,	/* NXP PCAL9685 */
};

/* Cypress CY8C9548A registers */
#define GEN1_GPIO_EXP_INPUT0		0x00
#define GEN1_GPIO_EXP_INPUT1		0x01
#define GEN1_GPIO_EXP_INPUT2		0x02
#define GEN1_GPIO_EXP_INPUT3		0x03
#define GEN1_GPIO_EXP_INPUT4		0x04
#define GEN1_GPIO_EXP_INPUT5		0x05
#define GEN1_GPIO_EXP_OUTPUT0		0x08
#define GEN1_GPIO_EXP_OUTPUT1		0x09
#define GEN1_GPIO_EXP_OUTPUT2		0x0a
#define GEN1_GPIO_EXP_OUTPUT3		0x0b
#define GEN1_GPIO_EXP_OUTPUT4		0x0c
#define GEN1_GPIO_EXP_OUTPUT5		0x0d
#define GEN1_GPIO_EXP_PORT_SELECT	0x18
#define GEN1_GPIO_EXP_PORT_DIR		0x1c

/* NXP PCAL9535A registers */
#define GEN2_GPIO_EXP_INPUT0		0x00
#define GEN2_GPIO_EXP_INPUT1		0x01
#define GEN2_GPIO_EXP_OUTPUT0		0x02
#define GEN2_GPIO_EXP_OUTPUT1		0x03
#define GEN2_GPIO_EXP_POLARITY0		0x04
#define GEN2_GPIO_EXP_POLARITY1		0x05
#define GEN2_GPIO_EXP_CONFIG0		0x06
#define GEN2_GPIO_EXP_CONFIG1		0x07
#define GEN2_GPIO_EXP_INPUT_LATCH0	0x44
#define GEN2_GPIO_EXP_INPUT_LATCH1	0x45
#define GEN2_GPIO_EXP_PULL_UP_DOWN_EN0	0x46
#define GEN2_GPIO_EXP_PULL_UP_DOWN_EN1	0x47
#define GEN2_GPIO_EXP_PULL_UP_DOWN_SEL0	0x46
#define GEN2_GPIO_EXP_PULL_UP_DOWN_SEL1	0x47

#define MAINBOARD_ACCESS(cmd_, reg_, size_, mask_, value_, timeout_, reg_set_) \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_, MAINBOARD_TYPE,      \
			       size_, reg_, mask_, value_, timeout_, reg_set_)

/* I2C chip register access macros */
#define REG_I2C_ACCESS(cmd_, reg_, mask_, value_, timeout_, slave_addr_) \
	MAINBOARD_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_8, mask_, value_,   \
		timeout_, slave_addr_)
#define REG_I2C_READ(slave_addr_, reg_) \
	REG_I2C_ACCESS(READ, reg_, 0, 0, 0, slave_addr_)
#define REG_I2C_WRITE(slave_addr_, reg_, value_) \
	REG_I2C_ACCESS(WRITE, reg_, 0, value_, 0, slave_addr_)
#define REG_I2C_AND(slave_addr_, reg_, value_) \
	REG_I2C_RMW(slave_addr_, reg_, value_, 0)
#define REG_I2C_RMW(slave_addr_, reg_, mask_, value_) \
	REG_I2C_ACCESS(RMW, reg_, mask_, value_, 0, slave_addr_)
#define REG_I2C_RXW(slave_addr_, reg_, mask_, value_) \
	REG_I2C_ACCESS(RXW, reg_, mask_, value_, 0, slave_addr_)
#define REG_I2C_OR(slave_addr_, reg_, value_) \
	REG_I2C_RMW(slave_addr_, reg_, 0xff, value_)
#define REG_I2C_POLL(slave_addr_, reg_, mask_, value_, timeout_) \
	REG_I2C_ACCESS(POLL, reg_, mask_, value_, timeout_, slave_addr_)
#define REG_I2C_XOR(slave_addr_, reg_, value_) \
	REG_I2C_RXW(slave_addr_, reg_, 0xff, value_)

#endif /* _GALILEO_REG_ACCESS_H_ */
