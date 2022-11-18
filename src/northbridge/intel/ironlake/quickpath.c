/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/intel/model_2065x/model_2065x.h>
#include <cpu/x86/msr.h>
#include <delay.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <northbridge/intel/ironlake/raminit.h>
#include <types.h>

#define NORTHBRIDGE PCI_DEV(0, 0, 0)

static unsigned int gcd(unsigned int a, unsigned int b)
{
	unsigned int t;
	if (a > b) {
		t = a;
		a = b;
		b = t;
	}
	/* invariant a < b.  */
	while (a) {
		t = b % a;
		b = a;
		a = t;
	}
	return b;
}

static inline int div_roundup(int a, int b)
{
	return DIV_ROUND_UP(a, b);
}

static unsigned int lcm(unsigned int a, unsigned int b)
{
	return (a * b) / gcd(a, b);
}

struct stru1 {
	u8 freqs_reversed;
	u8 freq_diff_reduced;
	u8 freq_min_reduced;
	u8 divisor_f4_to_fmax;
	u8 divisor_f3_to_fmax;
	u8 freq4_to_max_remainder;
	u8 freq3_to_2_remainder;
	u8 freq3_to_2_remaindera;
	u8 freq4_to_2_remainder;
	int divisor_f3_to_f1, divisor_f4_to_f2;
	int common_time_unit_ps;
	int freq_max_reduced;
};

static void
compute_frequence_ratios(struct raminfo *info, u16 freq1, u16 freq2,
			 int num_cycles_2, int num_cycles_1, int round_it,
			 int add_freqs, struct stru1 *result)
{
	int g;
	int common_time_unit_ps;
	int freq1_reduced, freq2_reduced;
	int freq_min_reduced;
	int freq_max_reduced;
	int freq3, freq4;

	g = gcd(freq1, freq2);
	freq1_reduced = freq1 / g;
	freq2_reduced = freq2 / g;
	freq_min_reduced = MIN(freq1_reduced, freq2_reduced);
	freq_max_reduced = MAX(freq1_reduced, freq2_reduced);

	common_time_unit_ps = div_roundup(900000, lcm(freq1, freq2));
	freq3 = div_roundup(num_cycles_2, common_time_unit_ps) - 1;
	freq4 = div_roundup(num_cycles_1, common_time_unit_ps) - 1;
	if (add_freqs) {
		freq3 += freq2_reduced;
		freq4 += freq1_reduced;
	}

	if (round_it) {
		result->freq3_to_2_remainder = 0;
		result->freq3_to_2_remaindera = 0;
		result->freq4_to_max_remainder = 0;
		result->divisor_f4_to_f2 = 0;
		result->divisor_f3_to_f1 = 0;
	} else {
		if (freq2_reduced < freq1_reduced) {
			result->freq3_to_2_remainder =
			    result->freq3_to_2_remaindera =
			    freq3 % freq1_reduced - freq1_reduced + 1;
			result->freq4_to_max_remainder =
			    -(freq4 % freq1_reduced);
			result->divisor_f3_to_f1 = freq3 / freq1_reduced;
			result->divisor_f4_to_f2 =
			    (freq4 -
			     (freq1_reduced - freq2_reduced)) / freq2_reduced;
			result->freq4_to_2_remainder =
			    -(char)((freq1_reduced - freq2_reduced) +
				    ((u8)freq4 -
				     (freq1_reduced -
				      freq2_reduced)) % (u8)freq2_reduced);
		} else {
			if (freq2_reduced > freq1_reduced) {
				result->freq4_to_max_remainder =
				    (freq4 % freq2_reduced) - freq2_reduced + 1;
				result->freq4_to_2_remainder =
				    freq4 % freq_max_reduced -
				    freq_max_reduced + 1;
			} else {
				result->freq4_to_max_remainder =
				    -(freq4 % freq2_reduced);
				result->freq4_to_2_remainder =
				    -(char)(freq4 % freq_max_reduced);
			}
			result->divisor_f4_to_f2 = freq4 / freq2_reduced;
			result->divisor_f3_to_f1 =
			    (freq3 -
			     (freq2_reduced - freq1_reduced)) / freq1_reduced;
			result->freq3_to_2_remainder = -(freq3 % freq2_reduced);
			result->freq3_to_2_remaindera =
			    -(char)((freq_max_reduced - freq_min_reduced) +
				    (freq3 -
				     (freq_max_reduced -
				      freq_min_reduced)) % freq1_reduced);
		}
	}
	result->divisor_f3_to_fmax = freq3 / freq_max_reduced;
	result->divisor_f4_to_fmax = freq4 / freq_max_reduced;
	if (round_it) {
		if (freq2_reduced > freq1_reduced) {
			if (freq3 % freq_max_reduced)
				result->divisor_f3_to_fmax++;
		}
		if (freq2_reduced < freq1_reduced) {
			if (freq4 % freq_max_reduced)
				result->divisor_f4_to_fmax++;
		}
	}
	result->freqs_reversed = (freq2_reduced < freq1_reduced);
	result->freq_diff_reduced = freq_max_reduced - freq_min_reduced;
	result->freq_min_reduced = freq_min_reduced;
	result->common_time_unit_ps = common_time_unit_ps;
	result->freq_max_reduced = freq_max_reduced;
}

