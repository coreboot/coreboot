int ck804_smbus_read_byte(unsigned int, unsigned int, unsigned);
int ck804_smbus_write_byte(unsigned int, unsigned int, unsigned int, unsigned char);
void enable_smbus(void);
int smbus_read_byte(unsigned int, unsigned int);
int smbus_write_byte(unsigned int, unsigned int, unsigned char);
