/*
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Initialisation of the PCI-to-ISA bridge and disabling the BIOS
 * write protection (for flash) in function 0 of the chip.
 * Enabling function 1 (IDE controller of the chip.
 */

#include <arch/io.h>
#include <device/pci.h>
#include <console/console.h>
#include "w83c553f.h"

#ifndef CONFIG_ISA_MEM
#define CONFIG_ISA_MEM		0xFD000000
#endif
#ifndef CONFIG_ISA_IO
#define CONFIG_ISA_IO		0xFE000000
#endif

#ifndef CONFIG_IDE_MAXBUS
#define CONFIG_IDE_MAXBUS	2
#endif
#ifndef CONFIG_IDE_MAXDEVICE
#define CONFIG_IDE_MAXDEVICE	(CONFIG_IDE_MAXBUS*2)
#endif

uint32_t ide_bus_offset[CONFIG_IDE_MAXBUS];

void initialise_pic(void);
void initialise_dma(void);

extern struct pci_ops pci_direct_ppc;

#if 0
void southbridge_early_init(void)
{
	unsigned char reg8;

	/*
	 * Set ISA memory space
	 */
	pci_direct_ppc.read_byte(0, 0x58, W83C553F_IPADCR, &reg8);
	/* 16 MB ISA memory space */
	reg8 |= (W83C553F_IPADCR_IPATOM4 | W83C553F_IPADCR_IPATOM5 | W83C553F_IPADCR_IPATOM6 | W83C553F_IPADCR_IPATOM7);
	reg8 &= ~W83C553F_IPADCR_MBE512;
	pci_direct_ppc.write_byte(0, 0x58, W83C553F_IPADCR, &reg8);
}
#endif

void southbridge_init(void)
{
	struct device  *dev;
	unsigned char reg8;
	unsigned short reg16;
	unsigned int reg32;

	dev = dev_find_device(W83C553F_VID, W83C553F_DID, 0);
	if (dev == 0)
	{
		printk_info("Error: Cannot find W83C553F controller on any PCI bus\n");
		return;
	}

	printk_info("Found W83C553F controller\n");

	/* always enabled */
#if 0
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MASTER | PCI_COMMAND_IO | PCI_COMMAND_MEMORY;
	pci_write_config16(dev, PCI_COMMAND, reg16);
#endif

	/*
	 * Set ISA memory space
	 */
	reg8 = pci_read_config8(dev, W83C553F_IPADCR);
	/* 16 MB ISA memory space */
	reg8 |= (W83C553F_IPADCR_IPATOM4 | W83C553F_IPADCR_IPATOM5 | W83C553F_IPADCR_IPATOM6 | W83C553F_IPADCR_IPATOM7);
	reg8 &= ~W83C553F_IPADCR_MBE512;
	pci_write_config8(dev, W83C553F_IPADCR, reg8);

	/*
	 * Chip select: switch off BIOS write protection
	 */
	reg8 = pci_read_config8(dev, W83C553F_CSCR);
	reg8 |= W83C553F_CSCR_UBIOSCSE;
	reg8 &= ~W83C553F_CSCR_BIOSWP;
	pci_write_config8(dev, W83C553F_CSCR, reg8);


	/*
	 * Enable Port 92
	 */
	reg8 = W83C553F_ATSCR_P92E | W83C553F_ATSCR_KRCEE;
	pci_write_config8(dev, W83C553F_CSCR, reg8);

	/*
	 * Route IDE interrupts to IRQ 14 & 15 on 8259.
	 */
	pci_write_config8(dev, W83C553F_IDEIRCR, 0xef);
	pci_write_config16(dev, W83C553F_PCIIRCR, 0x0000);

	/*
	 * Read IDE bus offsets from function 1 device.
	 * We must unmask the LSB indicating that it is an IO address.
	 */
	dev = dev_find_device(W83C553F_VID, W83C553F_IDE, 0);
	if (dev == 0)
	{
		printk_info("Error: Cannot find W83C553F function 1 device\n");
		return;
	}

	/*
	 * Enable native mode on IDE ports and set base address.
	 */
	reg8 = W83C553F_PIR_P1NL | W83C553F_PIR_P0NL;
	pci_write_config8(dev, W83C553F_PIR, reg8);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, 0x1f0);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_1);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0x3f6);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_1);
	pci_write_config32(dev, PCI_BASE_ADDRESS_2, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	pci_write_config32(dev, PCI_BASE_ADDRESS_2, 0x170);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	pci_write_config32(dev, PCI_BASE_ADDRESS_3, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_3);
	pci_write_config32(dev, PCI_BASE_ADDRESS_3, 0x376);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_3);

	/*
	 * Set read-ahead duration to 0xff
	 * Enable P0 and P1
	 */
	reg32 = 0x00ff0000 | W83C553F_IDECSR_P1EN | W83C553F_IDECSR_P0EN;
	pci_write_config32(dev, W83C553F_IDECSR, reg32);

	ide_bus_offset[0] = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	printk_debug("ide bus offset = 0x%x\n", ide_bus_offset[0]);
	ide_bus_offset[0] &= ~1;
