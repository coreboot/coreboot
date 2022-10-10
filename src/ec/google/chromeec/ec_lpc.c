/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/pnp.h>
#include <ec/google/common/mec.h>
#include <stdint.h>
#include <timer.h>

#include "chip.h"
#include "ec.h"
#include "ec_commands.h"

/*
 * Read bytes from a given LPC-mapped address.
 *
 * @port: Base read address
 * @length: Number of bytes to read
 * @dest: Destination buffer
 * @csum: Optional parameter, sums data read
 */
static void read_bytes(u16 port, unsigned int length, u8 *dest, u8 *csum)
{
	int i;

#if CONFIG(EC_GOOGLE_CHROMEEC_MEC)
	/* Access desired range though EMI interface */
	if (port >= MEC_EMI_RANGE_START && port <= MEC_EMI_RANGE_END) {
		u8 ret = mec_io_bytes(MEC_IO_READ, MEC_EMI_BASE,
				     port - MEC_EMI_RANGE_START,
				     dest, length);
		if (csum)
			*csum += ret;
		return;
	}
#endif

	for (i = 0; i < length; ++i) {
		dest[i] = inb(port + i);
		if (csum)
			*csum += dest[i];
	}
}

/* Read single byte and return byte read */
static inline u8 read_byte(u16 port)
{
	u8 byte;
	read_bytes(port, 1, &byte, NULL);
	return byte;
}

/*
 * Write bytes to a given LPC-mapped address.
 *
 * @port: Base write address
 * @length: Number of bytes to write
 * @msg: Write data buffer
 * @csum: Optional parameter, sums data written
 */
static void write_bytes(u16 port, unsigned int length, u8 *msg, u8 *csum)
{
	int i;

#if CONFIG(EC_GOOGLE_CHROMEEC_MEC)
	/* Access desired range though EMI interface */
	if (port >= MEC_EMI_RANGE_START && port <= MEC_EMI_RANGE_END) {
		u8 ret = mec_io_bytes(MEC_IO_WRITE, MEC_EMI_BASE,
				     port - MEC_EMI_RANGE_START,
				     msg, length);
		if (csum)
			*csum += ret;
		return;
	}
#endif

	for (i = 0; i < length; ++i) {
		outb(msg[i], port + i);
		if (csum)
			*csum += msg[i];
	}
}

/* Write single byte and return byte written */
static inline u8 write_byte(u8 val, u16 port)
{
	u8 byte = val;
	write_bytes(port, 1, &byte, NULL);
	return byte;
}

static int google_chromeec_status_check(u16 port, u8 mask, u8 cond)
{
	struct stopwatch timeout_sw;
	/* One second is more than plenty for any EC operation to complete */
	const uint64_t ec_status_timeout_us = 1 * USECS_PER_SEC;
	/* Wait 1 usec between read attempts  */
	const uint64_t ec_status_read_period_us = 1;

	stopwatch_init_usecs_expire(&timeout_sw, ec_status_timeout_us);
	do {
		if ((read_byte(port) & mask) == cond)
			return 0;
		udelay(ec_status_read_period_us);
	} while (!stopwatch_expired(&timeout_sw));
	return -1;
}

static int google_chromeec_wait_ready(u16 port)
{
	return google_chromeec_status_check(port,
					    EC_LPC_CMDR_PENDING |
					    EC_LPC_CMDR_BUSY, 0);
}

#if CONFIG(EC_GOOGLE_CHROMEEC_ACPI_MEMMAP)
/* Read memmap data through ACPI port 66/62 */
static int read_memmap(u8 *data, u8 offset)
{
	if (google_chromeec_wait_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC ready!\n");
		return -1;
	}

	/* Issue the ACPI read command */
	write_byte(EC_CMD_ACPI_READ, EC_LPC_ADDR_ACPI_CMD);

	if (google_chromeec_wait_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC READ_EVENT!\n");
		return -1;
	}

	/* Write data address */
	write_byte(offset + EC_ACPI_MEM_MAPPED_BEGIN, EC_LPC_ADDR_ACPI_DATA);

	if (google_chromeec_wait_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC DATA!\n");
		return -1;
	}

	*data = read_byte(EC_LPC_ADDR_ACPI_DATA);
	return 0;
}
#endif

static int google_chromeec_command_version(void)
{
	u8 id1, id2, flags;

#if CONFIG(EC_GOOGLE_CHROMEEC_ACPI_MEMMAP)
	if (read_memmap(&id1, EC_MEMMAP_ID) ||
	    read_memmap(&id2, EC_MEMMAP_ID + 1) ||
	    read_memmap(&flags, EC_MEMMAP_HOST_CMD_FLAGS)) {
		printk(BIOS_ERR, "Error reading memmap data.\n");
		return -1;
	}
#else
	id1 = read_byte(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_ID);
	id2 = read_byte(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_ID + 1);
	flags = read_byte(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_HOST_CMD_FLAGS);
#endif

	if (id1 != 'E' || id2 != 'C') {
		printk(BIOS_ERR, "Missing Chromium EC memory map.\n");
		return -1;
	}

	if (flags & EC_HOST_CMD_FLAG_VERSION_3) {
		return EC_HOST_CMD_FLAG_VERSION_3;
	} else if (flags & EC_HOST_CMD_FLAG_LPC_ARGS_SUPPORTED) {
		return EC_HOST_CMD_FLAG_LPC_ARGS_SUPPORTED;
	}
	printk(BIOS_ERR, "Chromium EC command version unsupported\n");
	return -1;
}

