#include <subr.h>
#include <arch/pirq_routing.h>

// BROKEN, I THINK
const struct irq_routing_table intel_irq_routing_table = { 
0x52495024,	/* u32 signature */
0x100,	/* u16 version */
208,  	/* u16 size */
0x0,	/*  u8 rtr_bus   - router bus */
0xf8,	/*  u8 rtr_devfn      - router devfn */
0xe20,	/* u16 exclusive_irqs - mask of IRQs for PCI use */
0x8086,	/* u16 rtr_vendor     - router vendor id */
0x7000,	/* u16 rtr_devfn      - router device id */
0,  	/*  u8 miniport_data  - crap */
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */ 
19, 	/*  u8 checksum. sum of all bytes mod 256 must be 0
 */
{
  /* for each slot.. */ 
  { /* slot 0 */ 
    0x0, 	/*  u8 bus */
    0x10, 	/* u16 devfn */
    {
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x61, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x63, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x1, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 1 */ 
    0x2, 	/*  u8 bus */
    0x40, 	/* u16 devfn */
    {
      {
         0x68, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6b, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x2, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 2 */ 
    0x2, 	/*  u8 bus */
    0x0, 	/* u16 devfn */
    {
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x3, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 3 */ 
    0x2, 	/*  u8 bus */
    0x8, 	/* u16 devfn */
    {
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x4, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 4 */ 
    0x2, 	/*  u8 bus */
    0x10, 	/* u16 devfn */
    {
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x5, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 5 */ 
    0x2, 	/*  u8 bus */
    0x18, 	/* u16 devfn */
    {
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x6, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 6 */ 
    0x2, 	/*  u8 bus */
    0x20, 	/* u16 devfn */
    {
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x7, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 7 */ 
    0x2, 	/*  u8 bus */
    0x28, 	/* u16 devfn */
    {
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x8, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 8 */ 
    0x2, 	/*  u8 bus */
    0x50, 	/* u16 devfn */
    {
      {
         0x6a, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x69, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x9, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 9 */ 
    0x0, 	/*  u8 bus */
    0xf8, 	/* u16 devfn */
    {
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x61, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x6b, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x63, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x0, 	/* slot */
    0x0, 	/* rfu */
  }, 
  { /* slot 10 */ 
    0x0, 	/*  u8 bus */
    0x8, 	/* u16 devfn */
    {
      {
         0x60, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x61, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x62, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
      {
         0x63, 	/* link */ 
        0xdeb8, 	/* bitmap */ 
      },
    }, 
    0x0, 	/* slot */
    0x0, 	/* rfu */
  }, 
}
}; /* end of irq routing table */ 
