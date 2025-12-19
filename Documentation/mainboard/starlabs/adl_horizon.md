# StarBook Mk V

## Specs

- CPU (full processor specs available at <https://ark.intel.com>)
    - Intel N305 (Alder Lake)
- EC
    - ITE IT5570E
    - Backlit keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - USB-C PD Charger
    - Suspend / resume
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - eDP 13.4-inch 2520x1680 LCD
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 32GB LPDD% on-board memory
- Networking
    - AX210 2230 WiFi / Bluetooth
- Sound
    - Realtek ALC269-VB6
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 PCIe SSD
- USB
    - 1920x1080 CCD camera
    - USB 3.1 Gen 2 (left)
    - USB 3.1 Gen 2 (right)
    - USB 3.1 Gen 2 Type-A (right)

## Building coreboot

Please follow the [Star Labs build instructions](common/building.md) to build coreboot, using `config.starlabs_adl_horizon` as config file.

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
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_adl_horizon_n
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
| Package             | WSON-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```

Please see [here](common/flashing.md) for instructions on how to flash with fwupd.
