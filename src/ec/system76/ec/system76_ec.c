/* SPDX-License-Identifier: GPL-2.0-only */

#include "system76_ec.h"
#include <arch/io.h>
#include <console/system76_ec.h>
#include <console/console.h>
#include <timer.h>

// This is the command region for System76 EC firmware. It must be
// enabled for LPC in the mainboard.
#define SYSTEM76_EC_BASE 0x0E00
#define SYSTEM76_EC_SIZE 256

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

static inline uint8_t system76_ec_read(uint8_t addr)
{
	return inb(SYSTEM76_EC_BASE + (uint16_t)addr);
}

static inline void system76_ec_write(uint8_t addr, uint8_t data)
{
	outb(data, SYSTEM76_EC_BASE + (uint16_t)addr);
}

void system76_ec_init(void)
{
	// Clear entire command region
	for (int i = 0; i < SYSTEM76_EC_SIZE; i++)
		system76_ec_write((uint8_t)i, 0);
}

void system76_ec_flush(void)
{
	system76_ec_write(REG_CMD, CMD_PRINT);

	// Wait for command completion, for up to 10 milliseconds, with a
	// test period of 1 microsecond
	wait_us(10000, system76_ec_read(REG_CMD) == CMD_FINISHED);

	system76_ec_write(CMD_PRINT_REG_LEN, 0);
}

void system76_ec_print(uint8_t byte)
{
	uint8_t len = system76_ec_read(CMD_PRINT_REG_LEN);
	system76_ec_write(CMD_PRINT_REG_DATA + len, byte);
	system76_ec_write(CMD_PRINT_REG_LEN, len + 1);

	// If we hit the end of the buffer, or were given a newline, flush
	if (byte == '\n' || len >= (SYSTEM76_EC_SIZE - CMD_PRINT_REG_DATA))
		system76_ec_flush();
}

bool system76_ec_cmd(uint8_t cmd, const uint8_t *request_data,
	uint8_t request_size, uint8_t *reply_data, uint8_t reply_size)
{
	if (request_size > SYSTEM76_EC_SIZE - REG_DATA ||
		reply_size > SYSTEM76_EC_SIZE - REG_DATA) {
		printk(BIOS_ERR, "EC command %d too long - request size %u, reply size %u\n",
			cmd, request_size, reply_size);
		return false;
	}

	/* If any data were buffered by system76_ec_print(), flush it first */
	uint8_t buffered_len = system76_ec_read(CMD_PRINT_REG_LEN);
	if (buffered_len > 0)
		system76_ec_flush();

	/* Write the data */
	uint8_t i;
	for (i = 0; i < request_size; ++i)
		system76_ec_write(REG_DATA + i, request_data[i]);

	/* Write the command */
	system76_ec_write(REG_CMD, cmd);

	/* Wait for the command to complete */
	bool ret = true;
	int elapsed = wait_ms(1000, system76_ec_read(REG_CMD) == CMD_FINISHED);
	if (elapsed == 0) {
		/* Timed out: fail the command, don't attempt to read a reply. */
		printk(BIOS_WARNING, "EC command %d timed out - request size %d, reply size %d\n",
			cmd, request_size, reply_size);
		ret = false;
	} else {
		/* Read the reply */
		for (i = 0; i < reply_size; ++i)
			reply_data[i] = system76_ec_read(REG_DATA+i);
		/* Check the reply status */
		ret = (system76_ec_read(REG_RESULT) == RESULT_OK);
	}

	/* Reset the flags and length so we can buffer console prints again */
	system76_ec_write(CMD_PRINT_REG_FLAGS, 0);
	system76_ec_write(CMD_PRINT_REG_LEN, 0);

	return ret;
}
