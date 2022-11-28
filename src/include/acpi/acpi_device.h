/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_ACPI_DEVICE_H__
#define __ACPI_ACPI_DEVICE_H__

#include <device/i2c.h>
#include <spi-generic.h>
#include <types.h>

enum acpi_dp_type {
	ACPI_DP_TYPE_UNKNOWN,
	ACPI_DP_TYPE_INTEGER,
	ACPI_DP_TYPE_STRING,
	ACPI_DP_TYPE_REFERENCE,
	ACPI_DP_TYPE_TABLE,
	ACPI_DP_TYPE_ARRAY,
	ACPI_DP_TYPE_CHILD,
	ACPI_DP_TYPE_PACKAGE,
};

struct acpi_dp {
	enum acpi_dp_type type;
	const char *name;
	const char *uuid;
	struct acpi_dp *next;
	union {
		struct acpi_dp *child;
		struct acpi_dp *array;
	};
	union {
		uint64_t integer;
		const char *string;
	};
};

#define ACPI_DESCRIPTOR_LARGE		(1 << 7)
#define ACPI_DESCRIPTOR_INTERRUPT	(ACPI_DESCRIPTOR_LARGE | 9)
#define ACPI_DESCRIPTOR_GPIO		(ACPI_DESCRIPTOR_LARGE | 12)
#define ACPI_DESCRIPTOR_SERIAL_BUS	(ACPI_DESCRIPTOR_LARGE | 14)

/*
 * PRP0001 is a special DT namespace link device ID. It provides a means to use
 * existing DT-compatible device identification in ACPI. When this _HID is used
 * by an ACPI device, the ACPI subsystem in OS looks up "compatible" property in
 * device object's _DSD and will use the value of that property to identify the
 * corresponding device in analogy with the original DT device identification
 * algorithm.
 * More details can be found in Linux kernel documentation:
 * Documentation/acpi/enumeration.txt
 */
#define ACPI_DT_NAMESPACE_HID		"PRP0001"

struct device;
const char *acpi_device_name(const struct device *dev);
const char *acpi_device_hid(const struct device *dev);
uint32_t acpi_device_uid(const struct device *dev);
const char *acpi_device_path(const struct device *dev);
const char *acpi_device_scope(const struct device *dev);
const char *acpi_device_path_join(const struct device *dev, const char *name);
int acpi_device_status(const struct device *dev);
void acpi_device_write_uid(const struct device *dev);

/*
 * ACPI Descriptor for extended Interrupt()
 */

enum acpi_irq_mode {
	ACPI_IRQ_EDGE_TRIGGERED,
	ACPI_IRQ_LEVEL_TRIGGERED
};

enum acpi_irq_polarity {
	ACPI_IRQ_ACTIVE_LOW,
	ACPI_IRQ_ACTIVE_HIGH,
	ACPI_IRQ_ACTIVE_BOTH
};

enum acpi_irq_shared {
	ACPI_IRQ_EXCLUSIVE,
	ACPI_IRQ_SHARED
};

enum acpi_irq_wake {
	ACPI_IRQ_NO_WAKE,
	ACPI_IRQ_WAKE
};

struct acpi_irq {
	unsigned int pin;
	enum acpi_irq_mode mode;
	enum acpi_irq_polarity polarity;
	enum acpi_irq_shared shared;
	enum acpi_irq_wake wake;
};

#define ACPI_IRQ_CFG(_pin, _mode, _pol, _shared, _wake) { \
	.pin = (_pin),						\
	.mode = (_mode),			\
	.polarity = (_pol),			\
	.shared = (_shared),	\
	.wake = (_wake) }

#define ACPI_IRQ_EDGE_LOW(x)	\
	ACPI_IRQ_CFG((x), ACPI_IRQ_EDGE_TRIGGERED, ACPI_IRQ_ACTIVE_LOW, \
		     ACPI_IRQ_EXCLUSIVE, ACPI_IRQ_NO_WAKE)

#define ACPI_IRQ_EDGE_HIGH(x)	\
	ACPI_IRQ_CFG((x), ACPI_IRQ_EDGE_TRIGGERED, ACPI_IRQ_ACTIVE_HIGH, \
		     ACPI_IRQ_EXCLUSIVE, ACPI_IRQ_NO_WAKE)

