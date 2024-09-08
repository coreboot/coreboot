# QEMU SBSA emulator
This page describes how to build and run ```coreboot``` for QEMU's sbsa-ref machine.
The qemu-sbsa ```coreboot``` image acts as BL-3.3 for Arm Trusted Firmware (```TF-A```) and
mainly takes care of setting up SMBIOS and ACPI tables, hence, in order to boot,
you also need to supply a ```TF-A``` image.

## Building TF-A

You can build ```TF-A``` from source by fetching
```
https://github.com/ARM-software/arm-trusted-firmware
```
and building the qemu-sbsa platform
```
PLAT=qemu_sbsa
```
Upon entry, ```coreboot``` expects a FDT pointer in x0, so make sure to compile ```TF-A``` with
```
ARM_LINUX_KERNEL_AS_BL33=1
```
This will force ```TF-A``` to pass a pointer to the FDT in x0.

## Building coreboot

Simply select the qemu-sbsa board and, optionally, configure a payload. We recommend
the ```leanefi``` payload. ```leanefi``` will setup a minimal set of UEFI services, just enough
to boot into a linux kernel.

## Running coreboot in QEMU

Once you have obtained ```TF-A``` and ```coreboot``` images, launch qemu via

```bash
qemu-system-aarch64 -nographic -m 1024 -M sbsa-ref -pflash <path/to/TFA.fd> \
                                                   -pflash <path/to/coreboot.rom>
```

## LBBR bootflow

arm and 9elements worked together in order to create a LBBR compliant bootflow
consisting of ```TF-A```, ```coreboot```, ```leanefi``` and ```LinuxBoot```. A proof of concept
can be found here <https://gitlab.arm.com/systemready/firmware-build/linuxboot/lbbr-coreboot-poc>
