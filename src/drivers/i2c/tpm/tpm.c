/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Description:
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * This device driver implements the TPM interface as defined in
 * the TCG TPM Interface Spec version 1.2, revision 1.0 and the
 * Infineon I2C Protocol Stack Specification v0.20.
 *
 * It is based on the Linux kernel driver tpm.c from Leendert van
 * Dorn, Dave Safford, Reiner Sailer, and Kyleen Hall.
 *
 * Version: 2.1.1
 */

#include <commonlib/endian.h>
#include <string.h>
#include <types.h>
#include <delay.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <endian.h>
#include <timer.h>
#include <security/tpm/tis.h>
#include "tpm.h"

/* max. number of iterations after I2C NAK */
#define MAX_COUNT 3

#define SLEEP_DURATION 60 /* in usec */
#define SLEEP_DURATION_LONG 210 /* in usec */
#define SLEEP_DURATION_PROBE_MS 1000 /* in msec */

/* max. number of iterations after I2C NAK for 'long' commands
 * we need this especially for sending TPM_READY, since the cleanup after the
 * transition to the ready state may take some time, but it is unpredictable
 * how long it will take.
 */
#define MAX_COUNT_LONG 50

/* expected value for DIDVID register */
#define TPM_TIS_I2C_DID_VID_9635 0x000b15d1L
#define TPM_TIS_I2C_DID_VID_9645 0x001a15d1L

enum i2c_chip_type {
	SLB9635,
	SLB9645,
	UNKNOWN,
};

static const char *const chip_name[] = {
	[SLB9635] = "slb9635tt",
	[SLB9645] = "slb9645tt",
	[UNKNOWN] = "unknown/fallback to slb9635",
};

/* Structure to store I2C TPM specific stuff */
struct tpm_inf_dev {
	int bus;
	unsigned int addr;
	unsigned int sleep_short; /* Short sleep duration in usec */
	unsigned int sleep_long; /* Long sleep duration in usec */
	uint8_t buf[TPM_BUFSIZE + sizeof(uint8_t)]; // max. buffer size + addr
	enum i2c_chip_type chip_type;
};

static struct tpm_inf_dev tpm_dev;

/*
 * iic_tpm_read() - read from TPM register
 * @addr: register address to read from
 * @buffer: provided by caller
 * @len: number of bytes to read
 *
 * Read len bytes from TPM register and put them into
 * buffer (little-endian format, i.e. first byte is put into buffer[0]).
 *
 * NOTE: TPM is big-endian for multi-byte values. Multi-byte
 * values have to be swapped.
 *
 * Return -1 on error, 0 on success.
 */
static int iic_tpm_read(uint8_t addr, uint8_t *buffer, size_t len)
{
	int rc;
	int count;

	if (tpm_dev.addr == 0)
		return -1;

	switch (tpm_dev.chip_type) {
	case SLB9635:
	case UNKNOWN:
		/* slb9635 protocol should work in both cases */
		for (count = 0; count < MAX_COUNT; count++) {
			rc = i2c_write_raw(tpm_dev.bus, tpm_dev.addr,
					   &addr, 1);
			if (rc == 0)
				break;  /* success, break to skip sleep */

			udelay(tpm_dev.sleep_short);
		}

		if (rc)
			return -1;

		/* After the TPM has successfully received the register address
		 * it needs some time, thus we're sleeping here again, before
		 * retrieving the data
		 */
		for (count = 0; count < MAX_COUNT; count++) {
			udelay(tpm_dev.sleep_short);
			rc = i2c_read_raw(tpm_dev.bus, tpm_dev.addr,
					  buffer, len);
			if (rc == 0)
				break;  /* success, break to skip sleep */

		}
		break;

	default:
	{
		/* use a combined read for newer chips
		 * unfortunately the smbus functions are not suitable due to
		 * the 32 byte limit of the smbus.
		 * retries should usually not be needed, but are kept just to
		 * be safe on the safe side.
		 */
		struct i2c_msg aseg = { .flags = 0, .slave = tpm_dev.addr,
					.buf = &addr, .len = 1 };
		struct i2c_msg dseg = { .flags = I2C_M_RD,
					.slave = tpm_dev.addr,
					.buf = buffer, .len = len };
		for (count = 0; count < MAX_COUNT; count++) {
			rc = i2c_transfer(tpm_dev.bus, &aseg, 1) ||
			     i2c_transfer(tpm_dev.bus, &dseg, 1);
			if (rc == 0)
				break;  /* break here to skip sleep */
			udelay(tpm_dev.sleep_short);
		}
	}
	}

	/* take care of 'guard time' */
	udelay(tpm_dev.sleep_short);
	if (rc)
		return -1;

	return 0;
}

