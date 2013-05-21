#include "ddr3_mrs_util.h"
//#include <../../dram/dram.h>

/*
 * MRS command structure:
 * cmd[15:0] = Address pins MA[15:0]
 * cmd[18:16] = Bank address BA[2:0]
 */
mrs_cmd_t ddr3_get_mr3(char dataflow_from_mpr)
{
	mrs_cmd_t cmd = 3 << 16;
	if(dataflow_from_mpr) cmd |= (1 << 2);
	return cmd;
}

mrs_cmd_t ddr3_mrs_swap_pins(mrs_cmd_t cmd);

/* Swap some bitties titties:
 *
 * MA3 <-> MA4
 * MA5 <-> MA6
 * MA7 <-> MA8
 * BA0 <-> BA1
 */
mrs_cmd_t ddr3_mrs_swap_pins(mrs_cmd_t cmd)
{
	u32 downshift, upshift;
	/* High bits=    A4    |    A6    |    A8    |    BA1 */
	/* Low bits =    A3    |    A5    |    A7    |    BA0 */
	u32 lowbits = (1 << 3) | (1 << 5) | (1 << 7) | (1 << 16);
	downshift = (cmd & (lowbits << 1));
	upshift = (cmd & lowbits);
	cmd &= ~(lowbits | (lowbits << 1));
	cmd |= (downshift >> 1) | (upshift << 1);
	return cmd;
}

/*
 * Translate the MRS command into the memory address corresponding to the
 * command. This is based on the CPU address to memory address mapping described
 * by the initial values of registers  0x52 and 0x53, so do not fuck with them
 * until after the MRS commands have been sent to all ranks
 */

u32 vx900_get_mrs_addr(mrs_cmd_t cmd);
/*
 * Translate the MRS command into an address on the CPU bus
 *
 * Take an MRS command (mrs_cmd_t) and translate it to a read address on the CPU
 * bus. Thus, reading from the returned address, will issue the correct MRS
 * command.
 *
 * A read from the returned address will produce the correct MRS command
 * provided the following conditions are met:
 *  - The MA pin mapping is set to VX900_MRS_MA_MAP
 *  - The memory controller's Fun3_RX6B[2:0] is set to 011b (MSR Enable)
 */
u32 vx900_get_mrs_addr(mrs_cmd_t cmd)
{
	u32 addr = 0;
	u8 mrs_type = (cmd >> 16) & 0x07;
	/* MA[9:0] <-> A[12:3] */
	addr |= ((cmd & 0x3ff) << 3);
	/* MA10 <-> A20 */
	addr |= (((cmd >> 10) & 0x1) << 20);
	/* MA[12:11] <-> A[14:13] */
	addr |= (((cmd >> 11) & 0x3) << 13);
	/* BA[2:0] <-> A[19:17] */
	addr |= mrs_type << 17;
	return addr;
}
