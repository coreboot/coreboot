#ifndef DEVICE_PNP_H
#define DEVICE_PNP_H

#include <stdint.h>
#include <device/device.h>
#include <device/pnp_def.h>

/* Primitive pnp resource manipulation */
void    pnp_write_config(device_t dev, uint8_t reg, uint8_t value);
uint8_t pnp_read_config(device_t dev, uint8_t reg);
void    pnp_set_logical_device(device_t dev);
void    pnp_set_enable(device_t dev, int enable);
int     pnp_read_enable(device_t dev);
void    pnp_set_iobase(device_t dev, unsigned index, unsigned iobase);
void    pnp_set_irq(device_t dev, unsigned index, unsigned irq);
void    pnp_set_drq(device_t dev, unsigned index, unsigned drq);

/* PNP device operations */
void pnp_read_resources(device_t dev);
void pnp_set_resources(device_t dev);
void pnp_enable_resources(device_t dev);
void pnp_enable(device_t dev);

extern struct device_operations pnp_ops;

/* PNP helper operations */

struct io_info {
	unsigned mask, set;
};

struct pnp_info {
	struct device_operations *ops;
	unsigned function;
	unsigned flags;
#define PNP_IO0  0x01
#define PNP_IO1  0x02
#define PNP_IO2  0x04
#define PNP_IO3  0x08
#define PNP_IRQ0 0x10
#define PNP_IRQ1 0x20
#define PNP_DRQ0 0x40
#define PNP_DRQ1 0x80
	struct io_info io0, io1, io2, io3;
};
struct resource *pnp_get_resource(device_t dev, unsigned index);
void pnp_enable_devices(struct device *dev, struct device_operations *ops,
	unsigned functions, struct pnp_info *info);

#endif /* DEVICE_PNP_H */
