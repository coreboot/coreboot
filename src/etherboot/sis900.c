/* -*- Mode:C; c-basic-offset:4; -*- */

/* 
   sis900.c: An SiS 900/7016 PCI Fast Ethernet driver for Etherboot
   Copyright (C) 2001 Entity Cyber, Inc.

   Revision:	1.0	March 1, 2001
   
   Author: Marty Connor (mdc@thinguin.org)

   Adapted from a Linux driver which was written by Donald Becker
   and modified by Ollie Lho and Chin-Shan Li of SiS Corporation.
   Rewritten for Etherboot by Marty Connor.
   
   This software may be used and distributed according to the terms
   of the GNU Public License (GPL), incorporated herein by reference.
   
   References:
   SiS 7016 Fast Ethernet PCI Bus 10/100 Mbps LAN Controller with OnNow Support,
   preliminary Rev. 1.0 Jan. 14, 1998
   SiS 900 Fast Ethernet PCI Bus 10/100 Mbps LAN Single Chip with OnNow Support,
   preliminary Rev. 1.0 Nov. 10, 1998
   SiS 7014 Single Chip 100BASE-TX/10BASE-T Physical Layer Solution,
   preliminary Rev. 1.0 Jan. 18, 1998
   http://www.sis.com.tw/support/databook.htm */

/* Revision History */

/*
  01 March 2001  mdc     1.0
     Initial Release.  Tested with PCI based sis900 card and ThinkNIC
     computer.
  20 March 2001 P.Koegel
     added support for sis630e and PHY ICS1893 and RTL8201
     Testet with SIS730S chipset + ICS1893
*/


/* Includes */

#include <types.h>
#include "etherboot.h"
#include "nic.h"
#include "pci.h"
#include "cards.h"

#include "sis900.h"

/* Globals */

static int sis900_debug = 0;

static unsigned short vendor, dev_id;
static unsigned long ioaddr;

static unsigned int cur_phy;

static unsigned int cur_rx;

static BufferDesc txd;
static BufferDesc rxd[NUM_RX_DESC];

#ifdef USE_LOWMEM_BUFFER
#define txb ((char *)0x10000 - TX_BUF_SIZE)
#define rxb ((char *)0x10000 - NUM_RX_DESC*RX_BUF_SIZE - TX_BUF_SIZE)
#else
static unsigned char txb[TX_BUF_SIZE];
static unsigned char rxb[NUM_RX_DESC * RX_BUF_SIZE];
#endif

static struct mac_chip_info {
    const char *name;
    u16 vendor_id, device_id, flags;
    int io_size;
} mac_chip_table[] = {
    { "SiS 900 PCI Fast Ethernet", PCI_VENDOR_ID_SIS, PCI_DEVICE_ID_SIS900,
      PCI_COMMAND_IO|PCI_COMMAND_MASTER, SIS900_TOTAL_SIZE},
    { "SiS 7016 PCI Fast Ethernet",PCI_VENDOR_ID_SIS, PCI_DEVICE_ID_SIS7016,
      PCI_COMMAND_IO|PCI_COMMAND_MASTER, SIS900_TOTAL_SIZE},
    {0,0,0,0,0} /* 0 terminated list. */
};

static void sis900_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex);
static void amd79c901_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex);
static void ics1893_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex);
static void rtl8201_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex);

static struct mii_chip_info {
    const char * name;
    u16 phy_id0;
    u16 phy_id1;
    void (*read_mode) (struct nic *nic, int phy_addr, int *speed, int *duplex);
} mii_chip_table[] = {
    {"SiS 900 Internal MII PHY", 0x001d, 0x8000, sis900_read_mode},
    {"SiS 7014 Physical Layer Solution", 0x0016, 0xf830,sis900_read_mode},
    {"AMD 79C901 10BASE-T PHY",  0x0000, 0x35b9, amd79c901_read_mode},
    {"AMD 79C901 HomePNA PHY",   0x0000, 0x35c8, amd79c901_read_mode},
    {"ICS 1893 Integrated PHYceiver"   , 0x0015, 0xf441,ics1893_read_mode},
    {"RTL 8201 10/100Mbps Phyceiver"   , 0x0000, 0x8201,rtl8201_read_mode},
    {0,0,0,0}
};

static struct mii_phy {
    struct mii_phy * next;
    struct mii_chip_info * chip_info;
    int phy_addr;
    u16 status;
} mii;


