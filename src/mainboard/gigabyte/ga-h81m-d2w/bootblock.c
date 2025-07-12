/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/lynxpoint/pch.h>

#include <device/pnp_ops.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8625e/it8625e.h>

#define UART_DEV PNP_DEV(0x2e, 0x1)
#define SIO_GPIO PNP_DEV(0x2e, IT8625E_GPIO)

void mainboard_config_superio(void)
{
	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);
	ite_reg_write(SIO_GPIO, 0xEF, 0x7E); // magic SIO disable reboot
}
