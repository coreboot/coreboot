/* fmap_from_fmd.h, tool to distill flashmap descriptors into raw FMAP sections */
/* SPDX-License-Identifier: GPL-2.0-only */

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
