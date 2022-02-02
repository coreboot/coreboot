/*
ETHERBOOT -  BOOTP/TFTP Bootstrap Program

Author: Martin Renters
  Date: May/94

 This code is based heavily on David Greenman's if_ed.c driver

 Copyright (C) 1993-1994, David Greenman, Martin Renters.
  This software may be used, modified, copied, distributed, and sold, in
  both source and binary form provided that the above copyright and these
  terms are retained. Under no circumstances are the authors responsible for
  the proper functioning of this software, nor do the authors assume any
  responsibility for damages incurred with its use.

Multicast support added by Timothy Legge (timlegge@users.sourceforge.net) 09/28/2003
Relocation support added by Ken Yap (ken_yap@users.sourceforge.net) 28/12/02
3c503 support added by Bill Paul (wpaul@ctr.columbia.edu) on 11/15/94
SMC8416 support added by Bill Paul (wpaul@ctr.columbia.edu) on 12/25/94
3c503 PIO support added by Jim Hague (jim.hague@acm.org) on 2/17/98
RX overrun by Klaus Espenlaub (espenlaub@informatik.uni-ulm.de) on 3/10/99
  parts taken from the Linux 8390 driver (by Donald Becker and Paul Gortmaker)
SMC8416 PIO support added by Andrew Bettison (andrewb@zip.com.au) on 4/3/02
  based on the Linux 8390 driver (by Donald Becker and Paul Gortmaker)

(C) Rudolf Marek <r.marek@assembler.cz> Simplify for RTL8029, Add coreboot glue logic

*/

#include <arch/io.h>
#include <console/ne2k.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <ip_checksum.h>

#include "ns8390.h"

#define ETH_ALEN		6	/* Size of Ethernet address */
#define ETH_HLEN		14	/* Size of ethernet header */
#define	ETH_ZLEN		60	/* Minimum packet */
#define	ETH_FRAME_LEN		1514	/* Maximum packet */
#define ETH_DATA_ALIGN		2	/* Amount needed to align the data after an ethernet header */
#define	ETH_MAX_MTU		(ETH_FRAME_LEN-ETH_HLEN)

#define MEM_SIZE MEM_32768
#define TX_START 64
#define RX_START (64 + D8390_TXBUF_SIZE)

static unsigned int get_count(unsigned int eth_nic_base)
{
	unsigned int ret;
	outb(D8390_COMMAND_RD2 + D8390_COMMAND_PS1,
	     eth_nic_base + D8390_P0_COMMAND);

	ret = inb(eth_nic_base + 8 + 0) | (inb(eth_nic_base + 8 + 1) << 8);

	outb(D8390_COMMAND_RD2 + D8390_COMMAND_PS0,
	     eth_nic_base + D8390_P0_COMMAND);
	return ret;
}

static void set_count(unsigned int eth_nic_base, unsigned int what)
{
	outb(D8390_COMMAND_RD2 + D8390_COMMAND_PS1,
	     eth_nic_base + D8390_P0_COMMAND);

	outb(what & 0xff,eth_nic_base + 8);
	outb((what >> 8) & 0xff,eth_nic_base + 8 + 1);

	outb(D8390_COMMAND_RD2 + D8390_COMMAND_PS0,
	     eth_nic_base + D8390_P0_COMMAND);
}

static void eth_pio_write(unsigned char *src, unsigned int dst, unsigned int cnt,
				unsigned int eth_nic_base)
{
	outb(D8390_COMMAND_RD2 | D8390_COMMAND_STA, eth_nic_base + D8390_P0_COMMAND);
	outb(D8390_ISR_RDC, eth_nic_base + D8390_P0_ISR);
	outb(cnt, eth_nic_base + D8390_P0_RBCR0);
	outb(cnt >> 8, eth_nic_base + D8390_P0_RBCR1);
	outb(dst, eth_nic_base + D8390_P0_RSAR0);
	outb(dst >> 8, eth_nic_base + D8390_P0_RSAR1);
	outb(D8390_COMMAND_RD1 | D8390_COMMAND_STA, eth_nic_base + D8390_P0_COMMAND);

	while (cnt--) {
		outb(*(src++), eth_nic_base + NE_ASIC_OFFSET + NE_DATA);
	}
	/*
	#warning "Add timeout"
	*/
	/* wait for operation finish */
	while ((inb(eth_nic_base + D8390_P0_ISR) & D8390_ISR_RDC) != D8390_ISR_RDC)
		;
}

