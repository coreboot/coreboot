/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <gpio.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include "ec/google/chromeec/ec.h"

#include <southbridge/intel/bd82x6x/chip.h>

void mainboard_pch_lpc_setup(void)
{
	/* Enable additional 0x200..0x207 for EC */
	pci_or_config16(PCH_LPC_DEV, LPC_EN, GAMEL_LPC_EN);
}

void mainboard_late_rcba_config(void)
{
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P3IP  WLAN   INTA -> PIRQB
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQF
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQH
	 * D31IP_TTIP  THRT   INTC -> PIRQA
	 * D27IP_ZIP   HDA    INTA -> PIRQA (MSI)
	 *
	 * TRACKPAD                -> PIRQE (Edge Triggered)
	 * TOUCHSCREEN             -> PIRQG (Edge Triggered)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTA << D28IP_P3IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQB, PIRQH, PIRQA, PIRQC);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQB, PIRQC, PIRQD, PIRQE);
	DIR_ROUTE(D27IR, PIRQA, PIRQH, PIRQA, PIRQB);
	DIR_ROUTE(D26IR, PIRQF, PIRQE, PIRQG, PIRQH);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);
}

static unsigned int get_spd_index(void)
{
	const gpio_t gpio_vector[] = {41, 42, 43, 10};
	return gpio_base2_value(gpio_vector, ARRAY_SIZE(gpio_vector));
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* TODO: Confirm if nortbridge_fill_pei_data() gets .system_type right (should be 0) */
}

void mb_get_spd_map(struct spd_info *spdi)
{
	/* LINK has 2 channels of memory down */
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->addresses[2] = SPD_MEMORY_DOWN;
	spdi->spd_index = get_spd_index();
}

void mainboard_early_init(int s3resume)
{
	if (!s3resume) {
		/* This is the fastest way to let users know
		 * the Intel CPU is now alive.
		 */
		google_chromeec_kbbacklight(100);
	}
}
