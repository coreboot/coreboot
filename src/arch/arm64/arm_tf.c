/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <arch/mmu.h>
#include <arch/transition.h>
#include <arm_tf.h>
#include <assert.h>
#include <cbfs.h>
#include <cbmem.h>
#include <compiler.h>
#include <program_loading.h>

/*
 * TODO: Many of these structures are currently unused. Better not fill them out
 * to make future changes fail fast, rather than try to come up with content
 * that might turn out to not make sense. Implement later as required.
 *
static image_info_t bl31_image_info;
static image_info_t bl32_image_info;
static image_info_t bl33_image_info;
 */
static entry_point_info_t bl32_ep_info;
static entry_point_info_t bl33_ep_info;
static bl31_params_t bl31_params;

void __weak *soc_get_bl31_plat_params(bl31_params_t *params)
{
	/* Default weak implementation. */
	return NULL;
}

void arm_tf_run_bl31(u64 payload_entry, u64 payload_arg0, u64 payload_spsr)
{
	struct prog bl31 = PROG_INIT(PROG_BL31, CONFIG_CBFS_PREFIX"/bl31");
	void (*bl31_entry)(bl31_params_t *params, void *plat_params) = NULL;

	if (prog_locate(&bl31))
		die("BL31 not found");

	if (!selfload(&bl31, false))
		die("BL31 load failed");
	bl31_entry = prog_entry(&bl31);

	SET_PARAM_HEAD(&bl31_params, PARAM_BL31, VERSION_1, 0);

	if (IS_ENABLED(CONFIG_ARM64_USE_SECURE_OS)) {
		struct prog bl32 = PROG_INIT(PROG_BL32,
					     CONFIG_CBFS_PREFIX"/secure_os");

		if (prog_locate(&bl32))
			die("BL32 not found");

		if (cbfs_prog_stage_load(&bl32))
			die("BL32 load failed");

		SET_PARAM_HEAD(&bl32_ep_info, PARAM_EP, VERSION_1,
			       PARAM_EP_SECURE);
		bl32_ep_info.pc = (uintptr_t)prog_entry(&bl32);
		bl32_ep_info.spsr = SPSR_EXCEPTION_MASK |
				get_eret_el(EL1, SPSR_USE_L);
		bl31_params.bl32_ep_info = &bl32_ep_info;
	}

	bl31_params.bl33_ep_info = &bl33_ep_info;

	SET_PARAM_HEAD(&bl33_ep_info, PARAM_EP, VERSION_1, PARAM_EP_NON_SECURE);
	bl33_ep_info.pc = payload_entry;
	bl33_ep_info.spsr = payload_spsr;
	bl33_ep_info.args.arg0 = payload_arg0;

	/* May update bl31_params if necessary. */
	void *bl31_plat_params = soc_get_bl31_plat_params(&bl31_params);

	/* MMU disable will flush cache, so passed params land in memory. */
	raw_write_daif(SPSR_EXCEPTION_MASK);
	mmu_disable();
	bl31_entry(&bl31_params, bl31_plat_params);
	die("BL31 returned!");
}
