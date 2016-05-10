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

#include <string.h>
#include <arch/acpi.h>
#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/path.h>
#if IS_ENABLED(CONFIG_GENERIC_GPIO_LIB)
#include <gpio.h>
#endif

/* Write empty word value and return pointer to it */
static void *acpi_device_write_zero_len(void)
{
	char *p = acpigen_get_current();
	acpigen_emit_word(0);
	return p;
}

/* Fill in length value from start to current at specified location */
static void acpi_device_fill_from_len(char *ptr, char *start)
{
	uint16_t len = acpigen_get_current() - start;
	ptr[0] = len & 0xff;
	ptr[1] = (len >> 8) & 0xff;
}

/*
 * Fill in the length field with the value calculated from after
 * the 16bit field to acpigen current as this length value does
 * not include the length field itself.
 */
static void acpi_device_fill_len(void *ptr)
{
	acpi_device_fill_from_len(ptr, ptr + sizeof(uint16_t));
}

/* Locate and return the ACPI name for this device */
const char *acpi_device_name(struct device *dev)
{
	if (!dev)
		return NULL;

	/* Check for device specific handler */
	if (dev->ops->acpi_name)
		return dev->ops->acpi_name(dev);

	/* Check parent device in case it has a global handler */
	if (dev->bus && dev->bus->dev->ops->acpi_name)
		return dev->bus->dev->ops->acpi_name(dev);

	return NULL;
}

/* Recursive function to find the root device and print a path from there */
static size_t acpi_device_path_fill(struct device *dev, char *buf,
				    size_t buf_len, size_t cur)
{
	const char *name = acpi_device_name(dev);
	size_t next = 0;

	/*
	 * Make sure this name segment will fit, including the path segment
	 * separator and possible NUL terminator if this is the last segment.
	 */
	if (!dev || !name || (cur + strlen(name) + 2) > buf_len)
		return cur;

	/* Walk up the tree to the root device */
	if (dev->path.type != DEVICE_PATH_ROOT && dev->bus && dev->bus->dev)
		next = acpi_device_path_fill(dev->bus->dev, buf, buf_len, cur);

	/* Fill in the path from the root device */
	next += snprintf(buf + next, buf_len - next, "%s%s",
			 dev->path.type == DEVICE_PATH_ROOT ? "" : ".", name);

	return next;
}

/*
 * Warning: just as with dev_path() this uses a static buffer
 * so should not be called mulitple times in one statement
 */
const char *acpi_device_path(struct device *dev)
{
	static char buf[DEVICE_PATH_MAX] = {};

	if (!dev)
		return NULL;

	if (acpi_device_path_fill(dev, buf, sizeof(buf), 0) <= 0)
		return NULL;

	return buf;
}

/* Return the path of the parent device as the ACPI Scope for this device */
const char *acpi_device_scope(struct device *dev)
{
	if (!dev || !dev->bus || !dev->bus->dev)
		return NULL;

	return acpi_device_path(dev->bus->dev);
}

/* Concatentate the device path and provided name suffix */
const char *acpi_device_path_join(struct device *dev, const char *name)
{
	static char buf[DEVICE_PATH_MAX] = {};
	size_t len;

	if (!dev)
		return NULL;

	/* Build the path of this device */
	len = acpi_device_path_fill(dev, buf, sizeof(buf), 0);
	if (len <= 0)
		return NULL;

	/* Ensure there is room for the added name, separator, and NUL */
	if ((len + strlen(name) + 2) > sizeof(buf))
		return NULL;
	snprintf(buf + len, sizeof(buf) - len, ".%s", name);

	return buf;
}

