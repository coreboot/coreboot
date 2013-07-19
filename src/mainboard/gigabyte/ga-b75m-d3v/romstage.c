/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <superio/ite/it8728f/early_serial.h>
#include "northbridge/intel/sandybridge/sandybridge.h"
#include "northbridge/intel/sandybridge/raminit.h"
#include "southbridge/intel/bd82x6x/pch.h"
#include "southbridge/intel/bd82x6x/gpio.h"
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include "gpio.h"
#include <cbfs.h>
#include "onboard.h"

/*
 * Mainboard specific superio IT8728F settings
 */
static void it8728f_b75md3v_conf(device_t dev)
{
	it8728f_reg_write(dev, 0x25, 0x40); // gpio pin function -> gp16
	it8728f_reg_write(dev, 0x27, 0x10); // gpio pin function -> gp34
	it8728f_reg_write(dev, 0x2c, 0x80); // smbus isolation on parallel port
	it8728f_reg_write(dev, 0x62, 0x0a); // simple iobase 0xa00
	it8728f_reg_write(dev, 0x73, 0x00); // watchdog timeout clear!
	it8728f_reg_write(dev, 0xcb, 0x00); // simple io set4 direction -> in
	it8728f_reg_write(dev, 0xe9, 0x21); // bus select disable
	it8728f_reg_write(dev, 0xf6, 0x1c); // hardware monitor alert beep -> gp36(pin12)
}

static void it8728f_b75md3v_disable_reboot(device_t dev)
{
	it8728f_reg_write(dev, 0xEF, 0x7E);
}

static void pch_enable_lpc(void)
{
	/*
	 * Enable:
	 *  EC Decode Range PortA30/A20
	 *  SuperIO Port2E/2F
	 *  PS/2 Keyboard/Mouse Port60/64
	 *  FDD Port3F0h-3F5h and Port3F7h
	 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, KBC_LPC_EN | MC_LPC_EN |
			CNF1_LPC_EN | CNF2_LPC_EN | COMA_LPC_EN);
}

static void rcba_config(void)
{
	u32 reg32;

	/*
	 *             GFX         INTA -> PIRQA (MSI)
	 * D20IP_XHCI  XHCI        INTA -> PIRQD (MSI)
	 * D26IP_E2P   EHCI #2     INTA -> PIRQF
	 * D27IP_ZIP   HDA         INTA -> PIRQA (MSI)
	 * D28IP_P2IP  WLAN        INTA -> PIRQD
	 * D28IP_P3IP  Card Reader INTB -> PIRQE
	 * D28IP_P6IP  LAN         INTC -> PIRQB
	 * D29IP_E1P   EHCI #1     INTA -> PIRQD
	 * D31IP_SIP   SATA        INTA -> PIRQB (MSI)
	 * D31IP_SMIP  SMBUS       INTB -> PIRQH
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (NOINT << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (NOINT << D28IP_P1IP) | (INTA << D28IP_P2IP) |
		(INTB << D28IP_P3IP) | (NOINT << D28IP_P4IP) |
		(NOINT << D28IP_P5IP) | (INTC << D28IP_P6IP) |
		(NOINT << D28IP_P7IP) | (NOINT << D28IP_P8IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);
	RCBA32(D20IP) = (INTA << D20IP_XHCIIP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQB, PIRQH, PIRQA, PIRQC);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQD, PIRQE, PIRQB, PIRQC);
	DIR_ROUTE(D27IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D26IR, PIRQF, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D20IR, PIRQD, PIRQE, PIRQF, PIRQG);

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);

	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	reg32 |= PCH_DISABLE_ALWAYS;

//	/* Disable PCI bridge so MRC does not probe this bus */
//	reg32 |= PCH_DISABLE_P2P;

	RCBA32(FD) = reg32;
}

// FIXME, this function is generic code that should go to sb/... or
// nb/../early_init.c
static void early_pch_init(void)
{

}

