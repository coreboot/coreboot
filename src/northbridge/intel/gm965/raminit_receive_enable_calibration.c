/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - Receive-Enable Calibration
 *
 * Receive-enable training adjusts the DQS capture timing for each
 * channel to find the optimal DQS sampling point.
 *
 * BIOS uses a 2-level coarse delay (coarse_high * 4 + coarse_low = 0..63)
 * plus a 15-step fine delay (0..14) per combined coarse unit. A DQS level
 * test function checks bit 30 of the DQS readback register via actual
 * memory reads.
 *
 * Algorithm (3-phase per channel):
 *   Phase 1 - Coarse sweep up: while DQS is HIGH, increment combined
 *             coarse by 1. Finds the DQS falling edge.
 *   Phase 2 - Fine sweep: step coarse +1, check if DQS went HIGH (back
 *             off if so), then sweep fine 0->14 (wrapping to next coarse
 *             on overflow) using test_dqs_level(LOW) until DQS rises.
 *   Phase 3 - Coarse sweep down: subtract 3, then sweep down by 4
 *             combined coarse per step using test_dqs_level(HIGH) until DQS falls.
 *   Final: add 1 combined coarse (center of window).
 *
 * BIOS functions (RAMINIT copy - actual execution path):
 *   FFF00EC9 - orchestrator (POST 0xFF39)
 *   FFF00DA9 - per-channel training
 *   FFF00D5C - Phase 1: coarse_sweep_up (-> FFF00B1F)
 *   FFF00CB2 - Phase 2: fine_sweep (-> FFF00B1F + FFF00B77)
 *   FFF00C67 - Phase 3: coarse_sweep_down (-> FFF00B1F)
 *   FFF00C11 - program_rec_timing (with MMIO read-back flush)
 *   FFF00BF0 - coarse_add
 *   FFF00BCF - coarse_sub
 *   FFF00B1F - test_dqs_level(HIGH): RW ptr toggle + memory read + bit 30 HIGH
 *   FFF00B77 - test_dqs_level(LOW):  RW ptr toggle + memory read + bit 30 LOW
 *
 * Both RAMINIT and RAMINIT3 copies have identical training logic.
 * Both DQS level test variants (FFF00B1F and FFF00B77) read from rank_addr
 * via [ESP+0xc].  Ghidra's decompiler missed the 2nd parameter in
 * FFF00B1F because it uses ESP-relative addressing, not EBP-relative.
 *
 * Key difference from RAMINIT3: RAMINIT copy does MMIO read-back
 * flushes after every register write in program_rec_timing (FFF00C11).
 * Both copies use identical post-training cleanup (SET then CLEAR
 * of CxDRC1 bit 6, RW_PTR sequencing).
 */

#include <stdint.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <console/console.h>
#include <northbridge/intel/gm965/gm965.h>

/*
 * Timing state for receive-enable training.
 * BIOS: 4-byte stack-local struct at [ebp-4..ebp-1].
 *   combined coarse = coarse_high * 4 + coarse_low (0..63)
 *   fine = 0..14 (15 steps per combined coarse unit)
 */
typedef struct {
	uint8_t coarse_high;	/* Main coarse delay, 0-15 -> CxDRT3[10:7] */
	uint8_t coarse_low;	/* Sub-coarse delay, 0-3 -> REC_COARSE_LOW[3:2] */
	uint8_t fine;		/* Fine delay, 0-14 -> CxTRAIN_CFG[7:4] */
	uint8_t config;		/* Fixed = 4 */
} rec_timing_t;

static void coarse_add(rec_timing_t *t, int step)
{
	int combined = t->coarse_high * 4 + t->coarse_low + step;
	t->coarse_high = (uint8_t)(combined >> 2);
	t->coarse_low = (uint8_t)(combined & 3);
}

/*
 * coarse_sub() - Subtract step from combined coarse delay
 * BIOS: FFFF3A6F (33 bytes)
 */