/* ACPI 6.1 section 6.4.3.6: Extended Interrupt Descriptor */
void acpi_device_write_interrupt(const struct acpi_irq *irq)
{
	void *desc_length;
	uint8_t flags;

	if (!irq || !irq->pin)
		return;

	/* This is supported by GpioInt() but not Interrupt() */
	if (irq->polarity == IRQ_ACTIVE_BOTH)
		return;

	/* Byte 0: Descriptor Type */
	acpigen_emit_byte(ACPI_DESCRIPTOR_INTERRUPT);

	/* Byte 1-2: Length (filled in later) */
	desc_length = acpi_device_write_zero_len();

	/*
	 * Byte 3: Flags
	 *  [7:5]: Reserved
	 *    [4]: Wake     (0=NO_WAKE   1=WAKE)
	 *    [3]: Sharing  (0=EXCLUSIVE 1=SHARED)
	 *    [2]: Polarity (0=HIGH      1=LOW)
	 *    [1]: Mode     (0=LEVEL     1=EDGE)
	 *    [0]: Resource (0=PRODUCER  1=CONSUMER)
	 */
	flags = 1 << 0; /* ResourceConsumer */
	if (irq->mode == IRQ_EDGE_TRIGGERED)
		flags |= 1 << 1;
	if (irq->polarity == IRQ_ACTIVE_LOW)
		flags |= 1 << 2;
	if (irq->shared == IRQ_SHARED)
		flags |= 1 << 3;
	if (irq->wake == IRQ_WAKE)
		flags |= 1 << 4;
	acpigen_emit_byte(flags);

	/* Byte 4: Interrupt Table Entry Count */
	acpigen_emit_byte(1);

	/* Byte 5-8: Interrupt Number */
	acpigen_emit_dword(irq->pin);

	/* Fill in Descriptor Length (account for len word) */
	acpi_device_fill_len(desc_length);
}

/* ACPI 6.1 section 6.4.3.8.1 - GPIO Interrupt or I/O */
void acpi_device_write_gpio(const struct acpi_gpio *gpio)
{
	void *start, *desc_length;
	void *pin_table_offset, *vendor_data_offset, *resource_offset;
	uint16_t flags = 0;
	int pin;

	if (!gpio || gpio->type > ACPI_GPIO_TYPE_IO)
		return;

	start = acpigen_get_current();

	/* Byte 0: Descriptor Type */
	acpigen_emit_byte(ACPI_DESCRIPTOR_GPIO);

	/* Byte 1-2: Length (fill in later) */
	desc_length = acpi_device_write_zero_len();

	/* Byte 3: Revision ID */
	acpigen_emit_byte(ACPI_GPIO_REVISION_ID);

	/* Byte 4: GpioIo or GpioInt */
	acpigen_emit_byte(gpio->type);

	/*
	 * Byte 5-6: General Flags
	 *   [15:1]: 0 => Reserved
	 *      [0]: 1 => ResourceConsumer
	 */
	acpigen_emit_word(1 << 0);

	switch (gpio->type) {
	case ACPI_GPIO_TYPE_INTERRUPT:
		/*
		 * Byte 7-8: GPIO Interrupt Flags
		 *   [15:5]: 0 => Reserved
		 *      [4]: Wake     (0=NO_WAKE   1=WAKE)
		 *      [3]: Sharing  (0=EXCLUSIVE 1=SHARED)
		 *    [2:1]: Polarity (0=HIGH      1=LOW     2=BOTH)
		 *      [0]: Mode     (0=LEVEL     1=EDGE)
		 */
		if (gpio->irq.mode == IRQ_EDGE_TRIGGERED)
			flags |= 1 << 0;
		if (gpio->irq.shared == IRQ_SHARED)
			flags |= 1 << 3;
		if (gpio->irq.wake == IRQ_WAKE)
			flags |= 1 << 4;

		switch (gpio->irq.polarity) {
		case IRQ_ACTIVE_HIGH:
			flags |= 0 << 1;
			break;
		case IRQ_ACTIVE_LOW:
			flags |= 1 << 1;
			break;
		case IRQ_ACTIVE_BOTH:
			flags |= 2 << 1;
			break;
		}
		break;

	case ACPI_GPIO_TYPE_IO:
		/*
		 * Byte 7-8: GPIO IO Flags
		 *   [15:4]: 0 => Reserved
		 *      [3]: Sharing  (0=EXCLUSIVE 1=SHARED)
		 *      [2]: 0 => Reserved
		 *    [1:0]: IO Restriction
		 *           0 => IoRestrictionNone
		 *           1 => IoRestrictionInputOnly
		 *           2 => IoRestrictionOutputOnly
		 *           3 => IoRestrictionNoneAndPreserve
		 */
		flags |= gpio->io_restrict & 3;
		if (gpio->io_shared)
			flags |= 1 << 3;
		break;
	}
	acpigen_emit_word(flags);

	/*
	 * Byte 9: Pin Configuration
	 *  0x01 => Default (no configuration applied)
	 *  0x02 => Pull-up
	 *  0x03 => Pull-down
	 *  0x04-0x7F => Reserved
	 *  0x80-0xff => Vendor defined
	 */
	acpigen_emit_byte(gpio->pull);

	/* Byte 10-11: Output Drive Strength in 1/100 mA */
	acpigen_emit_word(gpio->output_drive_strength);

	/* Byte 12-13: Debounce Timeout in 1/100 ms */
	acpigen_emit_word(gpio->interrupt_debounce_timeout);

	/* Byte 14-15: Pin Table Offset, relative to start */
	pin_table_offset = acpi_device_write_zero_len();

	/* Byte 16: Reserved */
	acpigen_emit_byte(0);

	/* Byte 17-18: Resource Source Name Offset, relative to start */
	resource_offset = acpi_device_write_zero_len();

	/* Byte 19-20: Vendor Data Offset, relative to start */
	vendor_data_offset = acpi_device_write_zero_len();

	/* Byte 21-22: Vendor Data Length */
	acpigen_emit_word(0);

	/* Fill in Pin Table Offset */
	acpi_device_fill_from_len(pin_table_offset, start);

	/* Pin Table, one word for each pin */
	for (pin = 0; pin < gpio->pin_count; pin++)
		acpigen_emit_word(gpio->pins[pin]);

	/* Fill in Resource Source Name Offset */
	acpi_device_fill_from_len(resource_offset, start);

	/* Resource Source Name String */
#if IS_ENABLED(CONFIG_GENERIC_GPIO_LIB)
	acpigen_emit_string(gpio->resource ? : gpio_acpi_path(gpio->pins[0]));
#else
	acpigen_emit_string(gpio->resource);
#endif

	/* Fill in Vendor Data Offset */
	acpi_device_fill_from_len(vendor_data_offset, start);

	/* Fill in GPIO Descriptor Length (account for len word) */
	acpi_device_fill_len(desc_length);
}

