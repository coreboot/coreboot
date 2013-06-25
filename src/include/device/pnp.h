#ifndef DEVICE_PNP_H
#define DEVICE_PNP_H

#include <stdint.h>
#include <arch/rules.h>
#include <device/device.h>
#include <device/pnp_def.h>

#ifndef __SIMPLE_DEVICE__

/* Primitive PNP resource manipulation */
void pnp_write_config(device_t dev, u8 reg, u8 value);
u8 pnp_read_config(device_t dev, u8 reg);
void pnp_set_logical_device(device_t dev);
void pnp_set_enable(device_t dev, int enable);
int pnp_read_enable(device_t dev);
void pnp_set_iobase(device_t dev, u8 index, u16 iobase);
void pnp_set_irq(device_t dev, u8 index, u8 irq);
void pnp_set_drq(device_t dev, u8 index, u8 drq);

/* PNP device operations */
void pnp_read_resources(device_t dev);
void pnp_set_resources(device_t dev);
void pnp_enable_resources(device_t dev);
void pnp_enable(device_t dev);
void pnp_alt_enable(device_t dev);

extern struct device_operations pnp_ops;

/* PNP helper operations */

struct io_info {
	unsigned mask, set;
};

struct pnp_info {
	struct device_operations *ops;
	unsigned int function; /* Must be at least 16 bits (virtual LDNs)! */
	unsigned int flags;
#define PNP_IO0  0x001
#define PNP_IO1  0x002
#define PNP_IO2  0x004
#define PNP_IO3  0x008
#define PNP_IRQ0 0x010
#define PNP_IRQ1 0x020
#define PNP_DRQ0 0x040
#define PNP_DRQ1 0x080
#define PNP_EN   0x100
#define PNP_MSC0 0x200
#define PNP_MSC1 0x400
	struct io_info io0, io1, io2, io3;
};
struct resource *pnp_get_resource(device_t dev, unsigned index);
void pnp_enable_devices(struct device *dev, struct device_operations *ops,
			unsigned int functions, struct pnp_info *info);

struct pnp_mode_ops {
	void (*enter_conf_mode)(device_t dev);
	void (*exit_conf_mode)(device_t dev);
};
void pnp_enter_conf_mode(device_t dev);
void pnp_exit_conf_mode(device_t dev);

#endif /* ! __SIMPLE_DEVICE__ */
#endif /* DEVICE_PNP_H */