static void coarse_sub(rec_timing_t *t, int step)
{
	int combined = t->coarse_high * 4 + t->coarse_low - step;
	t->coarse_high = (uint8_t)(combined >> 2);
	t->coarse_low = (uint8_t)(combined & 3);
}

/*
 * program_rec_timing() - Write coarse_high and coarse_low to MCHBAR
 * BIOS: FFFF3AB1 (86 bytes)
 *
 * coarse_high -> CxDRT3 (MCHBAR+0x121C+ch*0x100) bits [10:7]
 * coarse_low  -> REC_COARSE_LOW (MCHBAR+0x14B0+ch*0x100) bits [3:2]
 */
static void program_rec_timing(int ch, const rec_timing_t *t)
{
	/* Write coarse_high to CxDRT3 bits [10:7] (32-bit RMW) */
	uint32_t drt3 = mchbar_read32(CxDRT3_MCHBAR(ch));
	drt3 = (drt3 & ~(0x0fU << 7)) | ((uint32_t)(t->coarse_high & 0x0f) << 7);
	mchbar_write32(CxDRT3_MCHBAR(ch), drt3);
	/* TODO: Is this read-back necessary? The registers are in the MCH,
	 * not across PCIe/DMI, so posted-write flush should not be needed. */
	(void)mchbar_read32(CxDRT3_MCHBAR(ch));

	/* Write coarse_low to REC_COARSE_LOW bits [3:2] (8-bit RMW) */
	uint8_t reg = mchbar_read8(REC_COARSE_LOW(ch));
	reg = (reg & 0xf3) | ((t->coarse_low & 0x03) << 2);
	mchbar_write8(REC_COARSE_LOW(ch), reg);
	(void)mchbar_read32(REC_COARSE_LOW(ch));  /* TODO: same as above */
}

/*
 * write_fine_delay() - Write fine timing to CxTRAIN_CFG
 * BIOS: inline in fine_sweep at FFFF3BC5
 *
 * fine -> CxTRAIN_CFG (MCHBAR+0x1484+ch*0x100) bits [7:4]
 */
static void write_fine_delay(int ch, uint8_t fine)
{
	uint8_t reg = mchbar_read8(CxTRAIN_CFG(ch));
	reg = (reg & 0x0f) | (fine << 4);
	mchbar_write8(CxTRAIN_CFG(ch), reg);
}

/*
 * test_dqs_level() - Test if DQS is consistently at the expected level
 *
 * BIOS: FFF00B1F / FFFF39BF (expect HIGH), FFF00B77 / FFFF3A17 (expect LOW)
 * Both are 88 bytes each with identical logic, differing only in the
 * polarity of the bit 30 check.
 *
 * Both RAMINIT and RAMINIT3 copies do the same thing:
 *   1. Toggle RW pointers (clear then set bit 9) on both channels
 *   2. Read from rank_addr to trigger DQS strobe
 *   3. I/O delay (IN AL, 0x61)
 *   4. Check bit 30 of REC_DQS_LEVEL
 *
 * Note: The Ghidra decompiler showed FFF00B1F as having only one
 * parameter because both params use ESP-relative addressing.  The
 * raw disassembly confirms [ESP+0xc] = rank_addr IS dereferenced.
 *
 * @expect_high: 1 to test for DQS HIGH, 0 to test for DQS LOW.
 * Returns 1 if DQS matched the expected level in all 3 samples.
 */
static bool test_dqs_level(int ch, uintptr_t rank_addr, bool expect_high)
{
	int pass = true;
	int i;

	for (i = 0; i < 3; i++) {
		/* Reset RW pointers: clear bit 9, then set bit 9 - both channels */
		mchbar_clrbits16(RW_PTR_CTRL(0), 1 << 9);
		mchbar_setbits16(RW_PTR_CTRL(0), 1 << 9);
		mchbar_clrbits16(RW_PTR_CTRL(1), 1 << 9);
		mchbar_setbits16(RW_PTR_CTRL(1), 1 << 9);

		/* Trigger memory read - generates DQS strobe */
		read32((volatile void *)(uintptr_t)rank_addr);

		/* I/O delay for DQS capture latching */
		inb(0x61);

		/* Check DQS level: bit 30 of readback register */
		bool high = !!(mchbar_read32(REC_DQS_LEVEL(ch)) & (1 << 30));
		if (high != expect_high)
			pass = false;
	}

	return pass;
}

