/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pnp.h>
#include <stdint.h>
#include <device/pnp_ops.h>
#include "sch5545.h"

static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0x55, port);
}

static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0xaa, port);
}

/*
 * Set the BAR / iobase for a specific device.
 * pnp_devfn_t dev must be in conf state.
 * LDN LPC IF must be active.
 */
static void set_iobase(pnp_devfn_t dev, uint16_t device_addr, uint16_t bar_addr)
{
	uint16_t bar;

	/*
	 * Set the BAR. We have to flip the BAR due to different register layout:
	 * - LPC addr LSB on device_addr + 2
	 * - LPC addr MSB on device_addr + 3
	 */
	bar = ((bar_addr >> 8) & 0xff) | ((bar_addr & 0xff) << 8);
	pnp_set_iobase(dev, device_addr + 2, bar);
}

/*
 * Set the IRQ for the specific device.
 * pnp_devfn_t dev must be in conf state.
 * LDN LPC IF must be active.
 */
static void set_irq(pnp_devfn_t dev, uint8_t irq_device, unsigned int irq)
{
	if (irq > 15)
		return;

	pnp_write_config(dev, SCH5545_IRQ_BASE + irq, irq_device);
}

/*
 * sch5545 has 2 LEDs which are accessed via color (1 bit), 2 bits for a
 * pattern blink and 1 bit for "code fetch" which means the cpu/mainboard is
 * working (always set).
 */
void sch5545_set_led(unsigned int runtime_reg_base, unsigned int color, uint16_t blink)
{
	uint8_t val = blink & SCH5545_LED_BLINK_MASK;
	val |= SCH5545_LED_CODE_FETCH;
	if (color)
		val |= SCH5545_LED_COLOR_GREEN;
	outb(val, runtime_reg_base + SCH5545_RR_LED);
}

void sch5545_early_init(unsigned int port)
{
	pnp_devfn_t dev;

	/* Enable SERIRQ */
	dev = PNP_DEV(port, SCH5545_LDN_GCONF);
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_write_config(dev, 0x24, pnp_read_config(dev, 0x24) | 0x04);

	/* Enable LPC interface */
	dev = PNP_DEV(port, SCH5545_LDN_LPC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);
	/* Set LPC BAR mask */
	pnp_write_config(dev, SCH5545_BAR_LPC_IF, 0x01);
	/* BAR valid, Frame/LDN = 0xc */
	pnp_write_config(dev, SCH5545_BAR_LPC_IF + 1, SCH5545_LDN_LPC | 0x80);
	set_iobase(dev, SCH5545_BAR_LPC_IF, port);

	/* Enable Runtime Registers */

	/* The Runtime Registers BAR is 0x40 long */
	pnp_write_config(dev, SCH5545_BAR_RUNTIME_REG, 0x3f);
	/* BAR valid, Frame/LDN = 0xa */
	pnp_write_config(dev, SCH5545_BAR_RUNTIME_REG + 1, SCH5545_LDN_RR | 0x80);

	/* Map Runtime Registers */
	set_iobase(dev, SCH5545_BAR_RUNTIME_REG, SCH5545_RUNTIME_REG_BASE);
	dev = PNP_DEV(port, SCH5545_LDN_RR);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);

	/* Set LED color and indicate BIOS has reached code fetch phase */
	sch5545_set_led(SCH5545_RUNTIME_REG_BASE, SCH5545_LED_COLOR_GREEN,
			SCH5545_LED_BLINK_ON);

	/* Configure EMI */
	dev = PNP_DEV(port, SCH5545_LDN_LPC);
	pnp_set_logical_device(dev);
	/* EMI BAR has 11 registers, but vendor sets the mask to 0xf */
	pnp_write_config(dev, SCH5545_BAR_EM_IF, 0x0f);
	/* BAR valid, Frame/LDN = 0x00 */
	pnp_write_config(dev, SCH5545_BAR_EM_IF + 1, SCH5545_LDN_EMI | 0x80);
	set_iobase(dev, SCH5545_BAR_EM_IF, SCH5545_EMI_BASE);

	pnp_exit_conf_state(dev);
}

void sch5545_enable_uart(unsigned int port, unsigned int uart_no)
{
	pnp_devfn_t dev;

	if (uart_no > 1)
		return;

	/* Configure serial port */
	dev = PNP_DEV(port, SCH5545_LDN_LPC);
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	/* Set UART BAR mask to 0x07 (8 registers) */
	pnp_write_config(dev, SCH5545_BAR_UART1 + (4 * uart_no), 0x07);
	/* Set BAR valid, Frame/LDN = UART1/2 LDN 0x07/0x08 */
	pnp_write_config(dev, SCH5545_BAR_UART1 + (4 * uart_no) + 1,
			 (SCH5545_LDN_UART1 + uart_no) | 0x80);
	set_iobase(dev, SCH5545_BAR_UART1 + (4 * uart_no), (uart_no == 1) ? 0x2f8 : 0x3f8);
	/* IRQ 3 for UART2, IRQ4 for UART1 */
	set_irq(dev, SCH5545_LDN_UART1 + uart_no, 4 - uart_no);

	dev = PNP_DEV(port, SCH5545_LDN_UART1 + uart_no);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);
	pnp_write_config(dev, SCH5545_UART_CONFIG_SELECT, SCH5545_UART_POWER_VCC);

	pnp_exit_conf_state(dev);
}

int sch5545_get_gpio(uint8_t sio_port, uint8_t gpio)
{
	pnp_devfn_t dev;
	uint16_t runtime_reg_base;
	uint8_t gpio_bank, gpio_num;

	gpio_bank = gpio / 10;
	gpio_num = gpio % 10;
	/*
	 * GPIOs are divided into banks of 8 GPIOs (kind of). Each group starts at decimal
	 * base, i.e. 8 GPIOs from GPIO000, 8 GPIOs from GPIO010, etc., up to GPIO071 and
	 * GPIO072 which are an exception (only two GPIOs in the bank 7).
	 */
	if (gpio_num > 7)
		return -1;
	else if (gpio_bank == 7 && gpio_num > 1)
		return -1;
	else if (gpio_bank > 7)
		return -1;

	dev = PNP_DEV(sio_port, SCH5545_LDN_LPC);
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);

	runtime_reg_base = pnp_read_config(dev, SCH5545_BAR_RUNTIME_REG + 2);
	runtime_reg_base |= pnp_read_config(dev, SCH5545_BAR_RUNTIME_REG + 3) << 8;

	pnp_exit_conf_state(dev);

	if (runtime_reg_base == 0)
		return -1;

	outb(gpio_bank * 8 + gpio_num, runtime_reg_base + SCH5545_RR_GPIO_SEL);

	return inb(runtime_reg_base + SCH5545_RR_GPIO_READ) & 1;
}
