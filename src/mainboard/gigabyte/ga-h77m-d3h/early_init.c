/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SUPERIO_BASE 0x2e
#define SIO_GPIO PNP_DEV(SUPERIO_BASE, IT8728F_GPIO)
#define SERIAL_DEV PNP_DEV(SUPERIO_BASE, 0x01)

void bootblock_mainboard_early_init(void)
{
	/* Initialize SuperIO */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	ite_reg_write(SIO_GPIO, 0xEF, 0x7E); // magic SIO disable reboot

	/* FIXME: These values could be configured in ramstage */
	ite_reg_write(SIO_GPIO, 0x25, 0x40); // gpio pin function -> gp16
	ite_reg_write(SIO_GPIO, 0x27, 0x10); // gpio pin function -> gp34
	ite_reg_write(SIO_GPIO, 0x2c, 0x80); // smbus isolation on parallel port
	ite_reg_write(SIO_GPIO, 0x62, 0x0a); // simple iobase 0xa00
	ite_reg_write(SIO_GPIO, 0x72, 0x20); // watchdog timeout clear!
	ite_reg_write(SIO_GPIO, 0x73, 0x00); // watchdog timeout clear!
	ite_reg_write(SIO_GPIO, 0xcb, 0x00); // simple io set4 direction -> in
	ite_reg_write(SIO_GPIO, 0xe9, 0x27); // bus select disable
	ite_reg_write(SIO_GPIO, 0xf0, 0x10); // ?
	ite_reg_write(SIO_GPIO, 0xf1, 0x42); // ?
	ite_reg_write(SIO_GPIO, 0xf6, 0x1c); // hwmon alert beep -> gp36(pin12)

	/* EC SIO settings */
	ite_reg_write(IT8728F_EC, 0xf1, 0xc0);
	ite_reg_write(IT8728F_EC, 0xf6, 0xf0);
	ite_reg_write(IT8728F_EC, 0xf9, 0x48);
	ite_reg_write(IT8728F_EC, 0x60, 0x0a);
	ite_reg_write(IT8728F_EC, 0x61, 0x30);
	ite_reg_write(IT8728F_EC, 0x62, 0x0a);
	ite_reg_write(IT8728F_EC, 0x63, 0x20);
	ite_reg_write(IT8728F_EC, 0x30, 0x01);
}