/*
 * IO_INIT_CFG2 training byte table (BIOS file offset 0x175900).
 * Indexed by mem_clock. Written to bits [27:23] during training setup.
 */
static const uint8_t io_init_cfg2_training[] = {
	[0]               = 0x05,
	[MEM_CLOCK_533MT] = 0x07,
	[MEM_CLOCK_667MT] = 0x09,
};

/*
 * coarse_sweep_up() - Phase 1: Find falling edge of DQS
 * BIOS: FFF00D5C -> FFF00B1F (test_dqs_level HIGH with memory read)
 *
 * Sweeps combined coarse upward by 1 per iteration.
 * Loops while DQS is HIGH, stops when DQS falls.
 * Returns 1 on success, -1 on overflow.
 */
static int coarse_sweep_up(int ch, rec_timing_t *t, uintptr_t rank_addr)
{
	/* Test at current timing first */
	if (!test_dqs_level(ch, rank_addr, true))
		return 1;  /* Already not HIGH - done */

	/* Increment combined coarse until DQS falls */
	do {
		coarse_add(t, 1);
		if (t->coarse_high > 0x0f)
			return -1;  /* Overflow */
		program_rec_timing(ch, t);

		if (!test_dqs_level(ch, rank_addr, true))
			return 1;  /* Found transition */
	} while (1);
}

/*
 * fine_sweep() - Phase 2: Find precise LOW->HIGH DQS edge
 * BIOS: FFF00CB2 -> FFF00B1F (test_dqs_level HIGH) + FFF00B77 (test_dqs_level LOW)
 *
 * 1. Steps combined coarse forward by 1
 * 2. Tests if DQS went HIGH - if so, backs off by 1 (overshot)
 * 3. Sweeps fine delay 0->14, wrapping to next combined coarse on overflow
 * 4. Uses test_dqs_level(LOW) - loops while DQS is LOW
 * 5. Stops when DQS transitions to NOT-LOW (rising edge)
 */
static int fine_sweep(int ch, rec_timing_t *t, uintptr_t rank_addr)
{
	/* Step 1: increment combined coarse by 1 */
	coarse_add(t, 1);
	if (t->coarse_high > 0x0f)
		return -1;

	program_rec_timing(ch, t);

	/* Step 2: check if DQS went HIGH - if so, back off */
	if (test_dqs_level(ch, rank_addr, true)) {
		coarse_sub(t, 1);
		program_rec_timing(ch, t);
	}

	/* Step 3: fine sweep - loop while DQS is LOW */
	while (test_dqs_level(ch, rank_addr, false)) {
		t->fine++;
		if (t->fine > 0x0e) {
			/* Fine overflow: wrap to 0, increment combined coarse */
			t->fine = 0;
			coarse_add(t, 1);
			if (t->coarse_high > 0x0f)
				return -1;
			program_rec_timing(ch, t);
		}
		/* Write fine delay to CxTRAIN_CFG[7:4] */
		write_fine_delay(ch, t->fine);
	}

	return 1;
}

/*
 * coarse_sweep_down() - Phase 3: Find lower window boundary
 * BIOS: FFF00C67 -> FFF00B1F (test_dqs_level HIGH with memory read)
 *
 * Sweeps combined coarse downward by 4 per iteration (= 1 coarse_high unit).
 * Loops while DQS is HIGH, stops when DQS falls.
 * Returns 1 on success, -1 on underflow.
 */
