/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/lib_helpers.h>
#include <arch/mmu.h>
#include <arch/transition.h>
#include <bl31.h>
#include <bootmem.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>

#include <arm-trusted-firmware/include/export/common/bl_common_exp.h>

static entry_point_info_t bl32_ep_info = {
	.h = {
		.type = PARAM_EP,
		.version = PARAM_VERSION_1,
		.size = sizeof(bl32_ep_info),
		.attr = EP_SECURE,
	},
};
static entry_point_info_t bl33_ep_info = {
	.h = {
		.type = PARAM_EP,
		.version = PARAM_VERSION_1,
		.size = sizeof(bl33_ep_info),
		.attr = EP_NON_SECURE,
	},
};

static bl_params_node_t bl32_params_node = {
	.image_id = BL32_IMAGE_ID,
	.ep_info = &bl32_ep_info,
};
static bl_params_node_t bl33_params_node = {
	.image_id = BL33_IMAGE_ID,
	.ep_info = &bl33_ep_info,
};

static bl_params_t bl_params = {
	.h = {
		.type = PARAM_BL_PARAMS,
		.version = PARAM_VERSION_2,
		.size = sizeof(bl_params),
		.attr = 0,
	},
	.head = &bl33_params_node,
};

static struct bl_aux_param_header *bl_aux_params;

/* Only works when using the default soc_get_bl31_plat_params() below. */
void register_bl31_aux_param(struct bl_aux_param_header *param)
{
	param->next = (uintptr_t)bl_aux_params;
	bl_aux_params = param;
}

/* Default implementation. All newly added SoCs should use this if possible! */
__weak void *soc_get_bl31_plat_params(void)
{
	static struct bl_aux_param_uint64 cbtable_param = {
		.h = { .type = BL_AUX_PARAM_COREBOOT_TABLE, },
	};
	if (!cbtable_param.value) {
		cbtable_param.value = (uint64_t)cbmem_find(CBMEM_ID_CBTABLE);
		if (cbtable_param.value)
			register_bl31_aux_param(&cbtable_param.h);
	}
	return bl_aux_params;
}

void run_bl31(u64 payload_entry, u64 payload_arg0, u64 payload_spsr)
{
	struct prog bl31 = PROG_INIT(PROG_BL31, CONFIG_CBFS_PREFIX"/bl31");
	void (*bl31_entry)(bl_params_t *params, void *plat_params) = NULL;

	if (!selfload_check(&bl31, BM_MEM_BL31))
		die("BL31 load failed");
	bl31_entry = prog_entry(&bl31);

	if (CONFIG(ARM64_USE_SECURE_OS)) {
		struct prog bl32 = PROG_INIT(PROG_BL32,
					     CONFIG_CBFS_PREFIX"/secure_os");

		if (cbfs_prog_stage_load(&bl32))
			die("BL32 load failed");

		bl32_ep_info.pc = (uintptr_t)prog_entry(&bl32);
		bl32_ep_info.spsr = SPSR_EXCEPTION_MASK |
				get_eret_el(EL1, SPSR_USE_L);
		bl33_params_node.next_params_info = &bl32_params_node;
	}

	bl33_ep_info.pc = payload_entry;
	bl33_ep_info.spsr = payload_spsr;
	bl33_ep_info.args.arg0 = payload_arg0;

	void *bl31_plat_params = soc_get_bl31_plat_params();

	/* MMU disable will flush cache, so passed params land in memory. */
	raw_write_daif(SPSR_EXCEPTION_MASK);
	mmu_disable();
	bl31_entry(&bl_params, bl31_plat_params);
	die("BL31 returned!");
}
