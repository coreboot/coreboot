/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/espi.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/soc_chip.h>

/*
* As per the BWG, Chapter 5.9.1. "PCH BIOS component will reserve
* certain memory range as reserved range for BIOS usage.
* For this SOC, the range will be from 0FC800000h till FE7FFFFFh"
*/
static const struct lpc_mmio_range jsl_lpc_fixed_mmio_ranges[] = {
	{ PCH_PRESERVED_BASE_ADDRESS, PCH_PRESERVED_BASE_SIZE },
	{ 0, 0 }
};

const struct lpc_mmio_range *soc_get_fixed_mmio_ranges()
{
	return jsl_lpc_fixed_mmio_ranges;
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

static void pch_misc_init(void)
{
	uint8_t reg8;

	/* Setup NMI on errors, disable SERR */
	reg8 = (inb(0x61)) & 0xf0;
	outb((reg8 | (1 << 2)), 0x61);

	/* Disable NMI sources */
	outb((1 << 7), 0x70);
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
	pch_enable_ioapic();
	pch_pirq_init();
	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
	soc_mirror_dmi_pcr_io_dec();
}

#endif
