/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 Jon Harrison <bothlyn@blueyonder.co.uk>
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

/* Inspiration from other VIA SB code. */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <arch/ioapic.h>
#include <cpu/x86/lapic.h>
#include <cpu/cpu.h>
#include <pc80/keyboard.h>
#include <pc80/i8259.h>
#include <stdlib.h>
#include <arch/acpi.h>
#include "vt8237r.h"
#include "chip.h"

static void southbridge_init_common(struct device *dev);

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
                   /* Interrupts for  INT# A   B   C   D */
static const unsigned char pciIrqs[4]  = { 10, 11, 12, 0};

            /* Interrupt Assignments for Pins   1   2   3   4  */
static const unsigned char sataPins[4] =     { 'A','B','C','D'};
static const unsigned char vgaPins[4] =      { 'A','B','C','D'};
static const unsigned char usbPins[4] =      { 'A','B','C','D'};
static const unsigned char enetPins[4] =     { 'A','B','C','D'};
static const unsigned char vt8237Pins[4] =   { 'A','B','C','D'};
static const unsigned char slotPins[4] =     { 'C','D','A','B'};
static const unsigned char riserPins[4] =    { 'D','C','B','A'};

static unsigned char *pin_to_irq(const unsigned char *pin)
{
	static unsigned char Irqs[4];
	int i;
	for (i = 0; i < 4; i++)
		Irqs[i] = pciIrqs[ pin[i] - 'A' ];

	return Irqs;
}
#endif

/** Set up PCI IRQ routing, route everything through APIC. */
static void pci_routing_fixup(struct device *dev)
{
#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	struct device *pdev;
#endif

	/* PCI PNP Interrupt Routing INTE/F - disable */
	pci_write_config8(dev, 0x44, 0x00);

	/* PCI PNP Interrupt Routing INTG/H - disable */
	pci_write_config8(dev, 0x45, 0x00);

	/* Gate Interrupts until RAM Writes are flushed */
	pci_write_config8(dev, 0x49, 0x20);

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)

	/* Share INTE-INTH with INTA-INTD as per stock BIOS. */
	pci_write_config8(dev, 0x46, 0x00);

	/* setup PCI IRQ routing (For PCI Slot)*/
	pci_write_config8(dev, 0x55, pciIrqs[0] << 4);
	pci_write_config8(dev, 0x56, pciIrqs[1] | (pciIrqs[2] << 4) );
	pci_write_config8(dev, 0x57, pciIrqs[3] << 4);

	/* PCI Routing Fixup */

	//Setup MiniPCI Slot
	pci_assign_irqs(0, 0x14, pin_to_irq(slotPins));

	// Via 2 slot riser card 2nd slot
	pci_assign_irqs(0, 0x13, pin_to_irq(riserPins));

	//Setup USB
	pci_assign_irqs(0, 0x10, pin_to_irq(usbPins));

	//Setup VT8237R Sound
	pci_assign_irqs(0, 0x11, pin_to_irq(vt8237Pins));

	//Setup Ethernet
	pci_assign_irqs(0, 0x12, pin_to_irq(enetPins));

	//Setup VGA
	pci_assign_irqs(1, 0x00, pin_to_irq(vgaPins));

	/* APIC Routing Fixup */

	// Setup SATA
	pdev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT6420_SATA, 0);
	pci_write_config8(pdev, PCI_INTERRUPT_PIN, 0x02);
	pci_assign_irqs(0, 0x0f, pin_to_irq(sataPins));


	// Setup PATA Override
	pdev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_82C586_1, 0);
	pci_write_config8(pdev, PCI_INTERRUPT_PIN, 0x01);
	pci_write_config8(pdev, PCI_INTERRUPT_LINE, 0xFF);

#else
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
#endif
}



/**
 * Set up the power management capabilities directly into ACPI mode.
 * This avoids having to handle any System Management Interrupts (SMIs).
 */

