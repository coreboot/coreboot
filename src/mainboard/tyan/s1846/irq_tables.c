#include <arch/pirq_routing.h>

#define CHECKSUM 218
// #define PIIX4_DEVFN 0x38

/* Routing table for tyan s1846 */ 

const struct irq_routing_table intel_irq_routing_table = { 

PIRQ_SIGNATURE,     /* u32 signature */
PIRQ_VERSION,     /* u16 version */
176,       /* u16 size .. 32 + 16 * 9 = 176 */
0x0,     /*  u8 rtr_bus   - router bus */
PIIX4_DEVFN,     /*  u8 rtr_devfn      - router devfn */
0x0,     /* u16 exclusive_irqs - mask of IRQs for PCI use . FIXME...*/
0x8086,     /* u16 rtr_vendor     - router vendor id (Intel) */
0x7110,     /* u16 rtr_devfn      - router device id (82371ab) */
0,       /*  u8 miniport_data  - crap */
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */ 
CHECKSUM,       /*  u8 checksum. sum of all bytes mod 256 must be 0 */
{
  /* for each slot.. */ 
  { 
    0x0, /*  u8 bus */
    0x0, /* u16 devfn */
    {
      {
         0x0, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x0,       /* slot */
    0x0,       /* rfu */
  }, 
  { 
    0x0, /*  u8 bus */
    0x8, /* u16 devfn */
    {
      {
         0x60, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x61, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x0,       /* slot */
    0x0,       /* rfu */
  }, 
  { 
    0x0, /*  u8 bus */
    0x38, /* u16 devfn */
    {
      {
         0xfe, /* link */ 
        0x4000, /* bitmap */ 
      },
      {
         0xff, /* link */ 
        0x8000, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0x0, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x0,       /* slot */
    0x0,       /* rfu */
  }, 
  { /* slot 1 */ 
    0x0, /*  u8 bus */
    0x80, /* u16 devfn */
    {
      {
         0x60, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x61, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x62, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x63, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x1,       /* slot */
    0x0,       /* rfu */
  }, 
  { /* slot 2 */ 
    0x0, /*  u8 bus */
    0x88, /* u16 devfn */
    {
      {
         0x61, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x62, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x63, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x60, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x2,       /* slot */
    0x0,       /* rfu */
  }, 
  { /* slot 3 */ 
    0x0, /*  u8 bus */
    0x90, /* u16 devfn */
    {
      {
         0x62, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x63, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x60, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x61, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x3,       /* slot */
    0x0,       /* rfu */
  }, 
  { /* slot 4 */ 
    0x0, /*  u8 bus */
    0x98, /* u16 devfn */
    {
      {
         0x63, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x60, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x61, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x62, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x4,       /* slot */
    0x0,       /* rfu */
  }, 
  { /* slot 5 */ 
    0x0, /*  u8 bus */
    0xa0, /* u16 devfn */
    {
      {
         0x60, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x61, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x62, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x63, /* link */ 
        0xdeb8, /* bitmap */ 
      },
    }, 
    0x5,       /* slot */
    0x0,       /* rfu */
  }, 
  {
    0x0, /*  u8 bus */
    0x78, /* u16 devfn */
    {
      {
         0x60, /* link */ 
        0xdeb8, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0x0, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0x0, /* bitmap */ 
      },
      {
         0x0, /* link */ 
        0x0, /* bitmap */ 
      },
    }, 
    0x0,       /* slot */
    0x0,       /* rfu */
  }, 
}

}; /* end of irq routing table */ 









