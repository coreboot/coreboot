/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <cbmem.h>
#include <lib.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <northbridge/intel/gm45/gm45.h>
#include <superio/smsc/lpc47n227/early_serial.c>

#define LPC_DEV PCI_DEV(0, 0x1f, 0)
#define SERIAL_DEV PNP_DEV(0x2e, LPC47N227_SP1)

static void default_southbridge_gpio_setup(void)
{
	/* Enable GPIOs [31:0]. */
	outl(0x197e7dfe, DEFAULT_GPIOBASE + 0x00);
	/* Set input/output mode [31:0] (0 == out, 1 == in). */
	outl(0xe0ea43fe, DEFAULT_GPIOBASE + 0x04);
	/* Set gpio levels [31:0]. orig: 0x01140800 (~SATA0, ~SATA1, GSM, BT,
						     WLAN, ~ANTMUX, ~GPIO12,
						     ~SUSPWR, SMBALERT) */
	outl(0x00000800, DEFAULT_GPIOBASE + 0x0c);

	/* Disable blink [31:0]. */
	outl(0x00000000, DEFAULT_GPIOBASE + 0x18);
	/* Set input inversion [31:0]. */
	outl(0x00000182, DEFAULT_GPIOBASE + 0x2c);

	/* Enable GPIOs [60:32]. */
	outl(0x130300fe, DEFAULT_GPIOBASE + 0x30);
	/* Set input/output mode [60:32] (0 == out, 1 == in). */
	outl(0x0e55ffb0, DEFAULT_GPIOBASE + 0x34);
	/* Set gpio levels [60:32]. orig: 0x10020046 (LNKALERT, ~ATAIO,
						      DMITERM, TXT, ~CLKSATA,
						      GPS, AUDIO)  */
	outl(0x10020042, DEFAULT_GPIOBASE + 0x38);
}

static void early_lpc_setup(void)
{
	/* Set up SuperIO LPC forwards */

	/* Configure serial IRQs.*/
	pci_write_config8(LPC_DEV, D31F0_SERIRQ_CNTL, 0xd0);
	/* Map COMa on 0x3f8, COMb on 0x2f8. */
	pci_write_config16(LPC_DEV, D31F0_LPC_IODEC, 0x0010);
	/* Enable COMa, COMb, Kbd, SuperIO at 0x2e, MCs at 0x4e and 0x62/66. */
	pci_write_config16(LPC_DEV, D31F0_LPC_EN, 0x3c03);
}

static void default_superio_gpio_setup(void)
{
	/* Original settings:
	   idx 30 31 32 33 34 35 36 37  38 39
	   val 60 00 00 40 00 ff 00 e0  00 80
	   def 00 00 00 00 00 00 00 00  00 80

	   Values:
	   GP1 GP2 GP3 GP4
	    fd  17  88  14
	*/
	const device_t sio = PNP_DEV(0x2e, 0);

	/* Enter super-io's configuration state. */
	pnp_enter_conf_state(sio);

	/* Set lpc47n227's runtime register block's base address. */
	pnp_write_config(sio, 0x30, 0x600 >> 4);

	/* Set GP23 to alternate function. */
	pnp_write_config(sio, 0x33, 0x40);

	/* Set GP30 - GP37 to output mode: COM control */
	pnp_write_config(sio, 0x35, 0xff);

	/* Set GP45 - GP47 to output mode. */
	pnp_write_config(sio, 0x37, 0xe0);

	/* Set nIO_PME to open drain. */
	pnp_write_config(sio, 0x39, 0x80);

	/* Exit configuration state. */
	pnp_exit_conf_state(sio);


	/* Enable decoding of 0x600-0x60f through lpc. */
	pci_write_config32(LPC_DEV, D31F0_GEN1_DEC, 0x000c0601);

	/* Set GPIO output values: */
	outb(0x88, 0x600 + 0xb + 3); /* GP30 - GP37 */
	outb(0x10, 0x600 + 0xb + 4); /* GP40 - GP47 */
}

void main(unsigned long bist)
{
	sysinfo_t sysinfo;
	int s3resume = 0;
	u16 reg16;

	/* basic northbridge setup, including MMCONF BAR */
	gm45_early_init();

	if (bist == 0)
		enable_lapic();

	/* First, run everything needed for console output. */
	i82801ix_early_init();
	early_lpc_setup();
	default_superio_gpio_setup();
	lpc47n227_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	printk(BIOS_DEBUG, "running main(bist = %lu)\n", bist);

	reg16 = pci_read_config16(LPC_DEV, D31F0_GEN_PMCON_3);
	pci_write_config16(LPC_DEV, D31F0_GEN_PMCON_3, reg16);
	if ((MCHBAR16(SSKPD_MCHBAR) == 0xCAFE) && !(reg16 & (1 << 9))) {
		printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
		gm45_early_reset();
	}

	default_southbridge_gpio_setup();

	/* ASPM related setting, set early by original BIOS. */
	DMIBAR16(0x204) &= ~(3 << 10);

	/* Check for S3 resume. */
	const u32 pm1_cnt = inl(DEFAULT_PMBASE + 0x04);
	if (((pm1_cnt >> 10) & 7) == 5) {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		s3resume = 1;
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		outl(pm1_cnt & ~(7 << 10), DEFAULT_PMBASE + 0x04);
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}

	/* RAM initialization */
	enter_raminit_or_reset();
	get_gmch_info(&sysinfo);
	raminit(&sysinfo, s3resume);

	raminit_thermal(&sysinfo);
	init_igd(&sysinfo, 0, 1); /* Enable IGD, disable PEG. */
	init_pm(&sysinfo);

	i82801ix_dmi_setup();
	gm45_late_init(sysinfo.stepping);
	i82801ix_dmi_poll_vc1();

	MCHBAR16(SSKPD_MCHBAR) = 0xCAFE;

	init_iommu();

#if CONFIG_HAVE_ACPI_RESUME
	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */
	if (s3resume && cbmem_reinit() {
		void *resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);

		/* copy 1MB - 64K to high tables ram_base to prevent memory corruption
		 * through stage 2. We could keep stuff like stack and heap in high tables
		 * memory completely, but that's a wonderful clean up task for another
		 * day.
		 */
		if (resume_backup_memory)
			memcpy(resume_backup_memory, (void *)CONFIG_RAMBASE, HIGH_MEMORY_SAVE);

		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0, 0), D0F0_SKPD, SKPAD_ACPI_S3_MAGIC);
	}
#endif
	printk(BIOS_SPEW, "exit main()\n");
}

