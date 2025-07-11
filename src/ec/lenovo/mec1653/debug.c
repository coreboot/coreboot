/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <ec/acpi/ec.h>
#include "debug.h"
#include "mec1653.h"


static void debug_cmd(uint8_t cmd)
{
	ec_set_ports(EC_SC, EC_DATA);
	ec_write(EC_DEBUG_CMD, cmd);
	while (ec_read(EC_DEBUG_CMD) & 0x80)
		;
}

uint32_t debug_read_dword(uint32_t addr)
{
	ec_set_ports(EC3_CMD, EC3_DATA);
	ec_clear_out_queue();
	ec_ready_send(EC_SEND_TIMEOUT_US);
	outl(addr << 8 | 0xE2, EC3_DATA);
	ec_ready_recv(EC_SEND_TIMEOUT_US);
	return inl(EC3_DATA);
}

void debug_write_dword(uint32_t addr, uint32_t val)
{
	ec_set_ports(EC3_CMD, EC3_DATA);
	ec_clear_out_queue();
	ec_ready_send(EC_SEND_TIMEOUT_US);
	outl(addr << 8 | 0xEA, EC3_DATA);
	ec_ready_send(EC_SEND_TIMEOUT_US);
	outl(val, EC3_DATA);
}

// Helper function to convert hex character to byte
static uint8_t hex_char_to_byte(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0; // Invalid character
}

// Helper function to convert 16-char hex string to 8 bytes
static void hex_string_to_bytes(const char *hex_str, uint8_t *bytes)
{
	for (int i = 0; i < 8; ++i) {
		bytes[i] = (hex_char_to_byte(hex_str[i * 2]) << 4) |
			   hex_char_to_byte(hex_str[i * 2 + 1]);
	}
}

void debug_write_key(uint8_t i, const char *hex_key)
{
	uint8_t key_bytes[8];

	ec_set_ports(EC_SC, EC_DATA);
	hex_string_to_bytes(hex_key, key_bytes);

	for (int j = 0; j < 8; ++j)
		ec_write(0x3e + j, key_bytes[j]);
	debug_cmd(0xc0 |  (i & 0xf));
}

void debug_read_key(uint8_t i, uint8_t *key)
{
	debug_cmd(0x80 | (i & 0xf));
	for (int j = 0; j < 8; ++j)
		key[j] = ec_read(0x3e + j);
}

uint16_t debug_loaded_keys(void)
{
	ec_set_ports(EC_SC, EC_DATA);
	return (uint16_t) ec_read(0x87) << 8 | (uint16_t) ec_read(0x86);
}