// PCI to ISA bridge for SIS640E access
static struct pci_device   pci_isa_bridge_list[] = {
	{ 0x1039, 0x0008,
		"SIS 85C503/5513 PCI to ISA bridge", 0, 0, 0, 0},
	{0, 0, NULL, 0, 0, 0, 0}
};

/* Function Prototypes */

struct nic *sis900_probe(struct nic *nic, unsigned short *io_addrs, struct pci_device *pci);

static u16  sis900_read_eeprom(int location);
static void sis900_mdio_reset(long mdio_addr);
static void sis900_mdio_idle(long mdio_addr);
static u16  sis900_mdio_read(int phy_id, int location);
static void sis900_mdio_write(int phy_id, int location, int val);

static void sis900_init(struct nic *nic);

static void sis900_reset(struct nic *nic);

static void sis900_init_rxfilter(struct nic *nic);
static void sis900_init_txd(struct nic *nic);
static void sis900_init_rxd(struct nic *nic);
static void sis900_set_rx_mode(struct nic *nic);
static void sis900_check_mode(struct nic *nic);

static void sis900_transmit(struct nic *nic, const char *d, 
                            unsigned int t, unsigned int s, const char *p);
static int  sis900_poll(struct nic *nic);

static void sis900_disable(struct nic *nic);

/**
 *	sis900_get_mac_addr: - Get MAC address for stand alone SiS900 model
 *	@pci_dev: the sis900 pci device
 *	@net_dev: the net device to get address for
 *
 *	Older SiS900 and friends, use EEPROM to store MAC address.
 *	MAC address is read from read_eeprom() into @net_dev->dev_addr.
 */

static int sis900_get_mac_addr(struct pci_device * pci_dev , struct nic *nic)
{
	u16 signature;
	int i;

	/* check to see if we have sane EEPROM */
	signature = (u16) sis900_read_eeprom( EEPROMSignature);
	if (signature == 0xffff || signature == 0x0000) {
		printk ("sis900_probe: Error EERPOM read %x\n", signature);
		return 0;
	}

	/* get MAC address from EEPROM */
	for (i = 0; i < 3; i++)
			((u16 *)(nic->node_addr))[i] = sis900_read_eeprom(i+EEPROMMACAddr);
	return 1;
}

/**
 *	sis630e_get_mac_addr: - Get MAC address for SiS630E model
 *	@pci_dev: the sis900 pci device
 *	@net_dev: the net device to get address for
 *
 *	SiS630E model, use APC CMOS RAM to store MAC address.
 *	APC CMOS RAM is accessed through ISA bridge.
 *	MAC address is read into @net_dev->dev_addr.
 */

#if 0
static int sis630e_get_mac_addr(struct pci_device * pci_dev, struct nic *nic)
{
	u8 reg;
	int i;
	struct pci_device	*p;

	// find PCI to ISA bridge
	eth_pci_init(pci_isa_bridge_list);

    /* the firts entry in this list should contain bus/devfn */
    p = pci_isa_bridge_list;

	pcibios_read_config_byte(p->bus,p->devfn, 0x48, &reg);
	pcibios_write_config_byte(p->bus,p->devfn, 0x48, reg | 0x40);

	for (i = 0; i < 6; i++)
	{
		outb(0x09 + i, 0x70);
		((u8 *)(nic->node_addr))[i] = inb(0x71);
	}
	pcibios_write_config_byte(p->bus,p->devfn, 0x48, reg & ~0x40);

	return 1;
}

#else

static int sis630e_get_mac_addr(struct pci_device * pci_dev, struct nic *nic)
{
	u8 reg;
	int i;
	struct pci_device	*p;

	// find PCI to ISA bridge
//	eth_pci_init(pci_isa_bridge_list);

    /* the firts entry in this list should contain bus/devfn */
 //   p = pci_isa_bridge_list;

	outl(0x80000848, 0xcf8);
	outb(0x50,0x0cfc);
//	pcibios_read_config_byte(p->bus,p->devfn, 0x48, &reg);
//	pcibios_write_config_byte(p->bus,p->devfn, 0x48, reg | 0x40);

	for (i = 0; i < 6; i++)
	{
		outb(0x09 + i, 0x70);
		((u8 *)(nic->node_addr))[i] = inb(0x71);
	}
//	pcibios_write_config_byte(p->bus,p->devfn, 0x48, reg & ~0x40);

	return 1;
}