static int coarse_sweep_down(int ch, rec_timing_t *t, uintptr_t rank_addr)
{
	/* Test at current timing first */
	if (!test_dqs_level(ch, rank_addr, true))
		return 1;  /* Already not HIGH - done */

	do {
		if (t->coarse_high == 0)
			return -1;  /* Underflow */
		coarse_sub(t, 4);
		program_rec_timing(ch, t);

		if (!test_dqs_level(ch, rank_addr, true))
			return 1;  /* Found lower edge */
	} while (1);
}

/*
 * per_channel_rec_training() - Run 3-phase REC training for one channel
 * BIOS: FFFF4E70 (301 bytes)
 *
 * Computes rank_addr for memory test reads:
 *   Channel 0: address 0
 *   Channel 1 + dual-channel: address 0x40 (next cache line in interleave)
 *   Channel 1 + single-channel: MCHBAR16(0x1206) << 25 (ch0 boundary)
 *
 * Initializes timing struct, runs 3 phases, writes final CxDRT5.
 */
static void per_channel_rec_training(int ch, sysinfo_t *si)
{
	rec_timing_t t;

	/*
	 * BIOS: initial coarse_high = sysinfo[0x22] + 4.
	 * Vendor stores CAS as index (CAS_actual - 3) at sysinfo[0x22],
	 * so initial = (CAS - 3) + 4 = CAS + 1.
	 * Verified at FFFF4E7E: movzx edx, byte [eax+0x22] ; add dl, 4
	 */
	t.coarse_high = si->timings.CAS + 1;
	t.coarse_low = 0;
	t.fine = 0;
	t.config = 4;

	/*
	 * Compute rank address for memory test reads.
	 * BIOS: FFFF4E83-FFFF4EB4
	 *   ch0: rank_addr = 0
	 *   ch1 + dual-channel: rank_addr = 0x40
	 *   ch1 + single-channel: rank_addr = MCHBAR16(0x1206) << 25
	 */
	uintptr_t rank_addr = 0;
	if (ch == 1) {
		if (si->timings.channel_mode != CHANNEL_MODE_SINGLE) {
			rank_addr = 0x40;
		} else {
			rank_addr = (uint32_t)mchbar_read16(0x1206) << 25;
		}
	}

	/*
	 * Arm per-channel training comparator.
	 * CxTRAIN_CFG: set bit 3, preserve only bit 0, clear rest.
	 * BIOS: (val & 0x01) | 0x08
	 */
	mchbar_clrsetbits8(CxTRAIN_CFG(ch), 0xfe, 1 << 3);

	/* Program initial coarse timing */
	program_rec_timing(ch, &t);

	/*
	 * Phase 1: Coarse sweep up - find DQS falling edge
	 * BIOS: FFF00D5C -> FFF00B1F (test_dqs_level HIGH)
	 */
	if (coarse_sweep_up(ch, &t, rank_addr) != 1)
		die_with_post_code(POSTCODE_RAM_FAILURE, "REC: coarse sweep up failed");

	/*
	 * Phase 2: Fine sweep - find precise LOW->HIGH edge
	 * BIOS: FFF00CB2 -> FFF00B1F (overshoot) + FFF00B77 (fine)
	 */
	if (fine_sweep(ch, &t, rank_addr) != 1)
		die_with_post_code(POSTCODE_RAM_FAILURE, "REC: fine sweep failed");

	/*
	 * Phase 3: Coarse sweep down - find lower window boundary
	 * BIOS: FFF00C67 -> FFF00B1F (test_dqs_level HIGH)
	 */
	coarse_sub(&t, 3);
	program_rec_timing(ch, &t);

	if (coarse_sweep_down(ch, &t, rank_addr) != 1)
		die_with_post_code(POSTCODE_RAM_FAILURE, "REC: coarse sweep down failed");

	/*
	 * Final: add 1 to combined coarse (center of window).
	 * Verify coarse_high <= 15, then write registers.
	 */
	coarse_add(&t, 1);
	if (t.coarse_high > 0x0f)
		die_with_post_code(POSTCODE_RAM_FAILURE, "REC: final bounds exceeded");

	program_rec_timing(ch, &t);

	/* Store results for CMOS cache / S3 resume restore */
	si->rec_coarse[ch] = t.coarse_high;
	si->rec_coarse_low[ch] = t.coarse_low;
	si->rec_fine[ch] = t.fine;

	/*
	 * Write final value to CxDRT5 bits [7:4] = (coarse_high - 1).
	 * BIOS: 8-bit RMW at MCHBAR+0x1224+ch*0x100.
	 */
	uint8_t drt5 = mchbar_read8(CxDRT5_MCHBAR(ch));
	drt5 = (drt5 & 0x0f) | (((t.coarse_high - 1) & 0x0f) << 4);
	mchbar_write8(CxDRT5_MCHBAR(ch), drt5);

	printk(BIOS_DEBUG, "REC ch%d: coarse_high=%d coarse_low=%d fine=%d "
	       "DRT3=0x%08x DRT5=0x%02x\n",
	       ch, t.coarse_high, t.coarse_low, t.fine,
	       mchbar_read32(CxDRT3_MCHBAR(ch)),
	       mchbar_read8(CxDRT5_MCHBAR(ch)));
}

