/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __TYPES_H
#define __TYPES_H
#include <stdint.h>
#include <stddef.h>

/**
 * Coreboot error codes
 *
 * When building functions that return a status or an error code, use cb_err as
 * the return type. When failure reason needs to be communicated by the return
 * value, define a it here. Start new enum groups with values in decrements of
 * 100.
 */
enum cb_err {
	CB_SUCCESS = 0,		/**< Call completed succesfully */
	CB_ERR = -1,		/**< Generic error code */
	CB_ERR_ARG = -2,	/**< Invalid argument */

	/* NVRAM/CMOS errors */
	CB_CMOS_OTABLE_DISABLED = -100,		/**< Option table disabled */
	CB_CMOS_LAYOUT_NOT_FOUND = -101,	/**< Layout file not found */
	CB_CMOS_OPTION_NOT_FOUND = -102,	/**< Option string not found */
	CB_CMOS_ACCESS_ERROR = -103,		/**< CMOS access error */
	CB_CMOS_CHECKSUM_INVALID = -104,	/**< CMOS checksum is invalid */

	/* Keyboard test failures */
	CB_KBD_CONTROLLER_FAILURE = -200,
	CB_KBD_INTERFACE_FAILURE = -201
};

#endif /* __TYPES_H */
