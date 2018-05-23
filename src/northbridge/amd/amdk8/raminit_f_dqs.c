/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 YingHai Lu
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <arch/stages.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/mtrr.h>
#include <arch/early_variables.h>
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_SB700)
#include <southbridge/amd/sb700/sb700.h>
#endif

//0: mean no debug info
#define DQS_TRAIN_DEBUG 0

static inline void print_debug_dqs(const char *str, unsigned val, unsigned level)
{
#if DQS_TRAIN_DEBUG > 0
	if (DQS_TRAIN_DEBUG > level) {
		printk(BIOS_DEBUG, "%s%x\n", str, val);
	}
#endif
}

static inline void print_debug_dqs_pair(const char *str, unsigned val, const char *str2, unsigned val2, unsigned level)
{
#if DQS_TRAIN_DEBUG > 0
	if (DQS_TRAIN_DEBUG > level) {
		printk(BIOS_DEBUG, "%s%08x%s%08x\n", str, val, str2, val2);
	}
#endif
}

static inline void print_debug_dqs_tsc(const char *str, unsigned i, unsigned val, unsigned val2, unsigned level)
{
#if DQS_TRAIN_DEBUG > 0
	if (DQS_TRAIN_DEBUG > level) {
		printk(BIOS_DEBUG, "%s[%02x]=%08x%08x\n", str, i, val, val2);
	}
#endif
}

static inline void print_debug_dqs_tsc_x(const char *str, unsigned i, unsigned val, unsigned val2)
{
	printk(BIOS_DEBUG, "%s[%02x]=%08x%08x\n", str, i, val, val2);

}

static void fill_mem_cs_sysinfo(unsigned nodeid, const struct mem_controller *ctrl, struct sys_info *sysinfo)
{

	int i;
	sysinfo->mem_base[nodeid] = pci_read_config32(ctrl->f1, 0x40 + (nodeid << 3));

	for (i = 0; i < 8; i++) {
		sysinfo->cs_base[nodeid*8+i] = pci_read_config32(ctrl->f2, 0x40 + (i << 2));
	}

	sysinfo->hole_reg[nodeid] = pci_read_config32(ctrl->f1, 0xf0);

}
static unsigned Get_MCTSysAddr(const struct mem_controller *ctrl, unsigned cs_idx, struct sys_info *sysinfo)
{
	uint32_t dword;
	uint32_t mem_base;
	unsigned nodeid = ctrl->node_id;

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	uint32_t hole_reg;
#endif

	//get the local base addr of the chipselect
	dword = sysinfo->cs_base[nodeid * 8 + cs_idx];
	dword &= 0xfffffff0;

	//sys addr= node base + local cs base
	mem_base = sysinfo->mem_base[nodeid];
	mem_base &= 0xffff0000;

	dword += mem_base;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	hole_reg = sysinfo->hole_reg[nodeid];
	if (hole_reg & 1) {
		unsigned hole_startk;
		hole_startk = (hole_reg & (0xff << 24)) >> 10;
		if ((dword >= (hole_startk << 2)) && (dword < ((4*1024*1024) << 2))) {
			dword += ((4*1024*1024 - hole_startk) << 2);
		}
	}
#endif

	//add 1MB offset to avoid compat area
	dword += (1<<(20-8));

	//So final result is upper 32 bit addr

	return dword;

}

static unsigned Get_RcvrSysAddr(const struct mem_controller * ctrl, unsigned channel, unsigned cs_idx, struct sys_info *sysinfo)
{
	return Get_MCTSysAddr(ctrl, cs_idx, sysinfo);

}

static inline void enable_sse2(void)
{
	unsigned long cr4;
	cr4 = read_cr4();
	cr4 |= (1<<9);
	write_cr4(cr4);
}

static inline void disable_sse2(void)
{
	unsigned long cr4;
	cr4 = read_cr4();
	cr4 &= ~(1<<9);
	write_cr4(cr4);
}


static void set_wrap32dis(void) {
	msr_t msr;

	msr = rdmsr(0xc0010015);
	msr.lo |= (1<<17);

	wrmsr(0xc0010015, msr);

}

static void clear_wrap32dis(void) {
	msr_t msr;

	msr = rdmsr(0xc0010015);
	msr.lo &= ~(1<<17);

	wrmsr(0xc0010015, msr);

}

static void set_FSBASE(uint32_t addr_hi)
{
	msr_t msr;

	//set fs and use fs prefix to access the mem
	msr.hi = addr_hi;
	msr.lo = 0;
	wrmsr(0xc0000100, msr); //FS_BASE

}

static unsigned ChipSelPresent(const struct mem_controller *ctrl, unsigned cs_idx, struct sys_info *sysinfo)
{
	unsigned enabled;
	unsigned nodeid = ctrl->node_id;


	enabled = sysinfo->cs_base[nodeid * 8 + cs_idx];
	enabled &= 1;

	return enabled;

}

static unsigned RcvrRankEnabled(const struct mem_controller *ctrl, int channel, int cs_idx, unsigned is_Width128, struct sys_info *sysinfo)
{
	return ChipSelPresent(ctrl, cs_idx, sysinfo);
}

static void WriteLNTestPattern(unsigned addr_lo, uint8_t *buf_a, unsigned line_num)
{
	__asm__ volatile (
		"1:\n\t"
		"movdqa (%3), %%xmm0\n\t"
		"movntdq %%xmm0, %%fs:(%0)\n\t" /* xmm0 is 128 bit */
		"addl %1, %0\n\t"
		"addl %1, %3\n\t"
		"loop 1b\n\t"

		:: "a" (addr_lo), "d" (16), "c" (line_num * 4), "b"(buf_a)
	);


}

static void Write1LTestPattern(unsigned addr, unsigned p, uint8_t *buf_a, uint8_t *buf_b)
{
	uint8_t *buf;
	if (p == 1) { buf = buf_b; }
	else { buf = buf_a; }

	set_FSBASE (addr >> 24);

	WriteLNTestPattern(addr << 8, buf, 1);
}

static void Read1LTestPattern(unsigned addr)
{
	unsigned value;

	set_FSBASE(addr>>24);

	/* 1st move causes read fill (to exclusive or shared)*/
	__asm__ volatile (
		"movl %%fs:(%1), %0\n\t"
		:"=b"(value): "a" (addr << 8)
	);

}

#define DQS_PASS 0
#define DQS_FAIL 1

#define DQS_FIRST_PASS 1
#define DQS_SECOND_PASS 2

#define SB_NORCVREN 11
#define RCVREN_MARGIN 6
#define SB_SmallRCVR 13
#define SB_CHA2BRCVREN 12
#define SB_NODQSPOS  14
#define MIN_DQS_WNDW 3
#define SB_SMALLDQS 15


static unsigned CompareTestPatternQW0(unsigned channel, unsigned addr, unsigned pattern, const uint32_t *TestPattern0, const uint32_t *TestPattern1, const uint32_t *TestPattern2, unsigned Pass, unsigned is_Width128)
{
	uint32_t addr_lo;
	uint32_t *test_buf;
	uint32_t value;
	uint32_t value_test;
	unsigned result = DQS_FAIL;

	if (Pass == DQS_FIRST_PASS) {
		if (pattern == 1) {
			test_buf = (uint32_t *)TestPattern1;
		}
		else {
			test_buf = (uint32_t *)TestPattern0;
		}
	}
	else {
		test_buf = (uint32_t *)TestPattern2;
	}

	set_FSBASE(addr >> 24);

	addr_lo = addr << 8;

	if (is_Width128 && (channel == 1)) {
		addr_lo += 8; //second channel
		test_buf += 2;
	}

	__asm__ volatile (
		"movl %%fs:(%1), %0\n\t"
		:"=b"(value): "a" (addr_lo)
	);

	value_test = *test_buf;


	print_debug_dqs_pair("\t\t\t\t\t\tQW0.lo : test_buf= ", (unsigned)test_buf, " value = ", value_test, 4);
	print_debug_dqs_pair("\t\t\t\t\t\tQW0.lo : addr_lo = ", addr_lo, " value = ", value, 4);

	if (value == value_test) {
		addr_lo += 4;
		test_buf++;
		__asm__ volatile (
			"movl %%fs:(%1), %0\n\t"
			:"=b"(value): "a" (addr_lo)
		);
		value_test = *test_buf;
		print_debug_dqs_pair("\t\t\t\t\t\tQW0.hi : test_buf= ", (unsigned)test_buf, " value = ", value_test, 4);
		print_debug_dqs_pair("\t\t\t\t\t\tQW0.hi : addr_lo = ", addr_lo, " value = ", value, 4);

		if (value == value_test) {
			result = DQS_PASS;
		}
	}

	if (Pass == DQS_SECOND_PASS) { // second pass need to be inverted
		if (result == DQS_PASS) {
			result = DQS_FAIL;
		}
		else {
			result = DQS_PASS;
		}
	}

	return result;

}

static void SetMaxAL_RcvrDly(const struct mem_controller *ctrl, unsigned dly)
{
	uint32_t reg;

	dly += (20-1); // round it
	dly /= 20; // convert from unit 50ps to 1ns

	dly += 6;


	reg = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	reg &= ~(DCH_MaxAsyncLat_MASK << DCH_MaxAsyncLat_SHIFT);
	reg |= ((dly - DCH_MaxAsyncLat_BASE) << DCH_MaxAsyncLat_SHIFT);
	pci_write_config32(ctrl->f2, DRAM_CONFIG_HIGH, reg);

}

/*
	Set the Target range to WT IO (using an IORR overlapping the already existing
	WB dram type). Use IORR0
*/
static void SetTargetWTIO(unsigned addr)
{
	msr_t msr;
	msr.hi = addr >> 24;
	msr.lo = addr << 8;
	wrmsr(0xc0010016, msr); //IORR0 BASE

	msr.hi = 0xff;
	msr.lo = 0xfc000800; // 64MB Mask
	wrmsr(0xc0010017, msr); // IORR0 Mask
}

