/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMD_BLOCK_VBT_H_
#define _AMD_BLOCK_VBT_H_

/*
 * On AMD platforms the VBT is called ATOMBIOS and is always part of the
 * VGA Option ROM. As part of the FSP GOP init the ATOMBIOS tables are
 * updated in place. Thus the VBIOS must be loaded into RAM before FSP GOP
 * runs. The address of the VBIOS must be passed to FSP-S using UPDs, but
 * loading of the VBIOS can be delayed until before FSP AFTER_PCI_ENUM
 * notify is called. FSP expects a pointer to the PCI Option Rom instead of
 * a pointer to the ATOMBIOS table directly.
 *
 * Returns a pointer to the VGA Option ROM in DRAM after checking
 * prerequisites for Pre OS Graphics initialization. When returning
 * non NULL the Option ROM might not be loaded at this address yet,
 * but is guaranteed to be present at end of BS_DEV_RESOURCES phase.
 */
void *vbt_get(void);

#endif