/*
 * raminit_program_training() - Write cached training results to hardware.
 *
 * Called on warm boot / S3 resume instead of receive_enable_training().
 * Programs the same registers that per_channel_rec_training() would,
 * then runs the same post-training cleanup sequence.
 */
void raminit_program_training(sysinfo_t *si)
{
	int ch;

	for (ch = 0; ch < 2; ch++) {
		int slot = ch * 2;
		if (!(slot < 4 && si->dimms[slot].present))
			continue;

		rec_timing_t t;
		t.coarse_high = si->rec_coarse[ch];
		t.coarse_low  = si->rec_coarse_low[ch];
		t.fine        = si->rec_fine[ch];
		t.config      = 4;

		/* Program coarse timing to CxDRT3 + REC_COARSE_LOW */
		program_rec_timing(ch, &t);

		/* Program fine delay to CxTRAIN_CFG */
		write_fine_delay(ch, t.fine);

		/* Write CxDRT5 bits [7:4] = coarse_high - 1 */
		uint8_t drt5 = mchbar_read8(CxDRT5_MCHBAR(ch));
		drt5 = (drt5 & 0x0f) | (((t.coarse_high - 1) & 0x0f) << 4);
		mchbar_write8(CxDRT5_MCHBAR(ch), drt5);
	}

	/* Post-training cleanup - matches vendor FFFF4F9D */
	mchbar_setbits16(RW_PTR_CTRL(0), 0x0c00);
	mchbar_setbits16(RW_PTR_CTRL(1), 0x0c00);
	mchbar_clrbits32(TRAIN_ENABLE(0), TRAIN_ENABLE_BIT);
	mchbar_clrbits32(TRAIN_ENABLE(1), TRAIN_ENABLE_BIT);
	mchbar_setbits8(CxDRC1_MCHBAR(0), 0x40);
	mchbar_setbits8(CxDRC1_MCHBAR(1), 0x40);
	mchbar_clrbits8(CxDRC1_MCHBAR(0), 0x40);
	mchbar_clrbits8(CxDRC1_MCHBAR(1), 0x40);
	mchbar_clrbits16(RW_PTR_CTRL(0), 1 << 9);
	mchbar_setbits16(RW_PTR_CTRL(0), 0x0600);
	mchbar_clrbits16(RW_PTR_CTRL(1), 1 << 9);
	mchbar_setbits16(RW_PTR_CTRL(1), 0x0600);
}

