/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <bootblock_common.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <program_loading.h>
#include <soc/iomap.h>
#include <soc/intel/common/util.h>
#include <soc/pci_devs.h>
#include <soc/reg_access.h>

extern void asmlinkage light_sd_led(void);

static const struct reg_script legacy_gpio_init[] = {
	/* Temporarily enable the legacy GPIO controller */
	REG_PCI_WRITE32(R_QNC_LPC_GBA_BASE, IO_ADDRESS_VALID
		| LEGACY_GPIO_BASE_ADDRESS),
	/* Temporarily enable the GPE controller */
	REG_PCI_WRITE32(R_QNC_LPC_GPE0BLK, IO_ADDRESS_VALID
		| GPE0_BASE_ADDRESS),
	REG_PCI_OR8(PCI_COMMAND, PCI_COMMAND_IO),
	REG_SCRIPT_END
};

static const struct reg_script i2c_gpio_controller_init[] = {
	/* Temporarily enable the GPIO controller */
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_0, I2C_BASE_ADDRESS),
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_1, GPIO_BASE_ADDRESS),
	REG_PCI_OR8(PCI_COMMAND, PCI_COMMAND_MEMORY),
	REG_SCRIPT_END
};

static const struct reg_script hsuart_init[] = {
	/* Enable the HSUART */
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_0, UART_BASE_ADDRESS),
	REG_PCI_OR8(PCI_COMMAND, PCI_COMMAND_MEMORY),
	REG_SCRIPT_END
};

static const struct reg_script mtrr_init[] = {
	/* Use write-through caching, for FSP 2.0 the cache will be invalidated
	 * postchar (arch/x86/exit_car.S).
	 */

	/* Enable the cache */
	REG_CPU_CR_AND(0, ~(CR0_CD | CR0_NW)),

	/* Cache the SPI flash */
	REG_MSR_WRITE(MTRR_PHYS_BASE(0), (uint32_t)((-CONFIG_ROM_SIZE)
		| MTRR_TYPE_WRTHROUGH)),
	REG_MSR_WRITE(MTRR_PHYS_MASK(0), (uint32_t)((-CONFIG_ROM_SIZE)
		| MTRR_PHYS_MASK_VALID)),

	/* Cache ESRAM */
	REG_MSR_WRITE(MTRR_PHYS_BASE(1), (uint32_t)(0x80000000
		| MTRR_TYPE_WRTHROUGH)),
	REG_MSR_WRITE(MTRR_PHYS_MASK(1), (uint32_t)((~0x7ffff)
		| MTRR_PHYS_MASK_VALID)),

	/* Enable the variable MTRRs */
	REG_MSR_WRITE(MTRR_DEF_TYPE_MSR, MTRR_DEF_TYPE_EN
		| MTRR_TYPE_UNCACHEABLE),

	REG_SCRIPT_END
};

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	if (IS_ENABLED(CONFIG_ENABLE_DEBUG_LED_BOOTBLOCK_ENTRY))
		light_sd_led();

	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
}

void bootblock_soc_early_init(void)
{
	if (IS_ENABLED(CONFIG_ENABLE_DEBUG_LED_SOC_EARLY_INIT_ENTRY))
		light_sd_led();

	/* Initialize the MTRRs */
	reg_script_run(mtrr_init);

	/* Initialize the controllers */
	reg_script_run_on_dev(I2CGPIO_BDF, i2c_gpio_controller_init);
	reg_script_run_on_dev(LPC_BDF, legacy_gpio_init);

	/* Enable the HSUART */
	if (IS_ENABLED(CONFIG_ENABLE_BUILTIN_HSUART0))
		reg_script_run_on_dev(HSUART0_BDF, hsuart_init);
	if (IS_ENABLED(CONFIG_ENABLE_BUILTIN_HSUART1))
		reg_script_run_on_dev(HSUART1_BDF, hsuart_init);

	if (IS_ENABLED(CONFIG_ENABLE_DEBUG_LED_SOC_EARLY_INIT_EXIT))
		light_sd_led();
}

void bootblock_soc_init(void)
{
	if (IS_ENABLED(CONFIG_ENABLE_DEBUG_LED_SOC_INIT_ENTRY))
		light_sd_led();

	/* Display the MTRRs */
	soc_display_mtrrs();
}

void platform_prog_run(struct prog *prog)
{
	/* Display the program entry point */
	printk(BIOS_SPEW, "Calling %s, 0x%p(0x%p)\n", prog->name,
		prog->entry, prog->arg);
}
