/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include <device/device.h>

static void hybrid_graphics_init(void)
{
	bool peg, igd;
	u32 reg32;

	early_hybrid_graphics(&igd, &peg);

	if (peg && igd)
		return;

	/* Hide disabled devices */
	reg32 = pci_read_config32(PCI_DEV(0, 0, 0), DEVEN);
	reg32 &= ~(DEVEN_PEG10 | DEVEN_IGD);

	if (peg)
		reg32 |= DEVEN_PEG10;

	if (igd)
		reg32 |= DEVEN_IGD;
	else
		/* Disable IGD VGA decode, no GTT or GFX stolen */
		pci_write_config16(PCI_DEV(0, 0, 0), GGC, 2);

	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN, reg32);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 }, /* P0 left dual conn, OC 0 */
	{ 1, 1, 1 }, /* P1 system onboard USB (eSATA), (EHCI debug), OC 1 */
	{ 1, 2, -1 }, /* P2: wimax / WLAN */
	{ 1, 1, -1 }, /* P3: WWAN, no OC */
	{ 1, 1, -1 }, /* P4: smartcard, no OC */
	{ 1, 1, -1 }, /* P5: ExpressCard, no OC */
	{ 0, 2, -1 }, /* P6: empty */
	{ 0, 2, -1 }, /* P7: to touch panel, no OC */
	{ 1, 1, 4 }, /* P8: left dual conn, OC4 */
	{ 1, 4, 5 }, /* P9: to system subcard back right, (EHCI debug), OC 5 */
	{ 1, 1, -1 }, /* P10: fingerprint reader, no OC */
	{ 1, 2, -1 }, /* P11: bluetooth, no OC. */
	{ 1, 1, -1 }, /* P12: docking, no OC */
	{ 1, 1, -1 }, /* P13: CAMERA (LCD), no OC */
};

void mainboard_early_init(int s3resume)
{
	hybrid_graphics_init();
}