void ne2k_append_data(unsigned char *d, int len, unsigned int base)
{
	eth_pio_write(d, (TX_START << 8) + 42 + get_count(base), len, base);
	set_count(base, get_count(base)+len);
}

static void str2ip(const char *str, unsigned char *ip)
{
	unsigned char c, i = 0;
	int acc = 0;

	do {
		c = str[i];
		if ((c >= '0') && (c <= '9')) {
			acc *= 10;
			acc += (c - '0');
		} else {
			*ip++ = acc;
			acc = 0;
		}
		i++;
	} while (c != '\0');
}

static void str2mac(const char *str, unsigned char *mac)
{
	unsigned char c, i = 0;
	int acc = 0;

	do {
		c = str[i];
		if ((c >= '0') && (c <= '9')) {
			acc *= 16;
			acc += (c - '0');
		} else if ((c >= 'a') && (c <= 'f')) {
			acc *= 16;
			acc += ((c - 'a') + 10) ;
		} else if ((c >= 'A') && (c <= 'F')) {
			acc *= 16;
			acc += ((c - 'A') + 10) ;
		} else {
			*mac++ = acc;
			acc = 0;
		}

		i++;
	} while (c != '\0');
}

static void ns8390_tx_header(unsigned int eth_nic_base, int pktlen)
{
	unsigned short chksum;
	unsigned char hdr[] = {
		/* ETHERNET HDR */
		/* destination macaddr */
		0x02, 0x00, 0x00, 0x00, 0x00, 0x01,
		/* source mac */
		0x02, 0x00, 0x00, 0xC0, 0xFF, 0xEE,
		/* ethtype (IP) */
		0x08, 0x00,

		/* IP HDR */
		0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
		/* TTL, proto (UDP), chksum_hi, chksum_lo, IP0, IP1, IP2, IP3, */
		0x40, 0x11, 0x0, 0x0, 0x7f, 0x0, 0x0, 0x1,
		/* IP0, IP1, IP2, IP3  */
		0xff, 0xff, 0xff, 0xff,

		/* UDP HDR */
		/* SRC PORT DST PORT (2 bytes each),
		 * ulen, uchksum (must be zero or correct */
		0x1a, 0x0b, 0x1a, 0x0a, 0x00, 0x9, 0x00, 0x00,
	};

	str2mac(CONFIG_CONSOLE_NE2K_DST_MAC,  &hdr[0]);
	str2ip(CONFIG_CONSOLE_NE2K_DST_IP, &hdr[30]);
	str2ip(CONFIG_CONSOLE_NE2K_SRC_IP, &hdr[26]);

	/* zero checksum */
	hdr[24] = 0;
	hdr[25] = 0;

	/* update IP packet len */
	hdr[16] = ((28 + pktlen) >> 8) & 0xff;
	hdr[17] = (28 + pktlen) & 0xff;

	/* update UDP len */
	hdr[38] = (8 + pktlen) >> 8;
	hdr[39] = 8 + pktlen;

	chksum = compute_ip_checksum(&hdr[14], 20);

	hdr[25] = chksum >> 8;
	hdr[24] = chksum;
	eth_pio_write(hdr, (TX_START << 8), sizeof(hdr), eth_nic_base);
}

void ne2k_transmit(unsigned int eth_nic_base)
{
	unsigned int pktsize;
	unsigned int len = get_count(eth_nic_base);

	// so place whole header inside chip buffer
	ns8390_tx_header(eth_nic_base, len);

	// commit sending now
	outb(D8390_COMMAND_PS0 | D8390_COMMAND_RD2 | D8390_COMMAND_STA, eth_nic_base + D8390_P0_COMMAND);

	outb(TX_START, eth_nic_base + D8390_P0_TPSR);

	pktsize = 42 + len;
	if (pktsize < 64)
		pktsize = 64;

	outb(pktsize, eth_nic_base + D8390_P0_TBCR0);
	outb(pktsize >> 8, eth_nic_base + D8390_P0_TBCR1);

	outb(D8390_ISR_PTX, eth_nic_base + D8390_P0_ISR);

	outb(D8390_COMMAND_PS0 | D8390_COMMAND_TXP | D8390_COMMAND_RD2 | D8390_COMMAND_STA, eth_nic_base + D8390_P0_COMMAND);

	/* wait for operation finish */
	while ((inb(eth_nic_base + D8390_P0_ISR) & D8390_ISR_PTX) != D8390_ISR_PTX) ;

	set_count(eth_nic_base, 0);
}

