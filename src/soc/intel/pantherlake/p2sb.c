/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/p2sb.h>
#include <types.h>

void p2sb_soc_get_sb_mask(uint32_t *ep_mask, size_t count)
{
	uint32_t mask;

	if (count != P2SB_EP_MASK_MAX_REG) {
		printk(BIOS_ERR, "Unable to program EPMASK registers\n");
		return;
	}

	/* Remove the host accessing right to PSF register range.
	 * Set p2sb PCI offset EPMASK5 [29, 28, 27, 26] to disable Sideband
	 * access for PCI Root Bridge.
	 */
	mask = BIT(29) | BIT(28) | BIT(27)  | BIT(26);

	ep_mask[P2SB_EP_MASK_5_REG] = mask;

	/*
	 * Set p2sb PCI offset EPMASK7 [31, 30] to disable Sideband
	 * access for Broadcast and Multicast.
	 */
	mask = BIT(31) | BIT(30);

	ep_mask[P2SB_EP_MASK_7_REG] = mask;
}
