/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/cpu.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <string.h>

struct fsp_notify_params {
	enum fsp_notify_phase phase;
};

typedef asmlinkage enum fsp_status (*fsp_notify_fn)
				   (struct fsp_notify_params *);

enum fsp_status fsp_notify(enum fsp_notify_phase phase)
{
	fsp_notify_fn fspnotify;
	struct fsp_notify_params notify_params = { .phase = phase };

	if (!fsps_hdr.silicon_init_entry_offset)
		return FSP_NOT_FOUND;

	fspnotify = (void*) (fsps_hdr.image_base +
			    fsps_hdr.notify_phase_entry_offset);

	printk(BIOS_DEBUG, "FspNotify %x\n", (uint32_t) phase);

	return fspnotify(&notify_params);
}
