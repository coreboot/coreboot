/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <device/pci_def.h>
#include <southbridge/amd/sb800/sb800.h>
#include <arch/acpi.h>
#include "BiosCallOuts.h"
#include <cpu/amd/agesa/s3_resume.h>
#include <cpu/amd/mtrr.h>
#include "SBPLATFORM.h"
#include "OEM.h" /* SMBUS0_BASE_ADDRESS */

//#include "broadcom.c" /* only needed for CFR-AF 0v0 prototype */

/* Init SIO GPIOs. */
#define SIO_RUNTIME_BASE 0x0E00
static const u16 sio_init_table[] = { // hi=offset, lo=value
	0x4BA0, // GP1x: COM1/2 control   = RS232, no term, max 115200
	0x2300, // GP10: COM1 termination = push/pull output
	0x2400, // GP11: COM2 termination = push/pull output
	0x2500, // GP12: COM1 RS485 mode  = push/pull output
	0x2600, // GP13: COM2 RS485 mode  = push/pull output
	0x2700, // GP14: COM1 speed A     = push/pull output
	0x2900, // GP15: COM1 speed B     = push/pull output
	0x2A00, // GP16: COM2 speed A     = push/pull output
	0x2B00, // GP17: COM2 speed B     = push/pull output

	0x3904, // GP36                   = KBDRST# function

	0x4E74, // GP4x: Ethernet enable  = on
	0x6E84, // GP44: Ethernet enable  = open drain output

	// GP5x = COM2 function instead of GPIO
	0x3F05, 0x4005, 0x4105, 0x4204, 0x4305, 0x4404, 0x4505, 0x4604,

	0x470C, // GP60                   = WDT function
	0x5E00, // LED2: Live LED         = off
	0x4884, // GP61: Live LED         = LED2 function

	0x5038, // GP6x: USB power        = 3x on
	0x5580, // GP63: USB power 0/1    = open drain output
	0x5680, // GP64: USB power 2/3    = open drain output
	0x5780, // GP65: USB power 4/5    = open drain output
};

/* Write data block to slave on SMBUS0. */
#define SMB0_STATUS	((SMBUS0_BASE_ADDRESS) + 0)
#define SMB0_CONTROL	((SMBUS0_BASE_ADDRESS) + 2)
#define SMB0_HOSTCMD	((SMBUS0_BASE_ADDRESS) + 3)
#define SMB0_ADDRESS	((SMBUS0_BASE_ADDRESS) + 4)
#define SMB0_DATA0	((SMBUS0_BASE_ADDRESS) + 5)
#define SMB0_BLOCKDATA	((SMBUS0_BASE_ADDRESS) + 7)
static int smb_write_blk(u8 slave, u8 command, u8 length, const u8 *data)
{
	__outbyte(SMB0_STATUS, 0x1E);		// clear error status
	__outbyte(SMB0_ADDRESS, slave & ~1);	// slave addr + direction=out
	__outbyte(SMB0_HOSTCMD, command);	// or destination offset
	__outbyte(SMB0_DATA0, length);		// sent before data
	__inbyte(SMB0_CONTROL);			// reset block data array
	while (length--)
		__outbyte(SMB0_BLOCKDATA, *(data++));
	__outbyte(SMB0_CONTROL, 0x54);		// execute block write, no IRQ

	while (__inbyte(SMB0_STATUS) == 0x01) ;	// busy, no errors
	return __inbyte(SMB0_STATUS) ^ 0x02;	// 0x02 = completed, no errors
}