/*
 * receive_enable_training() - Run 3-phase REC training for all channels
 * BIOS: FFFF4F9D (orchestrator, 283 bytes)
 *
 * Pre-training setup:
 *   1. Enable training mode (TRAIN_ENABLE bit 31)
 *   2. Program IO_INIT_CFG2 bits [27:23] from freq-dependent table
 *   3. Clear RW_PTR_CTRL bits [11:10] (16-bit AND 0xF3FF)
 *   4. Clear DCC bit 15 (16-bit AND 0x7FFF)
 *
 * Post-training cleanup (verified 12-step sequence):
 *   1-2. Set RW_PTR_CTRL bits [11:10] on both channels
 *   3-4. Clear TRAIN_ENABLE bit 31 on both channels
 *   5-8. Toggle CxDRC1 bit 6 (set then clear) on both channels
 *   9-12. Clear bit 9, then set bits [10:9] of RW_PTR_CTRL
 */
void receive_enable_training(sysinfo_t *si)
{
	int ch;

	/* === Pre-training setup === */

	/* Enable training mode on both channels */
	mchbar_setbits32(TRAIN_ENABLE(0), TRAIN_ENABLE_BIT);
	mchbar_setbits32(TRAIN_ENABLE(1), TRAIN_ENABLE_BIT);

	/*
	 * Write frequency-dependent value to IO_INIT_CFG2 bits [27:23].
	 * BIOS: RMW with mask 0xF07FFFFF, shift << 23.
	 */
	mchbar_clrsetbits32(IO_INIT_CFG2,
		0x0f800000,  /* clear bits [27:23] */
		(uint32_t)io_init_cfg2_training[si->timings.mem_clock] << 23);

	/* Clear RW_PTR_CTRL bits [11:10] - 16-bit AND 0xF3FF */
	mchbar_clrbits16(RW_PTR_CTRL(0), 0x0c00);
	mchbar_clrbits16(RW_PTR_CTRL(1), 0x0c00);

	/* Clear DCC bit 15 - 16-bit AND 0x7FFF */
	mchbar_clrbits16(DCC_MCHBAR, 0x8000);

	/* === Per-channel training === */

	for (ch = 0; ch < 2; ch++) {
		int slot = ch * 2;
		if (!(slot < 4 && si->dimms[slot].present))
			continue;
		per_channel_rec_training(ch, si);
	}

	/*
	 * Post-training cleanup - vendor RAMINIT FFF00E99 (confirmed
	 * from disassembly at fff00f19-fff00fbc, identical 12-step
	 * sequence to RAMINIT3 FFFF4F9D).
	 *
	 * Order: RW_PTR set bits -> training disable -> CxDRC1 toggle ->
	 * final RW_PTR clear-bit9/set-bits.
	 */

	/* 1-2: Set RW_PTR_CTRL bits [11:10] */
	mchbar_setbits16(RW_PTR_CTRL(0), 0x0c00);
	mchbar_setbits16(RW_PTR_CTRL(1), 0x0c00);

	/* 3-4: Disable training mode */
	mchbar_clrbits32(TRAIN_ENABLE(0), TRAIN_ENABLE_BIT);
	mchbar_clrbits32(TRAIN_ENABLE(1), TRAIN_ENABLE_BIT);

	/* 5-8: Toggle CxDRC1 bit 6 - SET then CLEAR */
	mchbar_setbits8(CxDRC1_MCHBAR(0), 0x40);
	mchbar_setbits8(CxDRC1_MCHBAR(1), 0x40);
	mchbar_clrbits8(CxDRC1_MCHBAR(0), 0x40);
	mchbar_clrbits8(CxDRC1_MCHBAR(1), 0x40);

	/* 9-12: RW_PTR_CTRL - clear bit 9, then set bits [10:9] */
	mchbar_clrbits16(RW_PTR_CTRL(0), 1 << 9);
	mchbar_setbits16(RW_PTR_CTRL(0), 0x0600);
	mchbar_clrbits16(RW_PTR_CTRL(1), 1 << 9);
	mchbar_setbits16(RW_PTR_CTRL(1), 0x0600);
}
