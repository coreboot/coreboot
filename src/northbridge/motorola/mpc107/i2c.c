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
 
#include <types.h>
#include <stdlib.h>
#include <string.h>
#include "i2c.h"

static i2c_bus *first_i2c = NULL;

#if 0
int register_i2c_bus(const i2c_fn *fn, char *tag, void *data)
{
    i2c_bus *bus = malloc (sizeof (i2c_bus));

    if (bus)
    {
	bus->fn = fn;
	bus->tag = tag;
	bus->data = data;
	bus->next = first_i2c;
	first_i2c = bus;
	return 0;
    }
    return -1;
}
#endif

i2c_bus *find_i2c_bus(const char *name)
{
    int len;

    if (! name)
	return first_i2c;
    
    if (first_i2c)
    {
	i2c_bus *i2c;

	len = strlen(name);

	for (i2c = first_i2c; i2c; i2c = i2c->next)
	    if (strlen(i2c->tag) == len && memcmp (name, i2c->tag, len) == 0)
		return i2c;
    }
    return NULL;
}

void i2c_start(struct i2c_bus *bus)
{
    if (! bus)
	bus = first_i2c;

    bus->fn->start(bus);   
}

void i2c_stop(struct i2c_bus *bus)
{
    if (! bus)
	bus = first_i2c;

    bus->fn->stop(bus);   
}

int i2c_master_write(struct i2c_bus *bus, int target, int address,
	    const u8 *data, int length)
{
    if (! bus)
	bus = first_i2c;

    return bus->fn->master_write(bus, target, address, data, length);   
}

int i2c_master_read(struct i2c_bus *bus, int target, int address,
	    u8 *data, int length)
{
    if (! bus)
	bus = first_i2c;

    return bus->fn->master_read(bus, target, address, data, length);   
}