static void ResetTargetWTIO(void)
{
	msr_t msr;

	msr.hi = 0;
	msr.lo = 0;
	wrmsr(0xc0010017, msr); // IORR0 Mask
}

static void proc_CLFLUSH(unsigned addr)
{

	set_FSBASE(addr>>24);

	/* 1st move causes read fill (to exclusive or shared)*/
	__asm__ volatile (
			/* clflush fs:[eax] */
		"clflush %%fs:(%0)\n\t"
		::"a" (addr << 8)
	);

}
static void proc_IOCLFLUSH(unsigned addr)
{
	SetTargetWTIO(addr);
	proc_CLFLUSH(addr);
	ResetTargetWTIO();
}

static void ResetDCTWrPtr(const struct mem_controller *ctrl)
{
	uint32_t dword;
	unsigned index = 0x10;

	dword = pci_read_config32_index_wait(ctrl->f2, 0x98, index);
	pci_write_config32_index_wait(ctrl->f2, 0x98, index, dword);

	index += 0x20;
	dword = pci_read_config32_index_wait(ctrl->f2, 0x98, index);
	pci_write_config32_index_wait(ctrl->f2, 0x98, index, dword);

}


static uint16_t get_exact_T1000(unsigned i)
{
	//				   200   266,   333,  400
	static const uint16_t T1000_a[]= { 5000, 3759, 3003, 2500 };

	static const uint16_t TT_a[] = {
		 /*200   266   333   400 */
	 /*4 */   6250, 6250, 6250, 6250,
	 /*5 */   5000, 5000, 5000, 2500,
	 /*6 */   5000, 4166, 4166, 2500,
	 /*7 */   5000, 4285, 3571, 2500,

	 /*8 */   5000, 3750, 3125, 2500,
	 /*9 */   5000, 3888, 3333, 2500,
	 /*10*/   5000, 4000, 3000, 2500,
	 /*11*/   5000, 4090, 3181, 2500,

	 /*12*/   5000, 3750, 3333, 2500,
	 /*13*/   5000, 3846, 3076, 2500,
	 /*14*/   5000, 3928, 3214, 2500,
	 /*15*/   5000, 4000, 3000, 2500,
	};

	int index;
	msr_t msr;

	/* Check for FID control support */
	struct cpuid_result cpuid1;
	cpuid1 = cpuid(0x80000007);
	if (cpuid1.edx & 0x02) {
		/* Use current FID */
		unsigned fid_cur;
		msr = rdmsr(0xc0010042);
		fid_cur = msr.lo & 0x3f;

		index = fid_cur>>1;
	} else {
		/* Use startup FID */
		unsigned fid_start;
		msr = rdmsr(0xc0010015);
		fid_start = (msr.lo & (0x3f << 24));

		index = fid_start>>25;
	}

	if (index > 12) return T1000_a[i];

	return TT_a[index * 4+i];

}

static void InitDQSPos4RcvrEn(const struct mem_controller *ctrl)
{
	int i;
	uint32_t dword;

	dword = 0x00000000;
	for (i = 1; i <= 3; i++) {
		/* Program the DQS Write Timing Control Registers (Function 2:Offset 0x9c, index 0x01-0x03, 0x21-0x23) to 0x00 for all bytes */
		pci_write_config32_index_wait(ctrl->f2, 0x98, i, dword);
		pci_write_config32_index_wait(ctrl->f2, 0x98, i+0x20, dword);
	}

	dword = 0x2f2f2f2f;
	for (i = 5; i <= 7; i++) {
		/* Program the DQS Write Timing Control Registers (Function 2:Offset 0x9c, index 0x05-0x07, 0x25-0x27) to 0x2f for all bytes */
		pci_write_config32_index_wait(ctrl->f2, 0x98, i, dword);
		pci_write_config32_index_wait(ctrl->f2, 0x98, i+0x20, dword);
	}


}

