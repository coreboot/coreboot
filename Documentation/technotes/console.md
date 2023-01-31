# coreboot Console

coreboot supports multiple ways to access its console.
https://www.coreboot.org/Console_and_outputs


## SMBus Console

SMBus is a two-wire interface which is based on the principles of
operation of I2C. SMBus, was first was designed to allow a battery to
communicate with the charger, the system host, and/or other
power-related components in the system.

Enable the SMBus console with `CONSOLE_I2C_SMBUS` Kconfig. Set
`CONSOLE_I2C_SMBUS_SLAVE_ADDRESS` and
`CONSOLE_I2C_SMBUS_SLAVE_DATA_REGISTER` configuration values of the
slave I2C device which you will use to capture I2C packets.

Modern computer Random Access Memory (RAM) slot has SMBus in it
according to the JEDEC standards. We can use a breakout-board to expose
those SMBus pins. Some mainboard have SMBus pins in the PCIe slot as
well.

This feature has been tested on the following platforms:
```eval_rst
+------------------------------------+
| Tested platforms                   |
+====================================+
| GA-H61M-S2PV + Intel Ivy Bridge    |
+------------------------------------+
```

A minimal DDR3 DIMM breakout board PCB design with only the
SDA(Data line) and SCL(Clock line) pins of I2C/SMBus can be found
[here](https://github.com/drac98/ram-breakout-board).
See the PCB layout [here](https://archive.org/details/ddr3-dimm-F_Cu)

NOTE:
To capture the I2C packets, an I2C slave device is required. The easiest
way to capture the log message is to use a I2C to UART converter chip
with a UART to USB converter chip. The setup would be as follows.
```text
+---------+    +-------------+    +-------------+
+   PC    +----+ UART to USB +----+ I2C to UART |
+---------+    +-------------+    +-------------+
                                              | |
------------------------------------------------+--  System Management
----------------------------------------------+----  Bus
```

Watch this [video](https://youtu.be/Q0dK41n9db8) to see how it is set
up. A backup of the video is available
[here](https://web.archive.org/web/20220916172605/https://www.youtube.com/watch?v=Q0dK41n9db8)

If you are using any of the `SC16IS740/750/760` I2C to UART converter
chip, you can enable the `SC16IS7XX_INIT` option to initialize the chip.

If not we can use other I2C slave devices like an Arduino or a
Beagleboard.
* [Linux I2C Slave interface](https://web.archive.org/web/20220926173943/https://www.kernel.org/doc/html/latest/i2c/slave-interface.html)
* [BeagleBone Black I2C Slave](https://web.archive.org/web/20220926171211/https://forum.beagleboard.org/t/beaglebone-black-and-arduino-uno-i2c-communication-using-c/29990/8)

This feature was added as part of a GSoC 2022 project. Checkout the
following blog posts for more details.
* [coreboot Console via SMBus — Part I](https://medium.com/@husnifaiz/coreboot-console-via-smbus-introduction-38273691a8ac)
* [coreboot Console via SMBus — Part II](https://medium.com/@husnifaiz/coreboot-console-via-smbus-part-ii-bc324fdd2f24)
