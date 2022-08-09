/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#include <commonlib/bsd/cbfs_private.h>
#include <assert.h>

static enum cb_err read_next_header(cbfs_dev_t dev, size_t *offset, struct cbfs_file *buffer,
				    const size_t devsize)
{
	DEBUG("Looking for next file @%#zx...\n", *offset);
	*offset = ALIGN_UP(*offset, CBFS_ALIGNMENT);
	while (*offset + sizeof(*buffer) < devsize) {
		if (cbfs_dev_read(dev, buffer, *offset, sizeof(*buffer)) != sizeof(*buffer))
			return CB_CBFS_IO;

		if (memcmp(buffer->magic, CBFS_FILE_MAGIC, sizeof(buffer->magic)) == 0)
			return CB_SUCCESS;

		*offset += CBFS_ALIGNMENT;
	}

	DEBUG("End of CBFS reached\n");
	return CB_CBFS_NOT_FOUND;
}

enum cb_err cbfs_walk(cbfs_dev_t dev, enum cb_err (*walker)(cbfs_dev_t dev, size_t offset,
							    const union cbfs_mdata *mdata,
							    size_t already_read, void *arg),
		      void *arg, struct vb2_hash *metadata_hash, enum cbfs_walk_flags flags)
{
	const bool do_hash = CBFS_ENABLE_HASHING && metadata_hash;
	const size_t devsize = cbfs_dev_size(dev);
	struct vb2_digest_context dc;

	assert(CBFS_ENABLE_HASHING || (!metadata_hash && !(flags & CBFS_WALK_WRITEBACK_HASH)));
	if (do_hash && vb2_digest_init(&dc, CBFS_HASH_HWCRYPTO, metadata_hash->algo, 0))
		return CB_ERR_ARG;

	size_t offset = 0;
	enum cb_err ret_header;
	enum cb_err ret_walker = CB_CBFS_NOT_FOUND;
	union cbfs_mdata mdata;
	while ((ret_header = read_next_header(dev, &offset, &mdata.h, devsize)) == CB_SUCCESS) {
		const uint32_t attr_offset = be32toh(mdata.h.attributes_offset);
		const uint32_t data_offset = be32toh(mdata.h.offset);
		const uint32_t data_length = be32toh(mdata.h.len);
		const uint32_t type = be32toh(mdata.h.type);
		const bool empty = (type == CBFS_TYPE_DELETED || type == CBFS_TYPE_NULL);

		DEBUG("Found CBFS header @%#zx (type %d, attr +%#x, data +%#x, length %#x)\n",
		      offset, type, attr_offset, data_offset, data_length);
		if (data_offset > sizeof(mdata) || data_length > devsize ||
		    offset + data_offset + data_length > devsize) {
			ERROR("File @%#zx too large\n", offset);
			offset += CBFS_ALIGNMENT;
			continue;
		}

		if (empty && !(flags & CBFS_WALK_INCLUDE_EMPTY))
			goto next_file;

		/* When hashing we need to read everything. Otherwise skip the attributes.
		   attr_offset may be 0, which means there are no attributes. */
		ssize_t todo;
		if (do_hash || attr_offset == 0)
			todo = data_offset - sizeof(mdata.h);
		else
			todo = attr_offset - sizeof(mdata.h);
		if (todo <= 0 || data_offset < attr_offset) {
			ERROR("Corrupt file header @%#zx\n", offset);
			goto next_file;
		}

		/* Read the rest of the metadata (filename, and possibly attributes). */
		assert(todo > 0 && todo <= sizeof(mdata) - sizeof(mdata.h));
		if (cbfs_dev_read(dev, mdata.raw + sizeof(mdata.h),
				  offset + sizeof(mdata.h), todo) != todo)
			return CB_CBFS_IO;
		/* Force filename null-termination, just in case. */
		mdata.raw[attr_offset ? attr_offset - 1 : data_offset - 1] = '\0';
		DEBUG("File name: '%s'\n", mdata.h.filename);

		if (do_hash && !empty && vb2_digest_extend(&dc, mdata.raw, data_offset))
			return CB_ERR;

		if (walker && ret_walker == CB_CBFS_NOT_FOUND)
			ret_walker = walker(dev, offset, &mdata, sizeof(mdata.h) + todo, arg);

		/* Return IO errors immediately. For others, finish the hash first if needed. */
		if (ret_walker == CB_CBFS_IO || (ret_walker != CB_CBFS_NOT_FOUND && !do_hash))
			return ret_walker;

next_file:
		offset += data_offset + data_length;
	}

	if (ret_header != CB_CBFS_NOT_FOUND)
		return ret_header;

	if (do_hash) {
		uint8_t real_hash[VB2_MAX_DIGEST_SIZE];
		size_t hash_size = vb2_digest_size(metadata_hash->algo);
		if (vb2_digest_finalize(&dc, real_hash, hash_size))
			return CB_ERR;
		if (flags & CBFS_WALK_WRITEBACK_HASH)
			memcpy(metadata_hash->raw, real_hash, hash_size);
		else if (memcmp(metadata_hash->raw, real_hash, hash_size) != 0)
			return CB_CBFS_HASH_MISMATCH;
	}

	return ret_walker;
}

