void smbus_read_byte(void)
{
	unsigned char host_status_register;
	unsigned char byte;
	int result;

	host_status_register = __builtin_inb(0x1234);

	/* read results of transaction */
	byte = __builtin_inb(0x4567);

	result = byte;
	if (host_status_register != 0x02) {
		result = -1;
	}
}