static void early_ec_init(void)
{

}

#define SUPERIO_BASE 0x2e
#define SUPERIO_DEV PNP_DEV(SUPERIO_BASE, 0)
#define SUPERIO_GPIO PNP_DEV(SUPERIO_BASE, IT8728F_GPIO)
#define SERIAL_DEV PNP_DEV(SUPERIO_BASE, 0x01)
#define B75_GPIOBASE 0x0500
#define B75_PMBASE 0x0400
#define B75_RCBABASE 0xfed1c000

void main(unsigned long bist)
{
	int boot_mode = 0;
////	int cbmem_was_initted;
	u32 pm1_cnt;
	u16 pm1_sts;

#if CONFIG_COLLECT_TIMESTAMPS
	tsc_t start_romstage_time;
	tsc_t before_dram_time;
	tsc_t after_dram_time;
	tsc_t base_time = {
		.lo = pci_read_config32(PCI_DEV(0, 0x00, 0), 0xdc),
		.hi = pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0)
	};
#endif
	struct pei_data pei_data = {
		pei_version: PEI_VERSION,
		mchbar: DEFAULT_MCHBAR,
		dmibar: DEFAULT_DMIBAR,
		epbar: DEFAULT_EPBAR,
		pciexbar: CONFIG_MMCONF_BASE_ADDRESS,
		smbusbar: SMBUS_IO_BASE,
		wdbbar: 0x4000000,
		wdbsize: 0x1000,
		hpet_address: CONFIG_HPET_ADDRESS,
		rcba: B75_RCBABASE,
		pmbase: B75_PMBASE,
		gpiobase: B75_GPIOBASE,
		thermalbase: 0xfed08000,
		system_type: 1, // 0 Mobile, 1 Desktop/Server
		tseg_size: CONFIG_SMM_TSEG_SIZE,
		spd_addresses: { 0xA0, 0x00,0xA4,0x00 },
		ts_addresses: { 0x00, 0x00, 0x00, 0x00 },
		ec_present: 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		dimm_channel0_disabled: 2,
		dimm_channel1_disabled: 2,
		max_ddr3_freq: 1600,
		usb_port_config: {
			/* enabled   usb oc pin    length */
			{ 1, 0, 0x0040 }, /* P0: USB 3.0 1  (OC0) */
			{ 1, 0, 0x0040 }, /* P1: USB 3.0 2  (OC0) */
			{ 0, 1, 0x0000 }, /* P2: Empty */
			{ 1, 1, 0x0040 }, /* P3: Camera (no OC) */
			{ 1, 1, 0x0040 }, /* P4: WLAN   (no OC) */
			{ 1, 1, 0x0040 }, /* P5: WWAN   (no OC) */
			{ 0, 1, 0x0000 }, /* P6: Empty */
			{ 0, 1, 0x0000 }, /* P7: Empty */
			{ 0, 5, 0x0000 }, /* P8: Empty */
			{ 1, 4, 0x0040 }, /* P9: USB 2.0 (AUO4) (OC4) */
			{ 0, 5, 0x0000 }, /* P10: Empty */
			{ 0, 5, 0x0000 }, /* P11: Empty */
			{ 0, 5, 0x0000 }, /* P12: Empty */
			{ 1, 5, 0x0040 }, /* P13: Bluetooth (no OC) */
		},
		usb3: {
			mode: XHCI_MODE,
			hs_port_switch_mask: XHCI_PORTS,
			preboot_support: XHCI_PREBOOT,
			xhci_streams: XHCI_STREAMS,
		},
	};

#if CONFIG_COLLECT_TIMESTAMPS
	start_romstage_time = rdtsc();
