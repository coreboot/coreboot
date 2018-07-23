#ifndef DEVICE_PNP_H
#define DEVICE_PNP_H

#include <stdint.h>
#include <rules.h>
#include <device/device.h>
#include <device/pnp_def.h>
#include <arch/io.h>

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

struct pnp_info {
	struct device_operations *ops; /* LDN-specific ops override */
	unsigned int function; /* Must be at least 16 bits (virtual LDNs)! */
	unsigned int flags;
#define PNP_IO0  0x000001
#define PNP_IO1  0x000002
#define PNP_IO2  0x000004
#define PNP_IO3  0x000008
#define PNP_IRQ0 0x000010
#define PNP_IRQ1 0x000020
#define PNP_DRQ0 0x000040
#define PNP_DRQ1 0x000080
#define PNP_EN   0x000100
#define PNP_MSC0 0x000200
#define PNP_MSC1 0x000400
#define PNP_MSC2 0x000800
#define PNP_MSC3 0x001000
#define PNP_MSC4 0x002000
#define PNP_MSC5 0x004000
#define PNP_MSC6 0x008000
#define PNP_MSC7 0x010000
#define PNP_MSC8 0x020000
#define PNP_MSC9 0x040000
#define PNP_MSCA 0x080000
#define PNP_MSCB 0x100000
#define PNP_MSCC 0x200000
#define PNP_MSCD 0x400000
#define PNP_MSCE 0x800000
	u16 io0, io1, io2, io3;
};
struct resource *pnp_get_resource(device_t dev, unsigned int index);
void pnp_enable_devices(struct device *dev, struct device_operations *ops,
			unsigned int functions, struct pnp_info *info);

struct pnp_mode_ops {
	void (*enter_conf_mode)(device_t dev);
	void (*exit_conf_mode)(device_t dev);
};
void pnp_enter_conf_mode(device_t dev);
void pnp_exit_conf_mode(device_t dev);

/* PNP indexed I/O operations */

/*
 * u8 pnp_read_index(u16 port, u8 reg)
 * Description:
 *  This routine reads indexed I/O registers. The reg byte is written
 *  to the index register at I/O address = port. The result is then
 *  read from the data register at I/O address = port + 1.
 *
 * Parameters:
 *  @param[in]  u16 port   = The I/O address of the port index register.
 *  @param[in]  u8  reg    = The offset within the indexed space.
 *  @param[out] u8  result = The value read back from the data register.
 */
static inline u8 pnp_read_index(u16 port, u8 reg)
{
	outb(reg, port);
	return inb(port + 1);
}

/*
 * void pnp_write_index(u16 port, u8 reg, u8 value)
 * Description:
 *  This routine writes indexed I/O registers. The reg byte is written
 *  to the index register at I/O address = port. The value byte is then
 *  written to the data register at I/O address = port + 1.
 *
 * Parameters:
 *  @param[in] u16 port   = The address of the port index register.
 *  @param[in] u8  reg    = The offset within the indexed space.
 *  @param[in] u8  value  = The value to be written to the data register.
 */
static inline void pnp_write_index(u16 port, u8 reg, u8 value)
{
	outb(reg, port);
	outb(value, port + 1);
}

#endif /* ! __SIMPLE_DEVICE__ */
#endif /* DEVICE_PNP_H */