#endif

/* 
 * Function: sis900_probe
 *
 * Description: initializes initializes the NIC, retrieves the
 *    MAC address of the card, and sets up some globals required by 
 *    other routines.
 *
 * Side effects:
 *            leaves the ioaddress of the sis900 chip in the variable ioaddr.
 *            leaves the sis900 initialized, and ready to recieve packets.
 *
 * Returns:   struct nic *:          pointer to NIC data structure
 */

struct nic *sis900_probe(struct nic *nic, unsigned short *io_addrs, struct pci_device *pci)
{
    int i;
    int found=0;
    int phy_addr;
    u16 signature;
    u8 revision;
    int ret;

    if (io_addrs == 0 || *io_addrs == 0)
        return NULL;

    ioaddr  = *io_addrs & ~3;
    vendor  = pci->vendor;
    dev_id  = pci->dev_id;

    /* wakeup chip */
    pcibios_write_config_dword(pci->bus, pci->devfn, 0x40, 0x00000000);

    /* get MAC address */
    ret = 0;
    pcibios_read_config_byte(pci->bus,pci->devfn, PCI_REVISION, &revision);
    ret = sis630e_get_mac_addr(pci, nic);

#if 0
    if (revision == SIS630E_900_REV || revision == SIS630EA1_900_REV)
       ret = sis630e_get_mac_addr(pci, nic);
    else if (revision == SIS630S_900_REV)
        ret = sis630e_get_mac_addr(pci, nic);
    else
        ret = sis900_get_mac_addr(pci, nic);
#endif

    if (ret == 0)
    {
        printk ("sis900_probe: Error MAC address not found\n");
        return NULL;
    }

    printk("\nsis900_probe: MAC addr %02x:%02x:%02x:%02x:%02x:%02x at ioaddr %04x\n",
           nic->node_addr[0],nic->node_addr[1],nic->node_addr[2],
           nic->node_addr[3],nic->node_addr[4],nic->node_addr[5],
           ioaddr);
    printk("sis900_probe: Vendor:%04x Device:%04x\n", vendor, dev_id);

    /* probe for mii transceiver */
    /* search for total of 32 possible mii phy addresses */

    found = 0;
    for (phy_addr = 0; phy_addr < 32; phy_addr++) {
        u16 mii_status;
        u16 phy_id0, phy_id1;
                
        mii_status = sis900_mdio_read(phy_addr, MII_STATUS);
        if (mii_status == 0xffff || mii_status == 0x0000)
            /* the mii is not accessable, try next one */
            continue;
                
        phy_id0 = sis900_mdio_read(phy_addr, MII_PHY_ID0);
        phy_id1 = sis900_mdio_read(phy_addr, MII_PHY_ID1);
                
        /* search our mii table for the current mii */ 
        for (i = 0; mii_chip_table[i].phy_id1; i++) {

            if (phy_id0 == mii_chip_table[i].phy_id0) {

                printk("sis900_probe: %s transceiver found at address %d.\n",
                       mii_chip_table[i].name, phy_addr);

                mii.chip_info = &mii_chip_table[i];
                mii.phy_addr  = phy_addr;
                mii.status    = sis900_mdio_read(phy_addr, MII_STATUS);
                mii.next      = NULL;

                found=1;
                break;
            } else {
		printk("Found an MII, but can't recognize it. id= %u:%u\n", phy_id0, phy_id1);
            }
        }
    }
        
    if (found == 0) {
        printk("sis900_probe: No MII transceivers found!\n");
        return NULL;
    }

    /* Arbitrarily select the last PHY found as current PHY */
    cur_phy = mii.phy_addr;
    printk("sis900_probe: Using %s as default\n",  mii.chip_info->name);

    /* initialize device */
    sis900_init(nic);

    nic->reset    = sis900_init;
    nic->poll     = sis900_poll;
    nic->transmit = sis900_transmit;
    nic->disable  = sis900_disable;

    return nic;
}


/* 
 * EEPROM Routines:  These functions read and write to EEPROM for 
 *    retrieving the MAC address and other configuration information about 
 *    the card.
 */

/* Delay between EEPROM clock transitions. */
#define eeprom_delay()  inl(ee_addr)


/* Function: sis900_read_eeprom
 *
 * Description: reads and returns a given location from EEPROM
 *
 * Arguments: int location:       requested EEPROM location
 *
 * Returns:   u16:                contents of requested EEPROM location
 *
 */

