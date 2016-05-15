/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
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

#ifndef _QUARK_REG_ACCESS_H_
#define _QUARK_REG_ACCESS_H_

#include <delay.h>
#include <fsp/util.h>
#include <reg_script.h>
#include <soc/IntelQNCConfig.h>
#include <soc/Ioh.h>
#include <soc/QuarkNcSocId.h>

enum {
	USB_PHY_REGS = 1,
	SOC_UNIT_REGS,
	RMU_TEMP_REGS,
	MICROSECOND_DELAY,
	LEG_GPIO_REGS,
	GPIO_REGS,
};

enum {
	SOC_TYPE = REG_SCRIPT_TYPE_SOC_BASE,
	/* Add additional SOC access types here*/
};

#define SOC_ACCESS(cmd_, reg_, size_, mask_, value_, timeout_, reg_set_)   \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_, SOC_TYPE,        \
			       size_, reg_, mask_, value_, timeout_, reg_set_)

/* GPIO controller register access macros */
#define REG_GPIO_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		GPIO_REGS)
#define REG_GPIO_READ(reg_) \
	REG_GPIO_ACCESS(READ, reg_, 0, 0, 0)
#define REG_GPIO_WRITE(reg_, value_) \
	REG_GPIO_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_GPIO_AND(reg_, value_) \
	REG_GPIO_RMW(reg_, value_, 0)
#define REG_GPIO_RMW(reg_, mask_, value_) \
	REG_GPIO_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_GPIO_RXW(reg_, mask_, value_) \
	REG_GPIO_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_GPIO_OR(reg_, value_) \
	REG_GPIO_RMW(reg_, 0xffffffff, value_)
#define REG_GPIO_POLL(reg_, mask_, value_, timeout_) \
	REG_GPIO_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_GPIO_XOR(reg_, value_) \
	REG_GPIO_RXW(reg_, 0xffffffff, value_)

/* Legacy GPIO register access macros */
#define REG_LEG_GPIO_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		LEG_GPIO_REGS)
#define REG_LEG_GPIO_READ(reg_) \
	REG_LEG_GPIO_ACCESS(READ, reg_, 0, 0, 0)
#define REG_LEG_GPIO_WRITE(reg_, value_) \
	REG_LEG_GPIO_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_LEG_GPIO_AND(reg_, value_) \
	REG_LEG_GPIO_RMW(reg_, value_, 0)
#define REG_LEG_GPIO_RMW(reg_, mask_, value_) \
	REG_LEG_GPIO_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_LEG_GPIO_RXW(reg_, mask_, value_) \
	REG_LEG_GPIO_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_LEG_GPIO_OR(reg_, value_) \
	REG_LEG_GPIO_RMW(reg_, 0xffffffff, value_)
#define REG_LEG_GPIO_POLL(reg_, mask_, value_, timeout_) \
	REG_LEG_GPIO_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_LEG_GPIO_XOR(reg_, value_) \
	REG_LEG_GPIO_RXW(reg_, 0xffffffff, value_)

/* RMU temperature register access macros */
#define REG_RMU_TEMP_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		RMU_TEMP_REGS)
#define REG_RMU_TEMP_READ(reg_) \
	REG_RMU_TEMP_ACCESS(READ, reg_, 0, 0, 0)
#define REG_RMU_TEMP_WRITE(reg_, value_) \
	REG_RMU_TEMP_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_RMU_TEMP_AND(reg_, value_) \
	REG_RMU_TEMP_RMW(reg_, value_, 0)
#define REG_RMU_TEMP_RMW(reg_, mask_, value_) \
	REG_RMU_TEMP_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_RMU_TEMP_RXW(reg_, mask_, value_) \
	REG_RMU_TEMP_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_RMU_TEMP_OR(reg_, value_) \
	REG_RMU_TEMP_RMW(reg_, 0xffffffff, value_)
#define REG_RMU_TEMP_POLL(reg_, mask_, value_, timeout_) \
	REG_RMU_TEMP_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_RMU_TEMP_XOR(reg_, value_) \
	REG_RMU_TEMP_RXW(reg_, 0xffffffff, value_)

/* Temperature sensor access macros */
#define REG_SOC_UNIT_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		SOC_UNIT_REGS)
#define REG_SOC_UNIT_READ(reg_) \
	REG_SOC_UNIT_ACCESS(READ, reg_, 0, 0, 0)
#define REG_SOC_UNIT_WRITE(reg_, value_) \
	REG_SOC_UNIT_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_SOC_UNIT_AND(reg_, value_) \
	REG_SOC_UNIT_RMW(reg_, value_, 0)
#define REG_SOC_UNIT_RMW(reg_, mask_, value_) \
	REG_SOC_UNIT_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_SOC_UNIT_RXW(reg_, mask_, value_) \
	REG_SOC_UNIT_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_SOC_UNIT_OR(reg_, value_) \
	REG_SOC_UNIT_RMW(reg_, 0xffffffff, value_)
#define REG_SOC_UNIT_POLL(reg_, mask_, value_, timeout_) \
	REG_SOC_UNIT_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_SOC_UNIT_XOR(reg_, value_) \
	REG_SOC_UNIT_RXW(reg_, 0xffffffff, value_)

/* Time delays */
#define TIME_DELAY_USEC(value_)   \
	SOC_ACCESS(WRITE, 0, REG_SCRIPT_SIZE_32, 0, value_, 0, MICROSECOND_DELAY)

/* USB register access macros */
#define REG_USB_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		USB_PHY_REGS)
#define REG_USB_READ(reg_) \
	REG_USB_ACCESS(READ, reg_, 0, 0, 0)
#define REG_USB_WRITE(reg_, value_) \
	REG_USB_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_USB_AND(reg_, value_) \
	REG_USB_RMW(reg_, value_, 0)
#define REG_USB_RMW(reg_, mask_, value_) \
	REG_USB_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_USB_RXW(reg_, mask_, value_) \
	REG_USB_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_USB_OR(reg_, value_) \
	REG_USB_RMW(reg_, 0xffffffff, value_)
#define REG_USB_POLL(reg_, mask_, value_, timeout_) \
	REG_USB_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_USB_XOR(reg_, value_) \
	REG_USB_RXW(reg_, 0xffffffff, value_)

void *get_i2c_address(void);
void mainboard_gpio_init(void);
void mcr_write(uint8_t opcode, uint8_t port, uint32_t reg_address);
uint32_t mdr_read(void);
void mdr_write(uint32_t value);
void mea_write(uint32_t reg_address);
uint32_t reg_legacy_gpio_read(uint32_t reg_address);
void reg_legacy_gpio_write(uint32_t reg_address, uint32_t value);
uint32_t reg_rmu_temp_read(uint32_t reg_address);

#endif /* _QUARK_REG_ACCESS_H_ */
