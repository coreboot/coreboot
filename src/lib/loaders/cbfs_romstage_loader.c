/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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

#include <cbfs.h>
#include <program_loading.h>

static int cbfs_load_romstage(struct prog *romstage)
{
	return cbfs_load_prog_stage(CBFS_DEFAULT_MEDIA, romstage);
}

const struct prog_loader_ops cbfs_romstage_loader = {
	.name = "CBFS",
	.prepare = cbfs_load_romstage,
};
