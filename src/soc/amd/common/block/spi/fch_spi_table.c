/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Silverback Ltd.
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

#include <spi_flash.h>
#include <amdblocks/fch_spi.h>
#include <drivers/spi/spi_flash_internal.h>

/*
 * The following table holds all device probe functions
 *
 * shift:  number of continuation bytes before the ID
 * idcode: the expected IDCODE or 0xff for non JEDEC devices
 * probe:  the function to call
 *
 * Non JEDEC devices should be ordered in the table such that
 * the probe functions with best detection algorithms come first.
 *
 * Several matching entries are permitted, they will be tried
 * in sequence until a probe function returns non NULL.
 *
 * Probe functions will be given the idcode buffer starting at their
 * manu id byte (the "idcode" in the table below).  In other words,
 * all of the continuation bytes will be skipped (the "shift" below).
 */

const struct spi_flash_table flashes[] = {
	/* Keep it sorted by define name */
#if CONFIG(SPI_FLASH_ADESTO)
	{ 0, VENDOR_ID_ADESTO, spi_flash_probe_adesto, },
#endif
#if CONFIG(SPI_FLASH_AMIC)
	{ 0, VENDOR_ID_AMIC, spi_flash_probe_amic, },
#endif
#if CONFIG(SPI_FLASH_ATMEL)
	{ 0, VENDOR_ID_ATMEL, spi_flash_probe_atmel, },
#endif
#if CONFIG(SPI_FLASH_EON)
	{ 0, VENDOR_ID_EON, spi_flash_probe_eon, },
#endif
#if CONFIG(SPI_FLASH_GIGADEVICE)
	{ 0, VENDOR_ID_GIGADEVICE, spi_flash_probe_gigadevice, },
#endif
#if CONFIG(SPI_FLASH_MACRONIX)
	{ 0, VENDOR_ID_MACRONIX, spi_flash_probe_macronix, },
#endif
#if CONFIG(SPI_FLASH_SPANSION)
	{ 0, VENDOR_ID_SPANSION, spi_flash_probe_spansion, },
#endif
#if CONFIG(SPI_FLASH_SST)
	{ 0, VENDOR_ID_SST, spi_flash_probe_sst, },
#endif
#if CONFIG(SPI_FLASH_STMICRO)
	{ 0, VENDOR_ID_STMICRO, spi_flash_probe_stmicro, },
	{ 0, VENDOR_ID_STMICRO_FF, spi_flash_probe_stmicro, },
#endif
#if CONFIG(SPI_FLASH_WINBOND)
	{ 0, VENDOR_ID_WINBOND, spi_flash_probe_winbond, },
#endif
	/* Keep it sorted by best detection */
};

const struct spi_flash_table *get_spi_flash_table(int *table_size)
{
	*table_size = (int)ARRAY_SIZE(flashes);
	return &flashes[0];
}
