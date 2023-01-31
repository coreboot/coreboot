/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <string.h>

#include "chip.h"
#include "pch.h"

int pch_silicon_revision(void)
{
	static int pch_revision_id = -1;

#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
#else
	struct device *dev = pcidev_on_root(0x1f, 0);
#endif

	if (pch_revision_id < 0)
		pch_revision_id = pci_read_config8(dev,	PCI_REVISION_ID);
	return pch_revision_id;
}

int pch_silicon_type(void)
{
	static int pch_type = -1;

#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
#else
	struct device *dev = pcidev_on_root(0x1f, 0);
#endif

	if (pch_type < 0)
		pch_type = pci_read_config8(dev, PCI_DEVICE_ID + 1);
	return pch_type;
}

static int pch_silicon_supported(int type, int rev)
{
	int cur_type = pch_silicon_type();
	int cur_rev = pch_silicon_revision();

	switch (type) {
	case PCH_TYPE_CPT:
		/* CougarPoint minimum revision */
		if (cur_type == PCH_TYPE_CPT && cur_rev >= rev)
			return 1;
		/* PantherPoint any revision */
		if (cur_type == PCH_TYPE_PPT)
			return 1;
		break;

	case PCH_TYPE_PPT:
		/* PantherPoint minimum revision */
		if (cur_type == PCH_TYPE_PPT && cur_rev >= rev)
			return 1;
		break;
	}

	return 0;
}

#define IOBP_RETRY 1000
static inline int iobp_poll(void)
{
	unsigned int try = IOBP_RETRY;
	u32 data;

	while (try--) {
		data = RCBA32(IOBPS);
		if ((data & 1) == 0)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "IOBP timeout\n");
	return 0;
}

void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue)
{
	u32 data;

	/* Set the address */
	RCBA32(IOBPIRI) = address;

	/* READ OPCODE */
	if (pch_silicon_supported(PCH_TYPE_CPT, PCH_STEP_B0))
		RCBA32(IOBPS) = IOBPS_RW_BX;
	else
		RCBA32(IOBPS) = IOBPS_READ_AX;
	if (!iobp_poll())
		return;

	/* Read IOBP data */
	data = RCBA32(IOBPD);
	if (!iobp_poll())
		return;

	/* Check for successful transaction */
	if ((RCBA32(IOBPS) & 0x6) != 0) {
		printk(BIOS_ERR, "IOBP read 0x%08x failed\n", address);
		return;
	}

	/* Update the data */
	data &= andvalue;
	data |= orvalue;

	/* WRITE OPCODE */
	if (pch_silicon_supported(PCH_TYPE_CPT, PCH_STEP_B0))
		RCBA32(IOBPS) = IOBPS_RW_BX;
	else
		RCBA32(IOBPS) = IOBPS_WRITE_AX;
	if (!iobp_poll())
		return;

	/* Write IOBP data */
	RCBA32(IOBPD) = data;
	if (!iobp_poll())
		return;
}

#ifndef __SIMPLE_DEVICE__
/* Set bit in function disable register to hide this device */
static void pch_hide_devfn(unsigned int devfn)
{
	switch (devfn) {
	case PCI_DEVFN(20, 0): /* xHCI */
		if (pch_silicon_type() == PCH_TYPE_PPT) {
			/* on CPT this bit is reserved */
			RCBA32_OR(FD, PCH_DISABLE_XHCI);
		}
		break;
	case PCI_DEVFN(22, 0): /* MEI #1 */
		RCBA32_OR(FD2, PCH_DISABLE_MEI1);
		break;
	case PCI_DEVFN(22, 1): /* MEI #2 */
		RCBA32_OR(FD2, PCH_DISABLE_MEI2);
		break;
	case PCI_DEVFN(22, 2): /* IDE-R */
		RCBA32_OR(FD2, PCH_DISABLE_IDER);
		break;
	case PCI_DEVFN(22, 3): /* KT */
		RCBA32_OR(FD2, PCH_DISABLE_KT);
		break;
	case PCI_DEVFN(25, 0): /* Gigabit Ethernet */
		/* BUC is already handled in `early_pch.c`. */
		break;
	case PCI_DEVFN(26, 0): /* EHCI #2 */
		RCBA32_OR(FD, PCH_DISABLE_EHCI2);
		break;
	case PCI_DEVFN(27, 0): /* HD Audio Controller */
		RCBA32_OR(FD, PCH_DISABLE_HD_AUDIO);
		break;
	case PCI_DEVFN(28, 0): /* PCI Express Root Port 1 */
	case PCI_DEVFN(28, 1): /* PCI Express Root Port 2 */
	case PCI_DEVFN(28, 2): /* PCI Express Root Port 3 */
	case PCI_DEVFN(28, 3): /* PCI Express Root Port 4 */
	case PCI_DEVFN(28, 4): /* PCI Express Root Port 5 */
	case PCI_DEVFN(28, 5): /* PCI Express Root Port 6 */
	case PCI_DEVFN(28, 6): /* PCI Express Root Port 7 */
	case PCI_DEVFN(28, 7): /* PCI Express Root Port 8 */
		RCBA32_OR(FD, PCH_DISABLE_PCIE(PCI_FUNC(devfn)));
		break;
	case PCI_DEVFN(29, 0): /* EHCI #1 */
		RCBA32_OR(FD, PCH_DISABLE_EHCI1);
		break;
	case PCI_DEVFN(30, 0): /* PCI-to-PCI Bridge */
		RCBA32_OR(FD, PCH_DISABLE_P2P);
		break;
	case PCI_DEVFN(31, 0): /* LPC */
		RCBA32_OR(FD, PCH_DISABLE_LPC);
		break;
	case PCI_DEVFN(31, 2): /* SATA #1 */
		RCBA32_OR(FD, PCH_DISABLE_SATA1);
		break;
	case PCI_DEVFN(31, 3): /* SMBUS */
		RCBA32_OR(FD, PCH_DISABLE_SMBUS);
		break;
	case PCI_DEVFN(31, 5): /* SATA #22 */
		RCBA32_OR(FD, PCH_DISABLE_SATA2);
		break;
	case PCI_DEVFN(31, 6): /* Thermal Subsystem */
		RCBA32_OR(FD, PCH_DISABLE_THERMAL);
		break;
	}
}