/* Read Serial EEPROM through EEPROM Access Register, Note that location is 
   in word (16 bits) unit */
static u16 sis900_read_eeprom(int location)
{
    int i;
    u16 retval = 0;
    long ee_addr = ioaddr + mear;
    u32 read_cmd = location | EEread;

    outl(0, ee_addr);
    eeprom_delay();
    outl(EECLK, ee_addr);
    eeprom_delay();

    /* Shift the read command (9) bits out. */
    for (i = 8; i >= 0; i--) {
        u32 dataval = (read_cmd & (1 << i)) ? EEDI | EECS : EECS;
        outl(dataval, ee_addr);
        eeprom_delay();
        outl(dataval | EECLK, ee_addr);
        eeprom_delay();
    }
    outb(EECS, ee_addr);
    eeprom_delay();

    /* read the 16-bits data in */
    for (i = 16; i > 0; i--) {
        outl(EECS, ee_addr);
        eeprom_delay();
        outl(EECS | EECLK, ee_addr);
        eeprom_delay();
        retval = (retval << 1) | ((inl(ee_addr) & EEDO) ? 1 : 0);
        eeprom_delay();
    }
                
    /* Terminate the EEPROM access. */
    outl(0, ee_addr);
    eeprom_delay();
    outl(EECLK, ee_addr);

    return (retval);
}

#define sis900_mdio_delay()    inl(mdio_addr)


/* 
   Read and write the MII management registers using software-generated
   serial MDIO protocol. Note that the command bits and data bits are
   send out seperately 
*/

static void sis900_mdio_idle(long mdio_addr)
{
    outl(MDIO | MDDIR, mdio_addr);
    sis900_mdio_delay();
    outl(MDIO | MDDIR | MDC, mdio_addr);
}

/* Syncronize the MII management interface by shifting 32 one bits out. */
static void sis900_mdio_reset(long mdio_addr)
{
    int i;

    for (i = 31; i >= 0; i--) {
        outl(MDDIR | MDIO, mdio_addr);
        sis900_mdio_delay();
        outl(MDDIR | MDIO | MDC, mdio_addr);
        sis900_mdio_delay();
    }
    return;
}

static u16 sis900_mdio_read(int phy_id, int location)
{
    long mdio_addr = ioaddr + mear;
    int mii_cmd = MIIread|(phy_id<<MIIpmdShift)|(location<<MIIregShift);
    u16 retval = 0;
    int i;

    sis900_mdio_reset(mdio_addr);
    sis900_mdio_idle(mdio_addr);

    for (i = 15; i >= 0; i--) {
        int dataval = (mii_cmd & (1 << i)) ? MDDIR | MDIO : MDDIR;
        outl(dataval, mdio_addr);
        sis900_mdio_delay();
        outl(dataval | MDC, mdio_addr);
        sis900_mdio_delay();
    }

    /* Read the 16 data bits. */
    for (i = 16; i > 0; i--) {
        outl(0, mdio_addr);
        sis900_mdio_delay();
        retval = (retval << 1) | ((inl(mdio_addr) & MDIO) ? 1 : 0);
        outl(MDC, mdio_addr);
        sis900_mdio_delay();
    }
    return retval;
}

static void sis900_mdio_write(int phy_id, int location, int value)
{
    long mdio_addr = ioaddr + mear;
    int mii_cmd = MIIwrite|(phy_id<<MIIpmdShift)|(location<<MIIregShift);
    int i;

    sis900_mdio_reset(mdio_addr);
    sis900_mdio_idle(mdio_addr);

    /* Shift the command bits out. */
    for (i = 15; i >= 0; i--) {
        int dataval = (mii_cmd & (1 << i)) ? MDDIR | MDIO : MDDIR;
        outb(dataval, mdio_addr);
        sis900_mdio_delay();
        outb(dataval | MDC, mdio_addr);
        sis900_mdio_delay();
    }
    sis900_mdio_delay();

    /* Shift the value bits out. */
    for (i = 15; i >= 0; i--) {
        int dataval = (value & (1 << i)) ? MDDIR | MDIO : MDDIR;
        outl(dataval, mdio_addr);
        sis900_mdio_delay();
        outl(dataval | MDC, mdio_addr);
        sis900_mdio_delay();
    }
    sis900_mdio_delay();
        
    /* Clear out extra bits. */
    for (i = 2; i > 0; i--) {
        outb(0, mdio_addr);
        sis900_mdio_delay();
        outb(MDC, mdio_addr);
        sis900_mdio_delay();
    }
    return;
}