static unsigned TrainRcvrEn(const struct mem_controller *ctrl, unsigned Pass, struct sys_info *sysinfo)
{

	static const uint32_t TestPattern0[] = {
			0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
			0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
			0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
			0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
		};
	static const uint32_t TestPattern1[] = {
			0x55555555, 0x55555555, 0x55555555, 0x55555555,
			0x55555555, 0x55555555, 0x55555555, 0x55555555,
			0x55555555, 0x55555555, 0x55555555, 0x55555555,
			0x55555555, 0x55555555, 0x55555555, 0x55555555,
		};
	static const uint32_t TestPattern2[] = {
			0x12345678, 0x87654321, 0x23456789, 0x98765432,
			0x59385824, 0x30496724, 0x24490795, 0x99938733,
			0x40385642, 0x38465245, 0x29432163, 0x05067894,
			0x12349045, 0x98723467, 0x12387634, 0x34587623,
		};

	uint8_t pattern_buf_x[64 * 4 + 16]; // We need to two cache line So have more 16 bytes to keep 16 byte alignment */
	uint8_t *buf_a, *buf_b;
	uint32_t ecc_bit;
	uint32_t dword;
	uint8_t *dqs_rcvr_dly_a = &sysinfo->dqs_rcvr_dly_a[ctrl->node_id * 2* 8]; //8 node, channel 2, receiver 8

	int i;

	unsigned channel, receiver;

	unsigned Errors;
	unsigned CTLRMaxDelay;
	unsigned T1000;

	unsigned LastTest;
	unsigned CurrTest;
	unsigned Test0, Test1;

	unsigned RcvrEnDlyRmin;

	unsigned two_ranks;
	unsigned RcvrEnDly;

	unsigned PatternA;
	unsigned PatternB;

	unsigned TestAddr0, TestAddr0B, TestAddr1 = 0, TestAddr1B = 0;

	unsigned CurrRcvrCHADelay = 0;

	unsigned tmp;

	unsigned is_Width128 = sysinfo->meminfo[ctrl->node_id].is_Width128;

#if K8_REV_F_SUPPORT_F0_F1_WORKAROUND == 1
	unsigned cpu_f0_f1 = 0;
#endif

	if (Pass == DQS_FIRST_PASS) {
		InitDQSPos4RcvrEn(ctrl);
	}

	//enable SSE2
	enable_sse2();

	//wrap32dis
	set_wrap32dis();

	//disable ECC temp
	dword = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	ecc_bit = dword & DCL_DimmEccEn;
	dword &= ~(DCL_DimmEccEn);
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dword);


	if (Pass == DQS_FIRST_PASS) {
#if K8_REV_F_SUPPORT_F0_F1_WORKAROUND == 1
	cpu_f0_f1 = is_cpu_pre_f2_in_bsp(ctrl->node_id);
	if (!cpu_f0_f1)
#endif
	{
#if 1
		/* Set the DqsRcvEnTrain bit */
		dword = pci_read_config32(ctrl->f2, DRAM_CTRL);
		dword |= DC_DqsRcvEnTrain;
		pci_write_config32(ctrl->f2, DRAM_CTRL, dword);
#endif
	}
	}

	//get T1000 figures (cycle time (ns)) * 1K
	dword = pci_read_config32(ctrl->f2, DRAM_CONFIG_HIGH);
	dword &= DCH_MemClkFreq_MASK;

	T1000 = get_exact_T1000(dword);

	// SetupRcvrPattern
	buf_a = (uint8_t *)(((uint32_t)(&pattern_buf_x[0]) + 0x10) & (0xfffffff0));
	buf_b = buf_a + 128; //??
	if (Pass == DQS_FIRST_PASS) {
		for (i = 0; i < 16; i++) {
			*((uint32_t *)(buf_a + i*4)) = TestPattern0[i];
			*((uint32_t *)(buf_b + i*4)) = TestPattern1[i];
		}
	}
	else {
		for (i = 0; i < 16; i++) {
			*((uint32_t *)(buf_a + i*4)) = TestPattern2[i];
			*((uint32_t *)(buf_b + i*4)) = TestPattern2[i];
		}
	}

	print_debug_dqs("\nTrainRcvEn: 0 ctrl", ctrl->node_id, 0);

	print_debug_addr("TrainRcvEn: buf_a:", buf_a);

	Errors = 0;
	/* for each channel */
	CTLRMaxDelay = 0;
	channel = 0;

	if (!(sysinfo->meminfo[ctrl->node_id].dimm_mask & 0x0F) &&
	     (sysinfo->meminfo[ctrl->node_id].dimm_mask & 0xF0)) { /* channelB only? */
		channel = 1;
	}

	for (; (channel < 2) && (!Errors); channel++)
	{
		print_debug_dqs("\tTrainRcvEn51: channel ",channel, 1);

		/* for each rank */
		/* there are four receiver pairs, loosely associated with CS */
		for (receiver = 0; (receiver < 8) && (!Errors); receiver+=2)
		{

			unsigned index=(receiver>>1) * 3 + 0x10;

			print_debug_dqs("\t\tTrainRcvEn52: index ", index, 2);

			if (is_Width128) {
				if (channel) {
					dword = pci_read_config32_index_wait(ctrl->f2, 0x98, index);
					CurrRcvrCHADelay= dword & 0xff;
				}
			}
			else {
				if (channel) {
					index += 0x20;
				}
			}

			LastTest = DQS_FAIL;
			RcvrEnDlyRmin = 0xaf;

			if (!RcvrRankEnabled(ctrl, channel, receiver, is_Width128, sysinfo)) continue;

			/* for each DQS receiver enable setting */

			TestAddr0 = Get_RcvrSysAddr(ctrl, channel, receiver, sysinfo);

			TestAddr0B = TestAddr0 + (1<<(20+2-8)); // 4MB

			if (RcvrRankEnabled(ctrl, channel, receiver+1, is_Width128, sysinfo)) {
				TestAddr1 = Get_RcvrSysAddr(ctrl, channel, receiver+1, sysinfo);
				TestAddr1B = TestAddr1 + (1<<(20+2-8)); //4MB
				two_ranks = 1;
			}
			else {
				two_ranks = 0;
			}

			print_debug_dqs("\t\tTrainRcvEn53: TestAddr0B ", TestAddr0B, 2);

			Write1LTestPattern(TestAddr0, 0, buf_a, buf_b); // rank0 of dimm, test p0
			Write1LTestPattern(TestAddr0B, 1, buf_a, buf_b); //rank0 of dimm, test p1

			if (two_ranks == 1) {
				Write1LTestPattern(TestAddr1, 0, buf_a, buf_b); //rank 1 of dimm
				Write1LTestPattern(TestAddr1B, 1, buf_a, buf_b);//rank 1 of dimm
			}

			if (Pass == DQS_FIRST_PASS) {
				RcvrEnDly = 0;
			} else {
				RcvrEnDly = dqs_rcvr_dly_a[channel * 8 + receiver];
			}

			while (RcvrEnDly < 0xaf) { // Sweep Delay value here
				print_debug_dqs("\t\t\tTrainRcvEn541: RcvrEnDly ", RcvrEnDly, 3);

				if (RcvrEnDly & 1) {
					/* Odd steps get another pattern such that even
					 * and odd steps alternate.
					 * The pointers to the patterns will be swapped
					 * at the end of the loop so they are correspond
					 */
					PatternA = 1;
					PatternB = 0;
				}
				else {
					/* Even step */
					PatternA = 0;
					PatternB = 1;
				}

				/* Program current Receiver enable delay */
				pci_write_config32_index_wait(ctrl->f2, 0x98, index, RcvrEnDly);
				/* FIXME: 64bit MUX */

				if (is_Width128) {
					/* Program current Receiver enable delay channel b */
					pci_write_config32_index_wait(ctrl->f2, 0x98, index+ 0x20, RcvrEnDly);
				}

				/* Program the MaxAsyncLat filed with the
				 * current DQS receiver enable setting plus 6ns
				 */
				/* Program MaxAsyncLat to correspond with current delay */
				SetMaxAL_RcvrDly(ctrl, RcvrEnDly);

				CurrTest = DQS_FAIL;

				Read1LTestPattern(TestAddr0); //Cache Fill
				/* ROM vs cache compare */
				Test0 = CompareTestPatternQW0(channel, TestAddr0, PatternA, TestPattern0, TestPattern1, TestPattern2, Pass, is_Width128);
				proc_IOCLFLUSH(TestAddr0);

				ResetDCTWrPtr(ctrl);

				print_debug_dqs("\t\t\tTrainRcvEn542: Test0 ", Test0, 3);

				if (Test0 == DQS_PASS) {

					Read1LTestPattern(TestAddr0B);
					Test1 = CompareTestPatternQW0(channel, TestAddr0B, PatternB, TestPattern0, TestPattern1, TestPattern2, Pass, is_Width128);
					proc_IOCLFLUSH(TestAddr0B);

					ResetDCTWrPtr(ctrl);

					print_debug_dqs("\t\t\tTrainRcvEn543: Test1 ", Test1, 3);

					if (Test1 == DQS_PASS) {
						if (two_ranks) {
							Read1LTestPattern(TestAddr1);
							Test0 = CompareTestPatternQW0(channel, TestAddr1, PatternA, TestPattern0, TestPattern1, TestPattern2, Pass, is_Width128);
							proc_IOCLFLUSH(TestAddr1);
							ResetDCTWrPtr(ctrl);

							if (Test0 == DQS_PASS) {
								Read1LTestPattern(TestAddr1B);
								Test1 = CompareTestPatternQW0(channel, TestAddr1B, PatternB, TestPattern0, TestPattern1, TestPattern2, Pass, is_Width128);
								proc_IOCLFLUSH(TestAddr1B);
								ResetDCTWrPtr(ctrl);

								if (Test1 == DQS_PASS) {
									CurrTest = DQS_PASS;
								}
							}
							print_debug_dqs("\t\t\tTrainRcvEn544: Test0 ", Test0, 3);
						}
						else {
							CurrTest = DQS_PASS;
						}
					}
				}

				print_debug_dqs("\t\t\tTrainRcvEn55: RcvrEnDly ", RcvrEnDly, 3);

				if (CurrTest == DQS_PASS) {
					if (LastTest == DQS_FAIL) {
						RcvrEnDlyRmin = RcvrEnDly;
						break;
					}
				}

				LastTest = CurrTest;

				/* swap the rank 0 pointers */
				tmp = TestAddr0;
				TestAddr0 = TestAddr0B;
				TestAddr0B = tmp;

				/* swap the rank 1 pointers */
				tmp = TestAddr1;
				TestAddr1 = TestAddr1B;
				TestAddr1B = tmp;

				print_debug_dqs("\t\t\tTrainRcvEn56: RcvrEnDly ", RcvrEnDly, 3);

				RcvrEnDly++;

			} // while RcvrEnDly

			print_debug_dqs("\t\tTrainRcvEn61: RcvrEnDly ", RcvrEnDly, 2);

			if (RcvrEnDlyRmin == 0xaf) {
				//no passing window
				Errors |= SB_NORCVREN;
			}

			if (Pass == DQS_FIRST_PASS) {
				// We need a better value for DQSPos training
				RcvrEnDly = RcvrEnDlyRmin /* + RCVREN_MARGIN * T1000/64/50 */;
			} else {
				RcvrEnDly = RcvrEnDlyRmin;
			}

			if (RcvrEnDly > 0xae) {
				//passing window too narrow, too far delayed
				Errors |= SB_SmallRCVR;
				RcvrEnDly = 0xae;
			}

			if (Pass == DQS_SECOND_PASS) { //second pass must average vales
				RcvrEnDly += dqs_rcvr_dly_a[channel * 8 + receiver] /* - (RCVREN_MARGIN * T1000/64/50)*/;
				RcvrEnDly >>= 1;
			}

			dqs_rcvr_dly_a[channel * 8 + receiver] = RcvrEnDly;

			//Set final RcvrEnDly for this DIMM and Channel
			pci_write_config32_index_wait(ctrl->f2, 0x98, index, RcvrEnDly);

			if (is_Width128) {
				pci_write_config32_index_wait(ctrl->f2, 0x98, index+0x20, RcvrEnDly); // channel B
				if (channel) {
					pci_write_config32_index_wait(ctrl->f2, 0x98, index, CurrRcvrCHADelay);
					if (RcvrEnDly > CurrRcvrCHADelay) {
						dword = RcvrEnDly - CurrRcvrCHADelay;
					}
					else {
						dword = CurrRcvrCHADelay - RcvrEnDly;
					}
					dword *= 50;
					if (dword > T1000) {
						Errors |= SB_CHA2BRCVREN;
					}
				}
			}

			print_debug_dqs("\t\tTrainRcvEn63: RcvrEnDly ", RcvrEnDly, 2);

			if (RcvrEnDly > CTLRMaxDelay) {
				CTLRMaxDelay = RcvrEnDly;
			}

			print_debug_dqs("\t\tTrainRcvEn64: CTLRMaxDelay ", CTLRMaxDelay, 2);

		} /* receiver */
	} /* channel */

	print_debug_dqs("\tTrainRcvEn65: CTLRMaxDelay ", CTLRMaxDelay, 1);

	/* Program the MaxAsysncLat field with the largest DQS Receiver Enable setting */
	SetMaxAL_RcvrDly(ctrl, CTLRMaxDelay);
	ResetDCTWrPtr(ctrl);

	//Enable ECC again
	dword = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dword &= ~(DCL_DimmEccEn);
	dword |= ecc_bit;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dword);

	if (Pass == DQS_FIRST_PASS) {
#if K8_REV_F_SUPPORT_F0_F1_WORKAROUND == 1
	if (!cpu_f0_f1)
#endif
	{
		dword = pci_read_config32(ctrl->f2, DRAM_CTRL);
		dword &= ~DC_DqsRcvEnTrain;
		pci_write_config32(ctrl->f2, DRAM_CTRL, dword);
	}
	}

	//Clear wrap32dis

	clear_wrap32dis();

	//restore SSE2 setting
	disable_sse2();

#if CONFIG_MEM_TRAIN_SEQ != 1
	/* We need tidy output for type 1 */
	printk(BIOS_DEBUG, " CTLRMaxDelay=%02x\n", CTLRMaxDelay);
#endif

	return (CTLRMaxDelay == 0xae)?1:0;

}

#define DQS_READDIR 1
#define DQS_WRITEDIR 0


static void SetDQSDelayCSR(const struct mem_controller *ctrl, unsigned channel, unsigned bytelane, unsigned direction, unsigned dqs_delay)
{ //ByteLane could be 0-8, last is for ECC
	unsigned index;
	uint32_t dword;
	unsigned shift;

	dqs_delay &= 0xff;

	index = (bytelane>>2) + 1 + channel * 0x20 + (direction << 2);
	shift = bytelane;
	while (shift > 3) {
		shift-=4;
	}
	shift <<= 3; // 8 bit

	dword = pci_read_config32_index_wait(ctrl->f2, 0x98, index);
	dword &= ~(0x3f << shift);
	dword |= (dqs_delay << shift);
	pci_write_config32_index_wait(ctrl->f2, 0x98, index, dword);

}

static void SetDQSDelayAllCSR(const struct mem_controller *ctrl, unsigned channel, unsigned direction, unsigned dqs_delay)
{
	unsigned index;
	uint32_t dword;
	int i;

	dword = 0;
	dqs_delay &= 0xff;
	for (i = 0; i < 4; i++) {
		dword |= dqs_delay << (i*8);
	}

	index = 1 + channel * 0x20 + direction * 4;

	for (i = 0; i < 2; i++) {
		pci_write_config32_index_wait(ctrl->f2, 0x98, index + i, dword);
	}

}

static unsigned MiddleDQS(unsigned min_d, unsigned max_d)
{
	unsigned size_d;
	size_d = max_d-min_d;
	if (size_d & 1) { //need round up
		min_d++;
	}
	return (min_d + (size_d>>1));
}

static inline void save_dqs_delay(unsigned channel, unsigned bytelane, unsigned direction, uint8_t *dqs_delay_a, uint8_t dqs_delay)
{
	dqs_delay_a[channel * 2*9 + direction * 9 + bytelane] = dqs_delay;
}

