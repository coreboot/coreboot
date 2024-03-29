/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <device/mmio.h>
#include <console/console.h>
#include <console/uart.h>
#include <soc/addressmap.h>
#include <soc/otp.h>

/*
 * This is a driver for the eMemory EG004K32TQ028XW01 NeoFuse
 * One-Time-Programmable (OTP) memory used within the SiFive FU740.
 * It is documented in the FU740 manual here:
 * https://www.sifive.com/documentation/chips/freedom-u740-c000-manual/
 */

struct sifive_otp_registers {
	u32 pa;     /* Address input */
	u32 paio;   /* Program address input */
	u32 pas;    /* Program redundancy cell selection input */
	u32 pce;    /* OTP Macro enable input */
	u32 pclk;   /* Clock input */
	u32 pdin;   /* Write data input */
	u32 pdout;  /* Read data output */
	u32 pdstb;  /* Deep standby mode enable input (active low) */
	u32 pprog;  /* Program mode enable input */
	u32 ptc;    /* Test column enable input */
	u32 ptm;    /* Test mode enable input */
	u32 ptm_rep;/* Repair function test mode enable input */
	u32 ptr;    /* Test row enable input */
	u32 ptrim;  /* Repair function enable input */
	u32 pwe;    /* Write enable input (defines program cycle) */
} __packed;

/*
 * Read a 32 bit value addressed by its index from the OTP.
 * The FU740 stores 4096x32 bit (16KiB) values.
 * Index 0x00-0xff are reserved for SiFive internal use. (first 1KiB)
 */

u32 otp_read_word(u16 idx)
{
	u32 w;

	if (idx >= 0x1000)
		die("otp: idx out of bounds");

	struct sifive_otp_registers *regs = (void *)(FU740_OTP);

	// wake up from stand-by
	write32(&regs->pdstb, 0x01);

	// enable repair function
	write32(&regs->ptrim, 0x01);

	// enable input
	write32(&regs->pce, 0x01);

	// address to read
	write32(&regs->pa, idx);

	// cycle clock to read
	write32(&regs->pclk, 0x01);
	mdelay(1);

	write32(&regs->pclk, 0x00);
	mdelay(1);

	w = read32(&regs->pdout);

	// shut down
	write32(&regs->pce, 0x00);
	write32(&regs->ptrim, 0x00);
	write32(&regs->pdstb, 0x00);

	return w;
}

u32 otp_read_serial(void)
{
	u32 serial = 0;
	u32 serial_n = 0;
	for (int i = 0xfe; i > 0; i -= 2) {
		serial = otp_read_word(i);
		serial_n = otp_read_word(i+1);
		if (serial == ~serial_n)
			break;
	}
	return serial;
}
