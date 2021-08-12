/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{

}

int get_ec_is_trusted(void)
{
	/* Do not have a Chrome EC involved in entering recovery mode;
	   Always return trusted. */
	return 1;
}
