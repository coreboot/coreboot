/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/ioapic.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <soc/iomap.h>
#include <soc/pcr_ids.h>
#include <soc/intel/common/block/lpc/lpc_def.h>
#include <static.h>

#include "chip.h"

void soc_get_gen_io_dec_range(uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES])
{
	const config_t *config = config_of_soc();

	gen_io_dec[0] = config->gen1_dec;
	gen_io_dec[1] = config->gen2_dec;
	gen_io_dec[2] = config->gen3_dec;
	gen_io_dec[3] = config->gen4_dec;
}

void lpc_soc_init(struct device *dev)
{
	const config_t *const config = config_of(dev);

	/* Legacy initialization */
	isa_dma_init();
	pch_misc_init();

	/* Enable BIOS updates outside of SMM */
	pci_and_config8(PCH_DEV_LPC, 0xdc, ~(1 << 5));

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
