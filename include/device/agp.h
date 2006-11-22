#ifndef DEVICE_AGP_H
#define DEVICE_AGP_H
/* (c) 2005 Linux Networx GPL see COPYING for details */
/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/

unsigned int agp_scan_bus(struct bus *bus, 
	unsigned min_devfn, unsigned max_devfn, unsigned int max);
unsigned int agp_scan_bridge(device_t dev, unsigned int max);

extern struct device_operations default_agp_ops_bus;


#endif /* DEVICE_AGP_H */
