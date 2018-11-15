# qemu i440fx mainboard

## Running coreboot in qemu
Emulators like qemu don't need a firmware to do hardware init.
The hardware starts in the configured state already.

The coreboot port allows to test non mainboard specific code.
As you can easily attach a debugger, it's a good target for
experimental code.

## coreboot x86_64 support
coreboot historically runs in 32-bit protected mode, even though the
processor supports x86_64 instructions (long mode).

The qemu-i440fx mainboard has been ported to x86_64 and will serve as
reference platform to enable additional platforms.

To enable the support set the Kconfig option ``CONFIG_CPU_QEMU_X86_64=y``.

## Installing qemu

On debian you can install qemu by running:
```bash
$ sudo apt-get install qemu
```

On redhat you can install qemu by running:
```bash
$ sudo dnf install qemu
```

## Running coreboot

### To run the i386 version of coreboot (default)
Running on qemu-system-i386 will require a 32 bit operating system.

```bash
qemu-system-i386 -bios build/coreboot.rom -serial stdio -M pc
```

### To run the experimental x86_64 version of coreboot
Running on qemu-system-x86_64 allows to run a 32 bit or 64 bit operating system,
as well as firmware.

```bash
qemu-system-x86_64 -bios build/coreboot.rom -serial stdio -M pc
```

## Finding bugs
To test coreboot's x86 code it's recommended to run on a x86 host and enable KVM.
It will not only run faster, but is closer to real hardware. If you see the
following message:

    KVM internal error. Suberror: 1
    emulation failure

something went wrong. The same bug will likely cause a FAULT on real hardware,
too.

To enable KVM run:

```bash
qemu-system-x86_64 -bios build/coreboot.rom -serial stdio -M pc -accel kvm -cpu host
```
