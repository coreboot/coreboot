/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * This is a driver for a SPI interfaced TPM2 device.
 *
 * It assumes that the required SPI interface has been initialized before the
 * driver is started. A 'sruct spi_slave' pointer passed at initialization is
 * used to direct traffic to the correct SPI interface. This dirver does not
 * provide a way to instantiate multiple TPM devices. Also, to keep things
 * simple, the driver unconditionally uses of TPM locality zero.
 *
 * References to documentation are based on the TCG issued "TPM Profile (PTP)
 * Specification Revision 00.43".
 */

#include <arch/early_variables.h>
#include <assert.h>
#include <compiler.h>
#include <commonlib/endian.h>
#include <console/console.h>
#include <delay.h>
#include <endian.h>
#include <string.h>
#include <timer.h>
#include <security/tpm/tis.h>

#include "tpm.h"

#define TPM_LOCALITY_0_SPI_BASE 0x00d40000

/* Assorted TPM2 registers for interface type FIFO. */
#define TPM_ACCESS_REG    (TPM_LOCALITY_0_SPI_BASE + 0)
#define TPM_STS_REG       (TPM_LOCALITY_0_SPI_BASE + 0x18)
#define TPM_DATA_FIFO_REG (TPM_LOCALITY_0_SPI_BASE + 0x24)
#define TPM_DID_VID_REG   (TPM_LOCALITY_0_SPI_BASE + 0xf00)
#define TPM_RID_REG       (TPM_LOCALITY_0_SPI_BASE + 0xf04)
#define TPM_FW_VER	  (TPM_LOCALITY_0_SPI_BASE + 0xf90)

#define CR50_TIMEOUT_INIT_MS 30000 /* Very long timeout for TPM init */

/* SPI slave structure for TPM device. */
static struct spi_slave g_spi_slave CAR_GLOBAL;

/* Cached TPM device identification. */
static struct tpm2_info g_tpm_info CAR_GLOBAL;

/*
 * TODO(vbendeb): make CONFIG_DEBUG_TPM an int to allow different level of
 * debug traces. Right now it is either 0 or 1.
 */
static const int debug_level_ = CONFIG_DEBUG_TPM;

/*
 * SPI frame header for TPM transactions is 4 bytes in size, it is described
 * in section "6.4.6 Spi Bit Protocol".
 */
typedef struct {
	unsigned char body[4];
} spi_frame_header;

void tpm2_get_info(struct tpm2_info *info)
{
	*info = car_get_var(g_tpm_info);
}

__weak int tis_plat_irq_status(void)
{
	static int warning_displayed CAR_GLOBAL;

	if (!car_get_var(warning_displayed)) {
		printk(BIOS_WARNING, "WARNING: tis_plat_irq_status() not implemented, wasting 10ms to wait on Cr50!\n");
		car_set_var(warning_displayed, 1);
	}
	mdelay(10);

	return 1;
}

/*
 * TPM may trigger a irq after finish processing previous transfer.
 * Waiting for this irq to sync tpm status.
 *
 * Returns 1 on success, 0 on failure (timeout).
 */
static int tpm_sync(void)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, 10);
	while (!tis_plat_irq_status()) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "Timeout wait for tpm irq!\n");
			return 0;
		}
	}
	return 1;
}

/*
 * Each TPM2 SPI transaction starts the same: CS is asserted, the 4 byte
 * header is sent to the TPM, the master waits til TPM is ready to continue.
 *
 * Returns 1 on success, 0 on failure (TPM SPI flow control timeout.)
 */
