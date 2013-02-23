/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
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

#include <cpu/x86/msr.h>
#include <console/console.h>
#include <northbridge/amd/amdfam10/amdfam10.h>

/*----------------------------------------------------------------------------
 *			TYPEDEFS, DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/* Single CPU system? */
#if CONFIG_MAX_PHYSICAL_CPUS
	#define HT_BUILD_NC_ONLY 1
#endif

/* Debugging Options */
#define AMD_DEBUG 1
//#define AMD_DEBUG_ERROR_STOP 1

/*----------------------------------------------------------------------------
 *				MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#undef FILECODE
#define FILECODE 0xFF01
#include "comlib.h"
#include "h3gtopo.h"
#include "h3finit.h"

/* include the main HT source file */
#include "h3finit.c"


/*----------------------------------------------------------------------------
 *			LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* FIXME: Find a better place for these pre-ram functions. */
#define NODE_HT(x) NODE_PCI(x,0)
#define NODE_MP(x) NODE_PCI(x,1)
#define NODE_MC(x) NODE_PCI(x,3)
#define NODE_LC(x) NODE_PCI(x,4)

static  u32 get_nodes(void)
{
	device_t dev;
	u32 nodes;

	dev = PCI_DEV(CONFIG_CBB, CONFIG_CDB, 0);
	nodes = ((pci_read_config32(dev, 0x60)>>4) & 7) ;
#if CONFIG_MAX_PHYSICAL_CPUS > 8
	nodes += (((pci_read_config32(dev, 0x160)>>4) & 7)<<3);
#endif
	nodes++;

	return nodes;
}


/**
 * void AMD_CB_EventNotify (u8 evtClass, u16 event, const u8 *pEventData0)
 */
static void AMD_CB_EventNotify (u8 evtClass, u16 event, const u8 *pEventData0)
{
	u8 i;

	printk(BIOS_DEBUG, "AMD_CB_EventNotify()\n");
	printk(BIOS_DEBUG, " event class: %02x\n event: %04x\n data: ", evtClass, event);

	for (i = 0; i < *pEventData0; i++) {
		printk(BIOS_DEBUG, " %02x ", *(pEventData0 + i));
	}
	printk(BIOS_DEBUG, "\n");

}

/**
 * void getAmdTopolist(u8 ***p)
 *
 *  point to the stock topo list array
 *
 */
void getAmdTopolist(u8 ***p)
{
	*p = (u8 **)amd_topo_list;
}


/**
 * void amd_ht_init(struct sys_info *sysinfo)
 *
 *  AMD HT init coreboot wrapper
 *
 */
static void amd_ht_init(struct sys_info *sysinfo)
{

	AMD_HTBLOCK ht_wrapper = {
		NULL,	// u8 **topolist;
		0,	// u8 AutoBusStart;
		32,	// u8 AutoBusMax;
		6,	// u8 AutoBusIncrement;
		NULL,	// BOOL (*AMD_CB_IgnoreLink)();
		NULL,	// BOOL (*AMD_CB_OverrideBusNumbers)();
		AMD_CB_ManualBUIDSwapList,	// BOOL (*AMD_CB_ManualBUIDSwapList)();
		NULL,	// void (*AMD_CB_DeviceCapOverride)();
		NULL,	// void (*AMD_CB_Cpu2CpuPCBLimits)();
		NULL,	// void (*AMD_CB_IOPCBLimits)();
		NULL,	// BOOL (*AMD_CB_SkipRegang)();
		NULL,	// BOOL (*AMD_CB_CustomizeTrafficDistribution)();
		NULL,	// BOOL (*AMD_CB_CustomizeBuffers)();
		NULL,	// void (*AMD_CB_OverrideDevicePort)();
		NULL,	// void (*AMD_CB_OverrideCpuPort)();
		AMD_CB_EventNotify	// void (*AMD_CB_EventNotify) ();
	};

	printk(BIOS_DEBUG, "Enter amd_ht_init()\n");
	amdHtInitialize(&ht_wrapper);
	printk(BIOS_DEBUG, "Exit amd_ht_init()\n");


}




