/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <soc/espi.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/soc_chip.h>

/*
* To reserve certain memory range as reserved range for BIOS usage.
* For this SOC, the range will be from 0FC800000h till FE7FFFFFh"
*/
static const struct lpc_mmio_range ehl_lpc_fixed_mmio_ranges[] = {
	{ PCH_PRESERVED_BASE_ADDRESS, PCH_PRESERVED_BASE_SIZE },
	{ 0, 0 }
};

const struct lpc_mmio_range *soc_get_fixed_mmio_ranges()
{
	return ehl_lpc_fixed_mmio_ranges;
}

void soc_get_gen_io_dec_range(const struct device *dev, uint32_t *gen_io_dec)
{
	const config_t *config = config_of(dev);

	gen_io_dec[0] = config->gen1_dec;
	gen_io_dec[1] = config->gen2_dec;
	gen_io_dec[2] = config->gen3_dec;
	gen_io_dec[3] = config->gen4_dec;
}

void soc_setup_dmi_pcr_io_dec(uint32_t *gen_io_dec)
{
	/* Mirror these same settings in DMI PCR */
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR1, gen_io_dec[0]);
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR2, gen_io_dec[1]);
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR3, gen_io_dec[2]);
	pcr_write32(PID_DMI, PCR_DMI_LPCLGIR4, gen_io_dec[3]);
}

#if ENV_RAMSTAGE
static void soc_mirror_dmi_pcr_io_dec(void)
{
	struct device *dev = pcidev_on_root(PCH_DEV_SLOT_ESPI, 0);
	uint32_t io_dec_arr[] = {
		pci_read_config32(dev, ESPI_GEN1_DEC),
		pci_read_config32(dev, ESPI_GEN2_DEC),
		pci_read_config32(dev, ESPI_GEN3_DEC),
		pci_read_config32(dev, ESPI_GEN4_DEC),
	};
	/* Mirror these same settings in DMI PCR */
	soc_setup_dmi_pcr_io_dec(&io_dec_arr[0]);
}

static void pch_enable_ioapic(const struct device *dev)
{
	uint32_t reg32;
	/* PCH-LP has 120 redirection entries */
	const int redir_entries = 120;

	set_ioapic_id((void *)IO_APIC_ADDR, 0x02);

	/* affirm full set of redirection table entries ("write once") */
	reg32 = io_apic_read((void *)IO_APIC_ADDR, 0x01);

	reg32 &= ~0x00ff0000;
	reg32 |= (redir_entries - 1) << 16;

	io_apic_write((void *)IO_APIC_ADDR, 0x01, reg32);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write((void *)IO_APIC_ADDR, 0x03, 0x01);
}
/*
 * PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

void soc_pch_pirq_init(const struct device *dev)
{
	struct device *irq_dev;
	uint8_t pch_interrupt_routing[MAX_PXRC_CONFIG];

	pch_interrupt_routing[0] = PCH_IRQ11;
	pch_interrupt_routing[1] = PCH_IRQ10;
	pch_interrupt_routing[2] = PCH_IRQ11;
	pch_interrupt_routing[3] = PCH_IRQ11;
	pch_interrupt_routing[4] = PCH_IRQ11;
	pch_interrupt_routing[5] = PCH_IRQ11;
	pch_interrupt_routing[6] = PCH_IRQ11;
	pch_interrupt_routing[7] = PCH_IRQ11;

	itss_irq_init(pch_interrupt_routing);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		uint8_t int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */
			int_line = PCH_IRQ11;
			break;
		case 2: /* INTB# */
			int_line = PCH_IRQ10;
			break;
		case 3: /* INTC# */
			int_line = PCH_IRQ11;
			break;
		case 4: /* INTD# */
			int_line = PCH_IRQ11;
			break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void pch_misc_init(void)
{
	uint8_t reg8;

	/* Setup NMI on errors, disable SERR */
	reg8 = (inb(NMI_STS_CNT) & 0xf0);
	outb((reg8 | (1 << 2)), NMI_STS_CNT);

	/* Disable NMI sources */
	outb((1 << 7), NMI_EN);
};

void lpc_soc_init(struct device *dev)
{
	/* Legacy initialization */
	isa_dma_init();
	pch_misc_init();

	/* Enable CLKRUN_EN for power gating ESPI */
	lpc_enable_pci_clk_cntl();

	/* Set ESPI Serial IRQ mode */
	if (CONFIG(SERIRQ_CONTINUOUS_MODE))
		lpc_set_serirq_mode(SERIRQ_CONTINUOUS);
	else
		lpc_set_serirq_mode(SERIRQ_QUIET);

	/* Interrupt configuration */
	pch_enable_ioapic(dev);
	soc_pch_pirq_init(dev);
	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
	soc_mirror_dmi_pcr_io_dec();
}

#endif