#if CONFIG_IDE_MAXBUS > 1
	ide_bus_offset[1] = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	ide_bus_offset[1] &= ~1;
#endif

	/*
	 * Enable function 1, IDE -> busmastering and IO space access
	 */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MASTER | PCI_COMMAND_IO;
	pci_write_config16(dev, PCI_COMMAND, reg16);
	reg16 = pci_read_config16(dev, PCI_COMMAND);

	/*
	 * Initialise ISA interrupt controller
	 */
	initialise_pic();

	/*
	 * Initialise DMA controller
	 */
	initialise_dma();

	printk_info("W83C553F configuration complete\n");
}

void initialise_pic(void)
{
	outb(W83C553F_PIC1_ICW1, 0x11); /* start init sequence, ICW4 needed */
	outb(W83C553F_PIC1_ICW2, 0x08); /* base address 00001 */
	outb(W83C553F_PIC1_ICW3, 0x04); /* slave on IRQ2 */
	outb(W83C553F_PIC1_ICW4, 0x01); /* x86 mode */
	outb(W83C553F_PIC1_OCW1, 0xfb); /* enable IRQ 2 */
	outb(W83C553F_PIC1_ELC, 0xf8);	/* all IRQ's edge sensitive */

	outb(W83C553F_PIC2_ICW1, 0x11); /* start init sequence, ICW4 needed */
	outb(W83C553F_PIC2_ICW2, 0x08); /* base address 00001 */
	outb(W83C553F_PIC2_ICW3, 0x02); /* slave ID 2 */
	outb(W83C553F_PIC2_ICW4, 0x01); /* x86 mode */
	outb(W83C553F_PIC2_OCW1, 0xff); /* disable all IRQ's */
	outb(W83C553F_PIC2_ELC, 0xde);	/* all IRQ's edge sensitive */

	outb(W83C553F_TMR1_CMOD, 0x74);

	outb(W83C553F_PIC2_OCW1, 0x20);
	outb(W83C553F_PIC1_OCW1, 0x20);

	outb(W83C553F_PIC2_OCW1, 0x2b);
	outb(W83C553F_PIC1_OCW1, 0x2b);
}

void initialise_dma(void)
{
	unsigned int channel;
	unsigned int rvalue1, rvalue2;

	/* perform a H/W reset of the devices */

	outb(W83C553F_DMA1 + W83C553F_DMA1_MC, 0x00);
	outw(W83C553F_DMA2 + W83C553F_DMA2_MC, 0x0000);

	/* initialise all channels to a sane state */

	for (channel = 0; channel < 4; channel++) {
		/*
		 * dependent upon the channel, setup the specifics:
		 *
		 * demand
		 * address-increment
		 * autoinitialize-disable
		 * verify-transfer
		 */

		switch (channel) {
		case 0:
			rvalue1 = (W83C553F_MODE_TM_DEMAND|W83C553F_MODE_CH0SEL|W83C553F_MODE_TT_VERIFY);
			rvalue2 = (W83C553F_MODE_TM_CASCADE|W83C553F_MODE_CH0SEL);
			break;
	    	case 1:
			rvalue1 = (W83C553F_MODE_TM_DEMAND|W83C553F_MODE_CH1SEL|W83C553F_MODE_TT_VERIFY);
			rvalue2 = (W83C553F_MODE_TM_DEMAND|W83C553F_MODE_CH1SEL|W83C553F_MODE_TT_VERIFY);
			break;
		case 2:
			rvalue1 = (W83C553F_MODE_TM_DEMAND|W83C553F_MODE_CH2SEL|W83C553F_MODE_TT_VERIFY);
			rvalue2 = (W83C553F_MODE_TM_DEMAND|W83C553F_MODE_CH2SEL|W83C553F_MODE_TT_VERIFY);
			break;
		case 3:
			rvalue1 = (W83C553F_MODE_TM_DEMAND|W83C553F_MODE_CH3SEL|W83C553F_MODE_TT_VERIFY);
			rvalue2 = (W83C553F_MODE_TM_DEMAND|W83C553F_MODE_CH3SEL|W83C553F_MODE_TT_VERIFY);
			break;
		default:
			rvalue1 = 0x00;
			rvalue2 = 0x00;
			break;
		}

		/* write to write mode registers */

		outb(W83C553F_DMA1 + W83C553F_DMA1_WM, rvalue1 & 0xFF);
		outw(W83C553F_DMA2 + W83C553F_DMA2_WM, rvalue2 & 0x00FF);
	}

	/* enable all channels */

	outb(W83C553F_DMA1 + W83C553F_DMA1_CM, 0x00);
	outw(W83C553F_DMA2 + W83C553F_DMA2_CM, 0x0000);
	/*
	 * initialize the global DMA configuration
	 *
	 * DACK# active low
	 * DREQ active high
	 * fixed priority
	 * channel group enable
	 */

	outb(W83C553F_DMA1 + W83C553F_DMA1_CS, 0x00);
	outw(W83C553F_DMA2 + W83C553F_DMA2_CS, 0x0000);
}
