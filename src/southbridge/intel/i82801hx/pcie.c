/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel ICH8-M PCIe Root Port Initialization
 *
 * Reference: coreboot i82801ix/i82801ix.c, i82801ix/pcie.c
 */

#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <console/console.h>
#include <southbridge/intel/i82801hx/i82801hx.h>
#include "chip.h"

#define PCIE_PORT_COUNT  6

static uint32_t pcie_fd_bit(int fn)
{
	switch (fn) {
	case 0:  return FD_PE1D;
	case 1:  return FD_PE2D;
	case 2:  return FD_PE3D;
	case 3:  return FD_PE4D;
	case 4:  return FD_PE5D;
	case 5:  return FD_PE6D;
	default: return 0;
	}
}

/* ================================================================== */
/* Per-Port CIR Programming                                           */
/* ================================================================== */

static void pcie_port_cir_init(void)
{
	int i;

	for (i = 0; i < PCIE_PORT_COUNT; i++) {
		struct device *dev = pcidev_on_root(28, i);
		if (!dev)
			continue;

		pci_or_config32(dev, D28Fx_CIR_300, (1 << 21));
		pci_write_config8(dev, D28Fx_CIR_324, 0x40);
	}
}

/* ================================================================== */
/* Per-Port Device Initialization                                     */
/* ================================================================== */

static void pcie_port_init(struct device *dev)
{
	u32 reg32;

	/* Enable Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Set Cache Line Size to 0x10 (64 bytes) */
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, 0x10);

	/* Disable parity error response in Bridge Control */
	pci_and_config16(dev, PCI_BRIDGE_CONTROL, ~PCI_BRIDGE_CTL_PARITY);

	/* Enable IO xAPIC on this PCIe port */
	pci_or_config32(dev, D28Fx_IOXAPIC, (1 << 7));

	/* Enable Backbone Clock Gating */
	pci_or_config8(dev, D28Fx_BBCLKG, 0x0f);

	/* Set VC0 transaction class: TC0 only */
	reg32 = pci_read_config32(dev, D28Fx_VC0RCTL);
	reg32 = (reg32 & ~0x000000ff) | 1;
	pci_write_config32(dev, D28Fx_VC0RCTL, reg32);

	/* Mask completion timeouts */
	pci_or_config32(dev, D28Fx_CTTOMASK, (1 << 14));

	/* Lock R/WO Correctable Error Mask */
	pci_write_config32(dev, D28Fx_CEMASK,
			   pci_read_config32(dev, D28Fx_CEMASK));

	/* Clear errors in status registers */
	pci_write_config16(dev, PCI_STATUS,
			   pci_read_config16(dev, PCI_STATUS));
	pci_write_config16(dev, PCI_SEC_STATUS,
			   pci_read_config16(dev, PCI_SEC_STATUS));
}

/* ================================================================== */
/* Mobile ASPM Configuration                                          */
/* ================================================================== */

static void pcie_mobile_aspm(struct device *dev)
{
	u32 apmc;

	/* Enable mobile ASPM */
	pci_or_config32(dev, D28Fx_ASPM_MOBILE, (1 << 0));

	/* If both L0s and L1 are enabled, set bit 1 too. */
	apmc = pci_read_config32(dev, D28Fx_LCTL) & 3;
	if (apmc == 3)
		pci_or_config32(dev, D28Fx_ASPM_MOBILE, (1 << 1));
}

/* ================================================================== */
/* Slot Configuration                                                 */
/* ================================================================== */

static void pcie_slot_config(void)
{
	const struct device *lpc_dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_i82801hx_config *config;
	int i;
	int slot_number = 1;

	if (!lpc_dev || !lpc_dev->chip_info)
		return;

	config = lpc_dev->chip_info;

	for (i = 0; i < PCIE_PORT_COUNT; i++) {
		struct device *dev = pcidev_on_root(28, i);
		u32 xcap;

		if (!dev)
			continue;

		xcap = pci_read_config32(dev, D28Fx_XCAP);

		if (config->pcie_slot_implemented & (1 << i)) {
			u32 slcap;

			xcap |= PCI_EXP_FLAGS_SLOT;
			pci_write_config32(dev, D28Fx_XCAP, xcap);

			slcap = pci_read_config32(dev, D28Fx_SLCAP);
			slcap &= ~(0x1fff << D28_SLCAP_SLOTNUM_SHIFT);
			slcap |= (slot_number++ << D28_SLCAP_SLOTNUM_SHIFT);
			slcap &= ~(0x0003 << D28_SLCAP_SCALE_SHIFT);
			slcap |= (config->pcie_power_limits[i].scale
				  << D28_SLCAP_SCALE_SHIFT);
			slcap &= ~(0x00ff << D28_SLCAP_POWER_SHIFT);
			slcap |= (config->pcie_power_limits[i].value
				  << D28_SLCAP_POWER_SHIFT);
			pci_write_config32(dev, D28Fx_SLCAP, slcap);
		} else {
			xcap &= ~PCI_EXP_FLAGS_SLOT;
			pci_write_config32(dev, D28Fx_XCAP, xcap);
		}
	}
}

/* ================================================================== */
/* Trailing Port Disable                                              */
/* ================================================================== */

static void pcie_trailing_port_disable(void)
{
	int i;
	u32 fd;

	fd = RCBA32(RCBA_FD);

	/*
	 * Walk from port 5 (highest) down to port 0.
	 * If the port is disabled in FD, set bits [17:16] in 0x300
	 * to disable the link. Stop at the first enabled port.
	 */
	for (i = 5; i >= 0; i--) {
		struct device *dev;

		if (!(fd & pcie_fd_bit(i)))
			break;

		dev = pcidev_on_root(28, i);
		if (!dev)
			continue;

		pci_or_config32(dev, D28Fx_CIR_300, (0x3 << 16));
	}
}

/* ================================================================== */
/* ASPM Capability Lock                                               */
/* ================================================================== */

static void pcie_aspm_lock(void)
{
	int i;

	for (i = 0; i < PCIE_PORT_COUNT; i++) {
		struct device *dev = pcidev_on_root(28, i);
		if (!dev)
			continue;

		/* Write back to latch R/WO bits */
		pci_write_config32(dev, D28Fx_LCAP,
				   pci_read_config32(dev, D28Fx_LCAP));
	}
}

/* ================================================================== */
/* Root Port Function Number Remap (RPFN)                             */
/* ================================================================== */

static void pcie_rpfn_hide(void)
{
	int i;
	u32 rpfn;

	rpfn = RCBA32(RCBA_RPFN);

	for (i = 0; i < PCIE_PORT_COUNT; i++) {
		if (RCBA32(RCBA_FD) & pcie_fd_bit(i))
			rpfn |= (1 << ((i * 4) + 3));
	}

	RCBA32(RCBA_RPFN) = rpfn;
}

/* ================================================================== */
/* Public Entry Point                                                 */
/* ================================================================== */

void i82801hx_pcie_init(void)
{
	int i;

	/* Per-port CIR programming */
	pcie_port_cir_init();

	/* Mobile ASPM and per-port init on each root port */
	for (i = 0; i < PCIE_PORT_COUNT; i++) {
		struct device *dev = pcidev_on_root(28, i);
		if (!dev)
			continue;

		pcie_mobile_aspm(dev);
		pcie_port_init(dev);
	}

	/* Trailing port disable (top-down unused ports) */
	pcie_trailing_port_disable();

	/* Slot configuration from devicetree */
	pcie_slot_config();

	/* ASPM capability lock */
	pcie_aspm_lock();

	/* RPFN - hide disabled PCIe ports */
	pcie_rpfn_hide();
}
