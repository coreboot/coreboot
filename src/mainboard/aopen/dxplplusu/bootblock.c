/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <southbridge/intel/i82801dx/i82801dx.h>
#include <superio/smsc/lpc47m10x/lpc47m10x.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47M10X2_SP1)
#define GPIO_DEV PNP_DEV(0x2e, LPC47M10X2_PME)
#define PME_BASE 0xe00

/*
 * JP3
 * 1-2 0:1f.0 0xd4 GEN_STA SAFE_MODE = 0
 * 2-3 0:1f.0 0xd4 GEN_STA SAFE_MODE = 1
 */

static void enable_gpio(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);

	/* These should go under sb/. */
	pci_write_config8(dev, COM_DEC, 0x10);
	pci_write_config16(dev, LPC_EN, 0x300F);

	pnp_enter_conf_state(GPIO_DEV);
	pnp_set_logical_device(GPIO_DEV);
	pnp_set_enable(GPIO_DEV, 0);
	pnp_set_iobase(GPIO_DEV, PNP_IDX_IO0, PME_BASE);
	pnp_set_enable(GPIO_DEV, 1);
	pnp_exit_conf_state(GPIO_DEV);

	pci_write_config16(dev, GEN1_DEC, PME_BASE | 0x01);
}

static void gpio_write(u8 addr, u8 val)
{
	outb(val, PME_BASE + addr);
}

static void gpio_read_write(u8 addr, u8 amask, u8 omask)
{
	u8 val = inb(PME_BASE + addr);
	val &= amask;
	val |= omask;
	outb(val, PME_BASE + addr);
}

static void configure_gpios(void)
{
	/* GP10-17 */
	gpio_write(0x23, 0x00); /* push-pull output */
	gpio_write(0x24, 0x81); /* open-drain but input ? */
	gpio_write(0x25, 0x01);
	gpio_write(0x26, 0x01);
	gpio_write(0x27, 0x01);
	gpio_write(0x28, 0x01);
	gpio_write(0x29, 0x01);
	gpio_write(0x2a, 0x00); /* push-pull output */

	/* GP20-27 */
	gpio_write(0x2b, 0x01);
	gpio_write(0x2c, 0x0c); /* nDS1 floppy select */
	gpio_write(0x2d, 0x00); /* push-pull output */
	gpio_write(0x2f, 0x01);
	gpio_write(0x30, 0x01);
	gpio_write(0x31, 0x01);
	gpio_write(0x32, 0x04); /* nIO_SMI */

	/* GP30-37 */
	gpio_write(0x33, 0x04); /* FAN_TACH2 */
	gpio_write(0x34, 0x01);
	gpio_write(0x35, 0x01);
	gpio_write(0x36, 0x01);
	gpio_write(0x37, 0x01);
	gpio_write(0x38, 0x01);
	gpio_write(0x39, 0x04); /* nKBDRESET */
	gpio_write(0x3a, 0x04); /* A20M */

	/* GP40-43 */
	gpio_write(0x3b, 0x84); /* DRVDEN0 open-drain */
	gpio_write(0x3c, 0x04); /* DRVDEN1 push/pull */
	gpio_write(0x3d, 0x84); /* nIO_PME open-drain */
	gpio_write(0x3e, 0x01);

	/* GP50-GP57 to UART signals */
	gpio_write(0x3f, 0x05);
	gpio_write(0x40, 0x05);
	gpio_write(0x41, 0x05);
	gpio_write(0x42, 0x04);
	gpio_write(0x43, 0x05);
	gpio_write(0x44, 0x04);
	gpio_write(0x45, 0x05);
	gpio_write(0x46, 0x04);

	/* GP60-GP61 */
	gpio_write(0x47, 0x86); /* LED1 invert open-drain */
	gpio_write(0x48, 0x81); /* open-drain but input */

	/* Set initial output bits */
	gpio_read_write(0x4b, 0xff, 0x81);	/* GP1 */
	gpio_read_write(0x4c, ~0x01, 0x00);	/* GP2 */
	gpio_read_write(0x4d, 0xff, 0x80);	/* GP3 */
	gpio_read_write(0x4e, ~0x08, 0x00);	/* GP4 */

	/* LED1 off */
	gpio_write(0x5d, 0x00);
}

void bootblock_mainboard_early_init(void)
{
	enable_gpio();
	configure_gpios();

	/* Get the serial port configured. */
	lpc47m10x_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