static int iic_tpm_write_generic(uint8_t addr, uint8_t *buffer, size_t len,
				unsigned int sleep_time,
				uint8_t max_count)
{
	int rc = 0;
	int count;

	if (len > TPM_BUFSIZE) {
		printk(BIOS_DEBUG, "%s: Length %zd is too large\n",
			__func__, len);
		return -1;
	}

	/* prepare send buffer */
	tpm_dev.buf[0] = addr;
	memcpy(&(tpm_dev.buf[1]), buffer, len);

	if (tpm_dev.addr == 0)
		return -1;
	for (count = 0; count < max_count; count++) {
		rc = i2c_write_raw(tpm_dev.bus, tpm_dev.addr,
				   tpm_dev.buf, len + 1);
		if (rc == 0)
			break;  /* success, break to skip sleep */

		udelay(sleep_time);
	}

	/* take care of 'guard time' */
	udelay(tpm_dev.sleep_short);
	if (rc)
		return -1;

	return 0;
}

/*
 * iic_tpm_write() - write to TPM register
 * @addr: register address to write to
 * @buffer: containing data to be written
 * @len: number of bytes to write
 *
 * Write len bytes from provided buffer to TPM register (little
 * endian format, i.e. buffer[0] is written as first byte).
 *
 * NOTE: TPM is big-endian for multi-byte values. Multi-byte
 * values have to be swapped.
 *
 * NOTE: use this function instead of the iic_tpm_write_generic function.
 *
 * Return -EIO on error, 0 on success
 */
static int iic_tpm_write(uint8_t addr, uint8_t *buffer, size_t len)
{
	return iic_tpm_write_generic(addr, buffer, len, tpm_dev.sleep_short,
				     MAX_COUNT);
}

/*
 * This function is needed especially for the cleanup situation after
 * sending TPM_READY
 * */
static int iic_tpm_write_long(uint8_t addr, uint8_t *buffer, size_t len)
{
	return iic_tpm_write_generic(addr, buffer, len, tpm_dev.sleep_long,
				     MAX_COUNT_LONG);
}

static int check_locality(struct tpm_chip *chip, int loc)
{
	uint8_t buf;

	if (iic_tpm_read(TPM_ACCESS(loc), &buf, 1) < 0)
		return -1;

	if ((buf & (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID)) ==
		(TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID)) {
		chip->vendor.locality = loc;
		return loc;
	}

	return -1;
}

static void release_locality(struct tpm_chip *chip, int loc, int force)
{
	uint8_t buf;
	if (iic_tpm_read(TPM_ACCESS(loc), &buf, 1) < 0)
		return;

	if (force || (buf & (TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID)) ==
			(TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID)) {
		buf = TPM_ACCESS_ACTIVE_LOCALITY;
		iic_tpm_write(TPM_ACCESS(loc), &buf, 1);
	}
}

static int request_locality(struct tpm_chip *chip, int loc)
{
	uint8_t buf = TPM_ACCESS_REQUEST_USE;

	if (check_locality(chip, loc) >= 0)
		return loc; /* we already have the locality */

	iic_tpm_write(TPM_ACCESS(loc), &buf, 1);

	/* wait for burstcount */
	int timeout = 2 * 1000; /* 2s timeout */
	while (timeout) {
		if (check_locality(chip, loc) >= 0)
			return loc;
		mdelay(TPM_TIMEOUT);
		timeout--;
	}

	return -1;
}

static uint8_t tpm_tis_i2c_status(struct tpm_chip *chip)
{
	/* NOTE: Since I2C read may fail, return 0 in this case --> time-out */
	uint8_t buf;
	if (iic_tpm_read(TPM_STS(chip->vendor.locality), &buf, 1) < 0)
		return 0;
	else if (buf == 0xff)	/* Some TPMs sometimes randomly return 0xff. */
		return 0;
	else
		return buf;
}

static void tpm_tis_i2c_ready(struct tpm_chip *chip)
{
	/* this causes the current command to be aborted */
	uint8_t buf = TPM_STS_COMMAND_READY;
	iic_tpm_write_long(TPM_STS(chip->vendor.locality), &buf, 1);
}