static int google_chromeec_command_v3(struct chromeec_command *cec_command)
{
	struct ec_host_request rq;
	struct ec_host_response rs;
	const u8 *d;
	u8 csum = 0;
	int i;

	if (cec_command->cmd_size_in + sizeof(rq) > EC_LPC_HOST_PACKET_SIZE) {
		printk(BIOS_ERR, "EC cannot send %zu bytes\n",
		       cec_command->cmd_size_in + sizeof(rq));
		return -1;
	}

	if (cec_command->cmd_size_out > EC_LPC_HOST_PACKET_SIZE) {
		printk(BIOS_ERR, "EC cannot receive %d bytes\n",
		       cec_command->cmd_size_out);
		return -1;
	}

	if (google_chromeec_wait_ready(EC_LPC_ADDR_HOST_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC start command %d!\n",
		       cec_command->cmd_code);
		return -1;
	}

	/* Fill in request packet */
	rq.struct_version = EC_HOST_REQUEST_VERSION;
	rq.checksum = 0;
	rq.command = cec_command->cmd_code |
		EC_CMD_PASSTHRU_OFFSET(cec_command->cmd_dev_index);
	rq.command_version = cec_command->cmd_version;
	rq.reserved = 0;
	rq.data_len = cec_command->cmd_size_in;

	/* Copy data and start checksum */
	write_bytes(EC_LPC_ADDR_HOST_PACKET + sizeof(rq),
		    cec_command->cmd_size_in,
		    (u8*)cec_command->cmd_data_in,
		    &csum);

	/* Finish checksum */
	for (i = 0, d = (const u8 *)&rq; i < sizeof(rq); i++, d++)
		csum += *d;

	/* Write checksum field so the entire packet sums to 0 */
	rq.checksum = -csum;

	/* Copy header */
	write_bytes(EC_LPC_ADDR_HOST_PACKET, sizeof(rq), (u8*)&rq, NULL);

	/* Start the command */
	write_byte(EC_COMMAND_PROTOCOL_3, EC_LPC_ADDR_HOST_CMD);

	if (google_chromeec_wait_ready(EC_LPC_ADDR_HOST_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC process command %d!\n",
		       cec_command->cmd_code);
		return -1;
	}

	/* Check result */
	cec_command->cmd_code = read_byte(EC_LPC_ADDR_HOST_DATA);
	if (cec_command->cmd_code) {
		printk(BIOS_SPEW, "EC returned error result code %d\n",
			cec_command->cmd_code);
		return -i;
	}

	/* Read back response header and start checksum */
	csum = 0;
	read_bytes(EC_LPC_ADDR_HOST_PACKET, sizeof(rs), (u8*)&rs, &csum);

	if (rs.struct_version != EC_HOST_RESPONSE_VERSION) {
		printk(BIOS_ERR, "EC response version mismatch (%d != %d)\n",
		       rs.struct_version, EC_HOST_RESPONSE_VERSION);
		return -1;
	}

	if (rs.reserved) {
		printk(BIOS_ERR, "EC response reserved is %d, should be 0\n",
			rs.reserved);
		return -1;
	}

	if (rs.data_len > cec_command->cmd_size_out) {
		printk(BIOS_ERR, "EC returned too much data (%d > %d)\n",
		       rs.data_len, cec_command->cmd_size_out);
		return -1;
	}

	/* Read back data and update checksum */
	read_bytes(EC_LPC_ADDR_HOST_PACKET + sizeof(rs),
		   rs.data_len,
		   cec_command->cmd_data_out,
		   &csum);

	/* Verify checksum */
	if (csum) {
		printk(BIOS_ERR, "EC response has invalid checksum\n");
		return -1;
	}

	return 0;
}

