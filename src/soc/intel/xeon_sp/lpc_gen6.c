/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/irq.h>
#include <soc/pcr_ids.h>

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>

void lpc_soc_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	if (!scope || !name) {
		printk(BIOS_ERR, "%s: Missing ACPI path/scope\n", dev_path(dev));
		return;
	}

	/* Device */
	acpigen_write_scope(scope);
	acpigen_write_device(name);

	printk(BIOS_DEBUG, "%s.%s: %s\n", scope, name, dev_path(dev));
	acpigen_write_ADR_pci_device(dev);

	acpigen_write_device_end(); /* Device */
	acpigen_write_scope_end(); /* Scope */
}

uint32_t itss_soc_get_on_chip_dev_pir(const struct device *dev)
{
	assert(is_pci(dev));
	return PCI_ITSS_PIR(PCI_SLOT(dev->path.pci.devfn));
}

static void soc_itss_route_irq(const struct device *irq_dev, uint8_t int_pin)
{
	uint8_t pirq = itss_get_on_chip_dev_pirq(irq_dev, int_pin);
	if (pirq == PIRQ_INVALID)
		return;

	uint8_t pirq_rout = pcr_read8(PID_ITSS,
		PCR_ITSS_PIRQA_ROUT + pirq_idx(pirq));
	uint8_t int_line = (pirq_rout & 0x80) ? pirq_idx(pirq) + PCH_IRQ16 : pirq_rout & 0xf;

	printk(BIOS_SPEW, "routing irq: dev %s, pin %d, pirq %d, intline %d\n",
		dev_path(irq_dev), int_pin, pirq, int_line);
	pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
}

static void soc_pch_pirq_init(void)
{
	/* Program irq pin/line for PCI devices using 8259 compatible mode */
	size_t pirq_routes;
	const uint8_t *pirq_routing_legacy = lpc_get_pic_pirq_routing(&pirq_routes);

	itss_irq_init(pirq_routing_legacy);
	for (int i = 0; i < PIRQ_COUNT; i++)
		itss_set_irq_polarity(pirq_routing_legacy[i], 1);

	/* Route irq for end-points */
	struct device *domain = NULL;
	while ((domain = dev_find_path(domain, DEVICE_PATH_DOMAIN))) {
		struct device *irq_dev = NULL;
		while ((irq_dev = dev_bus_each_child(domain->downstream, irq_dev))) {
			if (!is_enabled_pci(irq_dev))
				continue;
			uint8_t int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);
			if (int_pin < PCI_INT_A || int_pin > PCI_INT_D)
				continue;

			soc_itss_route_irq(irq_dev, int_pin);
		}
	}
}

void lpc_soc_init(struct device *dev)
{
	printk(BIOS_SPEW, "pch: lpc_init\n");

	soc_pch_pirq_init();
}
