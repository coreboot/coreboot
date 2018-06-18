# Cavium bootflow

The on-chip **BOOTROM** first sets up the L2 cache and the SPI controller.
It then reads **CSIB_NBL1FW** and **CLIB_NBL1FW** configuration data to get
the position of the bootstage in flash. It then loads 192KiB from flash into
L2 cache to a fixed address. The boot mode is called "Non-Secure-Boot" as
the signature of the bootstage isn't verified.
The **BOOTROM** can do AES decryption for obfuscation or verify the signature
of the bootstage. Both features aren't used and won't be described any further.

* The typical position of bootstage in flash is at address **0x20000**.
* The entry point in physical DRAM is at address **0x100000**.

## Layout

![Bootflow of Cavium CN8xxx SoCs][cavium_bootflow]

[cavium_bootflow]: cavium_bootflow.png