static void WriteDQSTestPattern(unsigned addr_lo, unsigned pattern , uint8_t *buf_a)
{
	WriteLNTestPattern(addr_lo, buf_a, (pattern+1) * 9);
}

static void ReadL18TestPattern(unsigned addr_lo)
{
	//set fs and use fs prefix to access the mem
	__asm__ volatile (
		"movl %%fs:-128(%%esi), %%eax\n\t"	//TestAddr cache line
		"movl %%fs:-64(%%esi), %%eax\n\t"	//+1
		"movl %%fs:(%%esi), %%eax\n\t"		//+2
		"movl %%fs:64(%%esi), %%eax\n\t"	//+3

		"movl %%fs:-128(%%edi), %%eax\n\t"	//+4
		"movl %%fs:-64(%%edi), %%eax\n\t"	//+5
		"movl %%fs:(%%edi), %%eax\n\t"		//+6
		"movl %%fs:64(%%edi), %%eax\n\t"	//+7

		"movl %%fs:-128(%%ebx), %%eax\n\t"	//+8
		"movl %%fs:-64(%%ebx), %%eax\n\t"	//+9
		"movl %%fs:(%%ebx), %%eax\n\t"		//+10
		"movl %%fs:64(%%ebx), %%eax\n\t"	//+11

		"movl %%fs:-128(%%ecx), %%eax\n\t"	//+12
		"movl %%fs:-64(%%ecx), %%eax\n\t"	//+13
		"movl %%fs:(%%ecx), %%eax\n\t"		//+14
		"movl %%fs:64(%%ecx), %%eax\n\t"	//+15

		"movl %%fs:-128(%%edx), %%eax\n\t"	//+16
		"movl %%fs:-64(%%edx), %%eax\n\t"	//+17

		:: "a"(0), "b" (addr_lo+128+8*64), "c" (addr_lo+128+12*64), "d" (addr_lo +128+16*64), "S"(addr_lo+128), "D"(addr_lo+128+4*64)
	);

}

static void ReadL9TestPattern(unsigned addr_lo)
{

	//set fs and use fs prefix to access the mem
	__asm__ volatile (

		"movl %%fs:-128(%%ecx), %%eax\n\t"	//TestAddr cache line
		"movl %%fs:-64(%%ecx), %%eax\n\t"	//+1
		"movl %%fs:(%%ecx), %%eax\n\t"		//+2
		"movl %%fs:64(%%ecx), %%eax\n\t"	//+3

		"movl %%fs:-128(%%edx), %%eax\n\t"	//+4
		"movl %%fs:-64(%%edx), %%eax\n\t"	//+5
		"movl %%fs:(%%edx), %%eax\n\t"		//+6
		"movl %%fs:64(%%edx), %%eax\n\t"	//+7

		"movl %%fs:-128(%%ebx), %%eax\n\t"	//+8

		:: "a"(0), "b" (addr_lo+128+8*64), "c"(addr_lo+128), "d"(addr_lo+128+4*64)
	);

}


static void ReadDQSTestPattern(unsigned addr_lo, unsigned pattern)
{
	if (pattern == 0) {
		ReadL9TestPattern(addr_lo);
	}
	else {
		ReadL18TestPattern(addr_lo);
	}
}

static void FlushDQSTestPattern_L9(unsigned addr_lo)
{
	__asm__ volatile (
		"clflush %%fs:-128(%%ecx)\n\t"
		"clflush %%fs:-64(%%ecx)\n\t"
		"clflush %%fs:(%%ecx)\n\t"
		"clflush %%fs:64(%%ecx)\n\t"

		"clflush %%fs:-128(%%eax)\n\t"
		"clflush %%fs:-64(%%eax)\n\t"
		"clflush %%fs:(%%eax)\n\t"
		"clflush %%fs:64(%%eax)\n\t"

		"clflush %%fs:-128(%%ebx)\n\t"

		:: "b" (addr_lo+128+8*64), "c"(addr_lo+128), "a"(addr_lo+128+4*64)
	);

}
static __attribute__((noinline)) void FlushDQSTestPattern_L18(unsigned addr_lo)
{
	__asm__ volatile (
		"clflush %%fs:-128(%%eax)\n\t"
		"clflush %%fs:-64(%%eax)\n\t"
		"clflush %%fs:(%%eax)\n\t"
		"clflush %%fs:64(%%eax)\n\t"

		"clflush %%fs:-128(%%edi)\n\t"
		"clflush %%fs:-64(%%edi)\n\t"
		"clflush %%fs:(%%edi)\n\t"
		"clflush %%fs:64(%%edi)\n\t"

		"clflush %%fs:-128(%%ebx)\n\t"
		"clflush %%fs:-64(%%ebx)\n\t"
		"clflush %%fs:(%%ebx)\n\t"
		"clflush %%fs:64(%%ebx)\n\t"

		"clflush %%fs:-128(%%ecx)\n\t"
		"clflush %%fs:-64(%%ecx)\n\t"
		"clflush %%fs:(%%ecx)\n\t"
		"clflush %%fs:64(%%ecx)\n\t"

		"clflush %%fs:-128(%%edx)\n\t"
		"clflush %%fs:-64(%%edx)\n\t"

		:: "b" (addr_lo+128+8*64), "c" (addr_lo+128+12*64), "d" (addr_lo +128+16*64), "a"(addr_lo+128), "D"(addr_lo+128+4*64)
	);
}

static void FlushDQSTestPattern(unsigned addr_lo, unsigned pattern)
{

	if (pattern == 0) {
		FlushDQSTestPattern_L9(addr_lo);
	}
	else {
		FlushDQSTestPattern_L18(addr_lo);
	}
}

static unsigned CompareDQSTestPattern(unsigned channel, unsigned addr_lo, unsigned pattern, uint8_t *buf_a)
{
	uint32_t *test_buf;
	unsigned bitmap = 0xff;
	unsigned bytelane;
	int i;
	uint32_t value;
	int j;
	uint32_t value_test;

	test_buf = (uint32_t *)buf_a;


	if (pattern && channel) {
		addr_lo += 8; //second channel
		test_buf+= 2;
	}

	bytelane = 0;
	for (i = 0; i < 9*64/4; i++) {
		__asm__ volatile (
			"movl %%fs:(%1), %0\n\t"
			:"=b"(value): "a" (addr_lo)
		);
		value_test = *test_buf;

		print_debug_dqs_pair("\t\t\t\t\t\ttest_buf= ", (unsigned)test_buf, " value = ", value_test, 7);
		print_debug_dqs_pair("\t\t\t\t\t\ttaddr_lo = ",addr_lo, " value = ", value, 7);

		for (j = 0; j < 4*8; j+=8) {
			if (((value>>j)&0xff) != ((value_test>>j)& 0xff)) {
				bitmap &= ~(1 << bytelane);
			}

			bytelane++;
			bytelane &= 0x7;
		}
		print_debug_dqs("\t\t\t\t\t\tbitmap = ", bitmap, 7);

		if (bytelane == 0) {
			if (pattern == 1) { //dual channel
				addr_lo += 8; //skip over other channel's data
				test_buf += 2;
			}
		}
		addr_lo += 4;
		test_buf +=1;

	}


	return bitmap;

}