static ssize_t get_burstcount(struct tpm_chip *chip)
{
	ssize_t burstcnt;
	uint8_t buf[3];

	/* wait for burstcount */
	int timeout = 2 * 1000; /* 2s timeout */
	while (timeout) {
		/* Note: STS is little endian */
		if (iic_tpm_read(TPM_STS(chip->vendor.locality) + 1, buf, 3)
			< 0)
			burstcnt = 0;
		else
			burstcnt = (buf[2] << 16) + (buf[1] << 8) + buf[0];

		if (burstcnt && burstcnt != 0xffffff)
			return burstcnt;
		mdelay(TPM_TIMEOUT);
		timeout--;
	}
	return -1;
}

static int wait_for_stat(struct tpm_chip *chip, uint8_t mask, int *status)
{
	unsigned long timeout = 2 * 1024;
	while (timeout) {
		*status = tpm_tis_i2c_status(chip);
		if ((*status & mask) == mask)
			return 0;
		mdelay(TPM_TIMEOUT);
		timeout--;
	}

	return -1;
}

static int recv_data(struct tpm_chip *chip, uint8_t *buf, size_t count)
{
	size_t size = 0;

	while (size < count) {
		ssize_t burstcnt = get_burstcount(chip);
		int rc;

		/* burstcount < 0 = TPM is busy */
		if (burstcnt < 0)
			return burstcnt;

		/* limit received data to max. left */
		if (burstcnt > (count - size))
			burstcnt = count - size;

		rc = iic_tpm_read(TPM_DATA_FIFO(chip->vendor.locality),
				  &(buf[size]),
				  burstcnt);
		if (rc == 0)
			size += burstcnt;
	}
	return size;
}

static int tpm_tis_i2c_recv(struct tpm_chip *chip, uint8_t *buf, size_t count)
{
	int size = 0;
	uint32_t expected;
	int status;

	if (count < TPM_HEADER_SIZE) {
		size = -1;
		goto out;
	}

	/* read first 10 bytes, including tag, paramsize, and result */
	size = recv_data(chip, buf, TPM_HEADER_SIZE);
	if (size < TPM_HEADER_SIZE) {
		printk(BIOS_DEBUG, "%s: Unable to read header\n", __func__);
		goto out;
	}

	memcpy(&expected, buf + TPM_RSP_SIZE_BYTE, sizeof(expected));
	expected = be32_to_cpu(expected);
	if ((size_t)expected > count) {
		size = -1;
		goto out;
	}

	size += recv_data(chip, &buf[TPM_HEADER_SIZE],
				expected - TPM_HEADER_SIZE);
	if (size < expected) {
		printk(BIOS_DEBUG, "%s: Unable to read remainder of result\n", __func__);
		size = -1;
		goto out;
	}

	wait_for_stat(chip, TPM_STS_VALID, &status);
	if (status & TPM_STS_DATA_AVAIL) {	/* retry? */
		printk(BIOS_DEBUG, "%s: Error left over data\n", __func__);
		size = -1;
		goto out;
	}

out:
	tpm_tis_i2c_ready(chip);

	return size;
}

static int tpm_tis_i2c_send(struct tpm_chip *chip, uint8_t *buf, size_t len)
{
	int status;
	size_t count = 0;
	uint8_t sts = TPM_STS_GO;

	if (len > TPM_BUFSIZE)
		return -1; /* command is too long for our TPM, sorry */

	status = tpm_tis_i2c_status(chip);
	if ((status & TPM_STS_COMMAND_READY) == 0) {
		tpm_tis_i2c_ready(chip);
		if (wait_for_stat(chip, TPM_STS_COMMAND_READY, &status) < 0)
			goto out_err;
	}

	while (count < len - 1) {
		ssize_t burstcnt = get_burstcount(chip);

		/* burstcount < 0 = TPM is busy */
		if (burstcnt < 0)
			return burstcnt;

		if (burstcnt > (len-1-count))
			burstcnt = len-1-count;

		if (iic_tpm_write(TPM_DATA_FIFO(chip->vendor.locality),
						&(buf[count]), burstcnt) == 0)
			count += burstcnt;

		wait_for_stat(chip, TPM_STS_VALID, &status);
		if ((status & TPM_STS_DATA_EXPECT) == 0)
			goto out_err;
	}

	/* write last byte */
	iic_tpm_write(TPM_DATA_FIFO(chip->vendor.locality), &(buf[count]), 1);

	wait_for_stat(chip, TPM_STS_VALID, &status);
	if ((status & TPM_STS_DATA_EXPECT) != 0)
		goto out_err;

	/* go and do it */
	iic_tpm_write(TPM_STS(chip->vendor.locality), &sts, 1);

	return len;

out_err:
	tpm_tis_i2c_ready(chip);

	return -1;
}

