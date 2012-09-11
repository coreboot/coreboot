/*****************************************************************************\
 * cmos_lowlevel.c
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by David S. Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of nvramtool, a utility for reading/writing coreboot
 *  parameters and displaying information from the coreboot table.
 *  For details, see http://coreboot.org/nvramtool.
 *
 *  Please also read the file DISCLAIMER which is included in this software
 *  distribution.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the terms and
 *  conditions of the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
\*****************************************************************************/

#if defined(__FreeBSD__)
#include <fcntl.h>
#include <unistd.h>
#endif

#include "common.h"
#include "cmos_lowlevel.h"

/* Hardware Abstraction Layer: lowlevel byte-wise write access */

extern cmos_access_t cmos_hal, memory_hal;
static cmos_access_t *current_access =
#ifdef CMOS_HAL
	&cmos_hal;
#else
	&memory_hal;
#endif

void select_hal(hal_t hal, void *data)
{
	switch(hal) {
#ifdef CMOS_HAL
		case HAL_CMOS:
			current_access = &cmos_hal;
			break;
#endif
		case HAL_MEMORY:
		default:
			current_access = &memory_hal;
			break;
	}
	current_access->init(data);
}

/* Bit-level access */
typedef struct {
	unsigned byte_index;
	unsigned bit_offset;
} cmos_bit_op_location_t;

static unsigned cmos_bit_op_strategy(unsigned bit, unsigned bits_left,
				     cmos_bit_op_location_t * where);
static unsigned char cmos_read_bits(const cmos_bit_op_location_t * where,
				    unsigned nr_bits);
static void cmos_write_bits(const cmos_bit_op_location_t * where,
			    unsigned nr_bits, unsigned char value);
static unsigned char get_bits(unsigned long long value, unsigned bit,
			      unsigned nr_bits);
static void put_bits(unsigned char value, unsigned bit, unsigned nr_bits,
		     unsigned long long *result);

/****************************************************************************
 * get_bits
 *
 * Extract a value 'nr_bits' bits wide starting at bit position 'bit' from
 * 'value' and return the result.  It is assumed that 'nr_bits' is at most 8.
 ****************************************************************************/
static inline unsigned char get_bits(unsigned long long value, unsigned bit,
				     unsigned nr_bits)
{
	return (value >> bit) & ((unsigned char)((1 << nr_bits) - 1));
}

/****************************************************************************
 * put_bits
 *
 * Extract the low order 'nr_bits' bits from 'value' and store them in the
 * value pointed to by 'result' starting at bit position 'bit'.  The bit
 * positions in 'result' where the result is stored are assumed to be
 * initially zero.
 ****************************************************************************/
static inline void put_bits(unsigned char value, unsigned bit,
			    unsigned nr_bits, unsigned long long *result)
{
	*result += ((unsigned long long)(value &
				((unsigned char)((1 << nr_bits) - 1)))) << bit;
}

/****************************************************************************
 * cmos_read
 *
 * Read value from nonvolatile RAM at position given by 'bit' and 'length'
 * and return this value.  The I/O privilege level of the currently executing
 * process must be set appropriately.
 ****************************************************************************/
unsigned long long cmos_read(const cmos_entry_t * e)
{
	cmos_bit_op_location_t where;
	unsigned bit = e->bit, length = e->length;
	unsigned next_bit, bits_left, nr_bits;
	unsigned long long result = 0;
	unsigned char value;

	assert(!verify_cmos_op(bit, length, e->config));
	result = 0;

	if (e->config == CMOS_ENTRY_STRING) {
		char *newstring = calloc(1, (length + 7) / 8);
		unsigned usize = (8 * sizeof(unsigned long long));

		if (!newstring) {
			out_of_memory();
		}

		for (next_bit = 0, bits_left = length;
		     bits_left; next_bit += nr_bits, bits_left -= nr_bits) {
			nr_bits = cmos_bit_op_strategy(bit + next_bit,
				   bits_left > usize ? usize : bits_left, &where);
			value = cmos_read_bits(&where, nr_bits);
			put_bits(value, next_bit % usize, nr_bits,
				 &((unsigned long long *)newstring)[next_bit /
								    usize]);
			result = (unsigned long)newstring;
		}
	} else {
		for (next_bit = 0, bits_left = length;
		     bits_left; next_bit += nr_bits, bits_left -= nr_bits) {
			nr_bits =
			    cmos_bit_op_strategy(bit + next_bit, bits_left,
						 &where);
			value = cmos_read_bits(&where, nr_bits);
			put_bits(value, next_bit, nr_bits, &result);
		}
	}

	return result;
}

/****************************************************************************
 * cmos_write
 *
 * Write 'data' to nonvolatile RAM at position given by 'bit' and 'length'.
 * The I/O privilege level of the currently executing process must be set
 * appropriately.
 ****************************************************************************/
void cmos_write(const cmos_entry_t * e, unsigned long long value)
{
	cmos_bit_op_location_t where;
	unsigned bit = e->bit, length = e->length;
	unsigned next_bit, bits_left, nr_bits;

	assert(!verify_cmos_op(bit, length, e->config));

	if (e->config == CMOS_ENTRY_STRING) {
		unsigned long long *data =
		    (unsigned long long *)(unsigned long)value;
		unsigned usize = (8 * sizeof(unsigned long long));

		for (next_bit = 0, bits_left = length;
		     bits_left; next_bit += nr_bits, bits_left -= nr_bits) {
			nr_bits = cmos_bit_op_strategy(bit + next_bit,
					bits_left > usize ? usize : bits_left,
					&where);
			value = data[next_bit / usize];
			cmos_write_bits(&where, nr_bits,
				get_bits(value, next_bit % usize, nr_bits));
		}
	} else {
		for (next_bit = 0, bits_left = length;
		     bits_left; next_bit += nr_bits, bits_left -= nr_bits) {
			nr_bits = cmos_bit_op_strategy(bit + next_bit,
					bits_left, &where);
			cmos_write_bits(&where, nr_bits,
					get_bits(value, next_bit, nr_bits));
		}
	}
}