static unsigned TrainDQSPos(const struct mem_controller *ctrl, unsigned channel, unsigned Direction, unsigned Pattern, uint8_t *buf_a, uint8_t *dqs_delay_a, struct sys_info *sysinfo)
{
	unsigned ByteLane;
	unsigned Errors;
	unsigned BanksPresent;

	unsigned MutualCSPassW[48];

	unsigned ChipSel;
	unsigned DQSDelay;

	unsigned TestAddr;

	unsigned LastTest;
	unsigned RnkDlyFilterMax, RnkDlyFilterMin = 0;
	unsigned RnkDlySeqPassMax, RnkDlySeqPassMin = 0;

	Errors = 0;
	BanksPresent = 0;

	print_debug_dqs("\t\t\tTrainDQSPos begin ", 0, 3);

	printk(BIOS_DEBUG, "TrainDQSPos: MutualCSPassW[48] :%p\n", MutualCSPassW);

	for (DQSDelay = 0; DQSDelay < 48; DQSDelay++) {
		MutualCSPassW[DQSDelay] = 0xff; // Bitmapped status per delay setting, 0xff=All positions passing (1= PASS)
	}

	for (ChipSel = 0; ChipSel < 8; ChipSel++) { //logical register chipselects 0..7
		print_debug_dqs("\t\t\t\tTrainDQSPos: 11 ChipSel ", ChipSel, 4);
		//FIXME: process 64MUXedMode
		if (!ChipSelPresent(ctrl, ChipSel, sysinfo)) continue;
		BanksPresent = 1;

		TestAddr = Get_MCTSysAddr(ctrl, ChipSel, sysinfo);

		print_debug_dqs("\t\t\t\tTrainDQSPos: 12 TestAddr ", TestAddr, 4);

		//set fs and use fs prefix to access the mem
		set_FSBASE(TestAddr>>24);

		if (Direction == DQS_READDIR) {
			print_debug_dqs("\t\t\t\tTrainDQSPos: 13 for read so write at first", 0, 4);
			WriteDQSTestPattern(TestAddr << 8, Pattern, buf_a);
		}

		for (DQSDelay = 0; DQSDelay < 48; DQSDelay++) {
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 141 DQSDelay ", DQSDelay, 5);
			if (MutualCSPassW[DQSDelay] == 0) continue; //skip current delay value if other chipselects have failed all 8 bytelanes
			SetDQSDelayAllCSR(ctrl, channel, Direction, DQSDelay);
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 142 MutualCSPassW ", MutualCSPassW[DQSDelay], 5);
			if (Direction == DQS_WRITEDIR) {
				print_debug_dqs("\t\t\t\t\tTrainDQSPos: 143 for write", 0, 5);
				WriteDQSTestPattern(TestAddr << 8, Pattern, buf_a);
			}
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 144 Pattern ", Pattern, 5);
			ReadDQSTestPattern(TestAddr << 8, Pattern);
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 145 MutualCSPassW ", MutualCSPassW[DQSDelay], 5);
			MutualCSPassW[DQSDelay] &= CompareDQSTestPattern(channel, TestAddr << 8, Pattern, buf_a); //0: fail, 1=pass
			print_debug_dqs("\t\t\t\t\tTrainDQSPos: 146 MutualCSPassW ", MutualCSPassW[DQSDelay], 5);
			SetTargetWTIO(TestAddr);
			FlushDQSTestPattern(TestAddr << 8, Pattern);
			ResetTargetWTIO();
		}
	}

	if (BanksPresent)
	for (ByteLane = 0; ByteLane < 8; ByteLane++) {
		print_debug_dqs("\t\t\t\tTrainDQSPos: 31 ByteLane ",ByteLane, 4);

		LastTest = DQS_FAIL;
		RnkDlySeqPassMax = 0;
		RnkDlyFilterMax = 0;
		RnkDlyFilterMin = 0;
		for (DQSDelay = 0; DQSDelay < 48; DQSDelay++) {
			if (MutualCSPassW[DQSDelay] & (1 << ByteLane)) {

				print_debug_dqs("\t\t\t\t\tTrainDQSPos: 321 DQSDelay ", DQSDelay, 5);
				print_debug_dqs("\t\t\t\t\tTrainDQSPos: 322 MutualCSPassW ", MutualCSPassW[DQSDelay], 5);

				RnkDlySeqPassMax = DQSDelay;
				if (LastTest == DQS_FAIL) {
					RnkDlySeqPassMin = DQSDelay; //start sequential run
				}
				if ((RnkDlySeqPassMax - RnkDlySeqPassMin)>(RnkDlyFilterMax-RnkDlyFilterMin)) {
					RnkDlyFilterMin = RnkDlySeqPassMin;
					RnkDlyFilterMax = RnkDlySeqPassMax;
				}
				LastTest = DQS_PASS;
			}
			else {
				LastTest = DQS_FAIL;
			}
		}
		print_debug_dqs("\t\t\t\tTrainDQSPos: 33 RnkDlySeqPassMax ", RnkDlySeqPassMax, 4);

		if (RnkDlySeqPassMax == 0) {
			Errors |= SB_NODQSPOS; // no passing window
		}
		else {
			print_debug_dqs("\t\t\t\tTrainDQSPos: 34 RnkDlyFilterMax ", RnkDlyFilterMax, 4);
			print_debug_dqs("\t\t\t\tTrainDQSPos: 34 RnkDlyFilterMin ", RnkDlyFilterMin, 4);
			if ((RnkDlyFilterMax - RnkDlyFilterMin)< MIN_DQS_WNDW) {
				Errors |= SB_SMALLDQS;
			}
			else {
				unsigned middle_dqs;
				middle_dqs = MiddleDQS(RnkDlyFilterMin, RnkDlyFilterMax);
				print_debug_dqs("\t\t\t\tTrainDQSPos: 35 middle_dqs ",middle_dqs, 4);
				SetDQSDelayCSR(ctrl, channel, ByteLane, Direction, middle_dqs);
				save_dqs_delay(channel, ByteLane, Direction, dqs_delay_a, middle_dqs);
			}
		}

	}

	print_debug_dqs("\t\t\tTrainDQSPos: end", 0xff, 3);

	return Errors;


}

static unsigned TrainReadDQS(const struct mem_controller *ctrl, unsigned channel, unsigned pattern, uint8_t *buf_a, uint8_t *dqs_delay_a, struct sys_info *sysinfo)
{
	print_debug_dqs("\t\tTrainReadPos", 0, 2);
	return TrainDQSPos(ctrl, channel, DQS_READDIR, pattern, buf_a, dqs_delay_a, sysinfo);
}

static unsigned TrainWriteDQS(const struct mem_controller *ctrl, unsigned channel, unsigned pattern, uint8_t *buf_a, uint8_t *dqs_delay_a, struct sys_info *sysinfo)
{
	print_debug_dqs("\t\tTrainWritePos", 0, 2);
	return TrainDQSPos(ctrl, channel, DQS_WRITEDIR, pattern, buf_a, dqs_delay_a, sysinfo);
}



