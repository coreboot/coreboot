/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _PLATFORM_CFG_H_
#define _PLATFORM_CFG_H_


/* northbridge customize options */
/**
 * Max number of northbridges in the system
 */
#define MAX_NB_COUNT		1 //TODO: only 1 NB tested

/**
 *  Enable check for PCIe endpoint to be ready for PCI enumeration.
 *
 */
//#define  EPREADY_WORKAROUND_DISABLED

/**
 *  Enable IOMMU support. Initialize IOMMU subsystem, generate IVRS ACPI table.
 *
 */
#define  IOMMU_SUPPORT_DISABLE //TODO: enable it

/**
 *  Disable server PCIe hotplug support.
 */

//#define HOTPLUG_SUPPORT_DISABLED

/**
 *  Disable support for device number remapping for PCIe portsserver PCIe hotplug support.
 */

//#define DEVICE_REMAP_DISABLE

#endif //_PLATFORM_CFG_H_