#define ACPI_IRQ_LEVEL_LOW(x)	\
	ACPI_IRQ_CFG((x), ACPI_IRQ_LEVEL_TRIGGERED, ACPI_IRQ_ACTIVE_LOW, \
		     ACPI_IRQ_SHARED, ACPI_IRQ_NO_WAKE)

#define ACPI_IRQ_LEVEL_HIGH(x)	\
	ACPI_IRQ_CFG((x), ACPI_IRQ_LEVEL_TRIGGERED, ACPI_IRQ_ACTIVE_HIGH, \
		     ACPI_IRQ_SHARED, ACPI_IRQ_NO_WAKE)

#define ACPI_IRQ_WAKE_EDGE_LOW(x)	\
	ACPI_IRQ_CFG((x), ACPI_IRQ_EDGE_TRIGGERED, ACPI_IRQ_ACTIVE_LOW, \
		     ACPI_IRQ_EXCLUSIVE, ACPI_IRQ_WAKE)

#define ACPI_IRQ_WAKE_EDGE_HIGH(x)	\
	ACPI_IRQ_CFG((x), ACPI_IRQ_EDGE_TRIGGERED, ACPI_IRQ_ACTIVE_HIGH, \
		     ACPI_IRQ_EXCLUSIVE, ACPI_IRQ_WAKE)

#define ACPI_IRQ_WAKE_LEVEL_LOW(x)	\
	ACPI_IRQ_CFG((x), ACPI_IRQ_LEVEL_TRIGGERED, ACPI_IRQ_ACTIVE_LOW, \
		     ACPI_IRQ_SHARED, ACPI_IRQ_WAKE)

#define ACPI_IRQ_WAKE_LEVEL_HIGH(x)					\
	ACPI_IRQ_CFG((x), ACPI_IRQ_LEVEL_TRIGGERED, ACPI_IRQ_ACTIVE_HIGH, \
		     ACPI_IRQ_SHARED, ACPI_IRQ_WAKE)

/* Write extended Interrupt() descriptor to SSDT AML output */
void acpi_device_write_interrupt(const struct acpi_irq *irq);

/*
 * ACPI Descriptors for GpioIo() and GpioInterrupt()
 */

enum acpi_gpio_type {
	ACPI_GPIO_TYPE_INTERRUPT,
	ACPI_GPIO_TYPE_IO
};

enum acpi_gpio_pull {
	ACPI_GPIO_PULL_DEFAULT,
	ACPI_GPIO_PULL_UP,
	ACPI_GPIO_PULL_DOWN,
	ACPI_GPIO_PULL_NONE
};

enum acpi_gpio_io_restrict {
	ACPI_GPIO_IO_RESTRICT_NONE,
	ACPI_GPIO_IO_RESTRICT_INPUT,
	ACPI_GPIO_IO_RESTRICT_OUTPUT,
	ACPI_GPIO_IO_RESTRICT_PRESERVE
};

#define ACPI_GPIO_REVISION_ID		1
#define ACPI_GPIO_MAX_PINS		8

struct acpi_gpio {
	int pin_count;
	uint16_t pins[ACPI_GPIO_MAX_PINS];

	enum acpi_gpio_type type;
	enum acpi_gpio_pull pull;
	const char *resource;

	/* GpioInt */
	uint16_t interrupt_debounce_timeout;	/* 1/100 ms */
	struct acpi_irq irq;

	/* GpioIo */
	uint16_t output_drive_strength;		/* 1/100 mA */
	int io_shared;
	enum acpi_gpio_io_restrict io_restrict;
	/*
	 * As per ACPI spec, GpioIo does not have any polarity associated with it. Linux kernel
	 * uses `active_low` argument within GPIO _DSD property to allow BIOS to indicate if the
	 * corresponding GPIO should be treated as active low. Thus, if the GPIO has active high
	 * polarity or if it does not have any polarity, then the `active_low` argument is
	 * supposed to be set to 0.
	 *
	 * Reference:
	 * https://www.kernel.org/doc/html/latest/firmware-guide/acpi/gpio-properties.html
	 */
	bool active_low;
};

/* GpioIo-related macros */
#define ACPI_GPIO_CFG(_gpio, _io_restrict, _active_low) { \
	.type = ACPI_GPIO_TYPE_IO, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.io_restrict = _io_restrict, \
	.active_low = _active_low, \
	.pin_count = 1, \
	.pins = { (_gpio) } }