static void init(struct device *dev)
{
	int i;
	printk(BIOS_DEBUG, CONFIG_MAINBOARD_PART_NUMBER " ENTER %s\n", __func__);

	/* Init Hudson GPIOs. */
	printk(BIOS_DEBUG, "Init FCH GPIOs @ 0x%08x\n", ACPI_MMIO_BASE+GPIO_BASE);
	FCH_IOMUX( 50) = 2;    // GPIO50: FCH_ARST#_GATE resets stuck PCIe devices
	FCH_GPIO ( 50) = 0xC0; // = output set to 1 as it's never needed
	FCH_IOMUX(197) = 2;    // GPIO197: BIOS_DEFAULTS# = input (int. PU)
	FCH_IOMUX( 56) = 1;    // GPIO58-56: REV_ID2-0
	FCH_GPIO ( 56) = 0x28; // = inputs, disable int. pull-ups
	FCH_IOMUX( 57) = 1;
	FCH_GPIO ( 57) = 0x28;
	FCH_IOMUX( 58) = 1;
	FCH_GPIO ( 58) = 0x28;
	FCH_IOMUX( 96) = 1;    // "Gpio96": GEVENT0# signal on X2 connector (int. PU)
	FCH_IOMUX( 52) = 1;    // GPIO52,61,62,187-192 free to use on X2 connector
	FCH_IOMUX( 61) = 2;    // default to inputs with int. PU
	FCH_IOMUX( 62) = 2;
	FCH_IOMUX(187) = 2;
	FCH_IOMUX(188) = 2;
	FCH_IOMUX(189) = 1;
	FCH_IOMUX(190) = 1;
	FCH_IOMUX(191) = 1;
	FCH_IOMUX(192) = 1;
	if (!fch_gpio_state(197)) // just in case anyone cares
		printk(BIOS_INFO, "BIOS_DEFAULTS jumper is present.\n");
	printk(BIOS_INFO, "Board revision ID: %u\n",
	       fch_gpio_state(58)<<2 | fch_gpio_state(57)<<1 | fch_gpio_state(56));

	/* Init SIO GPIOs. */
	printk(BIOS_DEBUG, "Init SIO GPIOs @ 0x%04x\n", SIO_RUNTIME_BASE);
	for (i = 0; i < ARRAY_SIZE(sio_init_table); i++) {
		u16 val = sio_init_table[i];
		outb((u8)val, SIO_RUNTIME_BASE + (val >> 8));
	}

	/* Upload AMD A55E GbE 'NV'RAM contents. Only needed on CFR-AF revision
	 * 0v0 (prototype), not for any later production versions. */
	//broadcom_init();

	/* Notify the SMC we're alive and kicking, or after a while it will
	 * effect a power cycle and switch to the alternate BIOS chip.
	 * Should be done as late as possible. */
	printk(BIOS_INFO, "Sending BIOS alive message\n");
	const u8 i_am_alive[] = { 0x03 };
	if ((i = smb_write_blk(0x50, 0x25, sizeof(i_am_alive), i_am_alive)))
		printk(BIOS_ERR, "smb_write_blk failed: %d\n", i);

	printk(BIOS_DEBUG, CONFIG_MAINBOARD_PART_NUMBER " EXIT %s\n", __func__);
}

void set_pcie_reset(void);
void set_pcie_dereset(void);

/**
 * TODO
 * SB CIMx callback
 */
void set_pcie_reset(void)
{
}

/**
 * TODO
 * mainboard specific SB CIMx callback
 */
void set_pcie_dereset(void)
{
}


/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
	dev->ops->init = init;

/*
 * The mainboard is the first place that we get control in ramstage. Check
 * for S3 resume and call the approriate AGESA/CIMx resume functions.
 */
#if CONFIG_HAVE_ACPI_RESUME
	acpi_slp_type = acpi_get_sleep_type();
#endif

	/* enable GPP CLK0 */
	/* disable GPP CLK1 thru SLT_GFX_CLK */
	u8 *misc_mem_clk_cntrl = (u8 *)(ACPI_MMIO_BASE + MISC_BASE);
	*(misc_mem_clk_cntrl + 0) = 0x0F;
	*(misc_mem_clk_cntrl + 1) = 0x00;
	*(misc_mem_clk_cntrl + 2) = 0x00;
	*(misc_mem_clk_cntrl + 3) = 0x00;
	*(misc_mem_clk_cntrl + 4) = 0x00;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
