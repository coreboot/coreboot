#include "forgotten.h"
#include <../../dram/dram.h>

u16 ddr3_get_mr0(
	char precharge_pd,
	u8 write_recovery,
	char dll_reset,
	char mode,
	u8 cas,
	char interleaved_burst,
	u8 burst_lenght
){
	u32 cmd = 0;
	if(precharge_pd) cmd |= (1 << 12);
	/* Write recovery */
	cmd |= ( ((write_recovery - 4) & 0x7) << 9);
	if(dll_reset) cmd |= (1 << 8);
	if(mode) cmd |= (1<<7);
	/* CAS latency) */
	cmd |= ( ((cas - 4) & 0x7) << 4);
	if(interleaved_burst) cmd |= (1 << 3);
	/* Burst lenght */
	cmd |= (burst_lenght & 0x3);
	return cmd;
}

u16 ddr3_get_mr1(
	char q_off_disable,
	char tdqs,
	u8 rtt_nom,
	char write_leveling,
	u8 output_drive_strenght,
	u8 additive_latency,
	u8 dll_disable
){
	u32 cmd = 0;
	if(q_off_disable) cmd |= (1 << 12);
	if(tdqs) cmd |= (1 << 11);
	/* rtt_nom */
	cmd |= ( ((rtt_nom & 4) << (9-2)) | ((rtt_nom & 2) << (6-1))
			| ((rtt_nom & 1) << 2));
	if(write_leveling) cmd |= (1 << 7);
	/* output drive strenght */
	cmd |= ( ((output_drive_strenght & 2) << (5-1))
	| ((output_drive_strenght & 1) << 1) );
	/* Additive latency */
	cmd |= ((additive_latency & 0x3) << 3);
	if(dll_disable) cmd |= (1 << 0);
	return cmd;
}

u16 ddr3_get_mr2(
	u8 rtt_wr,
	char extended_temp,
	char auto_self_refresh,
	u8 cas_write
){
	u32 cmd = 0;
	/* Rtt_wr */
	cmd |= ( (rtt_wr & 0x3) << 9);
	if(extended_temp) cmd |= (1 << 7);
	if(auto_self_refresh) cmd |= (1 << 6);
	/* CAS write latency */
	cmd |= ( ((cas_write - 5) & 0x7) << 3);
	return cmd;
}

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
	
	/* Do not fuck with registers 0x52 and 0x53 if you want the following
	 * mappings to work for you:
	 * A17 <-> BA0, A18 <-> BA1, A19 <-> BA2 */
	addr |= ((mrs_type & 0x7) << 17);
	return addr;
}
void vx900_dram_ddr3_init_rank(device_t mcu, const ramctr_timing *ctrl,
			       int rank);
void vx900_dram_ddr3_init_rank(device_t mcu, const ramctr_timing *ctrl,
			       int rank)
{
	u8 reg8;
	u32 reg32, cmd, res, addr;
	int i;
	
	printram("Initializing rank %u\n", rank);
	
	/* Step 06 - Set Fun3_RX6B[2:0] to 001b (NOP Command Enable). */
	reg8 = pci_read_config8(mcu, 0x6b);
	reg8 &= ~(0x07);
	reg8 |= (1<<0);
	pci_write_config8(mcu, 0x6b, reg8);
	/* Step 07 - Read a double word from any address of the DIMM. */
	reg32 = volatile_read(0x0);
	udelay(10);
	printram("We just read   0x%x\n", reg32);
	/* Step 08 - Set Fun3_RX6B[2:0] to 011b (MSR Enable). */
	reg8 = pci_read_config8(mcu, 0x6b);
	reg8 &= ~(0x07);
	reg8 |= (3<<0);
	pci_write_config8(mcu, 0x6b, reg8);
	
	/* Step 09 – Issue MR2 cycle. Read a double word from the address depended
	 * on DRAM’s Rtt_WR and CWL settings.
	 * (Check the byte 63 of SPD. If memory address lines of target physical
	 * rank is mirrored, MA3~MA8 and BA0~BA1 should be swapped.) */
	cmd = ddr3_get_mr2(DDR3_MR2_RTT_WR_OFF,0,0,ctrl->CWL);
	addr = vx900_get_mrs_addr(2, cmd);
	if(addr != 0x00040040){
		printram("MR2 needed at addr 0x%.8x, but done at 0x%.8x\n", 0x00040040, addr);
	}
	res = volatile_read(addr);
	udelay(10);
	printram("MR2 cycle 0x%.4x @ addr 0x%.8x read 0x%.8x\n", cmd, addr, res);
	
	/* Step 10 – Issue MR3 cycle - set DRAM to normal operation mode.
	 */
	cmd = ddr3_get_mr3(0);
	addr = vx900_get_mrs_addr(3, cmd);
	res = volatile_read(addr);
	udelay(10);
	printram("MR3 cycle 0x%.4x @ addr 0x%.8x read 0x%.8x\n", cmd, addr, res);
	
	/* Step 11 –Issue MR1 cycle
	 * Set DRAM’s output driver impedance, and Rtt_Nom settings.
	 * * The DLL enable field, TDQS field, write leveling enable field,
	 *   additive latency field, and Qoff field should be set to 0.
	 */
	cmd = ddr3_get_mr1(DDR3_MR1_QOFF_ENABLE, DDR3_MR1_TQDS_DISABLE,
			   DDR3_MR1_RTT_NOM_RZQ2,
			   DDR3_MR1_WRITE_LEVELING_DISABLE, DDR3_MR1_ODS_RZQ7,
			   DDR3_MR1_AL_DISABLE, DDR3_MR1_DLL_ENABLE);
	addr = vx900_get_mrs_addr(1, cmd);
	res = volatile_read(addr);
	udelay(10);
	printram("MR1 cycle 0x%.4x @ addr 0x%.8x read 0x%.8x\n", cmd, addr, res);
	
	/* Step 12 - Issue MR0 cycle.
	 * Set DRAM’s burst length, CAS latency and write recovery settings.
	 * * The read burst type field should be set to interleave.
	 * * The mode field should be set to normal mode.
	 * * The DLL reset field should be set to No.
	 * * The DLL control for precharge PD field should be set to Fast exit.
	 */
	cmd = ddr3_get_mr0(DDR3_MR0_PRECHARGE_FAST, ctrl->WR,
			   DDR3_MR0_DLL_RESET_NO, DDR3_MR0_MODE_NORMAL,
			   ctrl->CAS, DDR3_MR0_BURST_TYPE_INTERLEAVED,
			   DDR3_MR0_BURST_LENGTH_CHOP);
	addr = vx900_get_mrs_addr(0, cmd);
	if(addr != 0x000069c8){
		printram("MR0 needed at addr 0x%.8x, but done at 0x%.8x\n", 0x000069c8, addr);
		addr = 0x000069c8;
	}
	res = volatile_read(addr);
	printram("MR0 cycle 0x%.4x @ addr 0x%.8x read 0x%.8x\n", cmd, addr, res);
	
	/* Step 13 - Set Rx6B[2:0] to 110b (Long ZQ calibration command). */
	reg8 = pci_read_config8(mcu, 0x6b);
	reg8 &= ~(0x07);
	reg8 |= (3<<1);
	pci_write_config8(mcu, 0x6b, reg8);
	for(i = 0; i < 1000; i++) inb(0x80);
	
	
	/* Step 14 - Read a double word from any address of the DIMM. */
	reg32 = volatile_read(0x0);
	printram("We just read   0x%x\n", reg32);
}