/****************************************************************************
 * cmos_read_byte
 *
 * Read a byte from nonvolatile RAM at a position given by 'index' and return
 * the result.  An 'index' value of 0 represents the first byte of
 * nonvolatile RAM.
 *
 * Note: the first 14 bytes of nonvolatile RAM provide an interface to the
 *       real time clock.
 ****************************************************************************/
unsigned char cmos_read_byte(unsigned index)
{
	return current_access->read(index);
}

/****************************************************************************
 * cmos_write_byte
 *
 * Write 'value' to nonvolatile RAM at a position given by 'index'.  An
 * 'index' of 0 represents the first byte of nonvolatile RAM.
 *
 * Note: the first 14 bytes of nonvolatile RAM provide an interface to the
 *       real time clock.  Writing to any of these bytes will therefore
 *       affect its functioning.
 ****************************************************************************/
void cmos_write_byte(unsigned index, unsigned char value)
{
	current_access->write(index, value);
}

/****************************************************************************
 * cmos_read_all
 *
 * Read all contents of CMOS memory into array 'data'.  The first 14 bytes of
 * 'data' are set to zero since this corresponds to the real time clock area.
 ****************************************************************************/
void cmos_read_all(unsigned char data[])
{
	unsigned i;

	for (i = 0; i < CMOS_RTC_AREA_SIZE; i++)
		data[i] = 0;

	for (; i < CMOS_SIZE; i++)
		data[i] = cmos_read_byte(i);
}

/****************************************************************************
 * cmos_write_all
 *
 * Update all of CMOS memory with the contents of array 'data'.  The first 14
 * bytes of 'data' are ignored since this corresponds to the real time clock
 * area.
 ****************************************************************************/
void cmos_write_all(unsigned char data[])
{
	unsigned i;

	for (i = CMOS_RTC_AREA_SIZE; i < CMOS_SIZE; i++)
		cmos_write_byte(i, data[i]);
}

/****************************************************************************
 * set_iopl
 *
 * Set the I/O privilege level of the executing process.  Root privileges are
 * required for performing this action.  A sufficient I/O privilege level
 * allows the process to access x86 I/O address space and to disable/reenable
 * interrupts while executing in user space.  Messing with the I/O privilege
 * level is therefore somewhat dangerous.
 ****************************************************************************/
void set_iopl(int level)
{
	current_access->set_iopl(level);
}

/****************************************************************************
 * verify_cmos_op
 *
 * 'bit' represents a bit position in the nonvolatile RAM.  The first bit
 * (i.e. the lowest order bit of the first byte) of nonvolatile RAM is
 * labeled as bit 0.  'length' represents the width in bits of a value we
 * wish to read or write.  Perform sanity checking on 'bit' and 'length'.  If
 * no problems were encountered, return OK.  Else return an error code.
 ****************************************************************************/
int verify_cmos_op(unsigned bit, unsigned length, cmos_entry_config_t config)
{
	if ((bit >= (8 * CMOS_SIZE)) || ((bit + length) > (8 * CMOS_SIZE)))
		return CMOS_AREA_OUT_OF_RANGE;

	if (bit < (8 * CMOS_RTC_AREA_SIZE))
		return CMOS_AREA_OVERLAPS_RTC;

	if (config == CMOS_ENTRY_STRING)
		return OK;

	if (length > (8 * sizeof(unsigned long long)))
		return CMOS_AREA_TOO_WIDE;

	return OK;
}

/****************************************************************************
 * cmos_bit_op_strategy
 *
 * Helper function used by cmos_read() and cmos_write() to determine which
 * bits to read or write next.
 ****************************************************************************/
static unsigned cmos_bit_op_strategy(unsigned bit, unsigned bits_left,
				     cmos_bit_op_location_t * where)
{
	unsigned max_bits;

	where->byte_index = bit >> 3;
	where->bit_offset = bit & 0x07;
	max_bits = 8 - where->bit_offset;
	return (bits_left > max_bits) ? max_bits : bits_left;
}

/****************************************************************************
 * cmos_read_bits
 *
 * Read a chunk of bits from a byte location within CMOS memory.  Return the
 * value represented by the chunk of bits.
 ****************************************************************************/
static unsigned char cmos_read_bits(const cmos_bit_op_location_t * where,
				    unsigned nr_bits)
{
	return (cmos_read_byte(where->byte_index) >> where->bit_offset) &
	    ((unsigned char)((1 << nr_bits) - 1));
}

/****************************************************************************
 * cmos_write_bits
 *
 * Write a chunk of bits (the low order 'nr_bits' bits of 'value') to an area
 * within a particular byte of CMOS memory.
 ****************************************************************************/
static void cmos_write_bits(const cmos_bit_op_location_t * where,
			    unsigned nr_bits, unsigned char value)
{
	unsigned char n, mask;

	if (nr_bits == 8) {
		cmos_write_byte(where->byte_index, value);
		return;
	}

	n = cmos_read_byte(where->byte_index);
	mask = ((unsigned char)((1 << nr_bits) - 1)) << where->bit_offset;
	n = (n & ~mask) + ((value << where->bit_offset) & mask);
	cmos_write_byte(where->byte_index, n);
}
