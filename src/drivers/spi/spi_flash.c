/*
 * SPI flash interface
 *
 * Copyright (C) 2008 Atmel Corporation
 * Copyright (C) 2010 Reinhard Meyer, EMK Elektronik
 *
 * Licensed under the GPL-2 or later.
 */

#include <cpu/x86/smm.h>
#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <spi-generic.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"
#include <timer.h>

static void spi_flash_addr(u32 addr, u8 *cmd)
{
	/* cmd[0] is actual command */
	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr >> 0;
}

/*
 * If atomic sequencing is used, the cycle type is known to the SPI
 * controller so that it can perform consecutive transfers and arbitrate
 * automatically. Otherwise the SPI controller transfers whatever the
 * user requests immediately, without regard to sequence. Atomic
 * sequencing is commonly used on x86 platforms.
 *
 * SPI flash commands are simple two-step sequences. The command byte is
 * always written first and may be followed by an address. Then data is
 * either read or written. For atomic sequencing we'll pass everything into
 * spi_xfer() at once and let the controller handle the details. Otherwise
 * we will write all output bytes first and then read if necessary.
 *
 * FIXME: This really should be abstracted better, but that will
 * require overhauling the entire SPI infrastructure.
 */
static int do_spi_flash_cmd(struct spi_slave *spi, const void *dout,
		unsigned int bytes_out, void *din, unsigned int bytes_in)
{
	int ret = 1;

	if (spi_claim_bus(spi))
		return ret;

#if CONFIG_SPI_ATOMIC_SEQUENCING == 1
	if (spi_xfer(spi, dout, bytes_out, din, bytes_in) < 0)
		goto done;
#else
	if (dout && bytes_out) {
		if (spi_xfer(spi, dout, bytes_out, NULL, 0) < 0)
			goto done;
	}

	if (din && bytes_in) {
		if (spi_xfer(spi, NULL, 0, din, bytes_in) < 0)
			goto done;
	}
#endif

	ret = 0;
done:
	spi_release_bus(spi);
	return ret;
}

int spi_flash_cmd(struct spi_slave *spi, u8 cmd, void *response, size_t len)
{
	int ret = do_spi_flash_cmd(spi, &cmd, sizeof(cmd), response, len);
	if (ret)
		printk(BIOS_WARNING, "SF: Failed to send command %02x: %d\n", cmd, ret);

	return ret;
}

int spi_flash_cmd_read(struct spi_slave *spi, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len)
{
	int ret = do_spi_flash_cmd(spi, cmd, cmd_len, data, data_len);
	if (ret) {
		printk(BIOS_WARNING, "SF: Failed to send read command (%zu bytes): %d\n",
				data_len, ret);
	}

	return ret;
}

int spi_flash_cmd_write(struct spi_slave *spi, const u8 *cmd, size_t cmd_len,
		const void *data, size_t data_len)
{
	int ret;
	u8 buff[cmd_len + data_len];
	memcpy(buff, cmd, cmd_len);
	memcpy(buff + cmd_len, data, data_len);

	ret = do_spi_flash_cmd(spi, buff, cmd_len + data_len, NULL, 0);
	if (ret) {
		printk(BIOS_WARNING, "SF: Failed to send write command (%zu bytes): %d\n",
				data_len, ret);
	}

	return ret;
}

int spi_flash_read_common(struct spi_flash *flash, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len)
{
	struct spi_slave *spi = flash->spi;
	int ret;

	spi->rw = SPI_READ_FLAG;
	ret = spi_flash_cmd_read(spi, cmd, cmd_len, data, data_len);

	return ret;
}

int spi_flash_cmd_read_fast(struct spi_flash *flash, u32 offset,
		size_t len, void *data)
{
	struct spi_slave *spi = flash->spi;
	u8 cmd[5];

	cmd[0] = CMD_READ_ARRAY_FAST;
	spi_flash_addr(offset, cmd);
	cmd[4] = 0x00;

	return spi_flash_cmd_read(spi, cmd, sizeof(cmd), data, len);
}

