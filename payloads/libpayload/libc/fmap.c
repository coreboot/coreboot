/*
 *
 * Copyright (C) 2015 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload-config.h>
#include <libpayload.h>
#include <coreboot_tables.h>
#include <cbfs.h>
#include <fmap_serialized.h>
#include <stdint.h>

int fmap_region_by_name(const uint32_t fmap_offset, const char * const name,
			uint32_t * const offset, uint32_t * const size)
{
	int i;

	struct fmap *fmap;
	struct fmap fmap_head;
	struct cbfs_media default_media;
	struct cbfs_media *media = &default_media;

	if (init_default_cbfs_media(media) != 0)
		return -1;

	media->open(media);

	if (!media->read(media, &fmap_head, fmap_offset, sizeof(fmap_head)))
		return -1;

	if (memcmp(fmap_head.signature, FMAP_SIGNATURE, sizeof(fmap_head.signature))) {
		return -1;
	}

	int fmap_size = sizeof(*fmap) +
		fmap_head.nareas * sizeof(struct fmap_area);

	fmap = malloc(fmap_size);
	if (!fmap)
		return -1;

	if (!media->read(media, fmap, fmap_offset, fmap_size))
		goto err;

	media->close(media);

	for (i = 0; i < fmap->nareas; i++) {
		if (strcmp((const char *)fmap->areas[i].name, name) != 0)
			continue;
		if (offset)
			*offset = fmap->areas[i].offset;
		if (size)
			*size = fmap->areas[i].size;
		free(fmap);
		return 0;
	}
err:
	free(fmap);
	return -1;
}
