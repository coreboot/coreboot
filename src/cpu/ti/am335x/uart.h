/*
 * Copyright 2013 Google Inc.
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

#ifndef AM335X_UART_H
#define AM335X_UART_H

#define AM335X_UART0_BASE	0x44e09000
#define AM335X_UART1_BASE	0x48020000
#define AM335X_UART2_BASE	0x48024000
#define AM335X_UART3_BASE	0x481A6000
#define AM335X_UART4_BASE	0x481A8000
#define AM335X_UART5_BASE	0x481AA000

/*
 * The meaning of some AM335x UART register offsets changes depending on read
 * or write operation as well as various modes. See section 19.3.7.1.2 for
 * register access submode description and 19.5.1 for register descriptions.
 */
struct am335x_uart {
	union {
		/* operational mode */
		uint16_t rhr;		/* receiver holding (read) */
		uint16_t thr;		/* transmit holding (write) */
		/* config mode A and B */
		uint16_t dll;		/* divisor latches low */
	};
	uint8_t rsvd_0x02[2];
	union {
		/* operational mode */
		uint16_t ier;		/* interrupt enable */
		/* config mode A and B */
		uint16_t dlh;		/* divisor latches high */
	};
	uint8_t rsvd_0x06[2];
	union {
		/* operational mode, config mode A */
		uint16_t iir;		/* interrupt ID (read) */
		uint16_t fcr;		/* FIFO control (write) */
		/* config mode B */
		uint16_t efr;
	};
	uint8_t rsvd_0x0a[2];
	uint16_t lcr;			/* line control */
	uint8_t rsvd_0x0e[2];

	/* 0x10 */
	union {
		/* operational mode, config mode A */
		uint16_t mcr;		/* modem control */
		/* config mode B */
		uint16_t xon1;		/* XON1 character (UART mode) */
		uint16_t addr1;		/* address 1 (IrDA mode)  */
	};
	uint8_t rsvd_0x12[2];
	union {
		/* operational mode, config mode A */
		uint16_t lsr;		/* line status, read-only */
		/* config mode B */
		uint16_t xon2;		/* XON2 character (UART mode) */
		uint16_t addr2;		/* IrDA mode (IrDA mode) */
	};
	uint8_t rsvd_0x16[2];

	/*
	 * Bytes 0x18 and 0x1c depend on submode TCR_TLR. When EFR[4] = 1 and
	 * MCR[6] = 1, transmission control register and trigger level register
	 * will be read/written. If not, the modem status register and the
	 * scratchpad register will be affected by read/write.
	 */
	union {
		/* operational mode and config mode A */
		uint16_t msr;		/* modem status */
		/* config mode B */
		uint16_t xoff1;		/* xoff1 character (UART MODE) */
		/* submode TCR_TLR */
		uint16_t tcr;		/* transmission control */
	};
	uint8_t rsvd_0x1a[2];
	union {
		uint16_t spr;		/* scratchpad */
		/* config mode B */
		uint16_t xoff2;		/* xoff2 character (UART mode) */
		/* submode TCR_TLR */
		uint16_t tlr;		/* trigger level */
	};
	uint8_t rsvd_0x1e[2];

	/* 0x20 */
	uint16_t mdr1;			/* mode definition 1 */
	uint8_t rsvd_0x22[2];
	uint16_t mdr2;			/* mode definition 2 */
	uint8_t rsvd_0x26[2];
	union {
		uint16_t sflsr;		/* status FIFO line status reg (read) */
		uint16_t txfll;		/* transmit frame length low (write) */
	};
	uint8_t rsvd_0x2a[2];
	union {
		uint16_t resume;	/* resume halted operation (read) */
		uint16_t txflh;		/* transmit frame length high (write) */
	};
	uint8_t rsvd_0x2e[2];

	/* 0x30 */
	union {
		uint16_t sfregl;	/* status FIFO low (read) */
		uint16_t rxfll;		/* received frame length low (write) */
	};
	uint8_t rsvd_0x32[2];
	union {
		uint16_t sfregh;	/* status FIFO high (read) */
		uint16_t rxflh;		/* received frame length high (write) */
	};
	uint8_t rsvd_0x36[2];
	uint16_t blr;			/* BOF control */
	uint8_t rsvd_0x3a[2];
	uint16_t acreg;			/* auxiliary control */
	uint8_t rsvd_0x3e[2];

	/* 0x40 */
	uint16_t scr;			/* supplementary control */
	uint8_t rsvd_0x42[2];
	uint16_t ssr;			/* supplementary status */
	uint8_t rsvd_0x46[2];

	uint16_t eblr;			/* BOF length (operational mode only) */
	uint8_t rsvd_0x4a[6];

	/* 0x50 */
	uint16_t mvr;			/* module version (read-only) */
	uint8_t rsvd_0x52[2];
	uint16_t sysc;			/* system config */
	uint8_t rsvd_0x56[2];
	uint16_t syss;			/* system status (read-only) */
	uint8_t rsvd_0x5a[2];
	uint16_t wer;			/* wake-up enable */
	uint8_t rsvd_0x5e[2];

	/* 0x60 */
	uint16_t cfps;			/* carrier prescale frequency */
	uint8_t rsvd_0x62[2];
	uint16_t rxfifo_lvl;		/* received FIFO level */
	uint8_t rsvd_0x66[2];
	uint16_t txfifo_lvl;		/* transmit FIFO level */
	uint8_t rsvd_0x6a[2];
	uint16_t ier2;			/* RX/TX FIFO empty interrupt enable */
	uint8_t rsvd_0x6e[2];

	/* 0x70 */
	uint16_t isr2;			/* RX/TX FIFO empty interrupt status */
	uint8_t rsvd_0x72[2];
	uint16_t freq_sel;		/* frequency select */
	uint8_t rsvd_0x76[10];

	/* 0x80 */
	uint16_t mdr3;			/* mode definition register 3 */
	uint8_t rsvd_0x82[2];
	uint16_t txdma;			/* TX DMA threshold */

} __attribute__((packed));

#endif	/* AM335X_UART_H */