static void setup_pm(struct device *dev)
{
	u16 tmp;
	struct southbridge_via_vt8237r_config *cfg;

	cfg = dev->chip_info;

	/* Debounce LID and PWRBTN# Inputs for 16ms. */
	pci_write_config8(dev, 0x80, 0x20);

	/* Set ACPI base address to I/O VT8237R_ACPI_IO_BASE. */
	pci_write_config16(dev, 0x88, VT8237R_ACPI_IO_BASE | 0x1);

	/* Set ACPI to 9, must set IRQ 9 override to level! Set PSON gating. */
	pci_write_config8(dev, 0x82, 0x40 | VT8237R_ACPI_IRQ);

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	/* Primary interrupt channel, define wake events 0=IRQ0 15=IRQ15 1=en. */
	pci_write_config16(dev, 0x84, 0x3052);
#else
	/* Primary interrupt channel, define wake events 0=IRQ0 15=IRQ15 1=en. */
	pci_write_config16(dev, 0x84, 0x30b2);

#endif
	/* SMI output level to low, 7.5us throttle clock */
	pci_write_config8(dev, 0x8d, 0x18);

	/* GP Timer Control 1s */
	pci_write_config8(dev, 0x93, 0x88);

	/*
	 * 7 = SMBus clock from RTC 32.768KHz
	 * 5 = Internal PLL reset from susp disabled
	 * 2 = GPO2 is SUSA#
	 */
	tmp = 0xa0;
	if (cfg && cfg->enable_gpo3)
		tmp |= 0x10;
	pci_write_config8(dev, 0x94, tmp);

	/*
	 * 7 = stp to sust delay 1msec
	 * 6 = SUSST# Deasserted Before PWRGD for STD
	 * 5 = Keyboard/Mouse Swap
	 * 4 = PWRGOOD reset on VT8237A/S
	 * 3 = GPO26/GPO27 is GPO
	 * 2 = Disable Alert on Lan
	 * 1 = SUSCLK/GPO4
	 * 0 = USB Wakeup
	 */

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	pci_write_config8(dev, 0x95, 0xc2);
#else
	tmp = 0xcc;
	if (cfg) {
		if (cfg->disable_gpo26_gpo27)
			tmp &= ~0x08;
		if (cfg->enable_aol_2_smb_slave)
			tmp &= ~0x04;
	}
	pci_write_config8(dev, 0x95, tmp);
#endif

	/* Disable GP3 timer. */
	pci_write_config8(dev, 0x98, 0);

	/* Enable ACPI access RTC signal gated with PSON. */
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

	/* Disable all global enable SMIs, except SW SMI */
	outw(0x40, VT8237R_ACPI_IO_BASE + 0x2a);

	/* Primary activity SMI disable. */
	outl(0x0, VT8237R_ACPI_IO_BASE + 0x34);

	/* GP timer reload on none. */
	outl(0x0, VT8237R_ACPI_IO_BASE + 0x38);

	/* Disable extended IO traps. */
	outb(0x0, VT8237R_ACPI_IO_BASE + 0x42);

	/* SCI is generated for RTC/pwrBtn/slpBtn. */
	tmp = inw(VT8237R_ACPI_IO_BASE + 0x04);

	/* All SMI on, both IDE buses ON, PSON rising edge. */
	outw(0x1, VT8237R_ACPI_IO_BASE + 0x2c);

	/* clear sleep */
	tmp &= ~(7 << 10);
	tmp |= 1;
	outw(tmp, VT8237R_ACPI_IO_BASE + 0x04);
}

int acpi_get_sleep_type(void)
{
	u16 tmp = inw(VT8237R_ACPI_IO_BASE + 0x04);
	return ((tmp & (7 << 10)) >> 10) == 1 ? 3 : 0;
}

