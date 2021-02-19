# GPIO toggling in ACPI AML for coreboot

## Table of contents
- Introduction
- Platform Interface
- Helper routines
- Implementation details
- Arguments and Local Variables Management

## Introduction

ACPI provides platform-independent interfaces enabling the operating
system to perform power management for devices as well as the entire
system. An operating system can simply call into Method()s implemented
by the interface to request different power management operations. In
order to be able to perform these operations, an interface might
require toggling of GPIOs. e.g. a touchscreen device interface might
require toggling of reset-gpio in order to take the device out of
reset or to put it back into reset.

Thus, any coreboot driver that implements such an ACPI interface might
require the ability to toggle GPIOs. However, toggling of GPIO is not
the same across different platforms and it will require the driver to
depend upon platform to do the required work. This document presents a
simple interface that can be used by any coreboot driver to generate
ACPI AML code for reading or toggling platform GPIOs.

## Platform Interface

All platforms that use drivers requiring ACPI AML code for GPIO
interactions need to be implement the following functions:
1. Return GPIO Rx value if it is acting as input
   int acpigen_soc_read_rx_gpio(unsigned int gpio_num)
2. Return GPIO Tx value if it is acting as output
   int acpigen_soc_get_tx_gpio(unsigned int gpio_num)
3. Set GPIO Tx value to 1 if it is acting as output
   int acpigen_soc_set_tx_gpio(unsigned int gpio_num)
4. Set GPIO Tx value to 0 if it is acting as output
   int acpigen_soc_clear_tx_gpio(unsigned int gpio_num)

Each of the above functions takes as input gpio_num which is the gpio
number that needs to be read or toggled and returns an integer which
is:
1. Error = -1
2. Success = 0

Above callback functions are chosen to be implemented in C rather than
adding them as AML code callbacks for the following reasons:
1. It is easier to add error prints in C which will inform the
   developer that these callbacks are missing. It restricts debugging
   to coreboot logs.
2. GPIO conversion from number to register offset can be easily done
   in C by reusing implemented functions rather than adding all the
   logic to AML code or depending upon complicated macros to be added
   to device-tree.
3. Allows GPIO AML methods to be present under any device scope and
   gives SoC the flexibility to call them without any restrictions.

## Helper routines

In order to relieve drivers of the task of implementing the same code
for enabling/disabling Tx GPIOs based on the GPIO polarity, helper
routines are provided which implement this common code and can be used
directly in the driver routines:
1. Enable Tx GPIO
   int acpigen_enable_tx_gpio(struct acpi_gpio gpio)
2. Disable Tx GPIO
   int acpigen_disable_tx_gpio(struct acpi_gpio gpio)

Both the above functions take as input struct acpi_gpio type and
return -1 on error and 0 on success. These helper routines end up
calling the platform specific acpigen_soc_{set,clear}_tx_gpio
functions internally. Thus, all the ACPI AML calling conventions for
the platform functions apply to these helper functions as well.

3. Get Rx GPIO
   int acpigen_get_rx_gpio(struct acpi_gpio gpio)

This function takes as input, an struct acpi_gpio type and outputs
AML code to read the *logical* value of a gpio (after taking its
polarity into consideration), into the Local0 variable. It calls
the platform specific acpigen_soc_read_rx_gpio() to actually read
the raw Rx gpio value.

## Implementation Details

ACPI library in coreboot will provide weak definitions for all the
above functions with error messages indicating that these functions
are being used. This allows drivers to conditionally make use of GPIOs
based on device-tree entries or any other config option. It is
recommended that the SoC code in coreboot should provide
implementations of all the above functions generating ACPI AML code
irrespective of them being used in any driver. This allows mainboards
to use any drivers and take advantage of this common infrastructure.

Platforms are restricted to using Local5, Local6 and Local7 variables
only in implementations of the above functions. Any AML methods called
by the above functions do not have any such restrictions on use of
Local variables in AML code. Local0 is to be used for all get/read
functions to return values. This means that the driver code should not
make any assumptions about the values in Local5, Local6 and Local7
variables.

```
 **Function**                   **Operation**                **Return**
 acpigen_soc_read_rx_gpio     Generate ACPI AML code to      Error = -1
                              read value of Rx in Local0.    Success = 0
 acpigen_soc_get_tx_gpio      Generate ACPI AML code to      Error = -1
                              get value of Tx in Local0.     Success = 0
 acpigen_soc_set_tx_gpio      Generate ACPI AML code to      Error = -1
                              set Tx to 1.                   Success = 0
 acpigen_soc_clear_tx_gpio    Generate ACPI AML code to      Error = -1
                              set Tx to 0.                   Success = 0
```

Ideally, the operation column in the above table should use one or
more functions implemented by the platform in AML code library (like
gpiolib.asl). In the example below SPC0 and GPC0 need to be
implemented by the SoC in AML code library and they can be used by
acpi_soc_set_tx_gpio to read and set bit in the appropriate register
for the GPIO.

**acpigen_soc_set_tx_gpio**

	uint64_t gpio_reg_offset = gpio_get_reg_offset(gpio_num);

	/* Store (\_SB.GPC0(gpio_reg_offset, Local5) */
	acpigen_write_store();
	acpigen_emit_namestring(“\\_SB.GPC0”);
	acpigen_write_integer(gpio_reg_offset);
	acpigen_emit_byte(LOCAL5_OP);


	/* Or (Local5, TX_BIT, Local5) */
	acpigen_write_or(LOCAL5_OP, TX_BIT, LOCAL5_OP);

	/* \_SB.SPC0(gpio_reg_offset, LOCAL5) */
	acpigen_emit_namestring(“\\_SB.SPC0”);
	acpigen_write_integer(gpio_reg_offset);
	acpigen_emit_byte(LOCAL5_OP);

	return 0;

**acpigen_soc_get_tx_gpio**

	uint64_t gpio_reg_offset = gpio_get_reg_offset(gpio_num);


	/* Store (\_SB.GPC0(gpio_reg_offset, Local5) */
	acpigen_write_store();
	acpigen_emit_namestring(“\\_SB.GPC0”);
	acpigen_write_integer(gpio_reg_offset);
	acpigen_emit_byte(LOCAL5_OP);


	/*
	 * If (And (Local5, TX_BIT)) Store (One, Local0) Else Store (Zero,
	 * Local0)
	 */
	acpigen_write_if_and(Local5, TX_BIT);
	acpigen_write_store_args(ONE_OP, LOCAL0_OP);
	acpigen_write_else();
	acpigen_write_store_args(ZERO_OP, LOCAL0_OP);
	acpigen_pop_len();

	return 0;


These are reference implementations and the platforms are free to
implement these functions in any way they like. coreboot driver can
then simply call into these functions to generate ACPI AML code to
get/set/clear any GPIO. In order to decide whether GPIO operations are
required, driver code can rely either on some config option or read
device-tree to use any user-provided GPIOs.

## Arguments and Local Variables Management

Platform-defined functions can call methods using the same calling
conventions provided by AML code. However, use of Local Variables is
restricted to Local5, Local6 and Local7 unless they call into some
other method. Called method can use any Local variables, Local0 -
Local7. In case of functions expected to return back value to the
caller, this value is expected to be returned in Local0.

Driver code should not make any assumptions about the contents of
Local5, Local6 and Local7 across callbacks to SoC code. If it makes a
read or get call to SoC, the return value should be used from Local0
on return. However, if it makes a set or clear call to SoC, the value
in Local0 is undefined.
