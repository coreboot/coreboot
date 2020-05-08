/* unsigned OPTION type */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef OPTION_H_
#define OPTION_H_

struct unsigned_option {
	bool val_known;
	/** It is an error to read this field unless val_known is set. */
	unsigned val;
};

#endif
