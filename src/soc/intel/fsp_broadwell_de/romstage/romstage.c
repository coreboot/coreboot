/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#include <stddef.h>
#include <device/pci_ops.h>
#include <arch/cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/usb.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cf9_reset.h>
#include <program_loading.h>
#include <timestamp.h>
#include <version.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <pc80/mc146818rtc.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/memory.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/gpio.h>
#include <soc/vtd.h>
#include <soc/ubox.h>
#include <build.h>

#include <security/intel/txt/txt.h>
#include <security/intel/txt/txt_register.h>

static void init_rtc(void)
{
	u16 gen_pmcon3 = pci_read_config16(PCI_DEV(0, LPC_DEV, LPC_FUNC), GEN_PMCON_3);

	if (gen_pmcon3 & RTC_PWR_STS) {
		printk(BIOS_DEBUG, "RTC Failure detected.  Resetting Date to %s\n",
			coreboot_dmi_date);
	}
	cmos_init(gen_pmcon3 & RTC_PWR_STS);
}

/* Set up IO address range and enable it for the GPIO block. */
static void setup_gpio_io_address(void)
{
	pci_write_config32(PCI_DEV(0, LPC_DEV, LPC_FUNC), GPIO_BASE_ADR_OFFSET,
					GPIO_BASE_ADDRESS);
	pci_write_config8(PCI_DEV(0, LPC_DEV, LPC_FUNC), GPIO_CTRL_OFFSET,
					GPIO_DECODE_ENABLE);
}


static void enable_integrated_uart(uint8_t port)
{
	uint32_t ubox_uart_en = 0, dfx1 = 0;
	pci_devfn_t ubox_dev;

	/* UBOX sits on CPUBUSNO(1) */
	ubox_dev = PCI_DEV(get_busno1(), UBOX_DEV, UBOX_FUNC);
	uint32_t reset_sts = pci_mmio_read_config32(ubox_dev, UBOX_SC_RESET_STATUS);

	/* In case we are in bypass mode do nothing */
	if (reset_sts & UBOX_SC_BYPASS)
		return;

	dfx1 = pci_mmio_read_config32(VTD_PCI_DEV, VTD_DFX1);
	ubox_uart_en = pci_mmio_read_config32(ubox_dev, UBOX_UART_ENABLE);

	switch (port) {
	case 0:
		ubox_uart_en |= UBOX_UART_ENABLE_PORT0;
		dfx1 |= VTD_DFX1_RANGE_3F8_DISABLE;
		break;
	case 1:
		ubox_uart_en |= UBOX_UART_ENABLE_PORT1;
		dfx1 |= VTD_DFX1_RANGE_2F8_DISABLE;
		break;
	default:
		printk(BIOS_ERR, "incorrect port number\n");
		return;
	}

	/* Disable decoding and enable the port we want */
	pci_mmio_write_config32(VTD_PCI_DEV, VTD_DFX1, dfx1);
	pci_mmio_write_config32(ubox_dev, UBOX_UART_ENABLE, ubox_uart_en);
}

static void early_iio_hide(void)
{
	DEVTREE_CONST struct device *dev;

	const pci_devfn_t iio_rootport[] = {
		PCI_DEVFN(PCIE_IIO_PORT_1_DEV, PCIE_IIO_PORT_1A_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_1_DEV, PCIE_IIO_PORT_1B_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_2_DEV, PCIE_IIO_PORT_2A_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_2_DEV, PCIE_IIO_PORT_2B_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_2_DEV, PCIE_IIO_PORT_2C_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_2_DEV, PCIE_IIO_PORT_2D_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_3_DEV, PCIE_IIO_PORT_3A_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_3_DEV, PCIE_IIO_PORT_3B_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_3_DEV, PCIE_IIO_PORT_3C_FUNC),
		PCI_DEVFN(PCIE_IIO_PORT_3_DEV, PCIE_IIO_PORT_3D_FUNC),
		};

	/* Walk through IIO root ports and hide if it is disabled in devtree */
	for (int i = 0; i < ARRAY_SIZE(iio_rootport); i++) {
		dev = pcidev_path_on_bus(BUS0, iio_rootport[i]);
		if (dev && !dev->enabled) {
			printk(BIOS_DEBUG, "Hiding IIO root port: %d:%d.%d\n",
					BUS0,
					PCI_SLOT(iio_rootport[i]),
					PCI_FUNC(iio_rootport[i]));
			iio_hide(dev);
		}
	}
}

