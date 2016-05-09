/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ACPI_DEVICE_H
#define __ACPI_DEVICE_H

#define ACPI_DESCRIPTOR_LARGE		(1 << 7)
#define ACPI_DESCRIPTOR_INTERRUPT	(ACPI_DESCRIPTOR_LARGE | 9)

struct device;
const char *acpi_device_name(struct device *dev);
const char *acpi_device_path(struct device *dev);
const char *acpi_device_scope(struct device *dev);
const char *acpi_device_path_join(struct device *dev, const char *name);

/*
 * ACPI Descriptor for extended Interrupt()
 */

enum irq_mode {
	IRQ_EDGE_TRIGGERED,
	IRQ_LEVEL_TRIGGERED
};

enum irq_polarity {
	IRQ_ACTIVE_LOW,
	IRQ_ACTIVE_HIGH,
	IRQ_ACTIVE_BOTH
};

enum irq_shared {
	IRQ_EXCLUSIVE,
	IRQ_SHARED
};

enum irq_wake {
	IRQ_NO_WAKE,
	IRQ_WAKE
};

struct acpi_irq {
	unsigned int pin;
	enum irq_mode mode;
	enum irq_polarity polarity;
	enum irq_shared shared;
	enum irq_wake wake;
};

#define IRQ_EDGE_LOW(x) { \
	.pin = (x), \
	.mode = IRQ_EDGE_TRIGGERED, \
	.polarity = IRQ_ACTIVE_LOW, \
	.shared = IRQ_EXCLUSIVE, \
	.wake = IRQ_NO_WAKE }

#define IRQ_EDGE_HIGH(x) { \
	.pin = (x), \
	.mode = IRQ_EDGE_TRIGGERED, \
	.polarity = IRQ_ACTIVE_HIGH, \
	.shared = IRQ_EXCLUSIVE, \
	.wake = IRQ_NO_WAKE }

#define IRQ_LEVEL_LOW(x) { \
	.pin = (x), \
	.mode = IRQ_LEVEL_TRIGGERED, \
	.polarity = IRQ_ACTIVE_LOW, \
	.shared = IRQ_SHARED, \
	.wake = IRQ_NO_WAKE }

/* Write extended Interrupt() descriptor to SSDT AML output */
void acpi_device_write_interrupt(const struct acpi_irq *irq);

#endif
