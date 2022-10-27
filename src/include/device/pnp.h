/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_PNP_H
#define DEVICE_PNP_H

#include <stdint.h>
#include <device/device.h>
/* When <device/pnp.h> is needed, it supposed to provide <device/pnp_{def,type}.h> */
#include <device/pnp_def.h> /* IWYU pragma: export */
#include <device/pnp_type.h> /* IWYU pragma: export */
#include <arch/io.h>

#if !ENV_PNP_SIMPLE_DEVICE

/* Primitive PNP resource manipulation */
void pnp_write_config(struct device *dev, u8 reg, u8 value);
u8 pnp_read_config(struct device *dev, u8 reg);
void pnp_unset_and_set_config(struct device *dev, u8 reg, u8 unset, u8 set);
void pnp_set_logical_device(struct device *dev);
void pnp_set_enable(struct device *dev, int enable);
int pnp_read_enable(struct device *dev);
void pnp_set_iobase(struct device *dev, u8 index, u16 iobase);
void pnp_set_irq(struct device *dev, u8 index, u8 irq);
void pnp_set_drq(struct device *dev, u8 index, u8 drq);

#endif

/* PNP device operations */
void pnp_read_resources(struct device *dev);
void pnp_set_resources(struct device *dev);
void pnp_enable_resources(struct device *dev);
void pnp_enable(struct device *dev);
void pnp_alt_enable(struct device *dev);

extern struct device_operations pnp_ops;

/* PNP helper operations */

struct pnp_info {
	struct device_operations *ops; /* LDN-specific ops override */
#define PNP_SKIP_FUNCTION 0xffff
	u16 function; /* Must be at least 16 bits (virtual LDNs)! */
	unsigned int flags;
#define PNP_IO0  0x0000001
#define PNP_IO1  0x0000002
#define PNP_IO2  0x0000004
#define PNP_IO3  0x0000008
#define PNP_IO4  0x0000010
#define PNP_IRQ0 0x0000020
#define PNP_IRQ1 0x0000040
#define PNP_DRQ0 0x0000080
#define PNP_DRQ1 0x0000100
#define PNP_EN   0x0000200
#define PNP_MSC0 0x0000400
#define PNP_MSC1 0x0000800
#define PNP_MSC2 0x0001000
#define PNP_MSC3 0x0002000
#define PNP_MSC4 0x0004000
#define PNP_MSC5 0x0008000
#define PNP_MSC6 0x0010000
#define PNP_MSC7 0x0020000
#define PNP_MSC8 0x0040000
#define PNP_MSC9 0x0080000
#define PNP_MSCA 0x0100000
#define PNP_MSCB 0x0200000
#define PNP_MSCC 0x0400000
#define PNP_MSCD 0x0800000
#define PNP_MSCE 0x1000000
	u16 io0, io1, io2, io3, io4;
};
struct resource *pnp_get_resource(struct device *dev, unsigned int index);
void pnp_enable_devices(struct device *dev, struct device_operations *ops,
			unsigned int functions, struct pnp_info *info);

struct pnp_mode_ops {
	void (*enter_conf_mode)(struct device *dev);
	void (*exit_conf_mode)(struct device *dev);
#if CONFIG(HAVE_ACPI_TABLES)
	/*
	 * Generates ASL code to enter/exit config mode.
	 *
	 * @param idx	The ACPI name of the SuperIO index port register. eg. 'INDX'.
	 * @param data	The ACPI name of the SuperIO data port register. eg. 'DATA'.
	 */
	void (*ssdt_enter_conf_mode)(struct device *dev, const char *idx, const char *data);
	void (*ssdt_exit_conf_mode)(struct device *dev, const char *idx, const char *data);
#endif
};
void pnp_enter_conf_mode(struct device *dev);
void pnp_exit_conf_mode(struct device *dev);
#if CONFIG(HAVE_ACPI_TABLES)
/*
 * Generates ASL code to enter/exit config mode if supported.
 * The calling code has to place this within an ASL MethodOP.
 *
 * @param idx	The ACPI name of the SuperIO index port register. eg. 'INDX'.
 * @param data	The ACPI name of the SuperIO data port register. eg. 'DATA'.
 */
void pnp_ssdt_enter_conf_mode(struct device *dev, const char *idx, const char *data);
void pnp_ssdt_exit_conf_mode(struct device *dev, const char *idx, const char *data);
#endif
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

/*
 * void pnp_unset_and_set_index(u16 port, u8 reg, u8 unset, u8 set)
 * Description:
 *  This routine unsets and sets bits from indexed I/O registers. The
 *  reg byte is written to the index register at I/O address = port.
 *  The value byte to update is data register at I/O address = port + 1.
 *
 *  Unlike and-then-or style operations, no bitwise negation is necessary
 *  to specify the bits to unset. Because the bitwise negation implicitly
 *  promotes operands to int before operating, one may have to explicitly
 *  downcast the result if the data width is smaller than that of an int.
 *  Since warnings are errors in coreboot, explicit casting is necessary.
 *
 *  Performing said negation inside this routine alleviates this problem,
 *  while allowing the compiler to warn if the input parameters overflow.
 *  Casting outside this function would silence valid compiler warnings.
 *
 * Parameters:
 *  @param[in] u16 port   = The address of the port index register.
 *  @param[in] u8  reg    = The offset within the indexed space.
 *  @param[in] u8  unset  = Bitmask with ones to the bits to unset from the data register.
 *  @param[in] u8  set    = Bitmask with ones to the bits to set from the data register.
 */
static inline void pnp_unset_and_set_index(u16 port, u8 reg, u8 unset, u8 set)
{
	outb(reg, port);

	u8 value = inb(port + 1);
	value &= (u8)~unset;
	value |= set;
	outb(value, port + 1);
}

#endif /* DEVICE_PNP_H */