static void vt8237r_init(struct device *dev)
{
	u8 enables;
	struct southbridge_via_vt8237r_config *cfg;

	cfg = dev->chip_info;

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	printk(BIOS_SPEW, "Entering vt8237r_init, for EPIA.\n");
	/*
	 * TODO: Looks like stock BIOS can do this but causes a hang
	 * Enable SATA LED, disable special CPU Frequency Change -
	 * GPIO28 GPIO22 GPIO29 GPIO23 are GPIOs.
	 * Setup to match EPIA default
	 * PCS0# on Pin U1
	 */
	enables = pci_read_config8(dev, 0xe5);
	enables |= 0x23;
	pci_write_config8(dev, 0xe5, enables);

	/*
	 * Enable Flash Write Access.
	 * Note EPIA-N Does not use REQ5 or PCISTP#(Hang)
	 */
	enables = pci_read_config8(dev, 0xe4);
	enables |= 0x2B;
	pci_write_config8(dev, 0xe4, enables);

	/* Enables Extra RTC Ports */
	enables = pci_read_config8(dev, 0x4E);
	enables |= 0x80;
	pci_write_config8(dev, 0x4E, enables);

#else
	printk(BIOS_SPEW, "Entering vt8237r_init.\n");
	/*
	 * Enable SATA LED, disable special CPU Frequency Change -
	 * GPIO28 GPIO22 GPIO29 GPIO23 are GPIOs.
	 */
	pci_write_config8(dev, 0xe5, 0x09);

	enables = 0x4;
	if (cfg) {
		if (cfg->enable_gpo5)
			enables |= 0x01;
		if (cfg->gpio15_12_dir_output)
			enables |= 0x10;
	}
	/* REQ5 as PCI request input - should be together with INTE-INTH. */
	pci_write_config8(dev, 0xe4, enables);
#endif

	/* Set bit 3 of 0x4f (use INIT# as CPU reset). */
	enables = pci_read_config8(dev, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev, 0x4f, enables);

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	/*
	 * Set Read Pass Write Control Enable
	 */
	pci_write_config8(dev, 0x48, 0x0c);
#else

  #if IS_ENABLED(CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800) || \
	IS_ENABLED(CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD)
	/* It seems that when we pair with the K8T800, we need to disable
	 * the A2 mask
	 */
	pci_write_config8(dev, 0x48, 0x0c);
  #else
	/*
	 * Set Read Pass Write Control Enable
	 * (force A2 from APIC FSB to low).
	 */
	pci_write_config8(dev, 0x48, 0x8c);
  #endif

#endif

	southbridge_init_common(dev);

#if !IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	/* FIXME: Intel needs more bit set for C2/C3. */

	/*
	 * Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change.
	 */
	outb(0x1, VT8237R_ACPI_IO_BASE + 0x11);
#endif

	printk(BIOS_SPEW, "Leaving %s.\n", __func__);
	printk(BIOS_SPEW, "And taking a dump:\n");
	dump_south(dev);
}

static void vt8237a_init(struct device *dev)
{
	/*
	 * FIXME: This is based on vt8237s_init() and the values the AMI
	 *        BIOS on my M2V wrote to these registers (by looking
	 *        at lspci -nxxx output).
	 *        Works for me.
	 */
	u32 tmp;

	/* Set bit 3 of 0x4f (use INIT# as CPU reset). */
	tmp = pci_read_config8(dev, 0x4f);
	tmp |= 0x08;
	pci_write_config8(dev, 0x4f, tmp);

	/*
	 * bit2: REQ5 as PCI request input - should be together with INTE-INTH.
	 * bit5: usb power control lines as gpio
	 */
	pci_write_config8(dev, 0xe4, 0x24);
	/*
	 * Enable APIC wakeup from INTH
	 * Enable SATA LED, disable special CPU Frequency Change -
	 * GPIO28 GPIO22 GPIO29 GPIO23 are GPIOs.
	 */
	pci_write_config8(dev, 0xe5, 0x69);

	/* Reduce further the STPCLK/LDTSTP signal to 5us. */
	pci_write_config8(dev, 0xec, 0x4);

	/* Host Bus Power Management Control, maybe not needed */
	pci_write_config8(dev, 0x8c, 0x5);

	/* Enable HPET */
	pci_write_config32(dev, 0x68, (CONFIG_HPET_ADDRESS | 0x80));

	southbridge_init_common(dev);

	/* Share INTE-INTH with INTA-INTD for simplicity */
	pci_write_config8(dev, 0x46, 0x00);

	/* FIXME: Intel needs more bit set for C2/C3. */

	/*
	 * Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change.
	 */
	outb(0x1, VT8237R_ACPI_IO_BASE + 0x11);

	dump_south(dev);
}