static int start_transaction(int read_write, size_t bytes, unsigned addr)
{
	spi_frame_header header;
	uint8_t byte;
	int i;
	struct stopwatch sw;
	static int tpm_sync_needed CAR_GLOBAL;
	static struct stopwatch wake_up_sw CAR_GLOBAL;
	struct spi_slave *spi_slave = car_get_var_ptr(&g_spi_slave);
	/*
	 * First Cr50 access in each coreboot stage where TPM is used will be
	 * prepended by a wake up pulse on the CS line.
	 */
	int wakeup_needed = 1;

	/* Wait for tpm to finish previous transaction if needed */
	if (car_get_var(tpm_sync_needed)) {
		tpm_sync();
		/*
		 * During the first invocation of this function on each stage
		 * this if () clause code does not run (as tpm_sync_needed
		 * value is zero), during all following invocations the
		 * stopwatch below is guaranteed to be started.
		 */
		if (!stopwatch_expired(car_get_var_ptr(&wake_up_sw)))
			wakeup_needed = 0;
	} else {
		car_set_var(tpm_sync_needed, 1);
	}

	if (wakeup_needed) {
		/* Just in case Cr50 is asleep. */
		spi_claim_bus(spi_slave);
		udelay(1);
		spi_release_bus(spi_slave);
		udelay(100);
	}

	/*
	 * The Cr50 on H1 does not go to sleep for 1 second after any
	 * SPI slave activity, let's be conservative and limit the
	 * window to 900 ms.
	 */
	stopwatch_init_msecs_expire(car_get_var_ptr(&wake_up_sw), 900);

	/*
	 * The first byte of the frame header encodes the transaction type
	 * (read or write) and transfer size (set to lentgh - 1), limited to
	 * 64 bytes.
	 */
	header.body[0] = (read_write ? 0x80 : 0) | 0x40 | (bytes - 1);

	/* The rest of the frame header is the TPM register address. */
	for (i = 0; i < 3; i++)
		header.body[i + 1] = (addr >> (8 * (2 - i))) & 0xff;

	/* CS assert wakes up the slave. */
	spi_claim_bus(spi_slave);

	/*
	 * The TCG TPM over SPI specification introduces the notion of SPI
	 * flow control (Section "6.4.5 Flow Control").
	 *
	 * Again, the slave (TPM device) expects each transaction to start
	 * with a 4 byte header trasmitted by master. The header indicates if
	 * the master needs to read or write a register, and the register
	 * address.
	 *
	 * If the slave needs to stall the transaction (for instance it is not
	 * ready to send the register value to the master), it sets the MOSI
	 * line to 0 during the last clock of the 4 byte header. In this case
	 * the master is supposed to start polling the SPI bus, one byte at
	 * time, until the last bit in the received byte (transferred during
	 * the last clock of the byte) is set to 1.
	 *
	 * Due to some SPI controllers' shortcomings (Rockchip comes to
	 * mind...) we trasmit the 4 byte header without checking the byte
	 * transmitted by the TPM during the transaction's last byte.
	 *
	 * We know that cr50 is guaranteed to set the flow control bit to 0
	 * during the header transfer, but real TPM2 might be fast enough not
	 * to require to stall the master, this would present an issue.
	 * crosbug.com/p/52132 has been opened to track this.
	 */
	spi_xfer(spi_slave, header.body, sizeof(header.body), NULL, 0);

	/*
	 * Now poll the bus until TPM removes the stall bit. Give it up to 100
	 * ms to sort it out - it could be saving stuff in nvram at some
	 * point.
	 */
	stopwatch_init_msecs_expire(&sw, 100);
	do {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "TPM flow control failure\n");
			spi_release_bus(spi_slave);
			return 0;
		}
		spi_xfer(spi_slave, NULL, 0, &byte, 1);
	} while (!(byte & 1));
	return 1;
}

/*
 * Print out the contents of a buffer, if debug is enabled. Skip registers
 * other than FIFO, unless debug_level_ is 2.
 */