/* Basic output GPIO with default pull settings */
#define ACPI_GPIO_OUTPUT_CFG(gpio, active_low) \
		ACPI_GPIO_CFG(gpio, ACPI_GPIO_IO_RESTRICT_OUTPUT, active_low)

#define ACPI_GPIO_OUTPUT(gpio)			ACPI_GPIO_OUTPUT_CFG(gpio, 0)
#define ACPI_GPIO_OUTPUT_ACTIVE_HIGH(gpio)	ACPI_GPIO_OUTPUT_CFG(gpio, 0)
#define ACPI_GPIO_OUTPUT_ACTIVE_LOW(gpio)	ACPI_GPIO_OUTPUT_CFG(gpio, 1)

/* Basic input GPIO with default pull settings */
#define ACPI_GPIO_INPUT_CFG(gpio, polarity) \
		ACPI_GPIO_CFG(gpio, ACPI_GPIO_IO_RESTRICT_INPUT, polarity)

#define ACPI_GPIO_INPUT(gpio)			ACPI_GPIO_INPUT_CFG(gpio, 0)
#define ACPI_GPIO_INPUT_ACTIVE_HIGH(gpio)	ACPI_GPIO_INPUT_CFG(gpio, 0)
#define ACPI_GPIO_INPUT_ACTIVE_LOW(gpio)	ACPI_GPIO_INPUT_CFG(gpio, 1)

/* GpioInt-related macros */
#define ACPI_GPIO_IRQ_CFG(_gpio, _mode, _polarity, _wake) { \
	.type = ACPI_GPIO_TYPE_INTERRUPT, \
	.pull = ACPI_GPIO_PULL_DEFAULT, \
	.irq.mode = _mode, \
	.irq.polarity = _polarity, \
	.irq.wake = _wake, \
	.pin_count = 1, \
	.pins = { (_gpio) } }

#define ACPI_GPIO_IRQ_EDGE(gpio, polarity) \
		ACPI_GPIO_IRQ_CFG(gpio, ACPI_IRQ_EDGE_TRIGGERED, polarity, 0)

#define ACPI_GPIO_IRQ_EDGE_WAKE(gpio, polarity) \
		ACPI_GPIO_IRQ_CFG(gpio, ACPI_IRQ_EDGE_TRIGGERED, polarity, ACPI_IRQ_WAKE)

#define ACPI_GPIO_IRQ_LEVEL(gpio, polarity) \
		ACPI_GPIO_IRQ_CFG(gpio, ACPI_IRQ_LEVEL_TRIGGERED, polarity, 0)

#define ACPI_GPIO_IRQ_LEVEL_WAKE(gpio, polarity) \
		ACPI_GPIO_IRQ_CFG(gpio, ACPI_IRQ_LEVEL_TRIGGERED, polarity, ACPI_IRQ_WAKE)

/* Edge Triggered Active High GPIO interrupt */
#define ACPI_GPIO_IRQ_EDGE_HIGH(gpio) \
		ACPI_GPIO_IRQ_EDGE(gpio, ACPI_IRQ_ACTIVE_HIGH)

/* Edge Triggered Active Low GPIO interrupt */
#define ACPI_GPIO_IRQ_EDGE_LOW(gpio) \
		ACPI_GPIO_IRQ_EDGE(gpio, ACPI_IRQ_ACTIVE_LOW)

/* Edge Triggered Active Both GPIO interrupt */
#define ACPI_GPIO_IRQ_EDGE_BOTH(gpio) \
		ACPI_GPIO_IRQ_EDGE(gpio, ACPI_IRQ_ACTIVE_BOTH)

/* Edge Triggered Active High GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_EDGE_HIGH_WAKE(gpio) \
		ACPI_GPIO_IRQ_EDGE_WAKE(gpio, ACPI_IRQ_ACTIVE_HIGH)

/* Edge Triggered Active Low GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_EDGE_LOW_WAKE(gpio) \
		ACPI_GPIO_IRQ_EDGE_WAKE(gpio, ACPI_IRQ_ACTIVE_LOW)

/* Edge Triggered Active Both GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_EDGE_BOTH_WAKE(gpio) \
		ACPI_GPIO_IRQ_EDGE_WAKE(gpio, ACPI_IRQ_ACTIVE_BOTH)

/* Level Triggered Active High GPIO interrupt */
#define ACPI_GPIO_IRQ_LEVEL_HIGH(gpio) \
		ACPI_GPIO_IRQ_LEVEL(gpio, ACPI_IRQ_ACTIVE_HIGH)

