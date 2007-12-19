/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


/*----------------------------------------------------------------------------
 *			TYPEDEFS, DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/* Single CPU system? */
#if (CONFIG_MAX_PHYSICAL_CPUS == 1)
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

	dev = PCI_DEV(CBB, CDB, 0);
	nodes = ((pci_read_config32(dev, 0x60)>>4) & 7) ;
#if CONFIG_MAX_PHYSICAL_CPUS > 8
	nodes += (((pci_read_config32(dev, 0x160)>>4) & 7)<<3);
#endif
	nodes++;

	return nodes;
}

static void enable_apic_ext_id(u32 node)
{
	u32 val;
	val = pci_read_config32(NODE_HT(node), 0x68);
	val |= (HTTC_APIC_EXT_SPUR | HTTC_APIC_EXT_ID | HTTC_APIC_EXT_BRD_CST);
	pci_write_config32(NODE_HT(node), 0x68, val);
}


static void setup_link_trans_cntrl()
{
	/* FIXME: Not sure that this belongs here but it is HT related */
	u32 val;
	val = pci_read_config32(NODE_HT(0), 0x68);
	val |= 0x00206800;	// DSNpReqLimit, LimitCldtCfg, BufRefPri, RespPassPW per BKDG;
	pci_write_config32(NODE_HT(0), 0x68, val);
}




/**
 * void AMD_CB_EventNotify (u8 evtClass, u16 event, const u8 *pEventData0)
 *
 *  Needs to be fixed to output the debug structures.
 *
 */
void AMD_CB_EventNotify (u8 evtClass, u16 event, const u8 *pEventData0)
{
	printk_debug("AMD_CB_EventNotify()\n");
	printk_debug("event class: %02x event: %04x\n", evtClass, event);

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
 *  AMD HT init LinuxBIOS wrapper
 *
 */
void amd_ht_init(struct sys_info *sysinfo)
{
	AMD_HTBLOCK ht_wrapper = {
		NULL,	// u8 **topolist;
		0,	// u8 AutoBusStart;
		32,	// u8 AutoBusMax;
		6,	// u8 AutoBusIncrement;
		NULL,	// BOOL (*AMD_CB_IgnoreLink)();
		NULL,	// BOOL (*AMD_CB_OverrideBusNumbers)();
		NULL,	// BOOL (*AMD_CB_ManualBUIDSwapList)();
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

	printk_debug("Enter amd_ht_init()\n");
	amdHtInitialize(&ht_wrapper);
	printk_debug("Exit amd_ht_init()\n");


}




