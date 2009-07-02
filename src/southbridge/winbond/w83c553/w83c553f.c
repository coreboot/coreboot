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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * Initialisation of the PCI-to-ISA bridge and disabling the BIOS
 * write protection (for flash) in function 0 of the chip.
 * Enabling function 1 (IDE controller of the chip.
 */

#ifndef CONFIG_ISA_IO
#define CONFIG_ISA_IO         0xFE000000
#endif

#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "w83c553f.h"

void initialise_pic(void);
void initialise_dma(void);

static void 
w83c553_init(struct device *dev)
{
	printk_info("Configure W83C553F ISA Bridge (Function 0)\n");

#if 0
	/*
	 * Set ISA memory space NOT SURE ABOUT THIS???
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
	pci_write_config8(dev, W83C553F_IDEIRCR, 0x90);
	pci_write_config16(dev, W83C553F_PCIIRCR, 0xABEF);

	/*
	 * Initialise ISA interrupt controller
	 */
	initialise_pic();

	/*
	 * Initialise DMA controller
	 */
	initialise_dma();
#endif

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

static void w83c553_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static void w83c553_read_resources(device_t dev)
{
	struct resource* res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x400UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = 0xfec00000;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations w83c553_ops  = {
        .read_resources   = w83c553_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = w83c553_enable_resources,
        .init             = w83c553_init,
        .scan_bus         = scan_static_bus,
};

static const struct pci_driver w83c553f_pci_driver __pci_driver = {
	/* w83c553f */
	.ops = &w83c553_ops,
	.device = PCI_DEVICE_ID_WINBOND_83C553,
	.vendor = PCI_VENDOR_ID_WINBOND,
};
