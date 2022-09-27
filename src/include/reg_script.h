/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef REG_SCRIPT_H
#define REG_SCRIPT_H

#include <stdint.h>
#include <device/device.h>
#include <device/pci_type.h>
#include <device/resource.h>

/*
 * The reg script library is a way to provide data-driven I/O accesses for
 * initializing devices. It currently supports PCI, legacy I/O,
 * memory-mapped I/O, and IOSF accesses.
 *
 * In order to simplify things for the developer the following features
 * are employed:
 * - Chaining of tables that allow runtime tables to chain to compile-time
 *   tables.
 *
 * Note that when using REG_SCRIPT_COMMAND_NEXT there is an implicit push
 * and pop of the context. A chained reg_script inherits the previous
 * context (such as current device), but it does not impact the previous
 * context in any way.
 */

enum {
	REG_SCRIPT_COMMAND_READ,
	REG_SCRIPT_COMMAND_WRITE,
	REG_SCRIPT_COMMAND_RMW,
	REG_SCRIPT_COMMAND_RXW,
	REG_SCRIPT_COMMAND_POLL,
	REG_SCRIPT_COMMAND_SET_DEV,
	REG_SCRIPT_COMMAND_NEXT,
	REG_SCRIPT_COMMAND_DISPLAY,

	/* Insert new types above this comment */

	REG_SCRIPT_COMMAND_END,
};

enum {
	REG_SCRIPT_TYPE_PCI,
	REG_SCRIPT_TYPE_IO,
	REG_SCRIPT_TYPE_MMIO,
	REG_SCRIPT_TYPE_RES,
	REG_SCRIPT_TYPE_IOSF,
	REG_SCRIPT_TYPE_MSR,

	/* Insert other platform independent values above this comment */

	REG_SCRIPT_TYPE_PLATFORM_BASE = 0x10000,
	REG_SCRIPT_TYPE_SOC_BASE = REG_SCRIPT_TYPE_PLATFORM_BASE,
	REG_SCRIPT_TYPE_MAINBOARD_BASE = 0x20000
};

enum {
	REG_SCRIPT_SIZE_8,
	REG_SCRIPT_SIZE_16,
	REG_SCRIPT_SIZE_32,
	REG_SCRIPT_SIZE_64,
};

struct reg_script {
	uint32_t command;
	uint32_t type;
	uint32_t size;
	uint32_t reg;
	uint64_t mask;
	uint64_t value;
	uint32_t timeout;
	union {
		uint32_t id;
		const struct reg_script *next;
#ifdef __SIMPLE_DEVICE__
		pci_devfn_t dev;
#else
		struct device *dev;
#endif
		unsigned int res_index;
	};
};

struct reg_script_context {
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	struct resource *res;
	const struct reg_script *step;
	uint8_t display_state;    /* Only modified by reg_script_run_step */
	uint8_t display_features; /* Step routine modifies to control display */
	const char *display_prefix; /* Prefix tag to display */
};

struct reg_script_bus_entry {
	uint32_t type;
	uint64_t (*reg_script_read)(struct reg_script_context *ctx);
	void (*reg_script_write)(struct reg_script_context *ctx);
};

#define REG_SCRIPT_TABLE_ATTRIBUTE __attribute__((used, section(".rsbe_init")))

#define REG_SCRIPT_BUS_ENTRY(bus_entry_)				\
	const struct reg_script_bus_entry *rsbe_ ## bus_entry_	\
		REG_SCRIPT_TABLE_ATTRIBUTE = &bus_entry_

/* Internal helper Macros. */

#define _REG_SCRIPT_ENCODE_RAW(cmd_, type_, size_, reg_, \
			  mask_, value_, timeout_, id_)  \
	{ .command = cmd_,     \
	  .type = type_,       \
	  .size = size_,       \
	  .reg = reg_,         \
	  .mask = mask_,       \
	  .value = value_,     \
	  .timeout = timeout_, \
	  .id = id_,           \
	}

#define _REG_SCRIPT_ENCODE_RES(cmd_, type_, res_index_, size_, reg_, \
			       mask_, value_, timeout_)  \
	{ .command = cmd_,         \
	  .type = type_,           \
	  .size = size_,           \
	  .reg = reg_,             \
	  .mask = mask_,           \
	  .value = value_,         \
	  .timeout = timeout_,     \
	  .res_index = res_index_, \
	}

/* Display control */
#define REG_SCRIPT_DISPLAY_ALL		0xff
#define REG_SCRIPT_DISPLAY_REGISTER	0x02
#define REG_SCRIPT_DISPLAY_VALUE	0x01
#define REG_SCRIPT_DISPLAY_NOTHING	0

