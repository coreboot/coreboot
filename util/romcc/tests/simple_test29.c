static void outb(unsigned char value, unsigned short port)
{
        __builtin_outb(value, port);
}

static unsigned char inb(unsigned short port)
{
        return __builtin_inb(port);
}

static void __console_tx_byte(unsigned char byte)
{
	while(inb(0x3f8 + 0x05))
		;
	outb(byte, 0x3f8 + 0x00);
}

static void __console_tx_string(int loglevel, const char *str)
{
        if (8 > loglevel) {
                unsigned char ch;
                while((ch = *str++) != '\0') {
                        __console_tx_byte(ch);
                }
        }
}
static void console_init(void)
{
        static const char console_test[] =
                "\r\n\r\nLinuxBIOS-"
                "1.1.0"
                ".0Fallback"
                " "
                "Mon Jun 9 18:15:20 MDT 2003"
                " starting...\r\n";
        __console_tx_string(6, console_test);
}