int spi_flash_cmd_read_slow(struct spi_flash *flash, u32 offset,
		size_t len, void *data)
{
	struct spi_slave *spi = flash->spi;
	u8 cmd[4];

	cmd[0] = CMD_READ_ARRAY_SLOW;
	spi_flash_addr(offset, cmd);

	return spi_flash_cmd_read(spi, cmd, sizeof(cmd), data, len);
}

int spi_flash_cmd_poll_bit(struct spi_flash *flash, unsigned long timeout,
			   u8 cmd, u8 poll_bit)
{
	struct spi_slave *spi = flash->spi;
	int ret;
	u8 status;
	struct mono_time current, end;

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_msecs(&end, timeout);

	do {
		ret = spi_flash_cmd_read(spi, &cmd, 1, &status, 1);
		if (ret)
			return -1;
		if ((status & poll_bit) == 0)
			return 0;
		timer_monotonic_get(&current);
	} while (!mono_time_after(&current, &end));

	printk(BIOS_DEBUG, "SF: timeout at %ld msec\n",timeout);
	return -1;
}

int spi_flash_cmd_wait_ready(struct spi_flash *flash, unsigned long timeout)
{
	return spi_flash_cmd_poll_bit(flash, timeout,
		CMD_READ_STATUS, STATUS_WIP);
}

int spi_flash_cmd_erase(struct spi_flash *flash, u8 erase_cmd,
			u32 offset, size_t len)
{
	u32 start, end, erase_size;
	int ret;
	u8 cmd[4];

	erase_size = flash->sector_size;
	if (offset % erase_size || len % erase_size) {
		printk(BIOS_WARNING, "SF: Erase offset/length not multiple of erase size\n");
		return -1;
	}

	flash->spi->rw = SPI_WRITE_FLAG;

	cmd[0] = erase_cmd;
	start = offset;
	end = start + len;

	while (offset < end) {
		spi_flash_addr(offset, cmd);
		offset += erase_size;

#if CONFIG_DEBUG_SPI_FLASH
		printk(BIOS_SPEW, "SF: erase %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
		      cmd[2], cmd[3], offset);
#endif
		ret = spi_flash_cmd(flash->spi, CMD_WRITE_ENABLE, NULL, 0);
		if (ret)
			goto out;

		ret = spi_flash_cmd_write(flash->spi, cmd, sizeof(cmd), NULL, 0);
		if (ret)
			goto out;

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);
		if (ret)
			goto out;
	}

	printk(BIOS_DEBUG, "SF: Successfully erased %zu bytes @ %#x\n", len, start);

out:
	return ret;
}

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
 * IDCODE_CONT_LEN may be redefined if a device needs to declare a
 * larger "shift" value.  IDCODE_PART_LEN generally shouldn't be
 * changed.  This is the max number of bytes probe functions may
 * examine when looking up part-specific identification info.
 *
 * Probe functions will be given the idcode buffer starting at their
 * manu id byte (the "idcode" in the table below).  In other words,
 * all of the continuation bytes will be skipped (the "shift" below).
 */