static void compute_274265(struct raminfo *info)
{
	int delay_a_ps, delay_b_ps, delay_c_ps, delay_d_ps;
	int delay_e_ps, delay_e_cycles, delay_f_cycles;
	int delay_e_over_cycle_ps;
	int cycletime_ps;
	int channel;

	delay_a_ps = 4 * halfcycle_ps(info) + 6 * fsbcycle_ps(info);
	info->training.reg2ca9_bit0 = 0;
	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		cycletime_ps =
		    900000 / lcm(2 * info->fsb_frequency, frequency_11(info));
		delay_d_ps =
		    (halfcycle_ps(info) * get_max_timing(info, channel) >> 6)
		    - info->some_delay_3_ps_rounded + 200;
		if (!
		    ((info->silicon_revision == 0
		      || info->silicon_revision == 1)
		     && (info->revision >= 8)))
			delay_d_ps += halfcycle_ps(info) * 2;
		delay_d_ps +=
		    halfcycle_ps(info) * (!info->revision_flag_1 +
					  info->some_delay_2_halfcycles_ceil +
					  2 * info->some_delay_1_cycle_floor +
					  info->clock_speed_index +
					  2 * info->cas_latency - 7 + 11);
		delay_d_ps += info->revision >= 8 ? 2758 : 4428;

		mchbar_clrsetbits32(0x140, 7 << 24, 2 << 24);
		mchbar_clrsetbits32(0x138, 7 << 24, 2 << 24);
		if ((mchbar_read8(0x144) & 0x1f) > 0x13)
			delay_d_ps += 650;
		delay_c_ps = delay_d_ps + 1800;
		if (delay_c_ps <= delay_a_ps)
			delay_e_ps = 0;
		else
			delay_e_ps =
			    cycletime_ps * div_roundup(delay_c_ps - delay_a_ps,
						       cycletime_ps);

		delay_e_over_cycle_ps = delay_e_ps % (2 * halfcycle_ps(info));
		delay_e_cycles = delay_e_ps / (2 * halfcycle_ps(info));
		delay_f_cycles =
		    div_roundup(2500 - delay_e_over_cycle_ps,
				2 * halfcycle_ps(info));
		if (delay_f_cycles > delay_e_cycles) {
			info->delay46_ps[channel] = delay_e_ps;
			delay_e_cycles = 0;
		} else {
			info->delay46_ps[channel] =
			    delay_e_over_cycle_ps +
			    2 * halfcycle_ps(info) * delay_f_cycles;
			delay_e_cycles -= delay_f_cycles;
		}

		if (info->delay46_ps[channel] < 2500) {
			info->delay46_ps[channel] = 2500;
			info->training.reg2ca9_bit0 = 1;
		}
		delay_b_ps = halfcycle_ps(info) + delay_c_ps;
		if (delay_b_ps <= delay_a_ps)
			delay_b_ps = 0;
		else
			delay_b_ps -= delay_a_ps;
		info->delay54_ps[channel] =
		    cycletime_ps * div_roundup(delay_b_ps,
					       cycletime_ps) -
		    2 * halfcycle_ps(info) * delay_e_cycles;
		if (info->delay54_ps[channel] < 2500)
			info->delay54_ps[channel] = 2500;
		info->training.reg274265[channel][0] = delay_e_cycles;
		if (delay_d_ps + 7 * halfcycle_ps(info) <=
		    24 * halfcycle_ps(info))
			info->training.reg274265[channel][1] = 0;
		else
			info->training.reg274265[channel][1] =
				div_roundup(delay_d_ps + 7 * halfcycle_ps(info),
				4 * halfcycle_ps(info)) - 6;
		info->training.reg274265[channel][2] =
			div_roundup(delay_c_ps + 3 * fsbcycle_ps(info),
			4 * halfcycle_ps(info)) + 1;
	}
}

static void program_274265(const struct ram_training *const training)
{
	int channel;

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_write32((channel << 10) + 0x274,
			(training->reg274265[channel][0] << 16) |
			training->reg274265[channel][1]);
		mchbar_write16((channel << 10) + 0x265,
			training->reg274265[channel][2] << 8);
	}
	if (training->reg2ca9_bit0)
		mchbar_setbits8(0x2ca9, 1 << 0);
	else
		mchbar_clrbits8(0x2ca9, 1 << 0);

	printk(RAM_SPEW, "reg2ca9_bit0 = %x\n", training->reg2ca9_bit0);

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 3; j++)
			printk(RAM_SPEW, "reg274265[%d][%d] = %x\n",
			       i, j, training->reg274265[i][j]);
}

