/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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
 * The code in this file has been heavily based on the article "Writing a TPM
 * Device Driver" published on http://ptgmedia.pearsoncmg.com and the
 * submission by Stefan Berger on Qemu-devel mailing list.
 *
 * One principal difference is that in the simplest config the other than 0
 * TPM localities do not get mapped by some devices (for instance, by
 * Infineon slb9635), so this driver provides access to locality 0 only.
 */

#include <stdlib.h>
#include <string.h>
#include <delay.h>
#include <arch/io.h>
#include <arch/byteorder.h>
#include <console/console.h>
#include <pc80/tpm.h>
#include <arch/early_variables.h>

#define PREFIX "lpc_tpm: "

/* coreboot wrapper for TPM driver (start) */
#define	TPM_DEBUG(fmt, args...)		\
	if (CONFIG_DEBUG_TPM) {		\
		printk(BIOS_DEBUG, PREFIX);		\
		printk(BIOS_DEBUG, fmt , ##args);	\
	}
#define TPM_DEBUG_IO_READ(reg_, val_) \
	TPM_DEBUG("Read reg 0x%x returns 0x%x\n", (reg_), (val_))
#define TPM_DEBUG_IO_WRITE(reg_, val_) \
	TPM_DEBUG("Write reg 0x%x with 0x%x\n", (reg_), (val_))
#define printf(x...) printk(BIOS_ERR, x)

#define min(a,b) MIN(a,b)
#define max(a,b) MAX(a,b)
#define readb(_a) (*(volatile unsigned char *) (_a))
#define writeb(_v, _a) (*(volatile unsigned char *) (_a) = (_v))
#define readl(_a) (*(volatile unsigned long *) (_a))
#define writel(_v, _a) (*(volatile unsigned long *) (_a) = (_v))
/* coreboot wrapper for TPM driver (end) */

#ifndef CONFIG_TPM_TIS_BASE_ADDRESS
/* Base TPM address standard for x86 systems */
#define CONFIG_TPM_TIS_BASE_ADDRESS        0xfed40000
#endif

/* the macro accepts the locality value, but only locality 0 is operational */
#define TIS_REG(LOCALITY, REG) \
    (void *)(CONFIG_TPM_TIS_BASE_ADDRESS + (LOCALITY << 12) + REG)

/* hardware registers' offsets */
#define TIS_REG_ACCESS                 0x0
#define TIS_REG_INT_ENABLE             0x8
#define TIS_REG_INT_VECTOR             0xc
#define TIS_REG_INT_STATUS             0x10
#define TIS_REG_INTF_CAPABILITY        0x14
#define TIS_REG_STS                    0x18
#define TIS_REG_BURST_COUNT            0x19
#define TIS_REG_DATA_FIFO              0x24
#define TIS_REG_DID_VID                0xf00
#define TIS_REG_RID                    0xf04

/* Some registers' bit field definitions */
#define TIS_STS_VALID                  (1 << 7) /* 0x80 */
#define TIS_STS_COMMAND_READY          (1 << 6) /* 0x40 */
#define TIS_STS_TPM_GO                 (1 << 5) /* 0x20 */
#define TIS_STS_DATA_AVAILABLE         (1 << 4) /* 0x10 */
#define TIS_STS_EXPECT                 (1 << 3) /* 0x08 */
#define TIS_STS_RESPONSE_RETRY         (1 << 1) /* 0x02 */

#define TIS_ACCESS_TPM_REG_VALID_STS   (1 << 7) /* 0x80 */
#define TIS_ACCESS_ACTIVE_LOCALITY     (1 << 5) /* 0x20 */
#define TIS_ACCESS_BEEN_SEIZED         (1 << 4) /* 0x10 */
#define TIS_ACCESS_SEIZE               (1 << 3) /* 0x08 */
#define TIS_ACCESS_PENDING_REQUEST     (1 << 2) /* 0x04 */
#define TIS_ACCESS_REQUEST_USE         (1 << 1) /* 0x02 */
#define TIS_ACCESS_TPM_ESTABLISHMENT   (1 << 0) /* 0x01 */

/*
 * Error value returned if a tpm register does not enter the expected state
 * after continuous polling. No actual TPM register reading ever returns ~0,
 * so this value is a safe error indication to be mixed with possible status
 * register values.
 */
#define TPM_TIMEOUT_ERR			(~0)

/* Error value returned on various TPM driver errors */
#define TPM_DRIVER_ERR		(~0)

 /* 1 second is plenty for anything TPM does.*/
#define MAX_DELAY_US	(1000 * 1000)

/*
 * Structures defined below allow creating descriptions of TPM vendor/device
 * ID information for run time discovery. The only device the system knows
 * about at this time is Infineon slb9635
 */
struct device_name {
	u16 dev_id;
	const char * const dev_name;
};

struct vendor_name {
	u16 vendor_id;
	const char * vendor_name;
	const struct device_name* dev_names;
};

static const struct device_name atmel_devices[] = {
	{0x3204, "AT97SC3204"},
	{0xffff}
};

static const struct device_name infineon_devices[] = {
	{0x000b, "SLB9635 TT 1.2"},
	{0xffff}
};

static const struct device_name nuvoton_devices[] = {
	{0x00fe, "NPCT420AA V2"},
	{0xffff}
};

static const struct device_name stmicro_devices[] = {
	{0x0000, "ST33ZP24" },
	{0xffff}
};

static const struct vendor_name vendor_names[] = {
	{0x1114, "Atmel", atmel_devices},
	{0x15d1, "Infineon", infineon_devices},
	{0x1050, "Nuvoton", nuvoton_devices},
	{0x104a, "ST Microelectronics", stmicro_devices},
};

/*
 * Cached vendor/device ID pair to indicate that the device has been already
 * discovered
 */
static u32 vendor_dev_id CAR_GLOBAL;

static inline u8 tpm_read_status(int locality)
{
	u8 value = readb(TIS_REG(locality, TIS_REG_STS));
	TPM_DEBUG_IO_READ(TIS_REG_STS, value);
	return value;
}

static inline void tpm_write_status(u8 sts, int locality)
{
	TPM_DEBUG_IO_WRITE(TIS_REG_STS, sts);
	writeb(sts, TIS_REG(locality, TIS_REG_STS));
}

static inline u8 tpm_read_data(int locality)
{
	u8 value = readb(TIS_REG(locality, TIS_REG_DATA_FIFO));
	TPM_DEBUG_IO_READ(TIS_REG_DATA_FIFO, value);
	return value;
}

static inline void tpm_write_data(u8 data, int locality)
{
	TPM_DEBUG_IO_WRITE(TIS_REG_STS, data);
	writeb(data, TIS_REG(locality, TIS_REG_DATA_FIFO));
}

static inline u16 tpm_read_burst_count(int locality)
{
	u16 count;
	count = readb(TIS_REG(locality, TIS_REG_BURST_COUNT));
	count |= readb(TIS_REG(locality, TIS_REG_BURST_COUNT + 1)) << 8;
	TPM_DEBUG_IO_READ(TIS_REG_BURST_COUNT, count);
	return count;
}

static inline u8 tpm_read_access(int locality)
{
	u8 value = readb(TIS_REG(locality, TIS_REG_ACCESS));
	TPM_DEBUG_IO_READ(TIS_REG_ACCESS, value);
	return value;
}

static inline void tpm_write_access(u8 data, int locality)
{
	TPM_DEBUG_IO_WRITE(TIS_REG_ACCESS, data);
	writeb(data, TIS_REG(locality, TIS_REG_ACCESS));
}

static inline u32 tpm_read_did_vid(int locality)
{
	u32 value = readl(TIS_REG(locality, TIS_REG_DID_VID));
	TPM_DEBUG_IO_READ(TIS_REG_DID_VID, value);
	return value;
}

/*
 * tis_wait_sts()
 *
 * Wait for at least a second for a status to change its state to match the
 * expected state. Normally the transition happens within microseconds.
 *
 * @locality - locality
 * @mask - bitmask for the bitfield(s) to watch
 * @expected - value the field(s) are supposed to be set to
 *
 * Returns 0 on success or TPM_TIMEOUT_ERR on timeout.
 */
static int tis_wait_sts(int locality, u8 mask, u8 expected)
{
	u32 time_us = MAX_DELAY_US;
	while (time_us > 0) {
		u8 value = tpm_read_status(locality);
		if ((value & mask) == expected)
			return 0;
		udelay(1); /* 1 us */
		time_us--;
	}
	return TPM_TIMEOUT_ERR;
}

static inline int tis_wait_ready(int locality)
{
	return tis_wait_sts(locality, TIS_STS_COMMAND_READY,
	                    TIS_STS_COMMAND_READY);
}

static inline int tis_wait_valid(int locality)
{
	return tis_wait_sts(locality, TIS_STS_VALID, TIS_STS_VALID);
}

static inline int tis_wait_valid_data(int locality)
{
	const u8 has_data = TIS_STS_DATA_AVAILABLE | TIS_STS_VALID;
	return tis_wait_sts(locality, has_data, has_data);
}

static inline int tis_has_valid_data(int locality)
{
	const u8 has_data = TIS_STS_DATA_AVAILABLE | TIS_STS_VALID;
	return (tpm_read_status(locality) & has_data) == has_data;
}

static inline int tis_expect_data(int locality)
{
	return !!(tpm_read_status(locality) & TIS_STS_EXPECT);
}

/*
 * tis_wait_access()
 *
 * Wait for at least a second for a access to change its state to match the
 * expected state. Normally the transition happens within microseconds.
 *
 * @locality - locality
 * @mask - bitmask for the bitfield(s) to watch
 * @expected - value the field(s) are supposed to be set to
 *
 * Returns 0 on success or TPM_TIMEOUT_ERR on timeout.
 */
static int tis_wait_access(int locality, u8 mask, u8 expected)
{
	u32 time_us = MAX_DELAY_US;
	while (time_us > 0) {
		u8 value = tpm_read_access(locality);
		if ((value & mask) == expected)
			return 0;
		udelay(1); /* 1 us */
		time_us--;
	}
	return TPM_TIMEOUT_ERR;
}

static inline int tis_wait_dropped_access(int locality)
{
	return tis_wait_access(locality, TIS_ACCESS_ACTIVE_LOCALITY, 0);
}

static inline int tis_wait_received_access(int locality)
{
	return tis_wait_access(locality, TIS_ACCESS_ACTIVE_LOCALITY,
	                       TIS_ACCESS_ACTIVE_LOCALITY);
}

static inline int tis_has_access(int locality)
{
	return !!(tpm_read_access(locality) & TIS_ACCESS_ACTIVE_LOCALITY);
}

static inline void tis_request_access(int locality)
{
	tpm_write_access(TIS_ACCESS_REQUEST_USE, locality);
}

static inline void tis_drop_access(int locality)
{
	tpm_write_access(TIS_ACCESS_ACTIVE_LOCALITY, locality);
}

/*
 * PC Client Specific TPM Interface Specification section 11.2.12:
 *
 *  Software must be prepared to send two writes of a "1" to command ready
 *  field: the first to indicate successful read of all the data, thus
 *  clearing the data from the ReadFIFO and freeing the TPM's resources,
 *  and the second to indicate to the TPM it is about to send a new command.
 *
 * In practice not all TPMs behave the same so it is necessary to be
 * flexible when trying to set command ready.
 *
 * Returns 0 on success if the TPM is ready for transactions.
 * Returns TPM_TIMEOUT_ERR if the command ready bit does not get set.
 */
static int tis_command_ready(u8 locality)
{
	u32 status;

	/* 1st attempt to set command ready */
	tpm_write_status(TIS_STS_COMMAND_READY, locality);

	/* Wait for response */
	status = tpm_read_status(locality);

	/* Check if command ready is set yet */
	if (status & TIS_STS_COMMAND_READY)
		return 0;

	/* 2nd attempt to set command ready */
	tpm_write_status(TIS_STS_COMMAND_READY, locality);

	return tis_wait_ready(locality);
}

/*
 * Probe the TPM device and try determining its manufacturer/device name.
 *
 * Returns 0 on success (the device is found or was found during an earlier
 * invocation) or TPM_DRIVER_ERR if the device is not found.
 */
static u32 tis_probe(void)
{
	const char *device_name = "unknown";
	const char *vendor_name = device_name;
	const struct device_name *dev;
	u32 didvid;
	u16 vid, did;
	int i;

	if (car_get_var(vendor_dev_id))
		return 0;  /* Already probed. */

	didvid = tpm_read_did_vid(0);
	if (!didvid || (didvid == 0xffffffff)) {
		printf("%s: No TPM device found\n", __FUNCTION__);
		return TPM_DRIVER_ERR;
	}

	car_set_var(vendor_dev_id, didvid);

	vid = didvid & 0xffff;
	did = (didvid >> 16) & 0xffff;
	for (i = 0; i < ARRAY_SIZE(vendor_names); i++) {
		int j = 0;
		u16 known_did;
		if (vid == vendor_names[i].vendor_id) {
			vendor_name = vendor_names[i].vendor_name;
		} else {
			continue;
		}
		dev = &vendor_names[i].dev_names[j];
		while ((known_did = dev->dev_id) != 0xffff) {
			if (known_did == did) {
				device_name = dev->dev_name;
				break;
			}
			j++;
		}
		break;
	}
	/* this will have to be converted into debug printout */
	printf("Found TPM %s by %s\n", device_name, vendor_name);
	return 0;
}

/*
 * tis_senddata()
 *
 * send the passed in data to the TPM device.
 *
 * @data - address of the data to send, byte by byte
 * @len - length of the data to send
 *
 * Returns 0 on success, TPM_DRIVER_ERR on error (in case the device does
 * not accept the entire command).
 */
static u32 tis_senddata(const u8 * const data, u32 len)
{
	u32 offset = 0;
	u16 burst = 0;
	u32 max_cycles = 0;
	u8 locality = 0;

	if (tis_wait_ready(locality)) {
		printf("%s:%d - failed to get 'command_ready' status\n",
		       __FILE__, __LINE__);
		return TPM_DRIVER_ERR;
	}
	burst = tpm_read_burst_count(locality);

	while (1) {
		unsigned count;

		/* Wait till the device is ready to accept more data. */
		while (!burst) {
			if (max_cycles++ == MAX_DELAY_US) {
				printf("%s:%d failed to feed %d bytes of %d\n",
				       __FILE__, __LINE__, len - offset, len);
				return TPM_DRIVER_ERR;
			}
			udelay(1);
			burst = tpm_read_burst_count(locality);
		}

		max_cycles = 0;

		/*
		 * Calculate number of bytes the TPM is ready to accept in one
		 * shot.
		 *
		 * We want to send the last byte outside of the loop (hence
		 * the -1 below) to make sure that the 'expected' status bit
		 * changes to zero exactly after the last byte is fed into the
		 * FIFO.
		 */
		count = min(burst, len - offset - 1);
		while (count--)
			tpm_write_data(data[offset++], locality);

		if (tis_wait_valid(locality) || !tis_expect_data(locality)) {
			printf("%s:%d TPM command feed overflow\n",
			       __FILE__, __LINE__);
			return TPM_DRIVER_ERR;
		}

		burst = tpm_read_burst_count(locality);
		if ((offset == (len - 1)) && burst)
			/*
			 * We need to be able to send the last byte to the
			 * device, so burst size must be nonzero before we
			 * break out.
			 */
			break;
	}

	/* Send the last byte. */
	tpm_write_data(data[offset++], locality);

	/*
	 * Verify that TPM does not expect any more data as part of this
	 * command.
	 */
	if (tis_wait_valid(locality) || tis_expect_data(locality)) {
		printf("%s:%d unexpected TPM status 0x%x\n",
		       __FILE__, __LINE__, tpm_read_status(locality));
		return TPM_DRIVER_ERR;
	}

	/* OK, sitting pretty, let's start the command execution. */
	tpm_write_status(TIS_STS_TPM_GO, locality);

	return 0;
}

/*
 * tis_readresponse()
 *
 * read the TPM device response after a command was issued.
 *
 * @buffer - address where to read the response, byte by byte.
 * @len - pointer to the size of buffer
 *
 * On success stores the number of received bytes to len and returns 0. On
 * errors (misformatted TPM data or synchronization problems) returns
 * TPM_DRIVER_ERR.
 */
static u32 tis_readresponse(u8 *buffer, size_t *len)
{
	u16 burst_count;
	u32 offset = 0;
	u8 locality = 0;
	u32 expected_count = *len;
	int max_cycles = 0;

	/* Wait for the TPM to process the command */
	if (tis_wait_valid_data(locality)) {
		printf("%s:%d failed processing command\n", __FILE__, __LINE__);
		return TPM_DRIVER_ERR;
	}

	do {
		while ((burst_count = tpm_read_burst_count(locality)) == 0) {
			if (max_cycles++ == MAX_DELAY_US) {
				printf("%s:%d TPM stuck on read\n",
				       __FILE__, __LINE__);
				return TPM_DRIVER_ERR;
			}
			udelay(1);
		}

		max_cycles = 0;

		while (burst_count-- && (offset < expected_count)) {
			buffer[offset++] = tpm_read_data(locality);
			if (offset == 6) {
				/*
				 * We got the first six bytes of the reply,
				 * let's figure out how many bytes to expect
				 * total - it is stored as a 4 byte number in
				 * network order, starting with offset 2 into
				 * the body of the reply.
				 */
				u32 real_length;
				memcpy(&real_length,
				       buffer + 2,
				       sizeof(real_length));
				expected_count = be32_to_cpu(real_length);

				if ((expected_count < offset) ||
				    (expected_count > *len)) {
					printf("%s:%d bad response size %d\n",
					       __FILE__, __LINE__,
					       expected_count);
					return TPM_DRIVER_ERR;
				}
			}
		}

		/* Wait for the next portion */
		if (tis_wait_valid(locality)) {
			printf("%s:%d failed to read response\n",
			       __FILE__, __LINE__);
			return TPM_DRIVER_ERR;
		}

		if (offset == expected_count)
			break;	/* We got all we need */

	} while (tis_has_valid_data(locality));

	/* * Make sure we indeed read all there was. */
	if (tis_has_valid_data(locality)) {
		printf("%s:%d wrong receive status: %x %d bytes left\n",
		       __FILE__, __LINE__, tpm_read_status(locality),
	               tpm_read_burst_count(locality));
		return TPM_DRIVER_ERR;
	}

	/* Tell the TPM that we are done. */
	if (tis_command_ready(locality) == TPM_TIMEOUT_ERR)
		return TPM_DRIVER_ERR;

	*len = offset;
	return 0;
}

/*
 * tis_init()
 *
 * Initialize the TPM device. Returns 0 on success or TPM_DRIVER_ERR on
 * failure (in case device probing did not succeed).
 */
int tis_init(void)
{
	if (tis_probe())
		return TPM_DRIVER_ERR;
	return 0;
}

/*
 * tis_open()
 *
 * Requests access to locality 0 for the caller. After all commands have been
 * completed the caller is supposed to call tis_close().
 *
 * Returns 0 on success, TPM_DRIVER_ERR on failure.
 */
int tis_open(void)
{
	u8 locality = 0; /* we use locality zero for everything */

	if (tis_close())
		return TPM_DRIVER_ERR;

	/* now request access to locality */
	tis_request_access(locality);

	/* did we get a lock? */
	if (tis_wait_received_access(locality)) {
		printf("%s:%d - failed to lock locality %d\n",
		       __FILE__, __LINE__, locality);
		return TPM_DRIVER_ERR;
	}

	/* Certain TPMs seem to need some delay here or they hang... */
	udelay(10);

	if (tis_command_ready(locality) == TPM_TIMEOUT_ERR)
		return TPM_DRIVER_ERR;

	return 0;
}

/*
 * tis_close()
 *
 * terminate the current session with the TPM by releasing the locked
 * locality. Returns 0 on success of TPM_DRIVER_ERR on failure (in case lock
 * removal did not succeed).
 */
int tis_close(void)
{
	u8 locality = 0;
	if (tis_has_access(locality)) {
		tis_drop_access(locality);
		if (tis_wait_dropped_access(locality)) {
			printf("%s:%d - failed to release locality %d\n",
			       __FILE__, __LINE__, locality);
			return TPM_DRIVER_ERR;
		}
	}
	return 0;
}

/*
 * tis_sendrecv()
 *
 * Send the requested data to the TPM and then try to get its response
 *
 * @sendbuf - buffer of the data to send
 * @send_size size of the data to send
 * @recvbuf - memory to save the response to
 * @recv_len - pointer to the size of the response buffer
 *
 * Returns 0 on success (and places the number of response bytes at recv_len)
 * or TPM_DRIVER_ERR on failure.
 */
int tis_sendrecv(const uint8_t *sendbuf, size_t send_size,
		 uint8_t *recvbuf, size_t *recv_len)
{
	if (tis_senddata(sendbuf, send_size)) {
		printf("%s:%d failed sending data to TPM\n",
		       __FILE__, __LINE__);
		return TPM_DRIVER_ERR;
	}

	return tis_readresponse(recvbuf, recv_len);
}
