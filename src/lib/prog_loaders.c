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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */


#include <stdlib.h>
#include <arch/stages.h>
#include <boot_device.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <fallback.h>
#include <halt.h>
#include <lib.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <rmodule.h>
#include <rules.h>
#include <stage_cache.h>
#include <symbols.h>
#include <timestamp.h>

#define DEFAULT_CBFS_LOADER_PRESENT \
	(!ENV_VERSTAGE || (ENV_VERSTAGE && !CONFIG_RETURN_FROM_VERSTAGE))

#if DEFAULT_CBFS_LOADER_PRESENT
static int cbfs_boot_prog_locate(struct prog *prog)
{
	return cbfs_boot_locate(&prog->rdev, prog->name, NULL);
}

static const struct prog_loader_ops cbfs_default_loader = {
	.locate = cbfs_boot_prog_locate,
};
#endif

extern const struct prog_loader_ops vboot_loader;

static const struct prog_loader_ops *loaders[] = {
#if CONFIG_VBOOT_VERIFY_FIRMWARE
	&vboot_loader,
#endif
#if DEFAULT_CBFS_LOADER_PRESENT
	&cbfs_default_loader,
#endif
};

int prog_locate(struct prog *prog)
{
	int i;

	boot_device_init();

	for (i = 0; i < ARRAY_SIZE(loaders); i++) {
		/* Default loader state is active. */
		int ret = 1;
		const struct prog_loader_ops *ops;

		ops = loaders[i];

		if (ops->is_loader_active != NULL)
			ret = ops->is_loader_active(prog);

		if (ret == 0) {
			printk(BIOS_DEBUG, "%s loader inactive.\n",
				ops->name);
			continue;
		} else if (ret < 0) {
			printk(BIOS_DEBUG, "%s loader failure.\n",
				ops->name);
			continue;
		}

		printk(BIOS_DEBUG, "%s loader active.\n", ops->name);

		if (ops->locate(prog))
			continue;

		printk(BIOS_DEBUG, "'%s' located at offset: %zx size: %zx\n",
			prog->name, region_device_offset(&prog->rdev),
			region_device_sz(&prog->rdev));

		return 0;
	}

	return -1;
}

void run_romstage(void)
{
	struct prog romstage = {
		.name = CONFIG_CBFS_PREFIX "/romstage",
		.type = PROG_ROMSTAGE,
	};

	/* The only time the default CBFS loader isn't present is during
	 * VERSTAGE in which it returns back to the calling stage. */
	if (!DEFAULT_CBFS_LOADER_PRESENT)
		return;

	if (prog_locate(&romstage))
		goto fail;

	timestamp_add_now(TS_START_COPYROM);

	if (cbfs_prog_stage_load(&romstage))
		goto fail;

	timestamp_add_now(TS_END_COPYROM);

	prog_run(&romstage);

fail:
	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
		die("Couldn't load romstage.\n");
	halt();
}

void __attribute__((weak)) stage_cache_add(int stage_id, struct prog *stage) {}
void __attribute__((weak)) stage_cache_load_stage(int stage_id,
							struct prog *stage) {}
void __attribute__((weak)) ramstage_cache_invalid(void) {}

static void run_ramstage_from_resume(struct romstage_handoff *handoff,
					struct prog *ramstage)
{
	if (handoff != NULL && handoff->s3_resume) {
		/* Load the cached ramstage to runtime location. */
		stage_cache_load_stage(STAGE_RAMSTAGE, ramstage);

		if (prog_entry(ramstage) != NULL) {
			printk(BIOS_DEBUG, "Jumping to image.\n");
			prog_run(ramstage);
		}
		ramstage_cache_invalid();
	}
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
	struct prog ramstage = {
		.name = CONFIG_CBFS_PREFIX "/ramstage",
		.type = PROG_RAMSTAGE,
	};

	/* Only x86 systems currently take the same firmware path on resume. */
	if (IS_ENABLED(CONFIG_ARCH_X86) && IS_ENABLED(CONFIG_EARLY_CBMEM_INIT))
		run_ramstage_from_resume(romstage_handoff_find_or_add(),
						&ramstage);

	if (prog_locate(&ramstage))
		goto fail;

	timestamp_add_now(TS_START_COPYRAM);

	if (IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE)) {
		if (load_relocatable_ramstage(&ramstage))
			goto fail;
	} else if (cbfs_prog_stage_load(&ramstage))
		goto fail;

	stage_cache_add(STAGE_RAMSTAGE, &ramstage);

	timestamp_add_now(TS_END_COPYRAM);

	prog_run(&ramstage);

fail:
	die("Ramstage was not loaded!\n");
}

static struct prog global_payload = {
	.name = CONFIG_CBFS_PREFIX "/payload",
	.type = PROG_PAYLOAD,
};

void __attribute__((weak)) mirror_payload(struct prog *payload)
{
	return;
}

void payload_load(void)
{
	struct prog *payload = &global_payload;

	timestamp_add_now(TS_LOAD_PAYLOAD);

	if (prog_locate(payload))
		goto out;

	mirror_payload(payload);

	/* Pass cbtables to payload if architecture desires it. */
	prog_set_entry(payload, selfload(payload),
			cbmem_find(CBMEM_ID_CBTABLE));

out:
	if (prog_entry(payload) == NULL)
		die("Payload not loaded.\n");
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
