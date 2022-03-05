/* SPDX-License-Identifier: BSD-3-Clause */

#include <libpayload-config.h>
#include <arch/virtual.h>
#include <assert.h>
#include <cbfs.h>
#include <cbfs_glue.h>
#include <commonlib/bsd/cbfs_private.h>
#include <commonlib/bsd/fmap_serialized.h>
#include <libpayload.h>
#include <lz4.h>
#include <lzma.h>
#include <string.h>
#include <sysinfo.h>


static const struct cbfs_boot_device *cbfs_get_boot_device(bool force_ro)
{
	static struct cbfs_boot_device ro;
	static struct cbfs_boot_device rw;

	if (!force_ro) {
		if (!rw.dev.size) {
			rw.dev.offset = lib_sysinfo.cbfs_offset;
			rw.dev.size = lib_sysinfo.cbfs_size;
			rw.mcache = phys_to_virt(lib_sysinfo.cbfs_rw_mcache_offset);
			rw.mcache_size = lib_sysinfo.cbfs_rw_mcache_size;
		}
		return &rw;
	}

	if (ro.dev.size)
		return &ro;

	if (fmap_locate_area("COREBOOT", &ro.dev.offset, &ro.dev.size))
		return NULL;

	ro.mcache = phys_to_virt(lib_sysinfo.cbfs_ro_mcache_offset);
	ro.mcache_size = lib_sysinfo.cbfs_ro_mcache_size;

	return &ro;
}

ssize_t _cbfs_boot_lookup(const char *name, bool force_ro, union cbfs_mdata *mdata)
{
	const struct cbfs_boot_device *cbd = cbfs_get_boot_device(force_ro);
	if (!cbd)
		return CB_ERR;

	size_t data_offset;
	enum cb_err err = CB_CBFS_CACHE_FULL;
	if (cbd->mcache_size)
		err = cbfs_mcache_lookup(cbd->mcache, cbd->mcache_size, name, mdata,
					 &data_offset);

	if (err == CB_CBFS_CACHE_FULL)
		err = cbfs_lookup(&cbd->dev, name, mdata, &data_offset, NULL);

	/* Fallback to RO if possible. */
	if (CONFIG(LP_ENABLE_CBFS_FALLBACK) && !force_ro && err == CB_CBFS_NOT_FOUND) {
		LOG("Fall back to RO region for '%s'\n", name);
		return _cbfs_boot_lookup(name, true, mdata);
	}

	if (err) {
		if (err == CB_CBFS_NOT_FOUND)
			LOG("'%s' not found.\n", name);
		else
			ERROR("Error %d when looking up '%s'\n", err, name);
		return err;
	}

	return cbd->dev.offset + data_offset;
}

void cbfs_unmap(void *mapping)
{
	free(mapping);
}

static bool cbfs_file_hash_mismatch(const void *buffer, size_t size,
				    const union cbfs_mdata *mdata, bool skip_verification)
{
	if (!CONFIG(LP_CBFS_VERIFICATION) || skip_verification)
		return false;

	const struct vb2_hash *hash = cbfs_file_hash(mdata);
	if (!hash) {
		ERROR("'%s' does not have a file hash!\n", mdata->h.filename);
		return true;
	}
	if (vb2_hash_verify(buffer, size, hash) != VB2_SUCCESS) {
		ERROR("'%s' file hash mismatch!\n", mdata->h.filename);
		return true;
	}

	return false;
}

static size_t cbfs_load_and_decompress(size_t offset, size_t in_size, void *buffer,
				       size_t buffer_size, uint32_t compression,
				       const union cbfs_mdata *mdata, bool skip_verification)
{
	void *load = buffer;
	size_t out_size = 0;

	DEBUG("Decompressing %zu bytes from '%s' to %p with algo %d\n", in_size,
	      mdata->h.filename, buffer, compression);

	if (compression != CBFS_COMPRESS_NONE) {
		load = malloc(in_size);
		if (!load) {
			ERROR("'%s' buffer allocation failed\n", mdata->h.filename);
			return 0;
		}
	}

	if (boot_device_read(load, offset, in_size) != in_size) {
		ERROR("'%s' failed to read contents of file\n", mdata->h.filename);
		goto out;
	}

	if (cbfs_file_hash_mismatch(buffer, in_size, mdata, skip_verification))
		goto out;

	switch (compression) {
	case CBFS_COMPRESS_NONE:
		out_size = in_size;
		break;
	case CBFS_COMPRESS_LZ4:
		if (!CONFIG(LP_LZ4))
			goto out;
		out_size = ulz4fn(load, in_size, buffer, buffer_size);
		break;
	case CBFS_COMPRESS_LZMA:
		if (!CONFIG(LP_LZMA))
			goto out;
		out_size = ulzman(load, in_size, buffer, buffer_size);
		break;
	default:
		ERROR("'%s' decompression algo %d not supported\n", mdata->h.filename,
		      compression);
	}
out:
	if (load != buffer)
		free(load);
	return out_size;
}

static void *do_load(union cbfs_mdata *mdata, ssize_t offset, void *buf, size_t *size_inout,
		     bool skip_verification)
{
	bool malloced = false;
	size_t out_size;
	uint32_t compression = CBFS_COMPRESS_NONE;
	const struct cbfs_file_attr_compression *cattr =
		cbfs_find_attr(mdata, CBFS_FILE_ATTR_TAG_COMPRESSION, sizeof(*cattr));
	if (cattr) {
		compression = be32toh(cattr->compression);
		out_size = be32toh(cattr->decompressed_size);
	} else {
		out_size = be32toh(mdata->h.len);
	}

	if (buf) {
		if (!size_inout || *size_inout < out_size) {
			ERROR("'%s' buffer too small\n", mdata->h.filename);
			return NULL;
		}
	} else {
		buf = malloc(out_size);
		if (!buf) {
			ERROR("'%s' allocation failure\n", mdata->h.filename);
			return NULL;
		}
		malloced = true;
	}

	if (cbfs_load_and_decompress(offset, be32toh(mdata->h.len), buf, out_size, compression,
				     mdata, skip_verification)
	    != out_size) {
		if (malloced)
			free(buf);
		return NULL;
	}
	if (size_inout)
		*size_inout = out_size;

	return buf;
}

void *_cbfs_load(const char *name, void *buf, size_t *size_inout, bool force_ro)
{
	ssize_t offset;
	union cbfs_mdata mdata;

	DEBUG("%s(name='%s', buf=%p, force_ro=%s)\n", __func__, name, buf,
	      force_ro ? "true" : "false");

	offset = _cbfs_boot_lookup(name, force_ro, &mdata);
	if (offset < 0)
		return NULL;

	return do_load(&mdata, offset, buf, size_inout, false);
}

void *_cbfs_unverified_area_load(const char *area, const char *name, void *buf,
				 size_t *size_inout)
{
	struct cbfs_dev dev;
	union cbfs_mdata mdata;
	size_t data_offset;

	DEBUG("%s(area='%s', name='%s', buf=%p)\n", __func__, area, name, buf);

	if (fmap_locate_area(area, &dev.offset, &dev.size) != CB_SUCCESS)
		return NULL;

	if (cbfs_lookup(&dev, name, &mdata, &data_offset, NULL)) {
		ERROR("'%s' not found in '%s'\n", name, area);
		return NULL;
	}

	return do_load(&mdata, dev.offset + data_offset, buf, size_inout, true);
}
