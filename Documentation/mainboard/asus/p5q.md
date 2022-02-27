# ASUS P5Q

This page describes how to run coreboot on the [ASUS P5Q] desktop board.

## Working

+ PCI slots
+ PCI-e slots
+ Onboard Ethernet
+ USB
+ Onboard sound card
+ PS/2 keyboard
+ All 4 DIMM slots
+ S3 suspend and resume
+ Red SATA ports
+ Fan control through the W83667HG chip
+ FireWire

## Not working

+ PS/2 mouse support
+ PATA aka IDE (because of buggy IDE controller)
+ Fan profiles with Q-Fan
+ TPM module (support not implemented)

## Untested

+ S/PDIF
+ CD Audio In
+ Floppy disk drive


## Flashing coreboot

```eval_rst
+-------------------+----------------+
| Type              | Value          |
+===================+================+
| Socketed flash    | Yes            |
+-------------------+----------------+
| Model             | MX25L8005      |
+-------------------+----------------+
| Size              | 1 MiB          |
+-------------------+----------------+
| Package           | Socketed DIP-8 |
+-------------------+----------------+
| Write protection  | No             |
+-------------------+----------------+
| Dual BIOS feature | No             |
+-------------------+----------------+
| Internal flashing | Yes            |
+-------------------+----------------+
```

You can flash coreboot into your motherboard using [this guide].

## Technology

```eval_rst
+------------------+---------------------------------------------------+
| Northbridge      | Intel P45 (called x4x in coreboot code)           |
+------------------+---------------------------------------------------+
| Southbridge      | Intel ICH10R (called i82801jx in coreboot code)   |
+------------------+---------------------------------------------------+
| CPU (LGA775)     | Model f4x, f6x, 6fx, 1067x (Pentium 4, d, Core 2) |
+------------------+---------------------------------------------------+
| SuperIO          | Winbond W83667HG                                  |
+------------------+---------------------------------------------------+
| Coprocessor      | No                                                |
+------------------+---------------------------------------------------+
| Clockgen (CK505) | ICS 9LPRS918JKLF                                  |
+------------------+---------------------------------------------------+
```

## Controlling fans

With vendor firmware, the P5Q uses the ATK0110 ACPI device to control its fans
according to the parameters configured in the BIOS setup menu. With coreboot,
one can instead control the Super I/O directly as described in the
[kernel docs]:

+ pwm1 controls fan1 (CHA_FAN1) and fan4 (CHA_FAN2)
+ pwm2 controls fan2 (CPU_FAN)
+ fan3 (PWR_FAN) cannot be controlled
+ temp1 (board) can be used to control fan1 and fan4
+ temp2 (CPU) can be used to control fan2

### Manual fan speed

These commands set the chassis fans to a constant speed:

    # Use PWM output
    echo 1 >/sys/class/hwmon/hwmon2/pwm1_mode
    # Set to manual mode
    echo 1 >/sys/class/hwmon/hwmon2/pwm1_enable
    # Set relative speed: 0 (stop) to 255 (full)
    echo 150 >/sys/class/hwmon/hwmon2/pwm1

### Automatic fan speed

The W83667HG can adjust fan speeds when things get too warm. These settings will
control the chassis fans:

    # Set to "Thermal Cruise" mode
    echo 2 >/sys/class/hwmon/hwmon2/pwm1_enable
    # Target temperature: 60°C
    echo 60000 >/sys/class/hwmon/hwmon2/pwm1_target
    # Minimum fan speed when spinning up
    echo 135 >/sys/class/hwmon/hwmon2/pwm1_start_output
    # Minimum fan speed when spinning down
    echo 135 >/sys/class/hwmon/hwmon2/pwm1_stop_output
    # Tolerance: 2°C
    echo 2000 >/sys/class/hwmon/hwmon2/pwm1_tolerance
    # Turn fans off after 600 seconds when below defined range
    echo 600000 >/sys/class/hwmon/hwmon2/pwm1_stop_time

You can also control the CPU fan with similar rules:

    # Switch to "Thermal Cruise" mode
    echo 2 >/sys/class/hwmon/hwmon2/pwm2_enable
    # Target temperature: 55°C
    echo 55000 >/sys/class/hwmon/hwmon2/pwm2_target
    # Minimum fan speed when spinning down
    echo 50 >/sys/class/hwmon/hwmon2/pwm2_stop_output
    # Rate of fan speed change
    echo 50 >/sys/class/hwmon/hwmon2/pwm2_step_output
    # Maximum fan speed
    echo 200 >/sys/class/hwmon/hwmon2/pwm2_max_output
    # Tolerance: 2°C
    echo 2000 >/sys/class/hwmon/hwmon2/pwm1_tolerance

[ASUS P5Q]: https://www.asus.com/Motherboards/P5Q
[this guide]: ../../tutorial/flashing_firmware/int_flashrom.md
[kernel docs]: https://www.kernel.org/doc/Documentation/hwmon/w83627ehf.rst
