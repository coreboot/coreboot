/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

void bootblock_mainboard_early_init(void)
{
	static const pnp_devfn_t GLOBAL_PSEUDO_DEV = PNP_DEV(0x2e, 0);
	static const pnp_devfn_t ACPI_DEV = PNP_DEV(0x2e, NCT6776_ACPI);

	nuvoton_pnp_enter_conf_state(GLOBAL_PSEUDO_DEV);

	/* Select HWM/LED functions instead of floppy functions. */
	pnp_write_config(GLOBAL_PSEUDO_DEV, 0x1c, 0x03);
	pnp_write_config(GLOBAL_PSEUDO_DEV, 0x24, 0x24);

	/* Power RAM in S3. */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	nuvoton_pnp_exit_conf_state(GLOBAL_PSEUDO_DEV);
}
