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

#include <fsp/util.h>
#include <reg_script.h>
#include <soc/QuarkNcSocId.h>

enum {
	USB_PHY_REGS = 1,
};

enum {
	SOC_TYPE = REG_SCRIPT_TYPE_SOC_BASE,
	/* Add additional SOC access types here*/
};

#define SOC_ACCESS(cmd_, reg_, size_, mask_, value_, timeout_, reg_set_)   \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_, SOC_TYPE,        \
			       size_, reg_, mask_, value_, timeout_, reg_set_)
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

void mcr_write(uint8_t opcode, uint8_t port, uint32_t reg_address);
uint32_t mdr_read(void);
void mdr_write(uint32_t value);
void mea_write(uint32_t reg_address);

#endif /* _QUARK_REG_ACCESS_H_ */
