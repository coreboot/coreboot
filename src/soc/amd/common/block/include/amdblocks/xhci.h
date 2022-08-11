/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_XHCI_H
#define AMD_BLOCK_XHCI_H

#include <cpu/x86/smm.h>
#include <device/pci_type.h>
#include <device/pci_def.h>
#include <device/xhci.h>

#include <types.h>

#define XHCI_GEVENT GEVENT_31

#define SOC_XHCI_DEVICES {\
	SOC_XHCI_0,\
	SOC_XHCI_1,\
	SOC_XHCI_2,\
	SOC_XHCI_3,\
	SOC_XHCI_4,\
	SOC_XHCI_5,\
	SOC_XHCI_6,\
	SOC_XHCI_7,\
}

void soc_xhci_store_resources(struct smm_pci_resource_info *slots, size_t count);
void soc_xhci_log_wake_events(void);

#endif /* AMD_BLOCK_XHCI_H */
