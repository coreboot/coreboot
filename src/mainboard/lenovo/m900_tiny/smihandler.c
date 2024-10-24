/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Enable blinking power LED when entering S3 or S4 */
	switch (slp_typ) {
	case ACPI_S3:
	case ACPI_S4:
		/* Configure GPIO community 1 PWM duty cycle to 50% */
		pcr_rmw32(PID_GPIOCOM1, 0x204, 0xffffff00, 0x7f);
		/* Set the software update flag */
		pcr_or32(PID_GPIOCOM1, 0x204, (1 << 30));
		break;
	default:
		break;
	}
}