/* Function: sis900_init
 *
 * Description: resets the ethernet controller chip and various
 *    data structures required for sending and receiving packets.
 *    
 * Arguments: struct nic *nic:          NIC data structure
 *
 * returns:   void.
 */

static void
sis900_init(struct nic *nic)
{
    /* Soft reset the chip. */
    sis900_reset(nic);

    sis900_init_rxfilter(nic);

    sis900_init_txd(nic);
    sis900_init_rxd(nic);

    sis900_set_rx_mode(nic);

    sis900_check_mode(nic);

    outl(RxENA, ioaddr + cr);
}


/* 
 * Function: sis900_reset
 *
 * Description: disables interrupts and soft resets the controller chip
 *
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   void.
 */

static void 
sis900_reset(struct nic *nic)
{
    int i = 0;
    u32 status = TxRCMP | RxRCMP;

    outl(0, ioaddr + ier);
    outl(0, ioaddr + imr);
    outl(0, ioaddr + rfcr);

    outl(RxRESET | TxRESET | RESET, ioaddr + cr);
        
    /* Check that the chip has finished the reset. */
    while (status && (i++ < 1000)) {
        status ^= (inl(isr + ioaddr) & status);
    }
    outl(PESEL, ioaddr + cfg);
}


/* Function: sis_init_rxfilter
 *
 * Description: sets receive filter address to our MAC address
 *
 * Arguments: struct nic *nic:          NIC data structure
 *
 * returns:   void.
 */

static void
sis900_init_rxfilter(struct nic *nic)
{
    u32 rfcrSave;
    int i;
        
    rfcrSave = inl(rfcr + ioaddr);

    /* disable packet filtering before setting filter */
    outl(rfcrSave & ~RFEN, rfcr);

    /* load MAC addr to filter data register */
    for (i = 0 ; i < 3 ; i++) {
        u32 w;

        w = (u32) *((u16 *)(nic->node_addr)+i);
        outl((i << RFADDR_shift), ioaddr + rfcr);
        outl(w, ioaddr + rfdr);

        if (sis900_debug > 0)
            printk("sis900_init_rxfilter: Receive Filter Addrss[%d]=%x\n",
                   i, inl(ioaddr + rfdr));
    }

    /* enable packet filitering */
    outl(rfcrSave | RFEN, rfcr + ioaddr);
}


/* 
 * Function: sis_init_txd
 *
 * Description: initializes the Tx descriptor
 *
 * Arguments: struct nic *nic:          NIC data structure
 *
 * returns:   void.
 */

static void
sis900_init_txd(struct nic *nic)
{
    txd.link   = (u32) 0;
    txd.cmdsts = (u32) 0;
    txd.bufptr = (u32) &txb[0];

    /* load Transmit Descriptor Register */
    outl((u32) &txd, ioaddr + txdp); 
    if (sis900_debug > 0)
        printk("sis900_init_txd: TX descriptor register loaded with: %X\n", 
               inl(ioaddr + txdp));
}


/* Function: sis_init_rxd
 *
 * Description: initializes the Rx descriptor ring
 *    
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   void.
 */

static void 
sis900_init_rxd(struct nic *nic) 
{ 
    int i;

    cur_rx = 0; 

    /* init RX descriptor */
    for (i = 0; i < NUM_RX_DESC; i++) {
        rxd[i].link   = (i+1 < NUM_RX_DESC) ? (u32) &rxd[i+1] : (u32) &rxd[0];
        rxd[i].cmdsts = (u32) RX_BUF_SIZE;
        rxd[i].bufptr = (u32) &rxb[i*RX_BUF_SIZE];
        if (sis900_debug > 0)
            printk("sis900_init_rxd: rxd[%d]=%X link=%X cmdsts=%X bufptr=%X\n", 
                   i, &rxd[i], rxd[i].link, rxd[i].cmdsts, rxd[i].bufptr);
    }

    /* load Receive Descriptor Register */
    outl((u32) &rxd[0], ioaddr + rxdp);

    if (sis900_debug > 0)
        printk("sis900_init_rxd: RX descriptor register loaded with: %X\n", 
               inl(ioaddr + rxdp));

}


