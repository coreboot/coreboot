/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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

/* Inspiration from other VIA SB code. */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>
#include <stdlib.h>
#include "vt8237r.h"
#include "chip.h"

#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)

extern void dump_south(device_t dev);

static struct ioapicreg {
	u32 reg;
	u32 value_low;
	u32 value_high;
} ioapic_table[] = {
	/* IO-APIC virtual wire mode configuration. */
	/* mask, trigger, polarity, destination, delivery, vector */
	{0,  ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST |
	     ExtINT, NONE},
	{1,  DISABLED, NONE},
	{2,  DISABLED, NONE},
	{3,  DISABLED, NONE},
	{4,  DISABLED, NONE},
	{5,  DISABLED, NONE},
	{6,  DISABLED, NONE},
	{7,  DISABLED, NONE},
	{8,  DISABLED, NONE},
	{9,  DISABLED, NONE},
	{10, DISABLED, NONE},
	{11, DISABLED, NONE},
	{12, DISABLED, NONE},
	{13, DISABLED, NONE},
	{14, DISABLED, NONE},
	{15, DISABLED, NONE},
	{16, DISABLED, NONE},
	{17, DISABLED, NONE},
	{18, DISABLED, NONE},
	{19, DISABLED, NONE},
	{20, DISABLED, NONE},
	{21, DISABLED, NONE},
	{22, DISABLED, NONE},
	{23, DISABLED, NONE},
};

static void setup_ioapic(u32 ioapic_base)
{
	u32 value_low, value_high, val;
	volatile u32 *l;
	int i;

	/* All delivered to CPU0. */
	ioapic_table[0].value_high = (lapicid()) << (56 - 32);
	l = (unsigned long *)ioapic_base;

	/* Set APIC to FSB message bus. */
	l[0] = 0x3;
	val = l[4];
	l[4] = (val & 0xFFFFFE) | 1;

	/* Set APIC ADDR - this will be VT8237R_APIC_ID. */
	l[0] = 0;
	val = l[4];
	l[4] = (val & 0xF0FFFF) | (VT8237R_APIC_ID << 24);

	for (i = 0; i < ARRAY_SIZE(ioapic_table); i++) {
		l[0] = (ioapic_table[i].reg * 2) + 0x10;
		l[4] = ioapic_table[i].value_low;
		value_low = l[4];
		l[0] = (ioapic_table[i].reg * 2) + 0x11;
		l[4] = ioapic_table[i].value_high;
		value_high = l[4];

		if ((i == 0) && (value_low == 0xffffffff)) {
			printk_warning("IO APIC not responding.\n");
			return;
		}
	}
}

static void southbridge_init_common(struct device *dev);

/** Set up PCI IRQ routing, route everything through APIC. */
static void pci_routing_fixup(struct device *dev)
{
	/* PCI PNP Interrupt Routing INTE/F - disable */
	pci_write_config8(dev, 0x44, 0x00);

	/* PCI PNP Interrupt Routing INTG/H - disable */
	pci_write_config8(dev, 0x45, 0x00);

	/* Route INTE-INTH through registers above, no map to INTA-INTD. */
	pci_write_config8(dev, 0x46, 0x10);

	/* PCI Interrupt Polarity */
	pci_write_config8(dev, 0x54, 0x00);

	/* PCI INTA# Routing */
	pci_write_config8(dev, 0x55, 0x00);

	/* PCI INTB#/C# Routing */
	pci_write_config8(dev, 0x56, 0x00);

	/* PCI INTD# Routing */
	pci_write_config8(dev, 0x57, 0x00);
}

/**
 * Set up the power management capabilities directly into ACPI mode.
 * This avoids having to handle any System Management Interrupts (SMIs).
 */