static void trace_dump(const char *prefix, uint32_t reg,
		       size_t bytes, const uint8_t *buffer,
		       int force)
{
	static char prev_prefix CAR_GLOBAL;
	static unsigned prev_reg CAR_GLOBAL;
	static int current_char CAR_GLOBAL;
	const int BYTES_PER_LINE = 32;
	int *current_char_ptr = car_get_var_ptr(&current_char);

	if (!force) {
		if (!debug_level_)
			return;

		if ((debug_level_ < 2) && (reg != TPM_DATA_FIFO_REG))
			return;
	}

	/*
	 * Do not print register address again if the last dump print was for
	 * that register.
	 */
	if ((car_get_var(prev_prefix) != *prefix) ||
		(car_get_var(prev_reg) != reg)) {
		car_set_var(prev_prefix, *prefix);
		car_set_var(prev_reg, reg);
		printk(BIOS_DEBUG, "\n%s %2.2x:", prefix, reg);
		*current_char_ptr = 0;
	}

	if ((reg != TPM_DATA_FIFO_REG) && (bytes == 4)) {
		/*
		 * This must be a regular register address, print the 32 bit
		 * value.
		 */
		printk(BIOS_DEBUG, " %8.8x", *(const uint32_t *)buffer);
	} else {
		int i;

		/*
		 * Data read from or written to FIFO or not in 4 byte
		 * quantiites is printed byte at a time.
		 */
		for (i = 0; i < bytes; i++) {
			if (*current_char_ptr &&
				!(*current_char_ptr % BYTES_PER_LINE)) {
				printk(BIOS_DEBUG, "\n     ");
				*current_char_ptr = 0;
			}
			(*current_char_ptr)++;
			printk(BIOS_DEBUG, " %2.2x", buffer[i]);
		}
	}
}

/*
 * Once transaction is initiated and the TPM indicated that it is ready to go,
 * write the actual bytes to the register.
 */
static void write_bytes(const void *buffer, size_t bytes)
{
	struct spi_slave *spi_slave = car_get_var_ptr(&g_spi_slave);
	spi_xfer(spi_slave, buffer, bytes, NULL, 0);
}

/*
 * Once transaction is initiated and the TPM indicated that it is ready to go,
 * read the actual bytes from the register.
 */
static void read_bytes(void *buffer, size_t bytes)
{
	struct spi_slave *spi_slave = car_get_var_ptr(&g_spi_slave);
	spi_xfer(spi_slave, NULL, 0, buffer, bytes);
}

/*
 * To write a register, start transaction, transfer data to the TPM, deassert
 * CS when done.
 *
 * Returns one to indicate success, zero to indicate failure.
 */
static int tpm2_write_reg(unsigned reg_number, const void *buffer, size_t bytes)
{
	struct spi_slave *spi_slave = car_get_var_ptr(&g_spi_slave);
	trace_dump("W", reg_number, bytes, buffer, 0);
	if (!start_transaction(false, bytes, reg_number))
		return 0;
	write_bytes(buffer, bytes);
	spi_release_bus(spi_slave);
	return 1;
}

/*
 * To read a register, start transaction, transfer data from the TPM, deassert
 * CS when done.
 *
 * Returns one to indicate success, zero to indicate failure. In case of
 * failure zero out the user buffer.
 */
static int tpm2_read_reg(unsigned reg_number, void *buffer, size_t bytes)
{
	struct spi_slave *spi_slave = car_get_var_ptr(&g_spi_slave);
	if (!start_transaction(true, bytes, reg_number)) {
		memset(buffer, 0, bytes);
		return 0;
	}
	read_bytes(buffer, bytes);
	spi_release_bus(spi_slave);
	trace_dump("R", reg_number, bytes, buffer, 0);
	return 1;
}

/*
 * Status register is accessed often, wrap reading and writing it into
 * dedicated functions.
 */
static int read_tpm_sts(uint32_t *status)
{
	return tpm2_read_reg(TPM_STS_REG, status, sizeof(*status));
}

static int write_tpm_sts(uint32_t status)
{
	return tpm2_write_reg(TPM_STS_REG, &status, sizeof(status));
}

/*
 * The TPM may limit the transaction bytes count (burst count) below the 64
 * bytes max. The current value is available as a field of the status
 * register.
 */
static uint32_t get_burst_count(void)
{
	uint32_t status;

	read_tpm_sts(&status);
	return (status & TPM_STS_BURST_COUNT_MASK) >> TPM_STS_BURST_COUNT_SHIFT;
}

static uint8_t tpm2_read_access_reg(void)
{
	uint8_t access;
	tpm2_read_reg(TPM_ACCESS_REG, &access, sizeof(access));
	/* We do not care about access establishment bit state. Ignore it. */
	return access & ~TPM_ACCESS_ESTABLISHMENT;
}

