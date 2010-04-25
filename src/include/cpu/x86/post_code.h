

#define post_code(value)        \
	movb    $value, %al;    \
	outb    %al, $0x80