static void setup_pm(device_t dev)
{
	/* Debounce LID and PWRBTN# Inputs for 16ms. */
	pci_write_config8(dev, 0x80, 0x20);

	/* Set ACPI base address to I/O VT8237R_ACPI_IO_BASE. */
	pci_write_config16(dev, 0x88, VT8237R_ACPI_IO_BASE | 0x1);

	/* Set ACPI to 9, must set IRQ 9 override to level! Set PSON gating. */
	pci_write_config8(dev, 0x82, 0x40 | VT8237R_ACPI_IRQ);

	/* Primary interupt channel, define wake events 0=IRQ0 15=IRQ15 1=en. */
	pci_write_config16(dev, 0x84, 0x30b2);

	/* SMI output level to low, 7.5us throttle clock */
	pci_write_config8(dev, 0x8d, 0x18);

	/* GP Timer Control 1s */
	pci_write_config8(dev, 0x93, 0x88);

	/* 7 = SMBus clock from RTC 32.768KHz
	 * 5 = Internal PLL reset from susp
	 * 2 = GPO2 is GPIO
	 */
	pci_write_config8(dev, 0x94, 0xa4);

	/* 7 = stp to sust delay 1msec
	 * 6 = SUSST# Deasserted Before PWRGD for STD
	 * 4 = PWRGOOD reset on VT8237A/S
	 * 3 = GPO26/GPO27 is GPO 
	 * 2 = Disable Alert on Lan
	 */
	pci_write_config8(dev, 0x95, 0xcc);

	/* Disable GP3 timer. */
	pci_write_config8(dev, 0x98, 0);

	/* Enable ACPI accessm RTC signal gated with PSON. */
	pci_write_config8(dev, 0x81, 0x84);

	/* Clear status events. */
	outw(0xffff, VT8237R_ACPI_IO_BASE + 0x00);
	outw(0xffff, VT8237R_ACPI_IO_BASE + 0x20);
	outw(0xffff, VT8237R_ACPI_IO_BASE + 0x28);
	outl(0xffffffff, VT8237R_ACPI_IO_BASE + 0x30);

	/* Disable SCI on GPIO. */
	outw(0x0, VT8237R_ACPI_IO_BASE + 0x22);

	/* Disable SMI on GPIO. */
	outw(0x0, VT8237R_ACPI_IO_BASE + 0x24);

	/* Disable all global enable SMIs. */
	outw(0x0, VT8237R_ACPI_IO_BASE + 0x2a);

	/* All SMI off, both IDE buses ON, PSON rising edge. */
	outw(0x0, VT8237R_ACPI_IO_BASE + 0x2c);

	/* Primary activity SMI disable. */
	outl(0x0, VT8237R_ACPI_IO_BASE + 0x34);

	/* GP timer reload on none. */
	outl(0x0, VT8237R_ACPI_IO_BASE + 0x38);

	/* Disable extended IO traps. */
	outb(0x0, VT8237R_ACPI_IO_BASE + 0x42);

	/* SCI is generated for RTC/pwrBtn/slpBtn. */
	outw(0x001, VT8237R_ACPI_IO_BASE + 0x04);

}


static void vt8237r_init(struct device *dev) {
	u8 enables;

	/* Enable SATA LED, disable special CPU Frequency Change -
	 * GPIO28 GPIO22 GPIO29 GPIO23 are GPIOs.
	 */
	pci_write_config8(dev, 0xe5, 0x9);

	/* REQ5 as PCI request input - should be together with INTE-INTH. */
	pci_write_config8(dev, 0xe4, 0x4);

	/* Set bit 3 of 0x4f (use INIT# as CPU reset). */
	enables = pci_read_config8(dev, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev, 0x4f, enables);

	/* Set Read Pass Write Control Enable (force A2 from APIC FSB to low). */
	pci_write_config8(dev, 0x48, 0x8c);

	southbridge_init_common(dev);

	/* FIXME: Intel needs more bit set for C2/C3. */

	/* Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change.
	 */
	outb(0x1, VT8237R_ACPI_IO_BASE + 0x11);
}

