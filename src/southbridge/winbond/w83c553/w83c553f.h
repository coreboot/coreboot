/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

 /* winbond access routines and defines*/

/* from the winbond data sheet -
 The W83C553F SIO controller with PCI arbiter is a multi-function PCI device.
 Function 0 is the ISA bridge, and Function 1 is the bus master IDE controller.
*/

/*ISA bridge configuration space*/

#define W83C553F_VID		0x10AD
#define W83C553F_DID		0x0565
#define W83C553F_IDE		0x0105

/* Function 0 registers */
#define W83C553F_PCICONTR	0x40  /*pci control reg*/
#define W83C553F_SGBAR		0x41  /*scatter/gather base address reg*/
#define W83C553F_LBCR		0x42  /*Line Buffer Control reg*/
#define W83C553F_IDEIRCR	0x43  /*IDE Interrupt Routing Control  Reg*/
#define W83C553F_PCIIRCR	0x44  /*PCI Interrupt Routing Control Reg*/
#define W83C553F_BTBAR		0x46  /*BIOS Timer Base Address Register*/
#define W83C553F_IPADCR		0x48  /*ISA to PCI Address Decoder Control Register*/
#define W83C553F_IRADCR		0x49  /*ISA ROM Address Decoder Control Register*/
#define W83C553F_IPMHSAR	0x4a  /*ISA to PCI Memory Hole STart Address Register*/
#define W83C553F_IPMHSR		0x4b  /*ISA to PCI Memory Hols Size Register*/
#define W83C553F_CDR			0x4c  /*Clock Divisor Register*/
#define W83C553F_CSCR		0x4d  /*Chip Select Control Register*/
#define W83C553F_ATSCR		0x4e  /*AT System Control register*/
#define W83C553F_ATBCR		0x4f  /*AT Bus ControL Register*/
#define W83C553F_IRQBEE0R	0x60  /*IRQ Break Event Enable 0 Register*/
#define W83C553F_IRQBEE1R	0x61  /*IRQ Break Event Enable 1 Register*/
#define W83C553F_ABEER		0x62  /*Additional Break Event Enable Register*/
#define W83C553F_DMABEER	0x63  /*DMA Break Event Enable Register*/

/* Function 1 registers */
#define W83C553F_PIR		0x09  /*Programming Interface Register*/
#define W83C553F_IDECSR		0x40  /*IDE Control/Status Register*/

/* register bit definitions */
#define W83C553F_IPADCR_MBE512		0x1
#define W83C553F_IPADCR_MBE640		0x2
#define W83C553F_IPADCR_IPATOM4		0x10
#define W83C553F_IPADCR_IPATOM5		0x20
#define W83C553F_IPADCR_IPATOM6		0x40
#define W83C553F_IPADCR_IPATOM7		0x80

#define W83C553F_CSCR_UBIOSCSE		0x10
#define W83C553F_CSCR_BIOSWP		0x20

#define W83C553F_IDECSR_P0EN		0x01
#define W83C553F_IDECSR_P0F16		0x02
#define W83C553F_IDECSR_P1EN		0x10
#define W83C553F_IDECSR_P1F16		0x20
#define W83C553F_IDECSR_LEGIRQ		0x800

#define W83C553F_ATSCR_ISARE		0x40
#define W83C553F_ATSCR_FERRE		0x10
#define W83C553F_ATSCR_P92E		0x04
#define W83C553F_ATSCR_KRCEE		0x02
#define W83C553F_ATSCR_KGA20EE		0x01

#define W83C553F_PIR_BM			0x80
#define W83C553F_PIR_P1PROG		0x08
#define W83C553F_PIR_P1NL		0x04
#define W83C553F_PIR_P0PROG		0x02
#define W83C553F_PIR_P0NL		0x01

/*
 * Interrupt controller
 */
#define W83C553F_PIC1_ICW1	CONFIG_ISA_IO + 0x20
#define W83C553F_PIC1_ICW2	CONFIG_ISA_IO + 0x21
#define W83C553F_PIC1_ICW3	CONFIG_ISA_IO + 0x21
#define W83C553F_PIC1_ICW4	CONFIG_ISA_IO + 0x21
#define W83C553F_PIC1_OCW1	CONFIG_ISA_IO + 0x21
#define W83C553F_PIC1_OCW2	CONFIG_ISA_IO + 0x20
#define W83C553F_PIC1_OCW3	CONFIG_ISA_IO + 0x20
#define W83C553F_PIC1_ELC	CONFIG_ISA_IO + 0x4D0
#define W83C553F_PIC2_ICW1	CONFIG_ISA_IO + 0xA0
#define W83C553F_PIC2_ICW2	CONFIG_ISA_IO + 0xA1
#define W83C553F_PIC2_ICW3	CONFIG_ISA_IO + 0xA1
#define W83C553F_PIC2_ICW4	CONFIG_ISA_IO + 0xA1
#define W83C553F_PIC2_OCW1	CONFIG_ISA_IO + 0xA1
#define W83C553F_PIC2_OCW2	CONFIG_ISA_IO + 0xA0
#define W83C553F_PIC2_OCW3	CONFIG_ISA_IO + 0xA0
#define W83C553F_PIC2_ELC	CONFIG_ISA_IO + 0x4D1

#define W83C553F_TMR1_CMOD	CONFIG_ISA_IO + 0x43

/*
 * DMA controller
 */
