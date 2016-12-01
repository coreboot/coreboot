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

#include <commonlib/endian.h>
#include <console/console.h>
#include <delay.h>
#include <endian.h>
#include <string.h>
#include <timer.h>

#include "tpm.h"

#define TPM_LOCALITY_0_SPI_BASE 0x00d40000

/* Assorted TPM2 registers for interface type FIFO. */
#define TPM_ACCESS_REG    (TPM_LOCALITY_0_SPI_BASE + 0)
#define TPM_STS_REG       (TPM_LOCALITY_0_SPI_BASE + 0x18)
#define TPM_DATA_FIFO_REG (TPM_LOCALITY_0_SPI_BASE + 0x24)
#define TPM_DID_VID_REG   (TPM_LOCALITY_0_SPI_BASE + 0xf00)
#define TPM_RID_REG       (TPM_LOCALITY_0_SPI_BASE + 0xf04)
#define TPM_FW_VER	  (TPM_LOCALITY_0_SPI_BASE + 0xf90)

/* SPI Interface descriptor used by the driver. */
struct tpm_spi_if {
	struct spi_slave slave;
	int (*cs_assert)(const struct spi_slave *slave);
	void (*cs_deassert)(const struct spi_slave *slave);
	int  (*xfer)(const struct spi_slave *slave, const void *dout,
		     size_t bytesout, void *din,
		     size_t bytesin);
};

/* Use the common SPI driver wrapper as the interface callbacks. */
static struct tpm_spi_if tpm_if = {
	.cs_assert = spi_claim_bus,
	.cs_deassert = spi_release_bus,
	.xfer = spi_xfer
};

/* Cached TPM device identification. */
struct tpm2_info tpm_info;

/*
 * TODO(vbendeb): make CONFIG_DEBUG_TPM an int to allow different level of
 * debug traces. Right now it is either 0 or 1.
 */
static const int debug_level_ = CONFIG_DEBUG_TPM;

/* Locality management bits (in TPM_ACCESS_REG) */
enum tpm_access_bits {
	tpm_reg_valid_sts = (1 << 7),
	active_locality = (1 << 5),
	request_use = (1 << 1),
	tpm_establishment = (1 << 0),
};

/*
 * Variuous fields of the TPM status register, arguably the most important
 * register when interfacing to a TPM.
 */
enum tpm_sts_bits {
	tpm_family_shift = 26,
	tpm_family_mask = ((1 << 2) - 1),  /* 2 bits wide. */
	tpm_family_tpm2 = 1,
	reset_establishment_bit = (1 << 25),
	command_cancel = (1 << 24),
	burst_count_shift = 8,
	burst_count_mask = ((1 << 16) - 1),  /* 16 bits wide. */
	sts_valid = (1 << 7),
	command_ready = (1 << 6),
	tpm_go = (1 << 5),
	data_avail = (1 << 4),
	expect = (1 << 3),
	self_test_done = (1 << 2),
	response_retry = (1 << 1),
};

/*
 * SPI frame header for TPM transactions is 4 bytes in size, it is described
 * in section "6.4.6 Spi Bit Protocol".
 */
typedef struct {
	unsigned char body[4];
} spi_frame_header;

void tpm2_get_info(struct tpm2_info *info)
{
	*info = tpm_info;
}

/*
 * Each TPM2 SPI transaction starts the same: CS is asserted, the 4 byte
 * header is sent to the TPM, the master waits til TPM is ready to continue.
 */
static void start_transaction(int read_write, size_t bytes, unsigned addr)
{
	spi_frame_header header;
	uint8_t byte;
	int i;

	/*
	 * Give it 10 ms. TODO(vbendeb): remove this once cr50 SPS TPM driver
	 * performance is fixed.
	 */
	mdelay(10);

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
	tpm_if.cs_assert(&tpm_if.slave);

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
	tpm_if.xfer(&tpm_if.slave, header.body, sizeof(header.body), NULL, 0);

	/* Now poll the bus until TPM removes the stall bit. */
	do {
		tpm_if.xfer(&tpm_if.slave, NULL, 0, &byte, 1);
	} while (!(byte & 1));
}

/*
 * Print out the contents of a buffer, if debug is enabled. Skip registers
 * other than FIFO, unless debug_level_ is 2.
 */