#define REG_SCRIPT_DISPLAY_OFF                   \
	{ .command = REG_SCRIPT_COMMAND_DISPLAY, \
	  .value = REG_SCRIPT_DISPLAY_NOTHING,   \
	}
#define REG_SCRIPT_DISPLAY_ON                    \
	{ .command = REG_SCRIPT_COMMAND_DISPLAY, \
	  .value = REG_SCRIPT_DISPLAY_ALL,       \
	}

/*
 * PCI
 */

#define REG_SCRIPT_PCI(cmd_, bits_, reg_, mask_, value_, timeout_) \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_,          \
			       REG_SCRIPT_TYPE_PCI,                \
			       REG_SCRIPT_SIZE_##bits_,            \
			       reg_, mask_, value_, timeout_, 0)
#define REG_PCI_READ8(reg_) \
	REG_SCRIPT_PCI(READ, 8, reg_, 0, 0, 0)
#define REG_PCI_READ16(reg_) \
	REG_SCRIPT_PCI(READ, 16, reg_, 0, 0, 0)
#define REG_PCI_READ32(reg_) \
	REG_SCRIPT_PCI(READ, 32, reg_, 0, 0, 0)
#define REG_PCI_WRITE8(reg_, value_) \
	REG_SCRIPT_PCI(WRITE, 8, reg_, 0, value_, 0)
#define REG_PCI_WRITE16(reg_, value_) \
	REG_SCRIPT_PCI(WRITE, 16, reg_, 0, value_, 0)
#define REG_PCI_WRITE32(reg_, value_) \
	REG_SCRIPT_PCI(WRITE, 32, reg_, 0, value_, 0)
#define REG_PCI_RMW8(reg_, mask_, value_) \
	REG_SCRIPT_PCI(RMW, 8, reg_, mask_, value_, 0)
#define REG_PCI_RMW16(reg_, mask_, value_) \
	REG_SCRIPT_PCI(RMW, 16, reg_, mask_, value_, 0)
#define REG_PCI_RMW32(reg_, mask_, value_) \
	REG_SCRIPT_PCI(RMW, 32, reg_, mask_, value_, 0)
#define REG_PCI_RXW8(reg_, mask_, value_) \
	REG_SCRIPT_PCI(RXW, 8, reg_, mask_, value_, 0)
#define REG_PCI_RXW16(reg_, mask_, value_) \
	REG_SCRIPT_PCI(RXW, 16, reg_, mask_, value_, 0)
#define REG_PCI_RXW32(reg_, mask_, value_) \
	REG_SCRIPT_PCI(RXW, 32, reg_, mask_, value_, 0)
#define REG_PCI_OR8(reg_, value_) \
	REG_SCRIPT_PCI(RMW, 8, reg_, 0xff, value_, 0)
#define REG_PCI_OR16(reg_, value_) \
	REG_SCRIPT_PCI(RMW, 16, reg_, 0xffff, value_, 0)
#define REG_PCI_OR32(reg_, value_) \
	REG_SCRIPT_PCI(RMW, 32, reg_, 0xffffffff, value_, 0)
#define REG_PCI_POLL8(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_PCI(POLL, 8, reg_, mask_, value_, timeout_)
#define REG_PCI_POLL16(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_PCI(POLL, 16, reg_, mask_, value_, timeout_)
#define REG_PCI_POLL32(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_PCI(POLL, 32, reg_, mask_, value_, timeout_)
#define REG_PCI_XOR8(reg_, value_) \
	REG_SCRIPT_PCI(RXW, 8, reg_, 0xff, value_, 0)
#define REG_PCI_XOR16(reg_, value_) \
	REG_SCRIPT_PCI(RXW, 16, reg_, 0xffff, value_, 0)
#define REG_PCI_XOR32(reg_, value_) \
	REG_SCRIPT_PCI(RXW, 32, reg_, 0xffffffff, value_, 0)

/*
 * Legacy IO
 */

#define REG_SCRIPT_IO(cmd_, bits_, reg_, mask_, value_, timeout_) \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_,         \
			       REG_SCRIPT_TYPE_IO,                \
			       REG_SCRIPT_SIZE_##bits_,           \
			       reg_, mask_, value_, timeout_, 0)
#define REG_IO_READ8(reg_) \
	REG_SCRIPT_IO(READ, 8, reg_, 0, 0, 0)
#define REG_IO_READ16(reg_) \
	REG_SCRIPT_IO(READ, 16, reg_, 0, 0, 0)
