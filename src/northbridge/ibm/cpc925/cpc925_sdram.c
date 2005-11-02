#include "ppc970.h"

unsigned long sdram_size(void)
{
  unsigned long addr1, addr2;

  addr1=inint(NB_SDRAM_BASE+NB_SDRAM_MEMMODE7)&SDRAM_MEMMODE_BASEBANKADDR;
  addr1=addr1<<11;
  addr2=inint(NB_SDRAM_BASE+NB_SDRAM_MEMBOUNDAD7)&SDRAM_MEMBOUNDAD_BASEBANKADDR;
  addr2=addr2<<3;
  return(addr1|addr2);

}
