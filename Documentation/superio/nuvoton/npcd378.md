# NPCD378

This page describes the [Nuvoton] SuperIO chip that can be found on various [HP]
mainboards.

As no datasheet is available most of the functions have been reverse engineered and
might be inaccurate or wrong.

## LDNs

```{eval-rst}
+-------+---------------------------+
| LDN # | Function                  |
+=======+===========================+
| 0     | FDC                       |
+-------+---------------------------+
| 1     | Parallel Port             |
+-------+---------------------------+
| 2     | Com1                      |
+-------+---------------------------+
| 3     | Com2 / IR                 |
+-------+---------------------------+
| 4     | LED and PWR button CTRL   |
+-------+---------------------------+
| 5     | PS/2 AUX                  |
+-------+---------------------------+
| 6     | PS/2 KB                   |
+-------+---------------------------+
| 7     | WDT1                      |
+-------+---------------------------+
| 8     | HWM                       |
+-------+---------------------------+
| 0xf   | GPIO                      |
+-------+---------------------------+
| 0x15  | I2C ?                     |
+-------+---------------------------+
| 0x1e  | SUSPEND CTL ?             |
+-------+---------------------------+
| 0x1c  | GPIO ?                    |
+-------+---------------------------+
```

### LDN0

Follows [Nuvoton]'s default FDC register set. See [NCT6102D] for more details.

### LDN1

Follows [Nuvoton]'s default LPT register set. See [NCT6102D] for more details.

### LDN2

Follows [Nuvoton]'s default COM1 register set. See [NCT6102D] for more details.

### LDN3

Follows [Nuvoton]'s default COM2 register set. See [NCT6102D] for more details.

### LDN4

On most SuperIOs the use of LDN4 is forbidden. That's not the case on NPCD378.

It exposes 16 byte of IO config space to control the front LEDs PWM duty cycle
and power button behaviour on normal / during S3 resume.

### LDN5

A custom PS/2 AUX port.

### LDN6

Follows [Nuvoton]'s default KBC register set. See [NCT6102D] for more details.

### LDN7

Looks like a WDT.

### LDN8

Custom HWM space. It exposes 256 byte of IO config space.
See [HWM](#hwm) for more details.

## HWM

### Register

The registers are accessible via IO space and are located at LDN8's IOBASE.

```{eval-rst}
+---------------+-----------------------+
| IOBASE offset | Register              |
+---------------+-----------------------+
| 0x4           | Host Write CTRL       |
+---------------+-----------------------+
| 0x10 - 0xfe   | HWM Page #            |
+---------------+-----------------------+
| 0xff          | Page index select     |
+---------------+-----------------------+
```

### Host Write CTRL
Bit 0 must be cleared prior to writing any of the HWM register and it must be
set after writing to HWM register to signal the SuperIO that data has changed.
Reading register is possible at any time and doesn't need special locking.

### HWM Page
The SuperIO exposes 16 different pages. Nearly all registers are unknown.

**Page 1**

```{eval-rst}
+---------------+-----------------------+
| IOBASE offset | Register              |
+---------------+-----------------------+
| 0x98          | PSU fan PWM           |
+---------------+-----------------------+
```

### Page index
The 4 LSB of the page index register selects which HWM page is active.
A write takes effect immediately.

[NCT6102D]: https://www.nuvoton.com/resource-files/NCT6102D_NCT6106D_Datasheet_V1_0.pdf
[Nuvoton]: http://www.nuvoton.com/hq/
[HP]: https://www.hp.com/
