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
#include <device/path.h>
#if IS_ENABLED(CONFIG_GENERIC_GPIO_LIB)
#include <gpio.h>
#endif

#define ACPI_DP_UUID		"daffd814-6eba-4d8c-8a91-bc9bbf4aa301"
#define ACPI_DP_CHILD_UUID	"dbb8e3e6-5886-4ba6-8795-1319f52a966b"

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
	struct device *pdev = dev;
	const char *name = NULL;

	if (!dev)
		return NULL;

	/* Check for device specific handler */
	if (dev->ops->acpi_name)
		return dev->ops->acpi_name(dev);

	/* Walk up the tree to find if any parent can identify this device */
	while (pdev->bus) {
		pdev = pdev->bus->dev;
		if (!pdev)
			break;
		if (pdev->path.type == DEVICE_PATH_ROOT)
			break;
		if (pdev->ops && pdev->ops->acpi_name)
			name = pdev->ops->acpi_name(dev);
		if (name)
			return name;
	}

	return NULL;
}

/* Recursive function to find the root device and print a path from there */
static ssize_t acpi_device_path_fill(struct device *dev, char *buf,
				     size_t buf_len, size_t cur)
{
	const char *name = acpi_device_name(dev);
	ssize_t next = 0;

	if (!name)
		return -1;

	/*
	 * Make sure this name segment will fit, including the path segment
	 * separator and possible NUL terminator if this is the last segment.
	 */
	if (!dev || (cur + strlen(name) + 2) > buf_len)
		return cur;

	/* Walk up the tree to the root device */
	if (dev->path.type != DEVICE_PATH_ROOT && dev->bus && dev->bus->dev)
		next = acpi_device_path_fill(dev->bus->dev, buf, buf_len, cur);
	if (next < 0)
		return next;

	/* Fill in the path from the root device */
	next += snprintf(buf + next, buf_len - next, "%s%s",
			 (dev->path.type == DEVICE_PATH_ROOT
				|| (strlen(name) == 0)) ?
					"" : ".", name);

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
	static char buf[DEVICE_PATH_MAX] = {};

	if (!dev || !dev->bus || !dev->bus->dev)
		return NULL;

	if (acpi_device_path_fill(dev->bus->dev, buf, sizeof(buf), 0) <= 0)
		return NULL;

	return buf;
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

int acpi_device_status(const struct device *dev)
{
	if (!dev->enabled)
		return ACPI_STATUS_DEVICE_ALL_OFF;
	if (dev->hidden)
		return ACPI_STATUS_DEVICE_HIDDEN_ON;
	return ACPI_STATUS_DEVICE_ALL_ON;
}

/* ACPI 6.1 section 6.4.3.6: Extended Interrupt Descriptor */
void acpi_device_write_interrupt(const struct acpi_irq *irq)
{
	void *desc_length;
	uint8_t flags;

	if (!irq || !irq->pin)
		return;

	/* This is supported by GpioInt() but not Interrupt() */
	if (irq->polarity == ACPI_IRQ_ACTIVE_BOTH)
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
	if (irq->mode == ACPI_IRQ_EDGE_TRIGGERED)
		flags |= 1 << 1;
	if (irq->polarity == ACPI_IRQ_ACTIVE_LOW)
		flags |= 1 << 2;
	if (irq->shared == ACPI_IRQ_SHARED)
		flags |= 1 << 3;
	if (irq->wake == ACPI_IRQ_WAKE)
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
		if (gpio->irq.mode == ACPI_IRQ_EDGE_TRIGGERED)
			flags |= 1 << 0;
		if (gpio->irq.shared == ACPI_IRQ_SHARED)
			flags |= 1 << 3;
		if (gpio->irq.wake == ACPI_IRQ_WAKE)
			flags |= 1 << 4;

		switch (gpio->irq.polarity) {
		case ACPI_IRQ_ACTIVE_HIGH:
			flags |= 0 << 1;
			break;
		case ACPI_IRQ_ACTIVE_LOW:
			flags |= 1 << 1;
			break;
		case ACPI_IRQ_ACTIVE_BOTH:
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
	for (pin = 0; pin < gpio->pin_count; pin++) {
		uint16_t acpi_pin = gpio->pins[pin];
#if IS_ENABLED(CONFIG_GENERIC_GPIO_LIB)
		acpi_pin = gpio_acpi_pin(acpi_pin);
#endif
		acpigen_emit_word(acpi_pin);
	}

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

/* ACPI 6.1 section 6.4.3.8.2.2 - SpiSerialBus() */
void acpi_device_write_spi(const struct acpi_spi *spi)
{
	void *desc_length, *type_length;
	uint16_t flags = 0;

	/* Byte 0: Descriptor Type */
	acpigen_emit_byte(ACPI_DESCRIPTOR_SERIAL_BUS);

	/* Byte 1+2: Length (filled in later) */
	desc_length = acpi_device_write_zero_len();

	/* Byte 3: Revision ID */
	acpigen_emit_byte(ACPI_SERIAL_BUS_REVISION_ID);

	/* Byte 4: Resource Source Index is Reserved */
	acpigen_emit_byte(0);

	/* Byte 5: Serial Bus Type is SPI */
	acpigen_emit_byte(ACPI_SERIAL_BUS_TYPE_SPI);

	/*
	 * Byte 6: Flags
	 *  [7:2]: 0 => Reserved
	 *    [1]: 1 => ResourceConsumer
	 *    [0]: 0 => ControllerInitiated
	 */
	acpigen_emit_byte(1 << 1);

	/*
	 * Byte 7-8: Type Specific Flags
	 *   [15:2]: 0 => Reserved
	 *      [1]: 0 => ActiveLow, 1 => ActiveHigh
	 *      [0]: 0 => FourWire, 1 => ThreeWire
	 */
	if (spi->wire_mode == SPI_3_WIRE_MODE)
		flags |= 1 << 0;
	if (spi->device_select_polarity == SPI_POLARITY_HIGH)
		flags |= 1 << 1;
	acpigen_emit_word(flags);

	/* Byte 9: Type Specific Revision ID */
	acpigen_emit_byte(ACPI_SERIAL_BUS_REVISION_ID);

	/* Byte 10-11: SPI Type Data Length */
	type_length = acpi_device_write_zero_len();

	/* Byte 12-15: Connection Speed */
	acpigen_emit_dword(spi->speed);

	/* Byte 16: Data Bit Length */
	acpigen_emit_byte(spi->data_bit_length);

	/* Byte 17: Clock Phase */
	acpigen_emit_byte(spi->clock_phase);

	/* Byte 18: Clock Polarity */
	acpigen_emit_byte(spi->clock_polarity);

	/* Byte 19-20: Device Selection */
	acpigen_emit_word(spi->device_select);

	/* Fill in Type Data Length */
	acpi_device_fill_len(type_length);

	/* Byte 21+: ResourceSource String */
	acpigen_emit_string(spi->resource);

	/* Fill in SPI Descriptor Length */
	acpi_device_fill_len(desc_length);
}

/* PowerResource() with Enable and/or Reset control */
void acpi_device_add_power_res(const struct acpi_power_res_params *params)
{
	static const char *power_res_dev_states[] = { "_PR0", "_PR3" };
	unsigned int reset_gpio = params->reset_gpio->pins[0];
	unsigned int enable_gpio = params->enable_gpio->pins[0];
	unsigned int stop_gpio = params->stop_gpio->pins[0];

	if (!reset_gpio && !enable_gpio && !stop_gpio)
		return;

	/* PowerResource (PRIC, 0, 0) */
	acpigen_write_power_res("PRIC", 0, 0, power_res_dev_states,
				ARRAY_SIZE(power_res_dev_states));

	/* Method (_STA, 0, NotSerialized) { Return (0x1) } */
	acpigen_write_STA(0x1);

	/* Method (_ON, 0, Serialized) */
	acpigen_write_method_serialized("_ON", 0);
	if (reset_gpio)
		acpigen_enable_tx_gpio(params->reset_gpio);
	if (enable_gpio) {
		acpigen_enable_tx_gpio(params->enable_gpio);
		if (params->enable_delay_ms)
			acpigen_write_sleep(params->enable_delay_ms);
	}
	if (reset_gpio) {
		acpigen_disable_tx_gpio(params->reset_gpio);
		if (params->reset_delay_ms)
			acpigen_write_sleep(params->reset_delay_ms);
	}
	if (stop_gpio) {
		acpigen_disable_tx_gpio(params->stop_gpio);
		if (params->stop_delay_ms)
			acpigen_write_sleep(params->stop_delay_ms);
	}
	acpigen_pop_len();		/* _ON method */

	/* Method (_OFF, 0, Serialized) */
	acpigen_write_method_serialized("_OFF", 0);
	if (stop_gpio) {
		acpigen_enable_tx_gpio(params->stop_gpio);
		if (params->stop_off_delay_ms)
			acpigen_write_sleep(params->stop_off_delay_ms);
	}
	if (reset_gpio) {
		acpigen_enable_tx_gpio(params->reset_gpio);
		if (params->reset_off_delay_ms)
			acpigen_write_sleep(params->reset_off_delay_ms);
	}
	if (enable_gpio) {
		acpigen_disable_tx_gpio(params->enable_gpio);
		if (params->enable_off_delay_ms)
			acpigen_write_sleep(params->enable_off_delay_ms);
	}
	acpigen_pop_len();		/* _OFF method */

	acpigen_pop_len();		/* PowerResource PRIC */
}

static void acpi_dp_write_array(const struct acpi_dp *array);
static void acpi_dp_write_value(const struct acpi_dp *prop)
{
	switch (prop->type) {
	case ACPI_DP_TYPE_INTEGER:
		acpigen_write_integer(prop->integer);
		break;
	case ACPI_DP_TYPE_STRING:
	case ACPI_DP_TYPE_CHILD:
		acpigen_write_string(prop->string);
		break;
	case ACPI_DP_TYPE_REFERENCE:
		acpigen_emit_namestring(prop->string);
		break;
	case ACPI_DP_TYPE_ARRAY:
		acpi_dp_write_array(prop->array);
		break;
	default:
		break;
	}
}

/* Package (2) { "prop->name", VALUE } */
static void acpi_dp_write_property(const struct acpi_dp *prop)
{
	acpigen_write_package(2);
	acpigen_write_string(prop->name);
	acpi_dp_write_value(prop);
	acpigen_pop_len();
}

/* Write array of Device Properties */
static void acpi_dp_write_array(const struct acpi_dp *array)
{
	const struct acpi_dp *dp;
	char *pkg_count;

	/* Package element count determined as it is populated */
	pkg_count = acpigen_write_package(0);

	/*
	 * Only acpi_dp of type DP_TYPE_TABLE is allowed to be an array.
	 * DP_TYPE_TABLE does not have a value to be written. Thus, start
	 * the loop from next type in the array.
	 */
	for (dp = array->next; dp; dp = dp->next) {
		acpi_dp_write_value(dp);
		(*pkg_count)++;
	}

	acpigen_pop_len();
}

static void acpi_dp_free(struct acpi_dp *dp)
{
	while (dp) {
		struct acpi_dp *p = dp->next;

		switch (dp->type) {
		case ACPI_DP_TYPE_CHILD:
			acpi_dp_free(dp->child);
			break;
		case ACPI_DP_TYPE_ARRAY:
			acpi_dp_free(dp->array);
			break;
		default:
			break;
		}

		free(dp);
		dp = p;
	}
}

void acpi_dp_write(struct acpi_dp *table)
{
	struct acpi_dp *dp, *prop;
	char *dp_count, *prop_count;
	int child_count = 0;

	if (!table || table->type != ACPI_DP_TYPE_TABLE)
		return;

	/* Name (name) */
	acpigen_write_name(table->name);

	/* Device Property list starts with the next entry */
	prop = table->next;

	/* Package (DP), default to 2 elements (assuming no children) */
	dp_count = acpigen_write_package(2);

	/* ToUUID (ACPI_DP_UUID) */
	acpigen_write_uuid(ACPI_DP_UUID);

	/* Package (PROP), element count determined as it is populated */
	prop_count = acpigen_write_package(0);

	/* Print base properties */
	for (dp = prop; dp; dp = dp->next) {
		if (dp->type == ACPI_DP_TYPE_CHILD) {
			child_count++;
		} else {
			(*prop_count)++;
			acpi_dp_write_property(dp);
		}
	}

	/* Package (PROP) length */
	acpigen_pop_len();

	if (child_count) {
		/* Update DP package count to 4 */
		*dp_count = 4;

		/* ToUUID (ACPI_DP_CHILD_UUID) */
		acpigen_write_uuid(ACPI_DP_CHILD_UUID);

		/* Print child pointer properties */
		acpigen_write_package(child_count);

		for (dp = prop; dp; dp = dp->next)
			if (dp->type == ACPI_DP_TYPE_CHILD)
				acpi_dp_write_property(dp);

		acpigen_pop_len();
	}

	/* Package (DP) length */
	acpigen_pop_len();

	/* Recursively parse children into separate tables */
	for (dp = prop; dp; dp = dp->next)
		if (dp->type == ACPI_DP_TYPE_CHILD)
			acpi_dp_write(dp->child);

	/* Clean up */
	acpi_dp_free(table);
}

static struct acpi_dp *acpi_dp_new(struct acpi_dp *dp, enum acpi_dp_type type,
				   const char *name)
{
	struct acpi_dp *new;

	new = malloc(sizeof(struct acpi_dp));
	if (!new)
		return NULL;

	memset(new, 0, sizeof(*new));
	new->type = type;
	new->name = name;

	if (dp) {
		/* Add to end of property list */
		while (dp->next)
			dp = dp->next;
		dp->next = new;
	}

	return new;
}

struct acpi_dp *acpi_dp_new_table(const char *name)
{
	return acpi_dp_new(NULL, ACPI_DP_TYPE_TABLE, name);
}

size_t acpi_dp_add_property_list(struct acpi_dp *dp,
				 const struct acpi_dp *property_list,
				 size_t property_count)
{
	const struct acpi_dp *prop;
	size_t i, properties_added = 0;

	for (i = 0; i < property_count; i++) {
		prop = &property_list[i];

		if (prop->type == ACPI_DP_TYPE_UNKNOWN || !prop->name)
			continue;

		switch (prop->type) {
		case ACPI_DP_TYPE_INTEGER:
			acpi_dp_add_integer(dp, prop->name, prop->integer);
			break;
		case ACPI_DP_TYPE_STRING:
			acpi_dp_add_string(dp, prop->name, prop->string);
			break;
		case ACPI_DP_TYPE_REFERENCE:
			acpi_dp_add_reference(dp, prop->name, prop->string);
			break;
		case ACPI_DP_TYPE_ARRAY:
			acpi_dp_add_array(dp, prop->array);
			break;
		case ACPI_DP_TYPE_CHILD:
			acpi_dp_add_child(dp, prop->name, prop->child);
			break;
		default:
			continue;
		}

		++properties_added;
	}

	return properties_added;
}

struct acpi_dp *acpi_dp_add_integer(struct acpi_dp *dp, const char *name,
				    uint64_t value)
{
	struct acpi_dp *new = acpi_dp_new(dp, ACPI_DP_TYPE_INTEGER, name);

	if (new)
		new->integer = value;

	return new;
}

struct acpi_dp *acpi_dp_add_string(struct acpi_dp *dp, const char *name,
				   const char *string)
{
	struct acpi_dp *new = acpi_dp_new(dp, ACPI_DP_TYPE_STRING, name);

	if (new)
		new->string = string;

	return new;
}

struct acpi_dp *acpi_dp_add_reference(struct acpi_dp *dp, const char *name,
				      const char *reference)
{
	struct acpi_dp *new = acpi_dp_new(dp, ACPI_DP_TYPE_REFERENCE, name);

	if (new)
		new->string = reference;

	return new;
}

struct acpi_dp *acpi_dp_add_child(struct acpi_dp *dp, const char *name,
				  struct acpi_dp *child)
{
	struct acpi_dp *new;

	if (!child || child->type != ACPI_DP_TYPE_TABLE)
		return NULL;

	new = acpi_dp_new(dp, ACPI_DP_TYPE_CHILD, name);
	if (new) {
		new->child = child;
		new->string = child->name;
	}

	return new;
}

struct acpi_dp *acpi_dp_add_array(struct acpi_dp *dp, struct acpi_dp *array)
{
	struct acpi_dp *new;

	if (!array || array->type != ACPI_DP_TYPE_TABLE)
		return NULL;

	new = acpi_dp_new(dp, ACPI_DP_TYPE_ARRAY, array->name);
	if (new)
		new->array = array;

	return new;
}

struct acpi_dp *acpi_dp_add_integer_array(struct acpi_dp *dp, const char *name,
					  uint64_t *array, int len)
{
	struct acpi_dp *dp_array;
	int i;

	if (len <= 0)
		return NULL;

	dp_array = acpi_dp_new_table(name);
	if (!dp_array)
		return NULL;

	for (i = 0; i < len; i++)
		if (!acpi_dp_add_integer(dp_array, NULL, array[i]))
			break;

	acpi_dp_add_array(dp, dp_array);

	return dp_array;
}

struct acpi_dp *acpi_dp_add_gpio(struct acpi_dp *dp, const char *name,
				 const char *ref, int index, int pin,
				 int active_low)
{
	struct acpi_dp *gpio = acpi_dp_new_table(name);

	if (!gpio)
		return NULL;

	/* The device that has _CRS containing GpioIO()/GpioInt() */
	acpi_dp_add_reference(gpio, NULL, ref);

	/* Index of the GPIO resource in _CRS starting from zero */
	acpi_dp_add_integer(gpio, NULL, index);

	/* Pin in the GPIO resource, typically zero */
	acpi_dp_add_integer(gpio, NULL, pin);

	/* Set if pin is active low */
	acpi_dp_add_integer(gpio, NULL, active_low);

	acpi_dp_add_array(dp, gpio);

	return gpio;
}
