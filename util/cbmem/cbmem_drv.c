/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbmem_util.h"

/* "ANY" (0) means no backend is active. */
static enum cbmem_drv_backend_type active_backend = CBMEM_DRV_BACKEND_ANY;

static const char no_backend_error_message[] =
	"No active/initialized CBMEM backend detected. Terminating.\n";

static bool init_specific_backend(enum cbmem_drv_backend_type backend, bool writeable)
{
	switch (backend) {
	case CBMEM_DRV_BACKEND_DEVMEM:
		if (!cbmem_devmem_init(writeable))
			return false;
		debug("Initialized CBMEM backend: devmem\n");
		break;
	case CBMEM_DRV_BACKEND_SYSFS:
		if (!cbmem_sysfs_init())
			return false;
		debug("Initialized CBMEM backend: sysfs\n");
		break;
	default:
		die("Unsupported backend selected. Terminating.\n");
	}

	active_backend = backend;
	return true;
}

bool cbmem_drv_init(enum cbmem_drv_backend_type backend, bool writeable)
{
	if (backend != CBMEM_DRV_BACKEND_ANY)
		return init_specific_backend(backend, writeable);

	/* First try SYSFS backend as it's more secure. */
	if (!init_specific_backend(CBMEM_DRV_BACKEND_SYSFS, writeable))
		return init_specific_backend(CBMEM_DRV_BACKEND_DEVMEM, writeable);

	return true;
}

void cbmem_drv_terminate(void)
{
	/* Only /dev/mem driver requires termination. */
	if (active_backend == CBMEM_DRV_BACKEND_DEVMEM)
		cbmem_devmem_terminate();

	active_backend = CBMEM_DRV_BACKEND_ANY;
}

bool cbmem_drv_get_cbmem_entry(uint32_t id, uint8_t **buf_out, size_t *size_out, uint64_t *addr_out)
{
	switch (active_backend) {
	case CBMEM_DRV_BACKEND_DEVMEM:
		return cbmem_devmem_get_cbmem_entry(id, buf_out, size_out, addr_out);
	case CBMEM_DRV_BACKEND_SYSFS:
		return cbmem_sysfs_get_cbmem_entry(id, buf_out, size_out, addr_out);
	default:
		die(no_backend_error_message);
	}
	return false;
}

bool cbmem_drv_write_cbmem_entry(uint32_t id, uint8_t *buf, size_t buf_size)
{
	switch (active_backend) {
	case CBMEM_DRV_BACKEND_DEVMEM:
		return cbmem_devmem_write_cbmem_entry(id, buf, buf_size);
	case CBMEM_DRV_BACKEND_SYSFS:
		return cbmem_sysfs_write_cbmem_entry(id, buf, buf_size);
	default:
		die(no_backend_error_message);
	}
	return false;
}

void cbmem_drv_foreach_cbmem_entry(cbmem_iterator_callback cb, void *data, bool with_contents)
{
	switch (active_backend) {
	case CBMEM_DRV_BACKEND_DEVMEM:
		cbmem_devmem_foreach_cbmem_entry(cb, data, with_contents);
		break;
	case CBMEM_DRV_BACKEND_SYSFS:
		cbmem_sysfs_foreach_cbmem_entry(cb, data, with_contents);
		break;
	default:
		die(no_backend_error_message);
	}
}