/* Initialization of I2C TPM */

int tpm_vendor_probe(unsigned int bus, uint32_t addr)
{
	struct stopwatch sw;
	uint8_t buf = 0;
	int ret;
	long sw_run_duration = SLEEP_DURATION_PROBE_MS;

	tpm_dev.chip_type = UNKNOWN;
	tpm_dev.bus = bus;
	tpm_dev.addr = addr;
	tpm_dev.sleep_short = SLEEP_DURATION;
	tpm_dev.sleep_long = SLEEP_DURATION_LONG;

	/*
	 * Probe TPM. Check if the TPM_ACCESS register's ValidSts bit is set(1)
	 * If the bit remains clear(0) then claim that init has failed.
	 */
	stopwatch_init_msecs_expire(&sw, sw_run_duration);
	do {
		ret = iic_tpm_read(TPM_ACCESS(0), &buf, 1);
		if (!ret && (buf & TPM_STS_VALID)) {
			sw_run_duration = stopwatch_duration_msecs(&sw);
			break;
		}
		udelay(SLEEP_DURATION);
	} while (!stopwatch_expired(&sw));

	printk(BIOS_INFO,
	       "%s: ValidSts bit %s(%d) in TPM_ACCESS register after %ld ms\n",
	       __func__, (buf & TPM_STS_VALID) ? "set" : "clear",
	       (buf & TPM_STS_VALID) >> 7, sw_run_duration);

	/*
	 * Claim failure if the ValidSts (bit 7) is clear.
	 */
	if (!(buf & TPM_STS_VALID))
		return -1;

	return 0;
}

int tpm_vendor_init(struct tpm_chip *chip, unsigned int bus, uint32_t dev_addr)
{
	uint32_t vendor;

	if (dev_addr == 0) {
		printk(BIOS_ERR, "%s: missing device address\n", __func__);
		return -1;
	}

	tpm_dev.chip_type = UNKNOWN;
	tpm_dev.bus = bus;
	tpm_dev.addr = dev_addr;
	tpm_dev.sleep_short = SLEEP_DURATION;
	tpm_dev.sleep_long = SLEEP_DURATION_LONG;

	memset(&chip->vendor, 0, sizeof(struct tpm_vendor_specific));
	chip->is_open = 1;

	chip->vendor.req_complete_mask = TPM_STS_DATA_AVAIL | TPM_STS_VALID;
	chip->vendor.req_complete_val = TPM_STS_DATA_AVAIL | TPM_STS_VALID;
	chip->vendor.req_canceled = TPM_STS_COMMAND_READY;
	chip->vendor.status = &tpm_tis_i2c_status;
	chip->vendor.recv = &tpm_tis_i2c_recv;
	chip->vendor.send = &tpm_tis_i2c_send;
	chip->vendor.cancel = &tpm_tis_i2c_ready;

	if (request_locality(chip, 0) != 0)
		return -1;

	/* Read four bytes from DID_VID register */
	if (iic_tpm_read(TPM_DID_VID(0), (uint8_t *)&vendor, 4) < 0)
		goto out_err;

	if (vendor == TPM_TIS_I2C_DID_VID_9645) {
		tpm_dev.chip_type = SLB9645;
	} else if (be32_to_cpu(vendor) == TPM_TIS_I2C_DID_VID_9635) {
		tpm_dev.chip_type = SLB9635;
	} else {
		printk(BIOS_DEBUG, "Vendor ID 0x%08x not recognized.\n",
			vendor);
		goto out_err;
	}

	printk(BIOS_DEBUG, "I2C TPM %u:%02x (chip type %s device-id 0x%X)\n",
	       tpm_dev.bus, tpm_dev.addr,
	       chip_name[tpm_dev.chip_type], vendor >> 16);

	/*
	 * A timeout query to TPM can be placed here.
	 * Standard timeout values are used so far
	 */

	return 0;

out_err:
	release_locality(chip, 0, 1);
	return -1;
}

void tpm_vendor_cleanup(struct tpm_chip *chip)
{
	release_locality(chip, chip->vendor.locality, 1);
}
