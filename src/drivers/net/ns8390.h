/**************************************************************************
ETHERBOOT -  BOOTP/TFTP Bootstrap Program

Author: Martin Renters
  Date: Jun/94

**************************************************************************/

//FILE_LICENCE ( BSD2 );

#define VENDOR_NONE	0
#define VENDOR_WD	1
#define VENDOR_NOVELL	2
#define VENDOR_3COM	3

#define FLAG_PIO	0x01
#define FLAG_16BIT	0x02
#define FLAG_790	0x04

#define MEM_8192	32
#define MEM_16384	64
#define MEM_32768	128

#define	ISA_MAX_ADDR	0x400

/**************************************************************************
NE1000/2000 definitions
**************************************************************************/
#define NE_ASIC_OFFSET	0x10
#define NE_RESET	0x0F		/* Used to reset card */
#define NE_DATA		0x00		/* Used to read/write NIC mem */

#define COMPEX_RL2000_TRIES	200

/**************************************************************************
8390 Register Definitions
**************************************************************************/
#define D8390_P0_COMMAND	0x00
#define D8390_P0_PSTART		0x01
#define D8390_P0_PSTOP		0x02
#define D8390_P0_BOUND		0x03
#define D8390_P0_TSR		0x04
#define	D8390_P0_TPSR		0x04
#define D8390_P0_TBCR0		0x05
#define D8390_P0_TBCR1		0x06
#define D8390_P0_ISR		0x07
#define D8390_P0_RSAR0		0x08
#define D8390_P0_RSAR1		0x09
#define D8390_P0_RBCR0		0x0A
#define D8390_P0_RBCR1		0x0B
#define D8390_P0_RSR		0x0C
#define D8390_P0_RCR		0x0C
#define D8390_P0_TCR		0x0D
#define D8390_P0_DCR		0x0E
#define D8390_P0_IMR		0x0F
#define D8390_P1_COMMAND	0x00
#define D8390_P1_PAR0		0x01
#define D8390_P1_PAR1		0x02
#define D8390_P1_PAR2		0x03
#define D8390_P1_PAR3		0x04
#define D8390_P1_PAR4		0x05
#define D8390_P1_PAR5		0x06
#define D8390_P1_CURR		0x07
#define D8390_P1_MAR0		0x08

#define D8390_COMMAND_PS0	0x0		/* Page 0 select */
#define D8390_COMMAND_PS1	0x40		/* Page 1 select */
#define D8390_COMMAND_PS2	0x80		/* Page 2 select */
#define	D8390_COMMAND_RD2	0x20		/* Remote DMA control */
#define D8390_COMMAND_RD1	0x10
#define D8390_COMMAND_RD0	0x08
#define D8390_COMMAND_TXP	0x04		/* transmit packet */
#define D8390_COMMAND_STA	0x02		/* start */
#define D8390_COMMAND_STP	0x01		/* stop */

#define D8390_RCR_MON		0x20		/* monitor mode */

#define D8390_DCR_FT1		0x40
#define D8390_DCR_LS		0x08		/* Loopback select */
#define D8390_DCR_WTS		0x01		/* Word transfer select */

#define D8390_ISR_PRX		0x01		/* successful recv */
#define D8390_ISR_PTX		0x02		/* successful xmit */
#define D8390_ISR_RXE		0x04		/* receive error */
#define D8390_ISR_TXE		0x08		/* transmit error */
#define D8390_ISR_OVW		0x10		/* Overflow */
#define D8390_ISR_CNT		0x20		/* Counter overflow */
#define D8390_ISR_RDC		0x40		/* Remote DMA complete */
#define D8390_ISR_RST		0x80		/* reset */

#define D8390_RSTAT_PRX		0x01		/* successful recv */
#define D8390_RSTAT_CRC		0x02		/* CRC error */
#define D8390_RSTAT_FAE		0x04		/* Frame alignment error */
#define D8390_RSTAT_OVER	0x08		/* FIFO overrun */

#define D8390_TXBUF_SIZE	6
#define D8390_RXBUF_END		32
#define D8390_PAGE_SIZE         256

struct ringbuffer {
	unsigned char status;
	unsigned char next;
	unsigned short len;
};
/*
 * Local variables:
 *  c-basic-offset: 8
 * End:
 */

