#ifndef SUPERIO_GENERIC_H
#define SUPERIO_GENERIC_H

void pnp_write_config(unsigned char port, unsigned char value, unsigned char reg);
unsigned char pnp_read_config(unsigned char port, unsigned char reg);
void pnp_set_logical_device(unsigned char port, int device);
void pnp_set_enable(unsigned char port, int enable);
int pnp_read_enable(unsigned char port);
void pnp_set_iobase0(unsigned char port, unsigned iobase);
void pnp_set_iobase1(unsigned char port, unsigned iobase);
void pnp_set_irq0(unsigned char port, unsigned irq);
void pnp_set_irq1(unsigned char port, unsigned irq);
void pnp_set_drq(unsigned char port, unsigned drq);

#endif /* SUPERIO_GENERIC_H */