static void vt8237s_init(struct device *dev)
{
	u32 tmp;
	
	/* put SPI base VT8237S_SPI_MEM_BASE */
	tmp = pci_read_config32(dev, 0xbc);
	pci_write_config32(dev, 0xbc, (VT8237S_SPI_MEM_BASE >> 8) | (tmp & 0xFF000000));

	/* Enable SATA LED, VR timer = 100us, VR timer should be fixed */
	
	pci_write_config8(dev, 0xe5, 0x69);

	/* REQ5 as PCI request input - should be together with INTE-INTH. 
	 * Fast VR timer disable - need for LDTSTOP_L signal
	*/
	pci_write_config8(dev, 0xe4, 0xa5);

	/* reduce further the STPCLK/LDTSTP signal to 5us */

	pci_write_config8(dev, 0xec, 0x4);

	/* Host Bus Power Management Control, maybe not needed */
	pci_write_config8(dev, 0x8c, 0x5);

	/* Enable HPET at VT8237R_HPET_ADDR., does not work correctly on R */
	pci_write_config32(dev, 0x68, (VT8237R_HPET_ADDR | 0x80));

	southbridge_init_common(dev);
	
	/* FIXME: Intel needs more bit set for C2/C3. */

	/* Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change. FIXME FIXME, pre rev A2
	 */
	outb(0xff, VT8237R_ACPI_IO_BASE + 0x50);
	dump_south(dev);
}
static void vt8237_common_init(struct device *dev)
{
	u8 enables, byte;

	/* Enable addr/data stepping. */
	byte = pci_read_config8(dev, PCI_COMMAND);
	byte |= PCI_COMMAND_WAIT;
	pci_write_config8(dev, PCI_COMMAND, byte);

	/* Enable the internal I/O decode. */
	enables = pci_read_config8(dev, 0x6C);
	enables |= 0x80;
	pci_write_config8(dev, 0x6C, enables);

	/*
	 * ROM decode
	 * bit range
	 *   7 000E0000h-000EFFFFh
	 *   6 FFF00000h-FFF7FFFFh
	 *   5 FFE80000h-FFEFFFFFh
	 *   4 FFE00000h-FFE7FFFFh
	 *   3 FFD80000h-FFDFFFFFh
	 *   2 FFD00000h-FFD7FFFFh
	 *   1 FFC80000h-FFCFFFFFh
	 *   0 FFC00000h-FFC7FFFFh
	 * So 0x7f here sets ROM decode to FFC00000-FFFFFFFF or 4Mbyte.
	 */
	pci_write_config8(dev, 0x41, 0x7f);

	/* Set bit 6 of 0x40 (I/O recovery time).
	 * IMPORTANT FIX - EISA = ECLR reg at 0x4d0! Decoding must be on so
	 * that PCI interrupts can be properly marked as level triggered.
	 */
	enables = pci_read_config8(dev, 0x40);
	enables |= 0x44;
	pci_write_config8(dev, 0x40, enables);

	/* Line buffer control */
	enables = pci_read_config8(dev, 0x42);
	enables |= 0xf8;
	pci_write_config8(dev, 0x42, enables);

	/* Delay transaction control */
	pci_write_config8(dev, 0x43, 0xb);

	/* I/O recovery time, default IDE routing */
	pci_write_config8(dev, 0x4c, 0x44);

	/* ROM memory cycles go to LPC. */
	pci_write_config8(dev, 0x59, 0x80);

	/*
	 * bit meaning
	 *   3 Bypass APIC De-Assert Message (1=Enable)
	 *   1 possibly "INTE#, INTF#, INTG#, INTH# as PCI"
	 *     bit 1=1 works for Aaron at VIA, bit 1=0 works for jakllsch
	 *   0 Dynamic Clock Gating Main Switch (1=Enable)
	 */
	pci_write_config8(dev, 0x5b, 0xb);

	/* Set 0x58 to 0x43 APIC and RTC. */
	pci_write_config8(dev, 0x58, 0x43);

	/* Enable serial IRQ, 6PCI clocks. */
	pci_write_config8(dev, 0x52, 0x9);

	/* Power management setup */
	setup_pm(dev);

	/* Start the RTC. */
	rtc_init(0);
}

static void vt8237r_read_resources(device_t dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);
	/* Fixed APIC resource */
	res = new_resource(dev, 0x44);
	res->base = VT8237R_APIC_BASE;
	res->size = 256;
	res->limit = res->base + res->size - 1;
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

/**
 * The VT8237R is not a PCI bridge and has no resources of its own (other
 * than standard PC I/O addresses), however it does control the ISA bus
 * and so we need to manually call enable childrens resources on that bus.
 */
static void vt8237r_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static void init_keyboard(struct device *dev)
{
	u8 regval = pci_read_config8(dev, 0x51);
	if (regval & 0x1)
		init_pc_keyboard(0x60, 0x64, 0);
}

static void southbridge_init_common(struct device *dev)
{
	vt8237_common_init(dev);
	pci_routing_fixup(dev);
	setup_ioapic(VT8237R_APIC_BASE);
	setup_i8259();
	init_keyboard(dev);
}

static const struct device_operations vt8237r_lpc_ops_s = {
	.read_resources		= vt8237r_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= vt8237r_enable_resources,
	.init			= &vt8237s_init,
	.scan_bus		= scan_static_bus,
};


static const struct device_operations vt8237r_lpc_ops_r = {
	.read_resources		= vt8237r_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= vt8237r_enable_resources,
	.init			= &vt8237r_init,
	.scan_bus		= scan_static_bus,
};

static const struct pci_driver lpc_driver_r __pci_driver = {
	.ops	= &vt8237r_lpc_ops_r,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237R_LPC,
};

static const struct pci_driver lpc_driver_s __pci_driver = {
	.ops	= &vt8237r_lpc_ops_s,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237S_LPC,
};
