/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/itss.h>
#include <soc/pcr_ids.h>
#include <static.h>

static void soc_pch_pirq_init(void)
{
	for (uint32_t reg = PCR_ITSS_PIRQA_ROUT; reg <= PCR_ITSS_PIRQH_ROUT; reg++)
		printk(BIOS_SPEW, "PIRQ Routing Control %c at 0x%04x: 0x%02x\n",
		       reg - PCR_ITSS_PIRQA_ROUT + 'A', reg, pcr_read8(PID_ITSS, reg));

	for (uint32_t reg = 0; reg < 32; reg++)
		printk(BIOS_SPEW, "PCI Interrupt Route %d at 0x%04x: 0x%02x\n", reg,
		       PCI_ITSS_PIR(reg), pcr_read16(PID_ITSS, PCI_ITSS_PIR(reg)));

	/* The PIRQA-H registers are programmable thus legacy PIC mode is supported. */
	size_t num;
	const uint8_t *pch_interrupt_routing = lpc_get_pic_pirq_routing(&num);
	itss_irq_init(pch_interrupt_routing);

	for (struct device *dev = all_devices; dev; dev = dev->next) {
		if (!is_enabled_pci(dev))
			continue;

		enum pirq pirq = itss_soc_get_dev_pirq(dev);
		if (pirq == PIRQ_INVALID)
			continue;

		uint8_t pirq_value = pcr_read8(PID_ITSS, PCR_ITSS_PIRQA_ROUT + pirq_idx(pirq));
		if (pirq_value & 0x80)
			pci_write_config8(dev, PCI_INTERRUPT_LINE, PCH_IRQ16 + pirq_idx(pirq));
		else
			pci_write_config8(dev, PCI_INTERRUPT_LINE, pirq_value & 0x0f);

		printk(BIOS_DEBUG, "%s is using %s, line 0x%2x\n", dev_path(dev),
		       pin_to_str(pci_read_config8(dev, PCI_INTERRUPT_PIN)),
		       pci_read_config8(dev, PCI_INTERRUPT_LINE));
	}
}

void lpc_soc_init(struct device *dev)
{
	lpc_set_serirq_mode(CONFIG(SERIRQ_CONTINUOUS_MODE) ? SERIRQ_CONTINUOUS : SERIRQ_QUIET);

	ioapic_set_max_vectors(IO_APIC_ADDR, PCH_REDIR_ETR);

	setup_ioapic(IO_APIC_ADDR, PCH_IOAPIC_ID);
	ioapic_set_boot_config(IO_APIC_ADDR, true);

	soc_pch_pirq_init();
}

void pch_lpc_soc_fill_io_resources(struct device *dev)
{
	struct resource *res;

	res = new_resource(dev, PCI_BASE_ADDRESS_4); /**< Use the register offset in PMC. */
	res->base = ACPI_BASE_ADDRESS;
	res->size = 0x80; /**< 128 bytes I/O config space */
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	printk(BIOS_DEBUG, "Adding ACPI IO config space BAR at base 0x%08llx, size 0x%08llx\n",
	       res->base, res->size);
}
