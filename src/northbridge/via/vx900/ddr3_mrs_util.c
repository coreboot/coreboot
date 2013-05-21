#include "ddr3_mrs_util.h"
//#include <../../dram/dram.h>

/*
 * MRS command structure:
 * cmd[15:0] = Address pins MA[15:0]
 * cmd[18:16] = Bank address BA[2:0]
 */

static u16 ddr3_twr_to_mr0_map(u8 twr)
{
	if ((twr >= 5) && (twr <= 8))
		return (twr - 4) << 9;

	if ((twr >= 9) && (twr <= 14))
		return (twr >> 1) << 9;

	/* tWR == 16T is [000] */
	return 0;
}


/* Map the cas latency to a bitmask for the MR0 cycle */
static u16 ddr3_cas_to_mr0_map(u8 cas)
{
	u16 mask = 0;
	/* A[6:4] are bits [2:0] of (CAS - 4) */
	mask = ((cas - 4) & 0x07) << 4;

	/* A2 is the MSB of (CAS - 4) */
	if ((cas - 4) & (1 << 4))
		mask |= (1 << 2);

	return mask;
}

mrs_cmd_t ddr3_get_mr0(enum ddr3_mr0_precharge precharge_pd,
		       u8 write_recovery,
		       enum ddr3_mr0_dll_reset dll_reset,
		       enum ddr3_mr0_mode mode,
		       u8 cas,
		       enum ddr3_mr0_burst_type burst_type,
		       enum ddr3_mr0_burst_length burst_lenght)
{
	mrs_cmd_t cmd = 0 << 16;

	if (precharge_pd == DDR3_MR0_PRECHARGE_FAST)
		cmd |= (1 << 12);

	cmd |= ddr3_twr_to_mr0_map(write_recovery);

	if (dll_reset == DDR3_MR0_DLL_RESET_YES)
		cmd |= (1 << 8);

	if (mode == DDR3_MR0_MODE_TEST)
		cmd |= (1 << 7);

	cmd |= ddr3_cas_to_mr0_map(cas);

	if (burst_type == DDR3_MR0_BURST_TYPE_INTERLEAVED)
		cmd |= (1 << 3);

	cmd |= (burst_lenght & 0x03) << 0;

	return cmd;
}

static u16 ddr3_rtt_nom_to_mr1_map(enum ddr3_mr1_rtt_nom rtt_nom)
{
	u16 mask = 0;
	/* A9 <-> rtt_nom[2] */
	if (rtt_nom & (1 << 2))
		mask |= (1 << 9);
	/* A6 <-> rtt_nom[1] */
	if (rtt_nom & (1 << 1))
		mask |= (1 << 6);
	/* A2 <-> rtt_nom[0] */
	if (rtt_nom & (1 << 0))
		mask |= (1 << 2);

	return mask;
}

static u16 ddr3_ods_to_mr1_map(enum ddr3_mr1_ods ods)
{
	u16 mask = 0;
	/* A5 <-> ods[1] */
	if (ods & (1 << 1))
		mask |= (1 << 5);
	/* A1 <-> ods[0] */
	if (ods & (1 << 0))
		mask |= (1 << 1);

	return mask;
}

mrs_cmd_t ddr3_get_mr1(enum ddr3_mr1_qoff qoff,
		       enum ddr3_mr1_tqds tqds,
		       enum ddr3_mr1_rtt_nom rtt_nom,
		       enum ddr3_mr1_write_leveling write_leveling,
		       enum ddr3_mr1_ods ods,
		       enum ddr3_mr1_additive_latency additive_latency,
		       enum ddr3_mr1_dll dll_disable)
{
	mrs_cmd_t cmd = 1 << 16;

	if (qoff == DDR3_MR1_QOFF_DISABLE)
		cmd |= (1 << 12);

	if (tqds == DDR3_MR1_TQDS_ENABLE)
		cmd |= (1 << 11);

	cmd |= ddr3_rtt_nom_to_mr1_map(rtt_nom);

	if (write_leveling == DDR3_MR1_WRLVL_ENABLE)
		cmd |= (1 << 7);

	cmd |= ddr3_ods_to_mr1_map(ods);

	cmd |= (additive_latency & 0x03) << 3;

	if (dll_disable == DDR3_MR1_DLL_DISABLE)
		cmd |= (1 << 0);

	return cmd;
}

mrs_cmd_t ddr3_get_mr2(enum ddr3_mr2_rttwr rtt_wr,
		       enum ddr3_mr2_srt_range extended_temp,
		       enum ddr3_mr2_asr self_refresh,
		       u8 cas_cwl)
{
	mrs_cmd_t cmd = 2 << 16;

	cmd |= (rtt_wr & 0x03) << 9;

	if (extended_temp == DDR3_MR2_SRT_EXTENDED)
		cmd |= (1 << 7);

	if (self_refresh == DDR3_MR2_ASR_AUTO)
		cmd |= (1 << 6);

	cmd |= ((cas_cwl - 5) & 0x07) << 3;

	return cmd;
}


mrs_cmd_t ddr3_get_mr3(char dataflow_from_mpr)
{
	mrs_cmd_t cmd = 3 << 16;

	if(dataflow_from_mpr)
		cmd |= (1 << 2);

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