static void
set_2d5x_reg(struct raminfo *info, u16 reg, u16 freq1, u16 freq2,
	     int num_cycles_2, int num_cycles_1, int num_cycles_3,
	     int num_cycles_4, int reverse)
{
	struct stru1 vv;
	char multiplier;

	compute_frequence_ratios(info, freq1, freq2, num_cycles_2, num_cycles_1,
				 0, 1, &vv);

	multiplier =
	    div_roundup(MAX
			(div_roundup(num_cycles_2, vv.common_time_unit_ps) +
			 div_roundup(num_cycles_3, vv.common_time_unit_ps),
			 div_roundup(num_cycles_1,
				     vv.common_time_unit_ps) +
			 div_roundup(num_cycles_4, vv.common_time_unit_ps))
			+ vv.freq_min_reduced - 1, vv.freq_max_reduced) - 1;

	u32 y =
	    (u8)((vv.freq_max_reduced - vv.freq_min_reduced) +
		  vv.freq_max_reduced * multiplier)
	    | (vv.
	       freqs_reversed << 8) | ((u8)(vv.freq_min_reduced *
					     multiplier) << 16) | ((u8)(vv.
									 freq_min_reduced
									 *
									 multiplier)
								   << 24);
	u32 x =
	    vv.freq3_to_2_remaindera | (vv.freq4_to_2_remainder << 8) | (vv.
									 divisor_f3_to_f1
									 << 16)
	    | (vv.divisor_f4_to_f2 << 20) | (vv.freq_min_reduced << 24);
	if (reverse) {
		mchbar_write32(reg + 0, y);
		mchbar_write32(reg + 4, x);
	} else {
		mchbar_write32(reg + 4, y);
		mchbar_write32(reg + 0, x);
	}
}

static void
set_6d_reg(struct raminfo *info, u16 reg, u16 freq1, u16 freq2,
	   int num_cycles_1, int num_cycles_2, int num_cycles_3,
	   int num_cycles_4)
{
	struct stru1 ratios1;
	struct stru1 ratios2;

	compute_frequence_ratios(info, freq1, freq2, num_cycles_1, num_cycles_2,
				 0, 1, &ratios2);
	compute_frequence_ratios(info, freq1, freq2, num_cycles_3, num_cycles_4,
				 0, 1, &ratios1);
	printk(RAM_SPEW, "[%x] <= %x\n", reg,
		       ratios1.freq4_to_max_remainder | (ratios2.
							 freq4_to_max_remainder
							 << 8)
		       | (ratios1.divisor_f4_to_fmax << 16) | (ratios2.
							       divisor_f4_to_fmax
							       << 20));
	mchbar_write32(reg, ratios1.freq4_to_max_remainder |
		ratios2.freq4_to_max_remainder << 8 |
		ratios1.divisor_f4_to_fmax << 16 |
		ratios2.divisor_f4_to_fmax << 20);
}

static void
set_2dx8_reg(struct raminfo *info, u16 reg, u8 mode, u16 freq1, u16 freq2,
	     int num_cycles_2, int num_cycles_1, int round_it, int add_freqs)
{
	struct stru1 ratios;

	compute_frequence_ratios(info, freq1, freq2, num_cycles_2, num_cycles_1,
				 round_it, add_freqs, &ratios);
	switch (mode) {
	case 0:
		mchbar_write32(reg + 4,
			ratios.freq_diff_reduced |
			ratios.freqs_reversed << 8);
		mchbar_write32(reg,
			ratios.freq3_to_2_remainder |
			ratios.freq4_to_max_remainder << 8 |
			ratios.divisor_f3_to_fmax << 16 |
			ratios.divisor_f4_to_fmax << 20 |
			ratios.freq_min_reduced << 24);
		break;

	case 1:
		mchbar_write32(reg,
			ratios.freq3_to_2_remainder |
			ratios.divisor_f3_to_fmax << 16);
		break;

	case 2:
		mchbar_write32(reg,
			ratios.freq3_to_2_remainder |
			ratios.freq4_to_max_remainder << 8 |
			ratios.divisor_f3_to_fmax << 16 |
			ratios.divisor_f4_to_fmax << 20);
		break;

	case 4:
		mchbar_write32(reg,
			ratios.divisor_f3_to_fmax << 4 |
			ratios.divisor_f4_to_fmax << 8 |
			ratios.freqs_reversed << 12 |
			ratios.freq_min_reduced << 16 |
			ratios.freq_diff_reduced << 24);
		break;
	}
}

