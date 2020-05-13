/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <security/tpm/tis.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
}

int get_write_protect_state(void)
{
	return 0;
}

int tis_plat_irq_status(void)
{
	return 0;
}
