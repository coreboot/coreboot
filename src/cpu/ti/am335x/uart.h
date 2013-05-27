/*
 * (C) Copyright 2012 The ChromiumOS Authors
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Heungjun Kim <riverful.kim@samsung.com>
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
 *
 * This file is based off of arch/arm/include/asm/arch-exynos5/uart.h
 * from u-boot.
 */

#ifndef AM335X_UART_H
#define AM335X_UART_H

#define AM335X_UART0_BASE	0x44e09000
#define AM335X_UART1_BASE	0x48020000
#define AM335X_UART2_BASE	0x48024000
#define AM335X_UART3_BASE	0x481A6000
#define AM335X_UART4_BASE	0x481A8000
#define AM335X_UART5_BASE	0x481AA000

struct am335x_uart {
	union {
		uint32_t rhr;		/* receiver holding (read) */
		uint32_t thr;		/* transmit holding (write) */
	};
	uint32_t ier;			/* interrupt enable */
	union {
		uint32_t iir;		/* interrupt ID (read) */
		uint32_t fcr;		/* FIFO control (write) */
	};
	uint32_t lcr;			/* line control */

	/* 0x10 */
	uint32_t mcr;			/* modem control */
	uint32_t lsr;			/* line status, read-only */

	/*
	 * Bytes 0x18 and 0x1c are weird. When EFR[4] = 1 and MCR[6] = 1,
	 * transmission control register and trigger level register
	 * will be read/written. If not, the modem status register
	 * and scratchpad register will be affected by read/write.
	 */
	union {
		uint32_t msr;		/* modem status */
		uint32_t tcr;		/* transmission control */
	};
	union {
		uint32_t spr;		/* scratchpad */
		uint32_t tlr;		/* trigger level */
	};

	/* 0x20 */
	uint32_t mdr1;			/* mode definition 1 */
	uint32_t mdr2;			/* mode definition 2 */
	union {
		uint32_t sflsr;		/* status FIFO line status reg (read) */
		uint32_t txfll;		/* transmit frame length low (write) */
	};
	union {
		uint32_t resume;	/* resume halted operation (read) */
		uint32_t txflh;		/* transmit frame length high (write) */
	};

	/* 0x30 */
	union {
		uint32_t sfregl;	/* status FIFO low (read) */
		uint32_t rxfll;		/* received frame length low (write) */
	};
	union {
		uint32_t sfregh;	/* status FIFO high (read) */
		uint32_t rxflh;		/* received frame length high (write) */
	};
	uint32_t blr;			/* BOF control */
	uint32_t acreg;			/* auxilliary control */

	/* 0x40 */
	uint32_t scr;			/* supplementary control */
	uint32_t ssr;			/* supplementary status */
	uint32_t eblr;			/* BOF length  */

	/* 0x50 */
	uint32_t mvr;			/* module version (read-only) */
	uint32_t sysc;			/* system config */
	uint32_t syss;			/* system status (read-only) */
	uint32_t wer;			/* wake-up enable */

	/* 0x60 */
	uint32_t cfps;			/* carrier prescale frequency */
	uint32_t rxfifo_lvl;		/* received FIFO level */
	uint32_t txfifo_lvl;		/* transmit FIFO level */
	uint32_t ier2;

	/* 0x70 */
	uint32_t isr2;
	uint32_t freq_sel;		/* frequency select */
	uint32_t rsvd1;			/* reserved */
	uint32_t rsvd2;			/* reserved */

	/* 0x80 */
	uint32_t mdr3;			/* mode definition register 3 */
	uint32_t txdma;

} __attribute__((packed));

#endif	/* AM335X_UART_H */
