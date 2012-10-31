/*
 * (C) Copyright 2012 SAMSUNG Electronics
 * Abhilash Kesavan <a.kesavan@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __ASM_ARCH_COMMON_MSHC_H
#define __ASM_ARCH_COMMON_MSHC_H

#include <asm/arch/pinmux.h>
#ifndef __ASSEMBLY__
struct mshci_host {
	struct s5p_mshci	*reg;		/* Mapped address */
	unsigned int		clock;		/* Current clock in MHz */
	enum periph_id	peripheral;
};

struct s5p_mshci {
	unsigned int	ctrl;
	unsigned int	pwren;
	unsigned int	clkdiv;
	unsigned int	clksrc;
	unsigned int	clkena;
	unsigned int	tmout;
	unsigned int	ctype;
	unsigned int	blksiz;
	unsigned int	bytcnt;
	unsigned int	intmask;
	unsigned int	cmdarg;
	unsigned int	cmd;
	unsigned int	resp0;
	unsigned int	resp1;
	unsigned int	resp2;
	unsigned int	resp3;
	unsigned int	mintsts;
	unsigned int	rintsts;
	unsigned int	status;
	unsigned int	fifoth;
	unsigned int	cdetect;
	unsigned int	wrtprt;
	unsigned int	gpio;
	unsigned int	tcbcnt;
	unsigned int	tbbcnt;
	unsigned int	debnce;
	unsigned int	usrid;
	unsigned int	verid;
	unsigned int	hcon;
	unsigned int	uhs_reg;
	unsigned int	rst_n;
	unsigned char	reserved1[4];
	unsigned int	bmod;
	unsigned int	pldmnd;
	unsigned int	dbaddr;
	unsigned int	idsts;
	unsigned int	idinten;
	unsigned int	dscaddr;
	unsigned int	bufaddr;
	unsigned int	clksel;
	unsigned char	reserved2[460];
	unsigned int	cardthrctl;
};

/*
 * Struct idma
 * Holds the descriptor list
 */
struct mshci_idmac {
	u32	des0;
	u32	des1;
	u32	des2;
	u32	des3;
};

/*  Control Register  Register */
#define CTRL_RESET	(0x1 << 0)
#define FIFO_RESET	(0x1 << 1)
#define DMA_RESET	(0x1 << 2)
#define DMA_ENABLE	(0x1 << 5)
#define SEND_AS_CCSD	(0x1 << 10)
#define ENABLE_IDMAC    (0x1 << 25)

/*  Power Enable Register */
#define POWER_ENABLE	(0x1 << 0)

/*  Clock Enable Register */
#define CLK_ENABLE	(0x1 << 0)
#define CLK_DISABLE	(0x0 << 0)

/* Timeout Register */
#define TMOUT_MAX	0xffffffff

/*  Card Type Register */
#define PORT0_CARD_WIDTH1	0
#define PORT0_CARD_WIDTH4	(0x1 << 0)
#define PORT0_CARD_WIDTH8	(0x1 << 16)

/*  Interrupt Mask Register */
#define INTMSK_ALL	0xffffffff
#define INTMSK_RE	(0x1 << 1)
#define INTMSK_CDONE	(0x1 << 2)
#define INTMSK_DTO	(0x1 << 3)
#define INTMSK_DCRC	(0x1 << 7)
#define INTMSK_RTO	(0x1 << 8)
#define INTMSK_DRTO	(0x1 << 9)
#define INTMSK_HTO	(0x1 << 10)
#define INTMSK_FRUN	(0x1 << 11)
#define INTMSK_HLE	(0x1 << 12)
#define INTMSK_SBE	(0x1 << 13)
#define INTMSK_ACD	(0x1 << 14)
#define INTMSK_EBE	(0x1 << 15)

/* Command Register */
#define CMD_RESP_EXP_BIT	(0x1 << 6)
#define CMD_RESP_LENGTH_BIT	(0x1 << 7)
#define CMD_CHECK_CRC_BIT	(0x1 << 8)
#define CMD_DATA_EXP_BIT	(0x1 << 9)
#define CMD_RW_BIT		(0x1 << 10)
#define CMD_SENT_AUTO_STOP_BIT	(0x1 << 12)
#define CMD_WAIT_PRV_DAT_BIT	(0x1 << 13)
#define CMD_SEND_CLK_ONLY	(0x1 << 21)
#define CMD_USE_HOLD_REG	(0x1 << 29)
#define CMD_STRT_BIT		(0x1 << 31)
#define CMD_ONLY_CLK		(CMD_STRT_BIT | CMD_SEND_CLK_ONLY | \
				CMD_WAIT_PRV_DAT_BIT)

/*  Raw Interrupt Register */
#define DATA_ERR	(INTMSK_EBE | INTMSK_SBE | INTMSK_HLE |	\
			INTMSK_FRUN | INTMSK_EBE | INTMSK_DCRC)
#define DATA_TOUT	(INTMSK_HTO | INTMSK_DRTO)

/*  Status Register */
#define DATA_BUSY	(0x1 << 9)

/*  FIFO Threshold Watermark Register */
#define TX_WMARK	(0xFFF << 0)
#define RX_WMARK	(0xFFF << 16)
#define MSIZE_MASK	(0x7 << 28)

/* DW DMA Mutiple Transaction Size */
#define MSIZE_8		(2 << 28)

/*  Bus Mode Register */
#define BMOD_IDMAC_RESET	(0x1 << 0)
#define BMOD_IDMAC_FB		(0x1 << 1)
#define BMOD_IDMAC_ENABLE	(0x1 << 7)

/* IDMAC bits */
#define MSHCI_IDMAC_OWN		(0x1 << 31)
#define MSHCI_IDMAC_CH		(0x1 << 4)
#define MSHCI_IDMAC_FS		(0x1 << 3)
#define MSHCI_IDMAC_LD		(0x1 << 2)

int s5p_mshci_init(const void *blob);

#endif
#endif

