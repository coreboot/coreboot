/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <rmodule.h>
#include <stage_cache.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/image.h>

static void *agesa_map_raw_file(const char *name, size_t *size)
{
	enum cbfs_type type = CBFS_TYPE_RAW;
	return cbfs_type_map(name, size, &type);
}

static void *agesa_map_stage_file_early(const char *name, size_t *size)
{
	enum cbfs_type type = CBFS_TYPE_STAGE;
	return cbfs_type_map(name, size, &type);
}

static void *agesa_map_stage_file_ramstage(const char *name, size_t *size)
{
	struct prog prog = PROG_INIT(PROG_REFCODE, name);
	struct rmod_stage_load rmod_agesa = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.prog = &prog,
	};

	if (resume_from_stage_cache()) {
		stage_cache_load_stage(STAGE_REFCODE, &prog);
	} else {
		if (rmodule_stage_load(&rmod_agesa) < 0)
			return NULL;

		stage_cache_add(STAGE_REFCODE, &prog);
	}

	*size = prog_size(&prog);
	return prog_start(&prog);
}

static void *agesa_map_stage_file(const char *name, size_t *size)
{
	if (!ENV_RAMSTAGE || !CONFIG(AGESA_SPLIT_MEMORY_FILES))
		return agesa_map_stage_file_early(name, size);
	return agesa_map_stage_file_ramstage(name, size);
}

static const char *get_agesa_cbfs_name(void)
{
	if (!CONFIG(AGESA_SPLIT_MEMORY_FILES))
		return CONFIG_AGESA_CBFS_NAME;
	if (!ENV_RAMSTAGE)
		return CONFIG_AGESA_PRE_MEMORY_CBFS_NAME;
	return CONFIG_AGESA_POST_MEMORY_CBFS_NAME;
}

const void *agesawrapper_locate_module(const char name[8])
{
	const void *agesa;
	const AMD_IMAGE_HEADER *image;
	size_t file_size;
	const char *fname;

	/* Assume boot device is memory mapped so the mapping can leak. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	fname = get_agesa_cbfs_name();

	if (CONFIG(AGESA_BINARY_PI_AS_STAGE))
		agesa = agesa_map_stage_file(fname, &file_size);
	else
		agesa = agesa_map_raw_file(fname, &file_size);

	if (!agesa)
		return NULL;

	image =  amd_find_image(agesa, agesa + file_size, 4096, name);

	if (!image)
		return NULL;

	return (AMD_MODULE_HEADER *)image->ModuleInfoOffset;
}

static MODULE_ENTRY agesa_dispatcher;

MODULE_ENTRY agesa_get_dispatcher(void)
{
	const AMD_MODULE_HEADER *module;
	static const char id[8] = AGESA_ID;

	if (agesa_dispatcher != NULL)
		return agesa_dispatcher;

	module = agesawrapper_locate_module(id);
	if (!module)
		return NULL;

	agesa_dispatcher = module->ModuleDispatcher;
	return agesa_dispatcher;
}
