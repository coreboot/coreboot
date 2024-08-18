# StarBook Mk V

## Specs

- CPU (full processor specs available at https://ark.intel.com)
    - Intel N200 (Alder Lake)
- EC
    - ITE IT5570E
    - USB-C PD Charger
    - Suspend / resume
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - eDP 12.5-inch 2880x1920 or 2160x1440 LCD
    - 2 x USB-C DisplayPort video
- Memory
    - 16GB on-board
- Networking
    - 9560 CNVi WiFi / Bluetooth
- Sound
    - Realtek ALC269-VB6
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 PCIe SSD
    - RTS5129 MicroSD card reader
- USB
    - 2 x 5MP CCD Camera
    - 2 x USB 3.1 Gen 2 Type-C (right)

## Building coreboot

Please follow the [Star Labs build instructions](common/building.md) to build coreboot, using `config.starlabs_starbook_adl` as config file.

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
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_byte_adl
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
