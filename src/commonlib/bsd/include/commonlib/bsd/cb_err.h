/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#ifndef _COMMONLIB_BSD_CB_ERR_H_
#define _COMMONLIB_BSD_CB_ERR_H_

#include <stdint.h>

/**
 * coreboot error codes
 *
 * Common error definitions that can be used for any function. All error values
 * should be negative -- when useful, positive values can also be used to denote
 * success. Allocate a new group or errors every 100 values.
 */
enum cb_err {
	CB_SUCCESS = 0,			/**< Call completed successfully */
	CB_ERR = -1,			/**< Generic error code */
	CB_ERR_ARG = -2,		/**< Invalid argument */
	CB_ERR_NOT_IMPLEMENTED = -3,	/**< Function not implemented */

	/* NVRAM/CMOS errors */
	CB_CMOS_OTABLE_DISABLED = -100,		/**< Option table disabled */
	CB_CMOS_LAYOUT_NOT_FOUND = -101,	/**< Layout file not found */
	CB_CMOS_OPTION_NOT_FOUND = -102,	/**< Option string not found */
	CB_CMOS_ACCESS_ERROR = -103,		/**< CMOS access error */
	CB_CMOS_CHECKSUM_INVALID = -104,	/**< CMOS checksum is invalid */

	/* Keyboard test failures */
	CB_KBD_CONTROLLER_FAILURE = -200,
	CB_KBD_INTERFACE_FAILURE = -201,

	/* I2C controller failures */
	CB_I2C_NO_DEVICE	= -300,	/**< Device is not responding */
	CB_I2C_BUSY		= -301,	/**< Device tells it's busy */
	CB_I2C_PROTOCOL_ERROR	= -302,	/**< Data lost or spurious slave
					     device response, try again? */
	CB_I2C_TIMEOUT		= -303, /**< Transmission timed out */

	/* CBFS errors */
	CB_CBFS_IO		= -400, /**< Underlying I/O error */
	CB_CBFS_NOT_FOUND	= -401, /**< File not found in directory */
	CB_CBFS_HASH_MISMATCH	= -402, /**< Master hash validation failed */
	CB_CBFS_CACHE_FULL	= -403, /**< Metadata cache overflowed */
};

#endif	/* _COMMONLIB_BSD_CB_ERR_H_ */