static void set_2dxx_series(struct raminfo *info, int s3resume)
{
	set_2dx8_reg(info, 0x2d00, 0, 0x78, frequency_11(info) / 2, 1359, 1005,
		     0, 1);
	set_2dx8_reg(info, 0x2d08, 0, 0x78, 0x78, 3273, 5033, 1, 1);
	set_2dx8_reg(info, 0x2d10, 0, 0x78, info->fsb_frequency, 1475, 1131, 0,
		     1);
	set_2dx8_reg(info, 0x2d18, 0, 2 * info->fsb_frequency,
		     frequency_11(info), 1231, 1524, 0, 1);
	set_2dx8_reg(info, 0x2d20, 0, 2 * info->fsb_frequency,
		     frequency_11(info) / 2, 1278, 2008, 0, 1);
	set_2dx8_reg(info, 0x2d28, 0, info->fsb_frequency, frequency_11(info),
		     1167, 1539, 0, 1);
	set_2dx8_reg(info, 0x2d30, 0, info->fsb_frequency,
		     frequency_11(info) / 2, 1403, 1318, 0, 1);
	set_2dx8_reg(info, 0x2d38, 0, info->fsb_frequency, 0x78, 3460, 5363, 1,
		     1);
	set_2dx8_reg(info, 0x2d40, 0, info->fsb_frequency, 0x3c, 2792, 5178, 1,
		     1);
	set_2dx8_reg(info, 0x2d48, 0, 2 * info->fsb_frequency, 0x78, 2738, 4610,
		     1, 1);
	set_2dx8_reg(info, 0x2d50, 0, info->fsb_frequency, 0x78, 2819, 5932, 1,
		     1);
	set_2dx8_reg(info, 0x6d4, 1, info->fsb_frequency,
		     frequency_11(info) / 2, 4000, 0, 0, 0);
	set_2dx8_reg(info, 0x6d8, 2, info->fsb_frequency,
		     frequency_11(info) / 2, 4000, 4000, 0, 0);

	if (s3resume) {
		printk(RAM_SPEW, "[6dc] <= %x\n",
			info->cached_training->reg_6dc);
		mchbar_write32(0x6dc, info->cached_training->reg_6dc);
	} else
		set_6d_reg(info, 0x6dc, 2 * info->fsb_frequency, frequency_11(info), 0,
			   info->delay46_ps[0], 0,
			   info->delay54_ps[0]);
	set_2dx8_reg(info, 0x6e0, 1, 2 * info->fsb_frequency,
		     frequency_11(info), 2500, 0, 0, 0);
	set_2dx8_reg(info, 0x6e4, 1, 2 * info->fsb_frequency,
		     frequency_11(info) / 2, 3500, 0, 0, 0);
	if (s3resume) {
		printk(RAM_SPEW, "[6e8] <= %x\n",
			info->cached_training->reg_6e8);
		mchbar_write32(0x6e8, info->cached_training->reg_6e8);
	} else
		set_6d_reg(info, 0x6e8, 2 * info->fsb_frequency, frequency_11(info), 0,
			   info->delay46_ps[1], 0,
			   info->delay54_ps[1]);
	set_2d5x_reg(info, 0x2d58, 0x78, 0x78, 864, 1195, 762, 786, 0);
	set_2d5x_reg(info, 0x2d60, 0x195, info->fsb_frequency, 1352, 725, 455,
		     470, 0);
	set_2d5x_reg(info, 0x2d68, 0x195, 0x3c, 2707, 5632, 3277, 2207, 0);
	set_2d5x_reg(info, 0x2d70, 0x195, frequency_11(info) / 2, 1276, 758,
		     454, 459, 0);
	set_2d5x_reg(info, 0x2d78, 0x195, 0x78, 1021, 799, 510, 513, 0);
	set_2d5x_reg(info, 0x2d80, info->fsb_frequency, 0xe1, 0, 2862, 2579,
		     2588, 0);
	set_2d5x_reg(info, 0x2d88, info->fsb_frequency, 0xe1, 0, 2690, 2405,
		     2405, 0);
	set_2d5x_reg(info, 0x2da0, 0x78, 0xe1, 0, 2560, 2264, 2251, 0);
	set_2d5x_reg(info, 0x2da8, 0x195, frequency_11(info), 1060, 775, 484,
		     480, 0);
	set_2d5x_reg(info, 0x2db0, 0x195, 0x78, 4183, 6023, 2217, 2048, 0);
	mchbar_write32(0x2dbc, ((frequency_11(info) / 2) - 1) | 0xe00000);
	mchbar_write32(0x2db8, (info->fsb_frequency - 1) << 16 | 0x77);
}

