/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __X86_POST_CODE_H__
#define __X86_POST_CODE_H__

#include <commonlib/console/post_codes.h>

#if CONFIG(POST_IO) && !(ENV_BOOTBLOCK && CONFIG(NO_EARLY_BOOTBLOCK_POSTCODES))
#define post_code(value)        \
	movb    $value, %al;    \
	outb    %al, $CONFIG_POST_IO_PORT

#else
#define post_code(value)
#endif

#endif /* __X86_POST_CODE_H__ */
