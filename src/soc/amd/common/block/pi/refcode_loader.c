/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <rmodule.h>
#include <stage_cache.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/image.h>

static int agesa_locate_file(const char *name, struct region_device *rdev,
				uint32_t type)
{
	struct cbfsf fh;

	if (cbfs_boot_locate(&fh, name, &type))
		return -1;

	cbfs_file_data(rdev, &fh);
	return 0;
}

static int agesa_locate_raw_file(const char *name, struct region_device *rdev)
{
	return agesa_locate_file(name, rdev, CBFS_TYPE_RAW);
}

static int agesa_locate_stage_file_early(const char *name,
					struct region_device *rdev)
{
	const size_t metadata_sz = sizeof(struct cbfs_stage);

	if (agesa_locate_file(name, rdev, CBFS_TYPE_STAGE))
		return -1;

	/* Peel off the cbfs stage metadata. */
	return rdev_chain(rdev, rdev, metadata_sz,
			region_device_sz(rdev) - metadata_sz);
}

static int agesa_locate_stage_file_ramstage(const char *name,
						struct region_device *rdev)
{
	struct prog prog = PROG_INIT(PROG_REFCODE, name);
	struct rmod_stage_load rmod_agesa = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.prog = &prog,
	};

	if (resume_from_stage_cache()) {
		stage_cache_load_stage(STAGE_REFCODE, &prog);
	} else {
		if (prog_locate(&prog))
			return -1;

		if (rmodule_stage_load(&rmod_agesa) < 0)
			return -1;

		stage_cache_add(STAGE_REFCODE, &prog);
	}

	return rdev_chain(rdev, prog_rdev(&prog), 0,
		region_device_sz(prog_rdev(&prog)));
}

static int agesa_locate_stage_file(const char *name, struct region_device *rdev)
{
	if (!ENV_RAMSTAGE || !CONFIG(AGESA_SPLIT_MEMORY_FILES))
		return agesa_locate_stage_file_early(name, rdev);
	return agesa_locate_stage_file_ramstage(name, rdev);
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
	struct region_device rdev;
	size_t file_size;
	const char *fname;
	int ret;

	fname = get_agesa_cbfs_name();

	if (CONFIG(AGESA_BINARY_PI_AS_STAGE))
		ret = agesa_locate_stage_file(fname, &rdev);
	else
		ret = agesa_locate_raw_file(fname, &rdev);

	if (ret)
		return NULL;

	file_size = region_device_sz(&rdev);

	/* Assume boot device is memory mapped so the mapping can leak. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	agesa = rdev_mmap_full(&rdev);

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
