/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/msr.h>
#include <string.h>
#include <types.h>

/**
 * @brief Gather speedstep limits for current processor
 *
 * At least power limits are processor type specific. Penryn introduced half
 * steps in bus ratios. Don't know about Atom processors.
 */
static void speedstep_get_limits(sst_params_t *const params)
{
	msr_t msr;

	const uint16_t cpu_id = (cpuid_eax(1) >> 4) & 0xffff;
	const uint32_t state_mask =
		/* Penryn supports non integer (i.e. half) ratios. */
		((cpu_id == 0x1067) ? SPEEDSTEP_RATIO_NONINT : 0)
		| SPEEDSTEP_RATIO_VALUE_MASK | SPEEDSTEP_VID_MASK;

	/* Initialize params to zero. */
	memset(params, '\0', sizeof(*params));

	/* Read Super-LFM parameters. */
	if (((rdmsr(MSR_EXTENDED_CONFIG).lo >> 27) & 3) == 3) {/*supported and
								 enabled bits */
		msr = rdmsr(MSR_FSB_CLOCK_VCC);
		params->slfm = SPEEDSTEP_STATE_FROM_MSR(msr.lo, state_mask);
		params->slfm.dynfsb	= 1;
		params->slfm.is_slfm	= 1;
	}

	/* Read normal minimum parameters. */
	msr = rdmsr(MSR_THERM2_CTL);
	params->min = SPEEDSTEP_STATE_FROM_MSR(msr.lo, state_mask);

	/* Read normal maximum parameters. */
	/* Newer CPUs provide the normal maximum settings in
	   IA32_PLATFORM_ID. The values in IA32_PERF_STATUS change
	   when using turbo mode. */
	msr = rdmsr(IA32_PLATFORM_ID);
	params->max = SPEEDSTEP_STATE_FROM_MSR(msr.lo, state_mask);
	if (cpu_id == 0x006e) {
		/* Looks like Yonah CPUs don't have the frequency ratio in
		   IA32_PLATFORM_ID. Use IA32_PERF_STATUS instead, the reading
		   should be reliable as those CPUs don't have turbo mode. */
		msr = rdmsr(IA32_PERF_STATUS);
		params->max.ratio = (msr.hi & SPEEDSTEP_RATIO_VALUE_MASK)
						>> SPEEDSTEP_RATIO_SHIFT;
	}

	/* Read turbo parameters. */
	msr = rdmsr(MSR_FSB_CLOCK_VCC);
	if ((msr.hi & (1 << (63 - 32))) &&
		/* supported and */
			!(rdmsr(IA32_MISC_ENABLE).hi & (1 << (38 - 32)))) {
			/* not disabled */
		params->turbo = SPEEDSTEP_STATE_FROM_MSR(msr.hi, state_mask);
		params->turbo.is_turbo = 1;
	}

	/* Set power limits by processor type. */
	/* Defined values match the normal voltage versions only. But
	   they are only a hint for OSPM, so this should not hurt much. */
	switch (cpu_id) {
	case 0x006e:
		/* Yonah */
		params->min.power	= SPEEDSTEP_MIN_POWER_YONAH;
		params->max.power	= SPEEDSTEP_MAX_POWER_YONAH;
		break;
	case 0x1067:
		/* Penryn */
		params->slfm.power	= SPEEDSTEP_SLFM_POWER_PENRYN;
		params->min.power	= SPEEDSTEP_MIN_POWER_PENRYN;
		params->max.power	= SPEEDSTEP_MAX_POWER_PENRYN;
		params->turbo.power	= SPEEDSTEP_MAX_POWER_PENRYN;
		break;
	case 0x006f:
		/* Merom */
	default:
		/* Use Merom values by default (as before). */
		params->slfm.power	= SPEEDSTEP_SLFM_POWER_MEROM;
		params->min.power	= SPEEDSTEP_MIN_POWER_MEROM;
		params->max.power	= SPEEDSTEP_MAX_POWER_MEROM;
		params->turbo.power	= SPEEDSTEP_MAX_POWER_MEROM;
		break;
	}
}

/**
 * @brief Generate full p-states table from processor parameters
 *
 * This is generic code and should work at least for Merom and Penryn
 * processors. It is used to generate ACPI tables and configure EMTTM.
 */
void speedstep_gen_pstates(sst_table_t *const table)
{
	sst_params_t params;
	/* Gather speedstep limits. */
	speedstep_get_limits(&params);

	/*\ First, find the number of normal states: \*/

	/* Calculate with doubled values to work
	   around non-integer (.5) bus ratios. */
	const int power_diff2	= (params.max.power - params.min.power) * 2;
	const int vid_diff2	= (params.max.vid - params.min.vid) * 2;
	const int max_ratio2	= SPEEDSTEP_DOUBLE_RATIO(params.max);
	const int min_ratio2	= SPEEDSTEP_DOUBLE_RATIO(params.min);
	const int ratio_diff2	= max_ratio2 - min_ratio2;
	/* Calculate number of normal states (LFM to HFM, min to max). */
	/* Increase step size, until all states fit into the table.
	   (Note: First try should always work, if
	    SPEEDSTEP_MAX_NORMAL_STATES is set correctly.) */
	int states, step2 = 0;
	do {
		step2 += 2 * 2; /* Must be a multiple of 2 (doubled). */
		states = ratio_diff2 / step2 + 1;
	} while (states > SPEEDSTEP_MAX_NORMAL_STATES);
	if (step2 > 4)
		printk(BIOS_INFO, "Enhanced Speedstep processor with "
				  "more than %d possible p-states.\n",
			SPEEDSTEP_MAX_NORMAL_STATES);
	if (states < 2) /* Report at least two normal states. */
		states = 2;

	/*\ Now, fill the table: \*/

	table->num_states = 0;

	/* Add turbo state if supported. */
	if (params.turbo.is_turbo)
		table->states[table->num_states++] = params.turbo;

	/* Add HFM first. */
	table->states[table->num_states] = params.max;
	/* Work around HFM and LFM having the same bus ratio. */
	if ((params.max.dynfsb == params.min.dynfsb) &&
			(params.max.nonint == params.min.nonint) &&
			(params.max.ratio == params.min.ratio))
		table->states[table->num_states].vid = params.min.vid;
	++table->num_states;
	--states;

	/* Now, add all other normal states based on LFM (min). */
	const int power_step	= (power_diff2 / states) / 2;
	const int vid_step	= (vid_diff2 / states) / 2;
	const int ratio_step    = step2 / 2;
	int power   = params.min.power + (states - 1) * power_step;
	int vid	    = params.min.vid   + (states - 1) * vid_step;
	int ratio   = params.min.ratio + (states - 1) * ratio_step;
	for (; states > 0; --states) {
		table->states[table->num_states++] =
			(sst_state_t){ 0, 0, ratio, vid, 0, 0, power };
		power	-= power_step;
		vid	-= vid_step;
		ratio	-= ratio_step;
	}

	/* At last, add Super-LFM state if supported. */
	if (params.slfm.is_slfm)
		table->states[table->num_states++] = params.slfm;
}
