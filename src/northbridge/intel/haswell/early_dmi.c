/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

static void dmi_print_link_status(int loglevel)
{
	const uint16_t dmilsts = dmibar_read16(DMILSTS);
	printk(loglevel, "DMI: Running at Gen%u x%u\n", dmilsts & 0xf, dmilsts >> 4 & 0x1f);
}

#define RETRAIN	(1 << 5)

#define LTRN	(1 << 11)

static void dmi_setup_physical_layer(void)
{
	/* Program DMI AFE settings, which are needed for DMI to work */
	peg_dmi_recipe(false, 0);

	/* Additional DMI programming steps */
	dmibar_setbits32(0x258, 1 << 29);
	dmibar_clrsetbits32(0x208, 0x7ff, 0x6b5);
	dmibar_clrsetbits32(0x22c, 0xffff, 0x2020);

	/* Write SA reference code version */
	dmibar_write32(0x71c, 0x0000000f);
	dmibar_write32(0x720, 0x01060200);

	/* We also have to bring up the PCH side of the DMI link */
	pch_dmi_setup_physical_layer();

	/* Write-once settings */
	dmibar_clrsetbits32(DMILCAP, 0x3f00f, 2 << 0);

	printk(BIOS_DEBUG, "Retraining DMI at Gen2 speeds...\n");
	dmi_print_link_status(BIOS_DEBUG);

	/* Retrain link */
	dmibar_setbits16(DMILCTL, RETRAIN);
	do {} while (dmibar_read16(DMILSTS) & LTRN);
	dmi_print_link_status(BIOS_DEBUG);

	/* Retrain link again for DMI Gen2 speeds */
	dmibar_setbits16(DMILCTL, RETRAIN);
	do {} while (dmibar_read16(DMILSTS) & LTRN);
	dmi_print_link_status(BIOS_INFO);
}

#define VC_ACTIVE	(1U << 31)

#define VCNEGPND	(1 << 1)

#define DMI_VC_CFG(vcid, tcmap)	(VC_ACTIVE | ((vcid) << 24) | (tcmap))

static void dmi_tc_vc_mapping(void)
{
	printk(BIOS_DEBUG, "Programming SA  DMI VC/TC mappings...\n");

	if (CONFIG(INTEL_LYNXPOINT_LP))
		dmibar_setbits8(0xa78, 1 << 1);

	/* Each TC is mapped to one and only one VC */
	const u32 vc0 = DMI_VC_CFG(0, (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 0));
	const u32 vc1 = DMI_VC_CFG(1, (1 << 1));
	const u32 vcp = DMI_VC_CFG(2, (1 << 2));
	const u32 vcm = DMI_VC_CFG(7, (1 << 7));
	dmibar_write32(DMIVC0RCTL, vc0);
	dmibar_write32(DMIVC1RCTL, vc1);
	dmibar_write32(DMIVCPRCTL, vcp);
	dmibar_write32(DMIVCMRCTL, vcm);

	/* Set Extended VC Count (EVCC) to 1 if VC1 is active */
	dmibar_clrsetbits8(DMIPVCCAP1, 7, !!(vc1 & VC_ACTIVE));

	/*
	 * We also have to program the PCH side of the DMI link. Since both ends
	 * must use the same Virtual Channel settings, we pass them as arguments.
	 */
	pch_dmi_tc_vc_mapping(vc0, vc1, vcp, vcm);

	printk(BIOS_DEBUG, "Waiting for SA  DMI VC negotiation... ");
	do {} while (dmibar_read16(DMIVC0RSTS) & VCNEGPND);
	do {} while (dmibar_read16(DMIVC1RSTS) & VCNEGPND);
	do {} while (dmibar_read16(DMIVCPRSTS) & VCNEGPND);
	do {} while (dmibar_read16(DMIVCMRSTS) & VCNEGPND);
	printk(BIOS_DEBUG, "done!\n");
}

void dmi_early_init(void)
{
	dmi_setup_physical_layer();
	dmi_tc_vc_mapping();
}