/* Function: sis_init_rxd
 *
 * Description: 
 *    sets the receive mode to accept all broadcast packets and packets
 *    with our MAC address, and reject all multicast packets.      
 *    
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   void.
 */

static void sis900_set_rx_mode(struct nic *nic)
{
    int i;

    /* Configure Multicast Hash Table in Receive Filter 
       to reject all MCAST packets */
    for (i = 0; i < 8; i++) {
        /* why plus 0x04? That makes the correct value for hash table. */
        outl((u32)(0x00000004+i) << RFADDR_shift, ioaddr + rfcr);
        outl((u32)(0x0), ioaddr + rfdr);
    }
    /* Accept Broadcast packets, destination addresses that match 
       our MAC address */
    outl(RFEN | RFAAB, ioaddr + rfcr);

    return;
}


/* Function: sis900_check_mode
 *
 * Description: checks the state of transmit and receive
 *    parameters on the NIC, and updates NIC registers to match
 *    
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   void.
 */

static void
sis900_check_mode (struct nic *nic)
{
    int speed, duplex;
    u32 tx_flags = 0, rx_flags = 0;

    mii.chip_info->read_mode(nic, cur_phy, &speed, &duplex);

    tx_flags = TxATP | (TX_DMA_BURST << TxMXDMA_shift) | (TX_FILL_THRESH << TxFILLT_shift);
    rx_flags = RX_DMA_BURST << RxMXDMA_shift;

    if (speed == HW_SPEED_HOME || speed == HW_SPEED_10_MBPS) {
        rx_flags |= (RxDRNT_10 << RxDRNT_shift);
        tx_flags |= (TxDRNT_10 << TxDRNT_shift);
    }
    else {
        rx_flags |= (RxDRNT_100 << RxDRNT_shift);
        tx_flags |= (TxDRNT_100 << TxDRNT_shift);
    }

    if (duplex == FDX_CAPABLE_FULL_SELECTED) {
        tx_flags |= (TxCSI | TxHBI);
        rx_flags |= RxATX;
    }

    outl (tx_flags, ioaddr + txcfg);
    outl (rx_flags, ioaddr + rxcfg);
}


/* Function: sis900_read_mode
 *
 * Description: retrieves and displays speed and duplex
 *    parameters from the NIC
 *    
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   void.
 */

static void
sis900_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex)
{
    int i = 0;
    u32 status;
        
    /* STSOUT register is Latched on Transition, read operation updates it */
    while (i++ < 2)
        status = sis900_mdio_read(phy_addr, MII_STSOUT);

    if (status & MII_STSOUT_SPD)
        *speed = HW_SPEED_100_MBPS;
    else
        *speed = HW_SPEED_10_MBPS;

    if (status & MII_STSOUT_DPLX)
        *duplex = FDX_CAPABLE_FULL_SELECTED;
    else
        *duplex = FDX_CAPABLE_HALF_SELECTED;

    if (status & MII_STSOUT_LINK_FAIL)
        printk("sis900_read_mode: Media Link Off\n");
    else
        printk("sis900_read_mode: Media Link On %s %s-duplex \n", 
               *speed == HW_SPEED_100_MBPS ? 
               "100mbps" : "10mbps",
               *duplex == FDX_CAPABLE_FULL_SELECTED ?
               "full" : "half");
}


/* Function: amd79c901_read_mode
 *
 * Description: retrieves and displays speed and duplex
 *    parameters from the NIC
 *    
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   void.
 */

static void
amd79c901_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex)
{
    int i;
    u16 status;
        
    for (i = 0; i < 2; i++)
        status = sis900_mdio_read(phy_addr, MII_STATUS);

    if (status & MII_STAT_CAN_AUTO) {
        /* 10BASE-T PHY */
        for (i = 0; i < 2; i++)
            status = sis900_mdio_read(phy_addr, MII_STATUS_SUMMARY);
        if (status & MII_STSSUM_SPD)
            *speed = HW_SPEED_100_MBPS;
        else
            *speed = HW_SPEED_10_MBPS;
        if (status & MII_STSSUM_DPLX)
            *duplex = FDX_CAPABLE_FULL_SELECTED;
        else
            *duplex = FDX_CAPABLE_HALF_SELECTED;

        if (status & MII_STSSUM_LINK)
            printk("amd79c901_read_mode: Media Link On %s %s-duplex \n", 
                   *speed == HW_SPEED_100_MBPS ? 
                   "100mbps" : "10mbps",
                   *duplex == FDX_CAPABLE_FULL_SELECTED ?
                   "full" : "half");
        else
            printk("amd79c901_read_mode: Media Link Off\n");
    }
    else {
        /* HomePNA */
        *speed = HW_SPEED_HOME;
        *duplex = FDX_CAPABLE_HALF_SELECTED;
        if (status & MII_STAT_LINK)
            printk("amd79c901_read_mode:Media Link On 1mbps half-duplex \n");
        else
            printk("amd79c901_read_mode: Media Link Off\n");
    }
}


