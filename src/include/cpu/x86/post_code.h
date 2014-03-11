
#include <console/post_codes.h>


#if CONFIG_POST_IO
#define post_code(value)        \
	movb    $value, %al;    \
	outb    %al, $CONFIG_POST_IO_PORT

#else
#define post_code(value)
#endif