enum cb_err cbfs_copy_fill_metadata(union cbfs_mdata *dst, const union cbfs_mdata *src,
				    size_t already_read, cbfs_dev_t dev, size_t offset)
{
	/* First, copy the stuff that cbfs_walk() already read for us. */
	memcpy(dst, src, already_read);

	/* Then read in whatever metadata may be left (will only happen in non-hashing case). */
	const size_t todo = be32toh(src->h.offset) - already_read;
	assert(todo <= sizeof(*dst) - already_read);
	if (todo && cbfs_dev_read(dev, dst->raw + already_read, offset + already_read,
				  todo) != todo)
		return CB_CBFS_IO;
	return CB_SUCCESS;
}

struct cbfs_lookup_args {
	union cbfs_mdata *mdata_out;
	const char *name;
	size_t namesize;
	size_t *data_offset_out;
};

static enum cb_err lookup_walker(cbfs_dev_t dev, size_t offset, const union cbfs_mdata *mdata,
				 size_t already_read, void *arg)
{
	struct cbfs_lookup_args *args = arg;
	/* Check if the name we're looking for could fit, then we can safely memcmp() it. */
	if (args->namesize > already_read - offsetof(union cbfs_mdata, h.filename) ||
	    memcmp(args->name, mdata->h.filename, args->namesize) != 0)
		return CB_CBFS_NOT_FOUND;

	LOG("Found '%s' @%#zx size %#x\n", args->name, offset, be32toh(mdata->h.len));
	if (cbfs_copy_fill_metadata(args->mdata_out, mdata, already_read, dev, offset))
		return CB_CBFS_IO;

	*args->data_offset_out = offset + be32toh(mdata->h.offset);
	return CB_SUCCESS;
}

enum cb_err cbfs_lookup(cbfs_dev_t dev, const char *name, union cbfs_mdata *mdata_out,
			size_t *data_offset_out, struct vb2_hash *metadata_hash)
{
	struct cbfs_lookup_args args = {
		.mdata_out = mdata_out,
		.name = name,
		.namesize = strlen(name) + 1,	/* Count trailing \0 so we can memcmp() it. */
		.data_offset_out = data_offset_out,
	};
	return cbfs_walk(dev, lookup_walker, &args, metadata_hash, 0);
}

const void *cbfs_find_attr(const union cbfs_mdata *mdata, uint32_t attr_tag, size_t size_check)
{
	uint32_t offset = be32toh(mdata->h.attributes_offset);
	uint32_t end = be32toh(mdata->h.offset);

	if (!offset)
		return NULL;

	while (offset + sizeof(struct cbfs_file_attribute) <= end) {
		const struct cbfs_file_attribute *attr = (const void *)mdata->raw + offset;
		const uint32_t tag = be32toh(attr->tag);
		const uint32_t len = be32toh(attr->len);

		if (len < sizeof(struct cbfs_file_attribute) || len > end - offset) {
			ERROR("Attribute %s[%x] invalid length: %u\n",
			      mdata->h.filename, tag, len);
			return NULL;
		}
		if (tag == attr_tag) {
			if (size_check && len != size_check) {
				ERROR("Attribute %s[%x] size mismatch: %u != %zu\n",
				      mdata->h.filename, tag, len, size_check);
				return NULL;
			}
			return attr;
		}
		offset += len;
	}

	return NULL;
}

const struct vb2_hash *cbfs_file_hash(const union cbfs_mdata *mdata)
{
	/* Hashes are variable-length attributes, so need to manually check the length. */
	const struct cbfs_file_attr_hash *attr =
		cbfs_find_attr(mdata, CBFS_FILE_ATTR_TAG_HASH, 0);
	if (!attr)
		return NULL;	/* no hash */
	const size_t asize = be32toh(attr->len);

	const struct vb2_hash *hash = &attr->hash;
	const size_t hsize = vb2_digest_size(hash->algo);
	if (!hsize) {
		ERROR("Hash algo %u for '%s' unsupported.\n", hash->algo, mdata->h.filename);
		return NULL;
	}
	if (hsize != asize - offsetof(struct cbfs_file_attr_hash, hash.raw)) {
		ERROR("Hash attribute size for '%s' (%zu) incorrect for algo %u.\n",
		      mdata->h.filename, asize, hash->algo);
		return NULL;
	}
	return hash;
}
