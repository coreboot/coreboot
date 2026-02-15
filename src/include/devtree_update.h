/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVTREE_UPDATE_H__
#define __DEVTREE_UPDATE_H__

/*
 * Optional mainboard hook to update devicetree runtime enable/disable state
 * based on CMOS/NVRAM settings.
 *
 * This function is called early in ramstage at BS_PRE_DEVICE.
 */
void devtree_update(void);

#endif /* __DEVTREE_UPDATE_H__ */