/* Level Triggered Active Low GPIO interrupt */
#define ACPI_GPIO_IRQ_LEVEL_LOW(gpio) \
		ACPI_GPIO_IRQ_LEVEL(gpio, ACPI_IRQ_ACTIVE_LOW)

/* Level Triggered Active High GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_LEVEL_HIGH_WAKE(gpio) \
		ACPI_GPIO_IRQ_LEVEL_WAKE(gpio, ACPI_IRQ_ACTIVE_HIGH)

/* Level Triggered Active Low GPIO interrupt with wake */
#define ACPI_GPIO_IRQ_LEVEL_LOW_WAKE(gpio) \
		ACPI_GPIO_IRQ_LEVEL_WAKE(gpio, ACPI_IRQ_ACTIVE_LOW)

/* Write GpioIo() or GpioInt() descriptor to SSDT AML output */
void acpi_device_write_gpio(const struct acpi_gpio *gpio);

/*
 * ACPI Descriptors for Serial Bus interfaces
 */

#define ACPI_SERIAL_BUS_TYPE_I2C		1
#define ACPI_SERIAL_BUS_TYPE_SPI		2
#define ACPI_SERIAL_BUS_TYPE_UART		3

#define ACPI_I2C_SERIAL_BUS_REVISION_ID		1 /* TODO: upgrade to 2 */
#define ACPI_I2C_TYPE_SPECIFIC_REVISION_ID	1
#define ACPI_SPI_SERIAL_BUS_REVISION_ID		1
#define ACPI_SPI_TYPE_SPECIFIC_REVISION_ID	1
#define ACPI_UART_SERIAL_BUS_REVISION_ID	1
#define ACPI_UART_TYPE_SPECIFIC_REVISION_ID	1

/*
 * ACPI I2C Bus
 */

struct acpi_i2c {
	/* I2C Address */
	uint16_t address;
	/* 7 or 10 bit Address Mode */
	enum i2c_address_mode mode_10bit;
	/* I2C Bus Speed in Hz */
	enum i2c_speed speed;
	/* Reference to I2C controller */
	const char *resource;
};

/* Write I2cSerialBus() descriptor to SSDT AML output */
void acpi_device_write_i2c(const struct acpi_i2c *i2c);

/*
 * ACPI SPI Bus
 */

struct acpi_spi {
	/* Device selection */
	uint16_t device_select;
	/* Device selection line is active high or low */
	enum spi_polarity device_select_polarity;
	/* 3 or 4 wire SPI connection */
	enum spi_wire_mode wire_mode;
	/* Connection speed in HZ */
	unsigned int speed;
	/* Size in bits of smallest transfer unit */
	u8 data_bit_length;
	/* Phase of clock pulse on which to capture data */
	enum spi_clock_phase clock_phase;
	/* Indicate if clock is high or low during first phase */
	enum spi_polarity clock_polarity;
	/* Reference to SPI controller */
	const char *resource;
};

/* Write SPI Bus descriptor to SSDT AML output */
void acpi_device_write_spi(const struct acpi_spi *spi);

/*
 * ACPI UART Bus
 */

enum acpi_uart_data_bits {
	ACPI_UART_DATA_BITS_5,
	ACPI_UART_DATA_BITS_6,
	ACPI_UART_DATA_BITS_7,
	ACPI_UART_DATA_BITS_8,
	ACPI_UART_DATA_BITS_9
};

enum acpi_uart_stop_bits {
	ACPI_UART_STOP_BITS_0,
	ACPI_UART_STOP_BITS_1,
	ACPI_UART_STOP_BITS_1_5,
	ACPI_UART_STOP_BITS_2
};

enum acpi_uart_lines {
	ACPI_UART_LINE_DTD = BIT(2),	/* Data Carrier Detect */
	ACPI_UART_LINE_RI = BIT(3),	/* Ring Indicator */
	ACPI_UART_LINE_DSR = BIT(4),	/* Data Set Ready */
	ACPI_UART_LINE_DTR = BIT(5),	/* Data Terminal Ready */
	ACPI_UART_LINE_CTS = BIT(6),	/* Clear to Send */
	ACPI_UART_LINE_RTS = BIT(7)	/* Request to Send */
};

enum acpi_uart_endian {
	ACPI_UART_ENDIAN_LITTLE,
	ACPI_UART_ENDIAN_BIG
};

