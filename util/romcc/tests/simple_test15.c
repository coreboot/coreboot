static void outb(unsigned char value, unsigned short port)
{
        __builtin_outb(value, port);
}

static unsigned char inb(unsigned short port)
{
        return __builtin_inb(port);
}
static int uart_can_tx_byte(void)
{
        return inb(0x3f8 + 0x05) & 0x20;
}

static void uart_wait_to_tx_byte(void)
{
        while(!uart_can_tx_byte())
                ;
}

static void uart_wait_until_sent(void)
{
        while(!(inb(0x3f8 + 0x05) & 0x40))
                ;
}

static void uart_tx_byte(unsigned char data)
{
        uart_wait_to_tx_byte();
        outb(data, 0x3f8 + 0x00);

        uart_wait_until_sent();
}

static void print_debug(const char *str)
{
	unsigned char ch;
	while((ch = *str++) != '\0') {
		uart_tx_byte(ch);
	}
}

static void main(void)
{
	print_debug("one\r\n");
        print_debug("two\r\n");
}
