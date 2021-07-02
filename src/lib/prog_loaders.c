/* SPDX-License-Identifier: GPL-2.0-only */


#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <fallback.h>
#include <halt.h>
#include <lib.h>
#include <program_loading.h>
#include <reset.h>
#include <rmodule.h>
#include <stage_cache.h>
#include <symbols.h>
#include <thread.h>
#include <timestamp.h>
#include <security/vboot/vboot_common.h>

void run_romstage(void)
{
	struct prog romstage =
		PROG_INIT(PROG_ROMSTAGE, CONFIG_CBFS_PREFIX "/romstage");

	vboot_run_logic();

	timestamp_add_now(TS_START_COPYROM);

	if (ENV_X86 && CONFIG(BOOTBLOCK_NORMAL)) {
		if (legacy_romstage_select_and_load(&romstage))
			goto fail;
	} else {
		if (cbfs_prog_stage_load(&romstage))
			goto fail;
	}

	timestamp_add_now(TS_END_COPYROM);

	console_time_report();

	prog_run(&romstage);

fail:
	if (CONFIG(BOOTBLOCK_CONSOLE))
		die_with_post_code(POST_INVALID_ROM,
				   "Couldn't load romstage.\n");
	halt();
}

int __weak prog_locate_hook(struct prog *prog) { return 0; }

static void run_ramstage_from_resume(struct prog *ramstage)
{
	/* Load the cached ramstage to runtime location. */
	stage_cache_load_stage(STAGE_RAMSTAGE, ramstage);

	ramstage->cbfs_type = CBFS_TYPE_STAGE;
	prog_set_arg(ramstage, cbmem_top());

	if (prog_entry(ramstage) != NULL) {
		printk(BIOS_DEBUG, "Jumping to image.\n");
		prog_run(ramstage);
	}

	printk(BIOS_ERR, "ramstage cache invalid.\n");
	board_reset();
}

static int load_relocatable_ramstage(struct prog *ramstage)
{
	struct rmod_stage_load rmod_ram = {
		.cbmem_id = CBMEM_ID_RAMSTAGE,
		.prog = ramstage,
	};

	return rmodule_stage_load(&rmod_ram);
}

void run_ramstage(void)
{
	struct prog ramstage =
		PROG_INIT(PROG_RAMSTAGE, CONFIG_CBFS_PREFIX "/ramstage");

	if (ENV_POSTCAR)
		timestamp_add_now(TS_END_POSTCAR);

	/* Call "end of romstage" here if postcar stage doesn't exist */
	if (ENV_ROMSTAGE)
		timestamp_add_now(TS_END_ROMSTAGE);

	/*
	 * Only x86 systems using ramstage stage cache currently take the same
	 * firmware path on resume.
	 */
	if (ENV_X86 && resume_from_stage_cache())
		run_ramstage_from_resume(&ramstage);

	vboot_run_logic();

	timestamp_add_now(TS_START_COPYRAM);

	if (ENV_X86) {
		if (load_relocatable_ramstage(&ramstage))
			goto fail;
	} else {
		if (cbfs_prog_stage_load(&ramstage))
			goto fail;
	}

	stage_cache_add(STAGE_RAMSTAGE, &ramstage);

	timestamp_add_now(TS_END_COPYRAM);

	console_time_report();

	/* This overrides the arg fetched from the relocatable module */
	prog_set_arg(&ramstage, cbmem_top());

	prog_run(&ramstage);

fail:
	die_with_post_code(POST_INVALID_ROM, "Ramstage was not loaded!\n");
}

#if ENV_PAYLOAD_LOADER // gc-sections should take care of this

static struct prog global_payload =
	PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/payload");

static struct thread_handle payload_preload_handle;

static enum cb_err payload_preload_thread_entry(void *arg)
{
	size_t size;
	struct prog *payload = &global_payload;

	printk(BIOS_DEBUG, "Preloading payload\n");

	payload->cbfs_type = CBFS_TYPE_QUERY;

	size = cbfs_type_load(prog_name(payload), _payload_preload_cache,
			      REGION_SIZE(payload_preload_cache), &payload->cbfs_type);

	if (!size) {
		printk(BIOS_ERR, "ERROR: Preloading payload failed\n");
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "Preloading payload complete\n");

	return CB_SUCCESS;
}

void payload_preload(void)
{
	struct thread_handle *handle = &payload_preload_handle;

	if (!CONFIG(PAYLOAD_PRELOAD))
		return;

	if (thread_run(handle, payload_preload_thread_entry, NULL))
		printk(BIOS_ERR, "ERROR: Failed to start payload preload thread\n");
}

void payload_load(void)
{
	struct prog *payload = &global_payload;
	struct thread_handle *handle = &payload_preload_handle;
	void *mapping = NULL;
	void *buffer;

	timestamp_add_now(TS_LOAD_PAYLOAD);

	if (prog_locate_hook(payload))
		goto out;

	if (CONFIG(PAYLOAD_PRELOAD) && thread_join(handle) == CB_SUCCESS) {
		buffer = _payload_preload_cache;
	} else {
		payload->cbfs_type = CBFS_TYPE_QUERY;
		mapping = cbfs_type_map(prog_name(payload), NULL, &payload->cbfs_type);
		buffer = mapping;
	}

	if (!buffer)
		goto out;

	switch (prog_cbfs_type(payload)) {
	case CBFS_TYPE_SELF: /* Simple ELF */
		selfload_mapped(payload, buffer, BM_MEM_RAM);
		break;
	case CBFS_TYPE_FIT: /* Flattened image tree */
		if (CONFIG(PAYLOAD_FIT_SUPPORT)) {
			fit_payload(payload, buffer);
			break;
		} /* else fall-through */
	default:
		die_with_post_code(POST_INVALID_ROM,
				   "Unsupported payload type %d.\n", payload->cbfs_type);
		break;
	}

	if (mapping)
		cbfs_unmap(mapping);
out:
	if (prog_entry(payload) == NULL)
		die_with_post_code(POST_INVALID_ROM, "Payload not loaded.\n");
}

void payload_run(void)
{
	struct prog *payload = &global_payload;

	/* Reset to booting from this image as late as possible */
	boot_successful();

	printk(BIOS_DEBUG, "Jumping to boot code at %p(%p)\n",
		prog_entry(payload), prog_entry_arg(payload));

	post_code(POST_ENTER_ELF_BOOT);

	timestamp_add_now(TS_SELFBOOT_JUMP);

	/* Before we go off to run the payload, see if
	 * we stayed within our bounds.
	 */
	checkstack(_estack, 0);

	prog_run(payload);
}

#endif