static void trace_dump(const char *prefix, uint32_t reg,
		       size_t bytes, const uint8_t *buffer,
		       int force)
{
	static char prev_prefix;
	static unsigned prev_reg;
	static int current_char;
	const int BYTES_PER_LINE = 32;

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
	if ((prev_prefix != *prefix) || (prev_reg != reg)) {
		prev_prefix = *prefix;
		prev_reg = reg;
		printk(BIOS_DEBUG, "\n%s %2.2x:", prefix, reg);
		current_char = 0;
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
			if (current_char && !(current_char % BYTES_PER_LINE)) {
				printk(BIOS_DEBUG, "\n     ");
				current_char = 0;
			}
			current_char++;
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
	tpm_if.xfer(&tpm_if.slave, buffer, bytes, NULL, 0);
}

/*
 * Once transaction is initiated and the TPM indicated that it is ready to go,
 * read the actual bytes from the register.
 */
static void read_bytes(void *buffer, size_t bytes)
{
	tpm_if.xfer(&tpm_if.slave, NULL, 0, buffer, bytes);
}

/*
 * To write a register, start transaction, transfer data to the TPM, deassert
 * CS when done.
 *
 * Returns one to indicate success, zero (not yet implemented) to indicate
 * failure.
 */
static int tpm2_write_reg(unsigned reg_number, const void *buffer, size_t bytes)
{
	trace_dump("W", reg_number, bytes, buffer, 0);
	start_transaction(false, bytes, reg_number);
	write_bytes(buffer, bytes);
	tpm_if.cs_deassert(&tpm_if.slave);
	return 1;
}

/*
 * To read a register, start transaction, transfer data from the TPM, deassert
 * CS when done.
 *
 * Returns one to indicate success, zero (not yet implemented) to indicate
 * failure.
 */
static int tpm2_read_reg(unsigned reg_number, void *buffer, size_t bytes)
{
	start_transaction(true, bytes, reg_number);
	read_bytes(buffer, bytes);
	tpm_if.cs_deassert(&tpm_if.slave);
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
	return (status >> burst_count_shift) & burst_count_mask;
}

int tpm2_init(struct spi_slave *spi_if)
{
	uint32_t did_vid, status;
	uint8_t cmd;

	memcpy(&tpm_if.slave, spi_if, sizeof(*spi_if));

	tpm2_read_reg(TPM_DID_VID_REG, &did_vid, sizeof(did_vid));

	/* Try claiming locality zero. */
	tpm2_read_reg(TPM_ACCESS_REG, &cmd, sizeof(cmd));
	if ((cmd & (active_locality & tpm_reg_valid_sts)) ==
	    (active_locality & tpm_reg_valid_sts)) {
		/*
		 * Locality active - maybe reset line is not connected?
		 * Release the locality and try again
		 */
		cmd = active_locality;
		tpm2_write_reg(TPM_ACCESS_REG, &cmd, sizeof(cmd));
		tpm2_read_reg(TPM_ACCESS_REG, &cmd, sizeof(cmd));
	}

	/* The tpm_establishment bit can be either set or not, ignore it. */
	if ((cmd & ~tpm_establishment) != tpm_reg_valid_sts) {
		printk(BIOS_ERR, "invalid reset status: %#x\n", cmd);
		return -1;
	}

	cmd = request_use;
	tpm2_write_reg(TPM_ACCESS_REG, &cmd, sizeof(cmd));
	tpm2_read_reg(TPM_ACCESS_REG, &cmd, sizeof(cmd));
	if ((cmd &  ~tpm_establishment) !=
	    (tpm_reg_valid_sts | active_locality)) {
		printk(BIOS_ERR, "failed to claim locality 0, status: %#x\n",
		       cmd);
		return -1;
	}

	read_tpm_sts(&status);
	if (((status >> tpm_family_shift) & tpm_family_mask) !=
	    tpm_family_tpm2) {
		printk(BIOS_ERR, "unexpected TPM family value, status: %#x\n",
		       status);
		return -1;
	}

	/*
	 * Locality claimed, read the revision value and set up the tpm_info
	 * structure.
	 */
	tpm2_read_reg(TPM_RID_REG, &cmd, sizeof(cmd));
	tpm_info.vendor_id = did_vid & 0xffff;
	tpm_info.device_id = did_vid >> 16;
	tpm_info.revision = cmd;

	printk(BIOS_INFO, "Connected to device vid:did:rid of %4.4x:%4.4x:%2.2x\n",
	       tpm_info.vendor_id, tpm_info.device_id, tpm_info.revision);

	/* Let's report device FW version if available. */
	if (tpm_info.vendor_id == 0x1ae0) {
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
	write_tpm_sts(command_ready);

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
	write_tpm_sts(tpm_go);

	/* Now wait for it to report that the response is ready. */
	expected_status_bits = sts_valid | data_avail;
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
	if ((status & expected_status_bits) != sts_valid) {
		printk(BIOS_ERR, "unexpected final status %#x\n", status);
		return 0;
	}

	/* Move the TPM back to idle state. */
	write_tpm_sts(command_ready);

	return payload_size;
}