static void tpm2_write_access_reg(uint8_t cmd)
{
	/* Writes to access register can set only 1 bit at a time. */
	assert (!(cmd & (cmd - 1)));

	tpm2_write_reg(TPM_ACCESS_REG, &cmd, sizeof(cmd));
}

static int tpm2_claim_locality(void)
{
	uint8_t access;
	struct stopwatch sw;

	/*
	 * Locality is released by TPM reset.
	 *
	 * If locality is taken at this point, this could be due to the fact
	 * that the TPM is performing a long operation and has not processed
	 * reset request yet. We'll wait up to CR50_TIMEOUT_INIT_MS and see if
	 * it releases locality when reset is processed.
	 */
	stopwatch_init_msecs_expire(&sw, CR50_TIMEOUT_INIT_MS);
	do {
		access = tpm2_read_access_reg();
		if (access & TPM_ACCESS_ACTIVE_LOCALITY) {
			/*
			 * Don't bombard the chip with traffic, let it keep
			 * processing the command.
			 */
			mdelay(2);
			continue;
		}

		/*
		 * Ok, the locality is free, TPM must be reset, let's claim
		 * it.
		 */

		tpm2_write_access_reg(TPM_ACCESS_REQUEST_USE);
		access = tpm2_read_access_reg();
		if (access != (TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY)) {
			break;
		}

		printk(BIOS_INFO, "TPM ready after %ld ms\n",
		       stopwatch_duration_msecs(&sw));

		return 1;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR,
	       "Failed to claim locality 0 after %ld ms, status: %#x\n",
	       stopwatch_duration_msecs(&sw), access);

	return 0;
}

/* Device/vendor ID values of the TPM devices this driver supports. */
static const uint32_t supported_did_vids[] = {
	0x00281ae0  /* H1 based Cr50 security chip. */
};

int tpm2_init(struct spi_slave *spi_if)
{
	uint32_t did_vid, status;
	uint8_t cmd;
	int retries;
	struct tpm2_info *tpm_info = car_get_var_ptr(&g_tpm_info);
	struct spi_slave *spi_slave = car_get_var_ptr(&g_spi_slave);

	memcpy(spi_slave, spi_if, sizeof(*spi_if));

	/* clear any pending irqs */
	tis_plat_irq_status();

	/*
	 * 150 ms should be enough to synchronize with the TPM even under the
	 * worst nested reset request conditions. In vast majority of cases
	 * there would be no wait at all.
	 */
	printk(BIOS_INFO, "Probing TPM: ");
	for (retries = 15; retries > 0; retries--) {
		int i;

		/* In case of falure to read div_vid is set to zero. */
		tpm2_read_reg(TPM_DID_VID_REG, &did_vid, sizeof(did_vid));

		for (i = 0; i < ARRAY_SIZE(supported_did_vids); i++)
			if (did_vid == supported_did_vids[i])
				break; /* Tpm is up and ready. */

		if (i < ARRAY_SIZE(supported_did_vids))
			break;

		/* TPM might be resetting, let's retry in a bit. */
		mdelay(10);
		printk(BIOS_INFO, ".");
	}

	if (!retries) {
		printk(BIOS_ERR, "\n%s: Failed to connect to the TPM\n",
		       __func__);
		return -1;
	}

	printk(BIOS_INFO, " done!\n");

	if (ENV_VERSTAGE || ENV_BOOTBLOCK)
		/*
		 * Claim locality 0, do it only during the first
		 * initialization after reset.
		 */
		if (!tpm2_claim_locality())
			return -1;

	read_tpm_sts(&status);
	if ((status & TPM_STS_FAMILY_MASK) != TPM_STS_FAMILY_TPM_2_0) {
		printk(BIOS_ERR, "unexpected TPM family value, status: %#x\n",
		       status);
		return -1;
	}

	/*
	 * Locality claimed, read the revision value and set up the tpm_info
	 * structure.
	 */
	tpm2_read_reg(TPM_RID_REG, &cmd, sizeof(cmd));
	tpm_info->vendor_id = did_vid & 0xffff;
	tpm_info->device_id = did_vid >> 16;
	tpm_info->revision = cmd;

	printk(BIOS_INFO, "Connected to device vid:did:rid of %4.4x:%4.4x:%2.2x\n",
	       tpm_info->vendor_id, tpm_info->device_id, tpm_info->revision);

	/* Let's report device FW version if available. */
	if (tpm_info->vendor_id == 0x1ae0) {
		int chunk_count = 0;
		size_t chunk_size;
		/*
		 * let's read 50 bytes at a time; leave room for the trailing
		 * zero.
		 */
		char vstr[51];

		chunk_size = sizeof(vstr) - 1;

		printk(BIOS_INFO, "Firmware version: ");

		/*
		 * Does not really matter what's written, this just makes sure
		 * the version is reported from the beginning.
		 */
		tpm2_write_reg(TPM_FW_VER, &chunk_size, 1);

		/* Print it out in sizeof(vstr) - 1 byte chunks. */
		vstr[chunk_size] = 0;
		do {
			tpm2_read_reg(TPM_FW_VER, vstr, chunk_size);
			printk(BIOS_INFO, "%s", vstr);

			/*
			 * While string is not over, and is no longer than 300
			 * characters.
			 */
		} while (vstr[chunk_size - 1] &&
			 (chunk_count++ < (300 / chunk_size)));

		printk(BIOS_INFO, "\n");
	}
	return 0;
}

