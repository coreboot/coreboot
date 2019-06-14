/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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
#include <soc/romstage.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <cpu/x86/msr.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/lpc.h>
#include <superio/ite/common/ite.h>

#define SUPERIO_DEV 0x6e
#define SERIAL_DEV PNP_DEV(SUPERIO_DEV, 1)

/**
 * /brief mainboard call for setup that needs to be done before fsp init
 *
 */
void early_mainboard_romstage_entry(void)
{
	/* Decode 0x6e/0x6f on LPC bus (actually 0x6c-0x6f) */
	pci_write_config32(PCI_DEV(0x0, LPC_DEV, LPC_FUNC), LPC_GEN1_DEC,
			   (0 << 16) | ALIGN_DOWN(SUPERIO_DEV, 4) | 1);

	/* Decode IPMI KCS */
	pci_write_config32(PCI_DEV(0x0, LPC_DEV, LPC_FUNC), LPC_GEN2_DEC,
			   (0 << 16) | ALIGN_DOWN(0xca2, 4) | 1);

	if (CONFIG(CONSOLE_SERIAL))
		ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);


	/*
	 * Sometimes the system boots in an invalid state, where random values
	 * have been written to MSRs and then the MSRs are locked.
	 * Seems to always happen on warm reset.
	 *
	 * Power cycling or a board_reset() isn't sufficient in this case, so
	 * issue a full_reset() to "fix" this issue.
	 *
	 * It seems to be a deficiency in the reset logic, as other
	 * FSP broadwell DE boards are not affected.
	 */
	msr_t msr = rdmsr(IA32_FEATURE_CONTROL);
	if (msr.lo & 1) {
		console_init();
		printk(BIOS_EMERG, "Detected broken platform state. Issuing full reset\n");
		full_reset();
	}
}

/**
 * /brief mainboard call for setup that needs to be done after fsp init
 *
 */
void late_mainboard_romstage_entry(void)
{

}

/**
 * /brief customize fsp parameters here if needed
 */
void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{
	UPD_DATA_REGION *fsp_upd_data = FspRtBuffer->Common.UpdDataRgnPtr;
	if (CONFIG(FSP_USES_UPD)) {
		/* The internal UART operates on 0x3f8/0x2f8.
		 * As it's not wired up and conflicts with SuperIO decoding
		 * the same range, make sure to disable it.
		 */
		fsp_upd_data->SerialPortControllerInit0 = 0;
		fsp_upd_data->SerialPortControllerInit1 = 0;

		/* coreboot will initialize UART.
		 * No need for FSP to do it again.
		 */
		fsp_upd_data->SerialPortConfigure = 0;
		fsp_upd_data->SerialPortBaudRate = 0;

		/* Make FSP use serial IO */
		if (CONFIG(CONSOLE_SERIAL))
			fsp_upd_data->SerialPortType = 1;
		else
			fsp_upd_data->SerialPortType = 0;
	}
}