static unsigned TrainDQSRdWrPos(const struct mem_controller *ctrl, struct sys_info *sysinfo)
{
	static const uint32_t TestPatternJD1a[] = {
					0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFF, // QW0-1, ALL-EVEN
					0x00000000,0x00000000,0x00000000,0x00000000, // QW2-3, ALL-EVEN
					0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFF, // QW4-5, ALL-EVEN
					0x00000000,0x00000000,0x00000000,0x00000000, // QW6-7, ALL-EVEN
					0xFeFeFeFe,0xFeFeFeFe,0x01010101,0x01010101, // QW0-1, DQ0-ODD
					0xFeFeFeFe,0xFeFeFeFe,0x01010101,0x01010101, // QW2-3, DQ0-ODD
					0x01010101,0x01010101,0xFeFeFeFe,0xFeFeFeFe, // QW4-5, DQ0-ODD
					0xFeFeFeFe,0xFeFeFeFe,0x01010101,0x01010101, // QW6-7, DQ0-ODD
					0x02020202,0x02020202,0x02020202,0x02020202, // QW0-1, DQ1-ODD
					0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, // QW2-3, DQ1-ODD
					0xFdFdFdFd,0xFdFdFdFd,0x02020202,0x02020202, // QW4-5, DQ1-ODD
					0x02020202,0x02020202,0x02020202,0x02020202, // QW6-7, DQ1-ODD
					0x04040404,0x04040404,0xfBfBfBfB,0xfBfBfBfB, // QW0-1, DQ2-ODD
					0x04040404,0x04040404,0x04040404,0x04040404, // QW2-3, DQ2-ODD
					0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, // QW4-5, DQ2-ODD
					0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, // QW6-7, DQ2-ODD
					0x08080808,0x08080808,0xF7F7F7F7,0xF7F7F7F7, // QW0-1, DQ3-ODD
					0x08080808,0x08080808,0x08080808,0x08080808, // QW2-3, DQ3-ODD
					0xF7F7F7F7,0xF7F7F7F7,0x08080808,0x08080808, // QW4-5, DQ3-ODD
					0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, // QW6-7, DQ3-ODD
					0x10101010,0x10101010,0x10101010,0x10101010, // QW0-1, DQ4-ODD
					0xeFeFeFeF,0xeFeFeFeF,0x10101010,0x10101010, // QW2-3, DQ4-ODD
					0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, // QW4-5, DQ4-ODD
					0xeFeFeFeF,0xeFeFeFeF,0x10101010,0x10101010, // QW6-7, DQ4-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW0-1, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0x20202020,0x20202020, // QW2-3, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW4-5, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW6-7, DQ5-ODD
					0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, // QW0-1, DQ6-ODD
					0x40404040,0x40404040,0xBfBfBfBf,0xBfBfBfBf, // QW2-3, DQ6-ODD
					0x40404040,0x40404040,0xBfBfBfBf,0xBfBfBfBf, // QW4-5, DQ6-ODD
					0x40404040,0x40404040,0xBfBfBfBf,0xBfBfBfBf, // QW6-7, DQ6-ODD
					0x80808080,0x80808080,0x7F7F7F7F,0x7F7F7F7F, // QW0-1, DQ7-ODD
					0x80808080,0x80808080,0x7F7F7F7F,0x7F7F7F7F, // QW2-3, DQ7-ODD
					0x80808080,0x80808080,0x7F7F7F7F,0x7F7F7F7F, // QW4-5, DQ7-ODD
					0x80808080,0x80808080,0x80808080,0x80808080  // QW6-7, DQ7-ODD
		};
	static const uint32_t TestPatternJD1b[] = {
					0x00000000,0x00000000,0x00000000,0x00000000, // QW0,CHA-B, ALL-EVEN
					0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, // QW1,CHA-B, ALL-EVEN
					0x00000000,0x00000000,0x00000000,0x00000000, // QW2,CHA-B, ALL-EVEN
					0x00000000,0x00000000,0x00000000,0x00000000, // QW3,CHA-B, ALL-EVEN
					0x00000000,0x00000000,0x00000000,0x00000000, // QW4,CHA-B, ALL-EVEN
					0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, // QW5,CHA-B, ALL-EVEN
					0x00000000,0x00000000,0x00000000,0x00000000, // QW6,CHA-B, ALL-EVEN
					0x00000000,0x00000000,0x00000000,0x00000000, // QW7,CHA-B, ALL-EVEN
					0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, // QW0,CHA-B, DQ0-ODD
					0x01010101,0x01010101,0x01010101,0x01010101, // QW1,CHA-B, DQ0-ODD
					0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, // QW2,CHA-B, DQ0-ODD
					0x01010101,0x01010101,0x01010101,0x01010101, // QW3,CHA-B, DQ0-ODD
					0x01010101,0x01010101,0x01010101,0x01010101, // QW4,CHA-B, DQ0-ODD
					0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, // QW5,CHA-B, DQ0-ODD
					0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe,0xFeFeFeFe, // QW6,CHA-B, DQ0-ODD
					0x01010101,0x01010101,0x01010101,0x01010101, // QW7,CHA-B, DQ0-ODD
					0x02020202,0x02020202,0x02020202,0x02020202, // QW0,CHA-B, DQ1-ODD
					0x02020202,0x02020202,0x02020202,0x02020202, // QW1,CHA-B, DQ1-ODD
					0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, // QW2,CHA-B, DQ1-ODD
					0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, // QW3,CHA-B, DQ1-ODD
					0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd,0xFdFdFdFd, // QW4,CHA-B, DQ1-ODD
					0x02020202,0x02020202,0x02020202,0x02020202, // QW5,CHA-B, DQ1-ODD
					0x02020202,0x02020202,0x02020202,0x02020202, // QW6,CHA-B, DQ1-ODD
					0x02020202,0x02020202,0x02020202,0x02020202, // QW7,CHA-B, DQ1-ODD
					0x04040404,0x04040404,0x04040404,0x04040404, // QW0,CHA-B, DQ2-ODD
					0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, // QW1,CHA-B, DQ2-ODD
					0x04040404,0x04040404,0x04040404,0x04040404, // QW2,CHA-B, DQ2-ODD
					0x04040404,0x04040404,0x04040404,0x04040404, // QW3,CHA-B, DQ2-ODD
					0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, // QW4,CHA-B, DQ2-ODD
					0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, // QW5,CHA-B, DQ2-ODD
					0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, // QW6,CHA-B, DQ2-ODD
					0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB,0xfBfBfBfB, // QW7,CHA-B, DQ2-ODD
					0x08080808,0x08080808,0x08080808,0x08080808, // QW0,CHA-B, DQ3-ODD
					0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, // QW1,CHA-B, DQ3-ODD
					0x08080808,0x08080808,0x08080808,0x08080808, // QW2,CHA-B, DQ3-ODD
					0x08080808,0x08080808,0x08080808,0x08080808, // QW3,CHA-B, DQ3-ODD
					0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, // QW4,CHA-B, DQ3-ODD
					0x08080808,0x08080808,0x08080808,0x08080808, // QW5,CHA-B, DQ3-ODD
					0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, // QW6,CHA-B, DQ3-ODD
					0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7,0xF7F7F7F7, // QW7,CHA-B, DQ3-ODD
					0x10101010,0x10101010,0x10101010,0x10101010, // QW0,CHA-B, DQ4-ODD
					0x10101010,0x10101010,0x10101010,0x10101010, // QW1,CHA-B, DQ4-ODD
					0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, // QW2,CHA-B, DQ4-ODD
					0x10101010,0x10101010,0x10101010,0x10101010, // QW3,CHA-B, DQ4-ODD
					0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, // QW4,CHA-B, DQ4-ODD
					0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, // QW5,CHA-B, DQ4-ODD
					0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF,0xeFeFeFeF, // QW6,CHA-B, DQ4-ODD
					0x10101010,0x10101010,0x10101010,0x10101010, // QW7,CHA-B, DQ4-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW0,CHA-B, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW1,CHA-B, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW2,CHA-B, DQ5-ODD
					0x20202020,0x20202020,0x20202020,0x20202020, // QW3,CHA-B, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW4,CHA-B, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW5,CHA-B, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW6,CHA-B, DQ5-ODD
					0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF,0xdFdFdFdF, // QW7,CHA-B, DQ5-ODD
					0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, // QW0,CHA-B, DQ6-ODD
					0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, // QW1,CHA-B, DQ6-ODD
					0x40404040,0x40404040,0x40404040,0x40404040, // QW2,CHA-B, DQ6-ODD
					0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, // QW3,CHA-B, DQ6-ODD
					0x40404040,0x40404040,0x40404040,0x40404040, // QW4,CHA-B, DQ6-ODD
					0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, // QW5,CHA-B, DQ6-ODD
					0x40404040,0x40404040,0x40404040,0x40404040, // QW6,CHA-B, DQ6-ODD
					0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf,0xBfBfBfBf, // QW7,CHA-B, DQ6-ODD
					0x80808080,0x80808080,0x80808080,0x80808080, // QW0,CHA-B, DQ7-ODD
					0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F, // QW1,CHA-B, DQ7-ODD
					0x80808080,0x80808080,0x80808080,0x80808080, // QW2,CHA-B, DQ7-ODD
					0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F, // QW3,CHA-B, DQ7-ODD
					0x80808080,0x80808080,0x80808080,0x80808080, // QW4,CHA-B, DQ7-ODD
					0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F,0x7F7F7F7F, // QW5,CHA-B, DQ7-ODD
					0x80808080,0x80808080,0x80808080,0x80808080, // QW6,CHA-B, DQ7-ODD
					0x80808080,0x80808080,0x80808080,0x80808080  // QW7,CHA-B, DQ7-ODD
		};
	uint8_t pattern_buf_x[64 * 18 + 16]; // We need to two cache line So have more 16 bytes to keep 16 byte alignment */
	uint8_t *buf_a;

	unsigned pattern;
	uint32_t dword;
	uint32_t ecc_bit;
	unsigned Errors;
	unsigned channel;
	int i;
	unsigned DQSWrDelay;
	unsigned is_Width128 = sysinfo->meminfo[ctrl->node_id].is_Width128;
	uint8_t *dqs_delay_a = &sysinfo->dqs_delay_a[ctrl->node_id * 2*2*9]; //channel 2, direction 2 , bytelane *9

	//enable SSE2
	enable_sse2();

	//wrap32dis
	set_wrap32dis();

	//disable ECC temp
	dword = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	ecc_bit = dword & DCL_DimmEccEn;
	dword &= ~(DCL_DimmEccEn);
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dword);

	//SetupDqsPattern
	buf_a = (uint8_t *)(((uint32_t)(&pattern_buf_x[0]) + 0x10) & (~0xf));

	if (is_Width128) {
		pattern = 1;
		for (i = 0; i < 16*18; i++) {
			*((uint32_t *)(buf_a + i*4)) = TestPatternJD1b[i];
		}
	}
	else {
		pattern = 0;
		for (i = 0; i < 16*9; i++) {
			*((uint32_t *)(buf_a + i*4)) = TestPatternJD1a[i];
		}

	}

	print_debug_dqs("\nTrainDQSRdWrPos: 0 ctrl ", ctrl->node_id, 0);

	printk(BIOS_DEBUG, "TrainDQSRdWrPos: buf_a:%p\n", buf_a);

	Errors = 0;
	channel = 0;

	if (!(sysinfo->meminfo[ctrl->node_id].dimm_mask & 0x0F) &&
	     (sysinfo->meminfo[ctrl->node_id].dimm_mask & 0xF0)) { /* channelB only? */
		channel = 1;
	}

	while ((channel < 2) && (!Errors)) {
		print_debug_dqs("\tTrainDQSRdWrPos: 1 channel ",channel, 1);
		for (DQSWrDelay = 0; DQSWrDelay < 48; DQSWrDelay++) {
			unsigned err;
			SetDQSDelayAllCSR(ctrl, channel, DQS_WRITEDIR, DQSWrDelay);
			print_debug_dqs("\t\tTrainDQSRdWrPos: 21 DQSWrDelay ", DQSWrDelay, 2);
			err= TrainReadDQS(ctrl, channel, pattern, buf_a, dqs_delay_a, sysinfo);
			print_debug_dqs("\t\tTrainDQSRdWrPos: 22 err ",err, 2);
			if (err == 0) break;
			Errors |= err;
		}

		print_debug_dqs("\tTrainDQSRdWrPos: 3 DQSWrDelay ", DQSWrDelay, 1);

		if (DQSWrDelay < 48) {
			Errors = TrainWriteDQS(ctrl, channel, pattern, buf_a, dqs_delay_a, sysinfo);
			print_debug_dqs("\tTrainDQSRdWrPos: 4 Errors ", Errors, 1);

		}
		channel++;
		if (!is_Width128) {
			//FIXME: 64MuxMode??
			channel++; // skip channel if 64-bit mode
		}
	}

	//Enable ECC again
	dword = pci_read_config32(ctrl->f2, DRAM_CONFIG_LOW);
	dword &= ~(DCL_DimmEccEn);
	dword |= ecc_bit;
	pci_write_config32(ctrl->f2, DRAM_CONFIG_LOW, dword);

	//Clear wrap32dis

	clear_wrap32dis();

	//restore SSE2 setting
	disable_sse2();

	print_debug_dqs("TrainDQSRdWrPos: ", 5, 0);

	return Errors;

}
static inline uint8_t get_dqs_delay(unsigned channel, unsigned bytelane, unsigned direction, uint8_t *dqs_delay_a)
{
	return dqs_delay_a[channel * 2*9 + direction * 9 + bytelane];
}

static unsigned CalcEccDQSPos(unsigned channel,unsigned ByteLane0, unsigned ByteLane1, unsigned InterFactor, unsigned Direction, uint8_t *dqs_delay_a)
/* InterFactor: 0: 100% ByteLane 0
		0x80: 50% between ByteLane 0 and 1
		0xff: 99.6% ByteLane 1 and 0.4% like 0
*/
{
	unsigned DQSDelay0, DQSDelay1;
	unsigned DQSDelay;

	DQSDelay0 = get_dqs_delay(channel, ByteLane0, Direction, dqs_delay_a);
	DQSDelay1 = get_dqs_delay(channel, ByteLane1, Direction, dqs_delay_a);

	if (DQSDelay0 > DQSDelay1) {
		DQSDelay = DQSDelay0 - DQSDelay1;
		InterFactor = 0xff - InterFactor;
	}
	else {
		DQSDelay = DQSDelay1 - DQSDelay0;
	}

	DQSDelay *= InterFactor;

	DQSDelay >>= 8; // /255

	if (DQSDelay0 > DQSDelay1) {
		DQSDelay += DQSDelay1;
	}
	else {
		DQSDelay += DQSDelay0;
	}

	return DQSDelay;

}

static void SetEccDQSRdWrPos(const struct mem_controller *ctrl, struct sys_info *sysinfo)
{
	unsigned channel;
	unsigned ByteLane;
	unsigned Direction;
	unsigned lane0, lane1, ratio;
	unsigned dqs_delay;

	unsigned direction[] = { DQS_READDIR, DQS_WRITEDIR };
	int i;
	uint8_t *dqs_delay_a = &sysinfo->dqs_delay_a[ctrl->node_id * 2*2*9]; //channel 2, direction 2 , bytelane *9

	ByteLane = 8;

	for (channel = 0; channel < 2; channel++) {
		for (i = 0; i < 2; i++) {
			Direction = direction[i];
			lane0 = 4; lane1 = 5; ratio = 0;
			dqs_delay = CalcEccDQSPos(channel, lane0, lane1, ratio, Direction, dqs_delay_a);
			print_debug_dqs_pair("\t\tSetEccDQSRdWrPos: channel ", channel, Direction == DQS_READDIR? " R dqs_delay":" W dqs_delay", dqs_delay, 2);
			SetDQSDelayCSR(ctrl, channel, ByteLane, Direction, dqs_delay);
			save_dqs_delay(channel, ByteLane, Direction, dqs_delay_a, dqs_delay);
		}
	}
}

