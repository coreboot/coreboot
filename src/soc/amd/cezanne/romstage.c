/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/memmap.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <console/uart.h>
#include <fsp/api.h>
#include <program_loading.h>

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *mcfg = &mupd->FspmConfig;

	mcfg->pci_express_base_addr = CONFIG_MMCONF_BASE_ADDRESS;
	mcfg->tseg_size = CONFIG_SMM_TSEG_SIZE;
	mcfg->bert_size = CONFIG_ACPI_BERT_SIZE;
	mcfg->serial_port_base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	mcfg->serial_port_use_mmio = CONFIG(DRIVERS_UART_8250MEM);
	mcfg->serial_port_stride = CONFIG(DRIVERS_UART_8250MEM_32) ? 4 : 1;
	mcfg->serial_port_baudrate = get_uart_baudrate();
	mcfg->serial_port_refclk = uart_platform_refclk();
}

asmlinkage void car_stage_entry(void)
{
	post_code(0x40);
	console_init();

	post_code(0x41);

	fsp_memory_init(acpi_is_wakeup_s3());

	memmap_stash_early_dram_usage();

	run_ramstage();
}