/* ACPI 6.1 section 6.4.3.8.2.1 - I2cSerialBus() */
void acpi_device_write_i2c(const struct acpi_i2c *i2c)
{
	void *desc_length, *type_length;

	/* Byte 0: Descriptor Type */
	acpigen_emit_byte(ACPI_DESCRIPTOR_SERIAL_BUS);

	/* Byte 1+2: Length (filled in later) */
	desc_length = acpi_device_write_zero_len();

	/* Byte 3: Revision ID */
	acpigen_emit_byte(ACPI_SERIAL_BUS_REVISION_ID);

	/* Byte 4: Resource Source Index is Reserved */
	acpigen_emit_byte(0);

	/* Byte 5: Serial Bus Type is I2C */
	acpigen_emit_byte(ACPI_SERIAL_BUS_TYPE_I2C);

	/*
	 * Byte 6: Flags
	 *  [7:2]: 0 => Reserved
	 *    [1]: 1 => ResourceConsumer
	 *    [0]: 0 => ControllerInitiated
	 */
	acpigen_emit_byte(1 << 1);

	/*
	 * Byte 7-8: Type Specific Flags
	 *   [15:1]: 0 => Reserved
	 *      [0]: 0 => 7bit, 1 => 10bit
	 */
	acpigen_emit_word(i2c->mode_10bit);

	/* Byte 9: Type Specific Revision ID */
	acpigen_emit_byte(ACPI_SERIAL_BUS_REVISION_ID);

	/* Byte 10-11: I2C Type Data Length */
	type_length = acpi_device_write_zero_len();

	/* Byte 12-15: I2C Bus Speed */
	acpigen_emit_dword(i2c->speed);

	/* Byte 16-17: I2C Slave Address */
	acpigen_emit_word(i2c->address);

	/* Fill in Type Data Length */
	acpi_device_fill_len(type_length);

	/* Byte 18+: ResourceSource */
	acpigen_emit_string(i2c->resource);

	/* Fill in I2C Descriptor Length */
	acpi_device_fill_len(desc_length);
}