static unsigned train_DqsRcvrEn(const struct mem_controller *ctrl, unsigned Pass, struct sys_info *sysinfo)
{
	print_debug_dqs("\ntrain_DqsRcvrEn: begin ctrl ", ctrl->node_id, 0);
	if (TrainRcvrEn(ctrl, Pass, sysinfo)) {
		return 1;
	}
	print_debug_dqs("\ntrain_DqsRcvrEn: end ctrl ", ctrl->node_id, 0);
	return 0;

}
static unsigned train_DqsPos(const struct mem_controller *ctrl, struct sys_info *sysinfo)
{
	print_debug_dqs("\ntrain_DqsPos: begin ctrl ", ctrl->node_id, 0);
	if (TrainDQSRdWrPos(ctrl, sysinfo) != 0) {
		printk(BIOS_ERR, "\nDQS Training Rd Wr failed ctrl%02x\n", ctrl->node_id);
		return 1;
	}
	else {
		SetEccDQSRdWrPos(ctrl, sysinfo);
	}
	print_debug_dqs("\ntrain_DqsPos: end ctrl ", ctrl->node_id, 0);
	return 0;

}

#if K8_REV_F_SUPPORT_F0_F1_WORKAROUND == 1
static void f0_svm_workaround(int controllers, const struct mem_controller *ctrl, tsc_t *tsc0, struct sys_info *sysinfo)
{
	tsc_t tsc1[8];
	unsigned cpu_f0_f1[8];
	int i;

	print_debug_addr("dqs_timing: tsc1[8] :", tsc1);

	for (i = 0; i < controllers; i++) {
		if (!sysinfo->ctrl_present[i])
			continue;

		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00) continue;

		uint32_t dword;

		cpu_f0_f1[i] = is_cpu_pre_f2_in_bsp(i);

		if (!cpu_f0_f1[i]) continue;

		dword = pci_read_config32(ctrl[i].f2, DRAM_CTRL);
		dword &= ~DC_DqsRcvEnTrain;
		pci_write_config32(ctrl[i].f2, DRAM_CTRL, dword);

		dword = pci_read_config32(ctrl[i].f2, DRAM_INIT);
		dword |= DI_EnDramInit;
		pci_write_config32(ctrl[i].f2, DRAM_INIT, dword);
		dword &= ~DI_EnDramInit;
		pci_write_config32(ctrl[i].f2, DRAM_INIT, dword);

		tsc1[i] = rdtsc();
		print_debug_dqs_tsc("begin: tsc1", i, tsc1[i].hi, tsc1[i].lo, 2);

		dword = tsc1[i].lo + tsc0[i].lo;
		if ((dword < tsc1[i].lo) || (dword < tsc0[i].lo)) {
			tsc1[i].hi++;
		}
		tsc1[i].lo = dword;
		tsc1[i].hi+= tsc0[i].hi;

		print_debug_dqs_tsc("end  : tsc1", i, tsc1[i].hi, tsc1[i].lo, 2);

	}

	for (i = 0; i < controllers; i++) {
		if (!sysinfo->ctrl_present[i])
			continue;

		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00) continue;

		if (!cpu_f0_f1[i]) continue;

		tsc_t tsc;

		do {
			tsc = rdtsc();
		} while ((tsc1[i].hi > tsc.hi) || ((tsc1[i].hi == tsc.hi) && (tsc1[i].lo > tsc.lo)));

		print_debug_dqs_tsc("end  : tsc ", i, tsc.hi, tsc.lo, 2);
	}

}

#endif


/* setting variable mtrr, comes from linux kernel source */
static void set_var_mtrr_dqs(
	unsigned int reg, unsigned long basek, unsigned long sizek,
	unsigned char type, unsigned address_bits)
{
	msr_t base, mask;
	unsigned address_mask_high;

	address_mask_high = ((1u << (address_bits - 32u)) - 1u);

	base.hi = basek >> 22;
	base.lo = basek << 10;

	if (sizek < 4*1024*1024) {
		mask.hi = address_mask_high;
		mask.lo = ~((sizek << 10) -1);
	}
	else {
		mask.hi = address_mask_high & (~((sizek >> 22) -1));
		mask.lo = 0;
	}

	if (reg >= 8)
		return;

	if (sizek == 0) {
		msr_t zero;
		zero.lo = zero.hi = 0;
		/* The invalid bit is kept in the mask, so we simply clear the
		   relevant mask register to disable a range. */
		wrmsr (MTRR_PHYS_MASK(reg), zero);
	} else {
		/* Bit 32-35 of MTRRphysMask should be set to 1 */
		base.lo |= type;
		mask.lo |= 0x800;
		wrmsr (MTRR_PHYS_BASE(reg), base);
		wrmsr (MTRR_PHYS_MASK(reg), mask);
	}
}

static unsigned int range_to_mtrr(unsigned int reg,
	unsigned long range_startk, unsigned long range_sizek,
	unsigned long next_range_startk, unsigned char type, unsigned address_bits)
{
	if (!range_sizek || (reg >= 8)) {
		return reg;
	}
	while (range_sizek) {
		unsigned long max_align, align;
		unsigned long sizek;
		/* Compute the maximum size I can make a range */
		max_align = fls(range_startk);
		align = fms(range_sizek);
		if (align > max_align) {
			align = max_align;
		}
		sizek = 1 << align;
#if CONFIG_MEM_TRAIN_SEQ != 1
		printk(BIOS_DEBUG, "Setting variable MTRR %d, base: %4ldMB, range: %4ldMB, type %s\n",
			reg, range_startk >>10, sizek >> 10,
			(type == MTRR_TYPE_UNCACHEABLE)?"UC":
			    ((type == MTRR_TYPE_WRBACK)?"WB":"Other")
			);
#endif
		set_var_mtrr_dqs(reg++, range_startk, sizek, type, address_bits);
		range_startk += sizek;
		range_sizek -= sizek;
		if (reg >= 8)
			break;
	}
	return reg;
}

#if CONFIG_MEM_TRAIN_SEQ == 1
static void set_top_mem_ap(unsigned tom_k, unsigned tom2_k)
{
	msr_t msr;

	/* Now set top of memory */
	msr.lo = (tom2_k & 0x003fffff) << 10;
	msr.hi = (tom2_k & 0xffc00000) >> 22;
	wrmsr(TOP_MEM2, msr);

	msr.lo = (tom_k & 0x003fffff) << 10;
	msr.hi = (tom_k & 0xffc00000) >> 22;
	wrmsr(TOP_MEM, msr);
}
#endif

static void setup_mtrr_dqs(unsigned tom_k, unsigned tom2_k)
{
	msr_t msr;

	//[0,512k), [512k, 640k)
	msr.hi = 0x1e1e1e1e;
	msr.lo = msr.hi;
	wrmsr(0x250, msr);
	wrmsr(0x258, msr);

	//[1M, TOM)
	range_to_mtrr(2, 0, tom_k,4*1024*1024, MTRR_TYPE_WRBACK, 40);

	//[4G, TOM2)
	if (tom2_k) {
		//enable tom2 and type
		msr = rdmsr(SYSCFG_MSR);
		msr.lo |= (1<<21) | (1<<22); //MtrrTom2En and Tom2ForceMemTypeWB
		wrmsr(SYSCFG_MSR, msr);
	}

}

static void clear_mtrr_dqs(unsigned tom2_k)
{
	msr_t msr;
	unsigned i;

	//still enable from cache_as_ram.inc
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR,msr);

	//[0,512k), [512k, 640k)
	msr.hi = 0;
	msr.lo = msr.hi;
	wrmsr(0x250, msr);
	wrmsr(0x258, msr);

	//[1M, TOM)
	for (i = 0x204; i < 0x210; i++) {
		wrmsr(i, msr);
	}

	//[4G, TOM2)
	if (tom2_k) {
		//enable tom2 and type
		msr = rdmsr(SYSCFG_MSR);
		msr.lo &= ~((1<<21) | (1<<22)); //MtrrTom2En and Tom2ForceMemTypeWB
		wrmsr(SYSCFG_MSR, msr);
	}
}

#if CONFIG_MEM_TRAIN_SEQ == 1
static void set_htic_bit(unsigned i, unsigned val, unsigned bit)
{
	uint32_t dword;
	dword = pci_read_config32(PCI_DEV(0, 0x18+i, 0), HT_INIT_CONTROL);
	dword &= ~(1 << bit);
	dword |= ((val & 1) << bit);
	pci_write_config32(PCI_DEV(0, 0x18+i, 0), HT_INIT_CONTROL, dword);
}

static unsigned get_htic_bit(unsigned i, unsigned bit)
{
	uint32_t dword;
	dword = pci_read_config32(PCI_DEV(0, 0x18+i, 0), HT_INIT_CONTROL);
	dword &= (1 << bit);
	return dword;
}

static void wait_till_sysinfo_in_ram(void)
{
	while (1) {
		if (get_htic_bit(0, 9)) return;
	}
}
#endif

void set_sysinfo_in_ram(unsigned val)
{
#if CONFIG_MEM_TRAIN_SEQ == 1
	set_htic_bit(0, val, 9);
#endif
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)

#if CONFIG_MEM_TRAIN_SEQ == 0
static int save_index_to_pos(unsigned int dev, int size, int index, int nvram_pos)
{
	u32 dword = pci_read_config32_index_wait(dev, 0x98, index);

	return s3_save_nvram_early(dword, size, nvram_pos);
}
#endif

static int load_index_to_pos(unsigned int dev, int size, int index, int nvram_pos)
{

	u32 old_dword = pci_read_config32_index_wait(dev, 0x98, index);
	nvram_pos = s3_load_nvram_early(size, &old_dword, nvram_pos);
	pci_write_config32_index_wait(dev, 0x98, index, old_dword);
	return nvram_pos;
}