/*
 * This is in seconds, certain TPM commands, like key generation, can take
 * long time to complete.
 *
 * Returns one to indicate success, zero (not yet implemented) to indicate
 * failure.
 */
#define MAX_STATUS_TIMEOUT 120
static int wait_for_status(uint32_t status_mask, uint32_t status_expected)
{
	uint32_t status;
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, MAX_STATUS_TIMEOUT * 1000 * 1000);
	do {
		udelay(1000);
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "failed to get expected status %x\n",
			       status_expected);
			return false;
		}
		read_tpm_sts(&status);
	} while ((status & status_mask) != status_expected);

	return 1;
}

enum fifo_transfer_direction {
	fifo_transmit = 0,
	fifo_receive = 1
};

/* Union allows to avoid casting away 'const' on transmit buffers. */
union fifo_transfer_buffer {
	uint8_t *rx_buffer;
	const uint8_t *tx_buffer;
};

/*
 * Transfer requested number of bytes to or from TPM FIFO, accounting for the
 * current burst count value.
 */
static void fifo_transfer(size_t transfer_size,
			  union fifo_transfer_buffer buffer,
			  enum fifo_transfer_direction direction)
{
	size_t transaction_size;
	size_t burst_count;
	size_t handled_so_far = 0;

	do {
		do {
			/* Could be zero when TPM is busy. */
			burst_count = get_burst_count();
		} while (!burst_count);

		transaction_size = transfer_size - handled_so_far;
		transaction_size = MIN(transaction_size, burst_count);

		/*
		 * The SPI frame header does not allow to pass more than 64
		 * bytes.
		 */
		transaction_size = MIN(transaction_size, 64);

		if (direction == fifo_receive)
			tpm2_read_reg(TPM_DATA_FIFO_REG,
				      buffer.rx_buffer + handled_so_far,
				      transaction_size);
		else
			tpm2_write_reg(TPM_DATA_FIFO_REG,
				       buffer.tx_buffer + handled_so_far,
				       transaction_size);

		handled_so_far += transaction_size;

	} while (handled_so_far != transfer_size);
}

