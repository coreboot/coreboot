/*
 * fmap_from_fmd.h, tool to distill flashmap descriptors into raw FMAP sections
 *
 * Copyright (C) 2015 Google, Inc.
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

#ifndef FMAP_FROM_FMD_H_
#define FMAP_FROM_FMD_H_

#include "flashmap/fmap.h"
#include "fmd.h"

/**
 * @param desc The descriptor tree serving as a data source
 * @return     The FMAP section, which is also owned by the caller and must
 *             later be released with a call to fmap_destroy()
 */
struct fmap *fmap_from_fmd(const struct flashmap_descriptor *desc);

#endif