#define IDCODE_CONT_LEN 0
#define IDCODE_PART_LEN 5
static struct {
	const u8 shift;
	const u8 idcode;
	struct spi_flash *(*probe) (struct spi_slave *spi, u8 *idcode);
} flashes[] = {
	/* Keep it sorted by define name */
#if CONFIG_SPI_FLASH_AMIC
	{ 0, 0x37, spi_flash_probe_amic, },
#endif
#if CONFIG_SPI_FLASH_ATMEL
	{ 0, 0x1f, spi_flash_probe_atmel, },
#endif
#if CONFIG_SPI_FLASH_EON
	{ 0, 0x1c, spi_flash_probe_eon, },
#endif
#if CONFIG_SPI_FLASH_GIGADEVICE
	{ 0, 0xc8, spi_flash_probe_gigadevice, },
#endif
#if CONFIG_SPI_FLASH_MACRONIX
	{ 0, 0xc2, spi_flash_probe_macronix, },
#endif
#if CONFIG_SPI_FLASH_SPANSION
	{ 0, 0x01, spi_flash_probe_spansion, },
#endif
#if CONFIG_SPI_FLASH_SST
	{ 0, 0xbf, spi_flash_probe_sst, },
#endif
#if CONFIG_SPI_FLASH_STMICRO
	{ 0, 0x20, spi_flash_probe_stmicro, },
#endif
#if CONFIG_SPI_FLASH_WINBOND
	{ 0, 0xef, spi_flash_probe_winbond, },
#endif
	/* Keep it sorted by best detection */
#if CONFIG_SPI_FLASH_STMICRO
	{ 0, 0xff, spi_flash_probe_stmicro, },
#endif
#if CONFIG_SPI_FLASH_ADESTO
	{ 0, 0x1f, spi_flash_probe_adesto, },
#endif
};
#define IDCODE_LEN (IDCODE_CONT_LEN + IDCODE_PART_LEN)

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs)
{
	struct spi_slave *spi;
	struct spi_flash *flash = NULL;
	int ret, i, shift;
	u8 idcode[IDCODE_LEN], *idp;

	spi = spi_setup_slave(bus, cs);
	if (!spi) {
		printk(BIOS_WARNING, "SF: Failed to set up slave\n");
		return NULL;
	}

	spi->rw = SPI_READ_FLAG;

	if (spi->force_programmer_specific && spi->programmer_specific_probe) {
		flash = spi->programmer_specific_probe (spi);
		if (!flash)
			goto err_read_id;
		goto flash_detected;
	}

	/* Read the ID codes */
	ret = spi_flash_cmd(spi, CMD_READ_ID, idcode, sizeof(idcode));
	if (ret)
		goto err_read_id;

#if CONFIG_DEBUG_SPI_FLASH
	printk(BIOS_SPEW, "SF: Got idcode: ");
	for (i = 0; i < sizeof(idcode); i++)
		printk(BIOS_SPEW, "%02x ", idcode[i]);
	printk(BIOS_SPEW, "\n");
#endif

	/* count the number of continuation bytes */
	for (shift = 0, idp = idcode;
	     shift < IDCODE_CONT_LEN && *idp == 0x7f;
	     ++shift, ++idp)
		continue;

	/* search the table for matches in shift and id */
	for (i = 0; i < ARRAY_SIZE(flashes); ++i)
		if (flashes[i].shift == shift && flashes[i].idcode == *idp) {
#if CONFIG_SMM_TSEG && defined(__SMM__)
			/* Need to relocate this function */
			tseg_relocate((void **)&flashes[i].probe);
#endif
			/* we have a match, call probe */
			flash = flashes[i].probe(spi, idp);
			if (flash)
				break;
		}

	if (!flash && spi->programmer_specific_probe) {
#if CONFIG_SMM_TSEG && defined(__SMM__)
		/* Need to relocate this function */
		tseg_relocate((void **)&spi->programmer_specific_probe);
#endif
		flash = spi->programmer_specific_probe (spi);
	}
	if (!flash) {
		printk(BIOS_WARNING, "SF: Unsupported manufacturer %02x\n", *idp);
		goto err_manufacturer_probe;
	}

flash_detected:
#if CONFIG_SMM_TSEG && defined(__SMM__)
	/* Ensure flash handlers are valid for TSEG */
	tseg_relocate((void **)&flash->read);
	tseg_relocate((void **)&flash->write);
	tseg_relocate((void **)&flash->erase);
	tseg_relocate((void **)&flash->name);
#endif

	printk(BIOS_INFO, "SF: Detected %s with page size %x, total %x\n",
			flash->name, flash->sector_size, flash->size);

	return flash;

err_manufacturer_probe:
err_read_id:
	return NULL;
}