/**
 *	ics1893_read_mode: - read media mode for ICS1893 PHY
 *	@net_dev: the net device to read mode for
 *	@phy_addr: mii phy address
 *	@speed: the transmit speed to be determined
 *	@duplex: the duplex mode to be determined
 *
 *	ICS1893 PHY use Quick Poll Detailed Status register
 *	to determine the speed and duplex mode for sis900
 */

static void ics1893_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex)
{
	int i = 0;
	u32 status;

	/* MII_QPDSTS is Latched, read twice in succession will reflect the current state */
	for (i = 0; i < 2; i++)
		status = sis900_mdio_read(phy_addr, MII_QPDSTS);

	if (status & MII_STSICS_SPD)
		*speed = HW_SPEED_100_MBPS;
	else
		*speed = HW_SPEED_10_MBPS;

	if (status & MII_STSICS_DPLX)
		*duplex = FDX_CAPABLE_FULL_SELECTED;
	else
		*duplex = FDX_CAPABLE_HALF_SELECTED;

	if (status & MII_STSICS_LINKSTS)
		printk("ics1893_read_mode: Media Link On %s %s-duplex \n",
		       *speed == HW_SPEED_100_MBPS ?
		       "100mbps" : "10mbps",
		       *duplex == FDX_CAPABLE_FULL_SELECTED ?
		       "full" : "half");
	else
		printk("ics1893_read_mode: Media Link Off\n");
}

/**
 *	rtl8201_read_mode: - read media mode for rtl8201 phy
 *	@nic: the net device to read mode for
 *	@phy_addr: mii phy address
 *	@speed: the transmit speed to be determined
 *	@duplex: the duplex mode to be determined
 *
 *	read MII_STATUS register from rtl8201 phy
 *	to determine the speed and duplex mode for sis900
 */

static void rtl8201_read_mode(struct nic *nic, int phy_addr, int *speed, int *duplex)
{
	u32 status;

	status = sis900_mdio_read(phy_addr, MII_STATUS);

	if (status & MII_STAT_CAN_TX_FDX) {
		*speed = HW_SPEED_100_MBPS;
		*duplex = FDX_CAPABLE_FULL_SELECTED;
	}
	else if (status & MII_STAT_CAN_TX) {
		*speed = HW_SPEED_100_MBPS;
		*duplex = FDX_CAPABLE_HALF_SELECTED;
	}
	else if (status & MII_STAT_CAN_T_FDX) {
		*speed = HW_SPEED_10_MBPS;
		*duplex = FDX_CAPABLE_FULL_SELECTED;
	}
	else if (status & MII_STAT_CAN_T) {
		*speed = HW_SPEED_10_MBPS;
		*duplex = FDX_CAPABLE_HALF_SELECTED;
	}

	if (status & MII_STAT_LINK)
		printk("rtl8201_read_mode: Media Link On %s %s-duplex \n",
		       *speed == HW_SPEED_100_MBPS ?
		       "100mbps" : "10mbps",
		       *duplex == FDX_CAPABLE_FULL_SELECTED ?
		       "full" : "half");
	else
		printk("rtl9201_read_config_mode: Media Link Off\n");
}

/* Function: sis900_transmit
 *
 * Description: transmits a packet and waits for completion or timeout.
 *
 * Arguments: char d[6]:          destination ethernet address.
 *            unsigned short t:   ethernet protocol type.
 *            unsigned short s:   size of the data-part of the packet.
 *            char *p:            the data for the packet.
 *    
 * Returns:   void.
 */