/* Check if any port in set X to X+3 is enabled */
static int pch_pcie_check_set_enabled(struct device *dev)
{
	struct device *port;
	int port_func;
	int dev_func = PCI_FUNC(dev->path.pci.devfn);

	printk(BIOS_DEBUG, "%s: check set enabled\n", dev_path(dev));

	/* Go through static device tree list of devices
	 * because enumeration is still in progress */
	for (port = all_devices; port; port = port->next) {
		/* Only care about PCIe root ports */
		if (PCI_SLOT(port->path.pci.devfn) !=
		    PCI_SLOT(dev->path.pci.devfn))
			continue;

		/* Check if port is in range and enabled */
		port_func = PCI_FUNC(port->path.pci.devfn);
		if (port_func >= dev_func &&
		    port_func < (dev_func + 4) &&
		    port->enabled)
			return 1;
	}

	/* None of the ports in this set are enabled */
	return 0;
}

/* RPFN is a write-once register so keep a copy until it is written */
static u32 new_rpfn;

/* Swap function numbers assigned to two PCIe Root Ports */
static void pch_pcie_function_swap(u8 old_fn, u8 new_fn)
{
	u32 old_rpfn = new_rpfn;

	printk(BIOS_DEBUG, "PCH: Remap PCIe function %d to %d\n",
	       old_fn, new_fn);

	new_rpfn &= ~(RPFN_FNMASK(old_fn) | RPFN_FNMASK(new_fn));

	/* Old function set to new function and disabled */
	new_rpfn |= RPFN_FNSET(old_fn, RPFN_FNGET(old_rpfn, new_fn));
	new_rpfn |= RPFN_FNSET(new_fn, RPFN_FNGET(old_rpfn, old_fn));
}

/* Update devicetree with new Root Port function number assignment */
static void pch_pcie_devicetree_update(
		struct southbridge_intel_bd82x6x_config *config)
{
	struct device *dev;

	/*
	 * hotplug map should also be updated along with their
	 * corresponding port
	 */
	u8 new_hotplug_map[sizeof(config->pcie_hotplug_map)];

	/*
	 * Slots that didn't move need the hotplug setting copied too,
	 * so "new_hotplug_map" is initialized with the values of the old map.
	 */
	memcpy(new_hotplug_map, config->pcie_hotplug_map,
	       sizeof(new_hotplug_map));

	/* Update the function numbers in the static devicetree */
	for (dev = all_devices; dev; dev = dev->next) {
		u8 new_devfn;

		/* Only care about PCH PCIe root ports */
		if (PCI_SLOT(dev->path.pci.devfn) !=
		    PCH_PCIE_DEV_SLOT)
			continue;

		/* Determine the new devfn for this port */
		new_devfn = PCI_DEVFN(PCH_PCIE_DEV_SLOT,
			      RPFN_FNGET(new_rpfn,
				 PCI_FUNC(dev->path.pci.devfn)));

		if (dev->path.pci.devfn != new_devfn) {
			printk(BIOS_DEBUG,
			       "PCH: PCIe map %02x.%1x -> %02x.%1x\n",
			       PCI_SLOT(dev->path.pci.devfn),
			       PCI_FUNC(dev->path.pci.devfn),
			       PCI_SLOT(new_devfn), PCI_FUNC(new_devfn));

			/*
			 * Copy the flag to its new position along with
			 * the corresponding port
			 */
			new_hotplug_map[PCI_FUNC(new_devfn)] =
				config->pcie_hotplug_map
				[PCI_FUNC(dev->path.pci.devfn)];

			dev->path.pci.devfn = new_devfn;
		}
	}

