/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	/* TODO: add Chrome specific gpios */
}

int get_recovery_mode_switch(void)
{
	/* TODO: use Chrome EC switches when EC support is added */
	return 0;
}
