/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP2_0_GRAPHICS_H_
#define _FSP2_0_GRAPHICS_H_

#include <types.h>

/*
 * Report the fsp_graphics_info_guid HOB to framebuffer info.
 *
 * Must be called after PCI enumeration to make sure that the BAR
 * doesn't change any more.
 */
void fsp_report_framebuffer_info(const uintptr_t framebuffer_bar);

#endif /* _FSP2_0_GRAPHICS_H_ */
