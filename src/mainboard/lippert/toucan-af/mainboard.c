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
	volatile u8 *spi_base;	// base addr of Hudson's SPI host controller
	int i;
	printk(BIOS_DEBUG, CONFIG_MAINBOARD_PART_NUMBER " ENTER %s\n", __func__);

	/* Init Hudson GPIOs. */
	printk(BIOS_DEBUG, "Init FCH GPIOs @ 0x%08x\n", ACPI_MMIO_BASE+GPIO_BASE);
	FCH_IOMUX( 50) = 2;    // GPIO50: FCH_ARST#_GATE resets stuck PCIe devices
	FCH_GPIO ( 50) = 0xC0; // = output set to 1 as it's never needed
	FCH_IOMUX(197) = 2;    // GPIO197: BIOS_DEFAULTS#
	FCH_GPIO (197) = 0x28; // = input, disable int. pull-up
	FCH_IOMUX( 56) = 1;    // GPIO58-56: REV_ID2-0
	FCH_GPIO ( 56) = 0x28; // = inputs, disable int. pull-ups
	FCH_IOMUX( 57) = 1;
	FCH_GPIO ( 57) = 0x28;
	FCH_IOMUX( 58) = 1;
	FCH_GPIO ( 58) = 0x28;
	FCH_IOMUX(187) = 2;    // GPIO187,188,166,GPO160: GPO0-3 on COM Express connector
	FCH_GPIO (187) = 0x08; // = outputs, disable PUs, default to 0
	FCH_IOMUX(188) = 2;
	FCH_GPIO (188) = 0x08;
	FCH_IOMUX(166) = 2;
	FCH_GPIO (166) = 0x08;
	// needed to make GPO160 work (Hudson Register Reference section 2.3.6.1)
	FCH_PMIO(0xDC) &= ~0x80; FCH_PMIO(0xE6) = (FCH_PMIO(0xE6) & ~0x02) | 0x01;
	FCH_IOMUX(160) = 1;
	FCH_GPIO (160) = 0x08;
	FCH_IOMUX(189) = 1;    // GPIO189-192: GPI0-3 on COM Express connector
	FCH_IOMUX(190) = 1;    // default to inputs with int. PU
	FCH_IOMUX(191) = 1;
	FCH_IOMUX(192) = 1;
	if (!fch_gpio_state(197)) // just in case anyone cares
		printk(BIOS_INFO, "BIOS_DEFAULTS jumper is present.\n");
	printk(BIOS_INFO, "Board revision ID: %u\n",
	       fch_gpio_state(58)<<2 | fch_gpio_state(57)<<1 | fch_gpio_state(56));

	/* Lower SPI speed from default 66 to 22 MHz for SST 25VF032B */
	spi_base = (u8*)(pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x14, 3)), 0xA0) & 0xFFFFFFE0);
	spi_base[0x0D] = (spi_base[0x0D] & ~0x30) | 0x20; // NormSpeed in SPI_Cntrl1 register

	/* Notify the SMC we're alive and kicking, or after a while it will
	 * effect a power cycle and switch to the alternate BIOS chip.
	 * Should be done as late as possible. */
	printk(BIOS_INFO, "Sending BIOS alive message\n");
	const u8 i_am_alive[] = { 0x03 }; //bit2=SEL_DP0: 0=DDI2, 1=LVDS
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

	/* enable GPP CLK0 thru CLK1 */
	/* disable GPP CLK2 thru SLT_GFX_CLK */
	u8 *misc_mem_clk_cntrl = (u8 *)(ACPI_MMIO_BASE + MISC_BASE);
	*(misc_mem_clk_cntrl + 0) = 0xFF;
	*(misc_mem_clk_cntrl + 1) = 0x00;
	*(misc_mem_clk_cntrl + 2) = 0x00;
	*(misc_mem_clk_cntrl + 3) = 0x00;
	*(misc_mem_clk_cntrl + 4) = 0x00;

	/*
	 * Initialize ASF registers to an arbitrary address because someone
	 * long ago set things up this way inside the SPD read code.  The
	 * SPD read code has been made generic and moved out of the board
	 * directory, so the ASF init is being done here.
	 */
	pm_iowrite(0x29, 0x80);
	pm_iowrite(0x28, 0x61);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
