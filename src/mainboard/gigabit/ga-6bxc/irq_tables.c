
#include <subr.h>

/*
 * This table must be located between 0x000f0000 and 0x000fffff.
 * By defining it as a const it gets located in the code segment
 * and therefore inside the necessary 64K block.   -tds
 */

#define USB_DEVFN (PIIX4_DEVFN+2)
#define SUM_REST 0x00  /* ...just happens to be 0 */
#define CHECKSUM (0x00-(SUM_REST+PIIX4_DEVFN+USB_DEVFN))

// In spite of the comment below I have located this in the 
// gigabit tree until we work this all out -- RGM
/*
 * This table should work for most systems using the PIIX4
 * southbridge that have 4 PCI slots. 
 *
 * I recall that the 440GX board that Ron was using had 
 * the PIIX4 at a different location. This will effect the 
 * devfn of the router and USB controller as well as the
 * checksum.  Hopefully the defines will allow this to
 * be a bit more portable.
 *                             -tds
 */

const struct irq_routing_table intel_irq_routing_table = {
  PIRQ_SIGNATURE, /* u32 signature */
  PIRQ_VERSION,   /* u16 version   */
  32+16*5,        /* u16 size           - size of entire table struct */
  0,              /*  u8 rtr_bus        - router bus */
  PIIX4_DEVFN,    /*  u8 rtr_devfn      - router devfn */
  0x0e00,         /* u16 exclusive_irqs - mask of IRQs for PCI use */
  0x8086,         /* u16 rtr_vendor     - router vendor id */
  0x7110,         /* u16 rtr_devfn      - router device id */
  0,              /*  u8 miniport_data  - "crap" */
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
  CHECKSUM, /*  u8 checksum       - mod 256 checksum must give zero */
  /* struct irq_info slots[0] */
  {
    {
      0,          /*  u8 bus */
      USB_DEVFN,  /*  u8 devfn for USB controller */
      {
	{
	  0x00,   /*  u8 link   - IRQ line ID    */
	  0x0000, /* u16 bitmap - Available IRQs */
	},
	{
	  0x00,   /*  u8 link   - IRQ line ID    */
	  0x0000, /* u16 bitmap - Available IRQs */
	},
	{
	  0x00,   /*  u8 link   - IRQ line ID    */
	  0x0000, /* u16 bitmap - Available IRQs */
	},
	{
	  0x63,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	}
      },
      0, /*  u8 slot */
      0, /*  u8 rfu  */
    },
    {
      0,    /*  u8 bus */
      0x40, /*  u8 devfn for PCI slot 1 */
      {
	{
	  0x60,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x61,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x62,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x63,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	}
      },
      1, /*  u8 slot */
      0, /*  u8 rfu  */
    },
    {
      0,    /*  u8 bus */
      0x48, /*  u8 devfn for PCI slot 2 */
      {
	{
	  0x61,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x62,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x63,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x60,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	}
      },
      2, /*  u8 slot */
      0, /*  u8 rfu  */
    },
    {
      0,    /*  u8 bus */
      0x50, /*  u8 devfn for PCI slot 3 */
      {
	{
	  0x62,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x63,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x60,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x61,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	}
      },
      3, /*  u8 slot */
      0, /*  u8 rfu  */
    },
    {
      0,    /*  u8 bus */
      0x58, /*  u8 devfn for PCI slot 4 */
      {
	{
	  0x63,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x60,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x61,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	},
	{
	  0x62,   /*  u8 link   - IRQ line ID    */
	  0xdef8, /* u16 bitmap - Available IRQs */
	}
      },
      4, /*  u8 slot */
      0, /*  u8 rfu  */
    }
  }
};
