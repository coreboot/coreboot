/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel ICH8-M LPC Bridge - IOAPIC enable
 */

#include <device/pci_ops.h>
#include <southbridge/intel/i82801hx/i82801hx.h>

/* ================================================================== */
/* IOAPIC Enable                                                      */
/* ================================================================== */

void i82801hx_enable_ioapic(void)
{
	/* Enable IOAPIC. Keep APIC Range Select at zero. */
	RCBA8(OIC) = 0x03;

	/* Spec requires readback when bit 0 changes. */
	(void)RCBA8(OIC);
}
