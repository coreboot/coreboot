/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 */

#include <cpu/x86/msr.h>
#include <console/console.h>
#include "ht_wrapper.h"

/*----------------------------------------------------------------------------
 *			TYPEDEFS, DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/* Single CPU system? */
#if (CONFIG_MAX_PHYSICAL_CPUS == 1)
	/* FIXME
	 * This #define is used by other #included .c files
	 * When set, multiprocessor support is completely disabled
	 */
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

/*----------------------------------------------------------------------------
 *			LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

static const char * event_class_string_decodes[] = {
	[HT_EVENT_CLASS_CRITICAL] = "CRITICAL",
	[HT_EVENT_CLASS_ERROR] = "ERROR",
	[HT_EVENT_CLASS_HW_FAULT] = "HARDWARE FAULT",
	[HT_EVENT_CLASS_WARNING] = "WARNING",
	[HT_EVENT_CLASS_INFO] = "INFO"
};

typedef struct {
	uint32_t code;
	const char *string;
} event_string_decode_t;

static const event_string_decode_t event_string_decodes[] = {
	{ HT_EVENT_COH_EVENTS, "HT_EVENT_COH_EVENTS" },
	{ HT_EVENT_COH_NO_TOPOLOGY, "HT_EVENT_COH_NO_TOPOLOGY" },
	{ HT_EVENT_COH_LINK_EXCEED, "HT_EVENT_COH_LINK_EXCEED" },
	{ HT_EVENT_COH_FAMILY_FEUD, "HT_EVENT_COH_FAMILY_FEUD" },
	{ HT_EVENT_COH_NODE_DISCOVERED, "HT_EVENT_COH_NODE_DISCOVERED" },
	{ HT_EVENT_COH_MPCAP_MISMATCH, "HT_EVENT_COH_MPCAP_MISMATCH" },
	{ HT_EVENT_NCOH_EVENTS, "HT_EVENT_NCOH_EVENTS" },
	{ HT_EVENT_NCOH_BUID_EXCEED, "HT_EVENT_NCOH_BUID_EXCEED" },
	{ HT_EVENT_NCOH_LINK_EXCEED, "HT_EVENT_NCOH_LINK_EXCEED" },
	{ HT_EVENT_NCOH_BUS_MAX_EXCEED, "HT_EVENT_NCOH_BUS_MAX_EXCEED" },
	{ HT_EVENT_NCOH_CFG_MAP_EXCEED, "HT_EVENT_NCOH_CFG_MAP_EXCEED" },
	{ HT_EVENT_NCOH_DEVICE_FAILED, "HT_EVENT_NCOH_DEVICE_FAILED" },
	{ HT_EVENT_NCOH_AUTO_DEPTH, "HT_EVENT_NCOH_AUTO_DEPTH" },
	{ HT_EVENT_OPT_EVENTS, "HT_EVENT_OPT_EVENTS" },
	{ HT_EVENT_OPT_REQUIRED_CAP_RETRY, "HT_EVENT_OPT_REQUIRED_CAP_RETRY" },
	{ HT_EVENT_OPT_REQUIRED_CAP_GEN3, "HT_EVENT_OPT_REQUIRED_CAP_GEN3" },
	{ HT_EVENT_HW_EVENTS, "HT_EVENT_HW_EVENTS" },
	{ HT_EVENT_HW_SYNCHFLOOD, "HT_EVENT_HW_SYNCHFLOOD" },
	{ HT_EVENT_HW_HTCRC, "HT_EVENT_HW_HTCRC" }
};

static const char *event_string_decode(uint32_t event)
{
	uint32_t i;
	for (i = 0; i < ARRAY_SIZE(event_string_decodes); i++)
		if (event_string_decodes[i].code == event)
			break;
	if (i == ARRAY_SIZE(event_string_decodes))
		return "ERROR: Unmatched event code! "
			"Did you forget to update event_string_decodes[]?";
	return event_string_decodes[i].string;
}

/**
 * void AMD_CB_EventNotify (u8 evtClass, u16 event, const u8 *pEventData0)
 */
