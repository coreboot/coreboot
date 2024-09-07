# StarBook Mk V

## Specs

- CPU (full processor specs available at https://ark.intel.com)
    - Intel N200 (Alder Lake)
- EC
    - ITE IT5570E
    - Charger, using AC adapter or USB-C PD
    - Suspend / resume
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - HDMI video
    - DisplayPort video
    - USB-C DisplayPort video
- Memory
    - 1 x DDR4 SODIMM
- Networking
    - 9560 2230 WiFi / Bluetooth
    - Realtek RLT8111 1Gbps
    - Realtek RLT8125B 2.5Gbps
- Sound
    - SoundWire SN6140
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 PCIe SSD
- USB
    - 2 x USB 3.1 Gen 2 Type-A (front)
    - 1 x USB 3.1 Gen 2 Type-C (front)
    - 2 x USB 3.1 Gen 2 Type-A (back)

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
