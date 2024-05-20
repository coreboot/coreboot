/* SPDX-License-Identifier: GPL-2.0-only */

#include "dasharo_ec.h"
#include <arch/io.h>
#include <console/dasharo_ec.h>
#include <console/console.h>
#include <timer.h>

// This is the command region for Dasharo EC firmware. It must be
// enabled for LPC in the mainboard.
#define DASHARO_EC_BASE 0x0E00
#define DASHARO_EC_SIZE 256

#define REG_CMD 0
#define REG_RESULT 1
#define REG_DATA 2	// Start of command data

// When command register is 0, command is complete
#define CMD_FINISHED 0

#define RESULT_OK 0

// Print command. Registers are unique for each command
#define CMD_PRINT 4
#define CMD_PRINT_REG_FLAGS 2
#define CMD_PRINT_REG_LEN 3
#define CMD_PRINT_REG_DATA 4

static inline uint8_t dasharo_ec_read(uint8_t addr)
{
	return inb(DASHARO_EC_BASE + (uint16_t)addr);
}

static inline void dasharo_ec_write(uint8_t addr, uint8_t data)
{
	outb(data, DASHARO_EC_BASE + (uint16_t)addr);
}

void dasharo_ec_init(void)
{
	// Clear entire command region
	for (int i = 0; i < DASHARO_EC_SIZE; i++)
		dasharo_ec_write((uint8_t)i, 0);
}

void dasharo_ec_flush(void)
{
	dasharo_ec_write(REG_CMD, CMD_PRINT);

	// Wait for command completion, for up to 10 milliseconds, with a
	// test period of 1 microsecond
	wait_us(10000, dasharo_ec_read(REG_CMD) == CMD_FINISHED);

	dasharo_ec_write(CMD_PRINT_REG_LEN, 0);
}

void dasharo_ec_print(uint8_t byte)
{
	uint8_t len = dasharo_ec_read(CMD_PRINT_REG_LEN);
	dasharo_ec_write(CMD_PRINT_REG_DATA + len, byte);
	dasharo_ec_write(CMD_PRINT_REG_LEN, len + 1);

	// If we hit the end of the buffer, or were given a newline, flush
	if (byte == '\n' || len >= (DASHARO_EC_SIZE - CMD_PRINT_REG_DATA))
		dasharo_ec_flush();
}

bool dasharo_ec_cmd(uint8_t cmd, const uint8_t *request_data,
	uint8_t request_size, uint8_t *reply_data, uint8_t reply_size)
{
	if (request_size > DASHARO_EC_SIZE - REG_DATA ||
		reply_size > DASHARO_EC_SIZE - REG_DATA) {
		printk(BIOS_ERR, "EC command %d too long - request size %u, reply size %u\n",
			cmd, request_size, reply_size);
		return false;
	}

	/* If any data were buffered by dasharo_ec_print(), flush it first */
	uint8_t buffered_len = dasharo_ec_read(CMD_PRINT_REG_LEN);
	if (buffered_len > 0)
		dasharo_ec_flush();

	/* Write the data */
	uint8_t i;
	for (i = 0; i < request_size; ++i)
		dasharo_ec_write(REG_DATA + i, request_data[i]);

	/* Write the command */
	dasharo_ec_write(REG_CMD, cmd);

	/* Wait for the command to complete */
	bool ret = true;
	int elapsed = wait_ms(1000, dasharo_ec_read(REG_CMD) == CMD_FINISHED);
	if (elapsed == 0) {
		/* Timed out: fail the command, don't attempt to read a reply. */
		printk(BIOS_WARNING, "EC command %d timed out - request size %d, reply size %d\n",
			cmd, request_size, reply_size);
		ret = false;
	} else {
		/* Read the reply */
		for (i = 0; i < reply_size; ++i)
			reply_data[i] = dasharo_ec_read(REG_DATA+i);
		/* Check the reply status */
		ret = (dasharo_ec_read(REG_RESULT) == RESULT_OK);
	}

	/* Reset the flags and length so we can buffer console prints again */
	dasharo_ec_write(CMD_PRINT_REG_FLAGS, 0);
	dasharo_ec_write(CMD_PRINT_REG_LEN, 0);

	return ret;
}