size_t tpm2_process_command(const void *tpm2_command, size_t command_size,
			    void *tpm2_response, size_t max_response)
{
	uint32_t status;
	uint32_t expected_status_bits;
	size_t payload_size;
	size_t bytes_to_go;
	const uint8_t *cmd_body = tpm2_command;
	uint8_t *rsp_body = tpm2_response;
	union fifo_transfer_buffer fifo_buffer;
	const int HEADER_SIZE = 6;
	struct tpm2_info *tpm_info = car_get_var_ptr(&g_tpm_info);

	/* Do not try using an uninitialized TPM. */
	if (!tpm_info->vendor_id)
		return 0;

	/* Skip the two byte tag, read the size field. */
	payload_size = read_be32(cmd_body + 2);

	/* Sanity check. */
	if (payload_size != command_size) {
		printk(BIOS_ERR,
		       "Command size mismatch: encoded %zd != requested %zd\n",
		       payload_size, command_size);
		trace_dump("W", TPM_DATA_FIFO_REG, command_size, cmd_body, 1);
		printk(BIOS_DEBUG, "\n");
		return 0;
	}

	/* Let the TPM know that the command is coming. */
	write_tpm_sts(TPM_STS_COMMAND_READY);

	/*
	 * Tpm commands and responses written to and read from the FIFO
	 * register (0x24) are datagrams of variable size, prepended by a 6
	 * byte header.
	 *
	 * The specification description of the state machine is a bit vague,
	 * but from experience it looks like there is no need to wait for the
	 * sts.expect bit to be set, at least with the 9670 and cr50 devices.
	 * Just write the command into FIFO, making sure not to exceed the
	 * burst count or the maximum PDU size, whatever is smaller.
	 */
	fifo_buffer.tx_buffer = cmd_body;
	fifo_transfer(command_size, fifo_buffer, fifo_transmit);

	/* Now tell the TPM it can start processing the command. */
	write_tpm_sts(TPM_STS_GO);

	/* Now wait for it to report that the response is ready. */
	expected_status_bits = TPM_STS_VALID | TPM_STS_DATA_AVAIL;
	if (!wait_for_status(expected_status_bits, expected_status_bits)) {
		/*
		 * If timed out, which should never happen, let's at least
		 * print out the offending command.
		 */
		trace_dump("W", TPM_DATA_FIFO_REG, command_size, cmd_body, 1);
		printk(BIOS_DEBUG, "\n");
		return 0;
	}

	/*
	 * The response is ready, let's read it. First we read the FIFO
	 * payload header, to see how much data to expect. The response header
	 * size is fixed to six bytes, the total payload size is stored in
	 * network order in the last four bytes.
	 */
	tpm2_read_reg(TPM_DATA_FIFO_REG, rsp_body, HEADER_SIZE);

	/* Find out the total payload size, skipping the two byte tag. */
	payload_size = read_be32(rsp_body + 2);

	if (payload_size > max_response) {
		/*
		 * TODO(vbendeb): at least drain the FIFO here or somehow let
		 * the TPM know that the response can be dropped.
		 */
		printk(BIOS_ERR, " tpm response too long (%zd bytes)",
		       payload_size);
		return 0;
	}

	/*
	 * Now let's read all but the last byte in the FIFO to make sure the
	 * status register is showing correct flow control bits: 'more data'
	 * until the last byte and then 'no more data' once the last byte is
	 * read.
	 */
	bytes_to_go = payload_size - 1 - HEADER_SIZE;
	fifo_buffer.rx_buffer = rsp_body + HEADER_SIZE;
	fifo_transfer(bytes_to_go, fifo_buffer, fifo_receive);

	/* Verify that there is still data to read. */
	read_tpm_sts(&status);
	if ((status & expected_status_bits) != expected_status_bits) {
		printk(BIOS_ERR, "unexpected intermediate status %#x\n",
		       status);
		return 0;
	}

	/* Read the last byte of the PDU. */
	tpm2_read_reg(TPM_DATA_FIFO_REG, rsp_body + payload_size - 1, 1);

	/* Terminate the dump, if enabled. */
	if (debug_level_)
		printk(BIOS_DEBUG, "\n");

	/* Verify that 'data available' is not asseretd any more. */
	read_tpm_sts(&status);
	if ((status & expected_status_bits) != TPM_STS_VALID) {
		printk(BIOS_ERR, "unexpected final status %#x\n", status);
		return 0;
	}

	/* Move the TPM back to idle state. */
	write_tpm_sts(TPM_STS_COMMAND_READY);

	return payload_size;
}