static void
sis900_transmit(struct nic  *nic,
                const char  *d,     /* Destination */
                unsigned int t,     /* Type */
                unsigned int s,     /* size */
                const char  *p)     /* Packet */
{
    u32 status, to, nstype;
    u32 tx_status;
    
    /* Stop the transmitter */
    outl(TxDIS, ioaddr + cr);

    /* load Transmit Descriptor Register */
    outl((u32) &txd, ioaddr + txdp); 
    if (sis900_debug > 1)
        printk("sis900_transmit: TX descriptor register loaded with: %X\n", 
               inl(ioaddr + txdp));

    memcpy(txb, d, ETH_ALEN);
    memcpy(txb + ETH_ALEN, nic->node_addr, ETH_ALEN);
    nstype = htons(t);
    memcpy(txb + 2 * ETH_ALEN, (char*)&nstype, 2);
    memcpy(txb + ETH_HLEN, p, s);

    s += ETH_HLEN;
    s &= DSIZE;

    if (sis900_debug > 1)
        printk("sis900_transmit: sending %d bytes ethtype %x\n", (int) s, t);

    /* pad to minimum packet size */
    while (s < ETH_ZLEN)  
        txb[s++] = '\0';

    /* set the transmit buffer descriptor and enable Transmit State Machine */
    txd.bufptr = (u32) &txb[0];
    txd.cmdsts = (u32) OWN | s;

    /* restart the transmitter */
    outl(TxENA, ioaddr + cr);

    if (sis900_debug > 1)
        printk("sis900_transmit: Queued Tx packet size %d.\n", (int) s);

    to = currticks() + TX_TIMEOUT;

    while ((((volatile u32) tx_status=txd.cmdsts) & OWN) && (currticks() < to))
        /* wait */ ;

    if (currticks() >= to) {
        printk("sis900_transmit: TX Timeout! Tx status %X.\n", tx_status);
    }
    
    if (tx_status & (ABORT | UNDERRUN | OWCOLL)) {
        /* packet unsuccessfully transmited */
        printk("sis900_transmit: Transmit error, Tx status %X.\n", tx_status);
    }
    /* Disable interrupts by clearing the interrupt mask. */
    outl(0, ioaddr + imr);
}


/* Function: sis900_poll
 *
 * Description: checks for a received packet and returns it if found.
 *
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   1 if a packet was recieved.
 *            0 if no pacet was recieved.
 *
 * Side effects:
 *            Returns (copies) the packet to the array nic->packet.
 *            Returns the length of the packet in nic->packetlen.
 */

static int
sis900_poll(struct nic *nic)
{
    u32 rx_status = rxd[cur_rx].cmdsts;
    int retstat = 0;

    if (sis900_debug > 2)
        printk("sis900_poll: cur_rx:%d, status:%X\n", cur_rx, rx_status);

    if (!(rx_status & OWN))
        return retstat;

    if (sis900_debug > 1)
        printk("sis900_poll: got a packet: cur_rx:%d, status:%X\n",
               cur_rx, rx_status);

    nic->packetlen = (rx_status & DSIZE) - CRC_SIZE;

    if (rx_status & (ABORT|OVERRUN|TOOLONG|RUNT|RXISERR|CRCERR|FAERR)) {
        /* corrupted packet received */
        printk("sis900_poll: Corrupted packet received, buffer status = %X\n",
               rx_status);
        retstat = 0;
    } else {
        /* give packet to higher level routine */
        memcpy(nic->packet, (rxb + cur_rx*RX_BUF_SIZE), nic->packetlen);
        retstat = 1;
    }

    /* return the descriptor and buffer to receive ring */
    rxd[cur_rx].cmdsts = RX_BUF_SIZE;
    rxd[cur_rx].bufptr = (u32) &rxb[cur_rx*RX_BUF_SIZE];
        
    if (++cur_rx == NUM_RX_DESC)
        cur_rx = 0;

    /* re-enable the potentially idle receive state machine */
    outl(RxENA , ioaddr + cr);

    return retstat;
}


/* Function: sis900_disable
 *
 * Description: Turns off interrupts and stops Tx and Rx engines
 *    
 * Arguments: struct nic *nic:          NIC data structure
 *
 * Returns:   void.
 */

static void
sis900_disable(struct nic *nic)
{
    /* Disable interrupts by clearing the interrupt mask. */
    outl(0, ioaddr + imr);
    outl(0, ioaddr + ier);
    
    /* Stop the chip's Tx and Rx Status Machine */
    outl(RxDIS | TxDIS, ioaddr + cr);
}