static u16 quickpath_configure_pll_ratio(struct raminfo *info, const u8 x2ca8)
{
	mchbar_setbits32(0x18b4, 1 << 21 | 1 << 16);
	mchbar_setbits32(0x1890, 1 << 25);
	mchbar_setbits32(0x18b4, 1 << 15);

	/* Get maximum supported PLL ratio */
	u16 qpi_pll_ratio = (pci_read_config32(QPI_PHY_0, QPI_PLL_STATUS) >> 24 & 0x7f);

	/* Adjust value if invalid */
	if (qpi_pll_ratio == 0 || qpi_pll_ratio > 40)
		qpi_pll_ratio = 40;

	if (qpi_pll_ratio == 16)
		qpi_pll_ratio = 12;

	while (qpi_pll_ratio >= 12) {
		if (qpi_pll_ratio <= (info->clock_speed_index + 3) * 8)
			break;

		qpi_pll_ratio -= 2;
	}

	/* Finally, program the ratio */
	pci_write_config8(QPI_PHY_0, QPI_PLL_RATIO, qpi_pll_ratio);

	const u16 csipll0 = mchbar_read16(0x2c10);
	mchbar_write16(0x2c10, (qpi_pll_ratio > 26) << 11 | 1 << 10 | qpi_pll_ratio);

	if (csipll0 != mchbar_read16(0x2c10) && x2ca8 == 0)
		mchbar_setbits8(0x2ca8, 1 << 0);

	mchbar_setbits16(0x2c12, 1 << 8);

	return qpi_pll_ratio;
}

void early_quickpath_init(struct raminfo *info, const u8 x2ca8)
{
	u8 reg8;
	u32 reg32;

	/* Initialize DDR MPLL first */
	if (x2ca8 == 0) {
		mchbar_clrsetbits8(0x164, 0x26, info->clock_speed_index == 0 ? 0x24 : 0x26);

		/* Program DDR MPLL feedback divider ratio */
		mchbar_write16(0x2c20, (info->clock_speed_index + 3) * 4);
	}

	const u16 qpi_pll_ratio = quickpath_configure_pll_ratio(info, x2ca8);

	mchbar_clrsetbits32(0x1804, 0x3, 0x8400080);
	pci_update_config32(QPI_PHY_0, QPI_PHY_CONTROL, 0xfffffffc, 0x400080);

	const u32 x1c04 = mchbar_read32(0x1c04) & 0xc01080;
	const u32 x1804 = mchbar_read32(0x1804) & 0xc01080;

	if (x1c04 != x1804 && x2ca8 == 0)
		mchbar_setbits8(0x2ca8, 1 << 0);

	reg32 = 0x3000000;
	if (info->revision >= 0x18 && qpi_pll_ratio <= 12) {
		/* Get TDP limit in 1/8W units */
		const msr_t msr = rdmsr(MSR_TURBO_POWER_CURRENT_LIMIT);
		if ((msr.lo & 0x7fff) <= 90)
			reg32 = 0;
	}
	mchbar_write32(0x18d8, 0x120000);
	mchbar_write32(0x18dc, reg32 | 0xa484a);

	reg32 = qpi_pll_ratio > 20 ? 8 : 16;
	pci_write_config32(QPI_PHY_0, QPI_PHY_EP_SELECT, 0x0);
	pci_write_config32(QPI_PHY_0, QPI_PHY_EP_MCTR, 0x9404a | reg32 << 7);

	mchbar_write32(0x18d8, 0x40000);
	mchbar_write32(0x18dc, 0xb000000);
	pci_write_config32(QPI_PHY_0, QPI_PHY_EP_SELECT, 0x60000);
	pci_write_config32(QPI_PHY_0, QPI_PHY_EP_MCTR, 0x0);
	mchbar_write32(0x18d8, 0x180000);
	mchbar_write32(0x18dc, 0xc0000142);
	pci_write_config32(QPI_PHY_0, QPI_PHY_EP_SELECT, 0x20000);
	pci_write_config32(QPI_PHY_0, QPI_PHY_EP_MCTR, 0x142);
	mchbar_write32(0x18d8, 0x1e0000);

	const u32 x18dc = mchbar_read32(0x18dc);
	mchbar_write32(0x18dc, qpi_pll_ratio < 18 ? 2 : 3);

	if (x18dc != mchbar_read32(0x18dc) && x2ca8 == 0)
		mchbar_setbits8(0x2ca8, 1 << 0);

	reg8 = qpi_pll_ratio > 20 ? 10 : 9;

	mchbar_write32(0x188c, 0x20bc00 | reg8);
	pci_write_config32(QPI_PHY_0, QPI_PHY_PWR_MGMT, 0x40b0c00 | reg8);

	if (qpi_pll_ratio <= 14)
		reg8 = 0x33;
	else if (qpi_pll_ratio <= 22)
		reg8 = 0x42;
	else
		reg8 = 0x51;

	info->fsb_frequency = qpi_pll_ratio * 15;
	mchbar_write32(0x1a10, reg8 << 24 | info->fsb_frequency);

	if (info->silicon_revision == 2 || info->silicon_revision == 3) {
		mchbar_setbits32(0x18b8, 0x200);
		mchbar_setbits32(0x1918, 0x300);
	}

	if (info->revision > 0x17)
		mchbar_setbits32(0x18b8, 0xc00);

	reg32 = ((qpi_pll_ratio > 20) + 1) << 16;

	mchbar_clrsetbits32(0x182c, ~0xfff0f0ff, reg32 | 0x200);
	pci_update_config32(QPI_PHY_0, QPI_PHY_PRIM_TIMEOUT, 0xfff0f0ff, reg32 | 0x200);
	mchbar_clrbits32(0x1a1c, 7 << 28);
	mchbar_setbits32(0x1a70, 1 << 20);

	mchbar_clrbits32(0x18b4, 1 << 15);
	mchbar_clrsetbits32(0x1a68, 0x00143fc0, 0x143800);

	const u32 x1e68 = mchbar_read32(0x1e68) & 0x143fc0;
	const u32 x1a68 = mchbar_read32(0x1a68) & 0x143fc0;

	if (x1e68 != x1a68 && x2ca8 == 0)
		mchbar_setbits8(0x2ca8, 1 << 0);

	pci_update_config32(QPI_LINK_0, QPI_QPILCL, 0xffffff3f, 0x140000);

	reg32 = pci_read_config32(QPI_LINK_0, QPI_DEF_RMT_VN_CREDITS);
	pci_write_config32(QPI_LINK_0, QPI_DEF_RMT_VN_CREDITS, (reg32 & 0xfffe4555) | 0x64555);

	if (reg32 != pci_read_config32(QPI_LINK_0, QPI_DEF_RMT_VN_CREDITS) && x2ca8 == 0)
		mchbar_setbits8(0x2ca8, 1 << 0);

	pci_update_config32(QPI_NON_CORE, MIRROR_PORT_CTL, ~3, 0x80 * 3);

	reg32 = mchbar_read32(0x1af0);
	mchbar_write32(0x1af0, (reg32 & 0xfdffcf) | 0x1f020000);

	if (reg32 != mchbar_read32(0x1af0) && x2ca8 == 0)
		mchbar_setbits8(0x2ca8, 1 << 0);

	mchbar_clrbits32(0x1890, 1 << 25);
	mchbar_clrsetbits32(0x18b4, 0xf << 12, 0x6 << 12);
	mchbar_write32(0x18a4, 0x22222222);
	mchbar_write32(0x18a8, 0x22222222);
	mchbar_write32(0x18ac, 0x22222);
}

