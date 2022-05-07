/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"

static void ldat_write_cacheline(
	const struct sysinfo *const ctrl,
	const uint8_t chunk,
	const uint16_t start,
	const uint64_t data)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		/*
		 * Do not do a 64-bit write here. The register is not aligned
		 * to a 64-bit boundary, which could potentially cause issues.
		 */
		mchbar_write32(QCLK_ch_LDAT_DATA_IN_x(channel, 0), data & UINT32_MAX);
		mchbar_write32(QCLK_ch_LDAT_DATA_IN_x(channel, 1), data >> 32);
		/*
		 * Set REPLICATE = 0 as you don't want to replicate the data.
		 * Set BANK_SEL to the chunk you want to write the 64 bits to.
		 * Set ARRAY_SEL = 0 (the MC WDB) and MODE = 1.
		 */
		const union ldat_sdat_reg ldat_sdat = {
			.bank_sel = chunk,
			.mode     = 1,
		};
		mchbar_write32(QCLK_ch_LDAT_SDAT(channel), ldat_sdat.raw);
		/*
		 * Finally, write the PDAT register indicating which cacheline
		 * of the WDB you want to write to by setting FAST_ADDR field
		 * to one of the 64 cache lines. Also set CMD_B in the PDAT
		 * register to 4'b1000, indicating that this is a LDAT write.
		 */
		const union ldat_pdat_reg ldat_pdat = {
			.fast_addr = MIN(start, 0xfff),
			.cmd_b     = 8,
		};
		mchbar_write32(QCLK_ch_LDAT_PDAT(channel), ldat_pdat.raw);
	}
}

static void clear_ldat_mode(const struct sysinfo *const ctrl)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
		mchbar_write32(QCLK_ch_LDAT_SDAT(channel), 0);
}

void write_wdb_fixed_pat(
	const struct sysinfo *const ctrl,
	const uint8_t patterns[],
	const uint8_t pat_mask[],
	const uint8_t spread,
	const uint16_t start)
{
	for (uint8_t chunk = 0; chunk < WDB_CACHE_LINE_SIZE; chunk++) {
		uint64_t data = 0;
		for (uint8_t b = 0; b < 64; b++) {
			const uint8_t beff  = b % spread;
			const uint8_t burst = patterns[pat_mask[beff]];
			if (burst & BIT(chunk))
				data |= 1ULL << b;
		}
		ldat_write_cacheline(ctrl, chunk, start, data);
	}
	clear_ldat_mode(ctrl);
}

static inline uint32_t rol_u32(const uint32_t val)
{
	return (val << 1) | ((val >> 31) & 1);
}

void write_wdb_va_pat(
	const struct sysinfo *const ctrl,
	const uint32_t agg_mask,
	const uint32_t vic_mask,
	const uint8_t vic_rot,
	const uint16_t start)
{
	static const uint8_t va_mask_to_compressed[4] = {0xaa, 0xc0, 0xcc, 0xf0};
	uint32_t v_mask = vic_mask;
	uint32_t a_mask = agg_mask;
	for (uint8_t v = 0; v < vic_rot; v++) {
		uint8_t compressed[32] = {0};
		/* Iterate through all 32 bits and create a compressed version of cacheline */
		for (uint8_t b = 0; b < ARRAY_SIZE(compressed); b++) {
			const uint8_t vic = !!(v_mask & BIT(b));
			const uint8_t agg = !!(a_mask & BIT(b));
			const uint8_t index = !vic << 1 | agg << 0;
			compressed[b] = va_mask_to_compressed[index];
		}
		for (uint8_t chunk = 0; chunk < WDB_CACHE_LINE_SIZE; chunk++) {
			uint32_t data = 0;
			for (uint8_t b = 0; b < ARRAY_SIZE(compressed); b++)
				data |= !!(compressed[b] & BIT(chunk)) << b;

			const uint64_t data64 = (uint64_t)data << 32 | data;
			ldat_write_cacheline(ctrl, chunk, start + v, data64);
		}
		v_mask = rol_u32(v_mask);
		a_mask = rol_u32(a_mask);
	}
	clear_ldat_mode(ctrl);
}

void program_wdb_lfsr(const struct sysinfo *ctrl, const bool cleanup)
{
	/* Cleanup LFSR seeds are sequential */
	const uint32_t cleanup_seeds[NUM_WDB_CL_MUX_SEEDS] = { 0xaaaaaa, 0xcccccc, 0xf0f0f0 };
	const uint32_t regular_seeds[NUM_WDB_CL_MUX_SEEDS] = { 0xa10ca1, 0xef0d08, 0xad0a1e };
	const uint32_t *seeds = cleanup ? cleanup_seeds : regular_seeds;

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		for (uint8_t i = 0; i < NUM_WDB_CL_MUX_SEEDS; i++) {
			mchbar_write32(REUT_ch_PAT_WDB_CL_MUX_RD_x(channel, i), seeds[i]);
			mchbar_write32(REUT_ch_PAT_WDB_CL_MUX_WR_x(channel, i), seeds[i]);
		}
	}
}

void setup_wdb(const struct sysinfo *ctrl)
{
	const uint32_t amask[9] = {
		0x86186186, 0x18618618, 0x30c30c30,
		0xa28a28a2, 0x8a28a28a, 0x14514514,
		0x28a28a28, 0x92492492, 0x24924924,
	};
	const uint32_t vmask = 0x41041041;

	/* Fill first 8 entries with simple 2-LFSR VA pattern */
	write_wdb_va_pat(ctrl, 0, BASIC_VA_PAT_SPREAD_8, 8, 0);

	/* Fill next 54 entries with 3-LFSR VA pattern */
	for (uint8_t a = 0; a < ARRAY_SIZE(amask); a++)
		write_wdb_va_pat(ctrl, amask[a], vmask, 6, 8 + a * 6);

	program_wdb_lfsr(ctrl, false);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel))
			continue;

		const union reut_pat_cl_mux_lmn_reg wdb_cl_mux_lmn = {
			.en_sweep_freq = 1,
			.l_counter     = 1,
			.m_counter     = 1,
			.n_counter     = 10,
		};
		mchbar_write32(REUT_ch_PAT_WDB_CL_MUX_LMN(channel), wdb_cl_mux_lmn.raw);
	}
}
