/* $Id$
 * (C) Copyright 2002
 * Humboldt Solutions Ltd, <adrian@humboldt.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _I2C_H
#define _I2C_H

struct i2c_bus;

typedef struct i2c_fn
{
    void (* start)(struct i2c_bus *bus);
    void (* stop)(struct i2c_bus *bus);
    int (* master_write)(struct i2c_bus *bus, int target, int address,
	    const uint8_t *data, int length);
    int (* master_read)(struct i2c_bus *bus, int target, int address,
	    uint8_t *data, int length);
} i2c_fn;

typedef struct i2c_bus
{
    const i2c_fn *fn;
    char *tag;
    void *data;
    struct i2c_bus *next;
} i2c_bus;

i2c_bus *find_i2c_bus(const char *name);
int register_i2c_bus(const i2c_fn *fn, char *tag, void *data);

void i2c_start(struct i2c_bus *bus);
void i2c_stop(struct i2c_bus *bus);
int i2c_master_write(struct i2c_bus *bus, int target, int address,
	    const uint8_t *data, int length);
int i2c_master_read(struct i2c_bus *bus, int target, int address,
	    uint8_t *data, int length);
void init_i2c_nvram(const char *i2c_tag);

extern i2c_fn mpc107_i2c_fn;

#endif