	/* Copy the updated map back to its place */
	memcpy(config->pcie_hotplug_map, new_hotplug_map,
	       sizeof(new_hotplug_map));
}

/* Special handling for PCIe Root Port devices */
static void pch_pcie_enable(struct device *dev)
{
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;

	if (!config)
		return;

	/*
	 * Save a copy of the Root Port Function Number map when
	 * starting to walk the list of PCIe Root Ports so it can
	 * be updated locally and written out when the last port
	 * has been processed.
	 */
	if (PCI_FUNC(dev->path.pci.devfn) == 0) {
		new_rpfn = RCBA32(RPFN);

		/*
		 * Enable Root Port coalescing if the first port is disabled
		 * or the other devices will not be enumerated by the OS.
		 */
		if (!dev->enabled)
			config->pcie_port_coalesce = true;

		if (config->pcie_port_coalesce)
			printk(BIOS_INFO,
			       "PCH: PCIe Root Port coalescing is enabled\n");
	}

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/*
		 * PCIE Power Savings for PantherPoint and CougarPoint/B1+
		 *
		 * If PCIe 0-3 disabled set Function 0 0xE2[0] = 1
		 * If PCIe 4-7 disabled set Function 4 0xE2[0] = 1
		 *
		 * This check is done here instead of PCIe driver
		 * because the PCIe driver enable() handler is not
		 * called unless the device is enabled.
		 */
		if ((PCI_FUNC(dev->path.pci.devfn) == 0 ||
		     PCI_FUNC(dev->path.pci.devfn) == 4)) {
			/* Handle workaround for PPT and CPT/B1+ */
			if (pch_silicon_supported(PCH_TYPE_CPT, PCH_STEP_B1) &&
			    !pch_pcie_check_set_enabled(dev)) {
				pci_or_config8(dev, 0xe2, 1);
			}

			/*
			 * Enable Clock Gating for shared PCIe resources
			 * before disabling this particular port.
			 */
			pci_write_config8(dev, 0xe1, 0x3c);
		}

		/* Ensure memory, io, and bus master are all disabled */
		pci_and_config16(dev, PCI_COMMAND,
				 ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

		/* Do not claim downstream transactions for PCIe ports */
		new_rpfn |= RPFN_HIDE(PCI_FUNC(dev->path.pci.devfn));

		/* Hide this device if possible */
		pch_hide_devfn(dev->path.pci.devfn);
	} else {
		int fn;

		/*
		 * Check if there is a lower disabled port to swap with this
		 * port in order to maintain linear order starting at zero.
		 */
		if (config->pcie_port_coalesce) {
			for (fn=0; fn < PCI_FUNC(dev->path.pci.devfn); fn++) {
				if (!(new_rpfn & RPFN_HIDE(fn)))
					continue;

				/* Swap places with this function */
				pch_pcie_function_swap(
					PCI_FUNC(dev->path.pci.devfn), fn);
				break;
			}
		}

		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}

	/*
	 * When processing the last PCIe root port we can now
	 * update the Root Port Function Number and Hide register.
	 */
	if (PCI_FUNC(dev->path.pci.devfn) == 7) {
		printk(BIOS_SPEW, "PCH: RPFN 0x%08x -> 0x%08x\n",
		       RCBA32(RPFN), new_rpfn);
		RCBA32(RPFN) = new_rpfn;

		/* Update static devictree with new function numbers */
		if (config->pcie_port_coalesce)
			pch_pcie_devicetree_update(config);
	}
}

void pch_enable(struct device *dev)
{
	/* PCH PCIe Root Ports get special handling */
	if (PCI_SLOT(dev->path.pci.devfn) == PCH_PCIE_DEV_SLOT)
		return pch_pcie_enable(dev);

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		pci_and_config16(dev, PCI_COMMAND,
				 ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

		/* Hide this device if possible */
		pch_hide_devfn(dev->path.pci.devfn);
	} else {
		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}
}

struct chip_operations southbridge_intel_bd82x6x_ops = {
	CHIP_NAME("Intel Series 6/7 (Cougar Point/Panther Point) Southbridge")
	.enable_dev = pch_enable,
};
#endif
