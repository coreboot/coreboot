/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/endian.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <endian.h>
#include <lib.h>
#include <security/tpm/tis.h>
#include <timer.h>
#include <types.h>

#define RECV_TIMEOUT            (1 * 1000)  /* 1 second */
#define XMIT_TIMEOUT            (1 * 1000)  /* 1 second */
#define SLEEP_DURATION          1000        /* microseconds */

struct tpm_output_header {
	uint16_t tag;
	uint32_t length;
	uint32_t return_code;
} __packed;

int tis_open(void)
{
	return 0;
}

int tis_init(void)
{
	return 0;
}

int tis_sendrecv(const uint8_t *sendbuf, size_t sbuf_size,
		uint8_t *recvbuf, size_t *rbuf_len)
{
	size_t hdr_bytes;
	struct tpm_output_header *header;
	size_t max_recv_bytes;
	size_t recv_bytes;
	int status;
	struct stopwatch sw;

	ASSERT(sbuf_size >= 10);
	if (CONFIG(DRIVER_TPM_DISPLAY_TIS_BYTES)) {
		/* Display the TPM command */
		if (sbuf_size >= 10)
			printk(BIOS_DEBUG, "TPM Command: 0x%08x\n",
				read_at_be32(sendbuf, sizeof(uint16_t)
				+ sizeof(uint32_t)));
		hexdump(sendbuf, sbuf_size);
	}

	/* Send the command to the TPM */
	stopwatch_init_msecs_expire(&sw, XMIT_TIMEOUT);
	while (1) {
		status = i2c_write_raw(CONFIG_DRIVER_TPM_I2C_BUS,
			CONFIG_DRIVER_TPM_I2C_ADDR, (uint8_t *)sendbuf,
			sbuf_size);
		if ((status < 0) && (!stopwatch_expired(&sw)))
			continue;
		if (status < 0)
			return status;
		break;
	}

	/* Read the TPM response header */
	max_recv_bytes = *rbuf_len;
	ASSERT(max_recv_bytes >= sizeof(*header));
	hdr_bytes = sizeof(*header);
	header = (struct tpm_output_header *)recvbuf;
	stopwatch_init_msecs_expire(&sw, RECV_TIMEOUT);
	do {
		status = i2c_read_raw(CONFIG_DRIVER_TPM_I2C_BUS,
			CONFIG_DRIVER_TPM_I2C_ADDR, recvbuf, hdr_bytes);
		if (status > 0)
			break;
		udelay(SLEEP_DURATION);
	} while (!stopwatch_expired(&sw));
	if (status != sizeof(*header))
		return -1;

	/* Determine the number of bytes remaining */
	recv_bytes = MIN(be32_to_cpu(*(uint32_t *)&header->length),
		max_recv_bytes);

	/* Determine if there is additional response data */
	if (recv_bytes > hdr_bytes) {
		/* Display the TPM response */
		if (CONFIG(DRIVER_TPM_DISPLAY_TIS_BYTES))
			hexdump(recvbuf, hdr_bytes);

		/* Read the full TPM response */
		status = i2c_read_raw(CONFIG_DRIVER_TPM_I2C_BUS,
			CONFIG_DRIVER_TPM_I2C_ADDR, recvbuf, recv_bytes);
		if (status < 0)
			return status;
	}

	/* Return the number of bytes received */
	*rbuf_len = status;

	/* Display the TPM response */
	if (CONFIG(DRIVER_TPM_DISPLAY_TIS_BYTES)) {
		printk(BIOS_DEBUG, "TPM Response: 0x%08x\n",
			read_at_be32(recvbuf, sizeof(uint16_t)
				+ sizeof(uint32_t)));
		hexdump(recvbuf, *rbuf_len);
	}

	/* Successful transfer */
	return 0;
}