static void AMD_CB_EventNotify (u8 evtClass, u16 event, const u8 *pEventData0)
{
	uint8_t i;
	uint8_t log_level;
	uint8_t dump_event_detail;

	printk(BIOS_DEBUG, "%s: ", __func__);

	/* Decode event */
	dump_event_detail = 1;
	switch (evtClass) {
		case HT_EVENT_CLASS_CRITICAL:
		case HT_EVENT_CLASS_ERROR:
		case HT_EVENT_CLASS_HW_FAULT:
		case HT_EVENT_CLASS_WARNING:
		case HT_EVENT_CLASS_INFO:
			log_level = BIOS_DEBUG;
			printk(log_level, "%s", event_class_string_decodes[evtClass]);
			break;
		default:
			log_level = BIOS_DEBUG;
			printk(log_level, "UNKNOWN");
			break;
	}
	printk(log_level, ": ");

	switch(event) {
		case HT_EVENT_COH_EVENTS:
		case HT_EVENT_COH_NO_TOPOLOGY:
		case HT_EVENT_COH_LINK_EXCEED:
		case HT_EVENT_COH_FAMILY_FEUD:
			printk(log_level, "%s", event_string_decode(event));
			break;
		case HT_EVENT_COH_NODE_DISCOVERED:
			{
				printk(log_level, "HT_EVENT_COH_NODE_DISCOVERED");
				sHtEventCohNodeDiscovered *evt = (sHtEventCohNodeDiscovered*)pEventData0;
				printk(log_level, ": node %d link %d new node: %d",
					evt->node, evt->link, evt->newNode);
				dump_event_detail = 0;
				break;
			}
		case HT_EVENT_COH_MPCAP_MISMATCH:
		case HT_EVENT_NCOH_EVENTS:
		case HT_EVENT_NCOH_BUID_EXCEED:
		case HT_EVENT_NCOH_LINK_EXCEED:
		case HT_EVENT_NCOH_BUS_MAX_EXCEED:
		case HT_EVENT_NCOH_CFG_MAP_EXCEED:
			printk(log_level, "%s", event_string_decode(event));
			break;
		case HT_EVENT_NCOH_DEVICE_FAILED:
			{
				printk(log_level, "%s", event_string_decode(event));
				sHtEventNcohDeviceFailed *evt = (sHtEventNcohDeviceFailed*)pEventData0;
				printk(log_level, ": node %d link %d depth: %d attemptedBUID: %d",
					evt->node, evt->link, evt->depth, evt->attemptedBUID);
				dump_event_detail = 0;
				break;
			}
		case HT_EVENT_NCOH_AUTO_DEPTH:
			{
				printk(log_level, "%s", event_string_decode(event));
				sHtEventNcohAutoDepth *evt = (sHtEventNcohAutoDepth*)pEventData0;
				printk(log_level, ": node %d link %d depth: %d",
					evt->node, evt->link, evt->depth);
				dump_event_detail = 0;
				break;
			}
		case HT_EVENT_OPT_EVENTS:
		case HT_EVENT_OPT_REQUIRED_CAP_RETRY:
		case HT_EVENT_OPT_REQUIRED_CAP_GEN3:
		case HT_EVENT_HW_EVENTS:
		case HT_EVENT_HW_SYNCHFLOOD:
		case HT_EVENT_HW_HTCRC:
			printk(log_level, "%s", event_string_decode(event));
			break;
		default:
			printk(log_level, "HT_EVENT_UNKNOWN");
			break;
	}
	printk(log_level, "\n");

	if (dump_event_detail) {
		printk(BIOS_DEBUG, " event class: %02x\n event: %04x\n data: ", evtClass, event);

		for (i = 0; i < *pEventData0; i++) {
			printk(BIOS_DEBUG, " %02x ", *(pEventData0 + i));
		}
		printk(BIOS_DEBUG, "\n");
	}
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
 * BOOL AMD_CB_IgnoreLink(u8 Node, u8 Link)
 * Description:
 *	This routine is used to ignore connected yet faulty HT links,
 *	such as those present in a G34 processor package.
 *
 * Parameters:
 *	@param[in]  node   = The node on which this chain is located
 *	@param[in]  link   = The link on the host for this chain
 */
static BOOL AMD_CB_IgnoreLink (u8 node, u8 link)
{
	return 0;
}

/**
 * void amd_ht_init(struct sys_info *sysinfo)
 *
 *  AMD HT init coreboot wrapper
 *
 */
void amd_ht_init(struct sys_info *sysinfo)
{

	if (!sysinfo) {
		printk(BIOS_DEBUG, "Skipping %s\n", __func__);
		return;
	}

	AMD_HTBLOCK ht_wrapper = {
		NULL,	// u8 **topolist;
		0,	// u8 AutoBusStart;
		32,	// u8 AutoBusMax;
		6,	// u8 AutoBusIncrement;
		AMD_CB_IgnoreLink,		// BOOL (*AMD_CB_IgnoreLink)();
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
		AMD_CB_EventNotify,	// void (*AMD_CB_EventNotify) ();
		&sysinfo->ht_link_cfg // struct ht_link_config*
	};

	printk(BIOS_DEBUG, "Enter %s\n", __func__);
	amdHtInitialize(&ht_wrapper);
	printk(BIOS_DEBUG, "Exit %s\n", __func__);
}

/**
 * void amd_ht_fixup(struct sys_info *sysinfo)
 *
 *  AMD HT fixup
 *
 */
void amd_ht_fixup(struct sys_info *sysinfo) {
	printk(BIOS_DEBUG, "%s\n", __func__);
	if (IS_ENABLED(CONFIG_CPU_AMD_MODEL_10XXX)) {
		uint8_t rev_gte_d = 0;
		uint8_t fam15h = 0;
		uint8_t dual_node = 0;
		uint32_t f3xe8;
		uint32_t family;
		uint32_t model;

		family = model = cpuid_eax(0x80000001);
		model = ((model & 0xf0000) >> 12) | ((model & 0xf0) >> 4);
		family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

		if (family >= 0x6f)
			/* Family 15h or later */
			fam15h = 1;

		if ((model >= 0x8) || fam15h)
			/* Family 10h Revision D or later */
			rev_gte_d = 1;

		if (rev_gte_d) {
			f3xe8 = pci_read_config32(NODE_PCI(0, 3), 0xe8);

			/* Check for dual node capability */
			if (f3xe8 & 0x20000000)
				dual_node = 1;

			if (dual_node) {
				/* Each G34 processor contains a defective HT link.
				* See the Family 10h BKDG Rev 3.62 section 2.7.1.5 for details
				* For Family 15h see the BKDG Rev. 3.14 section 2.12.1.5 for details.
				*/
				uint8_t node;
				uint8_t node_count = get_nodes();
				uint32_t dword;
				for (node = 0; node < node_count; node++) {
					f3xe8 = pci_read_config32(NODE_PCI(node, 3), 0xe8);
					uint8_t internal_node_number = ((f3xe8 & 0xc0000000) >> 30);
					printk(BIOS_DEBUG,
					       "%s: node %d (internal node "
					       "ID %d): disabling defective "
					       "HT link", __func__, node,
					       internal_node_number);
					if (internal_node_number == 0) {
						uint8_t package_link_3_connected = pci_read_config32(NODE_PCI(node, 0), (fam15h)?0x98:0xd8) & 0x1;
						printk(BIOS_DEBUG, " (L3 connected: %d)\n", package_link_3_connected);
						if (package_link_3_connected) {
							/* Set WidthIn and WidthOut to 0 */
							dword = pci_read_config32(NODE_PCI(node, 0), (fam15h)?0x84:0xc4);
							dword &= ~0x77000000;
							pci_write_config32(NODE_PCI(node, 0), (fam15h)?0x84:0xc4, dword);
							/* Set Ganged to 1 */
							dword = pci_read_config32(NODE_PCI(node, 0), (fam15h)?0x170:0x178);
							dword |= 0x00000001;
							pci_write_config32(NODE_PCI(node, 0), (fam15h)?0x170:0x178, dword);
						} else {
							/* Set ConnDly to 1 */
							dword = pci_read_config32(NODE_PCI(node, 0), 0x16c);
							dword |= 0x00000100;
							pci_write_config32(NODE_PCI(node, 0), 0x16c, dword);
							/* Set TransOff and EndOfChain to 1 */
							dword = pci_read_config32(NODE_PCI(node, 4), (fam15h)?0x84:0xc4);
							dword |= 0x000000c0;
							pci_write_config32(NODE_PCI(node, 4), (fam15h)?0x84:0xc4, dword);
						}
					} else if (internal_node_number == 1) {
						uint8_t package_link_3_connected = pci_read_config32(NODE_PCI(node, 0), (fam15h)?0xf8:0xb8) & 0x1;
						printk(BIOS_DEBUG, " (L3 connected: %d)\n", package_link_3_connected);
						if (package_link_3_connected) {
							/* Set WidthIn and WidthOut to 0 */
							dword = pci_read_config32(NODE_PCI(node, 0), (fam15h)?0xe4:0xa4);
							dword &= ~0x77000000;
							pci_write_config32(NODE_PCI(node, 0), (fam15h)?0xe4:0xa4, dword);
							/* Set Ganged to 1 */
							/* WARNING
							 * The Family 15h BKDG states that 0x18c should be set,
							 * however this is in error.  0x17c is the correct control
							 * register (sublink 0) for these processors...
							 */
							dword = pci_read_config32(NODE_PCI(node, 0), (fam15h)?0x17c:0x174);
							dword |= 0x00000001;
							pci_write_config32(NODE_PCI(node, 0), (fam15h)?0x17c:0x174, dword);
						} else {
							/* Set ConnDly to 1 */
							dword = pci_read_config32(NODE_PCI(node, 0), 0x16c);
							dword |= 0x00000100;
							pci_write_config32(NODE_PCI(node, 0), 0x16c, dword);
							/* Set TransOff and EndOfChain to 1 */
							dword = pci_read_config32(NODE_PCI(node, 4), (fam15h)?0xe4:0xa4);
							dword |= 0x000000c0;
							pci_write_config32(NODE_PCI(node, 4), (fam15h)?0xe4:0xa4, dword);
						}
					}
				}
			}
		}
	}
}

u32 get_nodes(void)
{
	pci_devfn_t dev;
	u32 nodes;

	dev = PCI_DEV(CONFIG_CBB, CONFIG_CDB, 0);
	nodes = ((pci_read_config32(dev, 0x60)>>4) & 7);
#if CONFIG_MAX_PHYSICAL_CPUS > 8
	nodes += (((pci_read_config32(dev, 0x160)>>4) & 7)<<3);
#endif
	nodes++;

	return nodes;
}
