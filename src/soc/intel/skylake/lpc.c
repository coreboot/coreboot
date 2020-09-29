/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/ioapic.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <reg_script.h>
#include <soc/iomap.h>
#include <soc/pcr_ids.h>
#include <soc/intel/common/block/lpc/lpc_def.h>

#include "chip.h"

/**
  PCH preserved MMIO range, 24 MB, from 0xFD000000 to 0xFE7FFFFF
**/

static const struct lpc_mmio_range skl_lpc_fixed_mmio_ranges[] = {
	{ PCH_PRESERVED_BASE_ADDRESS, PCH_PRESERVED_BASE_SIZE },
	{ 0, 0 }
};

const struct lpc_mmio_range *soc_get_fixed_mmio_ranges(void)
{
	return skl_lpc_fixed_mmio_ranges;
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

static const struct reg_script pch_misc_init_script[] = {
	/* Enable BIOS updates outside of SMM */
	REG_PCI_RMW8(0xdc, ~(1 << 5), 0),
	REG_SCRIPT_END
};

void lpc_soc_init(struct device *dev)
{
	const config_t *const config = config_of(dev);

	/* Legacy initialization */
	isa_dma_init();
	pch_misc_init();
	reg_script_run_on_dev(PCH_DEV_LPC, pch_misc_init_script);

	/* Enable CLKRUN_EN for power gating LPC */
	lpc_enable_pci_clk_cntl();

	/* Set LPC Serial IRQ mode */
	lpc_set_serirq_mode(config->serirq_mode);

	/* Interrupt configuration */
	pch_enable_ioapic();
	pch_pirq_init();
	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
}
