/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <ec/acpi/ec.h>
#include "include/ec.h"

/*
 * Notes:
 * - ACPI "CMDB": Writing to this offset is equivalent to sending commands.
 *   The CMDx bytes contain the command parameters.
 *
 * TODO - Implement:
 *   - Commands: 0x58, 0xE1 and 0xE2
 *     - 0x51, 0x52: EC flash write?
 *   - ACPI CMDB: 0x63 and 0x64, 0xC7
 *     - 0x0B: Flash lock/write (Set offset 0x0B?)
 *   - Key/recovery detection?
 *
 * Vendor's protocols:
 * - Only read and write are used.
 * - Query, ACPI "CMDB" processing and command 58 are unused.
 * - Equivalent KbcPeim is an unused PPI.
 *
 * NB: Also look for potential EC library
 */

#define EC_INDEX_IO_PORT		0x1200
#define EC_INDEX_IO_HIGH_ADDR_PORT	(EC_INDEX_IO_PORT + 1)
#define EC_INDEX_IO_LOW_ADDR_PORT	(EC_INDEX_IO_PORT + 2)
#define EC_INDEX_IO_DATA_PORT		(EC_INDEX_IO_PORT + 3)

uint8_t ec_cmd_90_read(uint8_t addr)
{
	/* EC ports: 0x62/0x66 */
	send_ec_command(0x90);
	send_ec_data(addr);
	return recv_ec_data();
}

void ec_cmd_91_write(uint8_t addr, uint8_t data)
{
	/* EC ports: 0x62/0x66 */
	send_ec_command(0x91);
	send_ec_data(addr);
	send_ec_data(data);
}

uint8_t ec_cmd_94_query(void)
{
	send_ec_command(0x94);
	return recv_ec_data();
}

uint8_t ec_idx_read(uint16_t addr)
{
	outb((uint8_t)(addr >> 8), EC_INDEX_IO_HIGH_ADDR_PORT);
	outb((uint8_t)addr, EC_INDEX_IO_LOW_ADDR_PORT);
	return inb(EC_INDEX_IO_DATA_PORT);
}

void ec_idx_write(uint16_t addr, uint8_t data)
{
	outb((uint8_t)(addr >> 8), EC_INDEX_IO_HIGH_ADDR_PORT);
	outb((uint8_t)addr, EC_INDEX_IO_LOW_ADDR_PORT);
	outb(data, EC_INDEX_IO_DATA_PORT);
}

/* TODO: Check if ADC is valid. Are there 4, or actually 8 ADCs? */
uint16_t read_ec_adc_converter(uint8_t adc)
{
	uint8_t adc_converters_enabled;	// Contains some ADCs and some DACs
	uint8_t idx_data;
	uint16_t adc_data;

	/* Backup enabled ADCs */
	adc_converters_enabled = ec_idx_read(0xff15);	// ADDAEN

	/* Enable desired ADC in bitmask (not enabled by EC FW, not used by vendor FW) */
	ec_idx_write(0xff15, adc_converters_enabled | ((1 << adc) & 0xf));	// ADDAEN

	/* Sample the desired ADC in binary field; OR the start bit */
	ec_idx_write(0xff18, ((adc << 1) & 0xf) | 1);	// ADCTRL

	/* Read the desired ADC */
	idx_data = ec_idx_read(0xff19);	// ADCDAT
	adc_data = (idx_data << 2);
	/* Lower 2-bits of 10-bit ADC are in high bits of next register */
	idx_data = ec_idx_read(0xff1a);	// ECIF
	adc_data |= ((idx_data & 0xc0) >> 6);

	/* Restore enabled ADCs */
	ec_idx_write(0xff15, adc_converters_enabled);	// ADDAEN

	return adc_data;
}