#define REG_IO_READ32(reg_) \
	REG_SCRIPT_IO(READ, 32, reg_, 0, 0, 0)
#define REG_IO_WRITE8(reg_, value_) \
	REG_SCRIPT_IO(WRITE, 8, reg_, 0, value_, 0)
#define REG_IO_WRITE16(reg_, value_) \
	REG_SCRIPT_IO(WRITE, 16, reg_, 0, value_, 0)
#define REG_IO_WRITE32(reg_, value_) \
	REG_SCRIPT_IO(WRITE, 32, reg_, 0, value_, 0)
#define REG_IO_RMW8(reg_, mask_, value_) \
	REG_SCRIPT_IO(RMW, 8, reg_, mask_, value_, 0)
#define REG_IO_RMW16(reg_, mask_, value_) \
	REG_SCRIPT_IO(RMW, 16, reg_, mask_, value_, 0)
#define REG_IO_RMW32(reg_, mask_, value_) \
	REG_SCRIPT_IO(RMW, 32, reg_, mask_, value_, 0)
#define REG_IO_RXW8(reg_, mask_, value_) \
	REG_SCRIPT_IO(RXW, 8, reg_, mask_, value_, 0)
#define REG_IO_RXW16(reg_, mask_, value_) \
	REG_SCRIPT_IO(RXW, 16, reg_, mask_, value_, 0)
#define REG_IO_RXW32(reg_, mask_, value_) \
	REG_SCRIPT_IO(RXW, 32, reg_, mask_, value_, 0)
#define REG_IO_OR8(reg_, value_) \
	REG_IO_RMW8(reg_, 0xff, value_)
#define REG_IO_OR16(reg_, value_) \
	REG_IO_RMW16(reg_, 0xffff, value_)
#define REG_IO_OR32(reg_, value_) \
	REG_IO_RMW32(reg_, 0xffffffff, value_)
#define REG_IO_POLL8(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_IO(POLL, 8, reg_, mask_, value_, timeout_)
#define REG_IO_POLL16(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_IO(POLL, 16, reg_, mask_, value_, timeout_)
#define REG_IO_POLL32(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_IO(POLL, 32, reg_, mask_, value_, timeout_)
#define REG_IO_XOR8(reg_, value_) \
	REG_IO_RXW8(reg_, 0xff, value_)
#define REG_IO_XOR16(reg_, value_) \
	REG_IO_RXW16(reg_, 0xffff, value_)
#define REG_IO_XOR32(reg_, value_) \
	REG_IO_RXW32(reg_, 0xffffffff, value_)

/*
 * Memory Mapped IO
 */

#define REG_SCRIPT_MMIO(cmd_, bits_, reg_, mask_, value_, timeout_) \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_,           \
			       REG_SCRIPT_TYPE_MMIO,                \
			       REG_SCRIPT_SIZE_##bits_,             \
			       reg_, mask_, value_, timeout_, 0)
#define REG_MMIO_READ8(reg_) \
	REG_SCRIPT_MMIO(READ, 8, reg_, 0, 0, 0)
#define REG_MMIO_READ16(reg_) \
	REG_SCRIPT_MMIO(READ, 16, reg_, 0, 0, 0)
#define REG_MMIO_READ32(reg_) \
	REG_SCRIPT_MMIO(READ, 32, reg_, 0, 0, 0)
#define REG_MMIO_WRITE8(reg_, value_) \
	REG_SCRIPT_MMIO(WRITE, 8, reg_, 0, value_, 0)
#define REG_MMIO_WRITE16(reg_, value_) \
	REG_SCRIPT_MMIO(WRITE, 16, reg_, 0, value_, 0)
#define REG_MMIO_WRITE32(reg_, value_) \
	REG_SCRIPT_MMIO(WRITE, 32, reg_, 0, value_, 0)
#define REG_MMIO_RMW8(reg_, mask_, value_) \
	REG_SCRIPT_MMIO(RMW, 8, reg_, mask_, value_, 0)
#define REG_MMIO_RMW16(reg_, mask_, value_) \
	REG_SCRIPT_MMIO(RMW, 16, reg_, mask_, value_, 0)
#define REG_MMIO_RMW32(reg_, mask_, value_) \
	REG_SCRIPT_MMIO(RMW, 32, reg_, mask_, value_, 0)
#define REG_MMIO_RXW8(reg_, mask_, value_) \
	REG_SCRIPT_MMIO(RXW, 8, reg_, mask_, value_, 0)
#define REG_MMIO_RXW16(reg_, mask_, value_) \
	REG_SCRIPT_MMIO(RXW, 16, reg_, mask_, value_, 0)
