/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * SMM routines common to Nuvoton NCTxxxx SIO chips,
 * except 5104, 6687, npcd378, wpcm450.
 */

/* SMM code needs to be kept simple */
#define __SIMPLE_DEVICE__

#include <assert.h>
#include <acpi/acpi.h>
#include <arch/io.h>
#include <device/pnp_ops.h>
#include "nuvoton.h"

#define ACPI_DEV   PNP_DEV(CONFIG_SUPERIO_NUVOTON_PNP_BASE, NCT677X_ACPI)

void nuvoton_smi_sleep(u8 slp_typ)
{
	if (slp_typ != ACPI_S5)
		return;

	_Static_assert(CONFIG_SUPERIO_NUVOTON_PNP_BASE != 0,
	       "Kconfig SUPERIO_NUVOTON_PNP_BASE must be set!");
	nuvoton_pnp_enter_conf_state(ACPI_DEV);
	pnp_set_logical_device(ACPI_DEV);
	pnp_unset_and_set_config(ACPI_DEV, 0xe0, BIT(6), 0); /* KB wakeup off */
	pnp_unset_and_set_config(ACPI_DEV, 0xe6, 0, BIT(4)); /* Power off */
	nuvoton_pnp_exit_conf_state(ACPI_DEV);
}