static int dqs_load_MC_NVRAM_ch(unsigned int dev, int ch, int pos)
{
	/* 30 bytes per channel */
	ch *= 0x20;
	pos = load_index_to_pos(dev, 4, 0x00 + ch, pos);
	pos = load_index_to_pos(dev, 4, 0x01 + ch, pos);
	pos = load_index_to_pos(dev, 4, 0x02 + ch, pos);
	pos = load_index_to_pos(dev, 1, 0x03 + ch, pos);
	pos = load_index_to_pos(dev, 4, 0x04 + ch, pos);
	pos = load_index_to_pos(dev, 4, 0x05 + ch, pos);
	pos = load_index_to_pos(dev, 4, 0x06 + ch, pos);
	pos = load_index_to_pos(dev, 1, 0x07 + ch, pos);
	pos = load_index_to_pos(dev, 1, 0x10 + ch, pos);
	pos = load_index_to_pos(dev, 1, 0x13 + ch, pos);
	pos = load_index_to_pos(dev, 1, 0x16 + ch, pos);
	pos = load_index_to_pos(dev, 1, 0x19 + ch, pos);
	return pos;
}

#if CONFIG_MEM_TRAIN_SEQ == 0
static int dqs_save_MC_NVRAM_ch(unsigned int dev, int ch, int pos)
{
	/* 30 bytes per channel */
	ch *= 0x20;
	pos = save_index_to_pos(dev, 4, 0x00 + ch, pos);
	pos = save_index_to_pos(dev, 4, 0x01 + ch, pos);
	pos = save_index_to_pos(dev, 4, 0x02 + ch, pos);
	pos = save_index_to_pos(dev, 1, 0x03 + ch, pos);
	pos = save_index_to_pos(dev, 4, 0x04 + ch, pos);
	pos = save_index_to_pos(dev, 4, 0x05 + ch, pos);
	pos = save_index_to_pos(dev, 4, 0x06 + ch, pos);
	pos = save_index_to_pos(dev, 1, 0x07 + ch, pos);
	pos = save_index_to_pos(dev, 1, 0x10 + ch, pos);
	pos = save_index_to_pos(dev, 1, 0x13 + ch, pos);
	pos = save_index_to_pos(dev, 1, 0x16 + ch, pos);
	pos = save_index_to_pos(dev, 1, 0x19 + ch, pos);
	return pos;
}

static void dqs_save_MC_NVRAM(unsigned int dev)
{
	int pos = 0;
	u32 reg;
	printk(BIOS_DEBUG, "DQS SAVE NVRAM: %x\n", dev);
	pos = dqs_save_MC_NVRAM_ch(dev, 0, pos);
	pos = dqs_save_MC_NVRAM_ch(dev, 1, pos);
	/* save the maxasync lat here */
	reg = pci_read_config32(dev, DRAM_CONFIG_HIGH);
	pos = s3_save_nvram_early(reg, 4, pos);
}
#endif

void dqs_restore_MC_NVRAM(unsigned int dev)
{
	int pos = 0;
	u32 reg;

	printk(BIOS_DEBUG, "DQS RESTORE FROM NVRAM: %x\n", dev);
	pos = dqs_load_MC_NVRAM_ch(dev, 0, pos);
	pos = dqs_load_MC_NVRAM_ch(dev, 1, pos);
	/* load the maxasync lat here */
	pos = s3_load_nvram_early(4, &reg, pos);
	reg &= (DCH_MaxAsyncLat_MASK << DCH_MaxAsyncLat_SHIFT);
	reg |= pci_read_config32(dev, DRAM_CONFIG_HIGH);
	pci_write_config32(dev, DRAM_CONFIG_HIGH, reg);
}
#endif

#if CONFIG_MEM_TRAIN_SEQ == 0
#if K8_REV_F_SUPPORT_F0_F1_WORKAROUND == 1
static void dqs_timing(int controllers, const struct mem_controller *ctrl, tsc_t *tsc0, struct sys_info *sysinfo)
#else
static void dqs_timing(int controllers, const struct mem_controller *ctrl, struct sys_info *sysinfo)
#endif
{
	int i;

	tsc_t tsc[5];

	//need to enable mtrr, so dqs training could access the test address
	setup_mtrr_dqs(sysinfo->tom_k, sysinfo->tom2_k);

	for (i = 0; i < controllers; i++) {
		if (!sysinfo->ctrl_present[ i ])
			continue;

		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00) continue;

		fill_mem_cs_sysinfo(i, ctrl+i, sysinfo);
	}

	tsc[0] = rdtsc();
	for (i = 0; i < controllers; i++) {
		if (!sysinfo->ctrl_present[ i ])
			continue;

		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00) continue;

		printk(BIOS_DEBUG, "DQS Training:RcvrEn:Pass1: %02x\n", i);
		if (train_DqsRcvrEn(ctrl+i, 1, sysinfo)) goto out;
		printk(BIOS_DEBUG, " done\n");
	}

	tsc[1] = rdtsc();
#if K8_REV_F_SUPPORT_F0_F1_WORKAROUND == 1
	f0_svm_workaround(controllers, ctrl, tsc0, sysinfo);
#endif

	tsc[2] = rdtsc();
	for (i = 0; i < controllers; i++) {
		if (!sysinfo->ctrl_present[i])
			continue;

		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00) continue;

		printk(BIOS_DEBUG, "DQS Training:DQSPos: %02x\n", i);
		if (train_DqsPos(ctrl+i, sysinfo)) goto out;
		printk(BIOS_DEBUG, " done\n");
	}

	tsc[3] = rdtsc();
	for (i = 0; i < controllers; i++) {
		if (!sysinfo->ctrl_present[i])
			continue;

		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00) continue;

		printk(BIOS_DEBUG, "DQS Training:RcvrEn:Pass2: %02x\n", i);
		if (train_DqsRcvrEn(ctrl+i, 2, sysinfo)) goto out;
		printk(BIOS_DEBUG, " done\n");
		sysinfo->mem_trained[i]=1;
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		dqs_save_MC_NVRAM((ctrl+i)->f2);
#endif
	}

out:
	tsc[4] = rdtsc();
	clear_mtrr_dqs(sysinfo->tom2_k);


	for (i = 0; i < 5; i++) {
		print_debug_dqs_tsc_x("DQS Training:tsc", i, tsc[i].hi, tsc[i].lo);
	}



}

#endif


#if CONFIG_MEM_TRAIN_SEQ > 0

static void dqs_timing(int i, const struct mem_controller *ctrl, struct sys_info *sysinfo, unsigned v)
{

	int ii;

	 tsc_t tsc[4];

	if (sysinfo->mem_trained[i] != 0x80) return;

#if CONFIG_MEM_TRAIN_SEQ == 1
	//need to enable mtrr, so dqs training could access the test address
	setup_mtrr_dqs(sysinfo->tom_k, sysinfo->tom2_k);
#endif

	fill_mem_cs_sysinfo(i, ctrl, sysinfo);

	if (v) {
		tsc[0] = rdtsc();

		printk(BIOS_DEBUG, "set DQS timing:RcvrEn:Pass1: %02x\n", i);
	}
	if (train_DqsRcvrEn(ctrl, 1, sysinfo)) {
		sysinfo->mem_trained[i]=0x81; //
		goto out;
	}

	if (v) {
		printk(BIOS_DEBUG, " done\n");
		tsc[1] = rdtsc();
		printk(BIOS_DEBUG, "set DQS timing:DQSPos: %02x\n", i);
	}

	if (train_DqsPos(ctrl, sysinfo)) {
		sysinfo->mem_trained[i]=0x82; //
		goto out;
	}

	if (v) {
		printk(BIOS_DEBUG, " done\n");
		tsc[2] = rdtsc();

		printk(BIOS_DEBUG, "set DQS timing:RcvrEn:Pass2: %02x\n", i);
	}
	if (train_DqsRcvrEn(ctrl, 2, sysinfo)) {
		sysinfo->mem_trained[i]=0x83; //
		goto out;
	}

	if (v) {
		printk(BIOS_DEBUG, " done\n");

		tsc[3] = rdtsc();
	}

out:
#if CONFIG_MEM_TRAIN_SEQ == 1
	clear_mtrr_dqs(sysinfo->tom2_k);
#endif

	if (v) {
		for (ii = 0; ii < 4; ii++) {
			print_debug_dqs_tsc_x("Total DQS Training : tsc ", ii, tsc[ii].hi, tsc[ii].lo);
		}
	}

	if (sysinfo->mem_trained[i] == 0x80) {
		sysinfo->mem_trained[i]=1;
	}

}
#endif

#if CONFIG_MEM_TRAIN_SEQ == 1
static void train_ram(unsigned nodeid, struct sys_info *sysinfo, struct sys_info *sysinfox)
{
	dqs_timing(nodeid, &sysinfo->ctrl[nodeid], sysinfo, 0); // keep the output tidy
	sysinfox->mem_trained[nodeid] = sysinfo->mem_trained[nodeid];

}

void train_ram_on_node(unsigned nodeid, unsigned coreid, struct sys_info *sysinfo, unsigned retcall)
{
	if (coreid) return; // only do it on core0
	struct sys_info *sysinfox;
	uintptr_t migrated_base = CONFIG_RAMTOP - car_data_size();

	sysinfox = (void *)(migrated_base + car_object_offset(&sysinfo_car));

	wait_till_sysinfo_in_ram(); // use pci to get it

	if (sysinfox->mem_trained[nodeid] == 0x80) {
		memcpy(sysinfo, sysinfox, sizeof(*sysinfo));
		set_top_mem_ap(sysinfo->tom_k, sysinfo->tom2_k); // keep the ap's tom consistent with bsp's
		printk(BIOS_DEBUG, "CODE IN ROM AND RUN ON NODE: %02x\n", nodeid);
		train_ram(nodeid, sysinfo, sysinfox);
	}
}
#endif
