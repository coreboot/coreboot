/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_PRIVATE_H_
#define _CBFS_PRIVATE_H_

#include <commonlib/bsd/cbfs_private.h>
#include <commonlib/region.h>
#include <types.h>

/*
 * This header contains low-level CBFS APIs that should only be used by code
 * that really needs this level of access. Most code (particularly platform
 * code) should use the higher-level CBFS APIs in <cbfs.h>. Code using these
 * APIs needs to take special care to ensure CBFS file data is verified (in a
 * TOCTOU-safe manner) before access (TODO: add details on how to do this once
 * file verification code is in).
 */

/* Find by name, load metadata into |mdata| and chain file data to |rdev|. */
cb_err_t cbfs_boot_lookup(const char *name, bool force_ro,
			  union cbfs_mdata *mdata, struct region_device *rdev);

#endif
