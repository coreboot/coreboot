/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/smbus.h>
#include <device/smbus_host.h>
#include <intelblocks/tco.h>
#include <southbridge/intel/common/smbus_ops.h>
#include "smbuslib.h"

static void pch_smbus_init(struct device *dev)
{
	struct resource *res;

	/* Enable clock gating */
	pci_update_config32(dev, 0x80,
		~((1 << 8) | (1 << 10) | (1 << 12) | (1 << 14)), 0);

	/* Set Receive Slave Address */
	res = probe_resource(dev, PCI_BASE_ADDRESS_4);
	if (res)
		smbus_set_slave_addr(res->base, SMBUS_SLAVE_ADDR);
}

/*
 * `finalize_smbus` function is native implementation of equivalent events
 * performed by each FSP NotifyPhase() API invocations.
 *
 * Operations are:
 * 1. TCO Lock.
 */
static void finalize_smbus(struct device *dev)
{
	if (!CONFIG(USE_FSP_NOTIFY_PHASE_POST_PCI_ENUM))
		tco_lockdown();
}

struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.init			= pch_smbus_init,
	.ops_pci		= &pci_dev_ops_pci,
	.ops_smbus_bus		= &lops_smbus_bus,
	.final			= finalize_smbus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_WCL_SMBUS,
	PCI_DID_INTEL_PTL_H_SMBUS,
	PCI_DID_INTEL_PTL_U_H_SMBUS,
	PCI_DID_INTEL_LNL_SMBUS,
	PCI_DID_INTEL_MTL_SMBUS,
	PCI_DID_INTEL_ARL_SMBUS,
	PCI_DID_INTEL_ARL_S_SMBUS,
	PCI_DID_INTEL_ARP_S_SMBUS,
	PCI_DID_INTEL_RPP_P_SMBUS,
	PCI_DID_INTEL_RPP_S_SMBUS,
	PCI_DID_INTEL_APL_SMBUS,
	PCI_DID_INTEL_GLK_SMBUS,
	PCI_DID_INTEL_CNL_SMBUS,
	PCI_DID_INTEL_CNP_H_SMBUS,
	PCI_DID_INTEL_EBG_SMBUS,
	PCI_DID_INTEL_LWB_SMBUS_SUPER,
	PCI_DID_INTEL_LWB_SMBUS,
	PCI_DID_INTEL_ICP_LP_SMBUS,
	PCI_DID_INTEL_CMP_SMBUS,
	PCI_DID_INTEL_CMP_H_SMBUS,
	PCI_DID_INTEL_TGP_LP_SMBUS,
	PCI_DID_INTEL_TGP_H_SMBUS,
	PCI_DID_INTEL_MCC_SMBUS,
	PCI_DID_INTEL_JSP_SMBUS,
	PCI_DID_INTEL_ADP_P_SMBUS,
	PCI_DID_INTEL_ADP_S_SMBUS,
	PCI_DID_INTEL_ADP_M_N_SMBUS,
	PCI_DID_INTEL_DNV_SMBUS_LEGACY,
	0
};

static const struct pci_driver pch_smbus __pci_driver = {
	.ops	 = &smbus_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices	 = pci_device_ids,
};
