/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2009 Thomas Jourdan <thomas.jourdan@gmail.com>
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

#include <delay.h>
#include <stdint.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/speedstep.h>
#include "southbridge/intel/i3100/early_smbus.c"
#include "southbridge/intel/i3100/early_lpc.c"
#include "southbridge/intel/i3100/reset.c"
#include "superio/intel/i3100/early_serial.c"
#include "superio/smsc/smscsuperio/early_serial.c"
#include "northbridge/intel/i3100/i3100.h"
#include "southbridge/intel/i3100/i3100.h"

#define DEVPRES_CONFIG  (DEVPRES_D1F0 | DEVPRES_D2F0 | DEVPRES_D3F0)
#define DEVPRES1_CONFIG (DEVPRES1_D0F1 | DEVPRES1_D8F0)

#define RCBA_RPC   0x0224 /* 32 bit */

#define RCBA_TCTL  0x3000 /*  8 bit */

#define RCBA_D31IP 0x3100 /* 32 bit */
#define RCBA_D30IP 0x3104 /* 32 bit */
#define RCBA_D29IP 0x3108 /* 32 bit */
#define RCBA_D28IP 0x310C /* 32 bit */
#define RCBA_D31IR 0x3140 /* 16 bit */
#define RCBA_D30IR 0x3142 /* 16 bit */
#define RCBA_D29IR 0x3144 /* 16 bit */
#define RCBA_D28IR 0x3146 /* 16 bit */

#define RCBA_RTC   0x3400 /* 32 bit */
#define RCBA_HPTC  0x3404 /* 32 bit */
#define RCBA_GCS   0x3410 /* 32 bit */
#define RCBA_BUC   0x3414 /*  8 bit */
#define RCBA_FD    0x3418 /* 32 bit */
#define RCBA_PRC   0x341C /* 32 bit */

static inline int spd_read_byte(u16 device, u8 address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/i3100/raminit.h"
#include "northbridge/intel/i3100/memory_initialized.c"
#include "northbridge/intel/i3100/raminit.c"
#include "lib/generic_sdram.c"
#include "northbridge/intel/i3100/reset_test.c"
#include "debug.c"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x4e, I3100_SP1)

static void early_config(void)
{
	u32 gcs, rpc, fd;

	/* Enable RCBA */
	pci_write_config32(PCI_DEV(0, 0x1F, 0), RCBA, DEFAULT_RCBA | 1);

	/* Disable watchdog */
	gcs = read32(DEFAULT_RCBA + RCBA_GCS);
	gcs |= (1 << 5); /* No reset */
	write32(DEFAULT_RCBA + RCBA_GCS, gcs);

	/* Configure PCIe port B as 4x */
	rpc = read32(DEFAULT_RCBA + RCBA_RPC);
	rpc |= (3 << 0);
	write32(DEFAULT_RCBA + RCBA_RPC, rpc);

	/* Disable Modem, Audio, PCIe ports 2/3/4 */
	fd = read32(DEFAULT_RCBA + RCBA_FD);
	fd |= (1 << 19) | (1 << 18) | (1 << 17) | (1 << 6) | (1 << 5);
	write32(DEFAULT_RCBA + RCBA_FD, fd);

	/* Enable HPET */
	write32(DEFAULT_RCBA + RCBA_HPTC, (1 << 7));

	/* Improve interrupt routing
	 * D31:F2 SATA        INTB# -> PIRQD
	 * D31:F3 SMBUS       INTB# -> PIRQD
	 * D31:F4 CHAP        INTD# -> PIRQA
	 * D29:F0 USB1#1      INTA# -> PIRQH
	 * D29:F1 USB1#2      INTB# -> PIRQD
	 * D29:F7 USB2        INTA# -> PIRQH
	 * D28:F0 PCIe Port 1 INTA# -> PIRQE
	 */

	write16(DEFAULT_RCBA + RCBA_D31IR, 0x0230);
	write16(DEFAULT_RCBA + RCBA_D30IR, 0x3210);
	write16(DEFAULT_RCBA + RCBA_D29IR, 0x3237);
	write16(DEFAULT_RCBA + RCBA_D28IR, 0x3214);

	/* Setup sata mode */
	pci_write_config8(PCI_DEV(0, 0x1F, 2), SATA_MAP, (SATA_MODE_AHCI << 6) | (0 << 0));
}

void main(unsigned long bist)
{
	/* int boot_mode = 0; */

	static const struct mem_controller mch[] = {
		{
			.node_id = 0,
			.f0 = PCI_DEV(0, 0x00, 0),
			.f1 = PCI_DEV(0, 0x00, 1),
			.f2 = PCI_DEV(0, 0x00, 2),
			.f3 = PCI_DEV(0, 0x00, 3),
			.channel0 = { DIMM3, DIMM2, DIMM1, DIMM0 },
			.channel1 = { DIMM7, DIMM6, DIMM5, DIMM4 },
		}
	};

	if (bist == 0)
		enable_lapic();

	/* Setup the console */
	i3100_enable_superio();
	i3100_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	i3100_configure_uart_clk(SERIAL_DEV, I3100_UART_CLK_PREDIVIDE_26);

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Perform early board specific init */
	early_config();

	/* Prevent the TCO timer from rebooting us */
	i3100_halt_tco_timer();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

	/* Enable SpeedStep and automatic thermal throttling */
	{
		msr_t msr;
		u16 perf;

		msr = rdmsr(IA32_MISC_ENABLES);
		msr.lo |= (1 << 3) | (1 << 16);
		wrmsr(IA32_MISC_ENABLES, msr);

		/* Set CPU frequency/voltage to maximum */

		/* Read performance status register and keep
		 * bits 47:32, where BUS_RATIO_MAX and VID_MAX
		 * are encoded
		 */
		msr = rdmsr(IA32_PERF_STS);
		perf = msr.hi & 0x0000ffff;

		/* Write VID_MAX & BUS_RATIO_MAX to
		 * performance control register
		 */
		msr = rdmsr(IA32_PERF_CTL);
		msr.lo &= 0xffff0000;
		msr.lo |= perf;
		wrmsr(IA32_PERF_CTL, msr);
	}

	/* Initialize memory */
	sdram_initialize(ARRAY_SIZE(mch), mch);
}