enum acpi_uart_parity {
	ACPI_UART_PARITY_NONE,
	ACPI_UART_PARITY_EVEN,
	ACPI_UART_PARITY_ODD,
	ACPI_UART_PARITY_MARK,
	ACPI_UART_PARITY_SPACE
};

enum acpi_uart_flow_control {
	ACPI_UART_FLOW_NONE,
	ACPI_UART_FLOW_HARDWARE,
	ACPI_UART_FLOW_SOFTWARE
};

struct acpi_uart {
	/* Initial Baud Rate in bits per second */
	uint32_t initial_baud_rate;
	/* Number of bits of data in a packet (value between 5-9) */
	enum acpi_uart_data_bits data_bits;
	/* Number of bits to signal end of packet */
	enum acpi_uart_stop_bits stop_bits;
	/* Bitmask indicating presence or absence of particular line */
	unsigned int lines_in_use;
	/* Specify if the device expects big or little endian format */
	enum acpi_uart_endian endian;
	/* Specify the type of parity bits included after the data in a packet */
	enum acpi_uart_parity parity;
	/* Specify the flow control method */
	enum acpi_uart_flow_control flow_control;
	/* Upper limit in bytes of the buffer sizes for this device */
	uint16_t rx_fifo_bytes;
	uint16_t tx_fifo_bytes;
	/* Set true if UART is shared, false if it is exclusive for one device */
	bool shared;
	/* Reference to UART controller */
	const char *resource;
};

#define ACPI_UART_RAW_DEVICE(baud_rate, fifo_bytes) { \
	.initial_baud_rate = (baud_rate), \
	.data_bits = ACPI_UART_DATA_BITS_8, \
	.stop_bits = ACPI_UART_STOP_BITS_1, \
	.endian = ACPI_UART_ENDIAN_LITTLE, \
	.parity = ACPI_UART_PARITY_NONE, \
	.flow_control = ACPI_UART_FLOW_NONE, \
	.rx_fifo_bytes = (fifo_bytes), \
	.tx_fifo_bytes = (fifo_bytes), \
	.shared = false }

/* Write UARTSerialBusV2() descriptor to SSDT AML output */
void acpi_device_write_uart(const struct acpi_uart *uart);

/* GPIO/timing information for the power on/off sequences */
struct acpi_power_res_params {
	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio *reset_gpio;
	/* Delay to be inserted after device is taken out of reset.
	 * (_ON method delay)
	 */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset.
	 * (_OFF method delay)
	 */
	unsigned int reset_off_delay_ms;
	/* GPIO used to enable device. */
	struct acpi_gpio *enable_gpio;
	/* Delay to be inserted after device is enabled.
	 * (_ON method delay)
	 */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled.
	 * (_OFF method delay)
	 */
	unsigned int enable_off_delay_ms;
	/* GPIO used to stop operation of device. */
	struct acpi_gpio *stop_gpio;
	/* Delay to be inserted after disabling stop.
	 * (_ON method delay)
	 */
	unsigned int stop_delay_ms;
	/* Delay to be inserted after enabling stop.
	 * (_OFF method delay)
	 */
	unsigned int stop_off_delay_ms;

	/* Write a _STA method that uses the state of the GPIOs to determine if
	 * the PowerResource is ON or OFF. If this is false, the _STA method
	 * will always return ON.
	 */
	bool use_gpio_for_status;
};

/*
 * Add a basic PowerResource block for a device that includes
 * GPIOs to control enable, reset and stop operation of the device. Each
 * GPIO is optional, but at least one must be provided.
 *
 * Reset - Put the device into / take the device out of reset.
 * Enable - Enable / disable power to device.
 * Stop - Stop / start operation of device.
 */
void acpi_device_add_power_res(const struct acpi_power_res_params *params);