static int google_chromeec_command_v1(struct chromeec_command *cec_command)
{
	struct ec_lpc_host_args args;
	u8 cmd_code = cec_command->cmd_code;
	u8 csum;

	/* Fill in args */
	args.flags = EC_HOST_ARGS_FLAG_FROM_HOST;
	args.command_version = cec_command->cmd_version;
	args.data_size = cec_command->cmd_size_in;

	/* Initialize checksum */
	csum = cmd_code + args.flags + args.command_version + args.data_size;

	write_bytes(EC_LPC_ADDR_HOST_PARAM,
		    cec_command->cmd_size_in,
		    (u8*)cec_command->cmd_data_in,
		    &csum);

	/* Finalize checksum and write args */
	args.checksum = csum;
	write_bytes(EC_LPC_ADDR_HOST_ARGS, sizeof(args), (u8*)&args, NULL);

	/* Issue the command */
	write_byte(cmd_code, EC_LPC_ADDR_HOST_CMD);

	if (google_chromeec_wait_ready(EC_LPC_ADDR_HOST_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC process command %d!\n",
		       cec_command->cmd_code);
		return 1;
	}

	/* Check result */
	cec_command->cmd_code = read_byte(EC_LPC_ADDR_HOST_DATA);
	if (cec_command->cmd_code)
		return 1;

	/* Read back args */
	read_bytes(EC_LPC_ADDR_HOST_ARGS, sizeof(args), (u8*)&args, NULL);

	/*
	 * If EC didn't modify args flags, then somehow we sent a new-style
	 * command to an old EC, which means it would have read its params
	 * from the wrong place.
	 */
	if (!(args.flags & EC_HOST_ARGS_FLAG_TO_HOST)) {
		printk(BIOS_ERR, "EC protocol mismatch\n");
		return 1;
	}

	if (args.data_size > cec_command->cmd_size_out) {
		printk(BIOS_ERR, "EC returned too much data\n");
		return 1;
	}
	cec_command->cmd_size_out = args.data_size;

	/* Start calculating response checksum */
	csum = cmd_code + args.flags + args.command_version + args.data_size;

	/* Read data, if any */
	read_bytes(EC_LPC_ADDR_HOST_PARAM,
		   args.data_size,
		   cec_command->cmd_data_out,
		   &csum);

	/* Verify checksum */
	if (args.checksum != csum) {
		printk(BIOS_ERR, "EC response has invalid checksum\n");
		return 1;
	}

	return 0;
}

/* Return the byte of EC switch states */
uint8_t google_chromeec_get_switches(void)
{
	return read_byte(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES);
}

void google_chromeec_ioport_range(uint16_t *out_base, size_t *out_size)
{
	uint16_t base;
	size_t size;

	if (CONFIG(EC_GOOGLE_CHROMEEC_MEC)) {
		base = MEC_EMI_BASE;
		size = MEC_EMI_SIZE;
	} else {
		base = EC_HOST_CMD_REGION0;
		size = 2 * EC_HOST_CMD_REGION_SIZE;
		/* Make sure MEMMAP region follows host cmd region. */
		assert(base + size == EC_LPC_ADDR_MEMMAP);
		size += EC_MEMMAP_SIZE;
	}

	*out_base = base;
	*out_size = size;
}

int google_chromeec_command(struct chromeec_command *cec_command)
{
	static int command_version;

	if (command_version <= 0)
		command_version = google_chromeec_command_version();

	switch (command_version) {
	case EC_HOST_CMD_FLAG_VERSION_3:
		return google_chromeec_command_v3(cec_command);
	case EC_HOST_CMD_FLAG_LPC_ARGS_SUPPORTED:
		return google_chromeec_command_v1(cec_command);
	}
	return -1;
}

static void lpc_ec_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	google_chromeec_init();
}

/*
 * Declare the IO ports that we are using:
 *
 * All ECs (not explicitly declared):
 * 0x60/0x64, 0x62/0x66, 0x80, 0x200->0x207
 *
 * mec1322:	0x800->0x807
 * All others:	0x800->0x9ff
 *
 * EC_GOOGLE_CHROMEEC_ACPI_MEMMAP is only used for MEC ECs.
 */
static void lpc_ec_read_resources(struct device *dev)
{
	unsigned int idx = 0;
	struct resource * res;
	uint16_t base;
	size_t size;

	google_chromeec_ioport_range(&base, &size);
	res = new_resource(dev, idx++);
	res->base = base;
	res->size = size;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations ops = {
	.init             = lpc_ec_init,
	.read_resources   = lpc_ec_read_resources,
	.set_resources    = noop_set_resources,
	.scan_bus         = scan_static_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = google_chromeec_acpi_name,
	.acpi_fill_ssdt   = google_chromeec_fill_ssdt_generator,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, 0, 0, 0, }
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations ec_google_chromeec_ops = {
	CHIP_NAME("Google Chrome EC")
	.enable_dev = enable_dev,
};

static int google_chromeec_data_ready(u16 port)
{
	return google_chromeec_status_check(port, EC_LPC_CMDR_DATA,
					    EC_LPC_CMDR_DATA);
}

enum host_event_code google_chromeec_get_event(void)
{
	if (google_chromeec_wait_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC ready!\n");
		return EC_HOST_EVENT_NONE;
	}

	/* Issue the ACPI query-event command */
	write_byte(EC_CMD_ACPI_QUERY_EVENT, EC_LPC_ADDR_ACPI_CMD);

	if (google_chromeec_wait_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for EC QUERY_EVENT!\n");
		return EC_HOST_EVENT_NONE;
	}

	if (google_chromeec_data_ready(EC_LPC_ADDR_ACPI_CMD)) {
		printk(BIOS_ERR, "Timeout waiting for data ready!\n");
		return EC_HOST_EVENT_NONE;
	}

	/* Event (or 0 if none) is returned directly in the data byte */
	return read_byte(EC_LPC_ADDR_ACPI_DATA);
}