void late_quickpath_init(struct raminfo *info, const int s3resume)
{
	const u16 deven = pci_read_config16(NORTHBRIDGE, DEVEN);

	if (s3resume && info->cached_training) {
		program_274265(info->cached_training);
	} else {
		compute_274265(info);
		program_274265(&info->training);
	}

	set_2dxx_series(info, s3resume);

	if (!(deven & 8)) {
		mchbar_clrsetbits32(0x2cb0, ~0, 0x40);
	}

	udelay(1000);

	if (deven & 8) {
		mchbar_setbits32(0xff8, 3 << 11);
		mchbar_clrbits32(0x2cb0, ~0);
		pci_read_config8(PCI_DEV (0, 0x2, 0x0), 0x4c);
		pci_read_config8(PCI_DEV (0, 0x2, 0x0), 0x4c);
		pci_read_config8(PCI_DEV (0, 0x2, 0x0), 0x4e);

		mchbar_read8(0x1150);
		mchbar_read8(0x1151);
		mchbar_read8(0x1022);
		mchbar_read8(0x16d0);
		mchbar_write32(0x1300, 0x60606060);
		mchbar_write32(0x1304, 0x60606060);
		mchbar_write32(0x1308, 0x78797a7b);
		mchbar_write32(0x130c, 0x7c7d7e7f);
		mchbar_write32(0x1310, 0x60606060);
		mchbar_write32(0x1314, 0x60606060);
		mchbar_write32(0x1318, 0x60606060);
		mchbar_write32(0x131c, 0x60606060);
		mchbar_write32(0x1320, 0x50515253);
		mchbar_write32(0x1324, 0x54555657);
		mchbar_write32(0x1328, 0x58595a5b);
		mchbar_write32(0x132c, 0x5c5d5e5f);
		mchbar_write32(0x1330, 0x40414243);
		mchbar_write32(0x1334, 0x44454647);
		mchbar_write32(0x1338, 0x48494a4b);
		mchbar_write32(0x133c, 0x4c4d4e4f);
		mchbar_write32(0x1340, 0x30313233);
		mchbar_write32(0x1344, 0x34353637);
		mchbar_write32(0x1348, 0x38393a3b);
		mchbar_write32(0x134c, 0x3c3d3e3f);
		mchbar_write32(0x1350, 0x20212223);
		mchbar_write32(0x1354, 0x24252627);
		mchbar_write32(0x1358, 0x28292a2b);
		mchbar_write32(0x135c, 0x2c2d2e2f);
		mchbar_write32(0x1360, 0x10111213);
		mchbar_write32(0x1364, 0x14151617);
		mchbar_write32(0x1368, 0x18191a1b);
		mchbar_write32(0x136c, 0x1c1d1e1f);
		mchbar_write32(0x1370, 0x10203);
		mchbar_write32(0x1374, 0x4050607);
		mchbar_write32(0x1378, 0x8090a0b);
		mchbar_write32(0x137c, 0xc0d0e0f);
		mchbar_write8(0x11cc, 0x4e);
		mchbar_write32(0x1110, 0x73970404);
		mchbar_write32(0x1114, 0x72960404);
		mchbar_write32(0x1118, 0x6f950404);
		mchbar_write32(0x111c, 0x6d940404);
		mchbar_write32(0x1120, 0x6a930404);
		mchbar_write32(0x1124, 0x68a41404);
		mchbar_write32(0x1128, 0x66a21404);
		mchbar_write32(0x112c, 0x63a01404);
		mchbar_write32(0x1130, 0x609e1404);
		mchbar_write32(0x1134, 0x5f9c1404);
		mchbar_write32(0x1138, 0x5c961404);
		mchbar_write32(0x113c, 0x58a02404);
		mchbar_write32(0x1140, 0x54942404);
		mchbar_write32(0x1190, 0x900080a);
		mchbar_write16(0x11c0, 0xc40b);
		mchbar_write16(0x11c2, 0x303);
		mchbar_write16(0x11c4, 0x301);
		mchbar_clrsetbits32(0x1190, ~0, 0x8900080a);
		mchbar_write32(0x11b8, 0x70c3000);
		mchbar_write8(0x11ec, 0xa);
		mchbar_write16(0x1100, 0x800);
		mchbar_clrsetbits32(0x11bc, ~0, 0x1e84800);
		mchbar_write16(0x11ca, 0xfa);
		mchbar_write32(0x11e4, 0x4e20);
		mchbar_write8(0x11bc, 0xf);
		mchbar_write16(0x11da, 0x19);
		mchbar_write16(0x11ba, 0x470c);
		mchbar_write32(0x1680, 0xe6ffe4ff);
		mchbar_write32(0x1684, 0xdeffdaff);
		mchbar_write32(0x1688, 0xd4ffd0ff);
		mchbar_write32(0x168c, 0xccffc6ff);
		mchbar_write32(0x1690, 0xc0ffbeff);
		mchbar_write32(0x1694, 0xb8ffb0ff);
		mchbar_write32(0x1698, 0xa8ff0000);
		mchbar_write32(0x169c, 0xc00);
		mchbar_write32(0x1290, 0x5000000);
	}

	mchbar_write32(0x124c, 0x15040d00);
	mchbar_write32(0x1250, 0x7f0000);
	mchbar_write32(0x1254, 0x1e220004);
	mchbar_write32(0x1258, 0x4000004);
	mchbar_write32(0x1278, 0x0);
	mchbar_write32(0x125c, 0x0);
	mchbar_write32(0x1260, 0x0);
	mchbar_write32(0x1264, 0x0);
	mchbar_write32(0x1268, 0x0);
	mchbar_write32(0x126c, 0x0);
	mchbar_write32(0x1270, 0x0);
	mchbar_write32(0x1274, 0x0);

	if (deven & 8) {
		mchbar_write16(0x1214, 0x320);
		mchbar_write32(0x1600, 0x40000000);
		mchbar_clrsetbits32(0x11f4, ~0, 1 << 28);
		mchbar_clrsetbits16(0x1230, ~0, 1 << 15);
		mchbar_write32(0x1400, 0x13040020);
		mchbar_write32(0x1404, 0xe090120);
		mchbar_write32(0x1408, 0x5120220);
		mchbar_write32(0x140c, 0x5120330);
		mchbar_write32(0x1410, 0xe090220);
		mchbar_write32(0x1414, 0x1010001);
		mchbar_write32(0x1418, 0x1110000);
		mchbar_write32(0x141c, 0x9020020);
		mchbar_write32(0x1420, 0xd090220);
		mchbar_write32(0x1424, 0x2090220);
		mchbar_write32(0x1428, 0x2090330);
		mchbar_write32(0x142c, 0xd090220);
		mchbar_write32(0x1430, 0x1010001);
		mchbar_write32(0x1434, 0x1110000);
		mchbar_write32(0x1438, 0x11040020);
		mchbar_write32(0x143c, 0x4030220);
		mchbar_write32(0x1440, 0x1060220);
		mchbar_write32(0x1444, 0x1060330);
		mchbar_write32(0x1448, 0x4030220);
		mchbar_write32(0x144c, 0x1010001);
		mchbar_write32(0x1450, 0x1110000);
		mchbar_write32(0x1454, 0x4010020);
		mchbar_write32(0x1458, 0xb090220);
		mchbar_write32(0x145c, 0x1090220);
		mchbar_write32(0x1460, 0x1090330);
		mchbar_write32(0x1464, 0xb090220);
		mchbar_write32(0x1468, 0x1010001);
		mchbar_write32(0x146c, 0x1110000);
		mchbar_write32(0x1470, 0xf040020);
		mchbar_write32(0x1474, 0xa090220);
		mchbar_write32(0x1478, 0x1120220);
		mchbar_write32(0x147c, 0x1120330);
		mchbar_write32(0x1480, 0xa090220);
		mchbar_write32(0x1484, 0x1010001);
		mchbar_write32(0x1488, 0x1110000);
		mchbar_write32(0x148c, 0x7020020);
		mchbar_write32(0x1490, 0x1010220);
		mchbar_write32(0x1494, 0x10210);
		mchbar_write32(0x1498, 0x10320);
		mchbar_write32(0x149c, 0x1010220);
		mchbar_write32(0x14a0, 0x1010001);
		mchbar_write32(0x14a4, 0x1110000);
		mchbar_write32(0x14a8, 0xd040020);
		mchbar_write32(0x14ac, 0x8090220);
		mchbar_write32(0x14b0, 0x1111310);
		mchbar_write32(0x14b4, 0x1111420);
		mchbar_write32(0x14b8, 0x8090220);
		mchbar_write32(0x14bc, 0x1010001);
		mchbar_write32(0x14c0, 0x1110000);
		mchbar_write32(0x14c4, 0x3010020);
		mchbar_write32(0x14c8, 0x7090220);
		mchbar_write32(0x14cc, 0x1081310);
		mchbar_write32(0x14d0, 0x1081420);
		mchbar_write32(0x14d4, 0x7090220);
		mchbar_write32(0x14d8, 0x1010001);
		mchbar_write32(0x14dc, 0x1110000);
		mchbar_write32(0x14e0, 0xb040020);
		mchbar_write32(0x14e4, 0x2030220);
		mchbar_write32(0x14e8, 0x1051310);
		mchbar_write32(0x14ec, 0x1051420);
		mchbar_write32(0x14f0, 0x2030220);
		mchbar_write32(0x14f4, 0x1010001);
		mchbar_write32(0x14f8, 0x1110000);
		mchbar_write32(0x14fc, 0x5020020);
		mchbar_write32(0x1500, 0x5090220);
		mchbar_write32(0x1504, 0x2071310);
		mchbar_write32(0x1508, 0x2071420);
		mchbar_write32(0x150c, 0x5090220);
		mchbar_write32(0x1510, 0x1010001);
		mchbar_write32(0x1514, 0x1110000);
		mchbar_write32(0x1518, 0x7040120);
		mchbar_write32(0x151c, 0x2090220);
		mchbar_write32(0x1520, 0x70b1210);
		mchbar_write32(0x1524, 0x70b1310);
		mchbar_write32(0x1528, 0x2090220);
		mchbar_write32(0x152c, 0x1010001);
		mchbar_write32(0x1530, 0x1110000);
		mchbar_write32(0x1534, 0x1010110);
		mchbar_write32(0x1538, 0x1081310);
		mchbar_write32(0x153c, 0x5041200);
		mchbar_write32(0x1540, 0x5041310);
		mchbar_write32(0x1544, 0x1081310);
		mchbar_write32(0x1548, 0x1010001);
		mchbar_write32(0x154c, 0x1110000);
		mchbar_write32(0x1550, 0x1040120);
		mchbar_write32(0x1554, 0x4051210);
		mchbar_write32(0x1558, 0xd051200);
		mchbar_write32(0x155c, 0xd051200);
		mchbar_write32(0x1560, 0x4051210);
		mchbar_write32(0x1564, 0x1010001);
		mchbar_write32(0x1568, 0x1110000);
		mchbar_write16(0x1222, 0x220a);
		mchbar_write16(0x123c, 0x1fc0);
		mchbar_write16(0x1220, 0x1388);
	}
}