/*
 * Writing Device Properties objects via _DSD
 *
 * http://uefi.org/sites/default/files/resources/_DSD-device-properties-UUID.pdf
 * http://uefi.org/sites/default/files/resources/_DSD-hierarchical-data-extension-UUID-v1.pdf
 *
 * The Device Property Hierarchy can be multiple levels deep with multiple
 * children possible in each level.  In order to support this flexibility
 * the device property hierarchy must be built up before being written out.
 *
 * For example:
 *
 * // Child table with string and integer
 * struct acpi_dp *child = acpi_dp_new_table("CHLD");
 * acpi_dp_add_string(child, "childstring", "CHILD");
 * acpi_dp_add_integer(child, "childint", 100);
 *
 * // _DSD table with integer and gpio and child pointer
 * struct acpi_dp *dsd = acpi_dp_new_table("_DSD");
 * acpi_dp_add_integer(dsd, "number1", 1);
 * acpi_dp_add_gpio(dsd, "gpio", "\_SB.PCI0.GPIO", 0, 0, 1);
 * acpi_dp_add_child(dsd, "child", child);
 *
 * // Write entries into SSDT and clean up resources
 * acpi_dp_write(dsd);
 *
 * Name(_DSD, Package() {
 *   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301")
 *   Package() {
 *     Package() { "gpio", Package() { \_SB.PCI0.GPIO, 0, 0, 0 } }
 *     Package() { "number1", 1 }
 *   }
 *   ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b")
 *   Package() {
 *     Package() { "child", CHLD }
 *   }
 * }
 * Name(CHLD, Package() {
 *   ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301")
 *   Package() {
 *     Package() { "childstring", "CHILD" }
 *     Package() { "childint", 100 }
 *   }
 * }
 */

/* Start a new Device Property table with provided ACPI reference */
struct acpi_dp *acpi_dp_new_table(const char *ref);

/* Add package of device properties with a unique UUID */
struct acpi_dp *acpi_dp_add_package(struct acpi_dp *dp, struct acpi_dp *package);

/* Add integer Device Property */
struct acpi_dp *acpi_dp_add_integer(struct acpi_dp *dp, const char *name,
				    uint64_t value);

/* Add string Device Property */
struct acpi_dp *acpi_dp_add_string(struct acpi_dp *dp, const char *name,
				   const char *string);

/* Add ACPI reference Device Property */
struct acpi_dp *acpi_dp_add_reference(struct acpi_dp *dp, const char *name,
				      const char *reference);

/* Add an array of Device Properties */
struct acpi_dp *acpi_dp_add_array(struct acpi_dp *dp, struct acpi_dp *array);

/* Add an array of integers Device Property */
struct acpi_dp *acpi_dp_add_integer_array(struct acpi_dp *dp, const char *name,
					  const uint64_t *array, int len);

/* Add a GPIO binding Device Property */
struct acpi_dp *acpi_dp_add_gpio(struct acpi_dp *dp, const char *name,
				 const char *ref, int index, int pin,
				 int active_low);

struct acpi_gpio_res_params {
	/* Reference to the parent device. */
	const char *ref;
	/* Index to the GpioIo resource within the _CRS. */
	int index;
	/* Index to the pin within the GpioIo resource, usually 0. */
	int pin;
	/* Flag to indicate if pin is active low. */
	int active_low;
};

/* Add a GPIO binding device property for array of GPIOs */
struct acpi_dp *acpi_dp_add_gpio_array(struct acpi_dp *dp, const char *name,
				       const struct acpi_gpio_res_params *params,
				       size_t param_count);

/* Add a child table of Device Properties */
struct acpi_dp *acpi_dp_add_child(struct acpi_dp *dp, const char *name,
				  struct acpi_dp *child);

/* Add a list of Device Properties, returns the number of properties added */
size_t acpi_dp_add_property_list(struct acpi_dp *dp,
				 const struct acpi_dp *property_list,
				 size_t property_count);

/* Write Device Property hierarchy and clean up resources */
void acpi_dp_write(struct acpi_dp *table);

/*
 * Helper function to write a PCI device with _ADR object defined.
 *
 * IMPORTANT: Scope of a device created in SSDT cannot be used to add ACPI nodes under that
 * scope in DSDT. So, if there are any references to this PCI device scope required from static
 * asl files, do not use this function and instead add the device to DSDT as well.
 */
void acpi_device_write_pci_dev(const struct device *dev);

/* Helper function to add ExternalFacingPort to _DSD in the current scope */
void acpi_device_add_external_facing_port(struct acpi_dp *dsd);

/* Helper function to add HotPlugSupportInD3 to _DSD in the current scope */
void acpi_device_add_hotplug_support_in_d3(struct acpi_dp *dsd);

/* Helper function to add DmaProperty to _DSD in the current scope */
void acpi_device_add_dma_property(struct acpi_dp *dsd);

/* Helper function to add StorageD3Enable to _DSD in the current scope */
void acpi_device_add_storage_d3_enable(struct acpi_dp *dsd);

#endif /* __ACPI_ACPI_DEVICE_H__ */