#endif

	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Initialize SuperIO */
	it8728f_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	it8728f_b75md3v_disable_reboot(SUPERIO_GPIO);

	/* Initialize console device(s) */
	console_init();
	printk(BIOS_EMERG, "START:\n");
	
	/* Enable GPIOs */
	//pci_write_config32(PCH_LPC_DEV, GPIO_BASE, B75_GPIOBASE|1);
	//pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
	//setup_pch_gpios(&b75md3v_gpio_map);
	
	//printk(BIOS_EMERG, "Setup GPIO success\n");
	
	/* Finish setting up SuperIO */
	it8728f_b75md3v_conf(PNP_DEV(SUPERIO_BASE, IT8728F_GPIO));
	
	printk(BIOS_EMERG, "Fixed SuperIO\n");

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	printk(BIOS_EMERG, "BIST was ok\n");
	
	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_EMERG, "soft reset detected\n");
		boot_mode = 1;

		/* System is not happy after keyboard reset... */
		printk(BIOS_EMERG, "Issuing CF9 warm reset\n");
		outb(0x6, 0xcf9);
		hlt();
	}
	
	printk(BIOS_EMERG, "CAFE passed\n");
	
	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	sandybridge_early_initialization(SANDYBRIDGE_DESKTOP);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = inw(B75_PMBASE + PM1_STS);

	/* Read PM1_CNT[12:10] to determine which Sx state */
	pm1_cnt = inl(B75_PMBASE + PM1_CNT);

	if ((pm1_sts & WAK_STS) && ((pm1_cnt >> 10) & 7) == 5) {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		boot_mode = 2;
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		outl(pm1_cnt & ~(7 << 10), B75_PMBASE + PM1_CNT);
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}

	/* Do ec reset as early as possible, but skip it on S3 resume */
	if (boot_mode < 2)
		early_ec_init();

	post_code(0x38);
	/* Enable SPD ROMs and DDR-III DRAM */
	enable_smbus();

	/* Prepare USB controller early in S3 resume */
	if (boot_mode == 2)
		enable_usb_bar();

	post_code(0x39);

	post_code(0x3a);
	pei_data.boot_mode = boot_mode;
#if CONFIG_COLLECT_TIMESTAMPS
	before_dram_time = rdtsc();
#endif
	sdram_initialize(&pei_data);

#if CONFIG_COLLECT_TIMESTAMPS
	after_dram_time = rdtsc();
#endif
	post_code(0x3b);
	/* Perform some initialization that must run before stage2 */
	early_pch_init();
	post_code(0x3c);

	rcba_config();
	post_code(0x3d);

	quick_ram_check();
	post_code(0x3e);

	MCHBAR16(SSKPD) = 0xCAFE;
#if CONFIG_EARLY_CBMEM_INIT
	cbmem_was_initted = !cbmem_initialize();
#else
////	cbmem_was_initted = cbmem_reinit((uint64_t) (get_top_of_ram()
////						     - HIGH_MEMORY_SIZE));
#endif

#if CONFIG_HAVE_ACPI_RESUME
	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */

	*(u32 *)CBMEM_BOOT_MODE = 0;
	*(u32 *)CBMEM_RESUME_BACKUP = 0;

	if ((boot_mode == 2) && cbmem_was_initted) {
		void *resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
		if (resume_backup_memory) {
			*(u32 *)CBMEM_BOOT_MODE = boot_mode;
			*(u32 *)CBMEM_RESUME_BACKUP = (u32)resume_backup_memory;
		}
		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafed00d);
	} else if (boot_mode == 2) {
		/* Failed S3 resume, reset to come up cleanly */
		outb(0x6, 0xcf9);
		hlt();
	} else {
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafebabe);
	}
#endif
	post_code(0x3f);
#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(base_time);
	timestamp_add(TS_START_ROMSTAGE, start_romstage_time );
	timestamp_add(TS_BEFORE_INITRAM, before_dram_time );
	timestamp_add(TS_AFTER_INITRAM, after_dram_time );
	timestamp_add_now(TS_END_ROMSTAGE);
#endif
#if CONFIG_CONSOLE_CBMEM
	/* Keep this the last thing this function does. */
	cbmemc_reinit();
#endif
}