static void check_msr_lock(void)
{
	/*
	 * Sometimes the system boots in an invalid state, where random values
	 * have been written to MSRs and then the MSRs are locked.
	 * Seems to always happen on warm reset.
	 *
	 * Power cycling or a board_reset() isn't sufficient in this case, so
	 * issue a full_reset() to "fix" this issue.
	 */
	msr_t msr = rdmsr(IA32_FEATURE_CONTROL);
	if (msr.lo & 1) {
		console_init();
		printk(BIOS_EMERG, "Detected broken platform state. Issuing full reset\n");
		full_reset();
	}
}

/* Entry from cache-as-ram.inc. */
void *asmlinkage main(FSP_INFO_HEADER *fsp_info_header)
{
	post_code(0x40);

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (!CONFIG(INTEGRATED_UART)) {
	/* Enable decoding of I/O locations for Super I/O devices */
		pci_write_config16(PCI_DEV(0x0, LPC_DEV, LPC_FUNC),
					   LPC_IO_DEC, 0x0010);
		pci_write_config16(PCI_DEV(0x0, LPC_DEV, LPC_FUNC),
					   LPC_EN, 0x340f);
	} else {
		enable_integrated_uart(CONFIG_UART_FOR_CONSOLE);
	}

	check_msr_lock();

	/* Call into mainboard. */
	post_code(0x41);
	early_mainboard_romstage_entry();

	post_code(0x42);
	console_init();
	init_rtc();
	setup_gpio_io_address();

	/* Hide before MemoryInit since hiding later seems to break FSP */
	early_iio_hide();
	timestamp_add_now(TS_BEFORE_INITRAM);
	post_code(0x48);

	if (CONFIG(INTEL_TXT)) {
		printk(BIOS_DEBUG, "Check TXT_ERROR register\n");
		intel_txt_log_acm_error(read32((void *)TXT_ERROR));
	}

	/*
	 * Call early init to initialize memory and chipset. This function returns
	 * to the romstage_main_continue function with a pointer to the HOB
	 * structure.
	 */
	printk(BIOS_DEBUG, "Starting the Intel FSP (early_init)\n");
	fsp_early_init(fsp_info_header);
	die_with_post_code(POST_INVALID_VENDOR_BINARY,
		"Uh Oh! fsp_early_init should not return here.\n");
}

/*******************************************************************************
 * The FSP early_init function returns to this function.
 * Memory is set up and the stack is set by the FSP.
 */
void romstage_main_continue(EFI_STATUS status, void *hob_list_ptr)
{
	void *cbmem_hob_ptr;

	post_code(0x4a);
	timestamp_add_now(TS_AFTER_INITRAM);
	printk(BIOS_DEBUG, "%s status: %x  hob_list_ptr: %x\n",
		__func__, (u32) status, (u32) hob_list_ptr);

	/* FSP reconfigures USB, so reinit it to have debug */
	if (CONFIG(USBDEBUG_IN_PRE_RAM))
		usbdebug_hw_init(true);

	printk(BIOS_DEBUG, "FSP Status: 0x%0x\n", (u32)status);

	post_code(0x4b);
	late_mainboard_romstage_entry();

	post_code(0x4d);
	cbmem_recovery(0);

	/* Save the HOB pointer in CBMEM to be used in ramstage*/
	cbmem_hob_ptr = cbmem_add(CBMEM_ID_HOB_POINTER, sizeof(*hob_list_ptr));
	if (cbmem_hob_ptr == NULL)
		die("Could not allocate cbmem for HOB pointer");
	*(u32 *)cbmem_hob_ptr = (u32)hob_list_ptr;

	if (!CONFIG(FSP_MEMORY_DOWN))
		save_dimm_info();

	if (CONFIG(SMM_TSEG))
		smm_list_regions();

	/* Load the ramstage. */
	post_code(0x4e);
	run_ramstage();
	while (1);
}

uint64_t get_initial_timestamp(void)
{
	return 0;
}
