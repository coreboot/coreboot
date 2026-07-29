/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <self_test.h>

#include "st_intel_cpu.h"

static struct self_test_t intel_cpu_selftests[] = {
	{
		.id = ST_CPU_INTEL_MICROCODE,
		.pre_check = CONFIG(SELF_TEST_INTEL_CPU_UCODE),
		.exec = st_intel_microcode,
		.state = BS_PAYLOAD_LOAD,
		.when = BS_ON_EXIT
	},
	{.id = ST_INVALID_ID, } /* Terminator */
};

REGISTER_SELFTEST(intel_cpu_selftests);
