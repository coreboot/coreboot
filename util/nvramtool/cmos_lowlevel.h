/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NVRAMTOOL_CMOS_LOWLEVEL_H
#define NVRAMTOOL_CMOS_LOWLEVEL_H

#include "common.h"
#include "layout.h"

typedef struct {
	void (*init)(void* data);
	unsigned char (*read)(unsigned addr);
	void (*write)(unsigned addr, unsigned char value);
	void (*set_iopl)(int level);
} cmos_access_t;

typedef enum { HAL_CMOS, HAL_MEMORY } hal_t;
void select_hal(hal_t hal, void *data);

#define CMOS_AREA_OUT_OF_RANGE (CMOS_RESULT_START + 0)
#define CMOS_AREA_OVERLAPS_RTC (CMOS_RESULT_START + 1)
#define CMOS_AREA_TOO_WIDE (CMOS_RESULT_START + 2)

unsigned long long cmos_read(const cmos_entry_t * e);
void cmos_write(const cmos_entry_t * e, unsigned long long value);
unsigned char cmos_read_byte(unsigned index);
void cmos_write_byte(unsigned index, unsigned char value);
void cmos_read_all(unsigned char data[]);
void cmos_write_all(unsigned char data[]);
void set_iopl(int level);
int verify_cmos_op(unsigned bit, unsigned length, cmos_entry_config_t config);

#define CMOS_SIZE 256		/* size of CMOS memory in bytes */
#define CMOS_RTC_AREA_SIZE 14	/* first 14 bytes control real time clock */

/****************************************************************************
 * verify_cmos_byte_index
 *
 * Return 1 if 'index' does NOT specify a valid CMOS memory location.  Else
 * return 0.
 ****************************************************************************/
static inline int verify_cmos_byte_index(unsigned index)
{
	return (index < CMOS_RTC_AREA_SIZE) || (index >= CMOS_SIZE);
}

#endif				/* NVRAMTOOL_CMOS_LOWLEVEL_H */
