/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <stddef.h>
#include <device/pci_ops.h>
#include <console/uart.h>
#include <device/pci.h>

static unsigned int oxpcie_present;
static DEVTREE_CONST u32 uart0_base = CONFIG_EARLY_PCI_MMIO_BASE + 0x1000;

int pci_early_device_probe(u8 bus, u8 dev, u32 mmio_base)
{
	pci_devfn_t device = PCI_DEV(bus, dev, 0);

	u32 id = pci_s_read_config32(device, PCI_VENDOR_ID);
	switch (id) {
	case 0xc1181415: /* e.g. Startech PEX1S1PMINI function 0 */
		/* On this device function 0 is the parallel port, and
		 * function 3 is the serial port. So let's go look for
		 * the UART.
		 */
		device = PCI_DEV(bus, dev, 3);
		id = pci_s_read_config32(device, PCI_VENDOR_ID);
		if (id != 0xc11b1415)
			return -1;
		break;
	case 0xc11b1415: /* e.g. Startech PEX1S1PMINI function 3 */
	case 0xc1581415: /* e.g. Startech MPEX2S952 */
		break;
	default:
		/* No UART here. */
		return -1;
	}

	/* Sanity-check, we assume fixed location. */
	if (mmio_base != CONFIG_EARLY_PCI_MMIO_BASE)
		return -1;

	/* Setup base address on device */
	pci_s_write_config32(device, PCI_BASE_ADDRESS_0, mmio_base);

	/* Enable memory on device */
	u16 reg16 = pci_s_read_config16(device, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_s_write_config16(device, PCI_COMMAND, reg16);

	oxpcie_present = 1;
	return 0;
}

static int oxpcie_uart_active(void)
{
	return oxpcie_present;
}

uintptr_t uart_platform_base(unsigned int idx)
{
	if ((idx < 8) && oxpcie_uart_active())
		return uart0_base + idx * 0x200;
	return 0;
}

void oxford_remap(u32 new_base)
{
#if ENV_RAMSTAGE
	uart0_base = new_base + 0x1000;
#endif
}

unsigned int uart_platform_refclk(void)
{
	return 62500000;
}
