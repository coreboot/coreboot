#include "forgotten.h"
//#include <../../dram/dram.h>

u16 ddr3_get_mr3(char dataflow_from_mpr)
{
	u32 cmd = 0;
	if(dataflow_from_mpr) cmd |= (1<<2);
	return cmd;
}

/*
 * Translate the MRS command into the memory address corresponding to the
 * command. This is based on the CPU address to memory address mapping described
 * by the initial values of registers  0x52 and 0x53, so do not fuck with them
 * until after the MRS commands have been sent to all ranks
 */

u32 vx900_get_mrs_addr(u8 mrs_type, u16 cmd);

u32 vx900_get_mrs_addr(u8 mrs_type, u16 cmd)
{
	u32 addr = 0;
	/* A3 <-> MA0, A4 <-> MA1, ... A12 <-> MA9 */
	addr |= ((cmd &0x3ff)<< 3);
	/* A20 <-> MA10 */
	addr |= (((cmd >> 10) & 0x1) << 20);
	/* A13 <-> MA11, A14 <-> MA12 */
	addr |= (((cmd >> 11) & 0x3) << 13);
	/* A17 <-> BA0, A18 <-> BA1, A19 <-> BA2 */
	addr |= ((mrs_type & 0x7) << 17);
	return addr;
}
