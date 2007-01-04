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
#ifndef _BYTEORDER_H
#define _BYTEORDER_H

#define __LITTLE_ENDIAN 1234

#include <arch/swab.h>

#define cpu_to_le32(x) ((unsigned int)(x))
#define le32_to_cpu(x) ((unsigned int)(x))
#define cpu_to_le16(x) ((unsigned short)(x))
#define le16_to_cpu(x) ((unsigned short)(x))
#define cpu_to_be32(x) swab32((x))
#define be32_to_cpu(x) swab32((x))
#define cpu_to_be16(x) swab16((x))
#define be16_to_cpu(x) swab16((x))

#endif /* _BYTEORDER_H */
