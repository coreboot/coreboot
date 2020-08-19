/*
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

#include <libpayload.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <storage/ata.h>

/** Reads non-sector-aligned blocks of 512 bytes. */
static ssize_t ata_read_unaligned(ata_dev_t *const dev,
				  const lba_t blk_start, size_t blk_count,
				  u8 *blk_buf)
{
	ssize_t ret = 0;
	lba_t sec_start;
	size_t blk_residue;

	u8 *const sec_buf = malloc(dev->sector_size);
	if (sec_buf == NULL)
		return -1;

	const size_t shift = dev->sector_size_shift - 9;
	const size_t mask = (dev->sector_size >> 9) - 1;

	/* Sector aligned start block. */
	const lba_t blk_aligned = blk_start & ~mask;

	/* First sector to read from. */
	sec_start = blk_aligned >> shift;

	/* Calculate and read residue before sector aligned blocks. */
	blk_residue = MIN(blk_start - blk_aligned, blk_count);
	if (blk_residue) {
		if (dev->read_sectors(dev, sec_start, 1, sec_buf) != 1)
			goto _free_ret;

		const size_t bytes = blk_residue << 9;
		memcpy(blk_buf, sec_buf + (dev->sector_size - bytes), bytes);
		blk_count -= blk_residue;
		blk_buf += bytes;
		++sec_start;

		ret = blk_residue;
		if (blk_count == 0)
			goto _free_ret;
	}

	/* Read all sector aligned blocks. */
	const size_t sec_count = (blk_count & ~mask) >> shift;
	const int sec_read = dev->read_sectors(
				dev, sec_start, sec_count, blk_buf);
	if (sec_read < 0)
		goto _free_ret;
	ret += sec_read << shift;
	if (sec_read != sec_count)
		goto _free_ret;

	/* Calculate and read residue. */
	blk_residue = blk_count & mask;
	if (blk_residue) {
		sec_start += sec_read;
		blk_buf += sec_read << dev->sector_size_shift;

		if (dev->read_sectors(dev, sec_start, 1, sec_buf) != 1)
			goto _free_ret;

		const size_t bytes = blk_residue << 9;
		memcpy(blk_buf, sec_buf, bytes);
		ret += blk_residue;
	}

_free_ret:
	free(sec_buf);
	return ret;
}

static ssize_t ata_read512(storage_dev_t *_dev,
			   const lba_t start, const size_t count,
			   unsigned char *const buf)
{
	ata_dev_t *const dev = (ata_dev_t *)_dev;

	if (dev->read_sectors == NULL) {
		printf("ata: No read support implemented.\n");
		return -1;
	}

	if (dev->sector_size == 512) {
		return dev->read_sectors(dev, start, count, buf);
	} else if (dev->sector_size > 512) {
		/* Sector size has to be a power of two. */
		const size_t mask = (dev->sector_size >> 9) - 1;
		if (!(start & mask) && !(count & mask)) {
			const size_t shift = dev->sector_size_shift - 9;
			const ssize_t ret = dev->read_sectors(dev,
					start >> shift, count >> shift, buf);
			if (ret < 0)
				return ret;
			else
				return ret << shift;
		} else {
			return ata_read_unaligned(dev, start, count, buf);
		}
	} else {
		printf("ata: No support for sectors smaller than 512 bytes.\n");
		return -1;
	}
}

static ssize_t ata_write512(storage_dev_t *const dev,
			    const lba_t start, const size_t count,
			    const unsigned char *const buf)
{
	printf("ata: No write support implemented.\n");
	return -1;
}

void ata_initialize_storage_ops(ata_dev_t *const dev)
{
	dev->storage_dev.read_blocks512 = ata_read512;
	dev->storage_dev.write_blocks512 = ata_write512;
}

int ata_set_sector_size(ata_dev_t *const dev, u32 sector_size)
{
	if (!sector_size || (sector_size & (sector_size - 1))) {
		printf("ata: Sector size is not a power of two (%u).\n",
								sector_size);
		return -1;
	}
	dev->sector_size = sector_size;
	dev->sector_size_shift = 0;
	while (sector_size >>= 1)
		++dev->sector_size_shift;

	return 0;
}

static int ata_decode_sector_size(ata_dev_t *const dev, const u16 *const id)
{
	u32 size;
	if ((id[ATA_ID_SECTOR_SIZE] & ((3 << 14) | (1 << 12)))
			!= ((1 << 14) | (1 << 12)))
		size = DEFAULT_ATA_SECTOR_SIZE;
	else
		size = (id[ATA_ID_LOGICAL_SECTOR_SIZE] |
			(id[ATA_ID_LOGICAL_SECTOR_SIZE + 1] << 16)) << 1;

	return ata_set_sector_size(dev, size);
}

/**
 * Copies n-1 bytes from src to dest swapping each two bytes, removes
 * trailing spaces and terminates dest with '\0'.
 */
char *ata_strncpy(char *const dest, const u16 *const src, const size_t n)
{
	int i;

	for (i = 0; i < (n - 1); i += 2) {
		dest[i] = ((const char *)src)[i + 1];
		dest[i + 1] = ((const char *)src)[i];
	}

	for (i = n - 2; i >= 0; --i)
		if (dest[i] != ' ')
			break;
	dest[i + 1] = '\0';

	return dest;
}

int ata_attach_device(ata_dev_t *const dev, const storage_port_t port_type)
{
	u16 id[256];

	dev->identify_cmd = ATA_IDENTIFY_DEVICE;
	if (dev->identify(dev, (u8 *)id))
		return -1;

	char fw[9], model[41];
	ata_strncpy(fw, id + 23, sizeof(fw));
	ata_strncpy(model, id + 27, sizeof(model));
	printf("ata: Identified %s [%s]\n", model, fw);

#if CONFIG(LP_STORAGE_64BIT_LBA)
	if (id[ATA_CMDS_AND_FEATURE_SETS + 1] & (1 << 10)) {
		printf("ata: Support for LBA-48 enabled.\n");
		dev->read_cmd = ATA_READ_DMA_EXT;
	} else {
		dev->read_cmd = ATA_READ_DMA;
	}
#else
	dev->read_cmd = ATA_READ_DMA;
#endif

	if (ata_decode_sector_size(dev, id))
		return -1;

	dev->storage_dev.port_type = port_type;
	ata_initialize_storage_ops(dev);

	return storage_attach_device(&dev->storage_dev);
}
