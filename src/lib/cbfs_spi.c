/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * This file provides a common CBFS wrapper for SPI storage. SPI driver
 * context is expanded with the buffer descriptor used to store data read from
 * SPI.
 */

#include <cbfs.h>
#include <spi_flash.h>

/* SPI flash as CBFS media. */
struct cbfs_spi_context {
	struct spi_flash *spi_flash_info;
	struct cbfs_simple_buffer buffer;
};

static struct cbfs_spi_context spi_context;

static int cbfs_media_open(struct cbfs_media *media)
{
	return 0;
}

static int cbfs_media_close(struct cbfs_media *media)
{
	return 0;
}

static size_t cbfs_media_read(struct cbfs_media *media,
			      void *dest, size_t offset,
			      size_t count)
{
	struct cbfs_spi_context *context = media->context;

	return context->spi_flash_info->read
		(context->spi_flash_info, offset, count, dest) ? 0 : count;
}

static void *cbfs_media_map(struct cbfs_media *media,
			    size_t offset, size_t count)
{
	struct cbfs_spi_context *context = media->context;

	return cbfs_simple_buffer_map(&context->buffer, media, offset, count);
}

static void *cbfs_media_unmap(struct cbfs_media *media,
			       const void *address)
{
	struct cbfs_spi_context *context = media->context;

	return cbfs_simple_buffer_unmap(&context->buffer, address);
}

static int init_cbfs_media_context(void)
{
	if (!spi_context.spi_flash_info) {

		spi_context.spi_flash_info = spi_flash_probe
			(CONFIG_BOOT_MEDIA_SPI_BUS, 0);

		if (!spi_context.spi_flash_info)
			return -1;

		spi_context.buffer.buffer = (void *)CONFIG_CBFS_CACHE_ADDRESS;
		spi_context.buffer.size = CONFIG_CBFS_CACHE_SIZE;
	}
	return 0;

}
int init_default_cbfs_media(struct cbfs_media *media)
{
	media->context = &spi_context;
	media->open = cbfs_media_open;
	media->close = cbfs_media_close;
	media->read = cbfs_media_read;
	media->map = cbfs_media_map;
	media->unmap = cbfs_media_unmap;

	return init_cbfs_media_context();
}
