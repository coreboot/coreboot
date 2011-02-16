/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2011 Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <pc80/mc146818rtc.h>
#include <bitops.h>
#include "k8x8xx.h"

static void dram_enable(struct device *dev)
{
	msr_t msr;
	u16 reg;

	/*
	 * Enable Lowest Interrupt arbitration for APIC, enable NB APIC
	 * decoding, MSI support, no SMRAM, compatible SMM.
	 */
	pci_write_config8(dev, 0x86, 0x19);

	/*
	 * We want to use the 0xC0000-0xEFFFF as RAM mark area as RW, even if
	 * memory is doing K8 the DMA from SB will fail if we have it wrong,
	 * AND even we have it here, we must later copy it to SB to make it work :/
	 */

	/* For CC000-CFFFF, bits 7:6 (10 = REn, 01 = WEn) bits 1:0 for
	 * C0000-C3FFF etc.
	 */
	pci_write_config8(dev, 0x80, 0xff);
	/* For page D0000-DFFFF */
	pci_write_config8(dev, 0x81, 0xff);
	/* For page E0000-EFFFF */
	pci_write_config8(dev, 0x82, 0xff);
	pci_write_config8(dev, 0x83, 0x30);

	msr = rdmsr(TOP_MEM);
	reg = pci_read_config16(dev, 0x84);
	reg &= 0xf;
	pci_write_config16(dev, 0x84, (msr.lo >> 16) | reg);

	reg = pci_read_config16(dev, 0x88);
	reg &= 0xf800;

	/* The Address Next to the Last Valid DRAM Address */
	pci_write_config16(dev, 0x88, (msr.lo >> 24) | reg);
	
	print_debug(" VIA_X_3 device dump:\n");
	dump_south(dev);

}

#if CONFIG_GFXUMA
extern uint64_t uma_memory_base, uma_memory_size;
#endif

static void dram_enable_k8m890(struct device *dev)
{
#if CONFIG_GFXUMA
	msr_t msr;
	int ret;
	unsigned int fbbits;

	/* use CMOS */
	if (CONFIG_VIDEO_MB == -1) {
		ret = get_option(&fbbits, "videoram_size");
		if (ret) {
			printk(BIOS_WARNING, "Failed to get videoram size (error %d), using default.\n", ret);
			fbbits = 5;
	 	}

		if ((fbbits < 1) || (fbbits > 7)) {
			printk(BIOS_WARNING, "Invalid videoram size (%d), using default.\n",
				       4 << fbbits);
			fbbits = 5;
	}
		uma_memory_size = 4 << (fbbits + 20);
	} else {
		uma_memory_size = (CONFIG_VIDEO_MB << 20);
	}

	msr = rdmsr(TOP_MEM);
	uma_memory_base = msr.lo - uma_memory_size;
	printk(BIOS_INFO, "K8M890: UMA base is %llx size is %u (MB)\n", uma_memory_base,
				(u32) (uma_memory_size / 1024 / 1024));
	/* enable VGA, so the bridges gets VGA_EN and resources are set */
	pci_write_config8(dev, 0xa1, 0x80);
#endif
	dram_enable(dev);
}

int
k8m890_host_fb_size_get(void)
{
	struct device *dev = dev_find_device(PCI_VENDOR_ID_VIA,
					     PCI_DEVICE_ID_VIA_K8M800_DRAM, 0);
	if(!dev) dev = dev_find_device(PCI_VENDOR_ID_VIA,
					     PCI_DEVICE_ID_VIA_K8M890CE_3, 0);
	unsigned char tmp;

	tmp = pci_read_config8(dev, 0xA1);
	tmp >>= 4;
	if (tmp & 0x08)
		return 4 << (tmp & 7);
	else
		return 0;
}

static void dram_init_fb(struct device *dev)
{
#if CONFIG_GFXUMA
	/* Important bits:
	 * Enable the internal GFX bit 7 of reg 0xa1 plus in same reg:
	 * bits 6:4 X fbuffer size will be  2^(X+2) or 100 = 64MB, 101 = 128MB
	 * bits 3:0 BASE [31:28]
	 * reg 0xa0 bits 7:1 BASE [27:21] bit0 enable CPU access
	 */
	unsigned int fbbits = 0;
	u8 tmp;

	fbbits = ((log2(uma_memory_size >> 20) - 2) << 4);
	printk(BIOS_INFO, "K8M890: Using a %dMB framebuffer.\n", (unsigned int) (uma_memory_size >> 20));

	/* Step 1: enable UMA but no FB */
	pci_write_config8(dev, 0xa1, 0x80);

	/* Step 2: enough is just the FB size, the CPU accessible address is not needed */
	tmp = fbbits | 0x80;
	pci_write_config8(dev, 0xa1, tmp);

	/* TODO K8 needs some UMA fine tuning too maybe call some generic routine here? */
#endif
}

static const struct device_operations dram_ops_t = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= dram_enable,
	.ops_pci		= 0,
};

static const struct device_operations dram_ops_m = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= dram_enable_k8m890,
	.init			= dram_init_fb,
	.ops_pci		= 0,
};

static const struct pci_driver northbridge_driver_t800 __pci_driver = {
	.ops	= &dram_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T800_DRAM,
};

static const struct pci_driver northbridge_driver_m800 __pci_driver = {
	.ops	= &dram_ops_m,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8M800_DRAM,
};

static const struct pci_driver northbridge_driver_t890 __pci_driver = {
	.ops	= &dram_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_3,
};

static const struct pci_driver northbridge_driver_t890cf __pci_driver = {
	.ops	= &dram_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CF_3,
};

static const struct pci_driver northbridge_driver_m890 __pci_driver = {
	.ops	= &dram_ops_m,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8M890CE_3,
};
