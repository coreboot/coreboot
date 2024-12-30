/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <soc/pch_pci_devs.h>
#include <soc/xhci.h>
#include <types.h>

// XHCI register
#define SYS_BUS_CFG2 0x44

static uint8_t *get_xhci_bar(void)
{
	const struct resource *res;
	res = probe_resource(PCH_DEV_XHCI, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "XHCI BAR is not found\n");
		return NULL;
	}

#if ENV_X86_32
	assert(res->base < 0x100000000ULL);
	if (res->base >= 0x100000000ULL)
		return NULL;
#endif

	return (void *)(uintptr_t)res->base;
}

void write_usb_oc_mapping(const struct usb_oc_mapping *config, uint8_t pins)
{
	uint8_t *mbar = get_xhci_bar();
	uint8_t i;

	if (mbar == NULL) {
		printk(BIOS_ERR, "XHCI BAR is invalid, skip USB OC mapping configuration\n");
		return;
	}
	for (i = 0; i < pins; i++)
		write32(mbar + config[i].pin, config[i].port);
}

void lock_oc_cfg(bool lock)
{
	uint32_t cfg = pci_read_config32(PCH_DEV_XHCI, SYS_BUS_CFG2);

	if (lock)
		cfg |= OCCFGDONE;
	else
		cfg &= ~(OCCFGDONE);
	pci_write_config32(PCH_DEV_XHCI, SYS_BUS_CFG2, cfg);
}
