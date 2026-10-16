/* SPDX_License-Identifier: BSD-3-Clause */

#ifndef _FMAP_H
#define _FMAP_H

#include <commonlib/bsd/cb_err.h>
#include <stddef.h>

/* Looks for area with |name| in FlashMap. Requires lib_sysinfo.fmap_cache. */
enum cb_err fmap_locate_area(const char *name, size_t *offset, size_t *size);

/* Looks for area matching |offset| and |size| in FlashMap. Requires lib_sysinfo.fmap_cache. */
enum cb_err fmap_locate_area_by_offset(size_t offset, size_t size, const char **name);

#endif /* _FMAP_H */