#define REG_MMIO_RXW32(reg_, mask_, value_) \
	REG_SCRIPT_MMIO(RXW, 32, reg_, mask_, value_, 0)
#define REG_MMIO_OR8(reg_, value_) \
	REG_MMIO_RMW8(reg_, 0xff, value_)
#define REG_MMIO_OR16(reg_, value_) \
	REG_MMIO_RMW16(reg_, 0xffff, value_)
#define REG_MMIO_OR32(reg_, value_) \
	REG_MMIO_RMW32(reg_, 0xffffffff, value_)
#define REG_MMIO_POLL8(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_MMIO(POLL, 8, reg_, mask_, value_, timeout_)
#define REG_MMIO_POLL16(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_MMIO(POLL, 16, reg_, mask_, value_, timeout_)
#define REG_MMIO_POLL32(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_MMIO(POLL, 32, reg_, mask_, value_, timeout_)
#define REG_MMIO_XOR8(reg_, value_) \
	REG_MMIO_RXW8(reg_, 0xff, value_)
#define REG_MMIO_XOR16(reg_, value_) \
	REG_MMIO_RXW16(reg_, 0xffff, value_)
#define REG_MMIO_XOR32(reg_, value_) \
	REG_MMIO_RXW32(reg_, 0xffffffff, value_)

/*
 * Access through a device's resource such as a Base Address Register (BAR)
 */

#define REG_SCRIPT_RES(cmd_, bits_, bar_, reg_, mask_, value_, timeout_) \
	_REG_SCRIPT_ENCODE_RES(REG_SCRIPT_COMMAND_##cmd_,                \
			       REG_SCRIPT_TYPE_RES, bar_,                \
			       REG_SCRIPT_SIZE_##bits_,                  \
			       reg_, mask_, value_, timeout_)
#define REG_RES_READ8(bar_, reg_) \
	REG_SCRIPT_RES(READ, 8, bar_, reg_, 0, 0, 0)
#define REG_RES_READ16(bar_, reg_) \
	REG_SCRIPT_RES(READ, 16, bar_, reg_, 0, 0, 0)
#define REG_RES_READ32(bar_, reg_) \
	REG_SCRIPT_RES(READ, 32, bar_, reg_, 0, 0, 0)
#define REG_RES_WRITE8(bar_, reg_, value_) \
	REG_SCRIPT_RES(WRITE, 8, bar_, reg_, 0, value_, 0)
#define REG_RES_WRITE16(bar_, reg_, value_) \
	REG_SCRIPT_RES(WRITE, 16, bar_, reg_, 0, value_, 0)
#define REG_RES_WRITE32(bar_, reg_, value_) \
	REG_SCRIPT_RES(WRITE, 32, bar_, reg_, 0, value_, 0)
#define REG_RES_RMW8(bar_, reg_, mask_, value_) \
	REG_SCRIPT_RES(RMW, 8, bar_, reg_, mask_, value_, 0)
#define REG_RES_RMW16(bar_, reg_, mask_, value_) \
	REG_SCRIPT_RES(RMW, 16, bar_, reg_, mask_, value_, 0)
#define REG_RES_RMW32(bar_, reg_, mask_, value_) \
	REG_SCRIPT_RES(RMW, 32, bar_, reg_, mask_, value_, 0)
#define REG_RES_RXW8(bar_, reg_, mask_, value_) \
	REG_SCRIPT_RES(RXW, 8, bar_, reg_, mask_, value_, 0)
#define REG_RES_RXW16(bar_, reg_, mask_, value_) \
	REG_SCRIPT_RES(RXW, 16, bar_, reg_, mask_, value_, 0)
#define REG_RES_RXW32(bar_, reg_, mask_, value_) \
	REG_SCRIPT_RES(RXW, 32, bar_, reg_, mask_, value_, 0)
#define REG_RES_OR8(bar_, reg_, value_) \
	REG_RES_RMW8(bar_, reg_, 0xff, value_)
#define REG_RES_OR16(bar_, reg_, value_) \
	REG_RES_RMW16(bar_, reg_, 0xffff, value_)
#define REG_RES_OR32(bar_, reg_, value_) \
	REG_RES_RMW32(bar_, reg_, 0xffffffff, value_)
#define REG_RES_POLL8(bar_, reg_, mask_, value_, timeout_) \
	REG_SCRIPT_RES(POLL, 8, bar_, reg_, mask_, value_, timeout_)
#define REG_RES_POLL16(bar_, reg_, mask_, value_, timeout_) \
	REG_SCRIPT_RES(POLL, 16, bar_, reg_, mask_, value_, timeout_)