static void vt8237s_init(struct device *dev)
{
	u32 tmp;

	/* Put SPI base VT8237S_SPI_MEM_BASE. */
	tmp = pci_read_config32(dev, 0xbc);
	pci_write_config32(dev, 0xbc,
			   (VT8237S_SPI_MEM_BASE >> 8) | (tmp & 0xFF000000));

	/*
	 * REQ5 as PCI request input - should be together with INTE-INTH.
	 */
	pci_write_config8(dev, 0xe4, 0x04);

	/* Reduce further the STPCLK/LDTSTP signal to 5us. */
	pci_write_config8(dev, 0xec, 0x4);

	/* Host Bus Power Management Control, maybe not needed */
	pci_write_config8(dev, 0x8c, 0x5);

	/* Enable HPET, does not work correctly on R. */
	pci_write_config32(dev, 0x68, (CONFIG_HPET_ADDRESS | 0x80));

	southbridge_init_common(dev);

	/* FIXME: Intel needs more bit set for C2/C3. */

	/*
	 * Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change. FIXME FIXME, pre rev A2.
	 */
	outb(0xff, VT8237R_ACPI_IO_BASE + 0x50);

	dump_south(dev);
}

static void vt8237_common_init(struct device *dev)
{
	u8 enables, byte;
	struct southbridge_via_vt8237r_config *cfg;
#if !IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	unsigned char pwr_on;
#endif

	cfg = dev->chip_info;

	/* Enable addr/data stepping. */
	byte = pci_read_config8(dev, PCI_COMMAND);
	byte |= PCI_COMMAND_WAIT;
	pci_write_config8(dev, PCI_COMMAND, byte);

/* EPIA-N(L) Uses CN400 for BIOS Access */
#if !IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
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
#endif

	/*
	 * Set bit 6 of 0x40 (I/O recovery time).
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

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	/* I/O recovery time, default IDE routing */
	pci_write_config8(dev, 0x4c, 0x04);

	/* ROM memory cycles go to LPC. */
	pci_write_config8(dev, 0x59, 0x80);

	/*
	 * Bit | Meaning
	 * -------------
	 *   3 | Bypass APIC De-Assert Message (1=Enable)
	 *   2 | APIC HyperTransport Mode (1=Enable)
	 *   1 | possibly "INTE#, INTF#, INTG#, INTH# as PCI"
	 *     | bit 1=1 works for Aaron at VIA, bit 1=0 works for jakllsch
	 *   0 | Dynamic Clock Gating Main Switch (1=Enable)
	 */
	pci_write_config8(dev, 0x5b, 0x9);

	/* Set 0x58 to 0x42 APIC On and RTC Write Protect.*/
	pci_write_config8(dev, 0x58, 0x42);

	/* Enable serial IRQ, 6PCI clocks. */
	pci_write_config8(dev, 0x52, 0x9);
#else
	/* I/O recovery time, default IDE routing */
	pci_write_config8(dev, 0x4c, 0x44);

	/* ROM memory cycles go to LPC. */
 	pci_write_config8(dev, 0x59, 0x80);

	/*
	 * Bit | Meaning
	 * -------------
	 *   3 | Bypass APIC De-Assert Message (1=Enable)
	 *   2 | APIC HyperTransport Mode (1=Enable)
	 *   1 | possibly "INTE#, INTF#, INTG#, INTH# as PCI"
	 *     | bit 1=1 works for Aaron at VIA, bit 1=0 works for jakllsch
	 *   0 | Dynamic Clock Gating Main Switch (1=Enable)
	 */
	if (cfg && cfg->int_efgh_as_gpio) {
		pci_write_config8(dev, 0x5b, 0x9);
	} else {
		pci_write_config8(dev, 0x5b, 0xb);
	}

	/* configure power state of the board after loss of power */
	if (get_option(&pwr_on, "power_on_after_fail") != CB_SUCCESS)
		pwr_on = 1;
	enables = pci_read_config8(dev, 0x58);
	pci_write_config8(dev, 0x58, enables & ~0x02);
	outb(0x0d, 0x70);
	outb(pwr_on ? 0x00 : 0x80, 0x71);
	pci_write_config8(dev, 0x58, enables);

	/* Set 0x58 to 0x43 APIC and RTC. */
	pci_write_config8(dev, 0x58, 0x43);

	/* Enable serial IRQ, 6PCI clocks. */
	pci_write_config8(dev, 0x52, 0x9);
#endif
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	smm_lock();
#endif

	/* Power management setup */
	setup_pm(dev);

	/* Start the RTC. */
	cmos_init(0);
}

