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

#ifndef  _RD890_CFG_H_
#define _RD890_CFG_H_

#include "NbPlatform.h"

/* platform dependent configuration default value */

/**
 * Path from CPU to NB
 * [0..7]   - Node  (0..8)
 * [8..11]  - Link  (0..3)
 * [12..15] - Sublink (1..2), If NB connected to full link than Sublink should be set to 0.
 */
#ifndef DEFAULT_HT_PATH
#if CONFIG_CPU_AMD_AGESA_FAMILY10
#define DEFAULT_HT_PATH		{0x0, 0x3}
#endif
#if CONFIG_CPU_AMD_AGESA_FAMILY15
#define DEFAULT_HT_PATH		{0x0, 0x1}
#endif
#endif

/**
 * Bitmap of enabled ports on NB #0/1/2/3
 * Bit[0] - Reserved
 * Bit[1] - Reserved
 * Bit[2] - Enable PCIe port 2
 * Bit[3] - Enable PCIe port 3
 * Bit[4] - Enable PCIe port 4
 * Bit[5] - Enable PCIe port 5
 * Bit[6] - Enable PCIe port 2
 * Bit[7] - Enable PCIe port 7
 * Bit[8] - Reserved
 * Bit[9] - Enable PCIe port 9
 * Bit[10]- Enable PCIe port 10
 * Bit[11]- Enable PCIe port 11
 * Bit[12]- Enable PCIe port 12
 * Bit[13]- Enable PCIe port 13
 * Example:
 *  port_enable = 0x14
 *  Port 2 and 4 enabled for training/initialization
 */
#ifndef DEFAULT_PORT_ENABLE_MAP
#define DEFAULT_PORT_ENABLE_MAP		0x0014
#endif

/**
 * Bitmap of ports that have slot or onboard device connected.
 * Example force PCIe Gen1 supporton port 2 and 4  (DEFAULT_PORT_ENABLE_MAP = BIT2 | BIT4)
 * #define DEFAULT_PORT_FORCE_GEN1        0x604
 */
#ifndef DEFAULT_PORT_FORCE_GEN1
#define DEFAULT_PORT_FORCE_GEN1		0x0
#endif

/**
 * Bitmap of ports that have server hotplug support
 */
#ifndef DEFAULT_HOTPLUG_SUPPORT
#define DEFAULT_HOTPLUG_SUPPORT		0x0
#endif

#ifndef DEFAULT_HOTPLUG_DESCRIPTOR
#define DEFAULT_HOTPLUG_DESCRIPTOR		{0, 0, 0, 0, 0, 0, 0, 0}
#endif

#ifndef DEFAULT_TEMPMMIO_BASE_ADDRESS
#define DEFAULT_TEMPMMIO_BASE_ADDRESS		0xD0000000
#endif

/**
 * Default GPP1 core configuraton on NB #0/1/2/3.
 *  2  x8 slot, GFX_CONFIG_AABB
 *  1 x16 slot, GFX_CONFIG_AAAA
 */
#ifndef DEFAULT_GPP1_CONFIG
#define DEFAULT_GPP1_CONFIG		GFX_CONFIG_AABB
#endif

/**
 * Default GPP2 core configuraton on NB #0/1/2/3.
 *  2  x8 slot, GFX_CONFIG_AABB
 *  1 x16 slot, GFX_CONFIG_AAAA
 */
#ifndef DEFAULT_GPP2_CONFIG
#define DEFAULT_GPP2_CONFIG		GFX_CONFIG_AABB
#endif

/**
 * Default GPP3a core configuraton on NB #0/1/2/3.
 * 4:2:0:0:0:0   - GPP_CONFIG_GPP420000, 0x1
 * 4:1:1:0:0:0   - GPP_CONFIG_GPP411000, 0x2
 * 2:2:2:0:0:0   - GPP_CONFIG_GPP222000, 0x3
 * 2:2:1:1:0:0   - GPP_CONFIG_GPP221100, 0x4
 * 2:1:1:1:1:0   - GPP_CONFIG_GPP211110, 0x5
 * 1:1:1:1:1:1   - GPP_CONFIG_GPP111111, 0x6
 */
#ifndef DEFAULT_GPP3A_CONFIG
#define DEFAULT_GPP3A_CONFIG		GPP_CONFIG_GPP111111
#endif


/**
 * Default HT Transmitter de-emphasis setting
 */
#ifndef DEFAULT_HT_DEEMPASIES
#define DEFAULT_HT_DEEMPASIES		0x3
#endif

/**
 * Default APIC nterrupt base for IOAPIC
 */
#ifndef DEFAULT_APIC_INTERRUPT_BASE
#define DEFAULT_APIC_INTERRUPT_BASE	24
#endif


#define DEFAULT_PLATFORM_CONFIG(name) \
	NB_PLATFORM_CONFIG name = { \
		DEFAULT_PORT_ENABLE_MAP, \
		DEFAULT_PORT_FORCE_GEN1, \
		DEFAULT_HOTPLUG_SUPPORT, \
		DEFAULT_HOTPLUG_DESCRIPTOR, \
		DEFAULT_TEMPMMIO_BASE_ADDRESS, \
		DEFAULT_GPP1_CONFIG, \
		DEFAULT_GPP2_CONFIG, \
		DEFAULT_GPP3A_CONFIG, \
		DEFAULT_HT_DEEMPASIES, \
		/*DEFAULT_HT_PATH,*/ \
		DEFAULT_APIC_INTERRUPT_BASE, \
	}

/**
 * Platform configuration
 */
typedef struct {
	UINT16  PortEnableMap;            ///< Bitmap of enabled ports
	UINT16  PortGen1Map;              ///< Bitmap of ports to disable Gen2
	UINT16  PortHotplugMap;           ///< Bitmap of ports support hotplug
	UINT8   PortHotplugDescriptors[8];///< Ports Hotplug descriptors
	UINT32  TemporaryMmio;            ///< Temporary MMIO
	UINT32  Gpp1Config;               ///< Default PCIe GFX core configuration
	UINT32  Gpp2Config;               ///< Default PCIe GPP2 core configuration
	UINT32  Gpp3aConfig;              ///< Default PCIe GPP3a core configuration
	UINT8   NbTransmitterDeemphasis;  ///< HT transmitter de-emphasis level
	//	HT_PATH NbHtPath;                 ///< HT path to NB
	UINT8   GlobalApicInterruptBase;  ///< Global APIC interrupt base that is used in MADT table for IO APIC.
} NB_PLATFORM_CONFIG;

/**
 * Bridge CIMx configuration
 */
void rd890_cimx_config(AMD_NB_CONFIG_BLOCK *pConfig, NB_CONFIG *nbConfig, HT_CONFIG *htConfig, PCIE_CONFIG *pcieConfig);

#endif //_RD890_CFG_H_
