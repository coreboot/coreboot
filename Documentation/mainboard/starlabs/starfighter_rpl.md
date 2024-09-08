# StarFighter Mk I

## Specs

- CPU (full processor specs available at <https://ark.intel.com>)
    - Intel i3-1315U (Raptor Lake)
    - Intel i7-13700H (Raptor Lake)
    - Intel i9-13900H (Raptor Lake)
- EC
    - ITE IT5570E
    - Backlit keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - USB-C PD Charger
    - Suspend / resume
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - eDP 16-inch 3840x2400 or 2560x1600 LCD
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 16, 32 or 64GB LPDDR5 on-board memory
- Networking
    - AX210 2230 WiFi / Bluetooth
- Sound
    - Realtek ALC256
    - Internal speakers
    - Removable microphone
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - 2 xM.2 PCIe SSD
    - RTS5129 MicroSD card reader
- USB
    - 1920x1080 removable CCD camera
    - 2 x Thunderbolt 4.0 (left) (Raptor Lake)
    - USB 3.1 Gen 2 Type-A (left)
    - USB 3.1 Gen 2 Type-A (right)
    - USB 3.1 Gen 1 Type-A (right)

## Building coreboot

Please follow the [Star Labs build instructions](common/building.md) to build coreboot, using `config.starlabs_starfighter_rpl` as config file.

### Preliminaries

Prior to building coreboot the following files are required:
* Intel Flash Descriptor file (descriptor.bin)
* Intel Management Engine firmware (me.bin)
* ITE Embedded Controller firmware (ec.bin)

The files listed below are optional:
- Splash screen image in Windows 3.1 BMP format (Logo.bmp)

These files exist in the correct location in the StarLabsLtd/blobs repo on GitHub which is used in place of the standard 3rdparty/blobs repo.

### Build

The following commands will build a working image:


```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_starfighter_rpl
make
```

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | W25Q256.V  |
+---------------------+------------+
| Size                | 32 MiB     |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```

Please see [here](common/flashing.md) for instructions on how to flash with fwupd.