#define W83C553F_DMA1	CONFIG_ISA_IO + 0x000	/* channel 0 - 3 */
#define W83C553F_DMA2	CONFIG_ISA_IO + 0x0C0	/* channel 4 - 7 */

/* command/status register bit definitions */

#define W83C553F_CS_COM_DACKAL	(1<<7)	/* DACK# assert level */
#define W83C553F_CS_COM_DREQSAL	(1<<6)	/* DREQ sense assert level */
#define W83C553F_CS_COM_GAP	(1<<4)	/* group arbitration priority */
#define W83C553F_CS_COM_CGE	(1<<2)	/* channel group enable */

#define W83C553F_CS_STAT_CH0REQ	(1<<4)	/* channel 0 (4) DREQ status */
#define W83C553F_CS_STAT_CH1REQ	(1<<5)	/* channel 1 (5) DREQ status */
#define W83C553F_CS_STAT_CH2REQ	(1<<6)	/* channel 2 (6) DREQ status */
#define W83C553F_CS_STAT_CH3REQ	(1<<7)	/* channel 3 (7) DREQ status */

#define W83C553F_CS_STAT_CH0TC	(1<<0)	/* channel 0 (4) TC status */
#define W83C553F_CS_STAT_CH1TC	(1<<1)	/* channel 1 (5) TC status */
#define W83C553F_CS_STAT_CH2TC	(1<<2)	/* channel 2 (6) TC status */
#define W83C553F_CS_STAT_CH3TC	(1<<3)	/* channel 3 (7) TC status */

/* mode register bit definitions */

#define W83C553F_MODE_TM_DEMAND	(0<<6)	/* transfer mode - demand */
#define W83C553F_MODE_TM_SINGLE	(1<<6)	/* transfer mode - single */
#define W83C553F_MODE_TM_BLOCK	(2<<6)	/* transfer mode - block */
#define W83C553F_MODE_TM_CASCADE	(3<<6)	/* transfer mode - cascade */
#define W83C553F_MODE_ADDRDEC	(1<<5)	/* address increment/decrement select */
#define W83C553F_MODE_AUTOINIT	(1<<4)	/* autoinitialize enable */
#define W83C553F_MODE_TT_VERIFY	(0<<2)	/* transfer type - verify */
#define W83C553F_MODE_TT_WRITE	(1<<2)	/* transfer type - write */
#define W83C553F_MODE_TT_READ	(2<<2)	/* transfer type - read */
#define W83C553F_MODE_TT_ILLEGAL	(3<<2)	/* transfer type - illegal */
#define W83C553F_MODE_CH0SEL	(0<<0)	/* channel 0 (4) select */
#define W83C553F_MODE_CH1SEL	(1<<0)	/* channel 1 (5) select */
#define W83C553F_MODE_CH2SEL	(2<<0)	/* channel 2 (6) select */
#define W83C553F_MODE_CH3SEL	(3<<0)	/* channel 3 (7) select */

/* request register bit definitions */

#define W83C553F_REQ_CHSERREQ	(1<<2)	/* channel service request */
#define W83C553F_REQ_CH0SEL	(0<<0)	/* channel 0 (4) select */
#define W83C553F_REQ_CH1SEL	(1<<0)	/* channel 1 (5) select */
#define W83C553F_REQ_CH2SEL	(2<<0)	/* channel 2 (6) select */
#define W83C553F_REQ_CH3SEL	(3<<0)	/* channel 3 (7) select */

/* write single mask bit register bit definitions */

#define W83C553F_WSMB_CHMASKSEL	(1<<2)	/* channel mask select */
#define W83C553F_WSMB_CH0SEL	(0<<0)	/* channel 0 (4) select */
#define W83C553F_WSMB_CH1SEL	(1<<0)	/* channel 1 (5) select */
#define W83C553F_WSMB_CH2SEL	(2<<0)	/* channel 2 (6) select */
#define W83C553F_WSMB_CH3SEL	(3<<0)	/* channel 3 (7) select */

/* read/write all mask bits register bit definitions */

#define W83C553F_RWAMB_CH0MASK	(1<<0)	/* channel 0 (4) mask */
#define W83C553F_RWAMB_CH1MASK	(1<<1)	/* channel 1 (5) mask */
#define W83C553F_RWAMB_CH2MASK	(1<<2)	/* channel 2 (6) mask */
#define W83C553F_RWAMB_CH3MASK	(1<<3)	/* channel 3 (7) mask */

/* typedefs */

#define W83C553F_DMA1_CS		0x8
#define W83C553F_DMA1_WR		0x9
#define W83C553F_DMA1_WSMB		0xA
#define W83C553F_DMA1_WM		0xB
#define W83C553F_DMA1_CBP		0xC
#define W83C553F_DMA1_MC		0xD
#define W83C553F_DMA1_CM		0xE
#define W83C553F_DMA1_RWAMB		0xF

#define W83C553F_DMA2_CS		0xD0
#define W83C553F_DMA2_WR		0xD2
#define W83C553F_DMA2_WSMB		0xD4
#define W83C553F_DMA2_WM		0xD6
#define W83C553F_DMA2_CBP		0xD8
#define W83C553F_DMA2_MC		0xDA
#define W83C553F_DMA2_CM		0xDC
#define W83C553F_DMA2_RWAMB		0xDE

void initialise_w83c553f(void);
