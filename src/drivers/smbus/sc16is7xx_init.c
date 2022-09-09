/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/i2c_smbus.h>
#include <device/i2c.h>
#include <device/smbus_host.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include "sc16is7xx_init.h"

/*
 * Datasheet - SC16IS740/750/760, Rev. 7 - 9 June 2011
 * https://web.archive.org/web/20210612105830/https://www.nxp.com/docs/en/data-sheet/SC16IS740_750_760.pdf
 */

// Bits [6:3] of the subaddress is to address device internal registers
#define INTERNAL_REG_SUB_ADDR_SHIFT	3

#define REG_THR 0x00    // Transmit Holding Register
#define REG_LCR 0x03    // Line Control Register

// Special Register Set is accessible only when LCR[7] is logic 1
#define REG_DLL 0x00    // divisor latch LSB
#define REG_DLH 0x01    // divisor latch MSB

#define LCR_WORD_LEN_BIT_0	BIT(0)
#define LCR_WORD_LEN_BIT_1	BIT(1)
#define LCR_STOP_BIT		BIT(2)
#define LCR_PARITY_BIT_0	BIT(3)
#define LCR_PARITY_BIT_1	BIT(4)
#define LCR_PARITY_BIT_2	BIT(5)
#define LCR_BREAK_CTL_BIT	BIT(6)
#define LCR_SPEC_REG_SET_EN	BIT(7)

#define UART_8_N_1	(LCR_WORD_LEN_BIT_0 | LCR_WORD_LEN_BIT_1)

/*
 *	UART configuration: 8 bit word length, No parity, 1 stop bit (8-N-1)
 *	Divisor value set here is calculated for 115200 baud rate
 *	in 14.7MHz clock input to chip.
 */

#define BAUD_115200_DLL	0x08
#define BAUD_115200_DLH	0x00

void sc16is7xx_write_byte(uint8_t reg, unsigned char c)
{
	do_smbus_write_byte(CONFIG_FIXED_SMBUS_IO_BASE,
			CONFIG_CONSOLE_I2C_SMBUS_SLAVE_ADDRESS, reg, c);
}

void sc16is7xx_init(void)
{
	// Configure 8-N-1 and enable special register set
	sc16is7xx_write_byte(REG_LCR << INTERNAL_REG_SUB_ADDR_SHIFT,
				(UART_8_N_1 | LCR_SPEC_REG_SET_EN));

	sc16is7xx_write_byte(REG_DLL << INTERNAL_REG_SUB_ADDR_SHIFT, BAUD_115200_DLL);
	sc16is7xx_write_byte(REG_DLH << INTERNAL_REG_SUB_ADDR_SHIFT, BAUD_115200_DLH);

	// Disable special register set
	sc16is7xx_write_byte(REG_LCR << INTERNAL_REG_SUB_ADDR_SHIFT,
				(UART_8_N_1 & ~LCR_SPEC_REG_SET_EN));
}
