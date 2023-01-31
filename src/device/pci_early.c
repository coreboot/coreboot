/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci.h>
#include <device/pci_ops.h>
#include <delay.h>

void pci_s_assert_secondary_reset(pci_devfn_t p2p_bridge)
{
	u16 reg16;
	reg16 = pci_s_read_config16(p2p_bridge, PCI_BRIDGE_CONTROL);
	reg16 |= PCI_BRIDGE_CTL_BUS_RESET;
	pci_s_write_config16(p2p_bridge, PCI_BRIDGE_CONTROL, reg16);
}

void pci_s_deassert_secondary_reset(pci_devfn_t p2p_bridge)
{
	u16 reg16;
	reg16 = pci_s_read_config16(p2p_bridge, PCI_BRIDGE_CONTROL);
	reg16 &= ~PCI_BRIDGE_CTL_BUS_RESET;
	pci_s_write_config16(p2p_bridge, PCI_BRIDGE_CONTROL, reg16);
}

void pci_s_bridge_set_secondary(pci_devfn_t p2p_bridge, u8 secondary)
{
	/* Disable config transaction forwarding. */
	pci_s_write_config8(p2p_bridge, PCI_SECONDARY_BUS, 0x00);
	pci_s_write_config8(p2p_bridge, PCI_SUBORDINATE_BUS, 0x00);
	/* Enable config transaction forwarding. */
	pci_s_write_config8(p2p_bridge, PCI_SECONDARY_BUS, secondary);
	pci_s_write_config8(p2p_bridge, PCI_SUBORDINATE_BUS, secondary);
}

static void pci_s_bridge_set_mmio(pci_devfn_t p2p_bridge, u32 base, u32 size)
{
	u16 reg16;

	/* Disable MMIO window behind the bridge. */
	reg16 = pci_s_read_config16(p2p_bridge, PCI_COMMAND);
	reg16 &= ~PCI_COMMAND_MEMORY;
	pci_s_write_config16(p2p_bridge, PCI_COMMAND, reg16);
	pci_s_write_config32(p2p_bridge, PCI_MEMORY_BASE, 0x10);

	if (!size)
		return;

	/* Enable MMIO window behind the bridge. */
	pci_s_write_config32(p2p_bridge, PCI_MEMORY_BASE,
		((base + size - 1) & 0xfff00000) | ((base >> 16) & 0xfff0));

	reg16 = pci_s_read_config16(p2p_bridge, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_s_write_config16(p2p_bridge, PCI_COMMAND, reg16);
}

static void pci_s_early_mmio_window(pci_devfn_t p2p_bridge, u32 mmio_base, u32 mmio_size)
{
	int timeout, ret = -1;

	/* Secondary bus number is mostly irrelevant as we disable
	 * configuration transactions right after the probe.
	 */
	u8 secondary = 15;
	u8 dev = 0;

	/* Enable configuration and MMIO over bridge. */
	pci_s_assert_secondary_reset(p2p_bridge);
	pci_s_deassert_secondary_reset(p2p_bridge);
	pci_s_bridge_set_secondary(p2p_bridge, secondary);
	pci_s_bridge_set_mmio(p2p_bridge, mmio_base, mmio_size);

	for (timeout = 20000; timeout; timeout--) {
		pci_devfn_t dbg_dev = PCI_DEV(secondary, dev, 0);
		u32 id = pci_s_read_config32(dbg_dev, PCI_VENDOR_ID);
		if (id != 0 && id != 0xffffffff && id != 0xffff0001)
			break;
		udelay(10);
	}

	if (timeout != 0)
		ret = pci_early_device_probe(secondary, dev, mmio_base);

	/* Disable MMIO window if we found no suitable device. */
	if (ret)
		pci_s_bridge_set_mmio(p2p_bridge, 0, 0);

	/* Resource allocator will reconfigure bridges and secondary bus
	 * number may change. Thus early device cannot reliably use config
	 * transactions from here on, so we may as well disable them.
	 */
	pci_s_bridge_set_secondary(p2p_bridge, 0);
}

void pci_early_bridge_init(void)
{
	/* No PCI-to-PCI bridges are enabled yet, so the one we try to
	 * configure must have its primary on bus 0.
	 */
	pci_devfn_t p2p_bridge = PCI_DEV(0, CONFIG_EARLY_PCI_BRIDGE_DEVICE,
		CONFIG_EARLY_PCI_BRIDGE_FUNCTION);

	pci_s_early_mmio_window(p2p_bridge, CONFIG_EARLY_PCI_MMIO_BASE, 0x4000);
}

/* FIXME: A lot of issues using the following, please avoid.
 * Assumes 256 PCI buses, scans them all even when PCI bridges are still
 * disabled. Probes all functions even if 0 is not present.
 */
pci_devfn_t pci_locate_device(unsigned int pci_id, pci_devfn_t dev)
{
	for (; dev <= PCI_DEV(255, 31, 7); dev += PCI_DEV(0, 0, 1)) {
		unsigned int id;
		id = pci_s_read_config32(dev, 0);
		if (id == pci_id)
			return dev;
	}
	return PCI_DEV_INVALID;
}

pci_devfn_t pci_locate_device_on_bus(unsigned int pci_id, unsigned int bus)
{
	pci_devfn_t dev, last;

	dev = PCI_DEV(bus, 0, 0);
	last = PCI_DEV(bus, 31, 7);

	for (; dev <= last; dev += PCI_DEV(0, 0, 1)) {
		unsigned int id;
		id = pci_s_read_config32(dev, 0);
		if (id == pci_id)
			return dev;
	}
	return PCI_DEV_INVALID;
}
