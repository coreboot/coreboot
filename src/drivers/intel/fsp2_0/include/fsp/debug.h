/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _FSP2_0_DEBUG_H_
#define _FSP2_0_DEBUG_H_

#include <fsp/util.h>

/* FSP debug API */
void fsp_debug_before_memory_init(fsp_memory_init_fn memory_init,
	const struct FSPM_UPD *fspm_old_upd,
	const struct FSPM_UPD *fspm_new_upd, void **hob_list_ptr);
void fsp_debug_after_memory_init(enum fsp_status status,
	const struct hob_header *hob_list_ptr);
void fsp_debug_before_silicon_init(fsp_silicon_init_fn silicon_init,
	const struct FSPS_UPD *fsps_old_upd,
	const struct FSPS_UPD *fsps_new_upd);
void fsp_debug_after_silicon_init(enum fsp_status status);
void fsp_before_debug_notify(fsp_notify_fn notify,
	const struct fsp_notify_params *notify_params);
void fsp_debug_after_notify(enum fsp_status status);

#endif /* _FSP2_0_DEBUG_H_ */