static void vt8237r_read_resources(struct device *dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	/* Fixed ACPI Base IO Base*/
	res = new_resource(dev, 0x88);
	res->base = VT8237R_ACPI_IO_BASE;
	res->size = 128;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Fixed EISA ECLR I/O Regs	*/
	res = new_resource(dev, 3);
	res->base = 0x4d0;
	res->size = 2;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Fixed System Management Bus I/O Resource */
	res = new_resource(dev, 0xD0);
	res->base = VT8237R_SMBUS_IO_BASE;
	res->size = 16;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Fixed APIC resource */
	res = new_resource(dev, 0x44);
	res->base = IO_APIC_ADDR;
	res->size = 256;
	res->limit = 0xffffffffUL;
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Fixed flashrom resource */
	res = new_resource(dev, 4);
	res->base = 0xff000000UL;
	res->size = 0x01000000UL; /* 16MB */
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x1000UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void init_keyboard(struct device *dev)
{
	u8 regval = pci_read_config8(dev, 0x51);
	if (regval & 0x1)
		pc_keyboard_init(NO_AUX_DEVICE);
}

static void southbridge_init_common(struct device *dev)
{
	vt8237_common_init(dev);
	pci_routing_fixup(dev);
	setup_ioapic(VIO_APIC_VADDR, VT8237R_APIC_ID);
	setup_i8259();
	init_keyboard(dev);
}


static void vt8237_set_subsystem(struct device *dev, unsigned vendor,
				 unsigned device)
{
	pci_write_config16(dev, 0x70, vendor);
	pci_write_config16(dev, 0x72, device);
}

static struct pci_operations lops_pci = {
	.set_subsystem = vt8237_set_subsystem,
};

static const struct device_operations vt8237r_lpc_ops_s = {
	.read_resources		= vt8237r_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= vt8237s_init,
	.scan_bus		= scan_lpc_bus,
	.ops_pci		= &lops_pci,
};

static const struct device_operations vt8237r_lpc_ops_r = {
	.read_resources		= vt8237r_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= vt8237r_init,
	.scan_bus		= scan_lpc_bus,
	.ops_pci		= &lops_pci,
};

static const struct device_operations vt8237r_lpc_ops_a = {
	.read_resources		= vt8237r_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= vt8237a_init,
	.scan_bus		= scan_lpc_bus,
	.ops_pci		= &lops_pci,
};

static const struct pci_driver lpc_driver_r __pci_driver = {
	.ops	= &vt8237r_lpc_ops_r,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237R_LPC,
};

static const struct pci_driver lpc_driver_a __pci_driver = {
	.ops	= &vt8237r_lpc_ops_a,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237A_LPC,
};

static const struct pci_driver lpc_driver_s __pci_driver = {
	.ops	= &vt8237r_lpc_ops_s,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237S_LPC,
};
