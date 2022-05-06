/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef HASWELL_RAMINIT_NATIVE_H
#define HASWELL_RAMINIT_NATIVE_H

enum raminit_boot_mode {
	BOOTMODE_COLD,
	BOOTMODE_WARM,
	BOOTMODE_S3,
	BOOTMODE_FAST,
};

enum raminit_status {
	RAMINIT_STATUS_SUCCESS = 0,
	RAMINIT_STATUS_UNSPECIFIED_ERROR, /** TODO: Deprecated in favor of specific values **/
};

enum generic_stepping {
	STEPPING_A0 = 1,
	STEPPING_B0 = 2,
	STEPPING_C0 = 3,
};

struct sysinfo {
	enum raminit_boot_mode bootmode;
	enum generic_stepping stepping;
	uint32_t cpu;		/* CPUID value */

	bool dq_pins_interleaved;
};

void raminit_main(enum raminit_boot_mode bootmode);

#endif
