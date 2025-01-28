/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>

#define GLOBAL_DEV PNP_DEV(CONFIG_SUPERIO_NUVOTON_PNP_BASE, 0)
#define SERIAL_DEV PNP_DEV(CONFIG_SUPERIO_NUVOTON_PNP_BASE, NCT6779D_SP1)
#define ACPI_DEV   PNP_DEV(CONFIG_SUPERIO_NUVOTON_PNP_BASE, NCT6779D_ACPI)

void mainboard_late_rcba_config(void)
{
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (INTB << D31IP_SIP2) |
			(INTC << D31IP_SMIP) | (INTB << D31IP_SIP);
	RCBA32(D22IP) = (INTB << D22IP_KTIP)   | (INTC << D22IP_IDERIP) |
			(INTB << D22IP_MEI2IP) | (INTA << D22IP_MEI1IP);

	DIR_ROUTE(D31IR, PIRQA, PIRQC, PIRQD, PIRQA);
	DIR_ROUTE(D29IR, PIRQH, PIRQD, PIRQA, PIRQC);
	DIR_ROUTE(D27IR, PIRQG, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D26IR, PIRQA, PIRQF, PIRQC, PIRQD);
	DIR_ROUTE(D25IR, PIRQE, PIRQF, PIRQG, PIRQH);
	DIR_ROUTE(D22IR, PIRQA, PIRQD, PIRQC, PIRQB);
}

void bootblock_mainboard_early_init(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin states */
	pnp_write_config(GLOBAL_DEV, 0x1a, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x40);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x00);

	/* Power RAM in S3 */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
