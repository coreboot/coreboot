#ifndef SMBUS_H
#define SMBUS_H

void smbus_enable(void);
void smbus_setup(void);
int smbus_read_byte(unsigned device, unsigned address, unsigned char *result);

#endif /* SMBUS_H */
