/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include "ec/compal/ene932/ec.h"

void mainboard_late_rcba_config(void)
{
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P2IP  WLAN   INTA -> PIRQB
	 * D28IP_P3IP  ETH0   INTC -> PIRQD
	 * D29IP_E1P   EHCI1  INTA -> PIRQE
	 * D26IP_E2P   EHCI2  INTA -> PIRQE
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQG
	 * D31IP_TTIP  THRT   INTC -> PIRQH
	 * D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
	 *
	 * Trackpad DVT PIRQA (16)
	 * Trackpad DVT PIRQE (20)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (NOINT << D28IP_P1IP) | (INTA << D28IP_P2IP) |
		(INTC << D28IP_P3IP) | (NOINT << D28IP_P4IP) |
		(NOINT << D28IP_P5IP) | (NOINT << D28IP_P6IP) |
		(NOINT << D28IP_P7IP) | (NOINT << D28IP_P8IP);
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

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* TODO: Confirm if nortbridge_fill_pei_data() gets .system_type right (should be 0) */
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled power  USB oc pin  */
	{ 0, 0, -1 }, /* P0: Empty */
	{ 1, 0, 0 }, /* P1: Left USB 1  (OC0) */
	{ 1, 0, 1 }, /* P2: Left USB 2  (OC1) */
	{ 1, 0, 1 }, /* P3: Left USB 3  (OC1) */
	{ 0, 0, -1 }, /* P4: Empty */
	{ 0, 0, -1 }, /* P5: Empty */
	{ 0, 0, -1 }, /* P6: Empty */
	{ 0, 0, -1 }, /* P7: Empty */
	/* Empty and onboard Ports 8-13, set to un-used pin OC4 */
	{ 1, 0, -1 }, /* P8: MiniPCIe (WLAN) (no OC) */
	{ 0, 0, -1 }, /* P9: Empty */
	{ 1, 0, -1 }, /* P10: Camera (no OC) */
	{ 0, 0, -1 }, /* P11: Empty */
	{ 0, 0, -1 }, /* P12: Empty */
	{ 0, 0, -1 }, /* P13: Empty */
};