#define REG_RES_POLL32(bar_, reg_, mask_, value_, timeout_) \
	REG_SCRIPT_RES(POLL, 32, bar_, reg_, mask_, value_, timeout_)
#define REG_RES_XOR8(bar_, reg_, value_) \
	REG_RES_RXW8(bar_, reg_, 0xff, value_)
#define REG_RES_XOR16(bar_, reg_, value_) \
	REG_RES_RXW16(bar_, reg_, 0xffff, value_)
#define REG_RES_XOR32(bar_, reg_, value_) \
	REG_RES_RXW32(bar_, reg_, 0xffffffff, value_)

#if CONFIG(SOC_INTEL_BAYTRAIL)
/*
 * IO Sideband Function
 */

#define REG_SCRIPT_IOSF(cmd_, unit_, reg_, mask_, value_, timeout_) \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_,      \
			       REG_SCRIPT_TYPE_IOSF,           \
			       REG_SCRIPT_SIZE_32,             \
			       reg_, mask_, value_, timeout_, unit_)
#define REG_IOSF_READ(unit_, reg_) \
	REG_SCRIPT_IOSF(READ, unit_, reg_, 0, 0, 0)
#define REG_IOSF_WRITE(unit_, reg_, value_) \
	REG_SCRIPT_IOSF(WRITE, unit_, reg_, 0, value_, 0)
#define REG_IOSF_RMW(unit_, reg_, mask_, value_) \
	REG_SCRIPT_IOSF(RMW, unit_, reg_, mask_, value_, 0)
#define REG_IOSF_RXW(unit_, reg_, mask_, value_) \
	REG_SCRIPT_IOSF(RXW, unit_, reg_, mask_, value_, 0)
#define REG_IOSF_OR(unit_, reg_, value_) \
	REG_IOSF_RMW(unit_, reg_, 0xffffffff, value_)
#define REG_IOSF_POLL(unit_, reg_, mask_, value_, timeout_) \
	REG_SCRIPT_IOSF(POLL, unit_, reg_, mask_, value_, timeout_)
#define REG_IOSF_XOR(unit_, reg_, value_) \
	REG_IOSF_RXW(unit_, reg_, 0xffffffff, value_)
#endif	/* CONFIG_SOC_INTEL_BAYTRAIL */

/*
 * CPU Model Specific Register
 */

#define REG_SCRIPT_MSR(cmd_, reg_, mask_, value_, timeout_) \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_,   \
			       REG_SCRIPT_TYPE_MSR,         \
			       REG_SCRIPT_SIZE_64,          \
			       reg_, mask_, value_, timeout_, 0)
#define REG_MSR_READ(reg_) \
	REG_SCRIPT_MSR(READ, reg_, 0, 0, 0)
#define REG_MSR_WRITE(reg_, value_) \
	REG_SCRIPT_MSR(WRITE, reg_, 0, value_, 0)
#define REG_MSR_RMW(reg_, mask_, value_) \
	REG_SCRIPT_MSR(RMW, reg_, mask_, value_, 0)
#define REG_MSR_RXW(reg_, mask_, value_) \
	REG_SCRIPT_MSR(RXW, reg_, mask_, value_, 0)
#define REG_MSR_OR(reg_, value_) \
	REG_MSR_RMW(reg_, -1ULL, value_)
#define REG_MSR_POLL(reg_, mask_, value_, timeout_) \
	REG_SCRIPT_MSR(POLL, reg_, mask_, value_, timeout_)
#define REG_MSR_XOR(reg_, value_) \
	REG_MSR_RXW(reg_, -1ULL, value_)

/*
 * Chain to another table.
 */
#define REG_SCRIPT_NEXT(next_)                \
	{ .command = REG_SCRIPT_COMMAND_NEXT, \
	  .next = next_,                      \
	}

/*
 * Set current device
 */
#define REG_SCRIPT_SET_DEV(dev_)                 \
	{ .command = REG_SCRIPT_COMMAND_SET_DEV, \
	  .dev = dev_,                           \
	}

/*
 * Last script entry. All tables need to end with REG_SCRIPT_END.
 */
#define REG_SCRIPT_END \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_END, 0, 0, 0, 0, 0, 0, 0)

void reg_script_run(const struct reg_script *script);
#ifdef __SIMPLE_DEVICE__
void reg_script_run_on_dev(pci_devfn_t dev, const struct reg_script *step);
#else
void reg_script_run_on_dev(struct device *dev, const struct reg_script *step);
#endif

#endif /* REG_SCRIPT_H */
