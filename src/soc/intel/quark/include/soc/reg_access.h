/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _QUARK_REG_ACCESS_H_
#define _QUARK_REG_ACCESS_H_

#include <cpu/x86/cr.h>
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
	PCIE_AFE_REGS,
	PCIE_RESET,
	GPE0_REGS,
	HOST_BRIDGE,
	CPU_CR,
};

enum {
	SOC_TYPE = REG_SCRIPT_TYPE_SOC_BASE,
	/* Add additional SOC access types here*/
};

#define SOC_ACCESS(cmd_, reg_, size_, mask_, value_, timeout_, reg_set_)   \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_, SOC_TYPE,        \
			       size_, reg_, mask_, value_, timeout_, reg_set_)

/* CPU CRx register access macros */
#define REG_CPU_CR_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		CPU_CR)
#define REG_CPU_CR_READ(reg_) \
	REG_CPU_CR_ACCESS(READ, reg_, 0, 0, 0)
#define REG_CPU_CR_WRITE(reg_, value_) \
	REG_CPU_CR_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_CPU_CR_AND(reg_, value_) \
	REG_CPU_CR_RMW(reg_, value_, 0)
#define REG_CPU_CR_RMW(reg_, mask_, value_) \
	REG_CPU_CR_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_CPU_CR_RXW(reg_, mask_, value_) \
	REG_CPU_CR_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_CPU_CR_OR(reg_, value_) \
	REG_CPU_CR_RMW(reg_, 0xffffffff, value_)
#define REG_CPU_CR_POLL(reg_, mask_, value_, timeout_) \
	REG_CPU_CR_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_CPU_CR_XOR(reg_, value_) \
	REG_CPU_CR_RXW(reg_, 0xffffffff, value_)

/* GPE0 controller register access macros */
#define REG_GPE0_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		GPE0_REGS)
#define REG_GPE0_READ(reg_) \
	REG_GPE0_ACCESS(READ, reg_, 0, 0, 0)
#define REG_GPE0_WRITE(reg_, value_) \
	REG_GPE0_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_GPE0_AND(reg_, value_) \
	REG_GPE0_RMW(reg_, value_, 0)
#define REG_GPE0_RMW(reg_, mask_, value_) \
	REG_GPE0_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_GPE0_RXW(reg_, mask_, value_) \
	REG_GPE0_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_GPE0_OR(reg_, value_) \
	REG_GPE0_RMW(reg_, 0xffffffff, value_)
#define REG_GPE0_POLL(reg_, mask_, value_, timeout_) \
	REG_GPE0_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_GPE0_XOR(reg_, value_) \
	REG_GPE0_RXW(reg_, 0xffffffff, value_)

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

/* Host bridge register access macros */
#define REG_HOST_BRIDGE_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		HOST_BRIDGE)
#define REG_HOST_BRIDGE_READ(reg_) \
	REG_HOST_BRIDGE_ACCESS(READ, reg_, 0, 0, 0)
#define REG_HOST_BRIDGE_WRITE(reg_, value_) \
	REG_HOST_BRIDGE_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_HOST_BRIDGE_AND(reg_, value_) \
	REG_HOST_BRIDGE_RMW(reg_, value_, 0)
#define REG_HOST_BRIDGE_RMW(reg_, mask_, value_) \
	REG_HOST_BRIDGE_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_HOST_BRIDGE_RXW(reg_, mask_, value_) \
	REG_HOST_BRIDGE_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_HOST_BRIDGE_OR(reg_, value_) \
	REG_HOST_BRIDGE_RMW(reg_, 0xffffffff, value_)
#define REG_HOST_BRIDGE_POLL(reg_, mask_, value_, timeout_) \
	REG_HOST_BRIDGE_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_HOST_BRIDGE_XOR(reg_, value_) \
	REG_HOST_BRIDGE_RXW(reg_, 0xffffffff, value_)

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

/* PCIE AFE register access macros */
#define REG_PCIE_AFE_ACCESS(cmd_, reg_, mask_, value_, timeout_) \
	SOC_ACCESS(cmd_, reg_, REG_SCRIPT_SIZE_32, mask_, value_, timeout_, \
		PCIE_AFE_REGS)
#define REG_PCIE_AFE_READ(reg_) \
	REG_PCIE_AFE_ACCESS(READ, reg_, 0, 0, 0)
#define REG_PCIE_AFE_WRITE(reg_, value_) \
	REG_PCIE_AFE_ACCESS(WRITE, reg_, 0, value_, 0)
#define REG_PCIE_AFE_AND(reg_, value_) \
	REG_PCIE_AFE_RMW(reg_, value_, 0)
#define REG_PCIE_AFE_RMW(reg_, mask_, value_) \
	REG_PCIE_AFE_ACCESS(RMW, reg_, mask_, value_, 0)
#define REG_PCIE_AFE_RXW(reg_, mask_, value_) \
	REG_PCIE_AFE_ACCESS(RXW, reg_, mask_, value_, 0)
#define REG_PCIE_AFE_OR(reg_, value_) \
	REG_PCIE_AFE_RMW(reg_, 0xffffffff, value_)
#define REG_PCIE_AFE_POLL(reg_, mask_, value_, timeout_) \
	REG_PCIE_AFE_ACCESS(POLL, reg_, mask_, value_, timeout_)
#define REG_PCIE_AFE_XOR(reg_, value_) \
	REG_PCIE_AFE_RXW(reg_, 0xffffffff, value_)

/* PCIe reset */
#define MAINBOARD_PCIE_RESET(pin_value_) \
	SOC_ACCESS(WRITE, 0, REG_SCRIPT_SIZE_32, 1, pin_value_, 0, PCIE_RESET)

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
	SOC_ACCESS(WRITE, 0, REG_SCRIPT_SIZE_32, 0, value_, 0, \
		MICROSECOND_DELAY)

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
void mainboard_gpio_pcie_reset(uint32_t pin_value);
void mcr_write(uint8_t opcode, uint8_t port, uint32_t reg_address);
uint32_t mdr_read(void);
void mdr_write(uint32_t value);
void mea_write(uint32_t reg_address);
uint32_t port_reg_read(uint8_t port, uint32_t offset);
void port_reg_write(uint8_t port, uint32_t offset, uint32_t value);
uint32_t reg_host_bridge_unit_read(uint32_t reg_address);
uint32_t reg_legacy_gpio_read(uint32_t reg_address);
void reg_legacy_gpio_write(uint32_t reg_address, uint32_t value);
uint32_t reg_rmu_temp_read(uint32_t reg_address);

#endif /* _QUARK_REG_ACCESS_H_ */
