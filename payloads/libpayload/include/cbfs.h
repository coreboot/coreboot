/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _CBFS_H_
#define _CBFS_H_

#include <commonlib/bsd/cb_err.h>
#include <commonlib/bsd/cbfs_mdata.h>
#include <endian.h>
#include <stdbool.h>


/**********************************************************************************************
 *                                  CBFS FILE ACCESS APIs                                     *
 **********************************************************************************************/

/* For documentation look in src/include/cbfs.h file in the main coreboot source tree. */

static inline size_t cbfs_load(const char *name, void *buf, size_t size);
static inline size_t cbfs_ro_load(const char *name, void *buf, size_t size);
static inline size_t cbfs_unverified_area_load(const char *area, const char *name, void *buf,
					       size_t size);

static inline void *cbfs_map(const char *name, size_t *size_out);
static inline void *cbfs_ro_map(const char *name, size_t *size_out);
static inline void *cbfs_unverified_area_map(const char *area, const char *name,
					     size_t *size_out);

void cbfs_unmap(void *mapping);

static inline size_t cbfs_get_size(const char *name);
static inline size_t cbfs_ro_get_size(const char *name);

static inline enum cbfs_type cbfs_get_type(const char *name);
static inline enum cbfs_type cbfs_ro_get_type(const char *name);

static inline bool cbfs_file_exists(const char *name);
static inline bool cbfs_ro_file_exists(const char *name);

/**********************************************************************************************
 *                         INTERNAL HELPERS FOR INLINES, DO NOT USE.                          *
 **********************************************************************************************/
ssize_t _cbfs_boot_lookup(const char *name, bool force_ro, union cbfs_mdata *mdata);

void *_cbfs_load(const char *name, void *buf, size_t *size_inout, bool force_ro);

void *_cbfs_unverified_area_load(const char *area, const char *name, void *buf,
				 size_t *size_inout);

/**********************************************************************************************
 *                                  INLINE IMPLEMENTATIONS                                    *
 **********************************************************************************************/

static inline void *cbfs_map(const char *name, size_t *size_out)
{
	return _cbfs_load(name, NULL, size_out, false);
}

static inline void *cbfs_ro_map(const char *name, size_t *size_out)
{
	return _cbfs_load(name, NULL, size_out, true);
}

static inline void *cbfs_unverified_area_map(const char *area, const char *name,
					     size_t *size_out)
{
	return _cbfs_unverified_area_load(area, name, NULL, size_out);
}

static inline size_t cbfs_load(const char *name, void *buf, size_t size)
{
	if (_cbfs_load(name, buf, &size, false))
		return size;
	else
		return 0;
}

static inline size_t cbfs_ro_load(const char *name, void *buf, size_t size)
{
	if (_cbfs_load(name, buf, &size, true))
		return size;
	else
		return 0;
}

static inline size_t cbfs_unverified_area_load(const char *area, const char *name, void *buf,
					       size_t size)
{
	if (_cbfs_unverified_area_load(area, name, buf, &size))
		return size;
	else
		return 0;
}

static inline size_t cbfs_get_size(const char *name)
{
	union cbfs_mdata mdata;
	if (_cbfs_boot_lookup(name, false, &mdata) < 0)
		return 0;
	else
		return be32toh(mdata.h.len);
}

static inline size_t cbfs_ro_get_size(const char *name)
{
	union cbfs_mdata mdata;
	if (_cbfs_boot_lookup(name, true, &mdata) < 0)
		return 0;
	else
		return be32toh(mdata.h.len);
}

static inline enum cbfs_type cbfs_get_type(const char *name)
{
	union cbfs_mdata mdata;
	if (_cbfs_boot_lookup(name, false, &mdata) < 0)
		return CBFS_TYPE_NULL;
	else
		return be32toh(mdata.h.type);
}

static inline enum cbfs_type cbfs_ro_get_type(const char *name)
{
	union cbfs_mdata mdata;
	if (_cbfs_boot_lookup(name, true, &mdata) < 0)
		return CBFS_TYPE_NULL;
	else
		return be32toh(mdata.h.type);
}

static inline bool cbfs_file_exists(const char *name)
{
	union cbfs_mdata mdata;
	return _cbfs_boot_lookup(name, false, &mdata) >= 0;
}

static inline bool cbfs_ro_file_exists(const char *name)
{
	union cbfs_mdata mdata;
	return _cbfs_boot_lookup(name, true, &mdata) >= 0;
}


/* Legacy API. Designated for removal in the future. */
#include <cbfs_legacy.h>

#endif
