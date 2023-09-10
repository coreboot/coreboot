/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>

#include "nct6687d_ec.h"

void nct6687d_ec_write_page(uint16_t iobase, uint8_t page, uint8_t index, uint8_t value)
{
	/* Need to write 0xff first to set the page */
	outb(0xff, iobase + EC_PORT0_PAGE);
	outb(page, iobase + EC_PORT0_PAGE);
	outb(index, iobase + EC_PORT0_INDEX);
	outb(value, iobase + EC_PORT0_DATA);
	/* Need to write 0xff at the end of transaction */
	outb(0xff, iobase + EC_PORT0_PAGE);
}

void nct6687d_ec_write_page_ff(uint16_t iobase, uint8_t page, uint8_t index, uint8_t value)
{
	/* Need to write 0xff first to set the page */
	outb(0xff, iobase + EC_PORT0_PAGE);
	outb(page, iobase + EC_PORT0_PAGE);
	/* Extra 0xff to the index register */
	outb(0xff, iobase + EC_PORT0_INDEX);
	outb(index, iobase + EC_PORT0_INDEX);
	outb(value, iobase + EC_PORT0_DATA);
	/* Need to write 0xff at the end of transaction */
	outb(0xff, iobase + EC_PORT0_PAGE);
}

void nct6687d_ec_and_or_page(uint16_t iobase, uint8_t page, uint8_t index,
			     uint8_t and_mask, uint8_t or_mask)
{
	uint8_t val;
	/* Need to write 0xff first to set the page */
	outb(0xff, iobase + EC_PORT0_PAGE);
	outb(page, iobase + EC_PORT0_PAGE);
	outb(index, iobase + EC_PORT0_INDEX);
	val = inb(iobase + EC_PORT0_DATA);
	val &= and_mask;
	val |= or_mask;
	outb(val, iobase + EC_PORT0_DATA);
	/* Need to write 0xff at the end of transaction */
	outb(0xff, iobase + EC_PORT0_PAGE);
}

void nct6687d_ec_and_or_page_ff(uint16_t iobase, uint8_t page, uint8_t index,
			     uint8_t and_mask, uint8_t or_mask)
{
	uint8_t val;
	/* Need to write 0xff first to set the page */
	outb(0xff, iobase + EC_PORT0_PAGE);
	outb(page, iobase + EC_PORT0_PAGE);
	/* Extra 0xff to the index register */
	outb(0xff, iobase + EC_PORT0_INDEX);
	outb(index, iobase + EC_PORT0_INDEX);
	val = inb(iobase + EC_PORT0_DATA);
	val &= and_mask;
	val |= or_mask;
	outb(val, iobase + EC_PORT0_DATA);
	/* Need to write 0xff at the end of transaction */
	outb(0xff, iobase + EC_PORT0_PAGE);
}

uint8_t nct6687d_ec_read_page(uint16_t iobase, uint8_t page, uint8_t index)
{
	uint8_t value;
	/* Need to write 0xff first to set the page */
	outb(0xff, iobase + EC_PORT0_PAGE);
	outb(page, iobase + EC_PORT0_PAGE);
	outb(index, iobase + EC_PORT0_INDEX);
	value = inb(iobase + EC_PORT0_DATA);
	/* Need to write 0xff at the end of transaction */
	outb(0xff, iobase + EC_PORT0_PAGE);

	return value;
}