static void ns8390_reset(unsigned int eth_nic_base)
{
	int i;

	outb(D8390_COMMAND_PS0 | D8390_COMMAND_RD2 |
	     D8390_COMMAND_STP, eth_nic_base + D8390_P0_COMMAND);

	outb(0x48, eth_nic_base + D8390_P0_DCR);
	outb(0, eth_nic_base + D8390_P0_RBCR0);
	outb(0, eth_nic_base + D8390_P0_RBCR1);
	outb(0x20, eth_nic_base + D8390_P0_RCR);
	outb(2, eth_nic_base + D8390_P0_TCR);
	outb(TX_START, eth_nic_base + D8390_P0_TPSR);
	outb(RX_START, eth_nic_base + D8390_P0_PSTART);
	outb(MEM_SIZE, eth_nic_base + D8390_P0_PSTOP);
	outb(MEM_SIZE - 1, eth_nic_base + D8390_P0_BOUND);
	outb(0xFF, eth_nic_base + D8390_P0_ISR);
	outb(0, eth_nic_base + D8390_P0_IMR);

	outb(D8390_COMMAND_PS1 |
	     D8390_COMMAND_RD2 | D8390_COMMAND_STP,
	     eth_nic_base + D8390_P0_COMMAND);

	for (i = 0; i < ETH_ALEN; i++)
		outb(0x0C, eth_nic_base + D8390_P1_PAR0 + i);

	for (i = 0; i < ETH_ALEN; i++)
		outb(0xFF, eth_nic_base + D8390_P1_MAR0 + i);

	outb(RX_START, eth_nic_base + D8390_P1_CURR);
	outb(D8390_COMMAND_PS0 |
	     D8390_COMMAND_RD2 | D8390_COMMAND_STA,
	     eth_nic_base + D8390_P0_COMMAND);
	outb(0xFF, eth_nic_base + D8390_P0_ISR);
	outb(0, eth_nic_base + D8390_P0_TCR);
	outb(4, eth_nic_base + D8390_P0_RCR);
	set_count(eth_nic_base, 0);
}

int ne2k_init(unsigned int eth_nic_base)
{
	pci_devfn_t dev;
	unsigned char c;

	/* FIXME: This console is not enabled for bootblock. */
	if (!ENV_ROMSTAGE)
		return 0;

	/* For this to work, mainboard code must have configured
	   PCI bridges prior to calling console_init(). */
	dev = pci_locate_device(PCI_ID(0x10ec, 0x8029), 0);
	if (dev == PCI_DEV_INVALID)
		return 0;

	pci_s_write_config32(dev, 0x10, eth_nic_base | 1);
	pci_s_write_config8(dev, 0x4, 0x1);

	c = inb(eth_nic_base + NE_ASIC_OFFSET + NE_RESET);
	outb(c, eth_nic_base + NE_ASIC_OFFSET + NE_RESET);

	(void) inb(0x84);

	outb(D8390_COMMAND_STP | D8390_COMMAND_RD2, eth_nic_base + D8390_P0_COMMAND);
	outb(D8390_RCR_MON, eth_nic_base + D8390_P0_RCR);

	outb(D8390_DCR_FT1 | D8390_DCR_LS, eth_nic_base + D8390_P0_DCR);
	outb(MEM_8192, eth_nic_base + D8390_P0_PSTART);
	outb(MEM_16384, eth_nic_base + D8390_P0_PSTOP);

	ns8390_reset(eth_nic_base);
	return 1;
}

static void read_resources(struct device *dev)
{
	struct resource *res;

	res = new_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = CONFIG_CONSOLE_NE2K_IO_PORT;
	res->size = 32;
	res->align = 5;
	res->gran = 5;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_STORED |
				IORESOURCE_ASSIGNED;
}

static struct device_operations ne2k_ops  = {
	.read_resources   = read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
};

static const struct pci_driver ne2k_driver __pci_driver = {
	.ops    = &ne2k_ops,
	.vendor = 0x10ec,
	.device = 0x8029,
};
