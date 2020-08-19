#ifndef __X86_POST_CODE_H__
#define __X86_POST_CODE_H__

#include <console/post_codes.h>

#if CONFIG(POST_IO)
#define post_code(value)        \
	movb    $value, %al;    \
	outb    %al, $CONFIG_POST_IO_PORT

#else
#define post_code(value)
#endif

#endif /* __X86_POST_CODE_H__ */
